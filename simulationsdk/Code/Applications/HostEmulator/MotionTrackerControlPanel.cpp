//Copyright SimBlocks LLC 2016-2026
#include "MotionTrackerControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CMotionTrackerControlPanel, wxPanel)
  EVT_BUTTON(ID_MOTION_TRACKER_CONTROL_SEND_PACKET, CMotionTrackerControlPanel::OnSend)
  EVT_RADIOBOX(ID_MOTION_TRACKER_CONTROL_RADIO_BUTTON_EVENT, CMotionTrackerControlPanel::OnRadio)
wxEND_EVENT_TABLE();
// clang-format on

CMotionTrackerControlPanel::CMotionTrackerControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Motion Tracker Control Packet");

  CreateIntControl(vbox, "Tracker ID");
  CreateBoolControl(vbox, "Tracker Enable");
  CreateBoolControl(vbox, "Boresight Enable");
  CreateBoolControl(vbox, "X Enable");
  CreateBoolControl(vbox, "Y Enable");
  CreateBoolControl(vbox, "Z Enable");
  CreateBoolControl(vbox, "Roll Enable");
  CreateBoolControl(vbox, "Pitch Enable");
  CreateBoolControl(vbox, "Yaw Enable");

  wxArrayString idOptions;
  idOptions.Add("View");
  idOptions.Add("View Group");
  CreateRadioControl(vbox, "ID Type", idOptions, ID_MOTION_TRACKER_CONTROL_RADIO_BUTTON_EVENT);

  CreateIntControl(vbox, "ID");

  wxButton* pSendButton = new wxButton(this, ID_MOTION_TRACKER_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);

  NotifyRadio("ID Type");
}

void CMotionTrackerControlPanel::OnRadio(wxCommandEvent& event)
{
  if (event.GetString() == "View ID")
  {
    ChangeText("ID", "View");
  }
  else
  {
    ChangeText("ID", "View Group");
  }
}

void CMotionTrackerControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  EViewType viewType = ConvertCigiStringToViewType(GetRadio("ID Type"));

  if (viewType == EViewType::VIEW)
  {
    SMotionTrackerViewControl motionTrackerControl;
    motionTrackerControl.motionTrackerID = MotionTrackerID(GetInt("Tracker ID"));
    motionTrackerControl.bEnable = GetBool("Tracker Enable");
    motionTrackerControl.bBoresightEnable = GetBool("Boresight Enable");
    motionTrackerControl.bXEnable = GetBool("X Enable");
    motionTrackerControl.bYEnable = GetBool("Y Enable");
    motionTrackerControl.bZEnable = GetBool("Z Enable");
    motionTrackerControl.bRollEnable = GetBool("Roll Enable");
    motionTrackerControl.bPitchEnable = GetBool("Pitch Enable");
    motionTrackerControl.bYawEnable = GetBool("Yaw Enable");

    motionTrackerControl.viewID = ViewID(GetInt("ID"));

    CHostSession* pSession = GetHostSession();
    pSession->SendMotionTrackerViewControl(motionTrackerControl);
  }
  else if (viewType == EViewType::VIEW_GROUP)
  {
    SMotionTrackerViewGroupControl motionTrackerControl;
    motionTrackerControl.motionTrackerID = MotionTrackerID(GetInt("Tracker ID"));
    motionTrackerControl.bEnable = GetBool("Tracker Enable");
    motionTrackerControl.bBoresightEnable = GetBool("Boresight Enable");
    motionTrackerControl.bXEnable = GetBool("X Enable");
    motionTrackerControl.bYEnable = GetBool("Y Enable");
    motionTrackerControl.bZEnable = GetBool("Z Enable");
    motionTrackerControl.bRollEnable = GetBool("Roll Enable");
    motionTrackerControl.bPitchEnable = GetBool("Pitch Enable");
    motionTrackerControl.bYawEnable = GetBool("Yaw Enable");

    motionTrackerControl.viewGroupID = ViewGroupID(GetInt("ID"));

    CHostSession* pSession = GetHostSession();
    pSession->SendMotionTrackerViewGroupControl(motionTrackerControl);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
