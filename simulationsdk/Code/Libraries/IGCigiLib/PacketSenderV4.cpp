//Copyright SimBlocks LLC 2016-2026
#include "PacketSenderV4.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypes.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/Buffer.h"
#include "libCIGI/Packets/4_0/AerosolConcentrationResponse.h"
#include "libCIGI/Packets/4_0/AnimationStopNotification.h"
#include "libCIGI/Packets/4_0/CollisionDetectionSegmentNotification.h"
#include "libCIGI/Packets/4_0/CollisionDetectionVolumeNotification.h"
#include "libCIGI/Packets/4_0/HATHOTResponse.h"
#include "libCIGI/Packets/4_0/HATHOTExtendedResponse.h"
#include "libCIGI/Packets/4_0/LineOfSightResponse.h"
#include "libCIGI/Packets/4_0/LineOfSightExtendedResponse.h"
#include "libCIGI/Packets/4_0/MaritimeSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/PositionResponse.h"
#include "libCIGI/Packets/4_0/SensorResponse.h"
#include "libCIGI/Packets/4_0/SensorExtendedResponse.h"
#include "libCIGI/Packets/4_0/SoF.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/WeatherConditionsResponse.h"
#include <list>

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace std;

CCigiPacketSenderV4::CCigiPacketSenderV4(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort) : CCigiPacketSender(imageGenerator, hostIPAddress, igToHostPort)
{
}

void CCigiPacketSenderV4::SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::LineOfSightResponse losRespV4;
  losRespV4.LOSId = response.lineOfSightRequestID.Value();
  losRespV4.hostFrameNumberLSN = response.hostFrameLSN;
  losRespV4.entityId = 0;
  losRespV4.range = response.dRange;
  losRespV4.valid = response.bValid;
  losRespV4.entityIdValid = false;
  losRespV4.visible = false;
  losRespV4.responseCount = response.responseCount;

  m_pBuffer->Write(losRespV4);
}

void CCigiPacketSenderV4::SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::LineOfSightResponse losRespV4;
  losRespV4.LOSId = response.lineOfSightRequestID.Value();
  losRespV4.hostFrameNumberLSN = response.hostFrameLSN;
  losRespV4.entityId = response.entityID.Value();
  losRespV4.range = response.dRange;
  losRespV4.valid = response.bValid;
  losRespV4.entityIdValid = true;
  losRespV4.visible = response.bVisible;
  losRespV4.responseCount = response.responseCount;

  m_pBuffer->Write(losRespV4);
}

void CCigiPacketSenderV4::SendPositionResponse(const SBasePositionResponse* response, sbio::cigi::EPositionResponseType type)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    if (type == sbio::cigi::EPositionResponseType::GEODETIC)
    {
      m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(*static_cast<const SPositionResponseGeodeticCoordinates*>(response));
    }
    else if (type == sbio::cigi::EPositionResponseType::PARENT)
    {
      m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(*static_cast<const SPositionResponseParentEntityCoordinates*>(response));
    }
    else if (type == sbio::cigi::EPositionResponseType::ARTICULATED)
    {
      m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(*static_cast<const SPositionResponseArticulatedPartCoordinates*>(response));
    }
  }

  CIGI::V40::PositionResponse posResp;

  posResp.objectClass = ConvertObjectClass(response->eObjectClass);
  posResp.objectId = response->objectID;
  posResp.yaw = response->rotation.yaw.Value();
  posResp.pitch = response->rotation.pitch.Value();
  posResp.roll = response->rotation.roll.Value();

  if (type == sbio::cigi::EPositionResponseType::GEODETIC)
  {
    SPositionResponseGeodeticCoordinates* geodetic = (SPositionResponseGeodeticCoordinates*)response;
    posResp.latitudeXOffset = geodetic->geodeticCoordinates.latitude.Value();
    posResp.longitudeYOffset = geodetic->geodeticCoordinates.longitude.Value();
    posResp.altitudeZOffset = geodetic->geodeticCoordinates.altitude.Value();
    posResp.coordinateSystem = CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic;
  }
  else if (type == sbio::cigi::EPositionResponseType::PARENT)
  {
    auto parent = (SPositionResponseParentEntityCoordinates*)response;
    posResp.latitudeXOffset = parent->offset.toVec3().x();
    posResp.longitudeYOffset = parent->offset.toVec3().y();
    posResp.altitudeZOffset = parent->offset.toVec3().z();
    posResp.coordinateSystem = (CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_ParentEntity);
  }
  else if (type == sbio::cigi::EPositionResponseType::ARTICULATED)
  {
    auto articulated = (SPositionResponseArticulatedPartCoordinates*)response;
    posResp.articulatedPartId = articulated->articulatedPartID.Value();
    posResp.latitudeXOffset = articulated->offset.toVec3().x();
    posResp.longitudeYOffset = articulated->offset.toVec3().y();
    posResp.altitudeZOffset = articulated->offset.toVec3().z();
    posResp.coordinateSystem = CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Submodel;
  }

  m_pBuffer->Write(posResp);
}

