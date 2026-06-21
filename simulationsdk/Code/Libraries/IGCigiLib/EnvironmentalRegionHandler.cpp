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
#include "UtilitiesLib/Logger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::math;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiEnvironmentalRegionHandler::CCigiEnvironmentalRegionHandler()
{
  m_Regions = std::make_unique<CCigiRegionTree>();
  m_GlobalRegion = std::make_unique<CCigiEnvironmentalRegion>(RegionID(0));
}

CCigiEnvironmentalRegionHandler::~CCigiEnvironmentalRegionHandler()
{
}

void CCigiEnvironmentalRegionHandler::AccumulateTerrestrialSurfaceCondition(std::map<uint16_t, STerrestrialSurfaceConditionAccumulator>& accumulators, const SCigiTerrestrialSurfaceCondition& condition, float weight)
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
std::vector<SCigiTerrestrialSurfaceCondition> CCigiEnvironmentalRegionHandler::BuildMergedTerrestrialSurfaceConditions(const std::map<uint16_t, STerrestrialSurfaceConditionAccumulator>& accumulators)
{
  std::vector<SCigiTerrestrialSurfaceCondition> results;
  results.reserve(accumulators.size());

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
    result.coverage = Percentage(entry.second.weightedCoverage / entry.second.totalWeight);
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
    return;
  }

  CCigiEnvironmentalRegion* region = new CCigiEnvironmentalRegion(environmentalRegion.regionID);

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

  m_Regions->AddRegion(environmentalRegion.regionID, region);
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
    SCigiWeatherCondition condition = QueryWeather(environmentalConditionsRequest.geodeticCoordinates);

    SAerosolConcentrationResponse response;
    response.requestID = environmentalConditionsRequest.nRequestID;
    response.layerID = 0;
    response.fAerosolConcentration = condition.fAerosolConcentration;
    g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendAerosolConcentrationResponse(response);
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
    response.fWindDirection = condition.WindDirection;
    response.fBarometricPressure = condition.fBarometricPressure;
    g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendWeatherConditionsResponse(response);
  }
}

/// <summary>
/// Get weather conditions by querying against all active regions and returning the merged/averaged result.
/// Each region is made up of layers (up to 256 layers) at different altitudes.
/// </summary>
SCigiWeatherCondition CCigiEnvironmentalRegionHandler::QueryWeather(const SGeodeticCoordinates& query)
{
  float sum = 0;

  SCigiWeatherCondition sumCondition;

  // get a list of possible intersecting regions with BVH tree
  GeocentricCoordinates queryECEF = ConvertCigiGeodeticToWorldCoordinates(query);
  std::vector<CCigiEnvironmentalRegion*> queriedRegions = m_Regions->QueryRegions(queryECEF);

  // for each possible region
  for (std::vector<CCigiEnvironmentalRegion*>::iterator it = queriedRegions.begin(); it != queriedRegions.end(); ++it)
  {
    CCigiEnvironmentalRegion* region = *it;

    // only active regions
    if (region->IsActive())
    {
      // find amount of contribution (or amount of intersection between 0 and 1, including transition bounds)
      float contribution = region->IntersectionTest(query);

      // query layers within region
      bool used;
      SCigiWeatherCondition layersResult;
      region->QueryWeatherAtAltitude(query.altitude, layersResult, used);

      // if intersected layers
      if (used)
      {
        // scale weather condition by contribution and add to sum of weather conditions
        sumCondition = SCigiWeatherCondition::Sum(sumCondition, layersResult.Scale(contribution));
        sum += contribution;
      }
    }
  }

  // query global weather
  bool used;
  SCigiWeatherCondition globalResult;
  m_GlobalRegion->QueryWeatherAtAltitude(query.altitude, globalResult, used);
  if (used)
  {
    sumCondition = SCigiWeatherCondition::Sum(sumCondition, globalResult);
  }

  SCigiWeatherCondition result = sumCondition;

  // divide by sum of contributions
  if (sum > 0)
  {
    result.fAerosolConcentration /= sum;
    result.fAirTemperature /= sum;

    result.VerticalWindSpeed /= sum;
    result.HorizontalWindSpeed /= sum;
  }
  return result;
}

