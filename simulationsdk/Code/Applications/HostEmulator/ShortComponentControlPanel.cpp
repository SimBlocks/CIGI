//Copyright SimBlocks LLC 2016-2026
#include "ShortComponentControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CShortComponentControlPanel, wxPanel)
  EVT_BUTTON(ID_SHORT_COMPONENT_CONTROL_SEND_PACKET, CShortComponentControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CShortComponentControlPanel::CShortComponentControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Short Component Control Packet");

  CreateIntControl(vbox, "Component ID");

  wxArrayString componentClassOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto componentClasses = pHost->GetScriptRuntime()->GetComponentClasses();
      for (auto itComponentClass : componentClasses)
      {
        componentClassOptions.Add(itComponentClass);
      }
    }
  }

  CreateChoiceControl(vbox, "Component Class", componentClassOptions);

  CreateIntControl(vbox, "Component State");
  CreateIntControl(vbox, "Instance ID");

  for (int n = 0; n < 2; ++n)
  {
    char buf[256];
    sprintf(buf, "Component Data %d", (n + 1));
    CreateIntControl(vbox, buf);
  }

  wxButton* pSendButton = new wxButton(this, ID_SHORT_COMPONENT_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CShortComponentControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  sbio::cigi::SCigiShortComponentControl shortComponentCtrl;

  shortComponentCtrl.componentID = sbio::cigi::CigiComponentID(GetInt("Component ID"));
  shortComponentCtrl.componentClassID = CigiComponentClassID(GetChoiceIndex("Component Class"));
  shortComponentCtrl.nComponentState = GetInt("Component State");
  shortComponentCtrl.nInstanceID = GetInt("Instance ID");

  shortComponentCtrl.componentData1 = GetInt("Component Data 1");
  shortComponentCtrl.componentData2 = GetInt("Component Data 2");

  CHostSession* pSession = GetHostSession();
  pSession->SendShortComponentControl(shortComponentCtrl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
