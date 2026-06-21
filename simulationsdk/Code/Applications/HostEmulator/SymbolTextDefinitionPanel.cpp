//Copyright SimBlocks LLC 2016-2026
#include "SymbolTextDefinitionPanel.h"
#include "CigiLib/CigiLib.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "HostCigiLib/Host.h"
#include "MathLib/MathTypes.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::symbol;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolTextDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_TEXT_DEFINITION_SEND_PACKET, CSymbolTextDefinitionPanel::OnSend)
  EVT_COMBOBOX(ID_SYMBOL_TEXT_DEFINITION_FONT_CATEGORY, CSymbolTextDefinitionPanel::OnFontCategory)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolTextDefinitionPanel::CSymbolTextDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Text Definition Packet");

  wxArrayString alignmentOptions;
  alignmentOptions.Add("Top Left");
  alignmentOptions.Add("Top Center");
  alignmentOptions.Add("Top Right");
  alignmentOptions.Add("Center Left");
  alignmentOptions.Add("Center");
  alignmentOptions.Add("Center Right");
  alignmentOptions.Add("Bottom Left");
  alignmentOptions.Add("Bottom Center");
  alignmentOptions.Add("Bottom Right");
  CreateChoiceControl(vbox, "Alignment", alignmentOptions);

  wxArrayString orientationOptions;
  orientationOptions.Add("Left to Right");
  orientationOptions.Add("Top to Bottom");
  orientationOptions.Add("Right to Left");
  orientationOptions.Add("Bottom to Top");
  CreateChoiceControl(vbox, "Orientation", orientationOptions);

  wxArrayString fontCategoryOptions;
  wxArrayString fontNameOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto fontCategories = pHost->GetScriptRuntime()->GetFontCategories();
      for (auto itFontCategory : fontCategories)
      {
        fontCategoryOptions.Add(itFontCategory);
      }

      auto fontNames = pHost->GetScriptRuntime()->GetFontNames("");
      for (auto itFontName : fontNames)
      {
        fontNameOptions.Add(itFontName);
      }
    }
  }

  CreateChoiceControl(vbox, "Font Category", fontCategoryOptions, ID_SYMBOL_TEXT_DEFINITION_FONT_CATEGORY);
  CreateChoiceControl(vbox, "Font Name", fontNameOptions);

  CreateIntControl(vbox, "Symbol ID");
  CreateFloatControl(vbox, "Font Size");
  CreateTextAreaControl(vbox, "Text");

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_TEXT_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolTextDefinitionPanel::OnFontCategory(wxCommandEvent& event)
{
  wxArrayString fontNameOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto fontNames = pHost->GetScriptRuntime()->GetFontNames(event.GetString().ToStdString());
      for (auto itFontName : fontNames)
      {
        fontNameOptions.Add(itFontName);
      }
    }
  }

  UpdateChoiceControl("Font Name", fontNameOptions);
}

void CSymbolTextDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  CHost* pHost = g_pHostEmulatorGuiApp != nullptr ? g_pHostEmulatorGuiApp->GetHost() : nullptr;
  if (pHost == nullptr || pHost->GetScriptRuntime() == nullptr)
  {
    return;
  }

  SSymbolTextDefinition symbolTextDef;
  symbolTextDef.eTextAlignment = ConvertCigiStringToAlignment(GetChoice("Alignment"));
  symbolTextDef.eTextOrientation = ConvertCigiStringToOrientation(GetChoice("Orientation"));

  string sFontCategory = GetChoice("Font Category");
  FontID fontID = pHost->GetScriptRuntime()->GetFontID(sFontCategory, GetChoice("Font Name"));

  /*if (!FontID::CheckValid(fontID.Value()))
  {
    HostCigiErrorEventArgs args;
    args.sError = "Font ID must be between 0 and 255.";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox(wxT("Font ID must be between 0 and 255."));
    fontID = FontID(0);
  }*/
  symbolTextDef.fontID = fontID;
  symbolTextDef.fFontSize = GetFloat("Font Size");
  symbolTextDef.symbolID = SymbolID(GetInt("Symbol ID"));

  const int MAX_SYMBOL_TEXT_LENGTH = 235;

  string sText = GetString("Text");
  if (sText.length() >= MAX_SYMBOL_TEXT_LENGTH)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Text is too long. Length exceeds 235 bytes.";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox(wxT("Text is too long. Length exceeds 235 bytes."));
    sText = sText.substr(0, MAX_SYMBOL_TEXT_LENGTH);
  }
  symbolTextDef.sText = sText;

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolTextDefinition(symbolTextDef);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
