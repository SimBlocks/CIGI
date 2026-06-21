//Copyright SimBlocks LLC 2016-2026
#include "PacketSenderV3.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypes.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/Buffer.h"
#include "libCIGI/Packets/3_3/AerosolConcentrationResponse.h"
#include "libCIGI/Packets/3_3/AnimationStopNotification.h"
#include "libCIGI/Packets/3_3/CollisionDetectionSegmentNotification.h"
#include "libCIGI/Packets/3_3/CollisionDetectionVolumeNotification.h"
#include "libCIGI/Packets/3_3/HATHOTResponse.h"
#include "libCIGI/Packets/3_3/HATHOTExtendedResponse.h"
#include "libCIGI/Packets/3_3/LineOfSightResponse.h"
#include "libCIGI/Packets/3_3/LineOfSightExtendedResponse.h"
#include "libCIGI/Packets/3_3/MaritimeSurfaceConditionsResponse.h"
#include "libCIGI/Packets/3_3/PositionResponse.h"
#include "libCIGI/Packets/3_3/SensorResponse.h"
#include "libCIGI/Packets/3_3/SensorExtendedResponse.h"
#include "libCIGI/Packets/3_3/SoF.h"
#include "libCIGI/Packets/3_3/TerrestrialSurfaceConditionsResponse.h"
#include "libCIGI/Packets/3_3/WeatherConditionsResponse.h"
#include "libCIGI/Packets/3_3/IGMessage.h"
#include "libCIGI/Packets/3_3/EventNotification.h"
#include <list>

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiPacketSenderV3::CCigiPacketSenderV3(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort) : CCigiPacketSender(imageGenerator, hostIPAddress, igToHostPort)
{
  //m_pCurrentBufferPtr = m_pBuffer->GetBuffer() + 24;//Start of Frame V3 Packet Size
}

void CCigiPacketSenderV3::SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::LineOfSightResponse losRespV3;
  losRespV3.LOSId = response.lineOfSightRequestID.Value();
  losRespV3.hostFrameNumberLSN = response.hostFrameLSN;
  losRespV3.entityId = 0;
  losRespV3.range = response.dRange;
  losRespV3.valid = response.bValid;
  losRespV3.entityIdValid = false;
  losRespV3.visible = false;
  losRespV3.responseCount = response.responseCount;

  m_pBuffer->Write(losRespV3);
}

void CCigiPacketSenderV3::SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::LineOfSightResponse losRespV3;
  losRespV3.LOSId = response.lineOfSightRequestID.Value();
  losRespV3.hostFrameNumberLSN = response.hostFrameLSN;
  losRespV3.entityId = response.entityID.Value();
  losRespV3.range = response.dRange;
  losRespV3.valid = response.bValid;
  losRespV3.entityIdValid = true;
  losRespV3.visible = response.bVisible;
  losRespV3.responseCount = response.responseCount;

  m_pBuffer->Write(losRespV3);
}

void CCigiPacketSenderV3::SendPositionResponse(const SBasePositionResponse* response, sbio::cigi::EPositionResponseType type)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    if (type == sbio::cigi::EPositionResponseType::GEODETIC)
    {
      g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(*static_cast<const SPositionResponseGeodeticCoordinates*>(response));
    }
    else if (type == sbio::cigi::EPositionResponseType::PARENT)
    {
      g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(*static_cast<const SPositionResponseParentEntityCoordinates*>(response));
    }
    else if (type == sbio::cigi::EPositionResponseType::ARTICULATED)
    {
      g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(*static_cast<const SPositionResponseArticulatedPartCoordinates*>(response));
    }
  }

  CIGI::V33::PositionResponse posResp;

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
    posResp.coordinateSystem = CIGI::V33::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic;
  }
  else if (type == sbio::cigi::EPositionResponseType::PARENT)
  {
    auto parent = (SPositionResponseParentEntityCoordinates*)response;
    posResp.latitudeXOffset = parent->offset.toVec3().x();
    posResp.longitudeYOffset = parent->offset.toVec3().y();
    posResp.altitudeZOffset = parent->offset.toVec3().z();
    posResp.coordinateSystem = CIGI::V33::PositionResponse::CoordinateSystem::eCoordinateSystem_ParentEntity;
  }
  else if (type == sbio::cigi::EPositionResponseType::ARTICULATED)
  {
    auto articulated = (SPositionResponseArticulatedPartCoordinates*)response;
    posResp.articulatedPartId = articulated->articulatedPartID.Value();
    posResp.latitudeXOffset = articulated->offset.toVec3().x();
    posResp.longitudeYOffset = articulated->offset.toVec3().y();
    posResp.altitudeZOffset = articulated->offset.toVec3().z();
    posResp.coordinateSystem = CIGI::V33::PositionResponse::CoordinateSystem::eCoordinateSystem_Submodel;
  }

  m_pBuffer->Write(posResp);
}

