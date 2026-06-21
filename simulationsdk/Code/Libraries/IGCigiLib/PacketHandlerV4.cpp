//Copyright SimBlocks LLC 2016-2026
#include "PacketHandlerV4.h"
#include "AnimationControlHandler.h"
#include "AccelerationControlHandler.h"
#include "ArticulatedPartHandler.h"
#include "AtmosphereControlHandler.h"
#include "CelestialSphereHandler.h"
#include "CigiLib/CigiConversions.h"
#include "IGCigiLib/CigiEntity.h"
#include "ComponentControlHandler.h"
#include "EntityControlHandler.h"
#include "EntityLib/EntityManager.h"
#include "EnvironmentalRegionHandler.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "CigiLib/CigiEntityTypes.h"
#include "ImageGenerator.h"
#include "MaritimeSurfaceConditionPacketHandler.h"
#include "SensorControlHandler.h"
#include "SymbolHandler.h"
#include "UtilitiesLib/UDPReceiveSocket.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/PacketLoggerWriter.h"
#include "UtilitiesLib/PacketLoggerReader.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/Endian.h"
#include "VelocityControlHandler.h"
#include "ViewHandler.h"
#include "WaveControlHandler.h"
#include "SymbolLib/SymbolTypes.h"
#include "TerrainHandler.h"
#include "CollisionControlHandler.h"
#include "MotionTrackerControlHandler.h"
#include "EarthReferenceModelHandler.h"
#include "libCIGI/Packets/4_0/AccelerationCtrl.h"
#include "libCIGI/Packets/4_0/AerosolConcentrationResponse.h"
#include "libCIGI/Packets/4_0/AnimationStopNotification.h"
#include "libCIGI/Packets/4_0/ArticulatedPartCtrl.h"
#include "libCIGI/Packets/4_0/AtmosphereCtrl.h"
#include "libCIGI/Packets/4_0/CelestialSphereCtrl.h"
#include "libCIGI/Packets/4_0/CollisionDetectionSegmentDefinition.h"
#include "libCIGI/Packets/4_0/CollisionDetectionSegmentNotification.h"
#include "libCIGI/Packets/4_0/CollisionDetectionVolumeDefinition.h"
#include "libCIGI/Packets/4_0/CollisionDetectionVolumeNotification.h"
#include "libCIGI/Packets/4_0/ComponentCtrl.h"
#include "libCIGI/Packets/4_0/ConformalClampedEntityPosition.h"
#include "libCIGI/Packets/4_0/EarthReferenceModelDefinition.h"
#include "libCIGI/Packets/4_0/EntityCtrl.h"
#include "libCIGI/Packets/4_0/EnvironmentalConditionsRequest.h"
#include "libCIGI/Packets/4_0/EnvironmentalRegionCtrl.h"
#include "libCIGI/Packets/4_0/EventNotification.h"
#include "libCIGI/Packets/4_0/HATHOTExtendedResponse.h"
#include "libCIGI/Packets/4_0/HATHOTRequest.h"
#include "libCIGI/Packets/4_0/HATHOTResponse.h"
#include "libCIGI/Packets/4_0/IGCtrl.h"
#include "libCIGI/Packets/4_0/IGMessage.h"
#include "libCIGI/Packets/4_0/LineOfSightExtendedResponse.h"
#include "libCIGI/Packets/4_0/LineOfSightResponse.h"
#include "libCIGI/Packets/4_0/LineOfSightSegmentRequest.h"
#include "libCIGI/Packets/4_0/LineOfSightVectorRequest.h"
#include "libCIGI/Packets/4_0/MaritimeSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/4_0/MaritimeSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/MotionTrackerCtrl.h"
#include "libCIGI/Packets/4_0/PositionRequest.h"
#include "libCIGI/Packets/4_0/PositionResponse.h"
#include "libCIGI/Packets/4_0/SensorCtrl.h"
#include "libCIGI/Packets/4_0/SensorExtendedResponse.h"
#include "libCIGI/Packets/4_0/SensorResponse.h"
#include "libCIGI/Packets/4_0/ShortArticulatedPartCtrl.h"
#include "libCIGI/Packets/4_0/ShortComponentCtrl.h"
#include "libCIGI/Packets/4_0/ShortSymbolCtrl.h"
#include "libCIGI/Packets/4_0/SoF.h"
#include "libCIGI/Packets/4_0/SymbolCircleDefinition.h"
#include "libCIGI/Packets/4_0/SymbolClone.h"
#include "libCIGI/Packets/4_0/SymbolCtrl.h"
#include "libCIGI/Packets/4_0/SymbolPolygonDefinition.h"
#include "libCIGI/Packets/4_0/SymbolSurfaceDefinition.h"
#include "libCIGI/Packets/4_0/SymbolTextDefinition.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/ViewCtrl.h"
#include "libCIGI/Packets/4_0/ViewDefinition.h"
#include "libCIGI/Packets/4_0/WaveCtrl.h"
#include "libCIGI/Packets/4_0/WeatherConditionsResponse.h"
#include "libCIGI/Packets/4_0/WeatherCtrl.h"
#include <algorithm>
#include <iostream>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::symbol;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiPacketHandlerV4::CCigiPacketHandlerV4(CCigiImageGenerator& imageGenerator, const std::string& sHostIP, int nHostToIgPort, int nIgToHostPort) : CCigiPacketHandler(imageGenerator, nHostToIgPort)
{
  m_PacketHandlerFunctions[ECigiOpCodeV4::ACCELERATION_CONTROL] = &CCigiPacketHandlerV4::ParseAccelerationControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ANIMATION_CONTROL] = &CCigiPacketHandlerV4::ParseAnimationControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ARTICULATED_PART_CONTROL] = &CCigiPacketHandlerV4::ParseArticulatedPartControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ATMOSPHERE_CONTROL] = &CCigiPacketHandlerV4::ParseAtmosphereControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::CELESTIAL_SPHERE_CONTROL] = &CCigiPacketHandlerV4::ParseCelestialSphereControlPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_DEFINITION] = &CCigiPacketHandlerV4::ParseCollisionDetectionSegmentDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_DEFINITION] = &CCigiPacketHandlerV4::ParseCollisionDetectionVolumeDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::COMPONENT_CONTROL] = &CCigiPacketHandlerV4::ParseComponentControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::CONFORMAL_CLAMPED_ENTITY_POSITION] = &CCigiPacketHandlerV4::ParseConformalClampedEntityPositionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::EARTH_REFERENCE_MODEL_DEFINITION] = &CCigiPacketHandlerV4::ParseEarthReferenceModelDefinitionPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::ENTITY_CONTROL] = &CCigiPacketHandlerV4::ParseEntityControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ENTITY_POSITION] = &CCigiPacketHandlerV4::ParseEntityPositionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ENVIRONMENTAL_CONDITIONS_REQUEST] = &CCigiPacketHandlerV4::ParseEnvironmentalConditionsRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ENVIRONMENTAL_REGION_CONTROL] = &CCigiPacketHandlerV4::ParseEnvironmentalRegionControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::HAT_HOT_REQUEST] = &CCigiPacketHandlerV4::ParseHatHotRequestPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::IG_CONTROL] = &CCigiPacketHandlerV4::ParseIgControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST] = &CCigiPacketHandlerV4::ParseLineOfSightSegmentRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::LINE_OF_SIGHT_VECTOR_REQUEST] = &CCigiPacketHandlerV4::ParseLineOfSightVectorRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL] = &CCigiPacketHandlerV4::ParseMaritimeSurfaceConditionsControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::MOTION_TRACKER_CONTROL] = &CCigiPacketHandlerV4::ParseMotionTrackerControlPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::POSITION_REQUEST] = &CCigiPacketHandlerV4::ParsePositionRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SENSOR_CONTROL] = &CCigiPacketHandlerV4::ParseSensorControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SHORT_ARTICULATED_PART_CONTROL] = &CCigiPacketHandlerV4::ParseShortArticulatedPartControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SHORT_COMPONENT_CONTROL] = &CCigiPacketHandlerV4::ParseShortComponentControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SHORT_SYMBOL_CONTROL] = &CCigiPacketHandlerV4::ParseShortSymbolControlPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_CIRCLE_DEFINITION] = &CCigiPacketHandlerV4::ParseSymbolCircleDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_CLONE] = &CCigiPacketHandlerV4::ParseSymbolClonePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_CONTROL] = &CCigiPacketHandlerV4::ParseSymbolControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_POLYGON_DEFINITION] = &CCigiPacketHandlerV4::ParseSymbolPolygonDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_SURFACE_DEFINITION] = &CCigiPacketHandlerV4::ParseSymbolSurfaceDefinitionPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_TEXT_DEFINITION] = &CCigiPacketHandlerV4::ParseSymbolTextDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_TEXTURED_CIRICLE_DEFINITION] = &CCigiPacketHandlerV4::ParseSymbolTexturedCircleDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SYMBOL_TEXTURED_POLYGON_DEFINITION] = &CCigiPacketHandlerV4::ParseSymbolTexturedPolygonDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL] = &CCigiPacketHandlerV4::ParseTerrestrialSurfaceConditionsControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::VELOCITY_CONTROL] = &CCigiPacketHandlerV4::ParseVelocityControlPacket;

  m_PacketHandlerFunctions[ECigiOpCodeV4::VIEW_CONTROL] = &CCigiPacketHandlerV4::ParseViewControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::VIEW_DEFINITION] = &CCigiPacketHandlerV4::ParseViewDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::WAVE_CONTROL] = &CCigiPacketHandlerV4::ParseWaveControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::WEATHER_CONTROL] = &CCigiPacketHandlerV4::ParseWeatherControlPacket;
}

CCigiPacketHandlerV4::~CCigiPacketHandlerV4()
{
}

EAnimationLoopMode ConvertToAnimationLoopMode(CIGI::V40::AnimationCtrl::AnimationLoopMode animationLoopMode)
{
  if (animationLoopMode == CIGI::V40::AnimationCtrl::AnimationLoopMode::eAnimationLoopMode_OneShot)
  {
    return EAnimationLoopMode::ONE_SHOT;
  }
  else if (animationLoopMode == CIGI::V40::AnimationCtrl::AnimationLoopMode::eAnimationLoopMode_Continuous)
  {
    return EAnimationLoopMode::CONTINUOUS;
  }

  return EAnimationLoopMode::UNKNOWN;
}

EAnimationFramePositionReset ConvertToAnimationFramePositionReset(CIGI::V40::AnimationCtrl::AnimationFramePositionReset animationFramePositionReset)
{
  if (animationFramePositionReset == CIGI::V40::AnimationCtrl::AnimationFramePositionReset::eAnimationFramePositionReset_Continue)
  {
    return EAnimationFramePositionReset::CONTINUE;
  }
  else if (animationFramePositionReset == CIGI::V40::AnimationCtrl::AnimationFramePositionReset::eAnimationFramePositionReset_Reset)
  {
    return EAnimationFramePositionReset::RESET;
  }

  return EAnimationFramePositionReset::UNKNOWN;
}

EAnimationState ConvertToAnimationState(CIGI::V40::AnimationCtrl::AnimationState animationState)
{
  if (animationState == CIGI::V40::AnimationCtrl::AnimationState::eAnimationState_Stop)
  {
    return EAnimationState::STOP;
  }
  else if (animationState == CIGI::V40::AnimationCtrl::AnimationState::eAnimationState_Play)
  {
    return EAnimationState::PLAY;
  }

  return EAnimationState::UNKNOWN;
}

EObjectCoordinateSystem ConvertToEntityCoordinateSystem(CIGI::V40::AccelerationCtrl::CoordinateSystem coordSystem)
{
  if (coordSystem == CIGI::V40::AccelerationCtrl::CoordinateSystem::eCoordinateSystem_Local)
  {
    return EObjectCoordinateSystem::LOCAL;
  }
  else if (coordSystem == CIGI::V40::AccelerationCtrl::CoordinateSystem::eCoordinateSystem_WorldParent)
  {
    return EObjectCoordinateSystem::WORLD;
  }

  return EObjectCoordinateSystem::UNKNOWN;
}

void CCigiPacketHandlerV4::CheckForByteSwap(const SCigiPacketHeaderV4& packetHeader)
{
  //check for byte swapping in CIGI 4
  if (((packetHeader.nPacketSize & 0xFF00) == 0) && packetHeader.eOpCode == ECigiOpCodeV4::IG_CONTROL)
  {
    m_bByteSwap = false;
  }
  else
  {
    uint16_t nOpCode = static_cast<uint16_t>(packetHeader.eOpCode);
    CEndian::Swap(nOpCode);
    ECigiOpCodeV4 eOpCode = static_cast<ECigiOpCodeV4>(nOpCode);

    if (((packetHeader.nPacketSize & 0xFF00) != 0) && eOpCode == ECigiOpCodeV4::IG_CONTROL)
    {
      m_bByteSwap = true;
    }
  }
}

