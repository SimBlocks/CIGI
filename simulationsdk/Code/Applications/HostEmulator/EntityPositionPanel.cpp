//Copyright SimBlocks LLC 2016-2026
#include "EntityPositionPanel.h"
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
wxBEGIN_EVENT_TABLE(CEntityPositionPanel, wxPanel)
  EVT_BUTTON(ID_ENTITY_POSITION_SEND_PACKET, CEntityPositionPanel::OnSend)
  EVT_COMBOBOX(ID_ENTITY_POSITION_ATTACH_STATE, CEntityPositionPanel::OnAttachStateSelection)
wxEND_EVENT_TABLE();
// clang-format on

CEntityPositionPanel::CEntityPositionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Entity Position Packet");

  wxArrayString attachStateOptions;
  attachStateOptions.Add("Detach");
  attachStateOptions.Add("Attach");
  CreateChoiceControl(vbox, ID_ENTITY_POSITION_ATTACH_STATE, "Attach State", attachStateOptions);

  wxArrayString groundOceanSystemOptions;
  groundOceanSystemOptions.Add("No Clamp");
  groundOceanSystemOptions.Add("Non-Conformal");
  groundOceanSystemOptions.Add("Conformal");
  CreateChoiceControl(vbox, "Ground/Ocean Clamp", groundOceanSystemOptions);

  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Parent ID");

  CreateFloatControl(vbox, "Roll");
  CreateFloatControl(vbox, "Pitch");
  CreateFloatControl(vbox, "Yaw");

  CreateFloatControl(vbox, "Latitude");
  CreateFloatControl(vbox, "Longitude");
  CreateFloatControl(vbox, "Altitude");

  wxButton* pSendButton = new wxButton(this, ID_ENTITY_POSITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CEntityPositionPanel::OnAttachStateSelection(wxCommandEvent& event)
{
  string sSelection = GetChoice("Attach State");

  if (sSelection == "Attach")
  {
    ChangeText("Latitude", "X Offset");
    ChangeText("Longitude", "Y Offset");
    ChangeText("Altitude", "Z Offset");
  }
  else if (sSelection == "Detach")
  {
    ChangeText("Latitude", "Latitude");
    ChangeText("Longitude", "Longitude");
    ChangeText("Altitude", "Altitude");
  }
}

void CEntityPositionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  EAttachState eAttachState = ConvertCigiStringToAttachState(GetChoice("Attach State"));

  CHostSession* pSession = GetHostSession();

  if (eAttachState == EAttachState::ATTACH)
  {
    sbio::cigi::SChildEntityPosition child;
    child.bAttached = true;
    child.entityID = sbio::EntityID(GetInt("Entity ID"));
    child.parentID = sbio::EntityID(GetInt("Parent ID"));
    child.offset[0] = GetFloat("Latitude");//X Offset
    child.offset[1] = GetFloat("Longitude");//Y Offset
    child.offset[2] = GetFloat("Altitude");//Z Offset

    HostCigiErrorEventArgs args;

    Degrees roll = Degrees(GetFloat("Roll"));

    if (!Degrees180::CheckValid(roll.Value()))
    {
      args.sError = "Roll: Enter number between -180.0 and 180.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Roll: Enter number between -180.0 and 180.0");
      roll = Degrees(ResetFloat("Roll"));
    }
    child.rotation.roll = roll;

    Degrees pitch = Degrees(GetFloat("Pitch"));
    if (!Degrees90::CheckValid(pitch.Value()))
    {
      args.sError = "Pitch: Enter number between -90.0 and 90.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Pitch: Enter number between -90.0 and 90.0");
      pitch = Degrees(ResetFloat("Pitch"));
    }
    child.rotation.pitch = pitch;

    Degrees yaw = Degrees(GetFloat("Yaw"));
    if (!Degrees::CheckValid(yaw.Value()))
    {
      args.sError = "Yaw: Enter number between 0.0 and 360.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Yaw: Enter number between 0.0 and 360.0");
      yaw = Degrees(ResetFloat("Yaw"));
    }
    child.rotation.yaw = yaw;

    delete wxLog::SetActiveTarget(NULL);

    pSession->SendChildEntityPosition(child);
  }
  else
  {
    sbio::cigi::STopLevelEntityPosition topLevel;
    topLevel.bAttached = false;
    topLevel.eClamp = ConvertCigiStringToEClamp(GetChoice("Ground/Ocean Clamp"));
    topLevel.entityID = sbio::EntityID(GetInt("Entity ID"));
    topLevel.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(GetDouble("Altitude"));

    HostCigiErrorEventArgs args;

    Degrees roll = Degrees(GetFloat("Roll"));
    if (!Degrees180::CheckValid(roll.Value()))
    {
      args.sError = "Roll: Enter number between -180.0 and 180.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Roll: Enter number between -180.0 and 180.0");
      roll = Degrees(ResetFloat("Roll"));
    }
    topLevel.rotation.roll = roll;

    Degrees pitch = Degrees(GetFloat("Pitch"));
    if (!Degrees90::CheckValid(pitch.Value()))
    {
      args.sError = "Pitch: Enter number between -90.0 and 90.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Pitch: Enter number between -90.0 and 90.0");
      pitch = Degrees(ResetFloat("Pitch"));
    }
    topLevel.rotation.pitch = pitch;

    Degrees yaw = Degrees(GetFloat("Yaw"));
    if (!Degrees::CheckValid(yaw.Value()))
    {
      args.sError = "Yaw: Enter number between 0.0 and 360.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Yaw: Enter number between 0.0 and 360.0");
      yaw = Degrees(ResetFloat("Yaw"));
    }
    topLevel.rotation.yaw = yaw;

    double latitude = GetDouble("Latitude");
    if (!Latitude::CheckValid(latitude))
    {
      args.sError = "Latitude: Enter number between -90.0 and 90.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Latitude: Enter number between -90.0 and 90.0");
      latitude = ResetDouble("Latitude");
    }
    topLevel.geodeticCoordinates.latitude = Latitude(latitude);

    double longitude = GetDouble("Longitude");
    if (!Longitude::CheckValid(longitude))
    {
      args.sError = "Longtitude: Enter number between -180.0 and 180.0";
      Event::Raise<HostCigiEvent>(args);

      wxLogError("Longtitude: Enter number between -180.0 and 180.0");
      longitude = ResetDouble("Longitude");
    }
    topLevel.geodeticCoordinates.longitude = Longitude(longitude);

    delete wxLog::SetActiveTarget(NULL);

    pSession->SendTopLevelEntityPosition(topLevel);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