void CCigiPacketSenderV3::SendWeatherConditionsResponse(const SWeatherConditionsResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::WeatherConditionsResponse data;
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

void CCigiPacketSenderV3::SendAerosolConcentrationResponse(const SAerosolConcentrationResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::AerosolConcentrationResponse data;
  data.requestId = response.requestID;
  data.layerId = response.layerID;
  data.aerosolConcentration = response.fAerosolConcentration;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV3::SendMaritimeSurfaceConditionsResponse(const SMaritimeSurfaceConditionsResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::MaritimeSurfaceConditionsResponse data;
  data.requestId = response.requestID;
  data.seaSurfaceHeight = static_cast<float>(response.fSeaSurfaceHeight.Value());
  data.surfaceWaterTemperature = response.fSurfaceWaterTemperature.Value();
  data.surfaceClarity = ConvertToCigiFloatPercentage(response.surfaceClarity).Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV3::SendTerrestrialSurfaceConditionsResponse(const STerrestrialSurfaceConditionsResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::TerrestrialSurfaceConditionsResponse data;
  data.requestId = response.requestID;
  data.surfaceConditionId = response.surfaceConditionID;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV3::SendEventNotification(const sbio::cigi::SEventNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::EventNotification data;
  data.eventId = response.EventID.Value();
  data.eventData1 = response.EventData1;
  data.eventData2 = response.EventData2;
  data.eventData3 = response.EventData3;

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV3::SendImageGeneratorMessageNotification(const sbio::cigi::SImageGeneratorNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::IGMessage data;

  // only message ID is currently supported.
  data.messageId = response.MessageID.Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV3::SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::LineOfSightExtendedResponse data;
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

void CCigiPacketSenderV3::SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::LineOfSightExtendedResponse data;
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

void CCigiPacketSenderV3::SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::LineOfSightExtendedResponse data;
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

void CCigiPacketSenderV3::SendHatResponse(const SHeightAboveTerrainResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::HATHOTResponse hatHotRespV3;
  hatHotRespV3.HATHOTId = response.HATHOTID.Value();
  hatHotRespV3.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV3.valid = response.bValid;

  hatHotRespV3.responseType = CIGI::V33::HATHOTResponse::ResponseType::eResponseType_HAT;
  hatHotRespV3.height = response.heightAboveTerrain;

  m_pBuffer->Write(hatHotRespV3);
}

void CCigiPacketSenderV3::SendHotResponse(const SHeightOfTerrainResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::HATHOTResponse hatHotRespV3;
  hatHotRespV3.HATHOTId = response.HATHOTID.Value();
  hatHotRespV3.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV3.valid = response.bValid;

  hatHotRespV3.responseType = CIGI::V33::HATHOTResponse::ResponseType::eResponseType_HOT;
  hatHotRespV3.height = response.heightOfTerrain.Value();

  m_pBuffer->Write(hatHotRespV3);
}

void CCigiPacketSenderV3::SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::HATHOTExtendedResponse hatHotRespV3;
  hatHotRespV3.HATHOTId = response.HATHOTID.Value();
  hatHotRespV3.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV3.valid = response.bValid;
  hatHotRespV3.HAT = response.heightAboveTerrain;
  hatHotRespV3.HOT = response.heightOfTerrain.Value();
  hatHotRespV3.hostFrameNumberLSN = response.hostFrameLSN;
  hatHotRespV3.materialCode = response.materialCode.Value();
  hatHotRespV3.normalVectorAzimuth = response.normalVectorAzimuth.Value();
  hatHotRespV3.normalVectorElevation = response.normalVectorElevation.Value();

  m_pBuffer->Write(hatHotRespV3);
}

void CCigiPacketSenderV3::SendSensorResponse(const sbio::cigi::SSensorResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::SensorResponse data;
  data.sensorId = response.sensorID.Value();

  if (response.eSensorStatus == ESensorStatus::SEARCHING_FOR_TARGET)
  {
    data.sensorStatus = CIGI::V33::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::TRACKING_TARGET)
  {
    data.sensorStatus = CIGI::V33::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::IMPENDING_BREAKLOCK)
  {
    data.sensorStatus = CIGI::V33::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock;
  }
  else if (response.eSensorStatus == ESensorStatus::BREAKLOCK)
  {
    data.sensorStatus = CIGI::V33::SensorResponse::SensorStatus::eSensorStatus_Breaklock;
  }

  data.viewId = response.viewID.Value();
  data.gateXSize = static_cast<uint16_t>(response.gateSize[0] + .05f);
  data.gateYSize = static_cast<uint16_t>(response.gateSize[1] + .05f);
  data.gateXPosition = response.gatePosition[0];
  data.gateYPosition = response.gatePosition[1];
  data.hostFrameCounter = m_ImageGenerator.GetFrameNumber().Value();

  m_pBuffer->Write(data);
}

void CCigiPacketSenderV3::SendSensorExtendedResponse(const sbio::cigi::SSensorExtendedResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::SensorExtendedResponse data;
  data.viewId = response.viewID.Value();
  data.sensorId = response.sensorID.Value();

  if (response.eSensorStatus == ESensorStatus::SEARCHING_FOR_TARGET)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_SearchingForTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::TRACKING_TARGET)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_TrackingTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::IMPENDING_BREAKLOCK)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_ImpendingBreaklock;
  }
  else if (response.eSensorStatus == ESensorStatus::BREAKLOCK)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_Breaklock;
  }

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

void CCigiPacketSenderV3::SendSensorExtendedEntityResponse(const sbio::cigi::SSensorExtendedEntityResponse& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::SensorExtendedResponse data;
  data.viewId = response.viewID.Value();
  data.entityId = response.entityID.Value();
  data.sensorId = response.sensorID.Value();

  if (response.eSensorStatus == ESensorStatus::SEARCHING_FOR_TARGET)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_SearchingForTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::TRACKING_TARGET)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_TrackingTarget;
  }
  else if (response.eSensorStatus == ESensorStatus::IMPENDING_BREAKLOCK)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_ImpendingBreaklock;
  }
  else if (response.eSensorStatus == ESensorStatus::BREAKLOCK)
  {
    data.sensorStatus = CIGI::V33::SensorExtendedResponse::SensorStatus::eSensorStatus_Breaklock;
  }

  data.entityIdValid = true;
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