void CCigiPacketSenderV4::SendWeatherConditionsResponse(const SWeatherConditionsResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::WeatherConditionsResponse data;
  data.requestId = response.requestID;
  data.humidity = ConvertToCigiIntPercentage(response.humidity).Value();
  data.airTemperature = response.fAirTemperature;
  data.visibilityRange = response.fVisibilityRange;
  data.horizontalWindSpeed = response.windSpeedHorVer.horizontalWindSpeed;
  data.verticalWindSpeed = response.windSpeedHorVer.verticalWindSpeed;
  data.windDirection = response.fWindDirection;
  data.barometricPressure = response.fBarometricPressure;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendAerosolConcentrationResponse(const SAerosolConcentrationResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::AerosolConcentrationResponse data;
  data.requestId = response.requestID;
  data.layerId = response.layerID;
  data.aerosolConcentration = response.fAerosolConcentration;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendMaritimeSurfaceConditionsResponse(const SMaritimeSurfaceConditionsResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::MaritimeSurfaceConditionsResponse data;
  data.requestId = response.requestID;
  data.seaSurfaceHeight = static_cast<float>(response.fSeaSurfaceHeight.Value());
  data.surfaceWaterTemperature = static_cast<float>(response.fSurfaceWaterTemperature.Value());
  data.surfaceClarity = ConvertToCigiFloatPercentage(response.surfaceClarity).Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendTerrestrialSurfaceConditionsResponse(const STerrestrialSurfaceConditionsResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::TerrestrialSurfaceConditionsResponse data;
  data.requestId = response.requestID;
  data.surfaceConditionId = response.surfaceConditionID;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendEventNotification(const sbio::cigi::SEventNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::EventNotification data;
  data.eventId = response.EventID.Value();
  data.eventData1 = response.EventData1;
  data.eventData2 = response.EventData2;
  data.eventData3 = response.EventData3;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendImageGeneratorMessageNotification(const sbio::cigi::SImageGeneratorNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::IGMessage data;

  // only message ID is currently supported.
  data.messageId = response.MessageID.Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::LineOfSightExtendedResponse data;
  data.LOSId = response.lineOfSightRequestID.Value();
  data.hostFrameNumberLSN = response.hostFrameLSN;
  data.entityId = 0;
  data.range = response.dRange;
  data.valid = response.bValid;
  data.entityIdValid = false;
  data.responseCount = response.responseCount;
  data.rangeValid = response.bRangeValid;

  data.latitudeXOffset = response.geodeticCoordinates.latitude.Value();
  data.longitudeYOffset = response.geodeticCoordinates.longitude.Value();
  data.altitudeZOffset = response.geodeticCoordinates.altitude.Value();

  data.normalVectorAzimuth = response.fNormalVectorAzimuth;
  data.normalVectorElevation = response.fNormalVectorElevation;
  data.red = response.surfaceColor.r;
  data.blue = response.surfaceColor.b;
  data.green = response.surfaceColor.g;
  data.alpha = response.surfaceColor.a;
  data.materialCode = response.materialCode.Value();
  data.range = response.dRange;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::LineOfSightExtendedResponse data;
  data.LOSId = response.lineOfSightRequestID.Value();
  data.hostFrameNumberLSN = response.hostFrameLSN;
  data.entityId = response.entityID.Value();
  data.range = response.dRange;
  data.valid = response.bValid;
  data.entityIdValid = true;
  data.responseCount = response.responseCount;
  data.rangeValid = response.bRangeValid;

  data.latitudeXOffset = response.geodeticCoordinates.latitude.Value();
  data.longitudeYOffset = response.geodeticCoordinates.longitude.Value();
  data.altitudeZOffset = response.geodeticCoordinates.altitude.Value();

  data.normalVectorAzimuth = response.fNormalVectorAzimuth;
  data.normalVectorElevation = response.fNormalVectorElevation;
  data.red = response.surfaceColor.r;
  data.blue = response.surfaceColor.b;
  data.green = response.surfaceColor.g;
  data.alpha = response.surfaceColor.a;
  data.materialCode = response.materialCode.Value();
  data.range = response.dRange;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::LineOfSightExtendedResponse data;
  data.LOSId = response.lineOfSightRequestID.Value();
  data.hostFrameNumberLSN = response.hostFrameLSN;
  data.entityId = response.entityID.Value();
  data.range = response.dRange;
  data.valid = response.bValid;
  data.entityIdValid = true;
  data.responseCount = response.responseCount;
  data.rangeValid = response.bRangeValid;

  data.latitudeXOffset = response.offset[0];
  data.longitudeYOffset = response.offset[1];
  data.altitudeZOffset = response.offset[2];

  data.normalVectorAzimuth = response.fNormalVectorAzimuth;
  data.normalVectorElevation = response.fNormalVectorElevation;
  data.red = response.surfaceColor.r;
  data.blue = response.surfaceColor.b;
  data.green = response.surfaceColor.g;
  data.alpha = response.surfaceColor.a;
  data.materialCode = response.materialCode.Value();
  data.range = response.dRange;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendHatResponse(const SHeightAboveTerrainResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::HATHOTResponse hatHotRespV4;
  hatHotRespV4.HATHOTId = response.HATHOTID.Value();
  hatHotRespV4.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV4.valid = response.bValid;

  hatHotRespV4.responseType = CIGI::V40::HATHOTResponse::Type::eType_HAT;
  hatHotRespV4.height = response.heightAboveTerrain;

  m_pBuffer->Write(hatHotRespV4);
}

void CCigiPacketSenderV4::SendHotResponse(const SHeightOfTerrainResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::HATHOTResponse hatHotRespV4;
  hatHotRespV4.HATHOTId = response.HATHOTID.Value();
  hatHotRespV4.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV4.valid = response.bValid;

  hatHotRespV4.responseType = CIGI::V40::HATHOTResponse::Type::eType_HOT;
  hatHotRespV4.height = response.heightOfTerrain.Value();

  m_pBuffer->Write(hatHotRespV4);
}

void CCigiPacketSenderV4::SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::HATHOTExtendedResponse hatHotRespV4;
  hatHotRespV4.HATHOTId = response.HATHOTID.Value();
  hatHotRespV4.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV4.valid = response.bValid;
  hatHotRespV4.HAT = response.heightAboveTerrain;
  hatHotRespV4.HOT = response.heightOfTerrain.Value();
  hatHotRespV4.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV4.materialCode = response.materialCode.Value();
  hatHotRespV4.normalVectorAzimuth = response.normalVectorAzimuth.Value();
  hatHotRespV4.normalVectorElevation = response.normalVectorElevation.Value();

  m_pBuffer->Write(hatHotRespV4);
}

void CCigiPacketSenderV4::SendSensorResponse(const sbio::cigi::SSensorResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::SensorResponse data;
  data.sensorId = response.sensorID.Value();

  if (response.eSensorStatus == ESensorStatus::SEARCHING_FOR_TARGET)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::TRACKING_TARGET)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::IMPENDING_BREAKLOCK)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock;
  }
  else if (response.eSensorStatus == ESensorStatus::BREAKLOCK)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_Breaklock;
  }

  data.viewId = response.viewID.Value();
  data.gateXSize = static_cast<uint16_t>(response.gateSize[0] + .05f);
  data.gateYSize = static_cast<uint16_t>(response.gateSize[1] + .05f);
  data.gateXPosition = response.gatePosition[0];
  data.gateYPosition = response.gatePosition[1];
  data.hostFrameCounter = m_ImageGenerator.GetFrameNumber().Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendSensorExtendedResponse(const sbio::cigi::SSensorExtendedResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::SensorExtendedResponse data;
  data.viewId = response.viewID.Value();
  data.sensorId = response.sensorID.Value();

  if (response.eSensorStatus == ESensorStatus::SEARCHING_FOR_TARGET)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::TRACKING_TARGET)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::IMPENDING_BREAKLOCK)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock;
  }
  else if (response.eSensorStatus == ESensorStatus::BREAKLOCK)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_Breaklock;
  }

  data.entityId = 0;
  data.entityIdValid = false;
  data.gateXSize = static_cast<uint16_t>(response.gateSize[0] + .05f);
  data.gateYSize = static_cast<uint16_t>(response.gateSize[1] + .05f);
  data.gateXPosition = response.gatePosition[0];
  data.gateYPosition = response.gatePosition[1];
  data.hostFrameCounter = m_ImageGenerator.GetFrameNumber().Value();
  data.trackPointLatitude = response.trackPoint.latitude.Value();
  data.trackPointLongitude = response.trackPoint.longitude.Value();
  data.trackPointAltitude = response.trackPoint.altitude.Value();
  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendSensorExtendedEntityResponse(const sbio::cigi::SSensorExtendedEntityResponse& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::SensorExtendedResponse data;
  data.viewId = response.viewID.Value();
  data.entityId = response.entityID.Value();
  data.entityIdValid = true;
  data.sensorId = response.sensorID.Value();

  if (response.eSensorStatus == ESensorStatus::SEARCHING_FOR_TARGET)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::TRACKING_TARGET)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::IMPENDING_BREAKLOCK)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock;
  }
  else if (response.eSensorStatus == ESensorStatus::BREAKLOCK)
  {
    data.sensorStatus = CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_Breaklock;
  }
  data.gateXSize = static_cast<uint16_t>(response.gateSize[0] + .05f);
  data.gateYSize = static_cast<uint16_t>(response.gateSize[1] + .05f);
  data.gateXPosition = response.gatePosition[0];
  data.gateYPosition = response.gatePosition[1];
  data.hostFrameCounter = m_ImageGenerator.GetFrameNumber().Value();
  data.trackPointLatitude = response.trackPoint.latitude.Value();
  data.trackPointLongitude = response.trackPoint.longitude.Value();
  data.trackPointAltitude = response.trackPoint.altitude.Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV4::SendAnimationFinishedNotification(const sbio::cigi::SAnimationStopNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::AnimationStopNotification animationStop;
  animationStop.entityId = response.entityID.Value();

  m_pBuffer->Write(animationStop);
}

