//Copyright SimBlocks LLC 2016-2026
#include "PacketHandlerV3.h"
#include "AnimationControlHandler.h"
#include "ArticulatedPartHandler.h"
#include "AtmosphereControlHandler.h"
#include "CelestialSphereHandler.h"
#include "CigiEntity.h"
#include "CigiEvent.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "CigiView.h"
#include "CollisionControlHandler.h"
#include "ComponentControlHandler.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/EngineTypes.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EntityControlHandler.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "EnvironmentalRegionHandler.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "CigiLib/CigiEntityTypes.h"
#include "ImageGenerator.h"
#include "MaritimeSurfaceConditionPacketHandler.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/MathTypes.h"
#include "SymbolHandler.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/EventHandler.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/Endian.h"
#include "UtilitiesLib/UDPReceiveSocket.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/PacketLoggerWriter.h"
#include "UtilitiesLib/PacketLoggerReader.h"
#include "VelocityControlHandler.h"
#include "ViewHandler.h"
#include "WaveControlHandler.h"
#include "TerrainHandler.h"
#include "AccelerationControlHandler.h"
#include "MotionTrackerControlHandler.h"
#include "EarthReferenceModelHandler.h"
#include "SensorControlHandler.h"
#include "libCIGI/Packets/3_3/AerosolConcentrationResponse.h"
#include "libCIGI/Packets/3_3/AnimationStopNotification.h"
#include "libCIGI/Packets/3_3/ArticulatedPartCtrl.h"
#include "libCIGI/Packets/3_3/AtmosphereCtrl.h"
#include "libCIGI/Packets/3_3/CelestialSphereCtrl.h"
#include "libCIGI/Packets/3_3/CollisionDetectionSegmentDefinition.h"
#include "libCIGI/Packets/3_3/CollisionDetectionSegmentNotification.h"
#include "libCIGI/Packets/3_3/CollisionDetectionVolumeDefinition.h"
#include "libCIGI/Packets/3_3/CollisionDetectionVolumeNotification.h"
#include "libCIGI/Packets/3_3/ComponentCtrl.h"
#include "libCIGI/Packets/3_3/ConformalClampedEntityCtrl.h"
#include "libCIGI/Packets/3_3/EarthReferenceModelDefinition.h"
#include "libCIGI/Packets/3_3/EntityCtrl.h"
#include "libCIGI/Packets/3_3/EnvironmentalConditionsRequest.h"
#include "libCIGI/Packets/3_3/EnvironmentalRegionCtrl.h"
#include "libCIGI/Packets/3_3/EventNotification.h"
#include "libCIGI/Packets/3_3/HATHOTExtendedResponse.h"
#include "libCIGI/Packets/3_3/HATHOTRequest.h"
#include "libCIGI/Packets/3_3/HATHOTResponse.h"
#include "libCIGI/Packets/3_3/IGCtrl.h"
#include "libCIGI/Packets/3_3/IGMessage.h"
#include "libCIGI/Packets/3_3/LineOfSightExtendedResponse.h"
#include "libCIGI/Packets/3_3/LineOfSightResponse.h"
#include "libCIGI/Packets/3_3/LineOfSightSegmentRequest.h"
#include "libCIGI/Packets/3_3/LineOfSightVectorRequest.h"
#include "libCIGI/Packets/3_3/MaritimeSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/3_3/MaritimeSurfaceConditionsResponse.h"
#include "libCIGI/Packets/3_3/MotionTrackerCtrl.h"
#include "libCIGI/Packets/3_3/PositionRequest.h"
#include "libCIGI/Packets/3_3/PositionResponse.h"
#include "libCIGI/Packets/3_3/SensorCtrl.h"
#include "libCIGI/Packets/3_3/SensorExtendedResponse.h"
#include "libCIGI/Packets/3_3/SensorResponse.h"
#include "libCIGI/Packets/3_3/ShortArticulatedPartCtrl.h"
#include "libCIGI/Packets/3_3/ShortComponentCtrl.h"
#include "libCIGI/Packets/3_3/ShortSymbolCtrl.h"
#include "libCIGI/Packets/3_3/SoF.h"
#include "libCIGI/Packets/3_3/SymbolCircleDefinition.h"
#include "libCIGI/Packets/3_3/SymbolClone.h"
#include "libCIGI/Packets/3_3/SymbolCtrl.h"
#include "libCIGI/Packets/3_3/SymbolLineDefinition.h"
#include "libCIGI/Packets/3_3/SymbolSurfaceDefinition.h"
#include "libCIGI/Packets/3_3/SymbolTextDefinition.h"
#include "libCIGI/Packets/3_3/TerrestrialSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/3_3/TerrestrialSurfaceConditionsResponse.h"
#include "libCIGI/Packets/3_3/TrajectoryDefinition.h"
#include "libCIGI/Packets/3_3/RateCtrl.h"
#include "libCIGI/Packets/3_3/ViewCtrl.h"
#include "libCIGI/Packets/3_3/ViewDefinition.h"
#include "libCIGI/Packets/3_3/WaveCtrl.h"
#include "libCIGI/Packets/3_3/WeatherConditionsResponse.h"
#include "libCIGI/Packets/3_3/WeatherCtrl.h"
#include <algorithm>
#include <iostream>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::symbol;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiPacketHandlerV3::CCigiPacketHandlerV3(CCigiImageGenerator& imageGenerator, const std::string& sHostIP, int nHostToIgPort, int nIgToHostPort) : CCigiPacketHandler(imageGenerator, nHostToIgPort)
{
  m_PacketHandlerFunctions[ECigiOpCodeV3::IG_CONTROL] = &CCigiPacketHandlerV3::ParseIgControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::ENTITY_CONTROL] = &CCigiPacketHandlerV3::ParseEntityControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::CONFORMAL_CLAMPED_ENTITY_CONTROL] = &CCigiPacketHandlerV3::ParseConformalClampedEntityControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::COMPONENT_CONTROL] = &CCigiPacketHandlerV3::ParseComponentControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SHORT_COMPONENT_CONTROL] = &CCigiPacketHandlerV3::ParseShortComponentControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SHORT_SYMBOL_CONTROL] = &CCigiPacketHandlerV3::ParseShortSymbolControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::ARTICULATED_PART_CONTROL] = &CCigiPacketHandlerV3::ParseArticulatedPartControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SHORT_ARTICULATED_PART_CONTROL] = &CCigiPacketHandlerV3::ParseShortArticulatedPartControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::RATE_CONTROL] = &CCigiPacketHandlerV3::ParseRateControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::CELESTIAL_SPHERE_CONTROL] = &CCigiPacketHandlerV3::ParseCelestialSphereControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::ATMOSPHERE_CONTROL] = &CCigiPacketHandlerV3::ParseAtmosphereControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::ENVIRONMENTAL_REGION_CONTROL] = &CCigiPacketHandlerV3::ParseEnvironmentalRegionControl;
  m_PacketHandlerFunctions[ECigiOpCodeV3::WEATHER_CONTROL] = &CCigiPacketHandlerV3::ParseWeatherControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::MARITIME_SURFACE_CONDITIONS_CONTROL] = &CCigiPacketHandlerV3::ParseMaritimeSurfaceConditionsControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::WAVE_CONTROL] = &CCigiPacketHandlerV3::ParseWaveControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL] = &CCigiPacketHandlerV3::ParseTerrestrialSurfaceConditionsControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::VIEW_CONTROL] = &CCigiPacketHandlerV3::ParseViewControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SENSOR_CONTROL] = &CCigiPacketHandlerV3::ParseSensorControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::MOTION_TRACKER_CONTROL] = &CCigiPacketHandlerV3::ParseMotionTrackerControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::EARTH_REFERENCE_MODEL_DEFINITION] = &CCigiPacketHandlerV3::ParseEarthReferenceModelDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::TRAJECTORY_DEFINITION] = &CCigiPacketHandlerV3::ParseTrajectoryDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::VIEW_DEFINITION] = &CCigiPacketHandlerV3::ParseViewDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_DEFINITION] = &CCigiPacketHandlerV3::ParseCollisionDetectionSegmentDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_DEFINITION] = &CCigiPacketHandlerV3::ParseCollisionDetectionVolumeDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::POSITION_REQUEST] = &CCigiPacketHandlerV3::ParsePositionRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::ENVIRONMENTAL_CONDITIONS_REQUEST] = &CCigiPacketHandlerV3::ParseEnvironmentalConditionsRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SYMBOL_SURFACE_DEFINITION] = &CCigiPacketHandlerV3::ParseSymbolSurfaceDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SYMBOL_TEXT_DEFINITION] = &CCigiPacketHandlerV3::ParseSymbolTextDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SYMBOL_CIRCLE_DEFINITION] = &CCigiPacketHandlerV3::ParseSymbolCircleDefinitionPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SYMBOL_LINE_DEFINITION] = &CCigiPacketHandlerV3::ParseSymbolLineDefinitonPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SYMBOL_CLONE] = &CCigiPacketHandlerV3::ParseSymbolClonePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SYMBOL_CONTROL] = &CCigiPacketHandlerV3::ParseSymbolControlPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::HAT_HOT_REQUEST] = &CCigiPacketHandlerV3::ParseHatHotRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::LINE_OF_SIGHT_SEGMENT_REQUEST] = &CCigiPacketHandlerV3::ParseLineOfSightSegmentRequestPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::LINE_OF_SIGHT_VECTOR_REQUEST] = &CCigiPacketHandlerV3::ParseLineOfSightVectorRequestPacket;
}

CCigiPacketHandlerV3::~CCigiPacketHandlerV3()
{
}

EClamp ConvertClamp(CIGI::V33::EntityCtrl::GroundClamp eGroundClamp)
{
  switch (eGroundClamp)
  {
  case CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_NoClamp:
    return EClamp::NONE;
    break;
  case CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_NonConformal:
    return EClamp::NON_CONFORMAL;
    break;
  case CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_Conformal:
    return EClamp::CONFORMAL;
    break;
  }

  return EClamp::UNKNOWN;
}

EAttachState ConvertAttachState33(CIGI::V33::EntityCtrl::AttachState eAttachState)
{
  switch (eAttachState)
  {
  case CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached:
    return EAttachState::ATTACH;
  case CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached:
    return EAttachState::DETACH;
  }
  return EAttachState::UNKNOWN;
}

EAnimationLoopMode ConvertToAnimationLoopMode(CIGI::ui8 animationLoopMode)
{
  if (animationLoopMode == 0)
  {
    return EAnimationLoopMode::ONE_SHOT;
  }
  else if (animationLoopMode == 1)
  {
    return EAnimationLoopMode::CONTINUOUS;
  }

  return EAnimationLoopMode::UNKNOWN;
}

EAnimationState ConvertToAnimationState(CIGI::V33::EntityCtrl::AnimationState animationState)
{
  switch (animationState)
  {
  case CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Play:
    return EAnimationState::PLAY;
  case CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Stop:
    return EAnimationState::STOP;
  case CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Pause:
    return EAnimationState::PAUSE;
  case CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Continue:
    return EAnimationState::CONTINUE;
  }
  return EAnimationState::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseArticulatedPartControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ArticulatedPartCtrl articulatedPartControlV3;
  memcpy(&articulatedPartControlV3, pBuffer, sizeof(CIGI::V33::ArticulatedPartCtrl));

  if (m_bByteSwap)
  {
    articulatedPartControlV3.doByteSwapping();
  }

  // verify entity exists
  auto entityID = EntityID(articulatedPartControlV3.entityId);
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entityID))
  {
    return;
  }

  SCigiArticulatedPart articulatedPart;
  articulatedPart.entityID = entityID;
  articulatedPart.articulatedPartID = ArticulatedPartID(articulatedPartControlV3.articulatedPartId);
  articulatedPart.bEnabled = articulatedPartControlV3.articulatedPartEnable;
  articulatedPart.bOffsetEnabled[0] = articulatedPartControlV3.xOffsetEnable;
  articulatedPart.bOffsetEnabled[1] = articulatedPartControlV3.yOffsetEnable;
  articulatedPart.bOffsetEnabled[2] = articulatedPartControlV3.zOffsetEnable;
  articulatedPart.offset[0] = articulatedPartControlV3.xOffset;
  articulatedPart.offset[1] = articulatedPartControlV3.yOffset;
  articulatedPart.offset[2] = articulatedPartControlV3.zOffset;
  articulatedPart.bYawEnabled = articulatedPartControlV3.yawEnable;
  articulatedPart.bPitchEnabled = articulatedPartControlV3.pitchEnable;
  articulatedPart.bRollEnabled = articulatedPartControlV3.rollEnable;
  articulatedPart.rotation.yaw = Degrees(articulatedPartControlV3.yaw);
  articulatedPart.rotation.pitch = Degrees(articulatedPartControlV3.pitch);
  articulatedPart.rotation.roll = Degrees(articulatedPartControlV3.roll);
  m_pArticulatedPartHandler->Handle(articulatedPart);
}

