//Copyright SimBlocks LLC 2016-2026
#include "EnvironmentalConditionsRequestPanel.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EntityLib/EntityLib.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CEnvironmentalConditionsRequestPanel, wxPanel)
  EVT_BUTTON(ID_ENVIRONMENTAL_CONDITIONS_REQUEST_PACKET, CEnvironmentalConditionsRequestPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CEnvironmentalConditionsRequestPanel::CEnvironmentalConditionsRequestPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Environmental Conditions Request Packet");

  wxArrayString requestTypeOptions;
  requestTypeOptions.Add("Maritime Surface Conditions");
  requestTypeOptions.Add("Terrestrial Surface Conditions");
  requestTypeOptions.Add("Weather Conditions");
  requestTypeOptions.Add("Aerosol Concentrations");
  CreateMultiSelectionControl(vbox, "Request Type", requestTypeOptions);

  CreateIntControl(vbox, "Request ID");

  CreateFloatControl(vbox, "Latitude");
  CreateFloatControl(vbox, "Longitude");
  CreateFloatControl(vbox, "Altitude");

  wxButton* pSendButton = new wxButton(this, ID_ENVIRONMENTAL_CONDITIONS_REQUEST_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CEnvironmentalConditionsRequestPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SEnvironmentalConditionsRequest environmentalConditionsRequest;

  set<string> requestTypes = GetMultipleSelections("Request Type");

  environmentalConditionsRequest.bMaritimeSurfaceConditionsRequest = requestTypes.find("Maritime Surface Conditions") != requestTypes.end();
  environmentalConditionsRequest.bTerrestrialSurfaceConditionsRequest = requestTypes.find("Terrestrial Surface Conditions") != requestTypes.end();
  environmentalConditionsRequest.bWeatherConditionsRequest = requestTypes.find("Weather Conditions") != requestTypes.end();
  environmentalConditionsRequest.bAerosolConcentrationsRequest = requestTypes.find("Aerosol Concentrations") != requestTypes.end();

  environmentalConditionsRequest.nRequestID = GetInt("Request ID");

  double latitude = GetDouble("Latitude");
  if (!Latitude::CheckValid(latitude))
  {
    HostCigiErrorEventArgs args;

    args.sError = "Latitude: Enter number between -90.0 and 90.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Latitude: Enter number between -90.0 and 90.0");
    latitude = ResetDouble("Latitude");
  }
  environmentalConditionsRequest.geodeticCoordinates.latitude = Latitude(latitude);

  double longitude = GetDouble("Longitude");
  if (!Longitude::CheckValid(longitude))
  {
    HostCigiErrorEventArgs args;

    args.sError = "Longtitude: Enter number between -180.0 and 180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Longitude: Enter number between -180.0 and 180.0");
    longitude = ResetDouble("Longitude");
  }
  environmentalConditionsRequest.geodeticCoordinates.longitude = Longitude(longitude);

  environmentalConditionsRequest.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(GetDouble("Altitude"));

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendEnvironmentalConditionsRequest(environmentalConditionsRequest);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
