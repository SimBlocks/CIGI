//Copyright SimBlocks LLC 2016-2026
#include "WeatherLayer.h"

using namespace sbio::cigi;
using namespace sbio::cigi::ig;

double CCigiWeatherLayer::IntersectionTest(sbio::math::HeightRelativeToWGS84Ellipsoid altitude)
{
  float fTotalThickness = m_SpatialCondition.fBottomTransitionBandThickness + m_SpatialCondition.fThickness + m_SpatialCondition.fTopTransitionBandThickness;
  double middle = m_SpatialCondition.fBaseElevation + (m_SpatialCondition.fThickness / 2.0);

  // equals m_Condition.Thickness / 2 at edges of clouds
  double value = std::abs(altitude.Value() - middle);

  // inside clouds
  if (value <= fTotalThickness / 2.0f)
  {
    return 1.0;
  }

  // reset so 0 is at edge of clouds
  value -= fTotalThickness / 2;

  // set end of transistion band at 1
  value /= (m_SpatialCondition.fBottomTransitionBandThickness + m_SpatialCondition.fTopTransitionBandThickness);

  // flip so edge of cloud is 1 and transition band is 0
  value = 1 - value;

  return std::max<double>(value, 0);
}

CCigiWeatherLayer::CCigiWeatherLayer()
{
}

bool CCigiWeatherLayer::GetActive() const
{
  return m_bActive;
}

bool CCigiWeatherLayer::GetScud() const
{
  return m_bScud;
}

bool CCigiWeatherLayer::GetRandomWinds() const
{
  return m_bRandomWinds;
}

bool CCigiWeatherLayer::GetRandomLightening() const
{
  return m_bRandomLightning;
}

void CCigiWeatherLayer::SetActive(bool val)
{
  m_bActive = val;
}

void CCigiWeatherLayer::SetScud(bool val)
{
  m_bScud = val;
}

void CCigiWeatherLayer::SetRandomWinds(bool val)
{
  m_bRandomWinds = val;
}

void CCigiWeatherLayer::SetRandomLightening(bool val)
{
  m_bRandomLightning = val;
}

void CCigiWeatherLayer::SetWeatherCondition(const SCigiWeatherCondition& condition)
{
  m_Condition = condition;
}

void CCigiWeatherLayer::SetSpatialWeatherCondition(const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  m_SpatialCondition = spatialWeatherCondition;
}

SCigiWeatherCondition& CCigiWeatherLayer::GetWeatherCondition()
{
  return m_Condition;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
