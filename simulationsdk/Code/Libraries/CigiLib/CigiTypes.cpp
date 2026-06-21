//Copyright SimBlocks LLC 2016-2026
#include "CigiLib/CigiTypesHostToIG.h"
#include "MathLib/Math.h"

using namespace sbio::cigi;
using namespace sbio::math;

/// <summary>
/// Add result of B onto A
/// </summary>
SCigiWeatherCondition SCigiWeatherCondition::Sum(const SCigiWeatherCondition& a, const SCigiWeatherCondition& b)
{
  SCigiWeatherCondition result;

  result.bWeatherEnabled = a.bWeatherEnabled || b.bWeatherEnabled;
  result.bBottomScudEnabled = a.bBottomScudEnabled || b.bBottomScudEnabled;
  result.bTopScudEnabled = a.bTopScudEnabled || b.bTopScudEnabled;
  result.bRandomWindsEnabled = a.bRandomWindsEnabled || b.bRandomWindsEnabled;
  result.bRandomLightningEnabled = a.bRandomLightningEnabled || b.bRandomLightningEnabled;

  result.cloudType = (b.cloudType != UnknownCloudType) ? b.cloudType : a.cloudType;
  result.severity = (b.severity != UnknownWeatherSeverity) ? b.severity : a.severity;

  result.fAerosolConcentration = a.fAerosolConcentration + b.fAerosolConcentration;
  result.fAirTemperature = a.fAirTemperature + b.fAirTemperature;
  result.fBarometricPressure = a.fBarometricPressure + b.fBarometricPressure;
  result.humidity = a.humidity + b.humidity;
  result.fVisibilityRange = a.fVisibilityRange + b.fVisibilityRange;
  result.HorizontalWindSpeed = a.HorizontalWindSpeed + b.HorizontalWindSpeed;
  result.VerticalWindSpeed = a.VerticalWindSpeed + b.VerticalWindSpeed;
  result.WindDirection = a.WindDirection + b.WindDirection;
  result.coverage = a.coverage + b.coverage;
  result.bottomScudFrequency = a.bottomScudFrequency + b.bottomScudFrequency;
  result.topScudFrequency = a.topScudFrequency + b.topScudFrequency;

  return result;
}

SCigiWeatherCondition SCigiWeatherCondition::Scale(float scale)
{
  SCigiWeatherCondition result;

  result.bWeatherEnabled = bWeatherEnabled;
  result.bBottomScudEnabled = bBottomScudEnabled;
  result.bTopScudEnabled = bTopScudEnabled;
  result.bRandomWindsEnabled = bRandomWindsEnabled;
  result.bRandomLightningEnabled = bRandomLightningEnabled;
  result.cloudType = cloudType;
  result.severity = severity;
  result.humidity = humidity * scale;
  result.fAirTemperature = fAirTemperature * scale;
  result.fVisibilityRange = fVisibilityRange * scale;
  result.bottomScudFrequency = bottomScudFrequency * scale;
  result.topScudFrequency = topScudFrequency * scale;
  result.coverage = coverage * scale;
  result.fBarometricPressure = fBarometricPressure * scale;
  result.fAerosolConcentration = fAerosolConcentration * scale;
  result.HorizontalWindSpeed = HorizontalWindSpeed * scale;
  result.VerticalWindSpeed = VerticalWindSpeed * scale;
  result.WindDirection = WindDirection;

  return result;
}

SCigiMaritimeSurfaceCondition SCigiMaritimeSurfaceCondition::Sum(const SCigiMaritimeSurfaceCondition& a, const SCigiMaritimeSurfaceCondition& b)
{
  SCigiMaritimeSurfaceCondition result;

  result.bActive = a.bActive || b.bActive;
  result.fSeaSurfaceHeight = a.fSeaSurfaceHeight + b.fSeaSurfaceHeight;
  result.fSurfaceWaterTemperature = a.fSurfaceWaterTemperature + b.fSurfaceWaterTemperature;
  result.surfaceClarity = a.surfaceClarity + b.surfaceClarity;
  result.bWhitecapEnabled = a.bWhitecapEnabled || b.bWhitecapEnabled;
  return result;
}

SCigiMaritimeSurfaceCondition SCigiMaritimeSurfaceCondition::Scale(float scale)
{
  SCigiMaritimeSurfaceCondition result;

  result.bActive = bActive;
  result.fSeaSurfaceHeight = fSeaSurfaceHeight * scale;
  result.fSurfaceWaterTemperature = fSurfaceWaterTemperature * scale;
  result.surfaceClarity = surfaceClarity * scale;
  result.bWhitecapEnabled = bWhitecapEnabled;
  return result;
}

bool SCigiTerrestrialSurfaceCondition::IsDry() const
{
  return surfaceConditionID == SurfaceConditionID(0);
}

bool SCigiComponentKey::operator==(const SCigiComponentKey& key) const
{
  return (componentID == key.componentID) && (componentClassID == key.componentClassID) && (nInstanceID == key.nInstanceID);
}

bool SCigiComponentKey::operator<(const SCigiComponentKey& key) const
{
  if (componentID == key.componentID)
  {
    if (componentClassID == key.componentClassID)
    {
      return nInstanceID < key.nInstanceID;
    }

    return componentClassID < key.componentClassID;
  }

  return componentID < key.componentID;
}

bool SCigiComponentControlState::operator==(const SCigiComponentControlState& state) const
{
  return (nComponentState == state.nComponentState) && (componentData[0] == state.componentData[0]) && (componentData[1] == state.componentData[1]) && (componentData[2] == state.componentData[2]) && (componentData[3] == state.componentData[3]) && (componentData[4] == state.componentData[4]) &&
         (componentData[5] == state.componentData[5]);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