void CCigiPacketSenderV3::SendAnimationFinishedNotification(const sbio::cigi::SAnimationStopNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::AnimationStopNotification animationStop;
  animationStop.entityId = response.entityID.Value();

  m_pBuffer->Write(animationStop);
}

void CCigiPacketSenderV3::SendCollisionDetectionSegmentNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::CollisionDetectionSegmentNotification collisionSegmentResponse;
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.segmentId = response.segmentID.Value();
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.materialCode = response.materialCode.Value();
  collisionSegmentResponse.intersectionDistance = response.fIntersectionDistance;
  collisionSegmentResponse.collisionType = 0;//non-entity

  m_pBuffer->Write(collisionSegmentResponse);
}

void CCigiPacketSenderV3::SendCollisionDetectionSegmentEntityNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::CollisionDetectionSegmentNotification collisionSegmentResponse;
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.segmentId = response.segmentID.Value();
  collisionSegmentResponse.entityId = response.entityID.Value();
  collisionSegmentResponse.materialCode = response.materialCode.Value();
  collisionSegmentResponse.intersectionDistance = response.fIntersectionDistance;
  collisionSegmentResponse.contactedEntityId = response.contactedEntityID.Value();

  collisionSegmentResponse.collisionType = 1;//entity

  m_pBuffer->Write(collisionSegmentResponse);
}

void CCigiPacketSenderV3::SendCollisionDetectionVolumeNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::CollisionDetectionVolumeNotification collisionVolumeResponse;
  collisionVolumeResponse.entityId = response.entityID.Value();
  collisionVolumeResponse.volumeId = response.volumeID.Value();

  collisionVolumeResponse.collisionType = 0;//non-entity

  m_pBuffer->Write(collisionVolumeResponse);
}

void CCigiPacketSenderV3::SendCollisionDetectionVolumeEntityNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& response)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(response);
  }

  CIGI::V33::CollisionDetectionVolumeNotification collisionVolumeResponse;
  collisionVolumeResponse.entityId = response.entityID.Value();
  collisionVolumeResponse.volumeId = response.volumeID.Value();
  collisionVolumeResponse.contactedEntityId = response.contactedEntityID.Value();
  collisionVolumeResponse.contactedVolumeId = response.contactedVolumeID.Value();

  collisionVolumeResponse.collisionType = 1;//entity

  m_pBuffer->Write(collisionVolumeResponse);
}

void CCigiPacketSenderV3::SendStartOfFramePacket()
{
  CIGI::V33::SoF startOfFramePacket;

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
  else
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
    startOfFramePacket.igStatusCode = 0;//normal operation
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

    startOfFramePacket.igStatusCode = highestError.nErrorCode.Value();
  }

  startOfFramePacket.timestamp = m_ImageGenerator.GetTimeStamp();
  startOfFramePacket.timestampValid = m_ImageGenerator.IsTimeStampValid();
  startOfFramePacket.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber().Value();

  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    SCigiStartOfFrame startOfFrame;
    startOfFrame.eVersion = ECigiVersion::VERSION_3_3;
    startOfFrame.databaseID = CigiDatabaseNumber(startOfFramePacket.dbNumber);
    startOfFrame.eIGMode = m_ImageGenerator.GetOperationMode();
    startOfFrame.bTimestampValid = startOfFramePacket.timestampValid;
    startOfFrame.bEarthReferenceModel = startOfFramePacket.earthReferenceModel != 0;
    startOfFrame.igFrameNumber = FrameNumber(startOfFramePacket.igFrameNumber);
    startOfFrame.microseconds = Microsecond(startOfFramePacket.timestamp);
    startOfFrame.lastHostFrameNumber = FrameNumber(startOfFramePacket.lastHostFrameNumber);
    startOfFrame.bOverframing = false;
    startOfFrame.bPagingTerrain = false;
    startOfFrame.bExcessiveVariableLengthData = false;
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(startOfFrame);
  }

  m_pBuffer->Write(startOfFramePacket);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