void CCigiPacketHandlerV3::ParseAtmosphereControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::AtmosphereCtrl atmosphereControlV3;
  memcpy(&atmosphereControlV3, pBuffer, sizeof(CIGI::V33::AtmosphereCtrl));

  if (m_bByteSwap)
  {
    atmosphereControlV3.doByteSwapping();
  }

  SAtmosphere atmosphere;
  atmosphere.fAirTemperature = TemperatureCelsius(atmosphereControlV3.globalAirTemperature);
  atmosphere.fBarometricPressure = atmosphereControlV3.globalBarometricPressure;
  atmosphere.fHorizontalWindSpeed = atmosphereControlV3.globalHorizontalWindSpeed;
  atmosphere.fVerticalWindSpeed = atmosphereControlV3.globalVerticalWindSpeed;
  atmosphere.fHumidity = atmosphereControlV3.globalHumidity;
  atmosphere.fVisibilityRange = atmosphereControlV3.globalVisibilityRange;
  atmosphere.fWindDirection = atmosphereControlV3.globalWindDirection;
  m_pAtmosphereControlHandler->Handle(atmosphereControlV3.atmosphericModelEnable, atmosphere);
}

void CCigiPacketHandlerV3::ParseCelestialSphereControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::CelestialSphereCtrl celestialSphereControl;
  memcpy(&celestialSphereControl, pBuffer, sizeof(CIGI::V33::CelestialSphereCtrl));

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
    time.seconds = Second(0);//added in version 4

    SDate date;
    date.year = Year(celestialSphereControl.year());
    CigiMonth month = CigiMonth(celestialSphereControl.month());
    date.month = ConvertToMonth(month);
    date.day = Day(celestialSphereControl.day());

    m_pCelestialSphereHandler->SetDateTime(date, time);
  }
}

void CCigiPacketHandlerV3::ParseCollisionDetectionSegmentDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::CollisionDetectionSegmentDefinition collisionDetectionSegmentDefinitionV3;
  memcpy(&collisionDetectionSegmentDefinitionV3, pBuffer, sizeof(CIGI::V33::CollisionDetectionSegmentDefinition));

  if (m_bByteSwap)
  {
    collisionDetectionSegmentDefinitionV3.doByteSwapping();
  }

  SCollisionDetectionSegmentDefinition collisionDetectionSegmentDefinition;
  collisionDetectionSegmentDefinition.beg[0] = collisionDetectionSegmentDefinitionV3.x1;
  collisionDetectionSegmentDefinition.beg[1] = collisionDetectionSegmentDefinitionV3.y1;
  collisionDetectionSegmentDefinition.beg[2] = collisionDetectionSegmentDefinitionV3.z1;
  collisionDetectionSegmentDefinition.end[0] = collisionDetectionSegmentDefinitionV3.x2;
  collisionDetectionSegmentDefinition.end[1] = collisionDetectionSegmentDefinitionV3.y2;
  collisionDetectionSegmentDefinition.end[2] = collisionDetectionSegmentDefinitionV3.z2;
  collisionDetectionSegmentDefinition.bSegmentEnabled = collisionDetectionSegmentDefinitionV3.segmentEnable;
  collisionDetectionSegmentDefinition.entityID = EntityID(collisionDetectionSegmentDefinitionV3.entityId);
  collisionDetectionSegmentDefinition.nMaterialMask = collisionDetectionSegmentDefinitionV3.materialMask;
  collisionDetectionSegmentDefinition.segmentID = SegmentID(collisionDetectionSegmentDefinitionV3.segmentId);

  m_pCollisionControlHandler->HandleCigiCollisionDetectionSegmentDefinition(collisionDetectionSegmentDefinition);
}

sbio::cigi::EVolumeType ConvertCollisionVolumeType33(CIGI::V33::CollisionDetectionVolumeDefinition::Type eVolumeType)
{
  switch (eVolumeType)
  {
  case CIGI::V33::CollisionDetectionVolumeDefinition::Type::eType_Cuboid:
    return sbio::cigi::EVolumeType::CUBOID;
  case CIGI::V33::CollisionDetectionVolumeDefinition::Type::eType_Sphere:
    return sbio::cigi::EVolumeType::SPHERE;
  }
  return sbio::cigi::EVolumeType::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseCollisionDetectionVolumeDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::CollisionDetectionVolumeDefinition collisionDetectionVolumeDefinitionV3;
  memcpy(&collisionDetectionVolumeDefinitionV3, pBuffer, sizeof(CIGI::V33::CollisionDetectionVolumeDefinition));

  if (m_bByteSwap)
  {
    collisionDetectionVolumeDefinitionV3.doByteSwapping();
  }

  EVolumeType eVolumeType = ConvertCollisionVolumeType33(static_cast<CIGI::V33::CollisionDetectionVolumeDefinition::Type>(collisionDetectionVolumeDefinitionV3.volumeType));

  if (eVolumeType == sbio::cigi::EVolumeType::CUBOID)
  {
    sbio::cigi::SCollisionDetectionCuboidDefinition collisionDetectionVolumeDefinition;
    collisionDetectionVolumeDefinition.entityID = EntityID(collisionDetectionVolumeDefinitionV3.entityId);
    collisionDetectionVolumeDefinition.bVolumeEnabled = collisionDetectionVolumeDefinitionV3.volumeEnable;
    collisionDetectionVolumeDefinition.offset[0] = collisionDetectionVolumeDefinitionV3.x;
    collisionDetectionVolumeDefinition.offset[1] = collisionDetectionVolumeDefinitionV3.y;
    collisionDetectionVolumeDefinition.offset[2] = collisionDetectionVolumeDefinitionV3.z;
    collisionDetectionVolumeDefinition.volumeID = VolumeID(collisionDetectionVolumeDefinitionV3.volumeId);

    collisionDetectionVolumeDefinition.fDepth = collisionDetectionVolumeDefinitionV3.depth;
    collisionDetectionVolumeDefinition.rotation.yaw = Degrees(collisionDetectionVolumeDefinitionV3.yaw);
    collisionDetectionVolumeDefinition.rotation.pitch = Degrees(collisionDetectionVolumeDefinitionV3.pitch);
    collisionDetectionVolumeDefinition.rotation.roll = Degrees(collisionDetectionVolumeDefinitionV3.roll);
    collisionDetectionVolumeDefinition.fWidth = collisionDetectionVolumeDefinitionV3.width;
    collisionDetectionVolumeDefinition.fHeight = collisionDetectionVolumeDefinitionV3.heightRadius;

    m_pCollisionControlHandler->HandleCigiCollisionCuboidDefinition(collisionDetectionVolumeDefinition);
  }
  else if (eVolumeType == sbio::cigi::EVolumeType::SPHERE)
  {
    sbio::cigi::SCollisionDetectionSphereDefinition collisionDetectionVolumeDefinition;
    collisionDetectionVolumeDefinition.entityID = EntityID(collisionDetectionVolumeDefinitionV3.entityId);
    collisionDetectionVolumeDefinition.bVolumeEnabled = collisionDetectionVolumeDefinitionV3.volumeEnable;
    collisionDetectionVolumeDefinition.offset[0] = collisionDetectionVolumeDefinitionV3.x;
    collisionDetectionVolumeDefinition.offset[1] = collisionDetectionVolumeDefinitionV3.y;
    collisionDetectionVolumeDefinition.offset[2] = collisionDetectionVolumeDefinitionV3.z;
    collisionDetectionVolumeDefinition.volumeID = VolumeID(collisionDetectionVolumeDefinitionV3.volumeId);

    collisionDetectionVolumeDefinition.fRadius = collisionDetectionVolumeDefinitionV3.heightRadius;

    m_pCollisionControlHandler->HandleCigiCollisionSphereDefinition(collisionDetectionVolumeDefinition);
  }
}

void CCigiPacketHandlerV3::ParseComponentControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ComponentCtrl componentControlV3;
  memcpy(&componentControlV3, pBuffer, sizeof(CIGI::V33::ComponentCtrl));

  if (m_bByteSwap)
  {
    componentControlV3.doByteSwapping();
  }

  SCigiComponentControl componentControl;
  componentControl.key.componentID = CigiComponentID(componentControlV3.componentId);
  componentControl.key.componentClassID = CigiComponentClassID(static_cast<uint8_t>(componentControlV3.componentClass));
  componentControl.key.nInstanceID = componentControlV3.instanceId;
  componentControl.state.nComponentState = componentControlV3.componentState;
  componentControl.state.componentData[0] = componentControlV3.componentData[0];
  componentControl.state.componentData[1] = componentControlV3.componentData[1];
  componentControl.state.componentData[2] = componentControlV3.componentData[2];
  componentControl.state.componentData[3] = componentControlV3.componentData[3];
  componentControl.state.componentData[4] = componentControlV3.componentData[4];
  componentControl.state.componentData[5] = componentControlV3.componentData[5];

  CComponentDataParser<CIGI::V33::ComponentCtrl> componentDataParser(componentControlV3);
  m_pComponentControlHandler->Handle(componentControl, &componentDataParser);
}

void CCigiPacketHandlerV3::ParseConformalClampedEntityControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ConformalClampedEntityCtrl conformalClampedEntityControl;
  memcpy(&conformalClampedEntityControl, pBuffer, sizeof(CIGI::V33::ConformalClampedEntityCtrl));

  if (m_bByteSwap)
  {
    conformalClampedEntityControl.doByteSwapping();
  }

  SCigiConformalClampedEntityPosition conformalClampedEntityPosition;
  conformalClampedEntityPosition.entityID = EntityID(conformalClampedEntityControl.entityId);
  conformalClampedEntityPosition.fYaw = Degrees(conformalClampedEntityControl.yaw);
  conformalClampedEntityPosition.latitude = Latitude(conformalClampedEntityControl.latitude);
  conformalClampedEntityPosition.longitude = Longitude(conformalClampedEntityControl.longitude);

  //Conformal Clamped Entity Control renamed to Conformal Clamped Entity Position in 4.0
  m_pEntityControlHandler->HandleCigiConformalClampedEntityPosition(conformalClampedEntityPosition);
}