void CCigiPacketHandlerV4::ParseAccelerationControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::AccelerationCtrl accelerationCtrl;
  memcpy(&accelerationCtrl, pBuffer, sizeof(CIGI::V40::AccelerationCtrl));

  if (m_bByteSwap)
  {
    accelerationCtrl.doByteSwapping();
  }

  bool bApplyToArticulatedPart = accelerationCtrl.applyToArticulatedPart;
  if (bApplyToArticulatedPart)
  {
    SCigiArticulatedPartAcceleration acceleration;
    acceleration.angularAcceleration.roll = Degrees(accelerationCtrl.rollAngularAcceleration);
    acceleration.angularAcceleration.pitch = Degrees(accelerationCtrl.pitchAngularAcceleration);
    acceleration.angularAcceleration.yaw = Degrees(accelerationCtrl.yawAngularAcceleration);

    acceleration.linearAcceleration[0] = accelerationCtrl.xLinearAcceleration;
    acceleration.linearAcceleration[1] = accelerationCtrl.yLinearAcceleration;
    acceleration.linearAcceleration[2] = accelerationCtrl.zLinearAcceleration;

    acceleration.eCoordinateSystem = ConvertToEntityCoordinateSystem(static_cast<CIGI::V40::AccelerationCtrl::CoordinateSystem>(accelerationCtrl.coordinateSystem));
    acceleration.entityID = EntityID(accelerationCtrl.entityId);
    acceleration.articulatedPartID = ArticulatedPartID(accelerationCtrl.articulatedPartId);

    m_pAccelerationControlHandler->HandleArticulatedPartAccelerationControl(acceleration);
  }
  else
  {
    SCigiEntityAcceleration acceleration;
    acceleration.angularAcceleration.roll = Degrees(accelerationCtrl.rollAngularAcceleration);
    acceleration.angularAcceleration.pitch = Degrees(accelerationCtrl.pitchAngularAcceleration);
    acceleration.angularAcceleration.yaw = Degrees(accelerationCtrl.yawAngularAcceleration);

    acceleration.linearAcceleration[0] = accelerationCtrl.xLinearAcceleration;
    acceleration.linearAcceleration[1] = accelerationCtrl.yLinearAcceleration;
    acceleration.linearAcceleration[2] = accelerationCtrl.zLinearAcceleration;

    acceleration.eCoordinateSystem = ConvertToEntityCoordinateSystem(static_cast<CIGI::V40::AccelerationCtrl::CoordinateSystem>(accelerationCtrl.coordinateSystem));
    acceleration.entityID = EntityID(accelerationCtrl.entityId);

    m_pAccelerationControlHandler->HandleEntityAccelerationControl(acceleration);
  }
}

void CCigiPacketHandlerV4::ParseAnimationControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::AnimationCtrl animationControlV4;
  memcpy(&animationControlV4, pBuffer, sizeof(CIGI::V40::AnimationCtrl));

  if (m_bByteSwap)
  {
    animationControlV4.doByteSwapping();
  }

  SCigiAnimationControl animationControl;
  animationControl.animationID = AnimationID(animationControlV4.animationId);//Animation ID new version 4.0
  animationControl.eAnimationLoopMode = ConvertToAnimationLoopMode(static_cast<CIGI::V40::AnimationCtrl::AnimationLoopMode>(animationControlV4.animationLoopMode));
  animationControl.bInheritAlpha = animationControlV4.inheritAlpha == CIGI::V40::AnimationCtrl::InheritAlpha::eInheritAlpha_Inherited ? true : false;
  animationControl.eAnimationFramePositionReset = ConvertToAnimationFramePositionReset(static_cast<CIGI::V40::AnimationCtrl::AnimationFramePositionReset>(animationControlV4.animationFramePositionReset));
  animationControl.eAnimationState = ConvertToAnimationState(static_cast<CIGI::V40::AnimationCtrl::AnimationState>(animationControlV4.animationState));
  animationControl.alpha = ConvertAlphaToPercentage(animationControlV4.alpha);
  animationControl.fAnimationSpeed = animationControlV4.animationSpeed;
  animationControl.entityID = EntityID(animationControlV4.entityId);

  m_pAnimationControlHandler->HandleCigiEntityAnimation(animationControl);
}

void CCigiPacketHandlerV4::ParseArticulatedPartControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::ArticulatedPartCtrl articulatedPartControlV4;
  memcpy(&articulatedPartControlV4, pBuffer, sizeof(CIGI::V40::ArticulatedPartCtrl));

  if (m_bByteSwap)
  {
    articulatedPartControlV4.doByteSwapping();
  }

  // verify entity exists
  auto entityID = EntityID(articulatedPartControlV4.entityId);
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entityID))
  {
    return;
  }

  SCigiArticulatedPart articulatedPart;
  articulatedPart.entityID = entityID;
  articulatedPart.articulatedPartID = ArticulatedPartID(articulatedPartControlV4.articulatedPartId);
  articulatedPart.bEnabled = articulatedPartControlV4.articulatedPartEnable;
  articulatedPart.bOffsetEnabled[0] = articulatedPartControlV4.xOffsetEnable;
  articulatedPart.bOffsetEnabled[1] = articulatedPartControlV4.yOffsetEnable;
  articulatedPart.bOffsetEnabled[2] = articulatedPartControlV4.zOffsetEnable;
  articulatedPart.offset[0] = articulatedPartControlV4.xOffset;
  articulatedPart.offset[1] = articulatedPartControlV4.yOffset;
  articulatedPart.offset[2] = articulatedPartControlV4.zOffset;
  articulatedPart.bYawEnabled = articulatedPartControlV4.yawEnable;
  articulatedPart.bPitchEnabled = articulatedPartControlV4.pitchEnable;
  articulatedPart.bRollEnabled = articulatedPartControlV4.rollEnable;
  articulatedPart.rotation.yaw = Degrees(articulatedPartControlV4.yaw);
  articulatedPart.rotation.pitch = Degrees(articulatedPartControlV4.pitch);
  articulatedPart.rotation.roll = Degrees(articulatedPartControlV4.roll);
  m_pArticulatedPartHandler->Handle(articulatedPart);
}

void CCigiPacketHandlerV4::ParseAtmosphereControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::AtmosphereCtrl atmosphereControlV4;
  memcpy(&atmosphereControlV4, pBuffer, sizeof(CIGI::V40::AtmosphereCtrl));

  if (m_bByteSwap)
  {
    atmosphereControlV4.doByteSwapping();
  }

  SAtmosphere atmosphere;
  atmosphere.fAirTemperature = TemperatureCelsius(atmosphereControlV4.globalAirTemperature);
  atmosphere.fBarometricPressure = atmosphereControlV4.globalBarometricPressure;
  atmosphere.fHorizontalWindSpeed = atmosphereControlV4.globalHorizontalWindSpeed;
  atmosphere.fVerticalWindSpeed = atmosphereControlV4.globalVerticalWindSpeed;
  atmosphere.fHumidity = atmosphereControlV4.globalHumidity;
  atmosphere.fVisibilityRange = atmosphereControlV4.globalVisibilityRange;
  atmosphere.fWindDirection = atmosphereControlV4.globalWindDirection;
  m_pAtmosphereControlHandler->Handle(atmosphereControlV4.atmosphericModelEnable, atmosphere);
}

void CCigiPacketHandlerV4::ParseCelestialSphereControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::CelestialSphereCtrl celestialSphereControl;
  memcpy(&celestialSphereControl, pBuffer, sizeof(CIGI::V40::CelestialSphereCtrl));

  if (m_bByteSwap)
  {
    celestialSphereControl.doByteSwapping();
  }

  SCelestialSphere celestialSphere;
  celestialSphere.bEphemerisEnabled = celestialSphereControl.continuousTimeOfDayEnable;
  celestialSphere.bSunEnabled = celestialSphereControl.sunEnable;
  celestialSphere.bMoonEnabled = celestialSphereControl.moonEnable;
  celestialSphere.bStarsEnabled = celestialSphereControl.starFieldEnable;
  celestialSphere.fStarIntensity = celestialSphereControl.starFieldIntensity;
  m_pCelestialSphereHandler->Handle(celestialSphere);

  if (celestialSphereControl.dateTimeValid)
  {
    STime time;
    time.hour = Hour(celestialSphereControl.hour);
    time.minute = Minute(celestialSphereControl.minute);
    time.seconds = Second(celestialSphereControl.seconds);

    SDate date;
    date.year = Year(celestialSphereControl.year());
    CigiMonth month = CigiMonth(celestialSphereControl.month());
    date.month = ConvertToMonth(month);
    date.day = Day(celestialSphereControl.day());

    m_pCelestialSphereHandler->SetDateTime(date, time);
  }
}

void CCigiPacketHandlerV4::ParseCollisionDetectionSegmentDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::CollisionDetectionSegmentDefinition collisionDetectionSegmentDefinitionV4;
  memcpy(&collisionDetectionSegmentDefinitionV4, pBuffer, sizeof(CIGI::V40::CollisionDetectionSegmentDefinition));

  if (m_bByteSwap)
  {
    collisionDetectionSegmentDefinitionV4.doByteSwapping();
  }

  SCollisionDetectionSegmentDefinition collisionDetectionSegmentDefinition;
  collisionDetectionSegmentDefinition.beg[0] = collisionDetectionSegmentDefinitionV4.x1;
  collisionDetectionSegmentDefinition.beg[1] = collisionDetectionSegmentDefinitionV4.y1;
  collisionDetectionSegmentDefinition.beg[2] = collisionDetectionSegmentDefinitionV4.z1;
  collisionDetectionSegmentDefinition.end[0] = collisionDetectionSegmentDefinitionV4.x2;
  collisionDetectionSegmentDefinition.end[1] = collisionDetectionSegmentDefinitionV4.y2;
  collisionDetectionSegmentDefinition.end[2] = collisionDetectionSegmentDefinitionV4.z2;
  collisionDetectionSegmentDefinition.bSegmentEnabled = collisionDetectionSegmentDefinitionV4.segmentEnable;
  collisionDetectionSegmentDefinition.entityID = EntityID(collisionDetectionSegmentDefinitionV4.entityId);
  collisionDetectionSegmentDefinition.nMaterialMask = collisionDetectionSegmentDefinitionV4.materialMask;
  collisionDetectionSegmentDefinition.segmentID = SegmentID(collisionDetectionSegmentDefinitionV4.segmentId);

  m_pCollisionControlHandler->HandleCigiCollisionDetectionSegmentDefinition(collisionDetectionSegmentDefinition);
}

void CCigiPacketHandlerV4::ParseCollisionDetectionVolumeDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::CollisionDetectionVolumeDefinition collisionDetectionVolumeDefinitionV4;
  memcpy(&collisionDetectionVolumeDefinitionV4, pBuffer, sizeof(CIGI::V40::CollisionDetectionVolumeDefinition));

  if (m_bByteSwap)
  {
    collisionDetectionVolumeDefinitionV4.doByteSwapping();
  }

  EVolumeType eVolumeType = ConvertCollisionVolumeType(static_cast<CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType>(collisionDetectionVolumeDefinitionV4.volumeType));

  if (eVolumeType == sbio::cigi::EVolumeType::CUBOID)
  {
    sbio::cigi::SCollisionDetectionCuboidDefinition collisionDetectionVolumeDefinition;
    collisionDetectionVolumeDefinition.entityID = EntityID(collisionDetectionVolumeDefinitionV4.entityId);
    collisionDetectionVolumeDefinition.bVolumeEnabled = collisionDetectionVolumeDefinitionV4.volumeEnable;
    collisionDetectionVolumeDefinition.offset[0] = collisionDetectionVolumeDefinitionV4.x;
    collisionDetectionVolumeDefinition.offset[1] = collisionDetectionVolumeDefinitionV4.y;
    collisionDetectionVolumeDefinition.offset[2] = collisionDetectionVolumeDefinitionV4.z;
    collisionDetectionVolumeDefinition.volumeID = VolumeID(collisionDetectionVolumeDefinitionV4.volumeId);

    collisionDetectionVolumeDefinition.fDepth = collisionDetectionVolumeDefinitionV4.depth;
    collisionDetectionVolumeDefinition.rotation.yaw = Degrees(collisionDetectionVolumeDefinitionV4.yaw);
    collisionDetectionVolumeDefinition.rotation.pitch = Degrees(collisionDetectionVolumeDefinitionV4.pitch);
    collisionDetectionVolumeDefinition.rotation.roll = Degrees(collisionDetectionVolumeDefinitionV4.roll);
    collisionDetectionVolumeDefinition.fWidth = collisionDetectionVolumeDefinitionV4.width;
    collisionDetectionVolumeDefinition.fHeight = collisionDetectionVolumeDefinitionV4.heightRadius;

    m_pCollisionControlHandler->HandleCigiCollisionCuboidDefinition(collisionDetectionVolumeDefinition);
  }
  else if (eVolumeType == sbio::cigi::EVolumeType::SPHERE)
  {
    sbio::cigi::SCollisionDetectionSphereDefinition collisionDetectionVolumeDefinition;
    collisionDetectionVolumeDefinition.entityID = EntityID(collisionDetectionVolumeDefinitionV4.entityId);
    collisionDetectionVolumeDefinition.bVolumeEnabled = collisionDetectionVolumeDefinitionV4.volumeEnable;
    collisionDetectionVolumeDefinition.offset[0] = collisionDetectionVolumeDefinitionV4.x;
    collisionDetectionVolumeDefinition.offset[1] = collisionDetectionVolumeDefinitionV4.y;
    collisionDetectionVolumeDefinition.offset[2] = collisionDetectionVolumeDefinitionV4.z;
    collisionDetectionVolumeDefinition.volumeID = VolumeID(collisionDetectionVolumeDefinitionV4.volumeId);

    collisionDetectionVolumeDefinition.fRadius = collisionDetectionVolumeDefinitionV4.heightRadius;

    m_pCollisionControlHandler->HandleCigiCollisionSphereDefinition(collisionDetectionVolumeDefinition);
  }
}

