//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightVectorRequestPanel.h"
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
wxBEGIN_EVENT_TABLE(CLineOfSightVectorRequestPanel, wxPanel)
  EVT_BUTTON(ID_LINE_OF_SIGHT_VECTOR_REQUEST_SEND_PACKET, CLineOfSightVectorRequestPanel::OnSend)
  EVT_RADIOBOX(ID_LINE_OF_SIGHT_VECTOR_REQUEST_SOURCE_POINT_COORDINATE_SYSTEM_EVENT, CLineOfSightVectorRequestPanel::OnSourcePointCoordinateSystemRadio)
  EVT_RADIOBOX(ID_LINE_OF_SIGHT_VECTOR_REQUEST_RESPONSE_COORDINATE_SYSTEM_EVENT, CLineOfSightVectorRequestPanel::OnSourcePointCoordinateSystemRadio)
wxEND_EVENT_TABLE();
// clang-format on

CLineOfSightVectorRequestPanel::CLineOfSightVectorRequestPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Line Of Sight Vector Request Packet");

  CreateIntControl(vbox, "LOS ID");
  CreateIntControl(vbox, "Source Entity ID");

  CreateBoolControl(vbox, "Extended Response");

  wxArrayString coordinateSystemOptions;
  coordinateSystemOptions.Add("Geodetic");
  coordinateSystemOptions.Add("Entity");

  CreateRadioControl(vbox, "Source Point Coordinate System", coordinateSystemOptions, ID_LINE_OF_SIGHT_VECTOR_REQUEST_SOURCE_POINT_COORDINATE_SYSTEM_EVENT);

  CreateRadioControl(vbox, "Response Coordinate System", coordinateSystemOptions, ID_LINE_OF_SIGHT_VECTOR_REQUEST_RESPONSE_COORDINATE_SYSTEM_EVENT);

  CreateIntControl(vbox, "Alpha Threshold");
  CreateIntControl(vbox, "Update Period");
  CreateFloatControl(vbox, "Azimuth");
  CreateFloatControl(vbox, "Elevation");
  CreateFloatControl(vbox, "Minimum Range");
  CreateFloatControl(vbox, "Maximum Range");

  CreateFloatControl(vbox, "Source Latitude");
  CreateFloatControl(vbox, "Source Longitude");
  CreateFloatControl(vbox, "Source Altitude");

  CreateIntControl(vbox, "Material Mask");

  wxButton* pSendButton = new wxButton(this, ID_LINE_OF_SIGHT_VECTOR_REQUEST_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CLineOfSightVectorRequestPanel::OnDestinationPointCoordinateSystemRadio(wxCommandEvent& event)
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

void CLineOfSightVectorRequestPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  uint16_t requestID = GetInt("LOS ID");
  uint16_t entityID = GetInt("Source Entity ID");

  ETopLevelCoordinateSystem eSourcePointCoordinateSystem = ConvertCigiStringToCoordinateSystem(GetRadio("Source Point Coordinate System"));

  if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
  {
    SLineOfSightVectorRequestGeodeticBasic lineOfSightVectorRequest;
    lineOfSightVectorRequest.requestID = LineOfSightRequestID(requestID);
    lineOfSightVectorRequest.nAlphaThreshold = GetInt("Alpha Threshold");
    lineOfSightVectorRequest.updatePeriod = UpdatePeriod(GetInt("Update Period"));
    lineOfSightVectorRequest.azimuth = Degrees180(GetFloat("Azimuth"));
    lineOfSightVectorRequest.elevation = Degrees90(GetFloat("Elevation"));
    lineOfSightVectorRequest.fMaximumRange = GetFloat("Maximum Range");
    lineOfSightVectorRequest.fMinimumRange = GetFloat("Minimum Range");
    lineOfSightVectorRequest.sourceGeodeticCoordinates.latitude = Latitude(GetDouble("Source Latitude"));
    lineOfSightVectorRequest.sourceGeodeticCoordinates.longitude = Longitude(GetDouble("Source Longitude"));
    lineOfSightVectorRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(GetDouble("Source Altitude"));
    lineOfSightVectorRequest.nMaterialMask = GetInt("Material Mask");

    CHostSession* pSession = GetHostSession();
    pSession->SendLineOfSightVectorRequestGeodeticBasic(lineOfSightVectorRequest);
  }
  else if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
  {
    SLineOfSightVectorRequestEntityBasic lineOfSightVectorRequest;
    lineOfSightVectorRequest.requestID = LineOfSightRequestID(requestID);
    lineOfSightVectorRequest.sourceEntityID = EntityID(entityID);
    lineOfSightVectorRequest.nAlphaThreshold = GetInt("Alpha Threshold");
    lineOfSightVectorRequest.updatePeriod = UpdatePeriod(GetInt("Update Period"));
    lineOfSightVectorRequest.azimuth = Degrees180(GetFloat("Azimuth"));
    lineOfSightVectorRequest.elevation = Degrees90(GetFloat("Elevation"));
    lineOfSightVectorRequest.fMaximumRange = GetFloat("Maximum Range");
    lineOfSightVectorRequest.fMinimumRange = GetFloat("Minimum Range");
    lineOfSightVectorRequest.sourceOffset[0] = GetFloat("Source Latitude");
    lineOfSightVectorRequest.sourceOffset[1] = GetFloat("Source Longitude");
    lineOfSightVectorRequest.sourceOffset[2] = GetFloat("Source Altitude");
    lineOfSightVectorRequest.nMaterialMask = GetInt("Material Mask");

    CHostSession* pSession = GetHostSession();
    pSession->SendLineOfSightVectorRequestEntityBasic(lineOfSightVectorRequest);
  }
}

void CLineOfSightVectorRequestPanel::OnSourcePointCoordinateSystemRadio(wxCommandEvent& event)
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

void CLineOfSightVectorRequestPanel::OnResponseCoordinateSystemRadio(wxCommandEvent& event)
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