void CCigiPacketSenderV4::SendCollisionDetectionSegmentNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::CollisionDetectionSegmentNotification collisionSegmentResponse;
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.segmentId = response.segmentID.Value();
  collisionSegmentResponse.materialCode = response.materialCode.Value();
  collisionSegmentResponse.intersectionDistance = response.fIntersectionDistance;

  collisionSegmentResponse.collisionType = CIGI::V40::CollisionDetectionSegmentNotification::CollisionType::eCollisionType_NonEntity;

  m_pBuffer->Write(collisionSegmentResponse);
}

void CCigiPacketSenderV4::SendCollisionDetectionSegmentEntityNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::CollisionDetectionSegmentNotification collisionSegmentResponse;
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.segmentId = response.segmentID.Value();
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.contactedEntityId = response.contactedEntityID.Value();
  collisionSegmentResponse.materialCode = response.materialCode.Value();
  collisionSegmentResponse.intersectionDistance = response.fIntersectionDistance;

  collisionSegmentResponse.collisionType = CIGI::V40::CollisionDetectionSegmentNotification::CollisionType::eCollisionType_Entity;

  m_pBuffer->Write(collisionSegmentResponse);
}

void CCigiPacketSenderV4::SendCollisionDetectionVolumeNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::CollisionDetectionVolumeNotification collisionVolumeResponse;
  collisionVolumeResponse.entityId = response.entityID.Value();
  collisionVolumeResponse.volumeId = response.volumeID.Value();

  collisionVolumeResponse.collisionType = CIGI::V40::CollisionDetectionVolumeNotification::CollisionType::eCollisionType_NonEntity;
  collisionVolumeResponse.contactedEntityId = 0;//ignored for a non-entity collision
  collisionVolumeResponse.contactedVolumeId = response.contactedVolumeID.Value();

  m_pBuffer->Write(collisionVolumeResponse);
}

