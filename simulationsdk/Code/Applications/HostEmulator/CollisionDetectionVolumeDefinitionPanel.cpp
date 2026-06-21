//Copyright SimBlocks LLC 2016-2026
#include "CollisionDetectionVolumeDefinitionPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CCollisionDetectionVolumeDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_COLLISION_DETECTION_VOLUME_DEFINITION_PACKET, CCollisionDetectionVolumeDefinitionPanel::OnSend)
  EVT_RADIOBOX(ID_COLLISION_DETECTION_VOLUME_TYPE_RADIO_BUTTON_EVENT, CCollisionDetectionVolumeDefinitionPanel::OnVolumeType)
wxEND_EVENT_TABLE();
// clang-format on

CCollisionDetectionVolumeDefinitionPanel::CCollisionDetectionVolumeDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Collision Detection Volume Definition Packet");

  CreateIntControl(vbox, "Volume ID");
  CreateBoolControl(vbox, "Volume Enable");

  wxArrayString volumeTypeOptions;
  volumeTypeOptions.Add("Sphere");
  volumeTypeOptions.Add("Cuboid");
  CreateRadioControl(vbox, "Volume Type", volumeTypeOptions, ID_COLLISION_DETECTION_VOLUME_TYPE_RADIO_BUTTON_EVENT);

  CreateIntControl(vbox, "Entity ID");

  CreateFloatControl(vbox, "X");
  CreateFloatControl(vbox, "Y");
  CreateFloatControl(vbox, "Z");
  CreateFloatControl(vbox, "Radius");
  CreateFloatControl(vbox, "Width");
  CreateFloatControl(vbox, "Depth");
  CreateFloatControl(vbox, "Roll");
  CreateFloatControl(vbox, "Pitch");
  CreateFloatControl(vbox, "Yaw");

  wxButton* pSendButton = new wxButton(this, ID_COLLISION_DETECTION_VOLUME_DEFINITION_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);

  NotifyRadio("Volume Type");
}

void CCollisionDetectionVolumeDefinitionPanel::OnVolumeType(wxCommandEvent& event)
{
  if (event.GetString() == "Sphere")
  {
    ChangeText("Sphere", "Sphere");
    ChangeText("Radius", "Radius");
  }
  else
  {
    ChangeText("Sphere", "Cuboid");
    ChangeText("Radius", "Height");
  }
}

void CCollisionDetectionVolumeDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  float min = 0.0;

  EVolumeType eVolumeType = ConvertCigiStringToVolumeType(GetRadio("Volume Type"));
  CHostSession* pSession = GetHostSession();

  SCollisionDetectionVolumeDefinition collisionDetectionVolumeDefinition;
  collisionDetectionVolumeDefinition.volumeID = VolumeID(GetInt("Volume ID"));
  collisionDetectionVolumeDefinition.entityID = EntityID(GetInt("Entity ID"));
  collisionDetectionVolumeDefinition.bVolumeEnabled = GetBool("Volume Enable");
  collisionDetectionVolumeDefinition.offset[0] = GetFloat("X");
  collisionDetectionVolumeDefinition.offset[1] = GetFloat("Y");
  collisionDetectionVolumeDefinition.offset[2] = GetFloat("Z");

  HostCigiErrorEventArgs args;

  if (eVolumeType == EVolumeType::SPHERE)
  {
    SCollisionDetectionSphereDefinition sphereDefinition;
    sphereDefinition.entityID = collisionDetectionVolumeDefinition.entityID;
    sphereDefinition.volumeID = collisionDetectionVolumeDefinition.volumeID;
    sphereDefinition.bVolumeEnabled = collisionDetectionVolumeDefinition.bVolumeEnabled;
    sphereDefinition.offset = collisionDetectionVolumeDefinition.offset;
    sphereDefinition.volumeID = collisionDetectionVolumeDefinition.volumeID;
    sphereDefinition.fRadius = GetFloat("Radius");

    if (sphereDefinition.fRadius < min)
    {
      args.sError = "Radius: Enter number greater than 0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Radius: Enter number greater than 0");
      sphereDefinition.fRadius = ResetFloat("Radius");
    }

    pSession->SendCollisionDetectionSphereVolume(sphereDefinition);
  }
  else
  {
    SCollisionDetectionCuboidDefinition cuboidDefinition;
    cuboidDefinition.entityID = collisionDetectionVolumeDefinition.entityID;
    cuboidDefinition.volumeID = collisionDetectionVolumeDefinition.volumeID;
    cuboidDefinition.bVolumeEnabled = collisionDetectionVolumeDefinition.bVolumeEnabled;
    cuboidDefinition.offset = collisionDetectionVolumeDefinition.offset;
    cuboidDefinition.fWidth = GetFloat("Width");
    cuboidDefinition.fDepth = GetFloat("Depth");
    cuboidDefinition.fHeight = GetFloat("Radius");

    if (cuboidDefinition.fHeight < min)
    {
      args.sError = "Height: Enter number greater than 0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Height: Enter number greater than 0");
      cuboidDefinition.fHeight = ResetFloat("Radius");
    }

    if (cuboidDefinition.fWidth < min)
    {
      args.sError = "Width: Enter number greater than 0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Width: Enter number greater than 0");
      cuboidDefinition.fHeight = ResetFloat("Width");
    }

    if (cuboidDefinition.fDepth < min)
    {
      args.sError = "Depth: Enter number greater than 0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Depth: Enter number greater than 0");
      cuboidDefinition.fHeight = ResetFloat("Depth");
    }

    float roll = GetFloat("Roll");
    if (!Degrees180::CheckValid(roll))
    {
      args.sError = "Roll: Enter number between -180.0 and 180.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Roll: Enter number between -180.0 and 180.0");
      roll = ResetFloat("Roll");
    }
    cuboidDefinition.rotation.roll = Degrees(roll);

    float pitch = GetFloat("Pitch");
    if (!Degrees90::CheckValid(pitch))
    {
      args.sError = "Pitch: Enter number between -180.0 and 180.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Pitch: Enter number between -180.0 and 180.0");
      pitch = ResetFloat("Pitch");
    }
    cuboidDefinition.rotation.pitch = Degrees(pitch);

    float yaw = GetFloat("Yaw");
    if (!Degrees::CheckValid(yaw))
    {
      args.sError = "Yaw: Enter number between 0.0 and 360.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Yaw: Enter number between 0.0 and 360.0");
      yaw = ResetFloat("Yaw");
    }
    cuboidDefinition.rotation.yaw = Degrees(yaw);

    pSession->SendCollisionDetectionCuboidVolume(cuboidDefinition);
  }

  delete wxLog::SetActiveTarget(NULL);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
