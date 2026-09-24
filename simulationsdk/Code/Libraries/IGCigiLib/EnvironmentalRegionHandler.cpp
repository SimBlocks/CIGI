//Copyright SimBlocks LLC 2016-2026
#include "EnvironmentalRegionHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypesIGToHost.h"
#include "EnvironmentalRegion.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/PacketSenders.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "RegionTree.h"
#include "IGCigiLib.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include <algorithm>
#include <cmath>

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::math;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiEnvironmentalRegionHandler::CCigiEnvironmentalRegionHandler()
{
  m_Regions = std::make_unique<CCigiRegionTree>();
  m_GlobalRegion = std::make_unique<CCigiEnvironmentalRegion>();

  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->RegisterListener<IGCIGIEvent>(this);
  }
}

CCigiEnvironmentalRegionHandler::~CCigiEnvironmentalRegionHandler()
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->UnregisterListener<IGCIGIEvent>(this);
  }
}

void CCigiEnvironmentalRegionHandler::OnEntityRemoved(sbio::EntityID entityID)
{
  if (m_EnvironmentalEntities.erase(entityID) > 0 && g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
  }
}

void CCigiEnvironmentalRegionHandler::Reset()
{
  m_Regions = std::make_unique<CCigiRegionTree>();
  m_EnvironmentalEntities.clear();
  m_GlobalRegion = std::make_unique<CCigiEnvironmentalRegion>();
  m_NextRegionUpdateSequence = 1;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
  }
}

void CCigiEnvironmentalRegionHandler::AccumulateTerrestrialSurfaceCondition(std::map<uint16_t, STerrestrialSurfaceConditionAccumulator>& accumulators,
                                                                            const SCigiTerrestrialSurfaceCondition& condition, float weight)
{
  if (!condition.bEnabled || weight <= 0.0f)
  {
    return;
  }

  STerrestrialSurfaceConditionAccumulator& accumulator = accumulators[condition.surfaceConditionID.Value()];
  accumulator.totalWeight += weight;
  accumulator.weightedSeverity += condition.severity.Value() * weight;
  accumulator.weightedCoverage += condition.coverage.Value() * weight;
}

// Builds a list of merged terrestrial surface conditions by calculating weighted averages from the accumulators.
std::vector<SCigiTerrestrialSurfaceCondition> CCigiEnvironmentalRegionHandler::BuildMergedTerrestrialSurfaceConditions(
  const std::map<uint16_t, STerrestrialSurfaceConditionAccumulator>& accumulators)
{
  std::vector<SCigiTerrestrialSurfaceCondition> results;
  results.reserve(accumulators.size());

  float coverageWeight = 0.0f;

  for (const auto& entry : accumulators)
  {
    coverageWeight += entry.second.totalWeight;
  }

  // Keep uncovered space dry and preserve the relative coverage of different condition IDs.
  coverageWeight = std::max(1.0f, coverageWeight);

  for (const auto& entry : accumulators)
  {
    if (entry.second.totalWeight <= 0.0f)
    {
      continue;
    }

    SCigiTerrestrialSurfaceCondition result;
    result.bEnabled = true;
    result.surfaceConditionID = SurfaceConditionID(entry.first);
    result.severity = Percentage(entry.second.weightedSeverity / entry.second.totalWeight);
    result.coverage = Percentage(entry.second.weightedCoverage / coverageWeight);
    results.push_back(result);
  }

  return results;
}

