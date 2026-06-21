//Copyright SimBlocks LLC 2016-2026
#include "AccelerationControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "EntityLib/EntityLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CAccelerationControlPanel, wxPanel)
  EVT_BUTTON(ID_ACCELERATION_CONTROL_SEND_PACKET, CAccelerationControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CAccelerationControlPanel::CAccelerationControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Acceleration Control Packet");

  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Articulated Part ID");
  CreateBoolControl(vbox, "Apply to Articulated Part");

  wxArrayString coordinateSystemOptions;
  coordinateSystemOptions.Add("World/Parent");
  coordinateSystemOptions.Add("Local");
  CreateChoiceControl(vbox, "Coordinate System", coordinateSystemOptions);

  CreateFloatControl(vbox, "X Linear Acceleration");
  CreateFloatControl(vbox, "Y Linear Acceleration");
  CreateFloatControl(vbox, "Z Linear Acceleration");

  CreateFloatControl(vbox, "Roll Angular Acceleration");
  CreateFloatControl(vbox, "Pitch Angular Acceleration");
  CreateFloatControl(vbox, "Yaw Angular Acceleration");

  wxButton* pSendButton = new wxButton(this, ID_ACCELERATION_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CAccelerationControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  bool bApplyToArticulatedPart = GetBool("Apply to Articulated Part");

  CHostSession* pSession = GetHostSession();

  if (bApplyToArticulatedPart)
  {
    SCigiArticulatedPartAcceleration accelerationControl;
    accelerationControl.entityID = EntityID(GetInt("Entity ID"));
    accelerationControl.articulatedPartID = ArticulatedPartID(GetInt("Articulated Part ID"));
    accelerationControl.eCoordinateSystem = ConvertCigiStringToObjectCoordinateSystem(GetChoice("Coordinate System"));

    accelerationControl.linearAcceleration[0] = GetFloat("X Linear Acceleration");
    accelerationControl.linearAcceleration[1] = GetFloat("Y Linear Acceleration");
    accelerationControl.linearAcceleration[2] = GetFloat("Z Linear Acceleration");

    accelerationControl.angularAcceleration.roll = Degrees(GetFloat("Roll Angular Acceleration"));
    accelerationControl.angularAcceleration.pitch = Degrees(GetFloat("Pitch Angular Acceleration"));
    accelerationControl.angularAcceleration.yaw = Degrees(GetFloat("Yaw Angular Acceleration"));

    pSession->SendArticulatedPartAccelerationControl(accelerationControl);
  }
  else
  {
    SCigiEntityAcceleration accelerationControl;
    accelerationControl.entityID = EntityID(GetInt("Entity ID"));
    accelerationControl.eCoordinateSystem = ConvertCigiStringToObjectCoordinateSystem(GetChoice("Coordinate System"));

    accelerationControl.linearAcceleration[0] = GetFloat("X Linear Acceleration");
    accelerationControl.linearAcceleration[1] = GetFloat("Y Linear Acceleration");
    accelerationControl.linearAcceleration[2] = GetFloat("Z Linear Acceleration");

    accelerationControl.angularAcceleration.roll = Degrees(GetFloat("Roll Angular Acceleration"));
    accelerationControl.angularAcceleration.pitch = Degrees(GetFloat("Pitch Angular Acceleration"));
    accelerationControl.angularAcceleration.yaw = Degrees(GetFloat("Yaw Angular Acceleration"));

    pSession->SendEntityAccelerationControl(accelerationControl);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
