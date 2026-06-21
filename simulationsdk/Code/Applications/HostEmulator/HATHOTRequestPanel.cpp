//Copyright SimBlocks LLC 2016-2026
#include "HATHOTRequestPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
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
wxBEGIN_EVENT_TABLE(CHATHOTRequestPanel, wxPanel)
  EVT_BUTTON(ID_HAT_HOT_REQUEST_SEND_PACKET, CHATHOTRequestPanel::OnSend)
  EVT_RADIOBOX(ID_HAT_HOT_ID_RADIO_BUTTON_EVENT, CHATHOTRequestPanel::OnIDRadio)
  EVT_RADIOBOX(ID_HAT_HOT_COORDINATE_SYSTEM_RADIO_BUTTON_EVENT, CHATHOTRequestPanel::OnCoordinateSystemRadio)
wxEND_EVENT_TABLE();
// clang-format on

CHATHOTRequestPanel::CHATHOTRequestPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "HAT HOT Request Packet");

  wxArrayString requestTypeOptions;
  requestTypeOptions.Add("HAT");
  requestTypeOptions.Add("HOT");
  requestTypeOptions.Add("Extended");
  CreateRadioControl(vbox, "Request Type", requestTypeOptions, ID_HAT_HOT_ID_RADIO_BUTTON_EVENT);

  CreateIntControl(vbox, "ID");
  CreateIntControl(vbox, "Entity ID");

  wxArrayString coordinateSystemOptions;
  coordinateSystemOptions.Add("Geodetic");
  coordinateSystemOptions.Add("Entity");
  CreateRadioControl(vbox, "Coordinate System", coordinateSystemOptions, ID_HAT_HOT_COORDINATE_SYSTEM_RADIO_BUTTON_EVENT);

  CreateIntControl(vbox, "Update Period");
  CreateFloatControl(vbox, "Latitude");
  CreateFloatControl(vbox, "Longitude");
  CreateFloatControl(vbox, "Altitude");

  wxButton* pSendButton = new wxButton(this, ID_HAT_HOT_REQUEST_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);

  NotifyRadio("Request Type");
  NotifyRadio("Coordinate System");
}

void CHATHOTRequestPanel::OnIDRadio(wxCommandEvent& event)
{
  if (event.GetString() == "HAT")
  {
    ChangeText("ID", "HAT ID");
  }
  else if (event.GetString() == "HOT")
  {
    ChangeText("ID", "HOT ID");
  }
  else if (event.GetString() == "Extended")
  {
    ChangeText("ID", "Extended ID");
  }
}

void CHATHOTRequestPanel::OnCoordinateSystemRadio(wxCommandEvent& event)
{
  if (event.GetString() == "Geodetic")
  {
    ChangeText("Latitude", "Latitude");
    ChangeText("Longitude", "Longitude");
    ChangeText("Altitude", "Altitude");
  }
  else
  {
    ChangeText("Latitude", "X Offset");
    ChangeText("Longitude", "Y Offset");
    ChangeText("Altitude", "Z Offset");
  }
}

void CHATHOTRequestPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  CHostSession* pSession = GetHostSession();
  if (pSession == nullptr)
  {
    return;
  }

  auto coordinateSystem = ConvertCigiStringToCoordinateSystem(GetRadio("Coordinate System"));
  auto requestID = HATHOTID(GetInt("ID"));
  auto updatePeriod = UpdatePeriod(GetInt("Update Period"));
  ERequestType eRequestType = (GetRadio("Request Type") == "Extended") ? ERequestType::EXTENDED : ((GetRadio("Request Type") == "HAT") ? ERequestType::HAT : ERequestType::HOT);

  if (coordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
  {
    auto latitude = Latitude(GetDouble("Latitude"));
    auto longitude = Longitude(GetDouble("Longitude"));
    auto altitude = GetDouble("Altitude");

    SHATHOTGlobalRequest request;
    request.requestID = requestID;
    request.updatePeriod = updatePeriod;
    request.geodeticCoordinates.latitude = latitude;
    request.geodeticCoordinates.longitude = longitude;
    request.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(altitude);
    request.eRequestType = eRequestType;
    pSession->SendHatHotRequest(request);
  }
  else
  {
    auto entityID = EntityID(GetInt("Entity ID"));
    sbio::math::Vec3 offset(GetFloat("Latitude"), GetFloat("Longitude"), GetFloat("Altitude"));

    SHATHOTEntityRequest request;
    request.requestID = requestID;
    request.updatePeriod = updatePeriod;
    request.entityID = entityID;
    request.offset = offset;
    request.eRequestType = eRequestType;
    pSession->SendHatHotRequest(request);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
