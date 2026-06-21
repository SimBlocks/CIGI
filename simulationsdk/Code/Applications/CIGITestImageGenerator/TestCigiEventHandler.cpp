//Copyright SimBlocks LLC 2016-2026
#include "TestCigiEventHandler.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/CigiEvent.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/IGResponseEventDispatcher.h"
#include "EntityLib/EntityManager.h"
#include "EntityLib/Entity.h"
#include "GlobalHeaders/Globals.h"
#include "UtilitiesLib/Logger.h"
#include "main.h"
#include <iostream>
#include <thread>

using namespace std;
using namespace sbio;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig;
using namespace sbio::math;

extern STestImageGeneratorGlobals globals;

CTestCigiEventHandler::CTestCigiEventHandler()
{
}

CTestCigiEventHandler::~CTestCigiEventHandler()
{
}

bool CTestCigiEventHandler::IsPointInEntityVolume(const sbio::math::GeocentricCoordinates& point, sbio::EntityID entityID) const
{
  CEntity* pEntity = globals.pEntityManager->GetEntity(entityID);

  if (pEntity == nullptr)
  {
    stringstream ss;
    ss << "IsPointInEntityVolume: Entity " << entityID.Value() << " does not exist." << endl;
    globals.pLogger->LogInformation(ss.str());
    return false;
  }

  // for testing purposes, consider the point to be in the entity volume if it is within 100 meters of the entity's world position
  const double testDistance = 100.0;

  Vec3 v = (pEntity->GetWorldTransform().pos - point).toVec3();
  double fDistance = v.norm();
  if (fDistance <= testDistance)
  {
    return true;
  }

  return false;
}