SCigiMaritimeSurfaceCondition CCigiEnvironmentalRegionHandler::QueryMaritimeSurface(const SGeodeticCoordinates& query)
{
  float sum = 0;

  SCigiMaritimeSurfaceCondition sumCondition;

  // for each region, query into BVH tree for a set of possib le intersecting regions
  GeocentricCoordinates queryECEF = ConvertCigiGeodeticToWorldCoordinates(query);
  std::vector<CCigiEnvironmentalRegion*> queriedRegions = m_Regions->QueryRegions(queryECEF);

  // foreach possible intersecting region
  for (std::vector<CCigiEnvironmentalRegion*>::iterator it = queriedRegions.begin(); it != queriedRegions.end(); ++it)
  {
    CCigiEnvironmentalRegion* region = *it;

    // only active regions
    if (region->IsActive())
    {
      // find contribution amount (0 to 1, including transition bounds)
      float contribution = region->IntersectionTest(query);

      // query maritime surface within region
      bool used;
      SCigiMaritimeSurfaceCondition regionalResult;
      region->QueryMaritimeSurface(regionalResult, used);

      // if has maritime surface
      if (used)
      {
        // scale by contribution and then sum into merged result
        sumCondition = SCigiMaritimeSurfaceCondition::Sum(sumCondition, regionalResult.Scale(contribution));
        sum += contribution;
      }
    }
  }

  // query global maritime surface
  bool used;
  SCigiMaritimeSurfaceCondition globalResult;
  m_GlobalRegion->QueryMaritimeSurface(globalResult, used);
  if (used)
  {
    sumCondition = SCigiMaritimeSurfaceCondition::Sum(sumCondition, globalResult);
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

    if (!g_CigiLibGlobals.pEventMessenger->IsPointInEntityVolume(queryPosGeocentric, pEntity->GetEntityID()))
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
  std::vector<CCigiEnvironmentalRegion*> queriedRegions = m_Regions->QueryRegions(queryPosGeocentric);

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
      float contribution = region->IntersectionTest(queryPos);

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

void CCigiEnvironmentalRegionHandler::SetEntityWeatherCondition(EntityID entityID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition)
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
    //TODO: using entity ID as region ID for now, but may want to decouple in the future to not conflict with non-entity based regions
    RegionID regionID = RegionID(entityID.Value());
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(regionID);
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

void CCigiEnvironmentalRegionHandler::SetRegionalWeatherCondition(RegionID regionID, RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition)
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

void CCigiEnvironmentalRegionHandler::SetGlobalWeatherCondition(GlobalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition)
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
    // TODO: using entity ID as region ID for now, but may want to decouple in the future to not conflict with non-entity based regions
    RegionID regionID = RegionID(entityID.Value());
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(regionID);
    m_EnvironmentalEntities[entityID] = std::move(pRegion);
    region = m_EnvironmentalEntities[entityID].get();
  }
  // found it, reuse old entry
  else
  {
    region = it->second.get();
  }

  // set conditions for entity
  region->SetMaritimeSurface(condition, true);
}

void CCigiEnvironmentalRegionHandler::SetRegionMaritimeSurfaceCondition(sbio::RegionID regionID, const SCigiMaritimeSurfaceCondition& condition)
{
  CCigiEnvironmentalRegion* region = m_Regions->GetRegion(regionID);

  // Region doesn't exist, cannot apply weather to it
  if (region == nullptr)
  {
    return;
  }

  region->SetMaritimeSurface(condition, true);
}

void CCigiEnvironmentalRegionHandler::SetGlobalMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& condition)
{
  m_GlobalRegion->SetMaritimeSurface(condition, true);
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
    RegionID regionID = RegionID(entityID.Value());
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(regionID);
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
    RegionID regionID = RegionID(entityID.Value());
    std::unique_ptr<CCigiEnvironmentalRegion> pRegion = std::make_unique<CCigiEnvironmentalRegion>(regionID);
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