void CCigiEnvironmentalRegionHandler::Handle(const SCigiEnvironmentalRegion& environmentalRegion)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(environmentalRegion);
  }

  // request to delete region, destroy old region and exit
  if (environmentalRegion.eRegionState == EActiveState::DESTROYED)
  {
    m_Regions->RemoveRegion(environmentalRegion.regionID);
    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
    }
    return;
  }

  CCigiEnvironmentalRegion* region = m_Regions->GetRegion(environmentalRegion.regionID);
  std::unique_ptr<CCigiEnvironmentalRegion> newRegion;
  if (region == nullptr)
  {
    newRegion = std::make_unique<CCigiEnvironmentalRegion>(environmentalRegion.regionID);
    region = newRegion.get();
  }

  // fill in values
  region->SetActive(environmentalRegion.eRegionState == EActiveState::ACTIVE);
  region->SetOrigin(environmentalRegion.latitude, environmentalRegion.longitude);

  float x = environmentalRegion.size[0];
  float y = environmentalRegion.size[1];
  float r = environmentalRegion.fCornerRadius;
  float t = environmentalRegion.fTransition;
  region->SetDimensions(x, y, r, t);

  region->SetRotation(environmentalRegion.fRotation.Value());

  // set merge flags for merging between layers
  region->SetMergeWeather(environmentalRegion.eMergeWeatherProperties);
  region->SetMergeAerosol(environmentalRegion.eMergeAerosolConcentrations);
  region->SetMergeMaritime(environmentalRegion.eMergeMaritimeSurfaceConditions);
  region->SetMergeTerrestrial(environmentalRegion.eMergeTerrestrialSurfaceConditions);
  region->SetUpdateSequence(m_NextRegionUpdateSequence++);

  if (newRegion != nullptr)
  {
    m_Regions->AddRegion(environmentalRegion.regionID, std::move(newRegion));
  }
  else
  {
    m_Regions->UpdateRegionBounds(environmentalRegion.regionID);
  }
  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
  }
}

void CCigiEnvironmentalRegionHandler::HandleGlobalTerrestrialSurfaceCondition(const SCigiTerrestrialSurfaceCondition& terrestrialSurfaceCondition)
{
  SetGlobalTerrestrialSurfaceCondition(terrestrialSurfaceCondition);
}

void CCigiEnvironmentalRegionHandler::HandleEntityTerrestrialSurfaceCondition(sbio::EntityID entityID, const SCigiTerrestrialSurfaceCondition& terrestrialSurfaceCondition)
{
  SetEntityTerrestrialSurfaceCondition(entityID, terrestrialSurfaceCondition);
}

void CCigiEnvironmentalRegionHandler::HandleRegionTerrestrialSurfaceCondition(sbio::RegionID regionID, const SCigiTerrestrialSurfaceCondition& terrestrialSurfaceCondition)
{
  SetRegionTerrestrialSurfaceCondition(regionID, terrestrialSurfaceCondition);
}

void CCigiEnvironmentalRegionHandler::Handle(const SEnvironmentalConditionsRequest& environmentalConditionsRequest)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(environmentalConditionsRequest);
  }

  if (g_CigiLibGlobals.pImageGenerator == nullptr || g_CigiLibGlobals.pImageGenerator->GetPacketSenders() == nullptr)
  {
    return;
  }

  if (environmentalConditionsRequest.bAerosolConcentrationsRequest)
  {
    const auto concentrations = QueryAerosolConcentrations(environmentalConditionsRequest.geodeticCoordinates);
    for (const auto& layer : concentrations)
    {
      SAerosolConcentrationResponse response;
      response.requestID = environmentalConditionsRequest.nRequestID;
      response.layerID = layer.first;
      response.fAerosolConcentration = layer.second;
      g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendAerosolConcentrationResponse(response);
    }
  }

  if (environmentalConditionsRequest.bMaritimeSurfaceConditionsRequest)
  {
    SCigiMaritimeSurfaceCondition condition = QueryMaritimeSurface(environmentalConditionsRequest.geodeticCoordinates);

    SMaritimeSurfaceConditionsResponse response;
    response.requestID = environmentalConditionsRequest.nRequestID;
    response.fSeaSurfaceHeight = condition.fSeaSurfaceHeight;
    response.fSurfaceWaterTemperature = condition.fSurfaceWaterTemperature;
    response.surfaceClarity = condition.surfaceClarity;
    g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendMaritimeSurfaceConditionsResponse(response);
  }

  if (environmentalConditionsRequest.bTerrestrialSurfaceConditionsRequest)
  {
    std::vector<SCigiTerrestrialSurfaceCondition> conditions = QueryTerrestrialSurface(environmentalConditionsRequest.geodeticCoordinates);

    for (const SCigiTerrestrialSurfaceCondition& condition : conditions)
    {
      STerrestrialSurfaceConditionsResponse response;
      response.requestID = environmentalConditionsRequest.nRequestID;
      response.surfaceConditionID = condition.surfaceConditionID.Value();
      g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendTerrestrialSurfaceConditionsResponse(response);
    }
  }

  if (environmentalConditionsRequest.bWeatherConditionsRequest)
  {
    SCigiWeatherCondition condition = QueryWeather(environmentalConditionsRequest.geodeticCoordinates);

    SWeatherConditionsResponse response;
    response.requestID = environmentalConditionsRequest.nRequestID;
    response.humidity = condition.humidity;
    response.fAirTemperature = condition.fAirTemperature.Value();
    response.fVisibilityRange = condition.fVisibilityRange;
    response.windSpeedHorVer.horizontalWindSpeed = condition.HorizontalWindSpeed;
    response.windSpeedHorVer.verticalWindSpeed = condition.VerticalWindSpeed;
    response.fWindDirection = condition.WindDirection.CheckValid() ? static_cast<float>(condition.WindDirection.Value()) : 0.0f;
    response.fBarometricPressure = condition.fBarometricPressure;
    g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendWeatherConditionsResponse(response);
  }
}

