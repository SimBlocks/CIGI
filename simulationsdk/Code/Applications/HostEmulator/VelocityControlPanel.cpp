//Copyright SimBlocks LLC 2016-2026
#include "VelocityControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CVelocityControlPanel, wxPanel)
  EVT_BUTTON(ID_VELOCITY_CONTROL_SEND_PACKET, CVelocityControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CVelocityControlPanel::CVelocityControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Velocity Control Packet");

  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Articulated Part ID");
  CreateBoolControl(vbox, "Apply to Articulated Part");

  wxArrayString coordinateSystemOptions;
  coordinateSystemOptions.Add("World/Parent");
  coordinateSystemOptions.Add("Local");
  CreateChoiceControl(vbox, "Coordinate System", coordinateSystemOptions);

  CreateFloatControl(vbox, "X Linear Velocity");
  CreateFloatControl(vbox, "Y Linear Velocity");
  CreateFloatControl(vbox, "Z Linear Velocity");

  CreateFloatControl(vbox, "Roll Angular Velocity");
  CreateFloatControl(vbox, "Pitch Angular Velocity");
  CreateFloatControl(vbox, "Yaw Angular Velocity");

  wxButton* pSendButton = new wxButton(this, ID_VELOCITY_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CVelocityControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  bool bApplyToArticulatedPart = GetBool("Apply to Articulated Part");

  CHostSession* pSession = GetHostSession();

  if (bApplyToArticulatedPart)
  {
    sbio::cigi::SCigiArticulatedPartVelocityControl articulatedPartVelocityControl;
    articulatedPartVelocityControl.entityID = sbio::EntityID(GetInt("Entity ID"));
    articulatedPartVelocityControl.articulatedPartID = sbio::ArticulatedPartID(GetInt("Articulated Part ID"));
    articulatedPartVelocityControl.linearVelocity[0] = GetFloat("X Linear Velocity");
    articulatedPartVelocityControl.linearVelocity[1] = GetFloat("Y Linear Velocity");
    articulatedPartVelocityControl.linearVelocity[2] = GetFloat("Z Linear Velocity");
    articulatedPartVelocityControl.angularVelocity.roll = Degrees(GetFloat("Roll Angular Velocity"));
    articulatedPartVelocityControl.angularVelocity.pitch = Degrees(GetFloat("Pitch Angular Velocity"));
    articulatedPartVelocityControl.angularVelocity.yaw = Degrees(GetFloat("Yaw Angular Velocity"));

    pSession->SendVelocityControl(articulatedPartVelocityControl);
  }
  else
  {
    sbio::cigi::SCigiEntityVelocityControl entityVelocityControl;
    entityVelocityControl.entityID = sbio::EntityID(GetInt("Entity ID"));
    entityVelocityControl.linearVelocity[0] = GetFloat("X Linear Velocity");
    entityVelocityControl.linearVelocity[1] = GetFloat("Y Linear Velocity");
    entityVelocityControl.linearVelocity[2] = GetFloat("Z Linear Velocity");
    entityVelocityControl.angularVelocity.roll = Degrees(GetFloat("Roll Angular Velocity"));
    entityVelocityControl.angularVelocity.pitch = Degrees(GetFloat("Pitch Angular Velocity"));
    entityVelocityControl.angularVelocity.yaw = Degrees(GetFloat("Yaw Angular Velocity"));
    entityVelocityControl.coordinateSystem = ConvertCigiStringToObjectCoordinateSystem(GetChoice("Coordinate System"));

    pSession->SendVelocityControl(entityVelocityControl);
  }

  Vec3f linearVelocity;
  linearVelocity[0] = GetFloat("X Linear Velocity");
  linearVelocity[1] = GetFloat("Y Linear Velocity");
  linearVelocity[2] = GetFloat("Z Linear Velocity");

  TCigiBodyEulerRotation rotation;
  rotation.yaw = Degrees(GetFloat("Roll Angular Velocity"));
  rotation.pitch = Degrees(GetFloat("Pitch Angular Velocity"));
  rotation.roll = Degrees(GetFloat("Yaw Angular Velocity"));
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