void CCigiPacketHandlerV4::ParseComponentControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::ComponentCtrl componentControlV4;
  memcpy(&componentControlV4, pBuffer, sizeof(CIGI::V40::ComponentCtrl));

  if (m_bByteSwap)
  {
    componentControlV4.doByteSwapping();
  }

  SCigiComponentControl componentControl;
  componentControl.key.componentID = CigiComponentID(componentControlV4.componentId);
  componentControl.key.componentClassID = CigiComponentClassID(static_cast<uint8_t>(componentControlV4.componentClass));
  componentControl.key.nInstanceID = componentControlV4.instanceId;
  componentControl.state.nComponentState = componentControlV4.componentState;
  componentControl.state.componentData[0] = componentControlV4.componentData[0];
  componentControl.state.componentData[1] = componentControlV4.componentData[1];
  componentControl.state.componentData[2] = componentControlV4.componentData[2];
  componentControl.state.componentData[3] = componentControlV4.componentData[3];
  componentControl.state.componentData[4] = componentControlV4.componentData[4];
  componentControl.state.componentData[5] = componentControlV4.componentData[5];

  CComponentDataParser<CIGI::V40::ComponentCtrl> componentDataParser(componentControlV4);
  m_pComponentControlHandler->Handle(componentControl, &componentDataParser);
}

void CCigiPacketHandlerV4::ParseConformalClampedEntityPositionPacket(uint8_t* pBuffer)
{
  CIGI::V40::ConformalClampedEntityPosition conformalClampedEntityControl;
  memcpy(&conformalClampedEntityControl, pBuffer, sizeof(CIGI::V40::ConformalClampedEntityPosition));

  if (m_bByteSwap)
  {
    conformalClampedEntityControl.doByteSwapping();
  }

  SCigiConformalClampedEntityPosition conformalClampedEntityPosition;
  conformalClampedEntityPosition.entityID = EntityID(conformalClampedEntityControl.entityId);
  conformalClampedEntityPosition.fYaw = Degrees(conformalClampedEntityControl.yaw);
  conformalClampedEntityPosition.latitude = Latitude(conformalClampedEntityControl.latitude);
  conformalClampedEntityPosition.longitude = Longitude(conformalClampedEntityControl.longitude);

  m_pEntityControlHandler->HandleCigiConformalClampedEntityPosition(conformalClampedEntityPosition);
}

void CCigiPacketHandlerV4::ParseEarthReferenceModelDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::EarthReferenceModelDefinition earthRefModelV4;
  memcpy(&earthRefModelV4, pBuffer, sizeof(CIGI::V40::EarthReferenceModelDefinition));

  if (m_bByteSwap)
  {
    earthRefModelV4.doByteSwapping();
  }

  SCigiEarthReferenceModel earthReferenceModel;
  if (earthRefModelV4.customERMEnable)
  {
    earthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::HOST_DEFINED;
    earthReferenceModel.fEquatorialRadius = earthRefModelV4.equatorialRadius;
    earthReferenceModel.fFlattening = earthRefModelV4.flattening;
    m_pEarthReferenceModelHandler->Handle(earthReferenceModel);
  }
  else
  {
    earthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::WGS84;

    //If this parameter is set to Disable (0), the IG shall use the WGS 84 reference model
    //and all other parameters in this packet shall be ignored.
    earthReferenceModel.fEquatorialRadius = 6378137.0;
    earthReferenceModel.fFlattening = 1.0 / 298.257223563;
    m_pEarthReferenceModelHandler->Handle(earthReferenceModel);
  }
}

void CCigiPacketHandlerV4::ParseEntityControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::EntityCtrl entityControlV4;
  memcpy(&entityControlV4, pBuffer, sizeof(CIGI::V40::EntityCtrl));

  if (m_bByteSwap)
  {
    entityControlV4.doByteSwapping();
  }

  SEntityControl entityControl;
  entityControl.alpha = entityControlV4.alpha;
  entityControl.bCollisionReportingEnabled = entityControlV4.collisionReportingEnable;
  entityControl.bInheritAlpha = entityControlV4.inheritAlpha;
  entityControl.bSmoothingEnabled = entityControlV4.smoothingEnable;
  entityControl.entityID = EntityID(entityControlV4.entityId);
  entityControl.eState = ConvertToActiveState(static_cast<CIGI::V40::EntityCtrl::EntityState>(entityControlV4.entityState));

  if (entityControlV4.extendedEntityType == CIGI::V40::EntityCtrl::ExtendedEntityType::eExtendedEntityType_Short)
  {
    entityControl.eExtendedEntityType = EExtendedEntityType::SHORT;
    ShortEntityTypeID entityTypeID(entityControlV4.entityCountryOrShortEntityType);
    entityControl.entityType = g_CigiLibGlobals.pCigiEntityTypes->GetExtendedEntityType(entityTypeID);
    // Also record the short entity type itself
    entityControl.shortEntityTypeID = entityTypeID;
  }
  else if (entityControlV4.extendedEntityType == CIGI::V40::EntityCtrl::ExtendedEntityType::eExtendedEntityType_Extended)
  {
    entityControl.eExtendedEntityType = EExtendedEntityType::EXTENDED;
    entityControl.entityType.entityCategoryID = SisoEntityCategoryID(entityControlV4.entityCategory);
    entityControl.entityType.entityCountryID = SisoEntityCountryID(entityControlV4.entityCountryOrShortEntityType);
    entityControl.entityType.entityDomainID = SisoEntityDomainID(entityControlV4.entityDomain);
    entityControl.entityType.entityExtraID = SisoEntityExtraID(entityControlV4.entityExtra);
    entityControl.entityType.entityKindID = SisoEntityKindID(entityControlV4.entityKind);
    entityControl.entityType.entitySpecificID = SisoEntitySpecificID(entityControlV4.entitySpecific);
    entityControl.entityType.entitySubCategoryID = SisoEntitySubCategoryID(entityControlV4.entitySubcategory);
  }

  m_pEntityControlHandler->HandleCigiEntityControl(entityControl, false);
}

EClamp ConvertClamp(CIGI::V40::EntityPosition::GroundOceanClamp eClampState)
{
  switch (eClampState)
  {
  case CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_NoClamp:
    return EClamp::NONE;
    break;
  case CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_NonConformal:
    return EClamp::NON_CONFORMAL;
    break;
  case CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_Conformal:
    return EClamp::CONFORMAL;
    break;
  }

  return EClamp::UNKNOWN;
}

void CCigiPacketHandlerV4::ParseEntityPositionPacket(uint8_t* pBuffer)
{
  CIGI::V40::EntityPosition entityPositionV4;
  memcpy(&entityPositionV4, pBuffer, sizeof(CIGI::V40::EntityPosition));

  if (m_bByteSwap)
  {
    entityPositionV4.doByteSwapping();
  }

  auto EntityId = EntityID(entityPositionV4.entityId);
  auto parentID = EntityID(entityPositionV4.parentId);
  auto attachState = static_cast<CIGI::V40::EntityPosition::AttachState>(entityPositionV4.attachState);
  auto clamp = static_cast<CIGI::V40::EntityPosition::GroundOceanClamp>(entityPositionV4.groundOceanClamp);

  sbio::math::Vec3 position;
  position[0] = entityPositionV4.latitudeXOffset;
  position[1] = entityPositionV4.longitudeYOffset;
  position[2] = entityPositionV4.altitudeZOffset;

  TCigiBodyEulerRotation rotation;
  rotation.yaw = Degrees(entityPositionV4.yaw);
  rotation.pitch = Degrees(entityPositionV4.pitch);
  rotation.roll = Degrees(entityPositionV4.roll);

  m_pEntityControlHandler->HandleCigiEntityPosition(EntityId, parentID, ConvertAttachState(attachState), ConvertClamp(clamp), position, rotation);
}

void CCigiPacketHandlerV4::ParseEnvironmentalConditionsRequestPacket(uint8_t* pBuffer)
{
  CIGI::V40::EnvironmentalConditionsRequest environmentalRegionConditionsRequestV4;
  memcpy(&environmentalRegionConditionsRequestV4, pBuffer, sizeof(CIGI::V40::EnvironmentalConditionsRequest));

  if (m_bByteSwap)
  {
    environmentalRegionConditionsRequestV4.doByteSwapping();
  }

  SEnvironmentalConditionsRequest request;
  request.bAerosolConcentrationsRequest = environmentalRegionConditionsRequestV4.requestType & CIGI::V40::EnvironmentalConditionsRequest::RequestType::eRequestType_AerosolConcentrations;
  request.bMaritimeSurfaceConditionsRequest = environmentalRegionConditionsRequestV4.requestType & CIGI::V40::EnvironmentalConditionsRequest::RequestType::eRequestType_MaritimeSurfaceConditions;
  request.bTerrestrialSurfaceConditionsRequest = environmentalRegionConditionsRequestV4.requestType & CIGI::V40::EnvironmentalConditionsRequest::RequestType::eRequestType_TerrestrialSurfaceConditions;
  request.bWeatherConditionsRequest = environmentalRegionConditionsRequestV4.requestType & CIGI::V40::EnvironmentalConditionsRequest::RequestType::eRequestType_WeatherConditions;

  request.geodeticCoordinates.latitude = Latitude(environmentalRegionConditionsRequestV4.latitude);
  request.geodeticCoordinates.longitude = Longitude(environmentalRegionConditionsRequestV4.longitude);
  request.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(environmentalRegionConditionsRequestV4.altitude);

  request.nRequestID = environmentalRegionConditionsRequestV4.requestId;
  m_pEnvironmentalRegionHandler->Handle(request);
}

void CCigiPacketHandlerV4::ParseEnvironmentalRegionControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::EnvironmentalRegionCtrl environmentalRegionControlV4;
  memcpy(&environmentalRegionControlV4, pBuffer, sizeof(CIGI::V40::EnvironmentalRegionCtrl));

  if (m_bByteSwap)
  {
    environmentalRegionControlV4.doByteSwapping();
  }

  SCigiEnvironmentalRegion environmentalRegion;
  environmentalRegion.regionID = RegionID(environmentalRegionControlV4.regionId);
  environmentalRegion.eRegionState = ConvertRegion(static_cast<CIGI::V40::EnvironmentalRegionCtrl::RegionState>(environmentalRegionControlV4.regionState));
  environmentalRegion.eMergeWeatherProperties = ConvertMergeState(static_cast<CIGI::V40::EnvironmentalRegionCtrl::Merge>(environmentalRegionControlV4.mergeWeatherProperties));
  environmentalRegion.eMergeAerosolConcentrations = ConvertMergeState(static_cast<CIGI::V40::EnvironmentalRegionCtrl::Merge>(environmentalRegionControlV4.mergeAerosolConcentrations));
  environmentalRegion.eMergeMaritimeSurfaceConditions = ConvertMergeState(static_cast<CIGI::V40::EnvironmentalRegionCtrl::Merge>(environmentalRegionControlV4.mergeMaritimeSurfaceConditions));
  environmentalRegion.eMergeTerrestrialSurfaceConditions = ConvertMergeState(static_cast<CIGI::V40::EnvironmentalRegionCtrl::Merge>(environmentalRegionControlV4.mergeTerrestrialSurfaceConditions));
  environmentalRegion.latitude = Latitude(environmentalRegionControlV4.latitude);
  environmentalRegion.longitude = Longitude(environmentalRegionControlV4.longitude);
  environmentalRegion.size[0] = environmentalRegionControlV4.sizeX;
  environmentalRegion.size[1] = environmentalRegionControlV4.sizeY;
  environmentalRegion.fCornerRadius = environmentalRegionControlV4.cornerRadius;
  environmentalRegion.fRotation = Degrees180(environmentalRegionControlV4.rotation);
  environmentalRegion.fTransition = environmentalRegionControlV4.transitionPerimeter;
  m_pEnvironmentalRegionHandler->Handle(environmentalRegion);
}

