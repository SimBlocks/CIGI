//Copyright SimBlocks LLC 2016-2026
#include "EnvironmentalRegionControlPanel.h"
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
wxBEGIN_EVENT_TABLE(CEnvironmentalRegionControlPanel, wxPanel)
  EVT_BUTTON(ID_ENVIRONMENTAL_REGION_CONTROL_SEND_PACKET, CEnvironmentalRegionControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CEnvironmentalRegionControlPanel::CEnvironmentalRegionControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Environmental Region Control Packet");

  wxArrayString regionStateSystemOptions;
  regionStateSystemOptions.Add("Inactive");
  regionStateSystemOptions.Add("Active");
  regionStateSystemOptions.Add("Destroyed");
  CreateChoiceControl(vbox, "Region State", regionStateSystemOptions);

  wxArrayString mergeOptions;
  mergeOptions.Add("Use Last");
  mergeOptions.Add("Merge");

  CreateChoiceControl(vbox, "Merge Weather Properties", mergeOptions);
  CreateChoiceControl(vbox, "Merge Aerosol Concentrations", mergeOptions);
  CreateChoiceControl(vbox, "Merge Maritime Surface Conditions", mergeOptions);
  CreateChoiceControl(vbox, "Merge Terrestrial Surface Conditions", mergeOptions);

  CreateIntControl(vbox, "Region ID");
  CreateFloatControl(vbox, "Latitude");
  CreateFloatControl(vbox, "Longitude");
  CreateFloatControl(vbox, "Size X");
  CreateFloatControl(vbox, "Size Y");
  CreateFloatControl(vbox, "Corner Radius");
  CreateFloatControl(vbox, "Rotation");
  CreateFloatControl(vbox, "Transition Parameter");

  wxButton* pSendButton = new wxButton(this, ID_ENVIRONMENTAL_REGION_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CEnvironmentalRegionControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  float minCornerRadius = 0;
  float sizeMin = 0;
  float transitionMin = 0;

  SCigiEnvironmentalRegion environmentalRegion;
  environmentalRegion.eRegionState = ConvertCigiStringToActiveState(GetChoice("Region State"));
  environmentalRegion.eMergeWeatherProperties = ConvertCigiStringToMergeState(GetChoice("Merge Weather Properties"));
  environmentalRegion.eMergeAerosolConcentrations = ConvertCigiStringToMergeState(GetChoice("Merge Aerosol Concentrations"));
  environmentalRegion.eMergeMaritimeSurfaceConditions = ConvertCigiStringToMergeState(GetChoice("Merge Maritime Surface Conditions"));
  environmentalRegion.eMergeTerrestrialSurfaceConditions = ConvertCigiStringToMergeState(GetChoice("Merge Terrestrial Surface Conditions"));
  environmentalRegion.regionID = RegionID(GetInt("Region ID"));

  HostCigiErrorEventArgs args;

  double latitude = GetDouble("Latitude");
  if (!Latitude::CheckValid(latitude))
  {
    args.sError = "Latitude: Enter number between -90.0 and 90.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Latitude: Enter number between -90.0 and 90.0");
    latitude = ResetDouble("Latitude");
  }
  environmentalRegion.latitude = Latitude(latitude);

  double longitude = GetDouble("Longitude");
  if (!Longitude::CheckValid(longitude))
  {
    args.sError = "Longtitude: Enter number between -180.0 and 180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Longitude: Enter number between -180.0 and -180.0");
    longitude = ResetDouble("Longitude");
  }
  environmentalRegion.longitude = Longitude(longitude);

  environmentalRegion.size[0] = GetFloat("Size X");
  environmentalRegion.size[1] = GetFloat("Size Y");
  environmentalRegion.fCornerRadius = GetFloat("Corner Radius");
  environmentalRegion.fTransition = GetFloat("Transition Parameter");

  float maxCornerRadius = 0.5 * environmentalRegion.size[0];
  if (0.5 * environmentalRegion.size[1] < 0.5 * environmentalRegion.size[0])
  {
    maxCornerRadius = 0.5 * environmentalRegion.size[1];
  }

  if (environmentalRegion.size[0] < sizeMin)
  {
    args.sError = "Size X: Enter number greater than 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Size X: Enter number greater than 0");
    environmentalRegion.size[0] = ResetFloat("Size X");
  }
  if (environmentalRegion.size[1] < sizeMin)
  {
    args.sError = "Size Y: Enter number greater than 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Size Y: Enter number greater than 0");
    environmentalRegion.size[1] = ResetFloat("Size Y");
  }
  if (environmentalRegion.fCornerRadius < minCornerRadius || environmentalRegion.fCornerRadius > maxCornerRadius)
  {
    args.sError = "Corner Radius: Enter number between min corner radius and max corner radius.";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Corner Radius: Enter number between min corner radius and max corner radius.");
    environmentalRegion.fCornerRadius = ResetFloat("Corner Radius");
  }

  float rotation = GetFloat("Rotation");
  if (!Degrees180::CheckValid(rotation))
  {
    args.sError = "Rotation: Enter number between -180.0 and -180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Rotation: Enter number between -180.0 and -180.0");
    rotation = ResetFloat("Rotation");
  }
  environmentalRegion.fRotation = Degrees180(rotation);

  if (environmentalRegion.fTransition < transitionMin)
  {
    args.sError = "Transition Perimeter: Enter number greater than 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Transition Perimeter: Enter number greater than 0");
    environmentalRegion.fTransition = ResetFloat("Transition Perimeter");
  }

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendEnvironmentalRegionControl(environmentalRegion);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