void CCigiPacketSenderV4::SendCollisionDetectionVolumeEntityNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& response)
{
  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(response);
  }

  CIGI::V40::CollisionDetectionVolumeNotification collisionVolumeResponse;
  collisionVolumeResponse.entityId = response.entityID.Value();
  collisionVolumeResponse.volumeId = response.volumeID.Value();
  collisionVolumeResponse.contactedEntityId = response.contactedEntityID.Value();
  collisionVolumeResponse.contactedVolumeId = response.contactedVolumeID.Value();

  collisionVolumeResponse.collisionType = CIGI::V40::CollisionDetectionVolumeNotification::CollisionType::eCollisionType_Entity;

  m_pBuffer->Write(collisionVolumeResponse);
}

void CCigiPacketSenderV4::SendStartOfFramePacket()
{
  CIGI::V40::SoF startOfFramePacket;

  EIGDatabaseState eDatabaseState = m_ImageGenerator.GetDatabaseState();

  if (eDatabaseState == EIGDatabaseState::NO_DATABASE)
  {
    // 0 instructs the host that the database is IG-controlled.
    // default database number is 1, but sending 1 would indicate database 1 has been loaded (received by host), when a database may have just started loading on the IG.
    // sending any negative number would indicate that a database is loading
    // sending any positive number between 1 and 127 would indicate that a database has been loaded
    // If the Host requests a database that does not exist or fails to load, the IG shall set this parameter to -128.
    // Send 0, but recommend that the host separately determine if the database is IG-controlled
    startOfFramePacket.dbNumber = 0;
  }
  else if (eDatabaseState == EIGDatabaseState::WAIT_FOR_ACKNOWLEDGE || eDatabaseState == EIGDatabaseState::LOADING)
  {
    startOfFramePacket.dbNumber = -static_cast<int8_t>(m_ImageGenerator.GetCurrentDatabaseID().Value());//negate ID to indicate that it is loading
  }
  else if (eDatabaseState == EIGDatabaseState::LOADING_FAILED)
  {
    //If the Host requests a database that does not exist or cannot be loaded, the IG will set this parameter to -128.
    startOfFramePacket.dbNumber = -128;
  }
  else if (eDatabaseState == EIGDatabaseState::IG_CONTROLLED)
  {
    //Indicates IG controls database loading
    startOfFramePacket.dbNumber = 0;
  }
  else if (eDatabaseState == EIGDatabaseState::LOADED)
  {
    //When the database load is complete and after the Host has acknowledged the database change,
    //the IG will set this parameter to the positive database number. The IG can now be considered mission-ready.
    startOfFramePacket.dbNumber = static_cast<int8_t>(m_ImageGenerator.GetCurrentDatabaseID().Value());
  }

  startOfFramePacket.earthReferenceModel = m_ImageGenerator.GetEarthReferenceModel() == EEarthReferenceModel::HOST_DEFINED ? 1 : 0;
  startOfFramePacket.igFrameNumber = m_ImageGenerator.GetFrameNumber().Value();
  startOfFramePacket.igMode = ConvertFrom(m_ImageGenerator.GetOperationMode());

  list<SImageGeneratorError> errors = m_ImageGenerator.GetErrors();

  if (errors.empty())
  {
  }
  else
  {
    //If more than one error is detected, the IG will report the one with the highest priority.
    SImageGeneratorError highestError = errors.front();

    for (auto error : errors)
    {
      if (error.nPriority > highestError.nPriority)
      {
        highestError = error;
      }
    }
  }

  startOfFramePacket.timestamp = m_ImageGenerator.GetTimeStamp();
  startOfFramePacket.timestampValid = m_ImageGenerator.IsTimeStampValid();
  startOfFramePacket.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber().Value();

  if (m_ImageGenerator.GetCigiMessageLogger() != nullptr)
  {
    SCigiStartOfFrame startOfFrame;
    startOfFrame.eVersion = ECigiVersion::VERSION_4_0;
    startOfFrame.databaseID = CigiDatabaseNumber(startOfFramePacket.dbNumber);
    startOfFrame.eIGMode = m_ImageGenerator.GetOperationMode();
    startOfFrame.bTimestampValid = startOfFramePacket.timestampValid;
    startOfFrame.bEarthReferenceModel = startOfFramePacket.earthReferenceModel != 0;
    startOfFrame.igFrameNumber = FrameNumber(startOfFramePacket.igFrameNumber);
    startOfFrame.microseconds = Microsecond(startOfFramePacket.timestamp);
    startOfFrame.lastHostFrameNumber = FrameNumber(startOfFramePacket.lastHostFrameNumber);
    startOfFrame.bOverframing = startOfFramePacket.overframing;
    startOfFrame.bPagingTerrain = startOfFramePacket.paging;
    startOfFrame.bExcessiveVariableLengthData = false;
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(startOfFrame);
  }

  m_pBuffer->Write(startOfFramePacket);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