void CCigiPacketHandlerV3::ParseEarthReferenceModelDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::EarthReferenceModelDefinition earthRefModelV3;
  memcpy(&earthRefModelV3, pBuffer, sizeof(CIGI::V33::EarthReferenceModelDefinition));

  if (m_bByteSwap)
  {
    earthRefModelV3.doByteSwapping();
  }

  SCigiEarthReferenceModel earthReferenceModel;
  if (earthRefModelV3.customERMEnable)
  {
    earthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::HOST_DEFINED;
    earthReferenceModel.fEquatorialRadius = earthRefModelV3.equatorialRadius;
    earthReferenceModel.fFlattening = earthRefModelV3.flattening;
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

sbio::cigi::EActiveState ConvertToActiveState33(CIGI::V33::EntityCtrl::State eEntityState)
{
  if (eEntityState == CIGI::V33::EntityCtrl::State::eState_Inactive)
  {
    return EActiveState::INACTIVE_STANDBY;
  }
  else if (eEntityState == CIGI::V33::EntityCtrl::State::eState_Active)
  {
    return EActiveState::ACTIVE;
  }
  else if (eEntityState == CIGI::V33::EntityCtrl::State::eState_Destroyed)
  {
    return EActiveState::DESTROYED;
  }

  return EActiveState::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseEntityControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::EntityCtrl entityControlV3;
  memcpy(&entityControlV3, pBuffer, sizeof(CIGI::V33::EntityCtrl));

  if (m_bByteSwap)
  {
    entityControlV3.doByteSwapping();
  }

  SEntityControl entityControl;
  entityControl.alpha = entityControlV3.alpha;
  entityControl.bCollisionReportingEnabled = entityControlV3.collisionReportEnable;
  entityControl.bInheritAlpha = entityControlV3.inheritAlpha;
  entityControl.bSmoothingEnabled = entityControlV3.extrapolationEnable;
  entityControl.entityID = EntityID(entityControlV3.entityId);
  entityControl.eState = ConvertToActiveState33(static_cast<CIGI::V33::EntityCtrl::State>(entityControlV3.entityState));
  entityControl.shortEntityTypeID = ShortEntityTypeID(entityControlV3.entityType);
  entityControl.entityType = g_CigiLibGlobals.pCigiEntityTypes->GetExtendedEntityType(ShortEntityTypeID(entityControl.shortEntityTypeID.Value()));
  entityControl.eExtendedEntityType = EExtendedEntityType::SHORT;

  m_pEntityControlHandler->HandleCigiEntityControl(entityControl, false);

  EntityID entityID = EntityID(entityControlV3.entityId);

  const int ANIMATION_FORWARD = 0;

  SCigiAnimationControl animationControl;
  animationControl.eAnimationLoopMode = ConvertToAnimationLoopMode(entityControlV3.animationLoopMode);
  animationControl.eAnimationState = ConvertToAnimationState(static_cast<CIGI::V33::EntityCtrl::AnimationState>(entityControlV3.animationState));
  animationControl.fAnimationSpeed = entityControlV3.animationDirection == ANIMATION_FORWARD ? 1.0f : -1.0f;
  animationControl.entityID = entityID;
  m_pAnimationControlHandler->HandleCigiEntityAnimation(animationControl);

  EntityID parentID = EntityID(entityControlV3.parentId);
  CIGI::V33::EntityCtrl::AttachState attachState = static_cast<CIGI::V33::EntityCtrl::AttachState>(entityControlV3.attachState);
  CIGI::V33::EntityCtrl::GroundClamp clamp = static_cast<CIGI::V33::EntityCtrl::GroundClamp>(entityControlV3.groundOceanClamp);

  sbio::math::Vec3 position;
  position[0] = entityControlV3.latitudeXOffset;
  position[1] = entityControlV3.longitudeYOffset;
  position[2] = entityControlV3.altitudeZOffset;

  TCigiBodyEulerRotation rotation;
  rotation.yaw = Degrees(entityControlV3.yaw);
  rotation.pitch = Degrees(entityControlV3.pitch);
  rotation.roll = Degrees(entityControlV3.roll);

  m_pEntityControlHandler->HandleCigiEntityPosition(entityID, parentID, ConvertAttachState33(attachState), ConvertClamp(clamp), position, rotation);
}

void CCigiPacketHandlerV3::ParseEnvironmentalConditionsRequestPacket(uint8_t* pBuffer)
{
  CIGI::V33::EnvironmentalConditionsRequest environmentalRegionConditionsRequestV3;
  memcpy(&environmentalRegionConditionsRequestV3, pBuffer, sizeof(CIGI::V33::EnvironmentalConditionsRequest));

  if (m_bByteSwap)
  {
    environmentalRegionConditionsRequestV3.doByteSwapping();
  }

  SEnvironmentalConditionsRequest request;
  request.bAerosolConcentrationsRequest = environmentalRegionConditionsRequestV3.requestType & CIGI::V33::EnvironmentalConditionsRequest::RequestType::eRequestType_AerosolConcentrations;
  request.bMaritimeSurfaceConditionsRequest = environmentalRegionConditionsRequestV3.requestType & CIGI::V33::EnvironmentalConditionsRequest::RequestType::eRequestType_MaritimeSurfaceConditions;
  request.bTerrestrialSurfaceConditionsRequest = environmentalRegionConditionsRequestV3.requestType & CIGI::V33::EnvironmentalConditionsRequest::RequestType::eRequestType_TerrestrialSurfaceConditions;
  request.bWeatherConditionsRequest = environmentalRegionConditionsRequestV3.requestType & CIGI::V33::EnvironmentalConditionsRequest::RequestType::eRequestType_WeatherConditions;

  request.geodeticCoordinates.latitude = Latitude(environmentalRegionConditionsRequestV3.latitude);
  request.geodeticCoordinates.longitude = Longitude(environmentalRegionConditionsRequestV3.longitude);
  request.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(environmentalRegionConditionsRequestV3.altitude);

  request.nRequestID = environmentalRegionConditionsRequestV3.requestId;
  m_pEnvironmentalRegionHandler->Handle(request);
}

EActiveState ConvertRegion33(CIGI::V33::EnvironmentalRegionCtrl::State eRegionState)
{
  switch (eRegionState)
  {
  default:
    return EActiveState::UNKNOWN;
    break;
  case CIGI::V33::EnvironmentalRegionCtrl::State::eState_Inactive:
    return EActiveState::INACTIVE_STANDBY;
    break;
  case CIGI::V33::EnvironmentalRegionCtrl::State::eState_Active:
    return EActiveState::ACTIVE;
    break;
  case CIGI::V33::EnvironmentalRegionCtrl::State::eState_Destroyed:
    return EActiveState::DESTROYED;
    break;
  }
}

sbio::cigi::EMergeState ConvertMergeState33(CIGI::ui8 eMergeState)
{
  switch (eMergeState)
  {
  case 0:
    return EMergeState::USE_LAST;
  case 1:
    return EMergeState::MERGE;
  }

  return EMergeState::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseEnvironmentalRegionControl(uint8_t* pBuffer)
{
  CIGI::V33::EnvironmentalRegionCtrl environmentalRegionControlV3;
  memcpy(&environmentalRegionControlV3, pBuffer, sizeof(CIGI::V33::EnvironmentalRegionCtrl));

  if (m_bByteSwap)
  {
    environmentalRegionControlV3.doByteSwapping();
  }

  SCigiEnvironmentalRegion environmentalRegion;
  environmentalRegion.regionID = RegionID(environmentalRegionControlV3.regionId);
  environmentalRegion.eRegionState = ConvertRegion33(static_cast<CIGI::V33::EnvironmentalRegionCtrl::State>(environmentalRegionControlV3.regionState));
  environmentalRegion.eMergeWeatherProperties = ConvertMergeState33(environmentalRegionControlV3.mergeWeatherProperties);
  environmentalRegion.eMergeAerosolConcentrations = ConvertMergeState33(environmentalRegionControlV3.mergeAerosolConcentrations);
  environmentalRegion.eMergeMaritimeSurfaceConditions = ConvertMergeState33(environmentalRegionControlV3.mergeMaritimeSurfaceConditions);
  environmentalRegion.eMergeTerrestrialSurfaceConditions = ConvertMergeState33(environmentalRegionControlV3.mergeTerrestrialSurfaceConditions);
  environmentalRegion.latitude = Latitude(environmentalRegionControlV3.latitude);
  environmentalRegion.longitude = Longitude(environmentalRegionControlV3.longitude);
  environmentalRegion.size[0] = environmentalRegionControlV3.sizeX;
  environmentalRegion.size[1] = environmentalRegionControlV3.sizeY;
  environmentalRegion.fCornerRadius = environmentalRegionControlV3.cornerRadius;
  environmentalRegion.fRotation = Degrees180(environmentalRegionControlV3.rotation);
  environmentalRegion.fTransition = environmentalRegionControlV3.transitionPerimeter;
  m_pEnvironmentalRegionHandler->Handle(environmentalRegion);
}

sbio::cigi::ERequestType ConvertRequestType33(CIGI::V33::HATHOTRequest::Type eRequestType)
{
  switch (eRequestType)
  {
  case CIGI::V33::HATHOTRequest::Type::eType_HAT:
    return ERequestType::HAT;
  case CIGI::V33::HATHOTRequest::Type::eType_HOT:
    return ERequestType::HOT;
  default:
    return ERequestType::EXTENDED;
  }
}

ETopLevelCoordinateSystem ConvertCoordinateSystem33(CIGI::V33::HATHOTRequest::CoordinateSystem eCoordSysGroup)
{
  switch (eCoordSysGroup)
  {
  case CIGI::V33::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
    return ETopLevelCoordinateSystem::GEODETIC;
  case CIGI::V33::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Entity:
    return ETopLevelCoordinateSystem::ENTITY;
    break;
  }

  return ETopLevelCoordinateSystem::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseHatHotRequestPacket(uint8_t* pBuffer)
{
  CIGI::V33::HATHOTRequest hatHotReqV3;
  memcpy(&hatHotReqV3, pBuffer, sizeof(CIGI::V33::HATHOTRequest));

  if (m_bByteSwap)
  {
    hatHotReqV3.doByteSwapping();
  }

  auto requestType = ConvertRequestType33(static_cast<CIGI::V33::HATHOTRequest::Type>(hatHotReqV3.requestType));
  auto coordinateSystem = ConvertCoordinateSystem33(static_cast<CIGI::V33::HATHOTRequest::CoordinateSystem>(hatHotReqV3.coordinateSystem));

  if (coordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
  {
    SHATHOTGlobalRequest data;
    data.requestID = HATHOTID(hatHotReqV3.HATHOTId);
    data.updatePeriod = UpdatePeriod(hatHotReqV3.updatePeriod);
    data.geodeticCoordinates.latitude = Latitude(hatHotReqV3.latitudeXOffset);
    data.geodeticCoordinates.longitude = Longitude(hatHotReqV3.longitudeYOffset);
    data.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(hatHotReqV3.altitudeZOffset);
    data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
    data.eRequestType = requestType;
    m_pTerrainHandler->Handle(data);
  }
  else
  {
    SHATHOTEntityRequest data;
    data.requestID = HATHOTID(hatHotReqV3.HATHOTId);
    data.updatePeriod = UpdatePeriod(hatHotReqV3.updatePeriod);
    data.entityID = EntityID(hatHotReqV3.entityId);
    data.offset[0] = hatHotReqV3.latitudeXOffset;
    data.offset[1] = hatHotReqV3.longitudeYOffset;
    data.offset[2] = hatHotReqV3.altitudeZOffset;
    data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
    data.eRequestType = requestType;
    m_pTerrainHandler->Handle(data);
  }
}

void CCigiPacketHandlerV3::ParseIgControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::IGCtrl igControlV3;
  memcpy(&igControlV3, pBuffer, sizeof(CIGI::V33::IGCtrl));

  bool bByteSwap = (igControlV3.byteSwapMagicNumber == CIGI::V33::IGCtrl::kByteSwap);

  if (!g_CigiLibGlobals.pImageGenerator->GetIgControlReceivedAfterPlay())
  {
    g_CigiLibGlobals.pImageGenerator->SetIgControlReceivedAfterPlay(true);
    m_bByteSwap = bByteSwap;
    if (m_bByteSwap)
    {
      g_CigiLibGlobals.pLogger->LogInformation("Byte swapping detected for host data.");
    }
    else
    {
      g_CigiLibGlobals.pLogger->LogInformation("No byte swapping required for host data.");
    }
  }

  if (m_bByteSwap)
  {
    igControlV3.doByteSwapping();
  }

  SCigiIgControl igControl;
  igControl.bEntityTypeSubstitutionEnabled = false;//added in version 4.0
  igControl.bSmoothingEnabled = igControlV3.extrapolationEnable;
  igControl.bTimestampValid = igControlV3.timestampValid;
  igControl.databaseNumber = CigiDatabaseNumber(igControlV3.dbNumber);
  igControl.eIgMode = ConvertToIGMode33(static_cast<CIGI::V33::IGCtrl::IGMode>(igControlV3.igMode));
  igControl.hostFrameNumber = FrameNumber(igControlV3.hostFrameNumber);
  igControl.lastIgFrameNumber = FrameNumber(igControlV3.lastIGFrameNumber);
  igControl.timestamp = igControlV3.timestamp;

  Handle(igControl);
}

ETopLevelCoordinateSystem ConvertCoordinateSystem33(CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem eCoordinateSystem)
{
  if (eCoordinateSystem == CIGI::V33::LineOfSightSegmentRequest::eCoordinateSystem_Geodetic)
  {
    return ETopLevelCoordinateSystem::GEODETIC;
  }
  else if (eCoordinateSystem == CIGI::V33::LineOfSightSegmentRequest::eCoordinateSystem_Entity)
  {
    return ETopLevelCoordinateSystem::ENTITY;
  }

  return ETopLevelCoordinateSystem::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseLineOfSightSegmentRequestPacket(uint8_t* pBuffer)
{
  CIGI::V33::LineOfSightSegmentRequest losRequest;
  memcpy(&losRequest, pBuffer, sizeof(CIGI::V33::LineOfSightSegmentRequest));

  if (m_bByteSwap)
  {
    losRequest.doByteSwapping();
  }

  ETopLevelCoordinateSystem eSourceCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.sourceCoordinateSystem));
  ETopLevelCoordinateSystem eDestinationCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.destinationCoordinateSystem));

  if (losRequest.requestType == CIGI::V33::LineOfSightSegmentRequest::Type::eType_Basic)
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
      data.sourceEntityID = EntityID(losRequest.entityId);
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
      data.sourceEntityID = EntityID(losRequest.entityId);
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
  else
  {
    ETopLevelCoordinateSystem eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));

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
      data.eResponseCoordinateSystem = eResponseCoordinateSystem;
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
      data.eResponseCoordinateSystem = eResponseCoordinateSystem;
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
    {
      SLineOfSightSegmentRequestEntityToGeodeticExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.entityId);
      data.sourceOffset[0] = losRequest.sourceLatitudeXOffset;
      data.sourceOffset[1] = losRequest.sourceLongitudeYOffset;
      data.sourceOffset[2] = losRequest.sourceAltitudeZOffset;
      data.destinationGeodeticCoordinates.latitude = Latitude(losRequest.destinationLatitudeXOffset);
      data.destinationGeodeticCoordinates.longitude = Longitude(losRequest.destinationLongitudeYOffset);
      data.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(losRequest.destinationAltitudeZOffset);
      data.nMaterialMask = losRequest.materialMask;
      data.updatePeriod = UpdatePeriod(losRequest.updatePeriod);
      data.lastHostFrameNumber = m_ImageGenerator.GetLastHostFrameNumber();
      data.eResponseCoordinateSystem = eResponseCoordinateSystem;
      m_pTerrainHandler->Handle(data);
    }
    else if (eSourceCoordinateSystem == ETopLevelCoordinateSystem::ENTITY && eDestinationCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightSegmentRequestEntityToEntityExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.entityId);
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
      data.eResponseCoordinateSystem = eResponseCoordinateSystem;
      m_pTerrainHandler->Handle(data);
    }
  }
}

