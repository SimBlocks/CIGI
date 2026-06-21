//Copyright SimBlocks LLC 2016-2026
#include "SymbolClonePanel.h"
#include "CigiLib/CigiLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::symbol;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolClonePanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_CLONE_SEND_PACKET, CSymbolClonePanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolClonePanel::CSymbolClonePanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Clone Packet");

  CreateIntControl(vbox, "Symbol ID");
  CreateIntControl(vbox, "Source ID");

  wxArrayString sourceTypeOptions;
  sourceTypeOptions.Add("Symbol");
  sourceTypeOptions.Add("Symbol Template");
  CreateChoiceControl(vbox, "Source Type", sourceTypeOptions);

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_CLONE_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CSymbolClonePanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolClonePanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SSymbolClone symbolClone;
  symbolClone.symbolID = SymbolID(GetInt("Symbol ID"));
  symbolClone.sourceID = SymbolID(GetInt("Source ID"));
  symbolClone.eSymbolSourceType = ConvertCigiStringToSourceType(GetChoice("Source Type"));

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolClone(symbolClone);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
