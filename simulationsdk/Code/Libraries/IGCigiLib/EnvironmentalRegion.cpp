//Copyright SimBlocks LLC 2016-2026
#include "EnvironmentalRegion.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiConversions.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include <algorithm>
#include <cmath>

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::cigi::ig;
using namespace sbio::ig::atmosphere;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiEnvironmentalRegion::CCigiEnvironmentalRegion() = default;

CCigiEnvironmentalRegion::CCigiEnvironmentalRegion(RegionID regionID) : m_Scope(ECigiScope::REGIONAL), m_RegionID(regionID)
{
}

CCigiEnvironmentalRegion::CCigiEnvironmentalRegion(EntityID entityID) : m_Scope(ECigiScope::ENTITY), m_EntityID(entityID)
{
}

void CCigiEnvironmentalRegion::SetActive(bool active)
{
  m_bActive = active;
}

bool CCigiEnvironmentalRegion::IsActive() const
{
  return m_bActive;
}

void CCigiEnvironmentalRegion::SetOrigin(Latitude latitude, Longitude longitude)
{
  m_Origin = SGeodeticCoordinates(latitude, longitude, 0);
}

GeocentricCoordinates CCigiEnvironmentalRegion::GetOrigin() const
{
  return ConvertCigiGeodeticToWorldCoordinates(m_Origin);
}

double CCigiEnvironmentalRegion::GetRadius() const
{
  // Calculate the radius of the region based on its dimensions and transition perimeter.
  return std::sqrt(m_SizeX * m_SizeX + m_SizeY * m_SizeY) + std::max(0.0f, m_TransitionPerimeter);
}

void CCigiEnvironmentalRegion::SetDimensions(float x, float y, float radius, float transitionPerimeter)
{
  m_SizeX = x / 2;
  m_SizeY = y / 2;
  m_CornerRadius = radius;
  m_TransitionPerimeter = transitionPerimeter;
}

void CCigiEnvironmentalRegion::SetRotation(double rotation)
{
  m_Rotation = rotation;
}