void CCigiPacketHandlerV4::ParseHatHotRequestPacket(uint8_t* pBuffer)
{
  CIGI::V40::HATHOTRequest hatHotReqV4;
  memcpy(&hatHotReqV4, pBuffer, sizeof(CIGI::V40::HATHOTRequest));

  if (m_bByteSwap)
  {
    hatHotReqV4.doByteSwapping();
  }

  auto requestType = ConvertRequestType(static_cast<CIGI::V40::HATHOTRequest::Type>(hatHotReqV4.requestType));
  auto coordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::HATHOTRequest::CoordinateSystem>(hatHotReqV4.coordinateSystem));

  if (coordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
  {
    SHATHOTGlobalRequest data;
    data.requestID = HATHOTID(hatHotReqV4.HATHOTId);
    data.updatePeriod = UpdatePeriod(hatHotReqV4.updatePeriod);
    data.geodeticCoordinates.latitude = Latitude(hatHotReqV4.latitudeXOffset);
    data.geodeticCoordinates.longitude = Longitude(hatHotReqV4.longitudeYOffset);
    data.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(hatHotReqV4.altitudeZOffset);
    data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
    data.eRequestType = requestType;
    m_pTerrainHandler->Handle(data);
  }
  else
  {
    SHATHOTEntityRequest data;
    data.requestID = HATHOTID(hatHotReqV4.HATHOTId);
    data.updatePeriod = UpdatePeriod(hatHotReqV4.updatePeriod);
    data.entityID = EntityID(hatHotReqV4.entityId);
    data.offset[0] = hatHotReqV4.latitudeXOffset;
    data.offset[1] = hatHotReqV4.longitudeYOffset;
    data.offset[2] = hatHotReqV4.altitudeZOffset;
    data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
    data.eRequestType = requestType;
    m_pTerrainHandler->Handle(data);
  }
}

void CCigiPacketHandlerV4::ParseIgControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::IGCtrl igControlV4;
  memcpy(&igControlV4, pBuffer, sizeof(CIGI::V40::IGCtrl));

  if (m_bByteSwap)
  {
    igControlV4.doByteSwapping();
  }

  SCigiIgControl igControl;
  igControl.bEntityTypeSubstitutionEnabled = igControlV4.entityTypeSubstitutionEnable;
  igControl.bSmoothingEnabled = igControlV4.smoothingEnable;
  igControl.bTimestampValid = igControlV4.timestampValid;
  igControl.databaseNumber = CigiDatabaseNumber(igControlV4.dbNumber);
  igControl.eIgMode = ConvertToIGMode(static_cast<CIGI::V40::IGCtrl::IGMode>(igControlV4.igMode));
  igControl.hostFrameNumber = FrameNumber(igControlV4.hostFrameNumber);
  igControl.lastIgFrameNumber = FrameNumber(igControlV4.lastIGFrameNumber);
  igControl.timestamp = igControlV4.timestamp;

  Handle(igControl);
}

void CCigiPacketHandlerV4::ParseLineOfSightSegmentRequestPacket(uint8_t* pBuffer)
{
  CIGI::V40::LineOfSightSegmentRequest losRequest;
  memcpy(&losRequest, pBuffer, sizeof(CIGI::V40::LineOfSightSegmentRequest));

  if (m_bByteSwap)
  {
    losRequest.doByteSwapping();
  }

  ETopLevelCoordinateSystem eSourceCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.sourcePointCoordinateSystem));
  ETopLevelCoordinateSystem eDestinationCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.destinationPointCoordinateSystem));

  if (losRequest.requestType == 0)//basic
  {
    if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightSegmentRequestGeodeticToGeodeticBasic data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceGeodeticCoordinates.latitude = Latitude(losRequest.sourceLatitudeXOffset);
      data.sourceGeodeticCoordinates.longitude = Longitude(losRequest.sourceLongitudeYOffset);
      data.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.sourceAltitudeZOffset);
      data.destinationGeodeticCoordinates.latitude = Latitude(losRequest.destinationLatitudeXOffset);
      data.destinationGeodeticCoordinates.longitude = Longitude(losRequest.destinationLongitudeYOffset);
      data.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.destinationAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightSegmentRequestGeodeticToEntityBasic data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceGeodeticCoordinates.latitude = Latitude(losRequest.sourceLatitudeXOffset);
      data.sourceGeodeticCoordinates.longitude = Longitude(losRequest.sourceLongitudeYOffset);
      data.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.sourceAltitudeZOffset);
      data.destinationOffset[0] = losRequest.destinationLatitudeXOffset;
      data.destinationOffset[1] = losRequest.destinationLongitudeYOffset;
      data.destinationOffset[2] = losRequest.destinationAltitudeZOffset;
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.destinationEntityID = EntityID(losRequest.destinationEntityId);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightSegmentRequestEntityToGeodeticBasic data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.sourceEntityId);
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.destinationGeodeticCoordinates.latitude = Latitude(losRequest.destinationLatitudeXOffset);
      data.destinationGeodeticCoordinates.longitude = Longitude(losRequest.destinationLongitudeYOffset);
      data.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.destinationAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightSegmentRequestEntityToEntityBasic data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.sourceEntityId);
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.destinationOffset[0] = losRequest.destinationLatitudeXOffset;
      data.destinationOffset[1] = losRequest.destinationLongitudeYOffset;
      data.destinationOffset[2] = losRequest.destinationAltitudeZOffset;
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.destinationEntityID = EntityID(losRequest.destinationEntityId);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
  }
  else//extended
  {
    if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightSegmentRequestGeodeticToGeodeticExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceGeodeticCoordinates.latitude = Latitude(losRequest.sourceLatitudeXOffset);
      data.sourceGeodeticCoordinates.longitude = Longitude(losRequest.sourceLongitudeYOffset);
      data.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.sourceAltitudeZOffset);
      data.destinationGeodeticCoordinates.latitude = Latitude(losRequest.destinationLatitudeXOffset);
      data.destinationGeodeticCoordinates.longitude = Longitude(losRequest.destinationLongitudeYOffset);
      data.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.destinationAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      data.eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightSegmentRequestGeodeticToEntityExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceGeodeticCoordinates.latitude = Latitude(losRequest.sourceLatitudeXOffset);
      data.sourceGeodeticCoordinates.longitude = Longitude(losRequest.sourceLongitudeYOffset);
      data.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.sourceAltitudeZOffset);
      data.destinationOffset[0] = losRequest.destinationLatitudeXOffset;
      data.destinationOffset[1] = losRequest.destinationLongitudeYOffset;
      data.destinationOffset[2] = losRequest.destinationAltitudeZOffset;
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.destinationEntityID = EntityID(losRequest.destinationEntityId);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      data.eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightSegmentRequestEntityToGeodeticExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.sourceEntityId);
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.destinationGeodeticCoordinates.latitude = Latitude(losRequest.destinationLatitudeXOffset);
      data.destinationGeodeticCoordinates.longitude = Longitude(losRequest.destinationLongitudeYOffset);
      data.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.destinationAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      data.eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightSegmentRequestEntityToEntityExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.sourceEntityId);
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.destinationOffset[0] = losRequest.destinationLatitudeXOffset;
      data.destinationOffset[1] = losRequest.destinationLongitudeYOffset;
      data.destinationOffset[2] = losRequest.destinationAltitudeZOffset;
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.destinationEntityID = EntityID(losRequest.destinationEntityId);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      data.eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));
      m_pTerrainHandler->Handle(data);
    }
  }
}

void CCigiPacketHandlerV4::ParseLineOfSightVectorRequestPacket(uint8_t* pBuffer)
{
  CIGI::V40::LineOfSightVectorRequest losRequest;
  memcpy(&losRequest, pBuffer, sizeof(CIGI::V40::LineOfSightVectorRequest));

  if (m_bByteSwap)
  {
    losRequest.doByteSwapping();
  }

  ETopLevelCoordinateSystem eSourcePointCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightVectorRequest::CoordinateSystem>(losRequest.sourcePointCoordinateSystem));

  if (losRequest.requestType == CIGI::V40::LineOfSightVectorRequest::RequestType::eRequestType_Basic)
  {
    if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightVectorRequestEntityBasic data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.entityId);
      data.azimuth = Degrees180(losRequest.azimuth);
      data.elevation = Degrees90(losRequest.elevation);
      data.fMinimumRange = losRequest.minimumRange;
      data.fMaximumRange = losRequest.maximumRange;
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightVectorRequestGeodeticBasic data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.azimuth = Degrees180(losRequest.azimuth);
      data.elevation = Degrees90(losRequest.elevation);
      data.fMinimumRange = losRequest.minimumRange;
      data.fMaximumRange = losRequest.maximumRange;
      data.sourceGeodeticCoordinates.latitude = Latitude(losRequest.sourceLatitudeXOffset);
      data.sourceGeodeticCoordinates.longitude = Longitude(losRequest.sourceLongitudeYOffset);
      data.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.sourceAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
  }
  else
  {
    if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightVectorRequestEntityExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.entityId);
      data.eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightVectorRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));
      data.azimuth = Degrees180(losRequest.azimuth);
      data.elevation = Degrees90(losRequest.elevation);
      data.fMinimumRange = losRequest.minimumRange;
      data.fMaximumRange = losRequest.maximumRange;
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightVectorRequestGeodeticExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.azimuth = Degrees180(losRequest.azimuth);
      data.elevation = Degrees90(losRequest.elevation);
      data.fMinimumRange = losRequest.minimumRange;
      data.fMaximumRange = losRequest.maximumRange;
      data.sourceGeodeticCoordinates.latitude = Latitude(losRequest.sourceLatitudeXOffset);
      data.sourceGeodeticCoordinates.longitude = Longitude(losRequest.sourceLongitudeYOffset);
      data.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.sourceAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      data.eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::LineOfSightVectorRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));
      m_pTerrainHandler->Handle(data);
    }
  }
}

void CCigiPacketHandlerV4::ParseMaritimeSurfaceConditionsControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::MaritimeSurfaceConditionsCtrl maritimeSurfaceConditionsControlPacketV4;
  memcpy(&maritimeSurfaceConditionsControlPacketV4, pBuffer, sizeof(CIGI::V40::MaritimeSurfaceConditionsCtrl));

  if (m_bByteSwap)
  {
    maritimeSurfaceConditionsControlPacketV4.doByteSwapping();
  }

  SCigiMaritimeSurfaceCondition maritimeSurface;
  maritimeSurface.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(maritimeSurfaceConditionsControlPacketV4.seaSurfaceHeight);
  maritimeSurface.fSurfaceWaterTemperature = TemperatureCelsius(maritimeSurfaceConditionsControlPacketV4.surfaceWaterTemperature);
  maritimeSurface.surfaceClarity = Percentage(maritimeSurfaceConditionsControlPacketV4.surfaceClarity);
  maritimeSurface.bWhitecapEnabled = maritimeSurfaceConditionsControlPacketV4.whitecapEnable;
  maritimeSurface.bActive = maritimeSurfaceConditionsControlPacketV4.surfaceConditionsEnable;

  ECigiScope eScope = ConvertScope(static_cast<CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope>(maritimeSurfaceConditionsControlPacketV4.scope));

  switch (eScope)
  {
  case ECigiScope::ENTITY:
    m_pMaritimeSurfaceConditionPacketHandler->HandleEntityMartiimeSurfaceCondition(maritimeSurface, EntityID(maritimeSurfaceConditionsControlPacketV4.entityRegionId));
    break;
  case ECigiScope::REGIONAL:
    m_pMaritimeSurfaceConditionPacketHandler->HandleRegionMaritimeSurfaceCondition(maritimeSurface, RegionID(maritimeSurfaceConditionsControlPacketV4.entityRegionId));
    break;
  case ECigiScope::GLOBAL:
    m_pMaritimeSurfaceConditionPacketHandler->HandleGlobalMaritimeSurfaceCondition(maritimeSurface);
    break;
  default:
    break;
  }
}