/// <summary>
/// Get weather conditions by querying against all active regions and returning the merged/averaged result.
/// Each region is made up of layers (up to 256 layers) at different altitudes.
/// </summary>
std::map<uint8_t, float> CCigiEnvironmentalRegionHandler::QueryAerosolConcentrations(const SGeodeticCoordinates& query)
{
  /** @brief Accumulates concentration times spatial weight and the corresponding weight for one aerosol layer. */
  struct SAccumulator
  {
    float weightedConcentration = 0;
    float weight = 0;
  };

  std::map<uint8_t, SAccumulator> accumulators;

  SGeodeticCoordinates horizontalQuery = query;
  horizontalQuery.altitude = HeightRelativeToWGS84Ellipsoid(0);
  auto regions = m_Regions->QueryRegions(ConvertCigiGeodeticToWorldCoordinates(horizontalQuery));
  std::sort(regions.begin(),
            regions.end(),
            [](const CCigiEnvironmentalRegion* left, const CCigiEnvironmentalRegion* right)
            {
              return left->GetUpdateSequence() < right->GetUpdateSequence();
            });

  for (auto* region : regions)
  {
    if (!region->IsActive())
    {
      continue;
    }
    const float horizontalWeight = region->IntersectionTest(horizontalQuery);
    if (!std::isfinite(horizontalWeight) || horizontalWeight <= 0)
    {
      continue;
    }

    for (const auto& layer : region->QueryAerosolsAtAltitude(query.altitude))
    {
      const float weight = std::min(1.0f, horizontalWeight * layer.second.weight);
      auto& accumulator = accumulators[layer.first];
      if (region->GetMergeAerosol() != EMergeState::MERGE)
      {
        // A newer region replaces this aerosol type, fading the previous value through its transition band.
        accumulator.weightedConcentration *= (1.0f - weight) / std::max(1.0f, accumulator.weight);
        accumulator.weight = std::min(1.0f, accumulator.weight) * (1.0f - weight);
      }
      accumulator.weightedConcentration += layer.second.concentration * weight;
      accumulator.weight += weight;
    }
  }

  for (const auto& layer : m_GlobalRegion->QueryAerosolsAtAltitude(query.altitude))
  {
    auto& accumulator = accumulators[layer.first];
    const float weight = std::max(0.0f, 1.0f - accumulator.weight) * layer.second.weight;
    accumulator.weightedConcentration += layer.second.concentration * weight;
    accumulator.weight += weight;
  }

  std::map<uint8_t, float> result;
  for (const auto& layer : accumulators)
  {
    // Uncovered transition weight represents aerosol-free air, rather than another aerosol layer.
    result[layer.first] = layer.second.weightedConcentration / std::max(1.0f, layer.second.weight);
  }
  return result;
}

