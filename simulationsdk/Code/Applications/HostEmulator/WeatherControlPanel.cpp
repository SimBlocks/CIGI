//Copyright SimBlocks LLC 2016-2026
#include "WeatherControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CWeatherControlPanel, wxPanel)
  EVT_BUTTON(ID_WEATHER_CONTROL_SEND_PACKET, CWeatherControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CWeatherControlPanel::CWeatherControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Weather Control Packet");

  wxArrayString layerOptions;
  wxArrayString cloudTypeOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto layers = pHost->GetScriptRuntime()->GetLayers();
      for (auto itLayer : layers)
      {
        layerOptions.Add(itLayer);
      }

      auto cloudTypes = pHost->GetScriptRuntime()->GetCloudTypes();
      for (auto itCloudType : cloudTypes)
      {
        cloudTypeOptions.Add(itCloudType);
      }
    }
  }
  CreateChoiceControl(vbox, "Layer ID", layerOptions);

  CreateIntControl(vbox, "Humidity ID");//create slider
  CreateBoolControl(vbox, "Weather Enable");
  CreateBoolControl(vbox, "Bottom Scud Enable");
  CreateBoolControl(vbox, "Random Winds Enable");
  CreateBoolControl(vbox, "Random Lightning Enable");

  CreateChoiceControl(vbox, "Cloud Type", cloudTypeOptions);

  wxArrayString scopeOptions;
  scopeOptions.Add("Global");
  scopeOptions.Add("Regional");
  scopeOptions.Add("Entity");
  CreateChoiceControl(vbox, "Scope", scopeOptions);

  CreateRangeControl(vbox, "Severity", 0, 5);
  CreateBoolControl(vbox, "Top Scud Enable");
  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Region ID");
  CreateFloatControl(vbox, "Air Temperature");
  CreateFloatControl(vbox, "Visibility Range");
  CreateFloatControl(vbox, "Bottom Scud Frequency");
  CreateFloatControl(vbox, "Coverage");
  CreateFloatControl(vbox, "Base Elevation");
  CreateFloatControl(vbox, "Thickness");
  CreateFloatControl(vbox, "Bottom Transition Band Thickness");
  CreateFloatControl(vbox, "Horizontal Wind Speed");
  CreateFloatControl(vbox, "Vertical Wind Speed");
  CreateFloatControl(vbox, "Wind Direction");
  CreateFloatControl(vbox, "Barometric Pressure");
  CreateFloatControl(vbox, "Aerosol Concentration");
  CreateFloatControl(vbox, "Top Scud Frequency");
  CreateFloatControl(vbox, "Top Transition Band Thickness");

  wxButton* pSendButton = new wxButton(this, ID_WEATHER_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CWeatherControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  float horizontalWindMin = 0;
  float windDirectionMin = 0.0, windDirectionMax = 360.0;
  float barometricMin = 0;
  float aerosolMin = 0;

  SCigiWeatherCondition condition;
  condition.bRandomWindsEnabled = GetBool("Random Winds Enable");
  condition.bRandomLightningEnabled = GetBool("Random Lightning Enable");
  condition.cloudType = CloudType((GetChoiceIndex("Cloud Type")));

  int severity = GetRange("Severity");
  if (severity < 0 || severity > 5)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Weather Severity out of range";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Weather Severity out of range");
    severity = 0;
  }

  condition.severity = WeatherSeverity(severity);
  condition.fAerosolConcentration = GetFloat("Aerosol Concentration");
  condition.fAirTemperature = TemperatureCelsius(GetFloat("Air Temperature"));
  condition.fBarometricPressure = GetFloat("Barometric Pressure");

  float coverage = GetFloat("Coverage");
  if (!CigiFloatPercentage::CheckValid(coverage))
  {
    HostCigiErrorEventArgs args;
    args.sError = "Coverage: Enter number between 0 and 100";
    Event::Raise<HostCigiEvent>(args);

    coverage = ResetFloat("Coverage");
  }
  CigiFloatPercentage cigiCoverage(coverage);
  condition.coverage = ConvertToPercentage(cigiCoverage);

  int humidity = GetInt("Humidity ID");
  if (!CigiIntPercentage::CheckValid(humidity))
  {
    HostCigiErrorEventArgs args;
    args.sError = "Humidity ID: Enter number between 0 and 100";
    Event::Raise<HostCigiEvent>(args);

    humidity = ResetInt("Humidity ID");
  }
  CigiIntPercentage cigiHumidity(humidity);
  condition.humidity = ConvertToPercentage(cigiHumidity);

  condition.fVisibilityRange = GetFloat("Visibility Range");
  condition.bTopScudEnabled = GetBool("Top Scud Enable");
  condition.bBottomScudEnabled = GetBool("Bottom Scud Enable");

  float bottomScudFrequency = GetFloat("Bottom Scud Frequency");
  if (!CigiFloatPercentage::CheckValid(bottomScudFrequency))
  {
    HostCigiErrorEventArgs args;
    args.sError = "Bottom Scud Frequency: Enter number between 0 and 100";
    Event::Raise<HostCigiEvent>(args);

    bottomScudFrequency = ResetFloat("Bottom Scud Frequency");
  }
  CigiFloatPercentage cigiBottomScudFrequency(bottomScudFrequency);
  condition.bottomScudFrequency = ConvertToPercentage(cigiBottomScudFrequency);

  float topScudFrequency = GetFloat("Top Scud Frequency");
  if (!CigiFloatPercentage::CheckValid(topScudFrequency))
  {
    HostCigiErrorEventArgs args;
    args.sError = "Top Scud Frequency: Enter number between 0.0 and 100.0";
    Event::Raise<HostCigiEvent>(args);

    topScudFrequency = ResetFloat("Top Scud Frequency");
  }
  CigiFloatPercentage cigiTopScudFrequency(topScudFrequency);
  condition.topScudFrequency = ConvertToPercentage(cigiTopScudFrequency);

  condition.bWeatherEnabled = GetBool("Weather Enable");

  int nLayerID = GetChoiceIndex("Layer ID");
  std::string sScope = GetChoice("Scope");
  int nEntityID = GetInt("Entity ID");
  int nRegionID = GetInt("Region ID");

  SCigiSpatialWeatherCondition spatialWeatherCondition;
  spatialWeatherCondition.fBaseElevation = GetFloat("Base Elevation");
  spatialWeatherCondition.fBottomTransitionBandThickness = GetFloat("Bottom Transition Band Thickness");
  spatialWeatherCondition.fTopTransitionBandThickness = GetFloat("Top Transition Band Thickness");
  spatialWeatherCondition.fThickness = GetFloat("Thickness");

  condition.HorizontalWindSpeed = GetFloat("Horizontal Wind Speed");
  condition.VerticalWindSpeed = GetFloat("Vertical Wind Speed");
  condition.WindDirection = GetFloat("Wind Direction");

  if (condition.HorizontalWindSpeed < horizontalWindMin)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Horizontal Wind Speed: Enter number greater than or equal to 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Horizontal Wind Speed: Enter number greater than or equal to 0");
    condition.HorizontalWindSpeed = ResetFloat("Horizontal Wind Speed");
  }
  if (condition.WindDirection < windDirectionMin || condition.WindDirection > windDirectionMax)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Wind Direction: Enter number between 0.0 and 360.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Wind Direction: Enter number between 0.0 and 360.0");
    condition.WindDirection = ResetFloat("Wind Direction");
  }
  if (condition.fBarometricPressure < barometricMin)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Barometric Pressure: Enter number greater than or equal to 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Barometric Pressure: Enter number greater than or equal to 0");
    condition.fBarometricPressure = ResetFloat("Barometric Pressure");
  }
  if (condition.fAerosolConcentration < aerosolMin)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Aerosol Concentration: Enter number greater than or equal to 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Aerosol Concentration: Enter number greater than or equal to 0");
    condition.fAerosolConcentration = ResetFloat("Aerosol Concentration");
  }

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();

  if (sScope == "Global")
  {
    pSession->SendWeatherControl(GlobalLayeredWeatherID(nLayerID), condition, spatialWeatherCondition);
  }
  else if (sScope == "Regional")
  {
    pSession->SendWeatherControl(RegionID(nRegionID), RegionalLayeredWeatherID(nLayerID), condition, spatialWeatherCondition);
  }
  else if (sScope == "Entity")
  {
    pSession->SendWeatherControl(EntityID(nEntityID), condition);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