void CCigiPacketHandlerV4::ParseMotionTrackerControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::MotionTrackerCtrl motionTrackerV4;
  memcpy(&motionTrackerV4, pBuffer, sizeof(CIGI::V40::MotionTrackerCtrl));

  if (m_bByteSwap)
  {
    motionTrackerV4.doByteSwapping();
  }

  if (motionTrackerV4.viewViewGroupSelect == CIGI::V40::MotionTrackerCtrl::eViewGroupSelect_View)
  {
    SMotionTrackerViewControl motionTrackerControl;
    motionTrackerControl.bBoresightEnable = motionTrackerV4.boresightEnable;
    motionTrackerControl.bEnable = motionTrackerV4.trackerEnable;
    motionTrackerControl.bPitchEnable = motionTrackerV4.pitchEnable;
    motionTrackerControl.bRollEnable = motionTrackerV4.rollEnable;
    motionTrackerControl.bXEnable = motionTrackerV4.xEnable;
    motionTrackerControl.bYawEnable = motionTrackerV4.yawEnable;
    motionTrackerControl.bYEnable = motionTrackerV4.yEnable;
    motionTrackerControl.bZEnable = motionTrackerV4.zEnable;
    motionTrackerControl.motionTrackerID = MotionTrackerID(motionTrackerV4.trackerId);
    motionTrackerControl.viewID = ViewID(static_cast<uint8_t>(motionTrackerV4.viewViewGroupId));
    m_pMotionTrackerControlHandler->Handle(motionTrackerControl);
  }
  else if (motionTrackerV4.viewViewGroupSelect == CIGI::V40::MotionTrackerCtrl::eViewGroupSelect_ViewGroup)
  {
    SMotionTrackerViewGroupControl motionTrackerControl;
    motionTrackerControl.bBoresightEnable = motionTrackerV4.boresightEnable;
    motionTrackerControl.bEnable = motionTrackerV4.trackerEnable;
    motionTrackerControl.bPitchEnable = motionTrackerV4.pitchEnable;
    motionTrackerControl.bRollEnable = motionTrackerV4.rollEnable;
    motionTrackerControl.bXEnable = motionTrackerV4.xEnable;
    motionTrackerControl.bYawEnable = motionTrackerV4.yawEnable;
    motionTrackerControl.bYEnable = motionTrackerV4.yEnable;
    motionTrackerControl.bZEnable = motionTrackerV4.zEnable;
    motionTrackerControl.motionTrackerID = MotionTrackerID(motionTrackerV4.trackerId);
    motionTrackerControl.viewGroupID = ViewGroupID(static_cast<uint8_t>(motionTrackerV4.viewViewGroupId));
    m_pMotionTrackerControlHandler->Handle(motionTrackerControl);
  }
}

void CCigiPacketHandlerV4::ParsePositionRequestPacket(uint8_t* pBuffer)
{
  CIGI::V40::PositionRequest positionRequestV4;
  memcpy(&positionRequestV4, pBuffer, sizeof(CIGI::V40::PositionRequest));

  if (m_bByteSwap)
  {
    positionRequestV4.doByteSwapping();
  }

  SPositionRequest request;
  request.articulatedPartID = ArticulatedPartID(positionRequestV4.articulatedPartId);
  request.eCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V40::PositionRequest::CoordinateSystem>(positionRequestV4.coordinateSystem));
  request.eObjectClass = ConvertObjectClass(static_cast<CIGI::V40::PositionRequest::ObjectClass>(positionRequestV4.objectClass));
  request.bContinuous = positionRequestV4.updateMode == CIGI::V40::PositionRequest::eUpdateMode_Continuous;
  request.nObjectID = positionRequestV4.objectId;

  m_pEntityControlHandler->HandleCigiPositionRequest(request);
}

ETrackMode ConvertTrackMode(CIGI::V40::SensorCtrl::TrackMode eTrackMode)
{
  if (eTrackMode == CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Off)
  {
    return ETrackMode::OFF;
  }
  else if (eTrackMode == CIGI::V40::SensorCtrl::TrackMode::eTrackMode_ForceCorrelate)
  {
    return ETrackMode::FORCE_CORRELATE;
  }
  else if (eTrackMode == CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Scene)
  {
    return ETrackMode::SCENE;
  }
  else if (eTrackMode == CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Target)
  {
    return ETrackMode::TARGET;
  }
  else if (eTrackMode == CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Ship)
  {
    return ETrackMode::SHIP;
  }
  else
  {
    return ETrackMode::UNKNOWN;
  }
}

void CCigiPacketHandlerV4::ParseSensorControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::SensorCtrl sensorCtrlV4;
  memcpy(&sensorCtrlV4, pBuffer, sizeof(CIGI::V40::SensorCtrl));

  if (m_bByteSwap)
  {
    sensorCtrlV4.doByteSwapping();
  }

  SCigiSensorControl sensorControl;
  sensorControl.sensorID = SensorID(sensorCtrlV4.sensorId);
  sensorControl.bAutomaticGain = sensorCtrlV4.automaticGain;
  sensorControl.eTrackMode = ConvertTrackMode(static_cast<CIGI::V40::SensorCtrl::TrackMode>(sensorCtrlV4.trackMode));

  if (sensorCtrlV4.polarity == CIGI::V40::SensorCtrl::Polarity::ePolarity_BlackHot)
  {
    sensorControl.ePolarity = EPolarity::BLACK_HOT;
  }
  else if (sensorCtrlV4.polarity == CIGI::V40::SensorCtrl::Polarity::ePolarity_WhiteHot)
  {
    sensorControl.ePolarity = EPolarity::WHITE_HOT;
  }
  else
  {
    sensorControl.ePolarity = EPolarity::UNKNOWN;
  }

  sensorControl.bSensorOn = sensorCtrlV4.sensorOnOff == 1;
  sensorControl.bLineByLineDropoutEnabled = sensorCtrlV4.lineByLineDropoutEnable;
  sensorControl.bAutomaticGain = sensorCtrlV4.automaticGain;

  if (sensorCtrlV4.trackWhiteBlack == CIGI::V40::SensorCtrl::Track::eTrack_Black)
  {
    sensorControl.eSensorTrack = ESensorTrack::BLACK;
  }
  else if (sensorCtrlV4.trackWhiteBlack == CIGI::V40::SensorCtrl::Track::eTrack_White)
  {
    sensorControl.eSensorTrack = ESensorTrack::WHITE;
  }
  else
  {
    sensorControl.eSensorTrack = ESensorTrack::UNKNOWN;
  }

  if (sensorCtrlV4.responseType == CIGI::V40::SensorCtrl::ResponseType::eResponseType_Extended)
  {
    sensorControl.bExtendedResponse = true;
  }
  else
  {
    sensorControl.bExtendedResponse = false;
  }

  sensorControl.viewID = ViewID(sensorCtrlV4.viewId);
  sensorControl.gain = SensorGain(sensorCtrlV4.gain);
  sensorControl.level = SensorLevel(sensorCtrlV4.level);
  sensorControl.fACCoupling = sensorCtrlV4.acCoupling;
  sensorControl.noise = SensorNoise(sensorCtrlV4.noise);

  if (sensorControl.gain.CheckValid())
  {
    m_pSensorControlHandler->Handle(sensorControl);
  }
}

void CCigiPacketHandlerV4::ParseShortArticulatedPartControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::ShortArticulatedPartCtrl shortArticulatedPartControlV4;
  memcpy(&shortArticulatedPartControlV4, pBuffer, sizeof(CIGI::V40::ShortArticulatedPartCtrl));

  if (m_bByteSwap)
  {
    shortArticulatedPartControlV4.doByteSwapping();
  }

  // verify entity exists
  auto entityID = EntityID(shortArticulatedPartControlV4.entityId);
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entityID))
  {
    return;
  }

  SCigiShortArticulatedPart shortArticulatedPart;
  shortArticulatedPart.entityID = entityID;
  shortArticulatedPart.articulatedPartID1 = ArticulatedPartID(shortArticulatedPartControlV4.articulatedPartId1);
  shortArticulatedPart.articulatedPartID2 = ArticulatedPartID(shortArticulatedPartControlV4.articulatedPartId2);
  shortArticulatedPart.eDOF1 = ConvertDegreeOfFreedom(static_cast<CIGI::V40::ShortArticulatedPartCtrl::DOFSelect>(shortArticulatedPartControlV4.dofSelect1));
  shortArticulatedPart.eDOF2 = ConvertDegreeOfFreedom(static_cast<CIGI::V40::ShortArticulatedPartCtrl::DOFSelect>(shortArticulatedPartControlV4.dofSelect2));
  shortArticulatedPart.fDOF1 = shortArticulatedPartControlV4.dof1;
  shortArticulatedPart.fDOF2 = shortArticulatedPartControlV4.dof2;
  shortArticulatedPart.bArticulatedPart1Enabled = shortArticulatedPartControlV4.articulatedPartEnable1;
  shortArticulatedPart.bArticulatedPart2Enabled = shortArticulatedPartControlV4.articulatedPartEnable2;
  m_pArticulatedPartHandler->Handle(shortArticulatedPart);
}

void CCigiPacketHandlerV4::ParseShortComponentControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::ShortComponentCtrl shortComponentControlV4;
  memcpy(&shortComponentControlV4, pBuffer, sizeof(CIGI::V40::ShortComponentCtrl));

  if (m_bByteSwap)
  {
    shortComponentControlV4.doByteSwapping();
  }

  SCigiComponentControl componentControl;
  componentControl.key.componentID = CigiComponentID(shortComponentControlV4.componentId);
  componentControl.key.componentClassID = CigiComponentClassID(static_cast<uint8_t>(shortComponentControlV4.componentClass));
  componentControl.key.nInstanceID = shortComponentControlV4.instanceId;
  componentControl.state.nComponentState = shortComponentControlV4.componentState;
  componentControl.state.componentData[0] = shortComponentControlV4.componentData[0];
  componentControl.state.componentData[1] = shortComponentControlV4.componentData[1];

  CShortComponentDataParser<CIGI::V40::ShortComponentCtrl> componentDataParser(shortComponentControlV4);
  m_pComponentControlHandler->Handle(componentControl, &componentDataParser);
}

