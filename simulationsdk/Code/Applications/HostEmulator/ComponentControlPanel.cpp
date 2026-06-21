//Copyright SimBlocks LLC 2016-2026
#include "ComponentControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CComponentControlPanel, wxPanel)
  EVT_BUTTON(ID_COMPONENT_CONTROL_SEND_PACKET, CComponentControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CComponentControlPanel::CComponentControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Component Control Packet");

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

  for (int n = 0; n < 6; ++n)
  {
    char buf[256];
    sprintf(buf, "Component Data %d", (n + 1));
    CreateIntControl(vbox, buf);
  }

  wxButton* pSendButton = new wxButton(this, ID_COMPONENT_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CComponentControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  sbio::cigi::SCigiComponentControl componentControl;
  componentControl.key.componentID = sbio::cigi::CigiComponentID(GetInt("Component ID"));
  componentControl.key.componentClassID = CigiComponentClassID(GetChoiceIndex("Component Class"));
  componentControl.state.nComponentState = GetInt("Component State");
  componentControl.key.nInstanceID = GetInt("Instance ID");

  uint16_t componentData[6];

  for (int n = 0; n < 6; ++n)
  {
    char buf[256];
    sprintf(buf, "Component Data %d", (n + 1));
    int data = GetInt(buf);

    componentData[n] = data;
    componentControl.state.componentData[n] = data;
  }

  CHostSession* pHostSession = GetHostSession();
  pHostSession->SendComponentControl(componentControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