SCigiWeatherCondition CCigiEnvironmentalRegionHandler::QueryWeather(const SGeodeticCoordinates& query)
{
  float weatherWeight = 0;
  float aerosolWeight = 0;
  float weightedAerosol = 0;
  double windDirectionX = 0;
  double windDirectionY = 0;
  SCigiWeatherCondition sumCondition;

  auto accumulateWindDirection = [&](Degrees360 directionDegrees, float weight)
  {
    // Skip invalid wind directions
    if (!directionDegrees.CheckValid())
    {
      return;
    }

    // Convert wind direction from degrees to radians for vector calculations
    Radians directionRadians = sbio::math::DegreesToRadians(directionDegrees);
    windDirectionX += std::cos(directionRadians.Value()) * weight;
    windDirectionY += std::sin(directionRadians.Value()) * weight;
  };

  // Region footprints are horizontal; altitude is evaluated by the weather layers below.
  SGeodeticCoordinates horizontalQuery = query;
  horizontalQuery.altitude = HeightRelativeToWGS84Ellipsoid(0);
  GeocentricCoordinates queryECEF = ConvertCigiGeodeticToWorldCoordinates(horizontalQuery);
  std::vector<CCigiEnvironmentalRegion*> queriedRegions = m_Regions->QueryRegions(queryECEF);

  // sort by update sequence so that newer regions take precedence over older regions
  std::sort(queriedRegions.begin(),
            queriedRegions.end(),
            [](const CCigiEnvironmentalRegion* left, const CCigiEnvironmentalRegion* right)
            {
              return left->GetUpdateSequence() < right->GetUpdateSequence();
            });

  // for each possible region
  for (std::vector<CCigiEnvironmentalRegion*>::iterator it = queriedRegions.begin(); it != queriedRegions.end(); ++it)
  {
    CCigiEnvironmentalRegion* region = *it;

    // only active regions
    if (region->IsActive())
    {
      // find amount of contribution (or amount of intersection between 0 and 1, including transition bounds)
      float contribution = region->IntersectionTest(horizontalQuery);
      if (contribution <= 0.0f)
      {
        continue;
      }

      // query layers within region
      bool used;
      SCigiWeatherCondition layersResult;
      contribution *= region->QueryWeatherAtAltitude(query.altitude, layersResult, used);

      // if intersected layers
      if (used)
      {
        if (region->GetMergeWeather() != EMergeState::MERGE)
        {
          const float remainingScale = (1.0f - contribution) / std::max(1.0f, weatherWeight);
          sumCondition = sumCondition.Scale(remainingScale);
          weatherWeight *= remainingScale;
          windDirectionX *= remainingScale;
          windDirectionY *= remainingScale;
        }

        if (region->GetMergeAerosol() != EMergeState::MERGE)
        {
          const float remainingScale = (1.0f - contribution) / std::max(1.0f, aerosolWeight);
          weightedAerosol *= remainingScale;
          aerosolWeight *= remainingScale;
        }

        // scale weather condition by contribution and add to sum of weather conditions
        SCigiWeatherCondition scaledCondition = layersResult.Scale(contribution);
        scaledCondition.fAerosolConcentration = 0;
        sumCondition = weatherWeight > 0 ? SCigiWeatherCondition::Sum(sumCondition, scaledCondition) : scaledCondition;
        weatherWeight += contribution;
        accumulateWindDirection(layersResult.WindDirection, contribution);
        weightedAerosol += layersResult.fAerosolConcentration * contribution;
        aerosolWeight += contribution;
      }
    }
  }

  // query global weather
  bool used;
  SCigiWeatherCondition globalResult;
  const float globalAltitudeWeight = m_GlobalRegion->QueryWeatherAtAltitude(query.altitude, globalResult, used);
  if (used && weatherWeight < 1.0f)
  {
    float globalContribution = (1.0f - weatherWeight) * globalAltitudeWeight;
    SCigiWeatherCondition scaledGlobalCondition = globalResult.Scale(globalContribution);
    scaledGlobalCondition.fAerosolConcentration = 0;
    sumCondition = weatherWeight > 0 ? SCigiWeatherCondition::Sum(sumCondition, scaledGlobalCondition) : scaledGlobalCondition;
    weatherWeight += globalContribution;
    accumulateWindDirection(globalResult.WindDirection, globalContribution);
  }

  if (used && aerosolWeight < 1.0f)
  {
    float globalContribution = (1.0f - aerosolWeight) * globalAltitudeWeight;
    weightedAerosol += globalResult.fAerosolConcentration * globalContribution;
    aerosolWeight += globalContribution;
  }

  SCigiWeatherCondition result = sumCondition;

  // Use north when no direction is available or opposing contributions cancel.
  result.WindDirection = Degrees360(0.0);

  // divide by sum of contributions
  if (weatherWeight > 0)
  {
    result.fAirTemperature /= weatherWeight;
    result.fBarometricPressure /= weatherWeight;
    result.humidity /= weatherWeight;
    result.fVisibilityRange /= weatherWeight;
    result.VerticalWindSpeed /= weatherWeight;
    result.HorizontalWindSpeed /= weatherWeight;
    result.coverage /= weatherWeight;
    result.bottomScudFrequency /= weatherWeight;
    result.topScudFrequency /= weatherWeight;

    // calculate wind direction from the weighted average of the wind direction vectors
    if (std::hypot(windDirectionX, windDirectionY) > 0.000001 * weatherWeight)
    {
      // calculate wind direction from the weighted average of the wind direction vectors
      Radians radians = Radians(std::atan2(windDirectionY, windDirectionX));
      Degrees windDirection = sbio::math::RadiansToDegrees(radians);

      // normalize wind direction to be in the range [0, 360)
      const double normalizedDegrees = windDirection.Value() < 0.0 ? windDirection.Value() + 360.0 : windDirection.Value();

      result.WindDirection = Degrees360(normalizedDegrees);
    }
  }

  if (aerosolWeight > 0)
  {
    result.fAerosolConcentration = weightedAerosol / aerosolWeight;
  }
  return result;
}