void ParseAttributeValue(SSymbolControl& symbolControl, CIGI::V40::ShortSymbolCtrl::AttributeSelect eAttributeSelect, CIGI::ui32 attributeValue)
{
  switch (eAttributeSelect)
  {
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_None:
  {
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_SurfaceID:
  {
    symbolControl.surfaceID = SymbolSurfaceID(static_cast<uint16_t>(attributeValue));
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ParentSymbolID:
  {
    symbolControl.parentSymbolID = SymbolID(static_cast<uint16_t>(attributeValue));
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Layer:
  {
    symbolControl.nLayerID = static_cast<uint8_t>(attributeValue);
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashDutyCyclePercentage:
  {
    symbolControl.flashDutyCyclePercentage = ConvertToPercentage(CigiIntPercentage(static_cast<uint8_t>(attributeValue)));
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashPeriod:
  {
    memcpy(&symbolControl.fFlashPeriod, &attributeValue, sizeof(uint32_t));
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionU:
  {
    memcpy(&symbolControl.fPositionU, &attributeValue, sizeof(uint32_t));
    symbolControl.bPositionSet = true;
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionV:
  {
    memcpy(&symbolControl.fPositionV, &attributeValue, sizeof(uint32_t));
    symbolControl.bPositionSet = true;
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Rotation:
  {
    float f = 0;
    memcpy(&f, &attributeValue, sizeof(uint32_t));
    symbolControl.fRotation = Degrees(f);
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Color:
  {
    symbolControl.color.r = (attributeValue & 0xF000) >> 12;
    symbolControl.color.b = (attributeValue & 0x0F00) >> 8;
    symbolControl.color.g = (attributeValue & 0x00F0) >> 4;
    symbolControl.color.a = attributeValue & 0x000F;

    symbolControl.bSetColor = true;
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleU:
  {
    memcpy(&symbolControl.fScaleU, &attributeValue, sizeof(uint32_t));
    symbolControl.bScaleSet = true;
    break;
  }
  case CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleV:
  {
    memcpy(&symbolControl.fScaleV, &attributeValue, sizeof(uint32_t));
    symbolControl.bScaleSet = true;
    break;
  }
  }
}

void CCigiPacketHandlerV4::ParseShortSymbolControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::ShortSymbolCtrl symbolControlV4;
  memcpy(&symbolControlV4, pBuffer, sizeof(CIGI::V40::ShortSymbolCtrl));

  if (m_bByteSwap)
  {
    symbolControlV4.doByteSwapping();
  }

  SSymbolControl symbolControl;
  symbolControl.symbolID = SymbolID(symbolControlV4.symbolId);
  symbolControl.eAttachState = ConvertAttachState(static_cast<CIGI::V40::SymbolCtrl::AttachState>(symbolControlV4.attachState));
  symbolControl.bInheritColor = symbolControlV4.inheritColor == CIGI::V40::ShortSymbolCtrl::InheritColor::eInheritColor_Inherited;
  symbolControl.eFlashControl = ConvertToFlashControl(static_cast<CIGI::V40::SymbolCtrl::FlashControl>(symbolControlV4.flashControl));
  symbolControl.eSymbolState = ConvertSymbolState(static_cast<CIGI::V40::SymbolCtrl::SymbolState>(symbolControlV4.symbolState));
  symbolControl.bScaleSet = false;
  symbolControl.bPositionSet = false;
  symbolControl.bSetColor = false;

  ParseAttributeValue(symbolControl, static_cast<CIGI::V40::ShortSymbolCtrl::AttributeSelect>(symbolControlV4.attributeSelect1), symbolControlV4.attributeValue1);
  ParseAttributeValue(symbolControl, static_cast<CIGI::V40::ShortSymbolCtrl::AttributeSelect>(symbolControlV4.attributeSelect2), symbolControlV4.attributeValue2);
  m_pSymbolHandler->Handle(symbolControl);
}

void CCigiPacketHandlerV4::ParseSymbolCircleDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolCircleDefinition symbolCircleDefV4;
  memcpy(&symbolCircleDefV4, pBuffer, sizeof(CIGI::V40::SymbolCircleDefinition));

  if (m_bByteSwap)
  {
    symbolCircleDefV4.doByteSwapping();
  }

  SSymbolCircle symbolCircle;
  symbolCircle.symbolID = SymbolID(symbolCircleDefV4.symbolId);
  symbolCircle.eDrawingStyle = ConvertDrawingStyle(static_cast<CIGI::V40::SymbolCircleDefinition::DrawingStyle>(symbolCircleDefV4.drawingStyle));
  symbolCircle.fLineWidth = symbolCircleDefV4.lineWidth;
  symbolCircle.stipplePattern = symbolCircleDefV4.stipplePattern;
  symbolCircle.fStipplePatternLength = symbolCircleDefV4.stipplePatternLength;

  CIGI::VariableRecordHelper<CIGI::V40::SymbolCircleDefinition> helper(symbolCircleDefV4);

  int numRecords = helper.numRecords(symbolCircleDefV4);
  for (int n = 0; n < numRecords; ++n)
  {
    CIGI::V40::SymbolCircleDefinition::Circle circle = symbolCircleDefV4.records[n];
    SCircleProperties circleProperties;
    circleProperties.centerUV.U = circle.centerU;
    circleProperties.centerUV.V = circle.centerV;
    circleProperties.fRadius = circle.radius;
    circleProperties.fInnerRadius = circle.innerRadius;
    circleProperties.startAngle = Degrees(circle.startAngle);
    circleProperties.endAngle = Degrees(circle.endAngle);
    symbolCircle.circles.push_back(circleProperties);
  }
  m_pSymbolHandler->Handle(symbolCircle);
}

void CCigiPacketHandlerV4::ParseSymbolClonePacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolClone symbolCloneV4;
  memcpy(&symbolCloneV4, pBuffer, sizeof(CIGI::V40::SymbolClone));

  if (m_bByteSwap)
  {
    symbolCloneV4.doByteSwapping();
  }

  SSymbolClone symbolClone;
  symbolClone.eSymbolSourceType = ConvertSymbolSourceType(static_cast<CIGI::V40::SymbolClone::SourceType>(symbolCloneV4.sourceType));
  symbolClone.sourceID = SymbolID(symbolCloneV4.sourceId);
  symbolClone.symbolID = SymbolID(symbolCloneV4.symbolId);
  m_pSymbolHandler->Handle(symbolClone);
}

void CCigiPacketHandlerV4::ParseSymbolControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolCtrl symbolControlV4;
  memcpy(&symbolControlV4, pBuffer, sizeof(CIGI::V40::SymbolCtrl));

  if (m_bByteSwap)
  {
    symbolControlV4.doByteSwapping();
  }

  SSymbolControl symbolControl;
  symbolControl.eAttachState = ConvertAttachState(static_cast<CIGI::V40::SymbolCtrl::AttachState>(symbolControlV4.attachState));
  symbolControl.bInheritColor = symbolControlV4.inheritColor == CIGI::V40::SymbolCtrl::InheritColor::eInheritColor_Inherited;
  symbolControl.eFlashControl = ConvertToFlashControl(static_cast<CIGI::V40::SymbolCtrl::FlashControl>(symbolControlV4.flashControl));
  symbolControl.eSymbolState = ConvertSymbolState(static_cast<CIGI::V40::SymbolCtrl::SymbolState>(symbolControlV4.symbolState));
  symbolControl.flashDutyCyclePercentage = Percentage(symbolControlV4.flashDutyCyclePercentage / 100.0f);
  symbolControl.fFlashPeriod = symbolControlV4.flashPeriod;
  symbolControl.fRotation = Degrees(symbolControlV4.rotation);
  symbolControl.fScaleU = symbolControlV4.scaleU;
  symbolControl.fScaleV = symbolControlV4.scaleV;
  symbolControl.nLayerID = symbolControlV4.layer;
  symbolControl.parentSymbolID = SymbolID(symbolControlV4.parentSymbolId);
  symbolControl.fPositionU = symbolControlV4.positionU;
  symbolControl.fPositionV = symbolControlV4.positionV;
  symbolControl.surfaceID = SymbolSurfaceID(symbolControlV4.surfaceId);
  symbolControl.symbolID = SymbolID(symbolControlV4.symbolId);
  symbolControl.color.r = symbolControlV4.red;
  symbolControl.color.g = symbolControlV4.green;
  symbolControl.color.b = symbolControlV4.blue;
  symbolControl.color.a = symbolControlV4.alpha;
  m_pSymbolHandler->Handle(symbolControl);
}

void CCigiPacketHandlerV4::ParseSymbolPolygonDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolPolygonDefinition symbolPolygonDefV4;
  memcpy(&symbolPolygonDefV4, pBuffer, sizeof(CIGI::V40::SymbolPolygonDefinition));

  if (m_bByteSwap)
  {
    symbolPolygonDefV4.doByteSwapping();
  }

  SSymbolPolygon symbolPolygon;
  symbolPolygon.symbolID = SymbolID(symbolPolygonDefV4.symbolId);
  symbolPolygon.ePrimitiveType = ConvertPrimitiveGrp(static_cast<CIGI::V40::SymbolPolygonDefinition::PrimitiveType>(symbolPolygonDefV4.primitiveType));
  symbolPolygon.fLineWidth = symbolPolygonDefV4.lineWidth;
  symbolPolygon.fStipplePatternLength = symbolPolygonDefV4.stipplePatternLength;
  symbolPolygon.nStipplePattern = symbolPolygonDefV4.stipplePattern;

  CIGI::VariableRecordHelper<CIGI::V40::SymbolPolygonDefinition> helper(symbolPolygonDefV4);
  int nNumVertices = helper.numRecords(symbolPolygonDefV4);
  for (int n = 0; n < nNumVertices; ++n)
  {
    Vec2f v;
    v[0] = symbolPolygonDefV4.records[n].vertexU;
    v[1] = symbolPolygonDefV4.records[n].vertexV;

    symbolPolygon.vertices.push_back(v);
  }
  m_pSymbolHandler->Handle(symbolPolygon);
}

void CCigiPacketHandlerV4::ParseSymbolSurfaceDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolSurfaceDefinition symbolSurfaceDefV4;
  memcpy(&symbolSurfaceDefV4, pBuffer, sizeof(CIGI::V40::SymbolSurfaceDefinition));

  if (m_bByteSwap)
  {
    symbolSurfaceDefV4.doByteSwapping();
  }

  if (symbolSurfaceDefV4.attachType == CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_Entity)
  {
    if (symbolSurfaceDefV4.billboard == CIGI::V40::SymbolSurfaceDefinition::Billboard::eBillboard_NonBillboard)
    {
      SEntitySymbolSurfaceDefinition entitySymbolSurfaceDefinition;
      entitySymbolSurfaceDefinition.surfaceID = SymbolSurfaceID(symbolSurfaceDefV4.surfaceId);
      entitySymbolSurfaceDefinition.eSurfaceState = ConvertToSurfaceState(static_cast<CIGI::V40::SymbolSurfaceDefinition::SurfaceState>(symbolSurfaceDefV4.surfaceState));
      entitySymbolSurfaceDefinition.uvMin[0] = symbolSurfaceDefV4.minU;
      entitySymbolSurfaceDefinition.uvMin[1] = symbolSurfaceDefV4.minV;
      entitySymbolSurfaceDefinition.uvMax[0] = symbolSurfaceDefV4.maxU;
      entitySymbolSurfaceDefinition.uvMax[1] = symbolSurfaceDefV4.maxV;
      entitySymbolSurfaceDefinition.entityID = EntityID(symbolSurfaceDefV4.entityIdViewId);
      entitySymbolSurfaceDefinition.offsetToSurface[0] = symbolSurfaceDefV4.xOffsetLeft;
      entitySymbolSurfaceDefinition.offsetToSurface[1] = symbolSurfaceDefV4.yOffsetRight;
      entitySymbolSurfaceDefinition.offsetToSurface[2] = symbolSurfaceDefV4.zOffsetTop;
      entitySymbolSurfaceDefinition.yaw = Degrees(symbolSurfaceDefV4.yawBottom);
      entitySymbolSurfaceDefinition.pitch = Degrees90(symbolSurfaceDefV4.pitch);
      entitySymbolSurfaceDefinition.roll = Degrees180(symbolSurfaceDefV4.roll);
      entitySymbolSurfaceDefinition.width = symbolSurfaceDefV4.width;
      entitySymbolSurfaceDefinition.height = symbolSurfaceDefV4.height;
      m_pSymbolHandler->Handle(entitySymbolSurfaceDefinition);
    }
    else if (symbolSurfaceDefV4.billboard == CIGI::V40::SymbolSurfaceDefinition::Billboard::eBillboard_Billboard)
    {
      SEntityBillboardSymbolSurfaceDefinition entityBillboardSurfaceDefinition;
      entityBillboardSurfaceDefinition.surfaceID = SymbolSurfaceID(symbolSurfaceDefV4.surfaceId);
      entityBillboardSurfaceDefinition.eSurfaceState = ConvertToSurfaceState(static_cast<CIGI::V40::SymbolSurfaceDefinition::SurfaceState>(symbolSurfaceDefV4.surfaceState));
      entityBillboardSurfaceDefinition.uvMin[0] = symbolSurfaceDefV4.minU;
      entityBillboardSurfaceDefinition.uvMin[1] = symbolSurfaceDefV4.minV;
      entityBillboardSurfaceDefinition.uvMax[0] = symbolSurfaceDefV4.maxU;
      entityBillboardSurfaceDefinition.uvMax[1] = symbolSurfaceDefV4.maxV;
      entityBillboardSurfaceDefinition.entityID = EntityID(symbolSurfaceDefV4.entityIdViewId);
      entityBillboardSurfaceDefinition.bPerspectiveGrowthEnabled = symbolSurfaceDefV4.perspectiveGrowthEnable;
      entityBillboardSurfaceDefinition.offsetToEntity[0] = symbolSurfaceDefV4.xOffsetLeft;
      entityBillboardSurfaceDefinition.offsetToEntity[1] = symbolSurfaceDefV4.yOffsetRight;
      entityBillboardSurfaceDefinition.offsetToEntity[2] = symbolSurfaceDefV4.zOffsetTop;
      entityBillboardSurfaceDefinition.width = symbolSurfaceDefV4.width;
      entityBillboardSurfaceDefinition.height = symbolSurfaceDefV4.height;
      m_pSymbolHandler->Handle(entityBillboardSurfaceDefinition);
    }
  }
  else if (symbolSurfaceDefV4.attachType == CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_View)
  {
    SViewSymbolSurfaceDefinition viewSymbolSurfaceDefinition;
    viewSymbolSurfaceDefinition.surfaceID = SymbolSurfaceID(symbolSurfaceDefV4.surfaceId);
    viewSymbolSurfaceDefinition.eSurfaceState = ConvertToSurfaceState(static_cast<CIGI::V40::SymbolSurfaceDefinition::SurfaceState>(symbolSurfaceDefV4.surfaceState));
    viewSymbolSurfaceDefinition.uvMin[0] = symbolSurfaceDefV4.minU;
    viewSymbolSurfaceDefinition.uvMin[1] = symbolSurfaceDefV4.minV;
    viewSymbolSurfaceDefinition.uvMax[0] = symbolSurfaceDefV4.maxU;
    viewSymbolSurfaceDefinition.uvMax[1] = symbolSurfaceDefV4.maxV;
    viewSymbolSurfaceDefinition.viewID = ViewID(symbolSurfaceDefV4.entityIdViewId);
    viewSymbolSurfaceDefinition.fLeft = symbolSurfaceDefV4.xOffsetLeft;
    viewSymbolSurfaceDefinition.fRight = symbolSurfaceDefV4.yOffsetRight;
    viewSymbolSurfaceDefinition.fTop = symbolSurfaceDefV4.zOffsetTop;
    viewSymbolSurfaceDefinition.fBottom = symbolSurfaceDefV4.yawBottom;
    m_pSymbolHandler->Handle(viewSymbolSurfaceDefinition);
  }
}

void CCigiPacketHandlerV4::ParseSymbolTextDefinitionPacket(uint8_t* pBuffer)
{
  SCigiPacketHeaderV4 packetHeader;
  memcpy(&packetHeader, pBuffer, sizeof(SCigiPacketHeaderV4));

  uint16_t nPacketSize = packetHeader.nPacketSize;
  if (m_bByteSwap)
  {
    CEndian::Swap(nPacketSize);
  }

  if (nPacketSize < CIGI::V40::SymbolTextDefinition::kBasePacketSize)
  {
    stringstream ss;
    ss << "Skipping invalid Symbol Text Definition packet: size " << nPacketSize << " is smaller than base size " << CIGI::V40::SymbolTextDefinition::kBasePacketSize << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  size_t nTextSize = static_cast<size_t>(nPacketSize - CIGI::V40::SymbolTextDefinition::kBasePacketSize);
  if (nTextSize > CIGI::V40::SymbolTextDefinition::kMaxDataLength)
  {
    stringstream ss;
    ss << "Skipping invalid Symbol Text Definition packet payload length " << nTextSize << " (max " << CIGI::V40::SymbolTextDefinition::kMaxDataLength << ")" << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  CIGI::V40::SymbolTextDefinition symbolTextDefV4 = {};
  memcpy(&symbolTextDefV4, pBuffer, CIGI::V40::SymbolTextDefinition::kBasePacketSize);

  if (m_bByteSwap)
  {
    symbolTextDefV4.doByteSwapping();
    CEndian::Swap(symbolTextDefV4.fontSize);
  }

  const char* pTextBegin = reinterpret_cast<const char*>(pBuffer + CIGI::V40::SymbolTextDefinition::kBasePacketSize);
  const char* pTextEnd = pTextBegin + nTextSize;
  const char* pTextTerminator = std::find(pTextBegin, pTextEnd, '\0');

  SSymbolTextDefinition symbolTextDefinition;
  symbolTextDefinition.symbolID = SymbolID(symbolTextDefV4.symbolId);
  symbolTextDefinition.eTextAlignment = ConvertTextAlignment(static_cast<CIGI::V40::SymbolTextDefinition::Alignment>(symbolTextDefV4.alignment));
  symbolTextDefinition.eTextOrientation = ConvertTextOrientation(static_cast<CIGI::V40::SymbolTextDefinition::Orientation>(symbolTextDefV4.orientation));
  symbolTextDefinition.fFontSize = symbolTextDefV4.fontSize;
  symbolTextDefinition.fontID = FontID(symbolTextDefV4.fontId);
  symbolTextDefinition.sText.assign(pTextBegin, pTextTerminator);
  m_pSymbolHandler->Handle(symbolTextDefinition);
}

void CCigiPacketHandlerV4::ParseSymbolTexturedCircleDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolTexturedCircleDefinition symbolTexturedCircleDefV4;
  memcpy(&symbolTexturedCircleDefV4, pBuffer, sizeof(CIGI::V40::SymbolTexturedCircleDefinition));

  if (m_bByteSwap)
  {
    symbolTexturedCircleDefV4.doByteSwapping();
  }

  SSymbolTexturedCircle symbolTexturedCircle;
  symbolTexturedCircle.symbolID = SymbolID(symbolTexturedCircleDefV4.symbolId);
  symbolTexturedCircle.textureID = TextureID(symbolTexturedCircleDefV4.textureId);
  symbolTexturedCircle.eTextureFilter = ConvertTextureFilterMode(static_cast<CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode>(symbolTexturedCircleDefV4.textureFilterMode));
  symbolTexturedCircle.eTextureWrap = ConvertTextureWrapMode(static_cast<CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp>(symbolTexturedCircleDefV4.textureRepeatOrClamp));

  CIGI::VariableRecordHelper<CIGI::V40::SymbolTexturedCircleDefinition> helper(symbolTexturedCircleDefV4);
  int numCircles = helper.numRecords(symbolTexturedCircleDefV4);

  for (int n = 0; n < numCircles; ++n)
  {
    const CIGI::V40::SymbolTexturedCircleDefinition::TexturedCircle& texturedCircle = symbolTexturedCircleDefV4.records[n];
    STexturedCircleProperties circleProperties;
    circleProperties.centerUV[0] = texturedCircle.centerU;
    circleProperties.centerUV[1] = texturedCircle.centerV;
    circleProperties.fRadius = texturedCircle.radius;
    circleProperties.fInnerRadius = texturedCircle.innerRadius;
    circleProperties.startAngle = Degrees(texturedCircle.startAngle);
    circleProperties.endAngle = Degrees(texturedCircle.endAngle);
    circleProperties.centerTextureST[0] = texturedCircle.textureCoordinateSAtCenterPoint;
    circleProperties.centerTextureST[1] = texturedCircle.textureCoordinateTAtCenterPoint;
    circleProperties.fTextureMapRadius = texturedCircle.textureMappingRadius;
    circleProperties.fTextureMapRotation = texturedCircle.textureMappingRotation;
    symbolTexturedCircle.circles.push_back(circleProperties);
  }
  m_pSymbolHandler->Handle(symbolTexturedCircle);
}

void CCigiPacketHandlerV4::ParseSymbolTexturedPolygonDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::SymbolTexturedPolygonDefinition symbolTexturedPolygonDefV4;
  memcpy(&symbolTexturedPolygonDefV4, pBuffer, sizeof(CIGI::V40::SymbolTexturedPolygonDefinition));

  if (m_bByteSwap)
  {
    symbolTexturedPolygonDefV4.doByteSwapping();
  }

  SSymbolTexturedPolygon symbolPolygon;
  symbolPolygon.symbolID = SymbolID(symbolTexturedPolygonDefV4.symbolId);
  symbolPolygon.ePrimitiveType = ConvertPrimitiveGrp(static_cast<CIGI::V40::SymbolPolygonDefinition::PrimitiveType>(symbolTexturedPolygonDefV4.primitiveType));
  symbolPolygon.eTextureFilterMode = ConvertTextureFilterMode(static_cast<CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode>(symbolTexturedPolygonDefV4.textureFilterMode));
  symbolPolygon.eTextureWrapMode = ConvertTextureWrapMode(static_cast<CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp>(symbolTexturedPolygonDefV4.textureRepeatOrClamp));
  symbolPolygon.textureID = TextureID(symbolTexturedPolygonDefV4.textureId);

  int numVertices = CIGI::VariableRecordHelper<CIGI::V40::SymbolTexturedPolygonDefinition>::numRecords(symbolTexturedPolygonDefV4);

  for (int n = 0; n < numVertices; ++n)
  {
    CIGI::V40::SymbolTexturedPolygonDefinition::TexturedVertex record = symbolTexturedPolygonDefV4.records[n];

    SSymbolTexturedPolygonVertex polygonVertex;
    polygonVertex.uv[0] = record.vertexU;
    polygonVertex.uv[1] = record.vertexV;
    polygonVertex.textureCoordinateST[0] = record.textureCoordinateS;
    polygonVertex.textureCoordinateST[1] = record.textureCoordinateT;

    symbolPolygon.vertices.push_back(polygonVertex);
  }
  m_pSymbolHandler->Handle(symbolPolygon);
}

void CCigiPacketHandlerV4::ParseTerrestrialSurfaceConditionsControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceControlV4;
  memcpy(&terrestrialSurfaceControlV4, pBuffer, sizeof(CIGI::V40::TerrestrialSurfaceConditionsCtrl));

  if (m_bByteSwap)
  {
    terrestrialSurfaceControlV4.doByteSwapping();
  }

  SCigiTerrestrialSurfaceCondition condition;
  condition.surfaceConditionID = SurfaceConditionID(terrestrialSurfaceControlV4.surfaceConditionId);
  condition.coverage = Percentage(terrestrialSurfaceControlV4.coverage);
  CigiTerrestrialSurfaceSeverity severity = CigiTerrestrialSurfaceSeverity(terrestrialSurfaceControlV4.severity);
  condition.severity = ConvertToPercentage(severity);
  condition.bEnabled = terrestrialSurfaceControlV4.surfaceConditionEnable;

  ECigiScope eScope = ConvertScope(static_cast<CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope>(terrestrialSurfaceControlV4.scope));
  switch (eScope)
  {
  case ECigiScope::ENTITY:
  {
    EntityID entityID = EntityID(terrestrialSurfaceControlV4.entityRegionId);
    m_pEnvironmentalRegionHandler->HandleEntityTerrestrialSurfaceCondition(entityID, condition);
    break;
  }
  case ECigiScope::REGIONAL:
  {
    RegionID regionID = RegionID(terrestrialSurfaceControlV4.entityRegionId);
    m_pEnvironmentalRegionHandler->HandleRegionTerrestrialSurfaceCondition(regionID, condition);
    break;
  }
  case ECigiScope::GLOBAL:
  {
    m_pEnvironmentalRegionHandler->HandleGlobalTerrestrialSurfaceCondition(condition);
    break;
  }
  default:
    break;
  }
}

void CCigiPacketHandlerV4::ParseVelocityControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::VelocityCtrl velocityControlV4;
  memcpy(&velocityControlV4, pBuffer, sizeof(CIGI::V40::VelocityCtrl));

  if (m_bByteSwap)
  {
    velocityControlV4.doByteSwapping();
  }

  CigiBodyCoordinates linearVelocity;
  TCigiBodyEulerRotation angularVelocity;

  linearVelocity[0] = velocityControlV4.xLinearVelocity;
  linearVelocity[1] = velocityControlV4.yLinearVelocity;
  linearVelocity[2] = velocityControlV4.zLinearVelocity;

  angularVelocity.yaw = Degrees(velocityControlV4.yawAngularVelocity);
  angularVelocity.pitch = Degrees(velocityControlV4.pitchAngularVelocity);
  angularVelocity.roll = Degrees(velocityControlV4.rollAngularVelocity);

  if (velocityControlV4.applyToArticulatedPart)
  {
    SCigiArticulatedPartVelocityControl articulatedPartVelocityControl;
    articulatedPartVelocityControl.entityID = EntityID(velocityControlV4.entityId);
    articulatedPartVelocityControl.articulatedPartID = ArticulatedPartID(velocityControlV4.articulatedPartId);
    articulatedPartVelocityControl.angularVelocity = angularVelocity;
    articulatedPartVelocityControl.linearVelocity = linearVelocity;

    m_pVelocityControlHandler->HandleArticulatedPartVelocityControl(articulatedPartVelocityControl);
  }
  else
  {
    SCigiEntityVelocityControl entityVelocityControl;
    entityVelocityControl.entityID = EntityID(velocityControlV4.entityId);

    if (velocityControlV4.coordinateSystem == CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent)
    {
      entityVelocityControl.coordinateSystem = EObjectCoordinateSystem::WORLD;
    }
    else
    {
      entityVelocityControl.coordinateSystem = EObjectCoordinateSystem::LOCAL;
    }

    entityVelocityControl.angularVelocity = angularVelocity;
    entityVelocityControl.linearVelocity = linearVelocity;

    m_pVelocityControlHandler->HandleEntityVelocityControl(entityVelocityControl);
  }
}

void CCigiPacketHandlerV4::ParseViewControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::ViewCtrl viewControlV4;
  memcpy(&viewControlV4, pBuffer, sizeof(CIGI::V40::ViewCtrl));

  if (m_bByteSwap)
  {
    viewControlV4.doByteSwapping();
  }

  SCigiViewControl viewControl;
  viewControl.viewGroupID = ViewGroupID(viewControlV4.groupId);
  viewControl.viewID = ViewID(viewControlV4.viewId);
  viewControl.entityID = EntityID(viewControlV4.entityId);
  viewControl.bYawEnabled = viewControlV4.yawEnable;
  viewControl.bPitchEnabled = viewControlV4.pitchEnable;
  viewControl.bRollEnabled = viewControlV4.rollEnable;
  viewControl.rotation.yaw = Degrees(viewControlV4.yaw);
  viewControl.rotation.pitch = Degrees(viewControlV4.pitch);
  viewControl.rotation.roll = Degrees(viewControlV4.roll);
  viewControl.offsetEnabled[0] = viewControlV4.xOffsetEnable;
  viewControl.offsetEnabled[1] = viewControlV4.yOffsetEnable;
  viewControl.offsetEnabled[2] = viewControlV4.zOffsetEnable;
  viewControl.offset[0] = viewControlV4.xOffset;
  viewControl.offset[1] = viewControlV4.yOffset;
  viewControl.offset[2] = viewControlV4.zOffset;
  m_pViewHandler->Handle(viewControl);
}

void CCigiPacketHandlerV4::ParseWaveControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::WaveCtrl waveControlV4;
  memcpy(&waveControlV4, pBuffer, sizeof(CIGI::V40::WaveCtrl));

  if (m_bByteSwap)
  {
    waveControlV4.doByteSwapping();
  }

  int nID = waveControlV4.entityRegionId;
  SCigiWaveCondition condition;
  condition.waveID = waveControlV4.waveId;
  condition.fWaveHeight = waveControlV4.waveHeight;
  condition.fWavelength = waveControlV4.wavelength;
  condition.fPeriod = waveControlV4.period;
  condition.phaseOffset = Degrees360(waveControlV4.phaseOffset);
  condition.leading = Degrees180(waveControlV4.leading);

  condition.direction = Degrees360(waveControlV4.direction);

  condition.bWaveEnabled = waveControlV4.waveEnable;

  ECigiScope eScope = ConvertScope(static_cast<CIGI::V40::WaveCtrl::Scope>(waveControlV4.scope));
  m_pWaveControlHandler->Handle(condition, eScope, nID);
}