ETopLevelCoordinateSystem ConvertCoordinateSystem33(CIGI::V33::LineOfSightVectorRequest::CoordinateSystem eCoordinateSystem)
{
  if (eCoordinateSystem == CIGI::V33::LineOfSightVectorRequest::eCoordinateSystem_Geodetic)
  {
    return ETopLevelCoordinateSystem::GEODETIC;
  }
  else if (eCoordinateSystem == CIGI::V33::LineOfSightVectorRequest::eCoordinateSystem_Entity)
  {
    return ETopLevelCoordinateSystem::ENTITY;
  }

  return ETopLevelCoordinateSystem::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseLineOfSightVectorRequestPacket(uint8_t* pBuffer)
{
  CIGI::V33::LineOfSightVectorRequest losRequest;
  memcpy(&losRequest, pBuffer, sizeof(CIGI::V33::LineOfSightVectorRequest));

  if (m_bByteSwap)
  {
    losRequest.doByteSwapping();
  }

  ETopLevelCoordinateSystem eSourcePointCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V33::LineOfSightVectorRequest::CoordinateSystem>(losRequest.sourceCoordinateSystem));

  if (losRequest.requestType == CIGI::V33::LineOfSightVectorRequest::Type::eType_Basic)
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
    ETopLevelCoordinateSystem eResponseCoordinateSystem = ConvertCoordinateSystem(static_cast<CIGI::V33::LineOfSightVectorRequest::CoordinateSystem>(losRequest.responseCoordinateSystem));

    if (eSourcePointCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
    {
      SLineOfSightVectorRequestEntityExtended data;
      data.requestID = LineOfSightRequestID(losRequest.LOSId);
      data.nAlphaThreshold = losRequest.alphaThreshold;
      data.sourceEntityID = EntityID(losRequest.entityId);
      data.eResponseCoordinateSystem = eResponseCoordinateSystem;
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
      data.eResponseCoordinateSystem = eResponseCoordinateSystem;
      m_pTerrainHandler->Handle(data);
    }
  }
}

ECigiScope ConvertScope33(CIGI::V33::MaritimeSurfaceConditionsCtrl::Scope eScope)
{
  switch (eScope)
  {
  case CIGI::V33::MaritimeSurfaceConditionsCtrl::Scope::eScope_Global:
    return ECigiScope::GLOBAL;
  case CIGI::V33::MaritimeSurfaceConditionsCtrl::Scope::eScope_Regional:
    return ECigiScope::REGIONAL;
  case CIGI::V33::MaritimeSurfaceConditionsCtrl::Scope::eScope_Entity:
    return ECigiScope::ENTITY;
  }

  return ECigiScope::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseMaritimeSurfaceConditionsControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::MaritimeSurfaceConditionsCtrl maritimeSurfaceConditionsControlPacketV3;
  memcpy(&maritimeSurfaceConditionsControlPacketV3, pBuffer, sizeof(CIGI::V33::MaritimeSurfaceConditionsCtrl));

  if (m_bByteSwap)
  {
    maritimeSurfaceConditionsControlPacketV3.doByteSwapping();
  }

  SCigiMaritimeSurfaceCondition maritimeSurface;
  maritimeSurface.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(maritimeSurfaceConditionsControlPacketV3.seaSurfaceHeight);
  maritimeSurface.fSurfaceWaterTemperature = TemperatureCelsius(maritimeSurfaceConditionsControlPacketV3.surfaceWaterTemperature);
  maritimeSurface.surfaceClarity = Percentage(maritimeSurfaceConditionsControlPacketV3.surfaceClarity);
  maritimeSurface.bWhitecapEnabled = maritimeSurfaceConditionsControlPacketV3.whitecapEnable;
  maritimeSurface.bActive = maritimeSurfaceConditionsControlPacketV3.surfaceConditionsEnable;

  ECigiScope eScope = ConvertScope33(static_cast<CIGI::V33::MaritimeSurfaceConditionsCtrl::Scope>(maritimeSurfaceConditionsControlPacketV3.scope));

  switch (eScope)
  {
  case ECigiScope::ENTITY:
    m_pMaritimeSurfaceConditionPacketHandler->HandleEntityMartiimeSurfaceCondition(maritimeSurface, EntityID(maritimeSurfaceConditionsControlPacketV3.entityRegionId));
    break;
  case ECigiScope::REGIONAL:
    m_pMaritimeSurfaceConditionPacketHandler->HandleRegionMaritimeSurfaceCondition(maritimeSurface, RegionID(maritimeSurfaceConditionsControlPacketV3.entityRegionId));
    break;
  case ECigiScope::GLOBAL:
    m_pMaritimeSurfaceConditionPacketHandler->HandleGlobalMaritimeSurfaceCondition(maritimeSurface);
    break;
  default:
    break;
  }
}

void CCigiPacketHandlerV3::ParseMotionTrackerControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::MotionTrackerCtrl motionTrackerV3;
  memcpy(&motionTrackerV3, pBuffer, sizeof(CIGI::V33::MotionTrackerCtrl));

  if (m_bByteSwap)
  {
    motionTrackerV3.doByteSwapping();
  }

  if (motionTrackerV3.viewViewGroupSelect == CIGI::V33::MotionTrackerCtrl::eAttachment_View)
  {
    SMotionTrackerViewControl motionTrackerControl;
    motionTrackerControl.bBoresightEnable = motionTrackerV3.boresightEnable;
    motionTrackerControl.bEnable = motionTrackerV3.trackerEnable;
    motionTrackerControl.bPitchEnable = motionTrackerV3.pitchEnable;
    motionTrackerControl.bRollEnable = motionTrackerV3.rollEnable;
    motionTrackerControl.bXEnable = motionTrackerV3.xEnable;
    motionTrackerControl.bYawEnable = motionTrackerV3.yawEnable;
    motionTrackerControl.bYEnable = motionTrackerV3.yEnable;
    motionTrackerControl.bZEnable = motionTrackerV3.zEnable;
    motionTrackerControl.motionTrackerID = MotionTrackerID(motionTrackerV3.trackerId);
    motionTrackerControl.viewID = ViewID(static_cast<uint8_t>(motionTrackerV3.viewViewGroupId));
    m_pMotionTrackerControlHandler->Handle(motionTrackerControl);
  }
  else if (motionTrackerV3.viewViewGroupSelect == CIGI::V33::MotionTrackerCtrl::eAttachment_Group)
  {
    SMotionTrackerViewGroupControl motionTrackerControl;
    motionTrackerControl.bBoresightEnable = motionTrackerV3.boresightEnable;
    motionTrackerControl.bEnable = motionTrackerV3.trackerEnable;
    motionTrackerControl.bPitchEnable = motionTrackerV3.pitchEnable;
    motionTrackerControl.bRollEnable = motionTrackerV3.rollEnable;
    motionTrackerControl.bXEnable = motionTrackerV3.xEnable;
    motionTrackerControl.bYawEnable = motionTrackerV3.yawEnable;
    motionTrackerControl.bYEnable = motionTrackerV3.yEnable;
    motionTrackerControl.bZEnable = motionTrackerV3.zEnable;
    motionTrackerControl.motionTrackerID = MotionTrackerID(motionTrackerV3.trackerId);
    motionTrackerControl.viewGroupID = ViewGroupID(static_cast<uint8_t>(motionTrackerV3.viewViewGroupId));
    m_pMotionTrackerControlHandler->Handle(motionTrackerControl);
  }
}

EObjectCoordinateSystem ConvertCoordinateSystem33(CIGI::V33::PositionRequest::CoordinateSystem eCoordSysGroup)
{
  switch (eCoordSysGroup)
  {
  case CIGI::V33::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
    return EObjectCoordinateSystem::WORLD;
  case CIGI::V33::PositionRequest::CoordinateSystem::eCoordinateSystem_ParentEntity:
    return EObjectCoordinateSystem::PARENT;
    break;
  case CIGI::V33::PositionRequest::CoordinateSystem::eCoordinateSystem_Submodel:
    return EObjectCoordinateSystem::LOCAL;
    break;
  }

  return EObjectCoordinateSystem::UNKNOWN;
}

EObjectClass ConvertObjectClass33(CIGI::V33::PositionRequest::ObjectClass eObjectClass)
{
  switch (eObjectClass)
  {
  case CIGI::V33::PositionRequest::ObjectClass::eObjectClass_ArticulatedPart:
    return EObjectClass::ARTICULATED_PART;
  case CIGI::V33::PositionRequest::ObjectClass::eObjectClass_Entity:
    return EObjectClass::ENTITY;
  case CIGI::V33::PositionRequest::ObjectClass::eObjectClass_MotionTracker:
    return EObjectClass::MOTION_TRACKER;
  case CIGI::V33::PositionRequest::ObjectClass::eObjectClass_View:
    return EObjectClass::VIEW;
  case CIGI::V33::PositionRequest::ObjectClass::eObjectClass_ViewGroup:
    return EObjectClass::VIEW_GROUP;
  }

  return EObjectClass::UNKNOWN;
}

void CCigiPacketHandlerV3::ParsePositionRequestPacket(uint8_t* pBuffer)
{
  CIGI::V33::PositionRequest positionRequestV3;
  memcpy(&positionRequestV3, pBuffer, sizeof(CIGI::V33::PositionRequest));

  if (m_bByteSwap)
  {
    positionRequestV3.doByteSwapping();
  }

  SPositionRequest request;
  request.articulatedPartID = ArticulatedPartID(positionRequestV3.articulatedPartId);
  request.eCoordinateSystem = ConvertCoordinateSystem33(static_cast<CIGI::V33::PositionRequest::CoordinateSystem>(positionRequestV3.coordinateSystem));
  request.eObjectClass = ConvertObjectClass33(static_cast<CIGI::V33::PositionRequest::ObjectClass>(positionRequestV3.objectClass));
  request.bContinuous = positionRequestV3.updateMode == CIGI::V33::PositionRequest::eUpdateMode_Continuous;
  request.nObjectID = positionRequestV3.objectId;

  m_pEntityControlHandler->HandleCigiPositionRequest(request);
}

