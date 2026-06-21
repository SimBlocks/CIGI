//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightSegmentRequestPanel.h"
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
wxBEGIN_EVENT_TABLE(CLineOfSightSegmentRequestPanel, wxPanel)
  EVT_BUTTON(ID_LINE_OF_SIGHT_SEGMENT_REQUEST_SEND_PACKET, CLineOfSightSegmentRequestPanel::OnSend)
  EVT_RADIOBOX(ID_LINE_OF_SIGHT_SEGMENT_REQUEST_SOURCE_POINT_COORDINATE_SYSTEM_EVENT, CLineOfSightSegmentRequestPanel::OnSourcePointCoordinateSystemRadio)
  EVT_RADIOBOX(ID_LINE_OF_SIGHT_SEGMENT_REQUEST_DESTINATION_POINT_COORDINATE_SYSTEM_EVENT, CLineOfSightSegmentRequestPanel::OnDestinationPointCoordinateSystemRadio)
wxEND_EVENT_TABLE();
// clang-format on

CLineOfSightSegmentRequestPanel::CLineOfSightSegmentRequestPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Line Of Sight Segment Request Packet");

  CreateIntControl(vbox, "LOS ID");
  CreateIntControl(vbox, "Source Entity ID");

  CreateBoolControl(vbox, "Extended Response");

  wxArrayString coordinateSystemOptions;
  coordinateSystemOptions.Add("Geodetic");
  coordinateSystemOptions.Add("Entity");

  CreateRadioControl(vbox, "Source Point Coordinate System", coordinateSystemOptions, ID_LINE_OF_SIGHT_SEGMENT_REQUEST_SOURCE_POINT_COORDINATE_SYSTEM_EVENT);
  CreateRadioControl(vbox, "Destination Point Coordinate System", coordinateSystemOptions, ID_LINE_OF_SIGHT_SEGMENT_REQUEST_DESTINATION_POINT_COORDINATE_SYSTEM_EVENT);

  CreateRadioControl(vbox, "Response Coordinate System", coordinateSystemOptions, ID_LINE_OF_SIGHT_SEGMENT_REQUEST_RESPONSE_COORDINATE_SYSTEM_EVENT);

  CreateBoolControl(vbox, "Destination Entity ID Valid");
  CreateIntControl(vbox, "Alpha Threshold");
  CreateIntControl(vbox, "Destination Entity ID");
  CreateIntControl(vbox, "Update Period");

  CreateFloatControl(vbox, "Source Latitude");
  CreateFloatControl(vbox, "Source Longitude");
  CreateFloatControl(vbox, "Source Altitude");
  CreateFloatControl(vbox, "Destination Latitude");
  CreateFloatControl(vbox, "Destination Longitude");
  CreateFloatControl(vbox, "Destination Altitude");

  CreateIntControl(vbox, "Material Mask");

  wxButton* pSendButton = new wxButton(this, ID_LINE_OF_SIGHT_SEGMENT_REQUEST_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CLineOfSightSegmentRequestPanel::OnDestinationPointCoordinateSystemRadio(wxCommandEvent& event)
{
  if (GetRadio("Destination Point Coordinate System") == "Geodetic")
  {
    ChangeText("Destination Latitude", "Destination Latitude");
    ChangeText("Destination Longitude", "Destination Longitude");
    ChangeText("Destination Altitude", "Destination Altitude");
  }
  else
  {
    ChangeText("Destination Latitude", "Destination X Offset");
    ChangeText("Destination Longitude", "Destination Y Offset");
    ChangeText("Destination Altitude", "Destination Z Offset");
  }
}

void CLineOfSightSegmentRequestPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  uint16_t losRequestID = GetInt("LOS ID");
  uint16_t sourceEntityID = GetInt("Source Entity ID");
  uint16_t destEntityID = GetInt("Destination Entity ID");

  ETopLevelCoordinateSystem eSourcePointCoordinateSystem = ConvertCigiStringToCoordinateSystem(GetRadio("Source Point Coordinate System"));
  ETopLevelCoordinateSystem eDestinationPointCoordinateSystem = ConvertCigiStringToCoordinateSystem(GetRadio("Destination Point Coordinate System"));

  if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC && eDestinationPointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
  {
    SLineOfSightSegmentRequestGeodeticToGeodeticBasic lineOfSightSegmentRequest;
    lineOfSightSegmentRequest.requestID = LineOfSightRequestID(losRequestID);
    lineOfSightSegmentRequest.nAlphaThreshold = GetInt("Alpha Threshold");
    lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(GetInt("Update Period"));
    lineOfSightSegmentRequest.nMaterialMask = GetInt("Material Mask");
    lineOfSightSegmentRequest.sourceGeodeticCoordinates.latitude = Latitude(GetDouble("Source Latitude"));
    lineOfSightSegmentRequest.sourceGeodeticCoordinates.longitude = Longitude(GetDouble("Source Longitude"));
    lineOfSightSegmentRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(GetFloat("Source Altitude"));
    lineOfSightSegmentRequest.destinationGeodeticCoordinates.latitude = Latitude(GetDouble("Destination Latitude"));
    lineOfSightSegmentRequest.destinationGeodeticCoordinates.longitude = Longitude(GetDouble("Destination Longitude"));
    lineOfSightSegmentRequest.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(GetFloat("Destination Altitude"));

    CHostSession* pSession = GetHostSession();
    pSession->SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(lineOfSightSegmentRequest);
  }
  else if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC && eDestinationPointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
  {
    SLineOfSightSegmentRequestGeodeticToEntityBasic lineOfSightSegmentRequest;
    lineOfSightSegmentRequest.requestID = LineOfSightRequestID(losRequestID);
    lineOfSightSegmentRequest.nAlphaThreshold = GetInt("Alpha Threshold");
    lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(GetInt("Update Period"));
    lineOfSightSegmentRequest.nMaterialMask = GetInt("Material Mask");
    lineOfSightSegmentRequest.sourceGeodeticCoordinates.latitude = Latitude(GetDouble("Source Latitude"));
    lineOfSightSegmentRequest.sourceGeodeticCoordinates.longitude = Longitude(GetDouble("Source Longitude"));
    lineOfSightSegmentRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(GetFloat("Source Altitude"));
    lineOfSightSegmentRequest.destinationEntityID = EntityID(destEntityID);
    lineOfSightSegmentRequest.destinationOffset[0] = GetFloat("Destination Latitude");
    lineOfSightSegmentRequest.destinationOffset[1] = GetFloat("Destination Longitude");
    lineOfSightSegmentRequest.destinationOffset[2] = GetFloat("Destination Altitude");

    CHostSession* pSession = GetHostSession();
    pSession->SendLineOfSightSegmentRequestGeodeticToEntityBasic(lineOfSightSegmentRequest);
  }
  else if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationPointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
  {
    SLineOfSightSegmentRequestEntityToGeodeticBasic lineOfSightSegmentRequest;
    lineOfSightSegmentRequest.requestID = LineOfSightRequestID(losRequestID);
    lineOfSightSegmentRequest.sourceEntityID = EntityID(sourceEntityID);
    lineOfSightSegmentRequest.nAlphaThreshold = GetInt("Alpha Threshold");
    lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(GetInt("Update Period"));
    lineOfSightSegmentRequest.nMaterialMask = GetInt("Material Mask");
    lineOfSightSegmentRequest.sourceOffset[0] = GetFloat("Source Latitude");
    lineOfSightSegmentRequest.sourceOffset[1] = GetFloat("Source Longitude");
    lineOfSightSegmentRequest.sourceOffset[2] = GetFloat("Source Altitude");

    CHostSession* pSession = GetHostSession();
    pSession->SendLineOfSightSegmentRequestEntityToGeodeticBasic(lineOfSightSegmentRequest);
  }
  else if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationPointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
  {
    SLineOfSightSegmentRequestEntityToEntityBasic lineOfSightSegmentRequest;
    lineOfSightSegmentRequest.requestID = LineOfSightRequestID(losRequestID);
    lineOfSightSegmentRequest.sourceEntityID = EntityID(sourceEntityID);
    lineOfSightSegmentRequest.nAlphaThreshold = GetInt("Alpha Threshold");
    lineOfSightSegmentRequest.destinationEntityID = EntityID(destEntityID);
    lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(GetInt("Update Period"));
    lineOfSightSegmentRequest.nMaterialMask = GetInt("Material Mask");
    lineOfSightSegmentRequest.sourceOffset[0] = GetFloat("Source Latitude");
    lineOfSightSegmentRequest.sourceOffset[1] = GetFloat("Source Longitude");
    lineOfSightSegmentRequest.sourceOffset[2] = GetFloat("Source Altitude");
    lineOfSightSegmentRequest.destinationEntityID = EntityID(destEntityID);
    lineOfSightSegmentRequest.destinationOffset[0] = GetFloat("Destination Latitude");
    lineOfSightSegmentRequest.destinationOffset[1] = GetFloat("Destination Longitude");
    lineOfSightSegmentRequest.destinationOffset[2] = GetFloat("Destination Altitude");

    CHostSession* pSession = GetHostSession();
    pSession->SendLineOfSightSegmentRequestEntityToEntityBasic(lineOfSightSegmentRequest);
  }
}

void CLineOfSightSegmentRequestPanel::OnSourcePointCoordinateSystemRadio(wxCommandEvent& event)
{
  if (GetRadio("Source Point Coordinate System") == "Geodetic")
  {
    ChangeText("Source Latitude", "Source Latitude");
    ChangeText("Source Longitude", "Source Longitude");
    ChangeText("Source Altitude", "Source Altitude");
  }
  else
  {
    ChangeText("Source Latitude", "Source X Offset");
    ChangeText("Source Longitude", "Source Y Offset");
    ChangeText("Source Altitude", "Source Z Offset");
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
