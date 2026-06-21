//Copyright SimBlocks LLC 2016-2026
#include "ViewControlPanel.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"

using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CViewControlPacketPanel, wxPanel)
  EVT_BUTTON(ID_VIEW_CONTROL_SEND_PACKET, CViewControlPacketPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CViewControlPacketPanel::CViewControlPacketPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "View Control Packet");

  CreateIntControl(vbox, "Group ID");

  CreateBoolControl(vbox, "X Offset Enable");
  CreateBoolControl(vbox, "Y Offset Enable");
  CreateBoolControl(vbox, "Z Offset Enable");

  CreateBoolControl(vbox, "Roll Enable");
  CreateBoolControl(vbox, "Pitch Enable");
  CreateBoolControl(vbox, "Yaw Enable");

  CreateIntControl(vbox, "View ID");
  CreateIntControl(vbox, "Entity ID");

  CreateFloatControl(vbox, "X Offset");
  CreateFloatControl(vbox, "Y Offset");
  CreateFloatControl(vbox, "Z Offset");

  CreateFloatControl(vbox, "Roll");
  CreateFloatControl(vbox, "Pitch");
  CreateFloatControl(vbox, "Yaw");

  wxButton* pSendButton = new wxButton(this, ID_VIEW_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CViewControlPacketPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  sbio::cigi::SCigiViewControl viewControl;

  viewControl.viewGroupID = sbio::ViewGroupID(GetInt("Group ID"));
  viewControl.offsetEnabled[0] = GetBool("X Offset Enable");
  viewControl.offsetEnabled[1] = GetBool("Y Offset Enable");
  viewControl.offsetEnabled[2] = GetBool("Z Offset Enable");

  viewControl.bRollEnabled = GetBool("Roll Enable");
  viewControl.bPitchEnabled = GetBool("Pitch Enable");
  viewControl.bYawEnabled = GetBool("Yaw Enable");

  viewControl.viewID = sbio::ViewID(GetInt("View ID"));
  viewControl.entityID = sbio::EntityID(GetInt("Entity ID"));

  viewControl.offset[0] = GetFloat("X Offset");
  viewControl.offset[1] = GetFloat("Y Offset");
  viewControl.offset[2] = GetFloat("Z Offset");

  viewControl.rotation.roll = Degrees(GetFloat("Roll"));
  viewControl.rotation.pitch = Degrees(GetFloat("Pitch"));
  viewControl.rotation.yaw = Degrees(GetFloat("Yaw"));

  CHostSession* pSession = GetHostSession();
  pSession->SendViewControl(viewControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