ETrackMode ConvertTrackMode33(CIGI::V33::SensorCtrl::TrackMode eTrackMode)
{
  if (eTrackMode == CIGI::V33::SensorCtrl::TrackMode::eTrackMode_Off)
  {
    return ETrackMode::OFF;
  }
  else if (eTrackMode == CIGI::V33::SensorCtrl::TrackMode::eTrackMode_ForceCorrelate)
  {
    return ETrackMode::FORCE_CORRELATE;
  }
  else if (eTrackMode == CIGI::V33::SensorCtrl::TrackMode::eTrackMode_Scene)
  {
    return ETrackMode::SCENE;
  }
  else if (eTrackMode == CIGI::V33::SensorCtrl::TrackMode::eTrackMode_Target)
  {
    return ETrackMode::TARGET;
  }
  else if (eTrackMode == CIGI::V33::SensorCtrl::TrackMode::eTrackMode_Ship)
  {
    return ETrackMode::SHIP;
  }
  else
  {
    return ETrackMode::UNKNOWN;
  }
}

void CCigiPacketHandlerV3::ParseSensorControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::SensorCtrl sensorCtrlV3;
  memcpy(&sensorCtrlV3, pBuffer, sizeof(CIGI::V33::SensorCtrl));

  if (m_bByteSwap)
  {
    sensorCtrlV3.doByteSwapping();
  }

  SCigiSensorControl sensorControl;
  sensorControl.sensorID = SensorID(sensorCtrlV3.sensorId);
  sensorControl.bAutomaticGain = sensorCtrlV3.automaticGain;
  sensorControl.eTrackMode = ConvertTrackMode33(static_cast<CIGI::V33::SensorCtrl::TrackMode>(sensorCtrlV3.trackMode));

  if (sensorCtrlV3.polarity == CIGI::V33::SensorCtrl::Polarity::ePolarity_BlackHot)
  {
    sensorControl.ePolarity = EPolarity::BLACK_HOT;
  }
  else if (sensorCtrlV3.polarity == CIGI::V33::SensorCtrl::Polarity::ePolarity_WhiteHot)
  {
    sensorControl.ePolarity = EPolarity::WHITE_HOT;
  }
  else
  {
    sensorControl.ePolarity = EPolarity::UNKNOWN;
  }

  sensorControl.bSensorOn = sensorCtrlV3.sensorOnOff == 1;
  sensorControl.bLineByLineDropoutEnabled = sensorCtrlV3.lineByLineDropoutEnable;
  sensorControl.bAutomaticGain = sensorCtrlV3.automaticGain;

  if (sensorCtrlV3.trackWhiteBlack == CIGI::V33::SensorCtrl::TrackWhiteBlack::eTrackWhiteBlack_Black)
  {
    sensorControl.eSensorTrack = ESensorTrack::BLACK;
  }
  else if (sensorCtrlV3.trackWhiteBlack == CIGI::V33::SensorCtrl::TrackWhiteBlack::eTrackWhiteBlack_White)
  {
    sensorControl.eSensorTrack = ESensorTrack::WHITE;
  }
  else
  {
    sensorControl.eSensorTrack = ESensorTrack::UNKNOWN;
  }

  if (sensorCtrlV3.responseType == CIGI::V33::SensorCtrl::ResponseType::eResponseType_Extended)
  {
    sensorControl.bExtendedResponse = true;
  }
  else
  {
    sensorControl.bExtendedResponse = false;
  }

  sensorControl.viewID = ViewID(sensorCtrlV3.viewId);
  sensorControl.gain = SensorGain(sensorCtrlV3.gain);
  sensorControl.level = SensorLevel(sensorCtrlV3.level);
  sensorControl.fACCoupling = sensorCtrlV3.acCoupling;
  sensorControl.noise = SensorNoise(sensorCtrlV3.noise);

  if (sensorControl.gain.CheckValid())
  {
    m_pSensorControlHandler->Handle(sensorControl);
  }
}

EDegreeOfFreedom ConvertDegreeOfFreedom33(CIGI::V33::ShortArticulatedPartCtrl::DoF eDOF)
{
  switch (eDOF)
  {
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_NotUsed:
    return EDegreeOfFreedom::NOT_USED;
    break;
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_XOffset:
    return EDegreeOfFreedom::X_OFFSET;
    break;
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_YOffset:
    return EDegreeOfFreedom::Y_OFFSET;
    break;
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_ZOffset:
    return EDegreeOfFreedom::Z_OFFSET;
    break;
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_Yaw:
    return EDegreeOfFreedom::YAW;
    break;
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_Pitch:
    return EDegreeOfFreedom::PITCH;
    break;
  case CIGI::V33::ShortArticulatedPartCtrl::DoF::eDoF_Roll:
    return EDegreeOfFreedom::ROLL;
    break;
  }

  return EDegreeOfFreedom::NOT_USED;
}

void CCigiPacketHandlerV3::ParseShortArticulatedPartControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ShortArticulatedPartCtrl shortArticulatedPartControlV3;
  memcpy(&shortArticulatedPartControlV3, pBuffer, sizeof(CIGI::V33::ShortArticulatedPartCtrl));

  if (m_bByteSwap)
  {
    shortArticulatedPartControlV3.doByteSwapping();
  }

  // verify entity exists
  auto entityID = EntityID(shortArticulatedPartControlV3.entityId);
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entityID))
  {
    return;
  }

  SCigiShortArticulatedPart shortArticulatedPart;
  shortArticulatedPart.entityID = entityID;
  shortArticulatedPart.articulatedPartID1 = ArticulatedPartID(shortArticulatedPartControlV3.articulatedPartId1);
  shortArticulatedPart.articulatedPartID2 = ArticulatedPartID(shortArticulatedPartControlV3.articulatedPartId2);
  shortArticulatedPart.eDOF1 = ConvertDegreeOfFreedom33(static_cast<CIGI::V33::ShortArticulatedPartCtrl::DoF>(shortArticulatedPartControlV3.dofSelect1));
  shortArticulatedPart.eDOF2 = ConvertDegreeOfFreedom33(static_cast<CIGI::V33::ShortArticulatedPartCtrl::DoF>(shortArticulatedPartControlV3.dofSelect2));
  shortArticulatedPart.fDOF1 = shortArticulatedPartControlV3.dof1;
  shortArticulatedPart.fDOF2 = shortArticulatedPartControlV3.dof2;
  shortArticulatedPart.bArticulatedPart1Enabled = shortArticulatedPartControlV3.articulatedPartEnable1;
  shortArticulatedPart.bArticulatedPart2Enabled = shortArticulatedPartControlV3.articulatedPartEnable2;
  m_pArticulatedPartHandler->Handle(shortArticulatedPart);
}

void CCigiPacketHandlerV3::ParseShortComponentControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ShortComponentCtrl shortComponentControlV3;
  memcpy(&shortComponentControlV3, pBuffer, sizeof(CIGI::V33::ShortComponentCtrl));

  if (m_bByteSwap)
  {
    shortComponentControlV3.doByteSwapping();
  }

  SCigiComponentControl componentControl;
  componentControl.key.componentID = CigiComponentID(shortComponentControlV3.componentId);
  componentControl.key.componentClassID = CigiComponentClassID(static_cast<uint8_t>(shortComponentControlV3.componentClass));
  componentControl.key.nInstanceID = shortComponentControlV3.instanceId;
  componentControl.state.nComponentState = shortComponentControlV3.componentState;
  componentControl.state.componentData[0] = shortComponentControlV3.componentData[0];
  componentControl.state.componentData[1] = shortComponentControlV3.componentData[1];

  CShortComponentDataParser<CIGI::V33::ShortComponentCtrl> componentDataParser(shortComponentControlV3);
  m_pComponentControlHandler->Handle(componentControl, &componentDataParser);
}

EAttachState ConvertAttachState33(CIGI::V33::SymbolCtrl::AttachState eAttachState)
{
  switch (eAttachState)
  {
  case CIGI::V33::SymbolCtrl::AttachState::eAttachState_Attach:
    return EAttachState::ATTACH;
  case CIGI::V33::SymbolCtrl::AttachState::eAttachState_Detach:
    return EAttachState::DETACH;
  }
  return EAttachState::UNKNOWN;
}

EFlashControl ConvertToFlashControl33(CIGI::V33::SymbolCtrl::FlashControl eFlashControl)
{
  if (eFlashControl == CIGI::V33::SymbolCtrl::FlashControl::eFlashControl_Continue)
  {
    return EFlashControl::CONTINUE;
  }
  else if (eFlashControl == CIGI::V33::SymbolCtrl::FlashControl::eFlashControl_Reset)
  {
    return EFlashControl::RESET;
  }

  return EFlashControl::UNKNOWN;
}

ESymbolState ConvertSymbolState33(CIGI::V33::SymbolCtrl::SymbolState eSymbolState)
{
  switch (eSymbolState)
  {
  case CIGI::V33::SymbolCtrl::SymbolState::eSymbolState_Destroyed:
    return ESymbolState::DESTROYED;
  case CIGI::V33::SymbolCtrl::SymbolState::eSymbolState_Hidden:
    return ESymbolState::HIDDEN;
  case CIGI::V33::SymbolCtrl::SymbolState::eSymbolState_Visible:
    return ESymbolState::VISIBLE;
  }

  return ESymbolState::UNKNOWN;
}