SCigiMaritimeSurfaceCondition CCigiEnvironmentalRegionHandler::QueryMaritimeSurface(const SGeodeticCoordinates& query)
{
  float sum = 0;

  SCigiMaritimeSurfaceCondition sumCondition;

  // for each region, query into BVH tree for a set of possible intersecting regions
  SGeodeticCoordinates horizontalQuery = query;
  horizontalQuery.altitude = HeightRelativeToWGS84Ellipsoid(0);
  GeocentricCoordinates queryECEF = ConvertCigiGeodeticToWorldCoordinates(horizontalQuery);
  std::vector<CCigiEnvironmentalRegion*> queriedRegions = m_Regions->QueryRegions(queryECEF);

  // sort by update sequence so that newer regions are processed last
  std::sort(queriedRegions.begin(),
            queriedRegions.end(),
            [](const CCigiEnvironmentalRegion* left, const CCigiEnvironmentalRegion* right)
            {
              return left->GetUpdateSequence() < right->GetUpdateSequence();
            });

  // foreach possible intersecting region
  for (std::vector<CCigiEnvironmentalRegion*>::iterator it = queriedRegions.begin(); it != queriedRegions.end(); ++it)
  {
    CCigiEnvironmentalRegion* region = *it;

    // only active regions
    if (region->IsActive())
    {
      // find contribution amount (0 to 1, including transition bounds)
      float contribution = region->IntersectionTest(horizontalQuery);
      if (contribution <= 0.0f)
      {
        continue;
      }

      // query maritime surface within region
      bool used;
      SCigiMaritimeSurfaceCondition regionalResult;
      region->QueryMaritimeSurface(regionalResult, used);

      // if has maritime surface
      if (used)
      {
        if (region->GetMergeMaritime() != EMergeState::MERGE)
        {
          const float remainingScale = (1.0f - contribution) / std::max(1.0f, sum);
          sumCondition = sumCondition.Scale(remainingScale);
          sum *= remainingScale;
        }

        // scale by contribution and then sum into merged result
        SCigiMaritimeSurfaceCondition scaledCondition = regionalResult.Scale(contribution);
        sumCondition = sum > 0 ? SCigiMaritimeSurfaceCondition::Sum(sumCondition, scaledCondition) : scaledCondition;
        sum += contribution;
      }
    }
  }

  // query global maritime surface
  bool used;
  SCigiMaritimeSurfaceCondition globalResult;
  m_GlobalRegion->QueryMaritimeSurface(globalResult, used);

  // if global maritime surface is used and there is remaining contribution space, add the global maritime surface to the sum
  if (used && sum < 1.0f)
  {
    float globalContribution = 1.0f - sum;
    SCigiMaritimeSurfaceCondition scaledCondition = globalResult.Scale(globalContribution);
    sumCondition = sum > 0 ? SCigiMaritimeSurfaceCondition::Sum(sumCondition, scaledCondition) : scaledCondition;
    sum += globalContribution;
  }

  SCigiMaritimeSurfaceCondition result = sumCondition;

  // divide by sum of contributions
  if (sum > 0)
  {
    result.fSeaSurfaceHeight /= sum;
    result.fSurfaceWaterTemperature /= sum;
    result.surfaceClarity /= sum;
  }

  return result;
}

