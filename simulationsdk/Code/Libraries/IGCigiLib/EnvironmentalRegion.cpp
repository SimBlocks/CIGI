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

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::cigi::ig;
using namespace sbio::ig::atmosphere;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiEnvironmentalRegion::CCigiEnvironmentalRegion(RegionID regionID)
{
  m_regionID = regionID.Value();
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
  return std::sqrt(m_SizeX * m_SizeX + m_SizeY * m_SizeY);
}

void CCigiEnvironmentalRegion::SetDimensions(float x, float y, float radius, float transitionPerimeter)
{
  m_SizeX = x / 2;
  m_SizeY = y / 2;
  m_CornerRadius = radius;
  m_TransitionPerimeter = transitionPerimeter;
}

void CCigiEnvironmentalRegion::SetRotation(float rotation)
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

void SetWeatherData(SSetWeatherMessage& data, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition, RegionalLayeredWeatherID layerID)
{
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
  data.WindDirection = condition.WindDirection;
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
  CCigiWeatherLayer* pLayer = nullptr;

  RegionalLayeredWeatherID regionaLayerWeatherID = RegionalLayeredWeatherID(0);//for entities
  TRegionalWeatherLayers::iterator it = m_WeatherLayers.find(regionaLayerWeatherID);

  // if not exist, create new layer
  if (it == m_WeatherLayers.end())
  {
    pLayer = new CCigiWeatherLayer;
  }
  // reuse old layer
  else
  {
    pLayer = it->second;
  }

  pLayer->SetWeatherCondition(condition);

  m_WeatherLayers[regionaLayerWeatherID] = pLayer;
  m_LastWeatherLayer = m_WeatherLayers[regionaLayerWeatherID];

  SSetWeatherMessage data;
  SetWeatherData(data, condition, spatialWeatherCondition, regionaLayerWeatherID);
  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetWeatherMessage(data);
  }
}

void CCigiEnvironmentalRegion::SetWeatherCondition(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  CCigiWeatherLayer* pLayer = nullptr;

  TRegionalWeatherLayers::iterator it = m_WeatherLayers.find(layerID);

  // if not exist, create new layer
  if (it == m_WeatherLayers.end())
  {
    pLayer = new CCigiWeatherLayer;
  }
  // reuse old layer
  else
  {
    pLayer = it->second;
  }

  pLayer->SetWeatherCondition(condition);
  pLayer->SetSpatialWeatherCondition(spatialWeatherCondition);

  m_WeatherLayers[layerID] = pLayer;
  m_LastWeatherLayer = m_WeatherLayers[layerID];

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

void CCigiEnvironmentalRegion::AddWeatherLayer(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  CCigiWeatherLayer* pLayer = nullptr;

  TRegionalWeatherLayers::iterator it = m_WeatherLayers.find(layerID);

  // if did not find, create new layer
  if (it == m_WeatherLayers.end())
  {
    pLayer = new CCigiWeatherLayer;
  }
  // reuse previous layer
  else
  {
    pLayer = it->second;
  }

  pLayer->SetWeatherCondition(condition);
  pLayer->SetSpatialWeatherCondition(spatialWeatherCondition);
  m_WeatherLayers[layerID] = pLayer;

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

  delete it->second;
  m_WeatherLayers.erase(layerID);
}

void CCigiEnvironmentalRegion::QueryWeatherAtAltitude(sbio::math::HeightRelativeToWGS84Ellipsoid altitude, SCigiWeatherCondition& out, bool& used)
{
  used = false;
  float sum = 0;
  SCigiWeatherCondition sumCondition;

  // for each weather layer
  for (TRegionalWeatherLayers::iterator it = m_WeatherLayers.begin(); it != m_WeatherLayers.end(); ++it)
  {
    if (it->second->GetActive())
    {
      used = true;

      CCigiWeatherLayer& weatherLayer = *it->second;
      double contribution = weatherLayer.IntersectionTest(altitude);
      sumCondition = SCigiWeatherCondition::Sum(sumCondition, weatherLayer.GetWeatherCondition());

      sum += static_cast<float>(contribution);
    }
  }

  out = sumCondition;

  // divide by sum of contributions
  if (sum > 0)
  {
    out.fAerosolConcentration /= sum;
    out.fAirTemperature /= sum;

    out.VerticalWindSpeed /= sum;
    out.HorizontalWindSpeed /= sum;
  }
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
  CCigiWaveLayer* layer;

  auto it = m_WaveLayers.find(waveID);

  // if did not find, create new layer
  if (it == m_WaveLayers.end())
  {
    layer = new CCigiWaveLayer;
  }
  // reuse previous layer
  else
  {
    layer = it->second;
  }

  layer->SetCondition(condition);
  m_WaveLayers[waveID] = layer;
}

void CCigiEnvironmentalRegion::RemoveWave(RegionalWaveID waveID)
{
  auto it = m_WaveLayers.find(waveID);

  // found wave
  if (it != m_WaveLayers.end())
  {
    // delete memory
    delete it->second;
    m_WaveLayers.erase(waveID);
  }
}

void CCigiEnvironmentalRegion::QueryWave(TWaveResult& out, bool& used)
{
  TWaveResult result;

  // add regional wave effects
  for (auto pair : m_WaveLayers)
  {
    RegionalWaveID waveID = pair.first;
    auto it = result.find(waveID);

    // couldn't find wave id, new wave, add to result
    if (it == result.end())
    {
      result[waveID] = pair.second;
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