void ParseAttributeValue(SSymbolControl& symbolControl, CIGI::V33::ShortSymbolCtrl::AttributeSelect eAttributeSelect, CIGI::ui32 attributeValue)
{
  switch (eAttributeSelect)
  {
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_None:
  {
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_SurfaceID:
  {
    symbolControl.surfaceID = SymbolSurfaceID(static_cast<uint16_t>(attributeValue));
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ParentSymbolID:
  {
    symbolControl.parentSymbolID = SymbolID(static_cast<uint16_t>(attributeValue));
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Layer:
  {
    symbolControl.nLayerID = static_cast<uint8_t>(attributeValue);
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashDutyCyclePercentage:
  {
    symbolControl.flashDutyCyclePercentage = ConvertToPercentage(CigiIntPercentage(static_cast<uint8_t>(attributeValue)));
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashPeriod:
  {
    memcpy(&symbolControl.fFlashPeriod, &attributeValue, sizeof(uint32_t));
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionU:
  {
    memcpy(&symbolControl.fPositionU, &attributeValue, sizeof(uint32_t));
    symbolControl.bPositionSet = true;
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionV:
  {
    memcpy(&symbolControl.fPositionV, &attributeValue, sizeof(uint32_t));
    symbolControl.bPositionSet = true;
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Rotation:
  {
    float f = 0;
    memcpy(&f, &attributeValue, sizeof(uint32_t));
    symbolControl.fRotation = Degrees(f);
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Color:
  {
    symbolControl.color.r = (attributeValue & 0xF000) >> 12;
    symbolControl.color.b = (attributeValue & 0x0F00) >> 8;
    symbolControl.color.g = (attributeValue & 0x00F0) >> 4;
    symbolControl.color.a = attributeValue & 0x000F;

    symbolControl.bSetColor = true;
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleU:
  {
    memcpy(&symbolControl.fScaleU, &attributeValue, sizeof(uint32_t));
    symbolControl.bScaleSet = true;
    break;
  }
  case CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleV:
  {
    memcpy(&symbolControl.fScaleV, &attributeValue, sizeof(uint32_t));
    symbolControl.bScaleSet = true;
    break;
  }
  }
}

void CCigiPacketHandlerV3::ParseShortSymbolControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ShortSymbolCtrl symbolControlV3;
  memcpy(&symbolControlV3, pBuffer, sizeof(CIGI::V33::ShortSymbolCtrl));

  if (m_bByteSwap)
  {
    symbolControlV3.doByteSwapping();
  }

  SSymbolControl symbolControl;
  symbolControl.symbolID = SymbolID(symbolControlV3.symbolId);
  symbolControl.eAttachState = ConvertAttachState33(static_cast<CIGI::V33::SymbolCtrl::AttachState>(symbolControlV3.attachState));
  symbolControl.bInheritColor = symbolControlV3.inheritColor == CIGI::V33::ShortSymbolCtrl::InheritColor::eInheritColor_Inherited;
  symbolControl.eFlashControl = ConvertToFlashControl33(static_cast<CIGI::V33::SymbolCtrl::FlashControl>(symbolControlV3.flashControl));
  symbolControl.eSymbolState = ConvertSymbolState33(static_cast<CIGI::V33::SymbolCtrl::SymbolState>(symbolControlV3.symbolState));
  symbolControl.bScaleSet = false;
  symbolControl.bPositionSet = false;
  symbolControl.bSetColor = false;

  ParseAttributeValue(symbolControl, static_cast<CIGI::V33::ShortSymbolCtrl::AttributeSelect>(symbolControlV3.attributeSelect1), symbolControlV3.attributeValue1);
  ParseAttributeValue(symbolControl, static_cast<CIGI::V33::ShortSymbolCtrl::AttributeSelect>(symbolControlV3.attributeSelect2), symbolControlV3.attributeValue2);
  m_pSymbolHandler->Handle(symbolControl);
}

sbio::symbol::EDrawingStyle ConvertDrawingStyle33(CIGI::V33::SymbolCircleDefinition::DrawingStyle drawingStyle)
{
  if (drawingStyle == CIGI::V33::SymbolCircleDefinition::DrawingStyle::eDrawingStyle_Line)
  {
    return EDrawingStyle::LINE;
  }
  else if (drawingStyle == CIGI::V33::SymbolCircleDefinition::DrawingStyle::eDrawingStyle_Fill)
  {
    return EDrawingStyle::FILL;
  }

  return EDrawingStyle::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseSymbolCircleDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::SymbolCircleDefinition symbolCircleDefV3;
  memcpy(&symbolCircleDefV3, pBuffer, sizeof(CIGI::V33::SymbolCircleDefinition));

  if (m_bByteSwap)
  {
    symbolCircleDefV3.doByteSwapping();
  }

  SSymbolCircle symbolCircle;
  symbolCircle.symbolID = SymbolID(symbolCircleDefV3.symbolId);
  symbolCircle.eDrawingStyle = ConvertDrawingStyle33(static_cast<CIGI::V33::SymbolCircleDefinition::DrawingStyle>(symbolCircleDefV3.drawingStyle));
  symbolCircle.fLineWidth = symbolCircleDefV3.lineWidth;
  symbolCircle.stipplePattern = symbolCircleDefV3.stipplePattern;
  symbolCircle.fStipplePatternLength = symbolCircleDefV3.stipplePatternLength;

  CIGI::VariableRecordHelper<CIGI::V33::SymbolCircleDefinition> helper(symbolCircleDefV3);

  int numRecords = helper.numRecords(symbolCircleDefV3);
  for (int n = 0; n < numRecords; ++n)
  {
    CIGI::V33::SymbolCircleDefinition::Circle circle = symbolCircleDefV3.records[n];

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

sbio::symbol::ESymbolSourceType ConvertSymbolSourceType33(CIGI::V33::SymbolClone::SourceType eSymbolSource)
{
  if (eSymbolSource == CIGI::V33::SymbolClone::SourceType::eSourceType_Symbol)
  {
    return ESymbolSourceType::SYMBOL;
  }
  else if (eSymbolSource == CIGI::V33::SymbolClone::SourceType::eSourceType_SymbolTemplate)
  {
    return ESymbolSourceType::SYMBOL_TEMPLATE;
  }

  return ESymbolSourceType::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseSymbolClonePacket(uint8_t* pBuffer)
{
  CIGI::V33::SymbolClone symbolCloneV3;
  memcpy(&symbolCloneV3, pBuffer, sizeof(CIGI::V33::SymbolClone));

  if (m_bByteSwap)
  {
    symbolCloneV3.doByteSwapping();
  }

  SSymbolClone symbolClone;
  symbolClone.eSymbolSourceType = ConvertSymbolSourceType33(static_cast<CIGI::V33::SymbolClone::SourceType>(symbolCloneV3.sourceType));
  symbolClone.sourceID = SymbolID(symbolCloneV3.sourceId);
  symbolClone.symbolID = SymbolID(symbolCloneV3.symbolId);
  m_pSymbolHandler->Handle(symbolClone);
}

void CCigiPacketHandlerV3::ParseSymbolControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::SymbolCtrl symbolControlV3;
  memcpy(&symbolControlV3, pBuffer, sizeof(CIGI::V33::SymbolCtrl));

  if (m_bByteSwap)
  {
    symbolControlV3.doByteSwapping();
  }

  SSymbolControl symbolControl;
  symbolControl.eAttachState = ConvertAttachState33(static_cast<CIGI::V33::SymbolCtrl::AttachState>(symbolControlV3.attachState));
  symbolControl.bInheritColor = symbolControlV3.inheritColor == CIGI::V33::SymbolCtrl::InheritColor::eInheritColor_Inherited;
  symbolControl.eFlashControl = ConvertToFlashControl33(static_cast<CIGI::V33::SymbolCtrl::FlashControl>(symbolControlV3.flashControl));
  symbolControl.eSymbolState = ConvertSymbolState33(static_cast<CIGI::V33::SymbolCtrl::SymbolState>(symbolControlV3.symbolState));
  symbolControl.flashDutyCyclePercentage = Percentage(symbolControlV3.flashDutyCyclePercentage / 100.0f);
  symbolControl.fFlashPeriod = symbolControlV3.flashPeriod;
  symbolControl.fRotation = Degrees(symbolControlV3.rotation);
  symbolControl.fScaleU = symbolControlV3.scaleU;
  symbolControl.fScaleV = symbolControlV3.scaleV;
  symbolControl.nLayerID = symbolControlV3.layer;
  symbolControl.parentSymbolID = SymbolID(symbolControlV3.parentSymbolId);
  symbolControl.fPositionU = symbolControlV3.positionU;
  symbolControl.fPositionV = symbolControlV3.positionV;
  symbolControl.surfaceID = SymbolSurfaceID(symbolControlV3.surfaceId);
  symbolControl.symbolID = SymbolID(symbolControlV3.symbolId);
  symbolControl.color.r = symbolControlV3.red;
  symbolControl.color.g = symbolControlV3.green;
  symbolControl.color.b = symbolControlV3.blue;
  symbolControl.color.a = symbolControlV3.alpha;
  m_pSymbolHandler->Handle(symbolControl);
}

sbio::symbol::EPrimitiveType ConvertPrimitiveGrp33(CIGI::V33::SymbolLineDefinition::PrimitiveType ePrimitiveGrp)
{
  switch (ePrimitiveGrp)
  {
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_Point:
    return EPrimitiveType::POINT;
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_Line:
    return EPrimitiveType::LINE;
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_LineStrip:
    return EPrimitiveType::LINE_STRIP;
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_LineLoop:
    return EPrimitiveType::LINE_LOOP;
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_Triangle:
    return EPrimitiveType::TRIANGLE;
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_TriangleStrip:
    return EPrimitiveType::TRIANGLE_STRIP;
  case CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_TriangleFan:
    return EPrimitiveType::TRIANGLE_FAN;
  default:
    return EPrimitiveType::UNKNOWN;
  }
}

void CCigiPacketHandlerV3::ParseSymbolLineDefinitonPacket(uint8_t* pBuffer)
{
  CIGI::V33::SymbolLineDefinition symbolLineDefinitionV3;
  memcpy(&symbolLineDefinitionV3, pBuffer, sizeof(CIGI::V33::SymbolLineDefinition));

  if (m_bByteSwap)
  {
    symbolLineDefinitionV3.doByteSwapping();
  }

  SSymbolPolygon symbolPolygon;
  symbolPolygon.symbolID = SymbolID(symbolLineDefinitionV3.symbolId);
  symbolPolygon.ePrimitiveType = ConvertPrimitiveGrp33(static_cast<CIGI::V33::SymbolLineDefinition::PrimitiveType>(symbolLineDefinitionV3.primitiveType));
  symbolPolygon.fLineWidth = symbolLineDefinitionV3.lineWidth;
  symbolPolygon.fStipplePatternLength = symbolLineDefinitionV3.stipplePatternLength;
  symbolPolygon.nStipplePattern = symbolLineDefinitionV3.stipplePattern;

  CIGI::VariableRecordHelper<CIGI::V33::SymbolLineDefinition> helper(symbolLineDefinitionV3);
  int nNumVertices = helper.numRecords(symbolLineDefinitionV3);
  for (int n = 0; n < nNumVertices; ++n)
  {
    Vec2f v;
    v[0] = symbolLineDefinitionV3.records[n].vertexU;
    v[1] = symbolLineDefinitionV3.records[n].vertexV;

    symbolPolygon.vertices.push_back(v);
  }
  m_pSymbolHandler->Handle(symbolPolygon);
}

ESurfaceState ConvertToSurfaceState33(CIGI::V33::SymbolSurfaceDefinition::SurfaceState surfaceState)
{
  if (surfaceState == CIGI::V33::SymbolSurfaceDefinition::SurfaceState::eSurfaceState_Active)
  {
    return ESurfaceState::ACTIVE;
  }
  else if (surfaceState == CIGI::V33::SymbolSurfaceDefinition::SurfaceState::eSurfaceState_Destroyed)
  {
    return ESurfaceState::DESTROYED;
  }

  return ESurfaceState::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseSymbolSurfaceDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::SymbolSurfaceDefinition symbolSurfaceDefV3;
  memcpy(&symbolSurfaceDefV3, pBuffer, sizeof(CIGI::V33::SymbolSurfaceDefinition));

  if (m_bByteSwap)
  {
    symbolSurfaceDefV3.doByteSwapping();
  }

  if (symbolSurfaceDefV3.attachType == CIGI::V33::SymbolSurfaceDefinition::AttachType::eAttachType_Entity)
  {
    if (symbolSurfaceDefV3.billboard == CIGI::V33::SymbolSurfaceDefinition::Billboard::eBillboard_NonBillboard)
    {
      SEntitySymbolSurfaceDefinition entitySymbolSurfaceDefinition;
      entitySymbolSurfaceDefinition.surfaceID = SymbolSurfaceID(symbolSurfaceDefV3.surfaceId);
      entitySymbolSurfaceDefinition.eSurfaceState = ConvertToSurfaceState33(static_cast<CIGI::V33::SymbolSurfaceDefinition::SurfaceState>(symbolSurfaceDefV3.surfaceState));
      entitySymbolSurfaceDefinition.uvMin[0] = symbolSurfaceDefV3.minU;
      entitySymbolSurfaceDefinition.uvMin[1] = symbolSurfaceDefV3.minV;
      entitySymbolSurfaceDefinition.uvMax[0] = symbolSurfaceDefV3.maxU;
      entitySymbolSurfaceDefinition.uvMax[1] = symbolSurfaceDefV3.maxV;
      entitySymbolSurfaceDefinition.entityID = EntityID(symbolSurfaceDefV3.entityIdViewId);
      entitySymbolSurfaceDefinition.offsetToSurface[0] = symbolSurfaceDefV3.xOffsetLeft;
      entitySymbolSurfaceDefinition.offsetToSurface[1] = symbolSurfaceDefV3.yOffsetRight;
      entitySymbolSurfaceDefinition.offsetToSurface[2] = symbolSurfaceDefV3.zOffsetTop;
      entitySymbolSurfaceDefinition.yaw = Degrees(symbolSurfaceDefV3.yawBottom);
      entitySymbolSurfaceDefinition.pitch = Degrees90(symbolSurfaceDefV3.pitch);
      entitySymbolSurfaceDefinition.roll = Degrees180(symbolSurfaceDefV3.roll);
      entitySymbolSurfaceDefinition.width = symbolSurfaceDefV3.width;
      entitySymbolSurfaceDefinition.height = symbolSurfaceDefV3.height;
      m_pSymbolHandler->Handle(entitySymbolSurfaceDefinition);
    }
    else if (symbolSurfaceDefV3.billboard == CIGI::V33::SymbolSurfaceDefinition::Billboard::eBillboard_Billboard)
    {
      SEntityBillboardSymbolSurfaceDefinition entityBillboardSurfaceDefinition;
      entityBillboardSurfaceDefinition.surfaceID = SymbolSurfaceID(symbolSurfaceDefV3.surfaceId);
      entityBillboardSurfaceDefinition.eSurfaceState = ConvertToSurfaceState33(static_cast<CIGI::V33::SymbolSurfaceDefinition::SurfaceState>(symbolSurfaceDefV3.surfaceState));
      entityBillboardSurfaceDefinition.uvMin[0] = symbolSurfaceDefV3.minU;
      entityBillboardSurfaceDefinition.uvMin[1] = symbolSurfaceDefV3.minV;
      entityBillboardSurfaceDefinition.uvMax[0] = symbolSurfaceDefV3.maxU;
      entityBillboardSurfaceDefinition.uvMax[1] = symbolSurfaceDefV3.maxV;
      entityBillboardSurfaceDefinition.entityID = EntityID(symbolSurfaceDefV3.entityIdViewId);
      entityBillboardSurfaceDefinition.bPerspectiveGrowthEnabled = symbolSurfaceDefV3.perspectiveGrowthEnable;
      entityBillboardSurfaceDefinition.offsetToEntity[0] = symbolSurfaceDefV3.xOffsetLeft;
      entityBillboardSurfaceDefinition.offsetToEntity[1] = symbolSurfaceDefV3.yOffsetRight;
      entityBillboardSurfaceDefinition.offsetToEntity[2] = symbolSurfaceDefV3.zOffsetTop;
      entityBillboardSurfaceDefinition.width = symbolSurfaceDefV3.width;
      entityBillboardSurfaceDefinition.height = symbolSurfaceDefV3.height;
      m_pSymbolHandler->Handle(entityBillboardSurfaceDefinition);
    }
  }
  else if (symbolSurfaceDefV3.attachType == CIGI::V33::SymbolSurfaceDefinition::AttachType::eAttachType_View)
  {
    SViewSymbolSurfaceDefinition viewSymbolSurfaceDefinition;
    viewSymbolSurfaceDefinition.surfaceID = SymbolSurfaceID(symbolSurfaceDefV3.surfaceId);
    viewSymbolSurfaceDefinition.eSurfaceState = ConvertToSurfaceState33(static_cast<CIGI::V33::SymbolSurfaceDefinition::SurfaceState>(symbolSurfaceDefV3.surfaceState));
    viewSymbolSurfaceDefinition.uvMin[0] = symbolSurfaceDefV3.minU;
    viewSymbolSurfaceDefinition.uvMin[1] = symbolSurfaceDefV3.minV;
    viewSymbolSurfaceDefinition.uvMax[0] = symbolSurfaceDefV3.maxU;
    viewSymbolSurfaceDefinition.uvMax[1] = symbolSurfaceDefV3.maxV;
    viewSymbolSurfaceDefinition.viewID = ViewID(symbolSurfaceDefV3.entityIdViewId);
    viewSymbolSurfaceDefinition.fLeft = symbolSurfaceDefV3.xOffsetLeft;
    viewSymbolSurfaceDefinition.fRight = symbolSurfaceDefV3.yOffsetRight;
    viewSymbolSurfaceDefinition.fTop = symbolSurfaceDefV3.zOffsetTop;
    viewSymbolSurfaceDefinition.fBottom = symbolSurfaceDefV3.yawBottom;
    m_pSymbolHandler->Handle(viewSymbolSurfaceDefinition);
  }
}

ETextAlignment ConvertTextAlignment33(CIGI::V33::SymbolTextDefinition::Alignment eAlignment)
{
  switch (eAlignment)
  {
  default:
    return ETextAlignment::UNKNOWN;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_BottomCenter:
    return ETextAlignment::BOTTOM_CENTER;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_BottomLeft:
    return ETextAlignment::BOTTOM_LEFT;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_BottomRight:
    return ETextAlignment::BOTTOM_RIGHT;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_Center:
    return ETextAlignment::CENTER;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_CenterLeft:
    return ETextAlignment::CENTER_LEFT;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_CenterRight:
    return ETextAlignment::CENTER_RIGHT;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_TopCenter:
    return ETextAlignment::TOP_CENTER;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_TopLeft:
    return ETextAlignment::TOP_LEFT;
    break;
  case CIGI::V33::SymbolTextDefinition::Alignment::eAlignment_TopRight:
    return ETextAlignment::TOP_RIGHT;
    break;
  }
}

ETextOrientation ConvertTextOrientation33(CIGI::V33::SymbolTextDefinition::Orientation eOrientation)
{
  switch (eOrientation)
  {
  case CIGI::V33::SymbolTextDefinition::Orientation::eOrientation_BottomToTop:
    return ETextOrientation::BOTTOM_TO_TOP;
    break;
  case CIGI::V33::SymbolTextDefinition::Orientation::eOrientation_LeftToRight:
    return ETextOrientation::LEFT_TO_RIGHT;
    break;
  case CIGI::V33::SymbolTextDefinition::Orientation::eOrientation_RightToLeft:
    return ETextOrientation::RIGHT_TO_LEFT;
    break;
  case CIGI::V33::SymbolTextDefinition::Orientation::eOrientation_TopToBottom:
    return ETextOrientation::TOP_TO_BOTTOM;
    break;
  }

  return ETextOrientation::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseSymbolTextDefinitionPacket(uint8_t* pBuffer)
{
  SCigiPacketHeaderV3 packetHeader;
  memcpy(&packetHeader, pBuffer, sizeof(SCigiPacketHeaderV3));

  uint16_t nPacketSize = packetHeader.nPacketSize;
  if (nPacketSize < CIGI::V33::SymbolTextDefinition::kBasePacketSize)
  {
    stringstream ss;
    ss << "Skipping invalid Symbol Text Definition packet: size " << nPacketSize << " is smaller than base size " << CIGI::V33::SymbolTextDefinition::kBasePacketSize << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  size_t nTextSize = static_cast<size_t>(nPacketSize - CIGI::V33::SymbolTextDefinition::kBasePacketSize);
  if (nTextSize > CIGI::V33::SymbolTextDefinition::kMaxDataLength)
  {
    stringstream ss;
    ss << "Skipping invalid Symbol Text Definition packet payload length " << nTextSize << " (max " << CIGI::V33::SymbolTextDefinition::kMaxDataLength << ")" << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  CIGI::V33::SymbolTextDefinition symbolTextDefV3 = {};
  memcpy(&symbolTextDefV3, pBuffer, CIGI::V33::SymbolTextDefinition::kBasePacketSize);

  if (m_bByteSwap)
  {
    symbolTextDefV3.doByteSwapping();
    CEndian::Swap(symbolTextDefV3.fontSize);
  }

  const char* pTextBegin = reinterpret_cast<const char*>(pBuffer + CIGI::V33::SymbolTextDefinition::kBasePacketSize);
  const char* pTextEnd = pTextBegin + nTextSize;
  const char* pTextTerminator = std::find(pTextBegin, pTextEnd, '\0');

  SSymbolTextDefinition symbolTextDefinition;
  symbolTextDefinition.symbolID = SymbolID(symbolTextDefV3.symbolId);
  symbolTextDefinition.eTextAlignment = ConvertTextAlignment33(static_cast<CIGI::V33::SymbolTextDefinition::Alignment>(symbolTextDefV3.alignment));
  symbolTextDefinition.eTextOrientation = ConvertTextOrientation33(static_cast<CIGI::V33::SymbolTextDefinition::Orientation>(symbolTextDefV3.orientation));
  symbolTextDefinition.fFontSize = symbolTextDefV3.fontSize;
  symbolTextDefinition.fontID = FontID(symbolTextDefV3.fontId);
  symbolTextDefinition.sText.assign(pTextBegin, pTextTerminator);
  m_pSymbolHandler->Handle(symbolTextDefinition);
}

ECigiScope ConvertScope33(CIGI::V33::TerrestrialSurfaceConditionsCtrl::Scope eScope)
{
  switch (eScope)
  {
  case CIGI::V33::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Global:
    return ECigiScope::GLOBAL;
  case CIGI::V33::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Regional:
    return ECigiScope::REGIONAL;
  case CIGI::V33::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Entity:
    return ECigiScope::ENTITY;
  }

  return ECigiScope::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseTerrestrialSurfaceConditionsControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceControlV3;
  memcpy(&terrestrialSurfaceControlV3, pBuffer, sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl));

  if (m_bByteSwap)
  {
    terrestrialSurfaceControlV3.doByteSwapping();
  }

  SCigiTerrestrialSurfaceCondition condition;
  condition.surfaceConditionID = SurfaceConditionID(terrestrialSurfaceControlV3.surfaceConditionId);
  condition.coverage = Percentage(terrestrialSurfaceControlV3.coverage);
  CigiTerrestrialSurfaceSeverity severity = CigiTerrestrialSurfaceSeverity(terrestrialSurfaceControlV3.severity);
  condition.severity = ConvertToPercentage(severity);
  condition.bEnabled = terrestrialSurfaceControlV3.surfaceConditionEnable;

  ECigiScope eScope = ConvertScope33(static_cast<CIGI::V33::TerrestrialSurfaceConditionsCtrl::Scope>(terrestrialSurfaceControlV3.scope));
  switch (eScope)
  {
  case ECigiScope::ENTITY:
  {
    EntityID entityID = EntityID(terrestrialSurfaceControlV3.entityRegionId);
    m_pEnvironmentalRegionHandler->HandleEntityTerrestrialSurfaceCondition(entityID, condition);
    break;
  }
  case ECigiScope::REGIONAL:
  {
    RegionID regionID = RegionID(terrestrialSurfaceControlV3.entityRegionId);
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

void CCigiPacketHandlerV3::ParseViewControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::ViewCtrl viewControlV3;
  memcpy(&viewControlV3, pBuffer, sizeof(CIGI::V33::ViewCtrl));

  if (m_bByteSwap)
  {
    viewControlV3.doByteSwapping();
  }

  SCigiViewControl viewControl;
  viewControl.viewGroupID = ViewGroupID(viewControlV3.groupId);
  viewControl.viewID = ViewID(viewControlV3.viewId);
  viewControl.entityID = EntityID(viewControlV3.entityId);
  viewControl.bYawEnabled = viewControlV3.yawEnable;
  viewControl.bPitchEnabled = viewControlV3.pitchEnable;
  viewControl.bRollEnabled = viewControlV3.rollEnable;
  viewControl.rotation.yaw = Degrees(viewControlV3.yaw);
  viewControl.rotation.pitch = Degrees(viewControlV3.pitch);
  viewControl.rotation.roll = Degrees(viewControlV3.roll);
  viewControl.offsetEnabled[0] = viewControlV3.xOffsetEnable;
  viewControl.offsetEnabled[1] = viewControlV3.yOffsetEnable;
  viewControl.offsetEnabled[2] = viewControlV3.zOffsetEnable;
  viewControl.offset[0] = viewControlV3.xOffset;
  viewControl.offset[1] = viewControlV3.yOffset;
  viewControl.offset[2] = viewControlV3.zOffset;
  m_pViewHandler->Handle(viewControl);
}

ECigiScope ConvertScope33(CIGI::V33::WaveCtrl::Scope eScope)
{
  switch (eScope)
  {
  case CIGI::V33::WaveCtrl::Scope::eScope_Global:
    return ECigiScope::GLOBAL;
  case CIGI::V33::WaveCtrl::Scope::eScope_Regional:
    return ECigiScope::REGIONAL;
  case CIGI::V33::WaveCtrl::Scope::eScope_Entity:
    return ECigiScope::ENTITY;
  }

  return ECigiScope::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseWaveControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::WaveCtrl waveControlV3;
  memcpy(&waveControlV3, pBuffer, sizeof(CIGI::V33::WaveCtrl));

  if (m_bByteSwap)
  {
    waveControlV3.doByteSwapping();
  }

  int nID = waveControlV3.entityRegionId;
  SCigiWaveCondition condition;
  condition.waveID = waveControlV3.waveId;
  condition.fWaveHeight = waveControlV3.waveHeight;
  condition.fWavelength = waveControlV3.wavelength;
  condition.fPeriod = waveControlV3.period;
  condition.phaseOffset = Degrees360(waveControlV3.phaseOffset);
  condition.leading = Degrees180(waveControlV3.leading);

  condition.direction = Degrees360(waveControlV3.direction);

  condition.bWaveEnabled = waveControlV3.waveEnable;

  ECigiScope eScope = ConvertScope33(static_cast<CIGI::V33::WaveCtrl::Scope>(waveControlV3.scope));
  m_pWaveControlHandler->Handle(condition, eScope, nID);
}

EProjectionMode ConvertProjectionMode33(CIGI::V33::ViewDefinition::ProjectionType eProjection)
{
  if (eProjection == CIGI::V33::ViewDefinition::ProjectionType::eProjection_Perspective)
  {
    return EProjectionMode::PERSPECTIVE;
  }
  else if (eProjection == CIGI::V33::ViewDefinition::ProjectionType::eProjection_Orthographic)
  {
    return EProjectionMode::ORTHOGRAPHIC;
  }

  return EProjectionMode::UNKNOWN;
}

EMirrorMode ConvertMirrorMode33(CIGI::V33::ViewDefinition::MirrorMode eMirror)
{
  if (eMirror == CIGI::V33::ViewDefinition::eMirrorMode_None)
  {
    return EMirrorMode::NONE;
  }
  else if (eMirror == CIGI::V33::ViewDefinition::eMirrorMode_Horizontal)
  {
    return EMirrorMode::HORIZONTAL;
  }
  else if (eMirror == CIGI::V33::ViewDefinition::eMirrorMode_Vertical)
  {
    return EMirrorMode::VERTICAL;
  }
  else if (eMirror == CIGI::V33::ViewDefinition::eMirrorMode_Both)
  {
    return EMirrorMode::HORIZONTAL_AND_VERTICAL;
  }

  return EMirrorMode::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseViewDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::ViewDefinition viewDefinitionV3;
  memcpy(&viewDefinitionV3, pBuffer, sizeof(CIGI::V33::ViewDefinition));

  if (m_bByteSwap)
  {
    viewDefinitionV3.doByteSwapping();
  }

  SCigiViewDefinition viewDefinition;
  viewDefinition.viewID = ViewID(viewDefinitionV3.viewId);
  viewDefinition.viewGroupID = ViewGroupID(viewDefinitionV3.groupId);
  viewDefinition.bTopEnabled = viewDefinitionV3.topEnable;
  viewDefinition.bBottomEnabled = viewDefinitionV3.bottomEnable;
  viewDefinition.bNearEnabled = viewDefinitionV3.nearEnable;
  viewDefinition.bFarEnabled = viewDefinitionV3.farEnable;
  viewDefinition.bLeftEnabled = viewDefinitionV3.leftEnable;
  viewDefinition.bRightEnabled = viewDefinitionV3.rightEnable;
  viewDefinition.fTop = viewDefinitionV3.top;
  viewDefinition.fBottom = viewDefinitionV3.bottom;
  viewDefinition.fNear = viewDefinitionV3.nearClip;
  viewDefinition.fFar = viewDefinitionV3.farClip;
  viewDefinition.fLeft = viewDefinitionV3.left;
  viewDefinition.fRight = viewDefinitionV3.right;
  viewDefinition.bReorder = viewDefinitionV3.reorder;
  viewDefinition.eProjectionMode = ConvertProjectionMode33(static_cast<CIGI::V33::ViewDefinition::ProjectionType>(viewDefinitionV3.projectionType));
  viewDefinition.eMirrorMode = ConvertMirrorMode33(static_cast<CIGI::V33::ViewDefinition::MirrorMode>(viewDefinitionV3.mirrorMode));
  viewDefinition.pixelReplicationMode = PixelReplicationMode(static_cast<uint8_t>(viewDefinitionV3.pixelReplicationMode));
  m_pViewHandler->Handle(viewDefinition);
}

void CCigiPacketHandlerV3::ParseWeatherControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::WeatherCtrl weatherControl;
  memcpy(&weatherControl, pBuffer, sizeof(CIGI::V33::WeatherCtrl));

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
  condition.bTopScudEnabled = weatherControl.scudEnable;//updated in version 4
  condition.bBottomScudEnabled = weatherControl.scudEnable;//updated in version 4
  condition.bottomScudFrequency = Percentage(weatherControl.scudFrequency);//updated in version 4
  condition.topScudFrequency = Percentage(weatherControl.scudFrequency);//updated in version 4

  SCigiSpatialWeatherCondition spatialWeatherCondition;
  spatialWeatherCondition.fBaseElevation = weatherControl.baseElevation;
  spatialWeatherCondition.fBottomTransitionBandThickness = weatherControl.transitionBand;//updated in version 4
  spatialWeatherCondition.fTopTransitionBandThickness = weatherControl.transitionBand;//updated in version 4
  spatialWeatherCondition.fThickness = weatherControl.thickness;

  // set weather conditions on entity, region, or global level
  switch (weatherControl.scope)
  {
  case CIGI::V33::WeatherCtrl::Scope::eScope_Entity:
  {
    EntityID entityID = EntityID(weatherControl.entityRegionId);

    m_pEnvironmentalRegionHandler->SetEntityWeatherCondition(entityID, condition, spatialWeatherCondition);
    break;
  }
  case CIGI::V33::WeatherCtrl::Scope::eScope_Regional:
  {
    RegionID regionID = RegionID(weatherControl.entityRegionId);
    RegionalLayeredWeatherID layerID = RegionalLayeredWeatherID(weatherControl.layerId);

    m_pEnvironmentalRegionHandler->SetRegionalWeatherCondition(regionID, layerID, condition, spatialWeatherCondition);
    break;
  }
  case CIGI::V33::WeatherCtrl::Scope::eScope_Global:
  {
    GlobalLayeredWeatherID layerID = GlobalLayeredWeatherID(weatherControl.layerId);

    m_pEnvironmentalRegionHandler->SetGlobalWeatherCondition(layerID, condition, spatialWeatherCondition);
    break;
  }
  }
}

EObjectCoordinateSystem ConvertCoordinateSystem33(CIGI::V33::RateCtrl::CoordinateSystem eCoordinateSystem)
{
  if (eCoordinateSystem == CIGI::V33::RateCtrl::CoordinateSystem::eCoordinateSystem_WorldParent)
  {
    return EObjectCoordinateSystem::WORLD;
  }
  else if (eCoordinateSystem == CIGI::V33::RateCtrl::CoordinateSystem::eCoordinateSystem_Local)
  {
    return EObjectCoordinateSystem::LOCAL;
  }

  return EObjectCoordinateSystem::UNKNOWN;
}

void CCigiPacketHandlerV3::ParseRateControlPacket(uint8_t* pBuffer)
{
  CIGI::V33::RateCtrl rateControl;
  memcpy(&rateControl, pBuffer, sizeof(CIGI::V33::RateCtrl));

  if (m_bByteSwap)
  {
    rateControl.doByteSwapping();
  }

  CigiBodyCoordinates linearVelocity;
  TCigiBodyEulerRotation angularVelocity;

  linearVelocity[0] = rateControl.xLinearRate;
  linearVelocity[1] = rateControl.yLinearRate;
  linearVelocity[2] = rateControl.zLinearRate;

  angularVelocity.yaw = Degrees(rateControl.yawAngularRate);
  angularVelocity.pitch = Degrees(rateControl.pitchAngularRate);
  angularVelocity.roll = Degrees(rateControl.rollAngularRate);

  if (rateControl.applyToArticulatedPart)
  {
    SCigiArticulatedPartVelocityControl articulatedPartVelocityControl;
    articulatedPartVelocityControl.entityID = EntityID(rateControl.entityId);
    articulatedPartVelocityControl.articulatedPartID = ArticulatedPartID(rateControl.articulatedPartId);
    articulatedPartVelocityControl.angularVelocity = angularVelocity;
    articulatedPartVelocityControl.linearVelocity = linearVelocity;

    m_pVelocityControlHandler->HandleArticulatedPartVelocityControl(articulatedPartVelocityControl);
  }
  else
  {
    SCigiEntityVelocityControl entityVelocityControl;
    entityVelocityControl.entityID = EntityID(rateControl.entityId);
    entityVelocityControl.coordinateSystem = ConvertCoordinateSystem33(static_cast<CIGI::V33::RateCtrl::CoordinateSystem>(rateControl.coordinateSystem));
    entityVelocityControl.angularVelocity = angularVelocity;
    entityVelocityControl.linearVelocity = linearVelocity;

    m_pVelocityControlHandler->HandleEntityVelocityControl(entityVelocityControl);
  }
}

void CCigiPacketHandlerV3::ParseTrajectoryDefinitionPacket(uint8_t* pBuffer)
{
  CIGI::V33::TrajectoryDefinition trajectoryDefV3;
  memcpy(&trajectoryDefV3, pBuffer, sizeof(CIGI::V33::TrajectoryDefinition));

  if (m_bByteSwap)
  {
    trajectoryDefV3.doByteSwapping();
  }

  SCigiEntityAcceleration acceleration;
  acceleration.angularAcceleration.roll = Degrees(0);//not in 3.3
  acceleration.angularAcceleration.pitch = Degrees(0);//not in 3.3
  acceleration.angularAcceleration.yaw = Degrees(0);//not in 3.3

  acceleration.linearAcceleration[0] = trajectoryDefV3.accelerationX;
  acceleration.linearAcceleration[1] = trajectoryDefV3.accelerationY;
  acceleration.linearAcceleration[2] = trajectoryDefV3.accelerationZ;

  //acceleration.articulatedPartID = UnknownArticulatedPartID;//not in 3.3
  //acceleration.bApplyToArticulatedPart = false;//not in 3.3
  acceleration.eCoordinateSystem = EObjectCoordinateSystem::LOCAL;//not in 3.3
  acceleration.entityID = EntityID(trajectoryDefV3.entityId);

  m_pAccelerationControlHandler->HandleEntityAccelerationControl(acceleration);
}

void CCigiPacketHandlerV3::ProcessPackets()
{
  // begin a new packet log frame if logging is enabled so that each call to ProcessPackets corresponds to a single frame in the log
  if (m_pPacketLoggerWriter)
  {
    m_pPacketLoggerWriter->BeginFrame();
  }

  uint8_t buffer[65535];
  const int nBufferSize = static_cast<int>(sizeof(buffer));

  SCigiPacketHeaderV3 packetHeader;
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

    buffer[nMessageSize] = '\0';

    uint8_t* pBuffer = buffer;
    const uint8_t* pBufferEnd = buffer + nMessageSize;

    // process all packets in the current message
    while (pBuffer < pBufferEnd)
    {
      ptrdiff_t nRemainingBytes = pBufferEnd - pBuffer;
      if (nRemainingBytes < static_cast<ptrdiff_t>(sizeof(SCigiPacketHeaderV3)))
      {
        stringstream ss;
        ss << "Skipping truncated packet header with " << nRemainingBytes << " byte(s) remaining; expected at least " << sizeof(SCigiPacketHeaderV3) << endl;
        g_CigiLibGlobals.pLogger->LogWarning(ss.str());
        break;
      }

      memcpy(&packetHeader, pBuffer, sizeof(SCigiPacketHeaderV3));

      uint16_t nPacketSize = packetHeader.nPacketSize;

      // validate packet header
      if (nPacketSize == 0)
      {
        break;
      }

      // ensure packet size is large enough to contain the header
      if (nPacketSize < sizeof(SCigiPacketHeaderV3))
      {
        stringstream ss;
        ss << "Skipping invalid packet: opcode " << (int)packetHeader.eOpCode << ", size " << nPacketSize << " is smaller than header size " << sizeof(SCigiPacketHeaderV3) << endl;
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
      else if (m_ImageGenerator.GetOperationMode() == EIGMode::RESET && packetHeader.eOpCode != ECigiOpCodeV3::IG_CONTROL)
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
      // if reading from a log, read the next message for the frame if available
      if (!buffers.empty())
      {
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