std::vector<SCigiTerrestrialSurfaceCondition> CCigiEnvironmentalRegionHandler::QueryTerrestrialSurface(const SGeodeticCoordinates& queryPos)
{
  std::map<uint16_t, STerrestrialSurfaceConditionAccumulator> entityAccumulators;
  GeocentricCoordinates queryPosGeocentric = ConvertCigiGeodeticToWorldCoordinates(queryPos);

  for (TEnvironmentalRegionEntities::const_iterator it = m_EnvironmentalEntities.begin(); it != m_EnvironmentalEntities.end(); ++it)
  {
    if (g_CigiLibGlobals.pEntityManager == nullptr)
    {
      break;
    }

    sbio::entity::CEntity* pEntity = g_CigiLibGlobals.pEntityManager->GetEntity(it->first);
    if (pEntity == nullptr)
    {
      continue;
    }

    if (g_CigiLibGlobals.pEventMessenger == nullptr || !g_CigiLibGlobals.pEventMessenger->IsPointInEntityVolume(queryPosGeocentric, pEntity->GetEntityID()))
    {
      continue;
    }

    bool used = false;
    SCigiTerrestrialSurfaceCondition entityResult;
    it->second->QueryTerrestrialSurface(entityResult, used);

    if (!used || !entityResult.bEnabled)
    {
      continue;
    }

    AccumulateTerrestrialSurfaceCondition(entityAccumulators, entityResult, 1.0f);
  }

  if (!entityAccumulators.empty())
  {
    return BuildMergedTerrestrialSurfaceConditions(entityAccumulators);
  }

  // for each region, query into BVH tree for a set of possible intersecting regions
  SGeodeticCoordinates horizontalQuery = queryPos;
  horizontalQuery.altitude = HeightRelativeToWGS84Ellipsoid(0);
  GeocentricCoordinates horizontalQueryGeocentric = ConvertCigiGeodeticToWorldCoordinates(horizontalQuery);
  std::vector<CCigiEnvironmentalRegion*> queriedRegions = m_Regions->QueryRegions(horizontalQueryGeocentric);

  // sort by update sequence so that newer regions take precedence over older regions
  std::sort(queriedRegions.begin(),
            queriedRegions.end(),
            [](const CCigiEnvironmentalRegion* left, const CCigiEnvironmentalRegion* right)
            {
              return left->GetUpdateSequence() < right->GetUpdateSequence();
            });

  float summedContribution = 0;
  std::map<uint16_t, STerrestrialSurfaceConditionAccumulator> regionalAccumulators;

  // foreach possible intersecting region
  for (std::vector<CCigiEnvironmentalRegion*>::iterator it = queriedRegions.begin(); it != queriedRegions.end(); ++it)
  {
    CCigiEnvironmentalRegion* region = *it;

    // only active regions
    if (region->IsActive())
    {
      // find contribution amount (0 to 1, including transition bounds)
      float contribution = region->IntersectionTest(horizontalQuery);

      if (contribution <= 0.0f)
      {
        continue;
      }

      // query terrestrial surface within region
      bool used;
      SCigiTerrestrialSurfaceCondition regionalResult;
      region->QueryTerrestrialSurface(regionalResult, used);

      // enabled regional terrestrial conditions take precedence over global conditions
      if (used && regionalResult.bEnabled)
      {
        if (region->GetMergeTerrestrial() != EMergeState::MERGE)
        {
          const float remainingScale = (1.0f - contribution) / std::max(1.0f, summedContribution);
          for (auto& entry : regionalAccumulators)
          {
            entry.second.totalWeight *= remainingScale;
            entry.second.weightedSeverity *= remainingScale;
            entry.second.weightedCoverage *= remainingScale;
          }
          summedContribution *= remainingScale;
        }

        summedContribution += contribution;
        AccumulateTerrestrialSurfaceCondition(regionalAccumulators, regionalResult, contribution);
      }
    }
  }

  // query global terrestrial surface. It only contributes where there is remaining
  // transition-space not already claimed by enabled regional terrestrial conditions.
  bool used;
  SCigiTerrestrialSurfaceCondition globalResult;
  m_GlobalRegion->QueryTerrestrialSurface(globalResult, used);

  if (used && globalResult.bEnabled && summedContribution < 1.0f)
  {
    float globalContribution = 1.0f - summedContribution;
    AccumulateTerrestrialSurfaceCondition(regionalAccumulators, globalResult, globalContribution);
    summedContribution += globalContribution;
  }

  if (summedContribution > 0.0f)
  {
    return BuildMergedTerrestrialSurfaceConditions(regionalAccumulators);
  }

  if (used)
  {
    std::vector<SCigiTerrestrialSurfaceCondition> results;
    if (globalResult.bEnabled)
    {
      results.push_back(globalResult);
    }

    return results;
  }

  return std::vector<SCigiTerrestrialSurfaceCondition>();
}