void CTestCigiEventHandler::OnCreateEntityMessage(const sbio::ig::entity::SCreateEntityMessage& data)
{
  stringstream ss;
  ss << "OnCreateEntityMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnDestroyEntityMessage(const sbio::ig::entity::SDestroyEntityMessage& data)
{
  stringstream ss;
  ss << "OnDestroyEntityMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateTopLevelEntityTransformMessage(const sbio::ig::entity::SUpdateTopLevelEntityTransformMessage& data)
{
  stringstream ss;
  ss << "OnUpdateTopLevelEntityTransformMessage: " << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateChildEntityTransformMessage(const sbio::ig::entity::SUpdateChildEntityTransformMessage& data)
{
  stringstream ss;
  ss << "OnUpdateChildEntityTransformMessage: " << data.EntityID << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEntityAttachedMessage(const sbio::ig::entity::SSetEntityAttachedMessage& data)
{
  stringstream ss;
  ss << "OnSetEntityAttachedMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEntityActiveMessage(const sbio::ig::entity::SSetEntityActiveMessage& data)
{
  stringstream ss;
  ss << "OnSetEntityActiveMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEntityUnattachedMessage(const sbio::ig::entity::SSetEntityUnattachedMessage& data)
{
  stringstream ss;
  ss << "OnSetEntityUnattachedMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEntityComponentStateMessage(const sbio::ig::entity::SSetEntityComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetEntityComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateArticulatedPartTransformMessage(const sbio::ig::entity::SUpdateArticulatedPartTransformMessage& data)
{
  stringstream ss;
  ss << "OnUpdateArticulatedPartTransformMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetArticulatedPartVisibleMessage(const sbio::ig::entity::SSetArticulatedPartVisibleMessage& data)
{
  stringstream ss;
  ss << "OnSetArticulatedPartVisibleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEntityAlphaMessage(const sbio::ig::entity::SSetEntityAlphaMessage& data)
{
  stringstream ss;
  ss << "OnSetEntityAlphaMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEntityCollisionDetectionEnabledMessage(const sbio::ig::entity::SSetEntityCollisionDetectionEnabledMessage& data)
{
  stringstream ss;
  ss << "OnSetEntityCollisionDetectionEnabledMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

// alternate between responding with entity intersection or non-entity intersection.
static std::atomic<uint32_t> g_lineOfSightResponseCounter = 0;

void CTestCigiEventHandler::OnLineOfSightSegmentRequestBasicMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestBasicMessage& data)
{
  stringstream ss;
  ss << "OnLineOfSightRequestMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  // simulate processing line of sight request asynchronously
  auto lineOfSightLambda = [data]()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (globals.pImageGenerator == nullptr || globals.pImageGenerator->GetExportedFunctionsEventDispatcher() == nullptr)
    {
      return;
    }

    const bool bEntityIntersection = (g_lineOfSightResponseCounter.fetch_add(1, std::memory_order_relaxed) % 2) != 0;

    if (bEntityIntersection)
    {
      sbio::cigi::SLineOfSightEntityResponse entityResponseData;
      entityResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
      entityResponseData.entityID = EntityID(2);
      entityResponseData.bValid = true;
      entityResponseData.bVisible = true;
      entityResponseData.hostFrameLSN = 0;
      entityResponseData.responseCount = 1;
      entityResponseData.dRange = 100.0;

      globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightEntityResponse(entityResponseData);
    }
    else
    {
      sbio::cigi::SLineOfSightResponse responseData;
      responseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
      responseData.bValid = true;
      responseData.hostFrameLSN = 0;
      responseData.responseCount = 1;
      responseData.dRange = 100.0;

      globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightResponse(responseData);
    }
  };

  std::thread(lineOfSightLambda).detach();
}

void CTestCigiEventHandler::OnLineOfSightSegmentRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestExtendedMessage& data)
{
  stringstream ss;
  ss << "OnLineOfSightRequestMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  // simulate processing line of sight request asynchronously
  auto lineOfSightLambda = [data]()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (globals.pImageGenerator == nullptr || globals.pImageGenerator->GetExportedFunctionsEventDispatcher() == nullptr)
    {
      return;
    }

    const bool bEntityIntersection = (g_lineOfSightResponseCounter.fetch_add(1, std::memory_order_relaxed) % 2) != 0;

    if (data.eResponseCoordinateSystem == sbio::ETopLevelCoordinateSystem::ENTITY)
    {
      if (bEntityIntersection)
      {
        sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse entityCoordinatesResponseData;
        entityCoordinatesResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        entityCoordinatesResponseData.entityID = EntityID(2);
        entityCoordinatesResponseData.bValid = true;
        entityCoordinatesResponseData.bRangeValid = true;
        entityCoordinatesResponseData.bVisible = true;
        entityCoordinatesResponseData.hostFrameLSN = 0;
        entityCoordinatesResponseData.responseCount = 1;
        entityCoordinatesResponseData.dRange = 100.0;
        entityCoordinatesResponseData.materialCode = MaterialID(1);
        entityCoordinatesResponseData.offset[0] = 0;
        entityCoordinatesResponseData.offset[1] = 0;
        entityCoordinatesResponseData.offset[2] = 0;

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedEntityCoordinatesResponse(entityCoordinatesResponseData);
      }
      else
      {
        sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse geodeticCoordinatesResponseData;
        geodeticCoordinatesResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        geodeticCoordinatesResponseData.bValid = true;
        geodeticCoordinatesResponseData.bRangeValid = true;
        geodeticCoordinatesResponseData.bVisible = true;
        geodeticCoordinatesResponseData.hostFrameLSN = 0;
        geodeticCoordinatesResponseData.responseCount = 1;
        geodeticCoordinatesResponseData.dRange = 100.0;
        geodeticCoordinatesResponseData.materialCode = MaterialID(1);
        geodeticCoordinatesResponseData.geodeticCoordinates.latitude = Latitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.longitude = Longitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedGeodeticCoordinatesResponse(geodeticCoordinatesResponseData);
      }
    }
    else
    {
      if (bEntityIntersection)
      {
        sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse entityGeodeticCoordinatesResponse;
        entityGeodeticCoordinatesResponse.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        entityGeodeticCoordinatesResponse.bValid = true;
        entityGeodeticCoordinatesResponse.bRangeValid = true;
        entityGeodeticCoordinatesResponse.bVisible = true;
        entityGeodeticCoordinatesResponse.hostFrameLSN = 0;
        entityGeodeticCoordinatesResponse.responseCount = 1;
        entityGeodeticCoordinatesResponse.dRange = 100.0;
        entityGeodeticCoordinatesResponse.materialCode = MaterialID(1);
        entityGeodeticCoordinatesResponse.entityID = EntityID(2);
        entityGeodeticCoordinatesResponse.geodeticCoordinates.latitude = Latitude(0);
        entityGeodeticCoordinatesResponse.geodeticCoordinates.longitude = Longitude(0);
        entityGeodeticCoordinatesResponse.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(entityGeodeticCoordinatesResponse);
      }
      else
      {
        sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse geodeticCoordinatesResponseData;
        geodeticCoordinatesResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        geodeticCoordinatesResponseData.bValid = true;
        geodeticCoordinatesResponseData.bRangeValid = true;
        geodeticCoordinatesResponseData.bVisible = true;
        geodeticCoordinatesResponseData.hostFrameLSN = 0;
        geodeticCoordinatesResponseData.responseCount = 1;
        geodeticCoordinatesResponseData.dRange = 100.0;
        geodeticCoordinatesResponseData.materialCode = MaterialID(1);
        geodeticCoordinatesResponseData.geodeticCoordinates.latitude = Latitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.longitude = Longitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedGeodeticCoordinatesResponse(geodeticCoordinatesResponseData);
      }
    }
  };

  std::thread(lineOfSightLambda).detach();
}

void CTestCigiEventHandler::OnLineOfSightVectorRequestBasicMessage(const sbio::ig::terrain::SLineOfSightVectorRequestBasicMessage& data)
{
  stringstream ss;
  ss << "OnLineOfSightVectorRequestBasicMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  auto lineOfSightLambda = [data]()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (globals.pImageGenerator == nullptr || globals.pImageGenerator->GetExportedFunctionsEventDispatcher() == nullptr)
    {
      return;
    }

    const bool bEntityIntersection = (g_lineOfSightResponseCounter.fetch_add(1, std::memory_order_relaxed) % 2) != 0;

    if (bEntityIntersection)
    {
      sbio::cigi::SLineOfSightEntityResponse entityResponseData;
      entityResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
      entityResponseData.entityID = EntityID(2);
      entityResponseData.bValid = true;
      entityResponseData.bVisible = true;
      entityResponseData.hostFrameLSN = 0;
      entityResponseData.responseCount = 1;
      entityResponseData.dRange = 100.0;

      globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightEntityResponse(entityResponseData);
    }
    else
    {
      sbio::cigi::SLineOfSightResponse responseData;
      responseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
      responseData.bValid = true;
      responseData.hostFrameLSN = 0;
      responseData.responseCount = 1;
      responseData.dRange = 100.0;

      globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightResponse(responseData);
    }
  };

  std::thread(lineOfSightLambda).detach();
}

void CTestCigiEventHandler::OnLineOfSightVectorRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightVectorRequestExtendedMessage& data)
{
  stringstream ss;
  ss << "OnLineOfSightVectorRequestExtendedMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  auto lineOfSightLambda = [data]()
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (globals.pImageGenerator == nullptr || globals.pImageGenerator->GetExportedFunctionsEventDispatcher() == nullptr)
    {
      return;
    }

    const bool bEntityIntersection = (g_lineOfSightResponseCounter.fetch_add(1, std::memory_order_relaxed) % 2) != 0;

    if (data.eResponseCoordinateSystem == sbio::ETopLevelCoordinateSystem::ENTITY)
    {
      if (bEntityIntersection)
      {
        sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse entityCoordinatesResponseData;
        entityCoordinatesResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        entityCoordinatesResponseData.entityID = EntityID(2);
        entityCoordinatesResponseData.bValid = true;
        entityCoordinatesResponseData.bRangeValid = true;
        entityCoordinatesResponseData.bVisible = true;
        entityCoordinatesResponseData.hostFrameLSN = 0;
        entityCoordinatesResponseData.responseCount = 1;
        entityCoordinatesResponseData.dRange = 100.0;
        entityCoordinatesResponseData.materialCode = MaterialID(1);
        entityCoordinatesResponseData.offset[0] = 0;
        entityCoordinatesResponseData.offset[1] = 0;
        entityCoordinatesResponseData.offset[2] = 0;

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedEntityCoordinatesResponse(entityCoordinatesResponseData);
      }
      else
      {
        sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse geodeticCoordinatesResponseData;
        geodeticCoordinatesResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        geodeticCoordinatesResponseData.bValid = true;
        geodeticCoordinatesResponseData.bRangeValid = true;
        geodeticCoordinatesResponseData.bVisible = true;
        geodeticCoordinatesResponseData.hostFrameLSN = 0;
        geodeticCoordinatesResponseData.responseCount = 1;
        geodeticCoordinatesResponseData.dRange = 100.0;
        geodeticCoordinatesResponseData.materialCode = MaterialID(1);
        geodeticCoordinatesResponseData.geodeticCoordinates.latitude = Latitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.longitude = Longitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedGeodeticCoordinatesResponse(geodeticCoordinatesResponseData);
      }
    }
    else
    {
      if (bEntityIntersection)
      {
        sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse entityGeodeticCoordinatesResponse;
        entityGeodeticCoordinatesResponse.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        entityGeodeticCoordinatesResponse.bValid = true;
        entityGeodeticCoordinatesResponse.bRangeValid = true;
        entityGeodeticCoordinatesResponse.bVisible = true;
        entityGeodeticCoordinatesResponse.hostFrameLSN = 0;
        entityGeodeticCoordinatesResponse.responseCount = 1;
        entityGeodeticCoordinatesResponse.dRange = 100.0;
        entityGeodeticCoordinatesResponse.materialCode = MaterialID(1);
        entityGeodeticCoordinatesResponse.entityID = EntityID(2);
        entityGeodeticCoordinatesResponse.geodeticCoordinates.latitude = Latitude(0);
        entityGeodeticCoordinatesResponse.geodeticCoordinates.longitude = Longitude(0);
        entityGeodeticCoordinatesResponse.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(entityGeodeticCoordinatesResponse);
      }
      else
      {
        sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse geodeticCoordinatesResponseData;
        geodeticCoordinatesResponseData.lineOfSightRequestID = LineOfSightRequestID(data.LosID);
        geodeticCoordinatesResponseData.bValid = true;
        geodeticCoordinatesResponseData.bRangeValid = true;
        geodeticCoordinatesResponseData.bVisible = true;
        geodeticCoordinatesResponseData.hostFrameLSN = 0;
        geodeticCoordinatesResponseData.responseCount = 1;
        geodeticCoordinatesResponseData.dRange = 100.0;
        geodeticCoordinatesResponseData.materialCode = MaterialID(1);
        geodeticCoordinatesResponseData.geodeticCoordinates.latitude = Latitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.longitude = Longitude(0);
        geodeticCoordinatesResponseData.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);

        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendLineOfSightExtendedGeodeticCoordinatesResponse(geodeticCoordinatesResponseData);
      }
    }
  };

  std::thread(lineOfSightLambda).detach();
}

void CTestCigiEventHandler::OnHeightAboveTerrainRequestMessage(const sbio::ig::terrain::SHeightAboveTerrainRequestMessage& data)
{
  stringstream ss;
  ss << "OnHeightAboveTerrainRequestMessage:" << endl;
  ss << "HatHotID: " << data.HatHotID << endl;
  ss << "Point: " << data.Point.latitude << ", " << data.Point.longitude << ", " << data.Point.altitude << endl;
  ss << "isExtendedRequest: " << data.isExtendedRequest << endl;
  globals.pLogger->LogDebug(ss.str());

  if (globals.pImageGenerator == nullptr || globals.pImageGenerator->GetExportedFunctionsEventDispatcher() == nullptr)
  {
    return;
  }

  auto dispatcher = globals.pImageGenerator->GetExportedFunctionsEventDispatcher();

  constexpr double TestTerrainAltitudeMeters = 0.0;
  constexpr float TestSurfaceNormalAzimuthDegrees = 0.0f;
  constexpr float TestSurfaceNormalElevationDegrees = 90.0f;
  const auto requestedAltitude = data.Point.altitude.Value();

  if (data.isExtendedRequest)
  {
    SHATHOTExtendedResponse response;
    response.HATHOTID = data.HatHotID;
    response.bValid = requestedAltitude >= TestTerrainAltitudeMeters;
    response.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(TestTerrainAltitudeMeters);
    response.heightAboveTerrain = requestedAltitude - TestTerrainAltitudeMeters;
    response.materialCode = MaterialID(1);
    response.normalVectorAzimuth = Degrees180(TestSurfaceNormalAzimuthDegrees);
    response.normalVectorElevation = Degrees90(TestSurfaceNormalElevationDegrees);

    dispatcher->SendHatHotExtendedResponse(response);
    return;
  }

  SHeightAboveTerrainResponse response;
  response.HATHOTID = data.HatHotID;
  response.bValid = requestedAltitude >= TestTerrainAltitudeMeters;
  response.heightAboveTerrain = requestedAltitude - TestTerrainAltitudeMeters;

  dispatcher->SendHeightAboveTerrainResponse(response);
}

void CTestCigiEventHandler::OnHeightOfTerrainRequestMessage(const sbio::ig::terrain::SHeightOfTerrainRequestMessage& data)
{
  stringstream ss;
  ss << "OnHeightOfTerrainRequestMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  if (globals.pImageGenerator == nullptr || globals.pImageGenerator->GetExportedFunctionsEventDispatcher() == nullptr)
  {
    return;
  }

  auto dispatcher = globals.pImageGenerator->GetExportedFunctionsEventDispatcher();

  constexpr double TestTerrainAltitudeMeters = 0.0;
  constexpr float TestSurfaceNormalAzimuthDegrees = 0.0f;
  constexpr float TestSurfaceNormalElevationDegrees = 90.0f;

  if (data.isExtendedRequest)
  {
    SHATHOTExtendedResponse response;
    response.HATHOTID = data.HatHotID;
    response.bValid = true;
    response.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(TestTerrainAltitudeMeters);
    response.heightAboveTerrain = data.Point.altitude.Value() - TestTerrainAltitudeMeters;
    response.materialCode = MaterialID(1);
    response.normalVectorAzimuth = Degrees180(TestSurfaceNormalAzimuthDegrees);
    response.normalVectorElevation = Degrees90(TestSurfaceNormalElevationDegrees);

    dispatcher->SendHatHotExtendedResponse(response);
    return;
  }

  const auto requestedAltitude = data.Point.altitude.Value();
  if (requestedAltitude > TestTerrainAltitudeMeters)
  {
    SHeightAboveTerrainResponse response;
    response.HATHOTID = data.HatHotID;
    response.bValid = true;
    response.heightAboveTerrain = requestedAltitude - TestTerrainAltitudeMeters;

    dispatcher->SendHeightAboveTerrainResponse(response);
    return;
  }

  SHeightOfTerrainResponse response;
  response.HATHOTID = data.HatHotID;
  response.bValid = true;
  response.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(TestTerrainAltitudeMeters);

  dispatcher->SendHeightOfTerrainResponse(response);
}

void CTestCigiEventHandler::OnTerrestrialSurfaceConditionsChangedMessage()
{
  SGeodeticCoordinates currentLocation(Latitude(0), Longitude(0), HeightRelativeToWGS84Ellipsoid(0));
  std::vector<SCigiTerrestrialSurfaceCondition> conditions = globals.pImageGenerator->QueryTerrestrialSurfaceCondition(currentLocation);

  stringstream ss;
  ss << "OnTerrestrialSurfaceConditionsChangedMessage:" << endl;
  for (const SCigiTerrestrialSurfaceCondition& condition : conditions)
  {
    ss << "Queried SurfaceConditionID: " << condition.surfaceConditionID << endl;
  }
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetRegionalTerrainSurfaceComponentStateMessage(const sbio::ig::terrain::SSetRegionalTerrainSurfaceComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetRegionalTerrainSurfaceComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetGlobalTerrainComponentStateMessage(const sbio::ig::terrain::SSetGlobalTerrainComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetGlobalTerrainComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetCollisionDetectionSegmentEnabledMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentEnabledMessage& data)
{
  stringstream ss;
  ss << "OnSetCollisionDetectionSegmentEnabledMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  if (data.Enabled)
  {
    // simulate processing collision segment creation asynchronously
    auto createCollisionSegmentLambda = [data]()
    {
      // send SCollisionDetectionSegmentNotification after a short delay to simulate asynchronous processing
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      SCollisionDetectionSegmentNotification collisionDetectionSegmentData;
      collisionDetectionSegmentData.entityID = data.EntityID;
      collisionDetectionSegmentData.segmentID = data.SegmentID;
      collisionDetectionSegmentData.materialCode = MaterialID(1);
      collisionDetectionSegmentData.fIntersectionDistance = 0.0f;

      if (globals.pImageGenerator != nullptr && globals.pImageGenerator->GetExportedFunctionsEventDispatcher() != nullptr)
      {
        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendCollisionDetectionSegmentNotification(collisionDetectionSegmentData);
      }

      // also test sending SCollisionDetectionSegmentEntityNotification
      SCollisionDetectionSegmentEntityNotification collisionDetectionSegmentEntityData;
      collisionDetectionSegmentEntityData.entityID = data.EntityID;
      collisionDetectionSegmentEntityData.segmentID = data.SegmentID;
      collisionDetectionSegmentEntityData.materialCode = MaterialID(1);
      collisionDetectionSegmentEntityData.fIntersectionDistance = 0.0f;
      collisionDetectionSegmentEntityData.contactedEntityID = EntityID(10);

      if (globals.pImageGenerator != nullptr && globals.pImageGenerator->GetExportedFunctionsEventDispatcher() != nullptr)
      {
        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendCollisionDetectionSegmentEntityNotification(collisionDetectionSegmentEntityData);
      }
    };

    std::thread(createCollisionSegmentLambda).detach();
  }
}

void CTestCigiEventHandler::OnSetCollisionDetectionSegmentMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentMessage& data)
{
  stringstream ss;
  ss << "OnSetCollisionDetectionSegmentMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateCollisionDetectionSegmentMessage(const sbio::ig::physics::SCreateCollisionDetectionSegmentMessage& data)
{
  stringstream ss;
  ss << "OnCreateCollisionDetectionSegmentMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateCollisionVolumeSphereMessage(const sbio::ig::physics::SCreateCollisionVolumeSphereMessage& data)
{
  stringstream ss;
  ss << "OnCreateCollisionVolumeSphereMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateCollisionVolumeCuboidMessage(const sbio::ig::physics::SCreateCollisionVolumeCuboidMessage& data)
{
  stringstream ss;
  ss << "OnCreateCollisionVolumeCuboidMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetCollisionVolumeEnabledMessage(const sbio::ig::physics::SSetCollisionVolumeEnabledMessage& data)
{
  stringstream ss;
  ss << "OnSetCollisionVolumeEnabledMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  if (data.Enabled)
  {
    // simulate processing collision volume sphere creation asynchronously
    auto createCollisionVolumeSphereLambda = [data]()
    {
      // send SCollisionDetectionVolumeNotificationMessage after a short delay to simulate asynchronous processing
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      SCollisionDetectionVolumeNotification collisionDetectionVolumeData;
      collisionDetectionVolumeData.entityID = data.EntityID;
      collisionDetectionVolumeData.volumeID = data.VolumeID;
      collisionDetectionVolumeData.contactedVolumeID = VolumeID(2);

      if (globals.pImageGenerator != nullptr && globals.pImageGenerator->GetExportedFunctionsEventDispatcher() != nullptr)
      {
        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendCollisionDetectionVolumeNotification(collisionDetectionVolumeData);
      }

      // also send SCollisionDetectionVolumeEntityNotification
      SCollisionDetectionVolumeEntityNotification collisionDetectionVolumeEntityData;
      collisionDetectionVolumeEntityData.entityID = data.EntityID;
      collisionDetectionVolumeEntityData.volumeID = data.VolumeID;
      collisionDetectionVolumeEntityData.contactedVolumeID = VolumeID(2);

      if (globals.pImageGenerator != nullptr && globals.pImageGenerator->GetExportedFunctionsEventDispatcher() != nullptr)
      {
        globals.pImageGenerator->GetExportedFunctionsEventDispatcher()->SendCollisionDetectionVolumeEntityNotification(collisionDetectionVolumeEntityData);
      }
    };

    std::thread(createCollisionVolumeSphereLambda).detach();
  }
}

void CTestCigiEventHandler::OnSetCollisionVolumeMessage(const sbio::ig::physics::SSetCollisionVolumeMessage& data)
{
  stringstream ss;
  ss << "OnSetCollisionVolumeMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnDestroyCollisionVolumeMessage(const sbio::ig::physics::SDestroyCollisionVolumeMessage& data)
{
  stringstream ss;
  ss << "OnDestroyCollisionVolumeMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateAttachedCameraTransformMessage(const sbio::ig::view::SUpdateAttachedCameraTransformMessage& data)
{
  stringstream ss;
  ss << "OnUpdateAttachedCameraTransformMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetCameraAttachedToEntityMessage(const sbio::ig::view::SSetCameraAttachedToEntityMessage& data)
{
  stringstream ss;
  ss << "OnSetCameraAttachedToEntityMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetCameraUnattachedMessage(const sbio::ig::view::SSetCameraUnattachedMessage& data)
{
  stringstream ss;
  ss << "OnSetCameraUnattachedMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetCameraProjectionMessage(const sbio::ig::view::SSetCameraProjectionMessage& data)
{
  stringstream ss;
  ss << "OnSetCameraProjectionMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetViewComponentStateMessage(const sbio::ig::view::SSetViewComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetViewComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetViewGroupComponentStateMessage(const sbio::ig::view::SSetViewGroupComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetViewGroupComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUnloadDatabaseMessage()
{
  stringstream ss;
  ss << "OnUnloadDatabaseMessage: " << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnLoadDatabaseMessage(const sbio::ig::database::SLoadDatabaseMessage& data)
{
  stringstream ss;
  ss << "OnLoadDatabaseMessage: " << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());

  m_bLoadDatabaseComplete.store(false);

  // simulate loading scenario asynchronously
  auto loadingScenarioLambda = [this]()
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    m_bLoadDatabaseComplete.store(true);
  };

  std::thread(loadingScenarioLambda).detach();
}

void CTestCigiEventHandler::OnSetAnimationDirectionMessage(const sbio::ig::animation::SSetAnimationDirectionMessage& data)
{
  stringstream ss;
  ss << "OnSetAnimationDirectionMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetAnimationLoopModeMessage(const sbio::ig::animation::SSetAnimationLoopModeMessage& data)
{
  stringstream ss;
  ss << "OnSetAnimationLoopModeMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetAnimationSpeedMessage(const sbio::ig::animation::SSetAnimationSpeedMessage& data)
{
  stringstream ss;
  ss << "OnSetAnimationSpeedMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnStopEntityAnimationMessage(const sbio::ig::animation::SStopEntityAnimationMessage& data)
{
  stringstream ss;
  ss << "OnStopEntityAnimationMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnStopAtCurrentFrameEntityAnimationMessage(const sbio::ig::animation::SStopAtCurrentFrameEntityAnimationMessage& data)
{
  stringstream ss;
  ss << "OnStopAtCurrentFrameEntityAnimationMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnPauseEntityAnimationMessage(const sbio::ig::animation::SPauseEntityAnimationMessage& data)
{
  stringstream ss;
  ss << "OnPauseEntityAnimationMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnPlayEntityAnimationMessage(const sbio::ig::animation::SPlayEntityAnimationMessage& data)
{
  stringstream ss;
  ss << "OnPlayEntityAnimationMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnRestartEntityAnimationMessage(const sbio::ig::animation::SRestartEntityAnimationMessage& data)
{
  stringstream ss;
  ss << "OnRestartEntityAnimationMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolTextMessage(const sbio::ig::symbol::SCreateSymbolTextMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolTextMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolTextMessage(const sbio::ig::symbol::SUpdateSymbolTextMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolTextMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolColorMessage(const sbio::ig::symbol::SSetSymbolColorMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolColorMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnDestroySymbolMessage(const sbio::ig::symbol::SDestroySymbolMessage& data)
{
  stringstream ss;
  ss << "OnDestroySymbolMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolCircleMessage(const sbio::ig::symbol::SCreateSymbolCircleMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolCircleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolCircleMessage(const sbio::ig::symbol::SUpdateSymbolCircleMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolCircleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolCircleElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleElementMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolCircleElementMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolCircleFilledMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolCircleFilledMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolCircleFilledElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledElementMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolCircleFilledElementMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolTexturedCircleMessage(const sbio::ig::symbol::SCreateSymbolTexturedCircleMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolTexturedCircleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolTexturedCircleMessage(const sbio::ig::symbol::SUpdateSymbolTexturedCircleMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolTexturedCircleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateTexturedCircleMessage(const sbio::ig::symbol::SUpdateTexturedCircleMessage& data)
{
  stringstream ss;
  ss << "OnUpdateTexturedCircleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolPolygonMessage(const sbio::ig::symbol::SCreateSymbolPolygonMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolPolygonMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolPolygonMessage(const sbio::ig::symbol::SUpdateSymbolPolygonMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolPolygonMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolPolygonVertexMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolPolygonVertexMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SCreateSymbolTexturedPolygonMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolTexturedPolygonMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SUpdateSymbolTexturedPolygonMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolTexturedPolygonMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolTexturedPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolTexturedPolygonVertexMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolTexturedPolygonVertexMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateEntityBillboardSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateEntityBillboardSymbolSurfaceMessage& data)
{
  stringstream ss;
  ss << "OnUpdateEntityBillboardSymbolSurfaceMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolSurfaceMessage(const sbio::ig::symbol::SCreateSymbolSurfaceMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolSurfaceMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnDestroySymbolSurfaceMessage(const sbio::ig::symbol::SDestroySymbolSurfaceMessage& data)
{
  stringstream ss;
  ss << "OnDestroySymbolSurfaceMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateSymbolSurfaceMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolSurfaceMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateViewSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateViewSymbolSurfaceMessage& data)
{
  stringstream ss;
  ss << "OnUpdateViewSymbolSurfaceMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolVisibleMessage(const sbio::ig::symbol::SSetSymbolVisibleMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolVisibleMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolAttachedMessage(const sbio::ig::symbol::SSetSymbolAttachedMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolAttachedMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolUnattachedMessage(const sbio::ig::symbol::SSetSymbolUnattachedMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolUnattachedMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolSurfaceMessage(const sbio::ig::symbol::SSetSymbolSurfaceMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolSurfaceMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetTopLevelSymbolTransformMessage(const sbio::ig::symbol::SSetTopLevelSymbolTransformMessage& data)
{
  stringstream ss;
  ss << "OnSetTopLevelSymbolTransformMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetChildSymbolTransformMessage(const sbio::ig::symbol::SSetChildSymbolTransformMessage& data)
{
  stringstream ss;
  ss << "OnSetChildSymbolTransformMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSymbolMessage(const sbio::ig::symbol::SUpdateSymbolMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSymbolMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolComponentStateMessage(const sbio::ig::symbol::SSetSymbolComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSymbolSurfaceComponentStateMessage(const sbio::ig::symbol::SSetSymbolSurfaceComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetSymbolSurfaceComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateSymbolFromTemplateMessage(const sbio::ig::symbol::SCreateSymbolFromTemplateMessage& data)
{
  stringstream ss;
  ss << "OnCreateSymbolFromTemplateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateCelestialSphereMessage(const sbio::ig::celestial::SUpdateCelestialSphereMessage& data)
{
  stringstream ss;
  ss << "OnUpdateCelestialSphereMessage:" << std::endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateDateTimeMessage(const sbio::ig::celestial::SUpdateDateTimeMessage& data)
{
  stringstream ss;
  ss << "OnUpdateDateTimeMessage:" << std::endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetCelestialSphereComponentStateMessage(const sbio::ig::celestial::SSetCelestialSphereComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetCelestialSphereComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSensorMessage(const sbio::ig::sensor::SUpdateSensorMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSensorMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnUpdateSensorComponentMessage(const sbio::ig::sensor::SUpdateSensorComponentMessage& data)
{
  stringstream ss;
  ss << "OnUpdateSensorComponentMessage" << std::endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateMotionTrackerViewMessage(const sbio::ig::sensor::SCreateMotionTrackerViewMessage& data)
{
  stringstream ss;
  ss << "OnCreateMotionTrackerViewMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnCreateMotionTrackerViewGroupMessage(const sbio::ig::sensor::SCreateMotionTrackerViewGroupMessage& data)
{
  stringstream ss;
  ss << "OnCreateMotionTrackerViewGroupMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetMotionTrackerMessage(const sbio::ig::sensor::SSetMotionTrackerMessage& data)
{
  stringstream ss;
  ss << "OnSetMotionTrackerMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetAtmosphereEnabledMessage(const sbio::ig::atmosphere::SSetAtmosphereEnabledMessage& data)
{
  stringstream ss;
  ss << "OnSetAtmosphereEnabledMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetAtmosphereMessage(const sbio::ig::atmosphere::SSetAtmosphereMessage& data)
{
  stringstream ss;
  ss << "OnSetAtmosphereMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetWeatherMessage(const sbio::ig::atmosphere::SSetWeatherMessage& data)
{
  stringstream ss;
  ss << "OnSetWeatherMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetRegionalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetRegionalLayeredWeatherComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetRegionalLayeredWeatherComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetGlobalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetGlobalLayeredWeatherComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetGlobalLayeredWeatherComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetAtmosphereComponentStateMessage(const sbio::ig::atmosphere::SSetAtmosphereComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetAtmosphereComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetMaritimeSurfaceConditionsMessage(const sbio::ig::ocean::SSetMaritimeSurfaceConditionsMessage& data)
{
  stringstream ss;
  ss << "OnSetMaritimeSurfaceConditionsMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetRegionMaritimeComponentStateMessage(const sbio::ig::ocean::SSetRegionMaritimeComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetRegionMaritimeComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetGlobalMaritimeComponentStateMessage(const sbio::ig::ocean::SSetGlobalMaritimeComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetGlobalMaritimeComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEarthReferenceModelMessage(const sbio::ig::earth::SSetEarthReferenceModelMessage& data)
{
  stringstream ss;
  ss << "OnSetEarthReferenceModelMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetEventComponentStateMessage(const sbio::ig::system::SSetEventComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetEventComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetSystemComponentStateMessage(const sbio::ig::system::SSetSystemComponentStateMessage& data)
{
  stringstream ss;
  ss << "OnSetSystemComponentStateMessage:" << endl;
  ss << data << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::Update()
{
  if (m_bLoadDatabaseComplete.load())
  {
    m_bLoadDatabaseComplete.store(false);

    // Raise event to indicate database load is complete
    sbio::cigi::ig::SDatabaseLoadedEventArgs args;
    args.bLoadSuccessful = true;
    sbio::utils::Event::Raise<sbio::cigi::ig::IGCIGIEvent>(args);
  }
}

void CTestCigiEventHandler::OnSetHostConnectedMessage(const sbio::ig::network::SHostConnectedMessage& data)
{
  stringstream ss;
  ss << "OnSetHostConnectedMessage: " << endl;
  ss << "\tHostIP: " << data.sHostIP << endl;
  globals.pLogger->LogDebug(ss.str());
}

void CTestCigiEventHandler::OnSetHostDisconnectedMessage(const sbio::ig::network::SHostDisconnectedMessage& data)
{
  stringstream ss;
  ss << "OnSetHostDisconnectedMessage: " << endl;
  ss << "\tHostIP: " << data.sHostIP << endl;
  globals.pLogger->LogDebug(ss.str());
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