void CCigiPacketHandlerV4::ParseViewDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V40::ViewDefinition viewDefinitionV4;
  memcpy(&viewDefinitionV4, pBuffer, sizeof(CIGI::V40::ViewDefinition));

  if (m_bByteSwap)
  {
    viewDefinitionV4.doByteSwapping();
  }

  SCigiViewDefinition viewDefinition;
  viewDefinition.viewID = ViewID(viewDefinitionV4.viewId);
  viewDefinition.viewGroupID = ViewGroupID(viewDefinitionV4.groupId);
  viewDefinition.bTopEnabled = viewDefinitionV4.topEnable;
  viewDefinition.bBottomEnabled = viewDefinitionV4.bottomEnable;
  viewDefinition.bNearEnabled = viewDefinitionV4.nearEnable;
  viewDefinition.bFarEnabled = viewDefinitionV4.farEnable;
  viewDefinition.bLeftEnabled = viewDefinitionV4.leftEnable;
  viewDefinition.bRightEnabled = viewDefinitionV4.rightEnable;
  viewDefinition.fTop = viewDefinitionV4.top;
  viewDefinition.fBottom = viewDefinitionV4.bottom;
  viewDefinition.fNear = viewDefinitionV4.nearClip;
  viewDefinition.fFar = viewDefinitionV4.farClip;
  viewDefinition.fLeft = viewDefinitionV4.left;
  viewDefinition.fRight = viewDefinitionV4.right;
  viewDefinition.bReorder = viewDefinitionV4.reorder;
  viewDefinition.eProjectionMode = ConvertProjectionMode(static_cast<CIGI::V40::ViewDefinition::ProjectionType>(viewDefinitionV4.projectionType));
  viewDefinition.eMirrorMode = ConvertMirrorMode(static_cast<CIGI::V40::ViewDefinition::MirrorMode>(viewDefinitionV4.mirrorMode));
  viewDefinition.pixelReplicationMode = PixelReplicationMode(static_cast<uint8_t>(viewDefinitionV4.pixelReplicationMode));
  m_pViewHandler->Handle(viewDefinition);
}