void CCigiEnvironmentalRegionHandler::SetEntityWeatherCondition(EntityID entityID, const SCigiWeatherCondition& condition,
                                                                const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  CCigiEnvironmentalRegion* region = nullptr;

  // lookup entity by entity ID from entity manager
  auto itEntity = g_CigiLibGlobals.pEntityManager->GetEntity(entityID);

  if (itEntity == nullptr)
  {
    stringstream ss;
    ss << "No Entity for Entity ID " << entityID << ". Can't set weather on entity." << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  // try and find entity in table
  auto it = m_EnvironmentalEntities.find(entityID);

  // couldn't find, make a new entry
  if (it == m_EnvironmentalEntities.end())
  {
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(entityID);
    m_EnvironmentalEntities[entityID] = std::move(pRegion);
    region = m_EnvironmentalEntities[entityID].get();
  }
  // found it, reuse old entry
  else
  {
    region = it->second.get();
  }

  // set conditions for entity
  region->SetWeatherCondition(condition, spatialWeatherCondition);
}

void CCigiEnvironmentalRegionHandler::SetRegionalWeatherCondition(RegionID regionID, RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition,
                                                                  const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  CCigiEnvironmentalRegion* region = m_Regions->GetRegion(regionID);

  // Region doesn't exist, cannot apply weather to it
  if (region == nullptr)
  {
    stringstream ss;
    ss << "No Region for Region ID " << regionID << ". Can't set weather on region." << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  region->AddWeatherLayer(layerID, condition, spatialWeatherCondition);
}

void CCigiEnvironmentalRegionHandler::SetGlobalWeatherCondition(GlobalLayeredWeatherID layerID, const SCigiWeatherCondition& condition,
                                                                const SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  // reuse regional weather for global weather
  m_GlobalRegion->SetWeatherCondition(RegionalLayeredWeatherID(layerID.Value()), condition, spatialWeatherCondition);
}

void CCigiEnvironmentalRegionHandler::SetEntityMaritimeSurfaceCondition(EntityID entityID, const SCigiMaritimeSurfaceCondition& condition)
{
  CCigiEnvironmentalRegion* region = nullptr;

  // lookup entity by entity ID from entity manager
  auto itEntity = g_CigiLibGlobals.pEntityManager->GetEntity(entityID);

  if (itEntity == nullptr)
  {
    stringstream ss;
    ss << "No Entity for Entity ID " << entityID << ". Can't set maritime surface condition on entity." << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  // try and find entity in table
  auto it = m_EnvironmentalEntities.find(entityID);

  // couldn't find, make a new entry
  if (it == m_EnvironmentalEntities.end())
  {
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(entityID);
    m_EnvironmentalEntities[entityID] = std::move(pRegion);
    region = m_EnvironmentalEntities[entityID].get();
  }
  // found it, reuse old entry
  else
  {
    region = it->second.get();
  }

  // set conditions for entity
  region->SetMaritimeSurface(condition, condition.bActive);
}

void CCigiEnvironmentalRegionHandler::SetRegionMaritimeSurfaceCondition(sbio::RegionID regionID, const SCigiMaritimeSurfaceCondition& condition)
{
  CCigiEnvironmentalRegion* region = m_Regions->GetRegion(regionID);

  // Region doesn't exist, cannot apply weather to it
  if (region == nullptr)
  {
    return;
  }

  region->SetMaritimeSurface(condition, condition.bActive);
}

void CCigiEnvironmentalRegionHandler::SetGlobalMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& condition)
{
  m_GlobalRegion->SetMaritimeSurface(condition, condition.bActive);
}

void CCigiEnvironmentalRegionHandler::SetEntityTerrestrialSurfaceCondition(sbio::EntityID entityID, const SCigiTerrestrialSurfaceCondition& condition)
{
  if (condition.IsDry())
  {
    auto it = m_EnvironmentalEntities.find(entityID);
    if (it != m_EnvironmentalEntities.end())
    {
      it->second->SetTerrestrialSurface(condition, false);
      g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
    }

    return;
  }

  CCigiEnvironmentalRegion* region = nullptr;

  // lookup entity by entity ID from entity manager
  auto itEntity = g_CigiLibGlobals.pEntityManager->GetEntity(entityID);

  if (itEntity == nullptr)
  {
    stringstream ss;
    ss << "No Entity for Entity ID " << entityID << ". Can't set terrestrial surface condition on entity." << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  // try and find entity in table
  auto it = m_EnvironmentalEntities.find(entityID);

  // couldn't find, make a new entry
  if (it == m_EnvironmentalEntities.end())
  {
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(entityID);
    m_EnvironmentalEntities[entityID] = std::move(pRegion);

    region = m_EnvironmentalEntities[entityID].get();
  }
  // found it, reuse old entry
  else
  {
    region = it->second.get();
  }

  // set conditions for entity
  region->SetTerrestrialSurface(static_cast<const SCigiTerrestrialSurfaceCondition&>(condition), true);

  g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
}

void CCigiEnvironmentalRegionHandler::SetRegionTerrestrialSurfaceCondition(sbio::RegionID regionID, const SCigiTerrestrialSurfaceCondition& condition)
{
  CCigiEnvironmentalRegion* region = m_Regions->GetRegion(regionID);

  // Region doesn't exist, cannot apply weather to it
  if (region == nullptr)
  {
    return;
  }

  if (condition.IsDry())
  {
    region->SetTerrestrialSurface(condition, false);
    g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
    return;
  }

  region->SetTerrestrialSurface(static_cast<const SCigiTerrestrialSurfaceCondition&>(condition), true);

  g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
}

void CCigiEnvironmentalRegionHandler::SetGlobalTerrestrialSurfaceCondition(const SCigiTerrestrialSurfaceCondition& condition)
{
  if (condition.IsDry())
  {
    m_GlobalRegion->SetTerrestrialSurface(condition, false);
    g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
    return;
  }

  m_GlobalRegion->SetTerrestrialSurface(static_cast<const SCigiTerrestrialSurfaceCondition&>(condition), true);

  g_CigiLibGlobals.pEventMessenger->SendTerrestrialSurfaceConditionsChangedMessage();
}

void CCigiEnvironmentalRegionHandler::SetEntityWaveCondition(EntityID entityID, EntityWaveID waveID, const SCigiWaveCondition& condition)
{
  CCigiEnvironmentalRegion* region = nullptr;

  // lookup entity by entity ID from entity manager
  auto itEntity = g_CigiLibGlobals.pEntityManager->GetEntity(entityID);

  if (itEntity == nullptr)
  {
    stringstream ss;
    ss << "No Entity for Entity ID " << entityID << ". Can't set wave condition on entity." << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  // try and find entity in table
  auto it = m_EnvironmentalEntities.find(entityID);

  // couldn't find, make a new entry
  if (it == m_EnvironmentalEntities.end())
  {
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(entityID);
    m_EnvironmentalEntities[entityID] = std::move(pRegion);
    region = m_EnvironmentalEntities[entityID].get();
  }
  // found it, reuse old entry
  else
  {
    region = it->second.get();
  }

  // set conditions for entity
  region->AddWave(RegionalWaveID(waveID.Value()), condition);
}

void CCigiEnvironmentalRegionHandler::SetRegionalWaveCondition(RegionID regionID, RegionalWaveID waveID, const SCigiWaveCondition& condition)
{
  CCigiEnvironmentalRegion* region = m_Regions->GetRegion(RegionID(regionID.Value()));

  // Region doesn't exist, cannot apply weather to it
  if (region == nullptr)
  {
    return;
  }

  region->AddWave(waveID, condition);
}

void CCigiEnvironmentalRegionHandler::SetGlobalWaveCondition(GlobalWaveID waveID, const SCigiWaveCondition& condition)
{
  m_GlobalRegion->AddWave(RegionalWaveID(waveID.Value()), condition);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