/// <summary>
/// Detect one of 9 cases in solving for rounded rectangles. One of 4 rounded corners, one of 4 edge rectangles, 1 center rectangle.
/// No transition bounds, sharp edges.
/// </summary>
float CCigiEnvironmentalRegion::SolveRoundedRectangleCases(Vec2d point, double X, double Y, double radius)
{
  // represents the 9 cases.
  //[4][7][1]
  //[6][9][3]
  //[5][8][2]

  // subtract by radius to account for the area between the quarter circles.
  double sizeX = X - radius;
  double sizeY = Y - radius;

  // top right corner, case 1
  if (point.x() > sizeX && point.y() > sizeY)
  {
    // use circle for top right corner
    double distance = (point - Vec2d(sizeX, sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // bottom right corner, case 2
  else if (point.x() > sizeX && point.y() < -sizeY)
  {
    // use circle for bottom right corner
    double distance = (point - Vec2d(sizeX, -sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // middle right edge, case 3
  else if (point.x() > sizeX)
  {
    // within rectangle between inner rectangle and right edge
    if (point.x() < X)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // top left corner, case 4
  else if (point.x() < -sizeX && point.y() > sizeY)
  {
    // use circle for top left corner
    double distance = (point - Vec2d(-sizeX, sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // bottom left corner, case 5
  else if (point.x() < -sizeX && point.y() < -sizeY)
  {
    // use circle for bottom left corner
    double distance = (point - Vec2d(-sizeX, -sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // middle left edge, case 6
  else if (point.x() < -sizeX)
  {
    // within rectangle between inner rectangle and left edge
    if (point.x() > -X)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // top edge, case 7
  else if (point.y() > sizeY)
  {
    // within rectangle between inner rectangle and top edge
    if (point.y() < Y)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // bottom edge, case 8
  else if (point.y() < -sizeY)
  {
    // within rectangle between inner rectangle and bottom edge
    if (point.y() > -Y)
    {
      return 1.0f;
    }
    else
    {
      return 0.0f;
    }
  }
  // middle square, case 9
  else
  {
    // inside all of the edges, automatically 1.
    return 1.0f;
  }
}

/// <summary>
/// Detect one of 9 cases in solving for rounded rectangles. One of 4 rounded corners, one of 4 edge rectangles, 1 center rectangle.
/// Transition defines a boundry around the rectangle where values range between 1 to 0. Used for smoothing the edges.
/// </summary>
float SolveRoundedRectangleCasesWithTransition(Vec2d point, double X, double Y, double radius, double transition)
{
  // represents the 9 cases.
  //[4][7][1]
  //[6][9][3]
  //[5][8][2]

  // subtract by radius to account for the area between the quarter circles.
  double sizeX = X - radius;
  double sizeY = Y - radius;

  // top right corner, case 1
  if (point.x() > sizeX && point.y() > sizeY)
  {
    // use circle for top right corner
    double distance = (point - Vec2d(sizeX, sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    // account for circle + transition bound, returns a value between 0 and 1
    else if (distance < radius + transition)
    {
      return static_cast<float>(1.0f - (distance - radius) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // bottom right corner, case 2
  else if (point.x() > sizeX && point.y() < -sizeY)
  {
    // use circle for bottom right corner
    double distance = (point - Vec2d(sizeX, -sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    // account for circle + transition bound, returns a value between 0 and 1
    else if (distance < radius + transition)
    {
      return static_cast<float>(1.0f - (distance - radius) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // middle right edge, case 3
  else if (point.x() > sizeX)
  {
    // within rectangle between inner rectangle and right edge
    if (point.x() < X)
    {
      return 1.0f;
    }
    // account for edge + transition bound, returns a value between 0 and 1
    else if (point.x() < X + transition)
    {
      return static_cast<float>(1.0f - (point.x() - X) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // top left corner, case 4
  else if (point.x() < -sizeX && point.y() > sizeY)
  {
    // use circle for top left corner
    double distance = (point - Vec2d(-sizeX, sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    // account for circle + transition bound, returns a value between 0 and 1
    else if (distance < radius + transition)
    {
      return static_cast<float>(1.0f - (distance - radius) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // bottom left corner, case 5
  else if (point.x() < -sizeX && point.y() < -sizeY)
  {
    // use circle for bottom left corner
    double distance = (point - Vec2d(-sizeX, -sizeY)).norm();
    if (distance < radius)
    {
      return 1.0f;
    }
    // account for circle + transition bound, returns a value between 0 and 1
    else if (distance < radius + transition)
    {
      return static_cast<float>(1.0f - (distance - radius) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // middle left edge, case 6
  else if (point.x() < -sizeX)
  {
    // within rectangle between inner rectangle and left edge
    if (point.x() > -X)
    {
      return 1.0f;
    }
    // account for edge + transition bound, returns a value between 0 and 1
    else if (point.x() > -X - transition)
    {
      return static_cast<float>(1.0f + (point.x() + X) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // top edge, case 7
  else if (point.y() > sizeY)
  {
    // within rectangle between inner rectangle and top edge
    if (point.y() < Y)
    {
      return 1.0f;
    }
    // account for edge + transition bound, returns a value between 0 and 1
    else if (point.y() < Y + transition)
    {
      return static_cast<float>(1.0f - (point.y() - Y) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // bottom edge, case 8
  else if (point.y() < -sizeY)
  {
    // within rectangle between inner rectangle and bottom edge
    if (point.y() > -Y)
    {
      return 1.0f;
    }
    // account for edge + transition bound, returns a value between 0 and 1
    else if (point.y() > -Y - transition)
    {
      return static_cast<float>(1.0f + (point.y() + Y) / transition);
    }
    else
    {
      return 0.0f;
    }
  }
  // middle square, case 9
  else
  {
    // inside all of the edges, automatically 1.
    return 1.0f;
  }
}

float CCigiEnvironmentalRegion::IntersectionTest(const SGeodeticCoordinates& query)
{
  GeocentricCoordinates geocentric = ConvertCigiGeodeticToWorldCoordinates(query);

  ReferencePlaneCoordinates referencePlane;

  // if there is an active database projection, the geocentric coordinates are already relative to the origin,
  // so just convert to reference plane coordinates. If not, convert geocentric to reference plane using the origin.
  if (HasActiveDatabaseProjection())
  {
    referencePlane = ReferencePlaneCoordinates(geocentric.toVec3() - GetOrigin().toVec3());
  }
  else
  {
    referencePlane = ConvertGeocentricToReferencePlaneCoordinates(geocentric, m_Origin);
  }

  Vec2d point(referencePlane.toVec3().x(), referencePlane.toVec3().y());

  // rotate point to align with rotation of rectangle
  Eigen::Rotation2D<double> rotation(DegreesToRadians(Degrees(m_Rotation)).Value());
  point = rotation * point;

  // Switch between solving rounded rectangle and rounded rectangle with transitions
  if (m_TransitionPerimeter > 0)
  {
    return SolveRoundedRectangleCasesWithTransition(point, m_SizeX, m_SizeY, m_CornerRadius, m_TransitionPerimeter);
  }
  else
  {
    return SolveRoundedRectangleCases(point, m_SizeX, m_SizeY, m_CornerRadius);
  }
}

void CCigiEnvironmentalRegion::SetWeatherData(SSetWeatherMessage& data, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition,
                                              RegionalLayeredWeatherID layerID) const
{
  data.Scope = m_Scope;
  data.RegionID = m_RegionID;
  data.EntityID = m_EntityID;
  data.AirTemperature = condition.fAirTemperature;
  data.BarometricPressure = condition.fBarometricPressure;
  data.AerosolConcentration = condition.fAerosolConcentration;
  data.BaseElevation = spatialWeatherCondition.fBaseElevation;
  data.BottomScudEnabled = condition.bBottomScudEnabled;
  data.BottomScudFrequency = condition.bottomScudFrequency.Value();
  data.CloudType = condition.cloudType.Value();
  data.Coverage = condition.coverage.Value();
  data.HorizontalWindSpeed = condition.HorizontalWindSpeed;
  data.VerticalWindSpeed = condition.VerticalWindSpeed;
  data.WindDirection = static_cast<float>(condition.WindDirection.Value());
  data.Humidity = condition.humidity.Value();
  data.RandomLightningEnabled = condition.bRandomLightningEnabled;
  data.RandomWindsEnabled = condition.bRandomWindsEnabled;
  data.Severity = condition.severity;
  data.TopScudEnabled = condition.bTopScudEnabled;
  data.TopScudFrequency = condition.topScudFrequency.Value();
  data.TopTransitionBandThickness = spatialWeatherCondition.fTopTransitionBandThickness;
  data.BottomTransitionBandThickness = spatialWeatherCondition.fBottomTransitionBandThickness;
  data.VerticalThickness = spatialWeatherCondition.fThickness;
  data.VisibilityRange = condition.fVisibilityRange;
  data.WeatherEnabled = condition.bWeatherEnabled;
  data.LayerID = layerID.Value();
}

void CCigiEnvironmentalRegion::SetWeatherCondition(const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  RegionalLayeredWeatherID regionaLayerWeatherID = RegionalLayeredWeatherID(0);// for entities
  auto& pLayer = m_WeatherLayers[regionaLayerWeatherID];

  // if not exist, create new layer
  if (!pLayer)
  {
    pLayer = std::make_unique<CCigiWeatherLayer>();
  }

  pLayer->SetWeatherCondition(condition);

  m_LastWeatherLayer = pLayer.get();

  SSetWeatherMessage data;
  SetWeatherData(data, condition, spatialWeatherCondition, regionaLayerWeatherID);
  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetWeatherMessage(data);
  }
}

void CCigiEnvironmentalRegion::SetWeatherCondition(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition,
                                                   const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  auto& pLayer = m_WeatherLayers[layerID];

  // if not exist, create new layer
  if (pLayer == nullptr)
  {
    pLayer = std::make_unique<CCigiWeatherLayer>();
  }

  pLayer->SetWeatherCondition(condition);
  pLayer->SetSpatialWeatherCondition(spatialWeatherCondition);
  m_LastWeatherLayer = pLayer.get();

  SSetWeatherMessage data;
  SetWeatherData(data, condition, spatialWeatherCondition, layerID);
  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetWeatherMessage(data);
  }
}

void CCigiEnvironmentalRegion::SetMergeWeather(EMergeState eMergeState)
{
  m_eMergeWeather = eMergeState;
}

void CCigiEnvironmentalRegion::SetMergeAerosol(EMergeState eMergeState)
{
  m_eMergeAerosol = eMergeState;
}

void CCigiEnvironmentalRegion::SetMergeMaritime(EMergeState eMergeState)
{
  m_eMergeMaritime = eMergeState;
}

void CCigiEnvironmentalRegion::SetMergeTerrestrial(EMergeState eMergeState)
{
  m_eMergeTerrestrial = eMergeState;
}

EMergeState CCigiEnvironmentalRegion::GetMergeWeather() const
{
  return m_eMergeWeather;
}

EMergeState CCigiEnvironmentalRegion::GetMergeAerosol() const
{
  return m_eMergeAerosol;
}

EMergeState CCigiEnvironmentalRegion::GetMergeMaritime() const
{
  return m_eMergeMaritime;
}

EMergeState CCigiEnvironmentalRegion::GetMergeTerrestrial() const
{
  return m_eMergeTerrestrial;
}

void CCigiEnvironmentalRegion::SetUpdateSequence(uint64_t updateSequence)
{
  m_UpdateSequence = updateSequence;
}

uint64_t CCigiEnvironmentalRegion::GetUpdateSequence() const
{
  return m_UpdateSequence;
}

void CCigiEnvironmentalRegion::AddWeatherLayer(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition,
                                               const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  auto& pLayer = m_WeatherLayers[layerID];

  // if did not find, create new layer
  if (pLayer == nullptr)
  {
    pLayer = std::make_unique<CCigiWeatherLayer>();
  }

  pLayer->SetWeatherCondition(condition);
  pLayer->SetSpatialWeatherCondition(spatialWeatherCondition);

  SSetWeatherMessage data;
  SetWeatherData(data, condition, spatialWeatherCondition, layerID);
  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetWeatherMessage(data);
  }
}

void CCigiEnvironmentalRegion::RemoveWeatherLayer(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition)
{
  TRegionalWeatherLayers::iterator it = m_WeatherLayers.find(layerID);

  // if did not find, do nothing
  if (it == m_WeatherLayers.end())
  {
    return;
  }

  if (m_LastWeatherLayer == it->second.get())
  {
    m_LastWeatherLayer = nullptr;
  }
  m_WeatherLayers.erase(layerID);
}

std::map<uint8_t, CCigiEnvironmentalRegion::SAerosolLayerContribution> CCigiEnvironmentalRegion::QueryAerosolsAtAltitude(sbio::math::HeightRelativeToWGS84Ellipsoid altitude)
{
  std::map<uint8_t, SAerosolLayerContribution> result;
  for (const auto& layer : m_WeatherLayers)
  {
    if (!layer.second->GetActive())
    {
      continue;
    }

    const float weight = static_cast<float>(layer.second->IntersectionTest(altitude));
    if (!std::isfinite(weight) || weight <= 0)
    {
      continue;
    }

    auto& contribution = result[static_cast<uint8_t>(layer.first.Value())];
    contribution.concentration = layer.second->GetWeatherCondition().fAerosolConcentration;
    contribution.weight = weight;
  }
  return result;
}

float CCigiEnvironmentalRegion::QueryWeatherAtAltitude(sbio::math::HeightRelativeToWGS84Ellipsoid altitude, SCigiWeatherCondition& out, bool& used)
{
  used = false;
  float sum = 0;
  double windDirectionX = 0;
  double windDirectionY = 0;
  SCigiWeatherCondition sumCondition;

  // for each weather layer
  for (TRegionalWeatherLayers::iterator it = m_WeatherLayers.begin(); it != m_WeatherLayers.end(); ++it)
  {
    // if the layer is active, get its contribution at the given altitude
    if (it->second->GetActive())
    {
      // get the contribution of this layer at the given altitude
      CCigiWeatherLayer& weatherLayer = *it->second;

      // if the contribution is not finite or less than or equal to 0, skip this layer
      const float contribution = static_cast<float>(weatherLayer.IntersectionTest(altitude));
      if (!std::isfinite(contribution) || contribution <= 0.0f)
      {
        continue;
      }

      // get the weather condition for this layer and scale it by the contribution
      SCigiWeatherCondition condition = weatherLayer.GetWeatherCondition();
      SCigiWeatherCondition scaledCondition = condition.Scale(contribution);
      sumCondition = used ? SCigiWeatherCondition::Sum(sumCondition, scaledCondition) : scaledCondition;
      used = true;
      sum += contribution;

      // calculate wind direction vector sum
      if (condition.WindDirection.CheckValid())
      {
        const Radians direction = DegreesToRadians(condition.WindDirection);
        windDirectionX += std::cos(direction.Value()) * contribution;
        windDirectionY += std::sin(direction.Value()) * contribution;
      }
    }
  }

  out = sumCondition;
  out.WindDirection = UnknownDegrees360;

  // divide by sum of contributions
  if (sum > 0)
  {
    out.fAerosolConcentration /= sum;
    out.fAirTemperature /= sum;
    out.fBarometricPressure /= sum;
    out.humidity /= sum;
    out.fVisibilityRange /= sum;
    out.coverage /= sum;
    out.bottomScudFrequency /= sum;
    out.topScudFrequency /= sum;
    out.VerticalWindSpeed /= sum;
    out.HorizontalWindSpeed /= sum;

    // calculate wind direction from the weighted average of the wind direction vectors if the magnitude is significant
    if (std::hypot(windDirectionX, windDirectionY) > 0.000001 * sum)
    {
      const double degrees = RadiansToDegrees(Radians(std::atan2(windDirectionY, windDirectionX))).Value();
      out.WindDirection = Degrees360(degrees < 0.0 ? degrees + 360.0 : degrees);
    }
  }
  return std::min(1.0f, sum);
}

void CCigiEnvironmentalRegion::QueryMaritimeSurface(SCigiMaritimeSurfaceCondition& out, bool& used)
{
  out = m_MaritimeSurfaceCondition.GetCondition();
  used = m_MaritimeSurfaceCondition.IsActive();
}

void CCigiEnvironmentalRegion::SetTerrestrialSurface(const SCigiTerrestrialSurfaceCondition& condition, bool used)
{
  m_TerrestrialSurfaceCondition.SetCondition(condition);
  m_TerrestrialSurfaceCondition.SetActive(used);
}

void CCigiEnvironmentalRegion::QueryTerrestrialSurface(SCigiTerrestrialSurfaceCondition& out, bool& used)
{
  out = m_TerrestrialSurfaceCondition.GetCondition();
  used = m_TerrestrialSurfaceCondition.IsActive();
}

void CCigiEnvironmentalRegion::AddWave(RegionalWaveID waveID, const SCigiWaveCondition& condition)
{
  auto& layer = m_WaveLayers[waveID];

  // if did not find, create new layer
  if (layer == nullptr)
  {
    layer = std::make_unique<CCigiWaveLayer>();
  }

  layer->SetCondition(condition);
}

void CCigiEnvironmentalRegion::RemoveWave(RegionalWaveID waveID)
{
  auto it = m_WaveLayers.find(waveID);

  // found wave
  if (it != m_WaveLayers.end())
  {
    m_WaveLayers.erase(waveID);
  }
}

void CCigiEnvironmentalRegion::QueryWave(TWaveResult& out, bool& used)
{
  TWaveResult result;

  // add regional wave effects
  for (const auto& pair : m_WaveLayers)
  {
    RegionalWaveID waveID = pair.first;
    auto it = result.find(waveID);

    // couldn't find wave id, new wave, add to result
    if (it == result.end())
    {
      result[waveID] = pair.second.get();
    }
  }

  out = result;
  used = result.size() > 0;
}

void CCigiEnvironmentalRegion::SetMaritimeSurface(const SCigiMaritimeSurfaceCondition& condition, bool used)
{
  m_MaritimeSurfaceCondition.SetCondition(condition);
  m_MaritimeSurfaceCondition.setActive(used);
}

const CCigiWeatherLayer* CCigiEnvironmentalRegion::GetLastWeatherLayer()
{
  return m_LastWeatherLayer;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
