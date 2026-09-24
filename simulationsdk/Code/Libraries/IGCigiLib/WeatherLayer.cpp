//Copyright SimBlocks LLC 2016-2026
#include "WeatherLayer.h"
#include <algorithm>

using namespace sbio::cigi;
using namespace sbio::cigi::ig;

double CCigiWeatherLayer::IntersectionTest(sbio::math::HeightRelativeToWGS84Ellipsoid altitude)
{
  const double base = m_SpatialCondition.fBaseElevation;
  const double top = base + m_SpatialCondition.fThickness;

  // Altitude is below the layer's bounds.
  if (altitude.Value() < base)
  {
    const double transition = m_SpatialCondition.fBottomTransitionBandThickness;

    // If the transition band thickness is greater than zero, compute the contribution factor based on the distance from the base of the layer.
    return transition > 0 ? std::max(0.0, 1.0 - (base - altitude.Value()) / transition) : 0.0;
  }

  // Altitude is above the layer's bounds.
  if (altitude.Value() > top)
  {
    const double transition = m_SpatialCondition.fTopTransitionBandThickness;

    // If the transition band thickness is greater than zero, compute the contribution factor based on the distance from the top of the layer.
    return transition > 0 ? std::max(0.0, 1.0 - (altitude.Value() - top) / transition) : 0.0;
  }

  // Altitude is within the layer's bounds.
  return 1.0;
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
  SetActive(condition.bWeatherEnabled);
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
