//Copyright SimBlocks LLC 2016-2026
#include "ArticulatedPartControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypes.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "HostEmulatorGuiFrame.h"
#include "MathLib/MathTypes.h"

using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CArticulatedPartControlPanel, wxPanel)
  EVT_BUTTON(ID_ARTICULATED_PART_CONTROL_SEND_PACKET, CArticulatedPartControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CArticulatedPartControlPanel::CArticulatedPartControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Articulated Part Control Packet");

  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Articulated Part ID");
  CreateBoolControl(vbox, "Articulated Part Enable");
  CreateBoolControl(vbox, "X Offset Enable");
  CreateBoolControl(vbox, "Y Offset Enable");
  CreateBoolControl(vbox, "Z Offset Enable");
  CreateBoolControl(vbox, "Roll Enable");
  CreateBoolControl(vbox, "Pitch Enable");
  CreateBoolControl(vbox, "Yaw Enable");
  CreateFloatControl(vbox, "X Offset");
  CreateFloatControl(vbox, "Y Offset");
  CreateFloatControl(vbox, "Z Offset");
  CreateFloatControl(vbox, "Roll");
  CreateFloatControl(vbox, "Pitch");
  CreateFloatControl(vbox, "Yaw");

  wxButton* pSendButton = new wxButton(this, ID_ARTICULATED_PART_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CArticulatedPartControlPanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CArticulatedPartControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  float min = -180.0, max = 180.0;

  sbio::cigi::SCigiArticulatedPart articulatedPartControl;
  articulatedPartControl.articulatedPartID = sbio::ArticulatedPartID(GetInt("Articulated Part ID"));
  articulatedPartControl.entityID = sbio::EntityID(GetInt("Entity ID"));
  articulatedPartControl.bEnabled = GetBool("Articulated Part Enable");
  articulatedPartControl.bOffsetEnabled[0] = GetBool("X Offset Enable");
  articulatedPartControl.bOffsetEnabled[1] = GetBool("Y Offset Enable");
  articulatedPartControl.bOffsetEnabled[2] = GetBool("Z Offset Enable");
  articulatedPartControl.bRollEnabled = GetBool("Roll Enable");
  articulatedPartControl.bPitchEnabled = GetBool("Pitch Enable");
  articulatedPartControl.bYawEnabled = GetBool("Yaw Enable");
  articulatedPartControl.offset[0] = GetFloat("X Offset");
  articulatedPartControl.offset[1] = GetFloat("Y Offset");
  articulatedPartControl.offset[2] = GetFloat("Z Offset");
  articulatedPartControl.rotation.roll = Degrees(GetFloat("Roll"));
  articulatedPartControl.rotation.pitch = Degrees(GetFloat("Pitch"));
  articulatedPartControl.rotation.yaw = Degrees(GetFloat("Yaw"));

  HostCigiErrorEventArgs args;

  if (articulatedPartControl.rotation.roll.Value() < min || articulatedPartControl.rotation.roll.Value() > max)
  {
    args.sError = "Roll: Enter number between -180.0 and 180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Roll: Enter number between -180.0 and 180.0");
    articulatedPartControl.rotation.roll = Degrees(ResetFloat("Roll"));
  }
  if (articulatedPartControl.rotation.pitch.Value() < min || articulatedPartControl.rotation.pitch.Value() > max)
  {
    args.sError = "Pitch: Enter number between -180.0 and 180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Pitch: Enter number between -180.0 and 180.0");
    articulatedPartControl.rotation.pitch = Degrees(ResetFloat("Pitch"));
  }
  if (articulatedPartControl.rotation.yaw.Value() < min || articulatedPartControl.rotation.yaw.Value() > max)
  {
    args.sError = "Yaw: Enter number between -180.0 and 180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Yaw: Enter number between -180.0 and 180.0");
    articulatedPartControl.rotation.yaw = Degrees(ResetFloat("Yaw"));
  }

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendArticulatedPartControl(articulatedPartControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