void CCigiPacketHandlerV4::ParseWeatherControlPacket(uint8_t* pBuffer)
{
  CIGI::V40::WeatherCtrl weatherControl;
  memcpy(&weatherControl, pBuffer, sizeof(CIGI::V40::WeatherCtrl));

  if (m_bByteSwap)
  {
    weatherControl.doByteSwapping();
  }

  SCigiWeatherCondition condition;
  condition.bRandomWindsEnabled = weatherControl.randomWindsEnable;
  condition.bRandomLightningEnabled = weatherControl.randomLightningEnable;
  condition.cloudType = CloudType(static_cast<uint8_t>(weatherControl.cloudType));
  condition.fAerosolConcentration = weatherControl.aerosolConcentration;
  condition.fAirTemperature = TemperatureCelsius(weatherControl.airTemperature);
  condition.fBarometricPressure = weatherControl.barometricPressure;
  condition.coverage = Percentage(weatherControl.coverage);
  condition.humidity = Percentage(weatherControl.humidity);
  condition.fVisibilityRange = weatherControl.visibilityRange;
  condition.severity = WeatherSeverity(weatherControl.severity);
  condition.HorizontalWindSpeed = weatherControl.horizontalWindSpeed;
  condition.VerticalWindSpeed = weatherControl.verticalWindSpeed;
  condition.WindDirection = weatherControl.windDirection;
  condition.bWeatherEnabled = weatherControl.weatherEnable;
  condition.bTopScudEnabled = weatherControl.topScudEnable;
  condition.bBottomScudEnabled = weatherControl.bottomScudEnable;
  condition.bottomScudFrequency = Percentage(weatherControl.bottomScudFrequency);
  condition.topScudFrequency = Percentage(weatherControl.topScudFrequency);

  SCigiSpatialWeatherCondition spatialWeatherCondition;
  spatialWeatherCondition.fBaseElevation = weatherControl.baseElevation;
  spatialWeatherCondition.fBottomTransitionBandThickness = weatherControl.bottomTransitionBandThickness;
  spatialWeatherCondition.fTopTransitionBandThickness = weatherControl.topTransitionBandThickness;
  spatialWeatherCondition.fThickness = weatherControl.thickness;

  // set weather conditions on entity, region, or global level
  switch (weatherControl.scope)
  {
  case CIGI::V40::WeatherCtrl::Scope::eScope_Entity:
  {
    EntityID entityID = EntityID(weatherControl.entityRegionId);

    m_pEnvironmentalRegionHandler->SetEntityWeatherCondition(entityID, condition, spatialWeatherCondition);
    break;
  }
  case CIGI::V40::WeatherCtrl::Scope::eScope_Regional:
  {
    RegionID regionID = RegionID(weatherControl.entityRegionId);
    RegionalLayeredWeatherID layerID = RegionalLayeredWeatherID(weatherControl.layerId);

    m_pEnvironmentalRegionHandler->SetRegionalWeatherCondition(regionID, layerID, condition, spatialWeatherCondition);
    break;
  }
  case CIGI::V40::WeatherCtrl::Scope::eScope_Global:
  {
    GlobalLayeredWeatherID layerID = GlobalLayeredWeatherID(weatherControl.layerId);

    m_pEnvironmentalRegionHandler->SetGlobalWeatherCondition(layerID, condition, spatialWeatherCondition);
    break;
  }
  }
}

void CCigiPacketHandlerV4::ProcessPackets()
{
  // begin a new packet log frame if logging is enabled so that each call to ProcessPackets corresponds to a single frame in the log
  if (m_pPacketLoggerWriter)
  {
    m_pPacketLoggerWriter->BeginFrame();
  }

  uint8_t buffer[65535];
  const int nBufferSize = static_cast<int>(sizeof(buffer));

  SCigiPacketHeaderV4 packetHeader;
  int nMessageSize = 0;
  std::list<std::unique_ptr<sbio::utils::TBuffer<char>>> buffers;

  // read frame data from the packet logger if enabled, otherwise read from the socket
  if (m_pPacketLoggerReader)
  {
    // if reading from a log, wait until data is available for the next frame
    if (!m_pPacketLoggerReader->HasData())
    {
      return;
    }

    // read all packets for the current frame into a list of buffers so that we can process them one at a time and log them together as a single frame
    buffers = m_pPacketLoggerReader->ReadFrame();

    if (buffers.empty())
    {
      return;
    }

    nMessageSize = buffers.front()->GetNumElements();
    if (nMessageSize < 0 || nMessageSize >= nBufferSize)
    {
      stringstream ss;
      ss << "Skipping replay frame buffer with invalid size " << nMessageSize << " (max " << nBufferSize - 1 << ")" << endl;
      g_CigiLibGlobals.pLogger->LogWarning(ss.str());
      return;
    }

    memcpy(buffer, buffers.front()->GetBuffer(), nMessageSize);
    buffers.pop_front();
  }
  else
  {
    // if reading from the socket, read the first message for the frame
    nMessageSize = m_pSocketHostToIG->Receive((char*)buffer, nBufferSize - 1);
  }

  // process messages until there are no more for the current frame (either from the log or the socket)
  while (nMessageSize != -1)
  {
    if (!m_pPacketLoggerReader)
    {
      // check for host connection
      std::string sHostAddress = m_pSocketHostToIG->GetAddress();
      double fCurrentTime = m_ImageGenerator.GetTimeInSeconds();
      CheckHostConnection(sHostAddress, fCurrentTime);
    }

    // log the raw packet data for this message if logging is enabled
    if (m_pPacketLoggerWriter)
    {
      m_pPacketLoggerWriter->WriteBuffer(reinterpret_cast<const char*>(buffer), nMessageSize);
    }

    if (m_ImageGenerator.GetSetupOptions().bLogPacketText)
    {
      g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(m_ImageGenerator.GetSetupOptions().eCigiVersion, m_pSocketHostToIG->GetAddress(), buffer, nMessageSize);
    }

    // reset IG Control received flag at the start of processing each message so that we can enforce that only one IG Control packet is processed per message
    m_ImageGenerator.SetIgControlReceived(false);

    uint8_t* pBuffer = buffer;
    const uint8_t* pBufferEnd = buffer + nMessageSize;

    // process all packets in the current message
    while (pBuffer < pBufferEnd)
    {
      ptrdiff_t nRemainingBytes = pBufferEnd - pBuffer;
      if (nRemainingBytes < static_cast<ptrdiff_t>(sizeof(SCigiPacketHeaderV4)))
      {
        stringstream ss;
        ss << "Skipping truncated packet header with " << nRemainingBytes << " byte(s) remaining; expected at least " << sizeof(SCigiPacketHeaderV4) << endl;
        g_CigiLibGlobals.pLogger->LogWarning(ss.str());
        break;
      }

      memcpy(&packetHeader, pBuffer, sizeof(SCigiPacketHeaderV4));

      CheckForByteSwap(packetHeader);

      if (m_bByteSwap)
      {
        uint16_t nOpCode = static_cast<uint16_t>(packetHeader.eOpCode);
        CEndian::Swap(nOpCode);
        packetHeader.eOpCode = static_cast<ECigiOpCodeV4>(nOpCode);
      }

      uint16_t nPacketSize = packetHeader.nPacketSize;
      if (m_bByteSwap)
      {
        CEndian::Swap(nPacketSize);
      }

      // validate packet header
      if (nPacketSize == 0)
      {
        break;
      }

      // ensure packet size is large enough to contain the header
      if (nPacketSize < sizeof(SCigiPacketHeaderV4))
      {
        stringstream ss;
        ss << "Skipping invalid packet: opcode " << (int)packetHeader.eOpCode << ", size " << nPacketSize << " is smaller than header size " << sizeof(SCigiPacketHeaderV4) << endl;
        g_CigiLibGlobals.pLogger->LogWarning(ss.str());
        break;
      }

      // ensure packet size does not exceed remaining message size
      if (nPacketSize > nRemainingBytes)
      {
        stringstream ss;
        ss << "Skipping truncated packet: opcode " << (int)packetHeader.eOpCode << ", size " << nPacketSize << " exceeds remaining message bytes " << nRemainingBytes << endl;
        g_CigiLibGlobals.pLogger->LogWarning(ss.str());
        break;
      }

      // find handler function for packet opcode
      auto itFunction = m_PacketHandlerFunctions.find(packetHeader.eOpCode);

      // if no handler function exists for this opcode, log a warning and skip the packet
      if (itFunction == m_PacketHandlerFunctions.end())
      {
        stringstream ss;
        ss << "Received invalid packet: opcode " << (int)packetHeader.eOpCode << ", size " << nPacketSize << endl;
        g_CigiLibGlobals.pLogger->LogWarning(ss.str());
      }
      else if (m_ImageGenerator.GetOperationMode() == EIGMode::RESET && packetHeader.eOpCode != ECigiOpCodeV4::IG_CONTROL)
      {
        g_CigiLibGlobals.pLogger->LogWarning("Only IG Control packets will be processed while IG is in Standby mode!");
      }
      else
      {
        // call the handler function for this packet
        auto pFunction = itFunction->second;
        (this->*pFunction)(pBuffer);
      }

      // advance buffer pointer to the next packet in the message
      pBuffer += nPacketSize;
    }

    nMessageSize = -1;

    if (m_pPacketLoggerReader)
    {
      if (!buffers.empty())
      {
        // if reading from a log, read the next message for the frame if available
        nMessageSize = buffers.front()->GetNumElements();
        if (nMessageSize < 0 || nMessageSize >= nBufferSize)
        {
          stringstream ss;
          ss << "Skipping replay frame buffer with invalid size " << nMessageSize << " (max " << nBufferSize - 1 << ")" << endl;
          g_CigiLibGlobals.pLogger->LogWarning(ss.str());
          break;
        }

        memcpy(buffer, buffers.front()->GetBuffer(), nMessageSize);
        buffers.pop_front();
      }
    }
    else
    {
      // if reading from the socket, read the next message for the frame if available
      nMessageSize = m_pSocketHostToIG->Receive((char*)buffer, nBufferSize - 1);
    }
  }

  // end the packet log frame if logging is enabled
  if (m_pPacketLoggerWriter)
  {
    m_pPacketLoggerWriter->EndFrame();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
