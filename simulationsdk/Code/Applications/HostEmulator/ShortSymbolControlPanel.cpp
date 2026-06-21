//Copyright SimBlocks LLC 2016-2026
#include "ShortSymbolControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio::math;
using namespace std;
using namespace sbio::symbol;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CShortSymbolControlPanel, wxPanel)
  EVT_BUTTON(ID_SHORT_SYMBOL_CONTROL_SEND_PACKET, CShortSymbolControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CShortSymbolControlPanel::CShortSymbolControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Short Symbol Control Packet");

  CreateIntControl(vbox, "Symbol ID");

  wxArrayString attributeSelect1Options;
  attributeSelect1Options.Add("None");
  attributeSelect1Options.Add("Surface ID");
  attributeSelect1Options.Add("Parent Symbol ID");
  attributeSelect1Options.Add("Layer");
  attributeSelect1Options.Add("Flash Duty Cycle Percentage");
  attributeSelect1Options.Add("Flash Period");
  attributeSelect1Options.Add("Position U");
  attributeSelect1Options.Add("Position V");
  attributeSelect1Options.Add("Rotation");
  attributeSelect1Options.Add("Color");
  attributeSelect1Options.Add("Scale U");
  attributeSelect1Options.Add("Scale V");
  CreateChoiceControl(vbox, "Attribute Select 1", attributeSelect1Options);

  wxArrayString attributeSelect2Options;
  attributeSelect2Options.Add("None");
  attributeSelect2Options.Add("Surface ID");
  attributeSelect2Options.Add("Parent Symbol ID");
  attributeSelect2Options.Add("Layer");
  attributeSelect2Options.Add("Flash Duty Cycle Percentage");
  attributeSelect2Options.Add("Flash Period");
  attributeSelect2Options.Add("Position U");
  attributeSelect2Options.Add("Position V");
  attributeSelect2Options.Add("Rotation");
  attributeSelect2Options.Add("Color");
  attributeSelect2Options.Add("Scale U");
  attributeSelect2Options.Add("Scale V");
  CreateChoiceControl(vbox, "Attribute Select 2", attributeSelect2Options);

  CreateTextControl(vbox, "Attribute Value 1");
  CreateTextControl(vbox, "Attribute Value 2");

  wxArrayString symbolStateOptions;
  symbolStateOptions.Add("Hidden");
  symbolStateOptions.Add("Visible");
  symbolStateOptions.Add("Destroyed");
  CreateChoiceControl(vbox, "Symbol State", symbolStateOptions);

  wxArrayString attachStateOptions;
  attachStateOptions.Add("Detach");
  attachStateOptions.Add("Attach");
  CreateChoiceControl(vbox, "Attach State", attachStateOptions);

  wxArrayString flashControlOptions;
  flashControlOptions.Add("Continue");
  flashControlOptions.Add("Reset");
  CreateChoiceControl(vbox, "Flash Control", flashControlOptions);

  CreateBoolControl(vbox, "Inherit Color");

  wxButton* pSendButton = new wxButton(this, ID_SHORT_SYMBOL_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CShortSymbolControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SShortSymbolControl shortSymbolControl;
  shortSymbolControl.symbolID = SymbolID(GetInt("Symbol ID"));
  shortSymbolControl.eAttributeSelect1 = ConvertCigiStringToAttributeSelect(GetChoice("Attribute Select 1"));
  shortSymbolControl.eAttributeSelect2 = ConvertCigiStringToAttributeSelect(GetChoice("Attribute Select 2"));

  shortSymbolControl.eSymbolState = ConvertCigiStringToSymbolState(GetChoice("Symbol State"));
  shortSymbolControl.eAttachState = ConvertCigiStringToAttachState(GetChoice("Attach State"));
  shortSymbolControl.eFlashControl = ConvertCigiStringToFlashControl(GetChoice("Flash Control"));
  shortSymbolControl.bInheritColor = GetBool("Inherit Color");

  string sAttributeValue1 = GetString("Attribute Value 1");
  string sAttributeValue2 = GetString("Attribute Value 2");

  if (sAttributeValue1.find('.') != std::string::npos)
  {
    shortSymbolControl.attributeValue1.f = stof(sAttributeValue1.c_str());
  }
  else if (!sAttributeValue1.empty())
  {
    shortSymbolControl.attributeValue1.n = stoi(sAttributeValue1.c_str());
  }

  if (sAttributeValue2.find('.') != std::string::npos)
  {
    shortSymbolControl.attributeValue2.f = stof(sAttributeValue2.c_str());
  }
  else if (!sAttributeValue2.empty())
  {
    shortSymbolControl.attributeValue2.n = stoi(sAttributeValue2.c_str());
  }

  CHostSession* pSession = GetHostSession();
  pSession->SendShortSymbolControl(shortSymbolControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
