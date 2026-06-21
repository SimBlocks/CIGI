//Copyright SimBlocks LLC 2016-2026
#include "IGControlPacketPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CIGControlPacketPanel, wxPanel)
  EVT_BUTTON(ID_IG_CONTROL_SEND_PACKET, CIGControlPacketPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

CIGControlPacketPanel::CIGControlPacketPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "IG Control Packet");

  DatabaseID defaultDatabaseID = DatabaseID(0);
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr)
    {
      defaultDatabaseID = pHost->GetHostSetupOptions().defaultDatabaseID;
    }
  }

  CreateIntControl(vbox, "Database Number", wxString::Format("%d", defaultDatabaseID.Value()).ToStdString());
  CreateBoolControl(vbox, "Entity Type Substitution Enable");

  wxArrayString modeOptions;
  modeOptions.Add("Reset");
  modeOptions.Add("Operate");
  modeOptions.Add("Debug");
  CreateChoiceControl(vbox, "IG Mode", modeOptions);

  CreateBoolControl(vbox, "Smoothing Enable");

  wxButton* pSendButton = new wxButton(this, ID_IG_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);

  //Connect(IG_IG_CONTROL_SEND_PACKET, EVT_BUTTON, wxEventHandler)

  //Bind(ID_IG_CONTROL_SEND_PACKET, CIGControlPacketPanel::OnSend);
}

void CIGControlPacketPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  CigiDatabaseNumber nDatabaseNumber = CigiDatabaseNumber(GetInt("Database Number"));
  bool bEntityTypeSubstitutionEnabled = GetBool("Entity Type Substitution Enable");
  EIGMode eIGMode = ConvertCigiStringToIGMode(GetChoice("IG Mode"));
  bool bSmoothingEnabled = GetBool("Smoothing Enable");

  CHostSession* pHostSession = GetHostSession();
  if (pHostSession == nullptr)
  {
    return;
  }

  // Only allow sending IG Control packets if connected to the IG.
  if (pHostSession->IsConnected())
  {
    pHostSession->SetIGControl(nDatabaseNumber, bEntityTypeSubstitutionEnabled, eIGMode, bSmoothingEnabled);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
