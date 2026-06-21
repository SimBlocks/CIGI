//Copyright SimBlocks LLC 2016-2026
#include "HostSessionV3_3.h"
#include "GlobalHeaders/CommonTypes.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiEntityTypes.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiEvent.h"
#include "HostCigiLib/HostCigiLib.h"
#include "HostCigiLib/Host.h"
#include "UtilitiesLib/Buffer.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "ScriptRuntime.h"
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

using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;
using namespace sbio::symbol;
using namespace std;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

CHostSessionV3_3::CHostSessionV3_3()
{
  m_IGControl.dbNumber = hostSetupOptions.defaultDatabaseID.Value();
  m_DesiredIGMode = EIGMode::RESET;
  m_IGControl.extrapolationEnable = false;

  m_PacketHandlerFunctions[ECigiOpCodeV3::START_OF_FRAME] = &CHostSessionV3_3::ParseStartOfFramePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::HAT_HOT_RESPONSE] = &CHostSessionV3_3::ParseHatHotResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::HAT_HOT_EXTENDED_RESPONSE] = &CHostSessionV3_3::ParseHatHotExtendedResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::LINE_OF_SIGHT_RESPONSE] = &CHostSessionV3_3::ParseLineOfSightResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::LINE_OF_SIGHT_EXTENDED_RESPONSE] = &CHostSessionV3_3::ParseLineOfSightExtendedResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SENSOR_RESPONSE] = &CHostSessionV3_3::ParseSensorResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::SENSOR_EXTENDED_RESPONSE] = &CHostSessionV3_3::ParseSensorExtendedResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::POSITION_RESPONSE] = &CHostSessionV3_3::ParsePositionResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::WEATHER_CONDITIONS_RESPONSE] = &CHostSessionV3_3::ParseWeatherConditionsResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::AEROSOL_CONCENTRATION_RESPONSE] = &CHostSessionV3_3::ParseAerosolConcentrationResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::MARITIME_SURFACE_CONDITIONS_RESPONSE] = &CHostSessionV3_3::ParseMaritimeSurfaceConditionsResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE] = &CHostSessionV3_3::ParseTerrestrialSurfaceConditionsResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_NOTIFICATION] = &CHostSessionV3_3::ParseCollisionDetectionSegmentNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_NOTIFICATION] = &CHostSessionV3_3::ParseCollisionDetectionVolumeNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::ANIMATION_STOP_NOTIFICATION] = &CHostSessionV3_3::ParseAnimationStopNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::EVENT_NOTIFICATION] = &CHostSessionV3_3::ParseEventNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV3::IMAGE_GENERATOR_MESSAGE] = &CHostSessionV3_3::ParseImageGeneratorMessagePacket;
}

void CHostSessionV3_3::ParseStartOfFramePacket(uint8_t* buffer)
{
  CIGI::V33::SoF startOfFramePacket;
  memcpy(&startOfFramePacket, buffer, sizeof(CIGI::V33::SoF));

  if (m_bByteSwap)
  {
    startOfFramePacket.doByteSwapping();
  }

  m_ActualIGMode = ConvertToIGMode33(static_cast<CIGI::V33::SoF::IGMode>(startOfFramePacket.igMode));
  m_LastReceivedIGFrame = FrameNumber(startOfFramePacket.igFrameNumber);
  DatabaseID databaseID = DatabaseID(startOfFramePacket.dbNumber);

  HostCigiStartOfFrameEventArgs startOfFrameArgs;
  startOfFrameArgs.startOfFrame.eVersion = ECigiVersion::VERSION_3_3;
  startOfFrameArgs.startOfFrame.databaseID = CigiDatabaseNumber(startOfFramePacket.dbNumber);
  startOfFrameArgs.startOfFrame.eIGMode = m_ActualIGMode;
  startOfFrameArgs.startOfFrame.bTimestampValid = startOfFramePacket.timestampValid != 0;
  startOfFrameArgs.startOfFrame.bEarthReferenceModel = startOfFramePacket.earthReferenceModel != 0;
  startOfFrameArgs.startOfFrame.igFrameNumber = m_LastReceivedIGFrame;
  startOfFrameArgs.startOfFrame.microseconds = Microsecond(startOfFramePacket.timestamp);
  startOfFrameArgs.startOfFrame.lastHostFrameNumber = FrameNumber(startOfFramePacket.lastHostFrameNumber);
  Event::Raise<HostCigiEvent>(startOfFrameArgs);

  NotifyStartOfFrameReceived();

  // handle IG-controlled database
  if (hostSetupOptions.bDatabaseIGControlled)
  {
    m_eDatabaseState = EHostSessionDatabaseState::IG_CONTROLLED;
  }

  if (m_DesiredIGMode == EIGMode::OPERATE)
  {
    if (!hostSetupOptions.bDatabaseIGControlled)
    {
      if (startOfFramePacket.dbNumber == -128)
      {
        // If the Host requests a database that does not exist or fails to load, the IG shall set this parameter to -128.
      }
      else if (startOfFramePacket.dbNumber >= -127 && startOfFramePacket.dbNumber < 0)
      {
        //IG is loading database
        m_eDatabaseState = EHostSessionDatabaseState::LOADING_ACKNOWLEDGED;
      }
      else if (startOfFramePacket.dbNumber > 0)
      {
        if (m_eDatabaseState != EHostSessionDatabaseState::LOADED)
        {
          m_eDatabaseState = EHostSessionDatabaseState::LOADED;

          HostCigiDatabaseLoadedEventArgs args;
          args.eDatabaseID = databaseID;
          Event::Raise<HostCigiEvent>(args);
        }
      }
    }
  }
}

void CHostSessionV3_3::ParseHatHotResponsePacket(uint8_t* buffer)
{
  CIGI::V33::HATHOTResponse hatHotResponsePacket;
  memcpy(&hatHotResponsePacket, buffer, sizeof(CIGI::V33::HATHOTResponse));

  if (m_bByteSwap)
  {
    hatHotResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::HAT_HOT_RESPONSE) << "\n"
                    << "HAT/HOT ID: " << std::to_string(hatHotResponsePacket.HATHOTId) << "\n"
                    << "Valid: " << ValidToString(hatHotResponsePacket.valid) << "\n"
                    << "Response Type: " << ((hatHotResponsePacket.responseType == CIGI::V32::HATHOTResponse::eResponseType_HAT) ? "HAT" : "HOT") << "\n"
                    << "Host Frame Number LSN: " << std::to_string(hatHotResponsePacket.hostFrameNumberLSN) << "\n"
                    << "Height: " << std::to_string(hatHotResponsePacket.height);
  Event::Raise<HostCigiEvent>(args);

  if (hatHotResponsePacket.responseType == CIGI::V32::HATHOTResponse::eResponseType_HAT)
  {
    HostCigiHatResponseEventArgs hatArgs;
    hatArgs.hatResponse.bValid = hatHotResponsePacket.valid != 0;
    hatArgs.hatResponse.hostFrameLSN = hatHotResponsePacket.hostFrameNumberLSN;
    hatArgs.hatResponse.HATHOTID = HATHOTID(hatHotResponsePacket.HATHOTId);
    hatArgs.hatResponse.heightAboveTerrain = hatHotResponsePacket.height;
    Event::Raise<HostCigiEvent>(hatArgs);
  }
  else
  {
    HostCigiHotResponseEventArgs hotArgs;
    hotArgs.hotResponse.bValid = hatHotResponsePacket.valid != 0;
    hotArgs.hotResponse.hostFrameLSN = hatHotResponsePacket.hostFrameNumberLSN;
    hotArgs.hotResponse.HATHOTID = HATHOTID(hatHotResponsePacket.HATHOTId);
    hotArgs.hotResponse.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(hatHotResponsePacket.height);
    Event::Raise<HostCigiEvent>(hotArgs);
  }
}

void CHostSessionV3_3::SendEntityControl(const sbio::cigi::SEntityControl& entityControl)
{
  if (entityControl.eExtendedEntityType == EExtendedEntityType::EXTENDED)
  {
    HostCigiErrorEventArgs args;
    args.sError = "CIGI 3.3 entity control only supports short entity types.";
    Event::Raise<HostCigiEvent>(args);
    return;
  }

  STopLevelEntityPosition topLevelEntityPosition;
  topLevelEntityPosition.entityID = entityControl.entityID;
  topLevelEntityPosition.bAttached = entityControl.bHasParent;
  topLevelEntityPosition.eClamp = EClamp::NONE;

  if (entityControl.bHasParent)
  {
    SChildEntityPosition childEntityPosition;
    childEntityPosition.entityID = entityControl.entityID;
    childEntityPosition.bAttached = true;
    childEntityPosition.parentID = entityControl.parentID;

    SCigiAnimationControl animationControl;
    animationControl.entityID = entityControl.entityID;

    SendChildEntityControl(entityControl, childEntityPosition, animationControl);
    return;
  }

  SCigiAnimationControl animationControl;
  animationControl.entityID = entityControl.entityID;

  SendTopLevelEntityControl(entityControl, topLevelEntityPosition, animationControl);
}

void CHostSessionV3_3::ParseHatHotExtendedResponsePacket(uint8_t* buffer)
{
  CIGI::V33::HATHOTExtendedResponse hatHotExtendedResponsePacket;
  memcpy(&hatHotExtendedResponsePacket, buffer, sizeof(CIGI::V33::HATHOTExtendedResponse));

  if (m_bByteSwap)
  {
    hatHotExtendedResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::HAT_HOT_EXTENDED_RESPONSE) << "\n"
                       << "Valid: " << ValidToString(hatHotExtendedResponsePacket.valid) << "\n"
                       << "Host Frame Number LSN: " << std::to_string(hatHotExtendedResponsePacket.hostFrameNumberLSN) << "\n"
                       << "HAT/HOT ID: " << std::to_string(hatHotExtendedResponsePacket.HATHOTId) << "\n"
                       << "HAT: " << std::to_string(hatHotExtendedResponsePacket.HAT) << "\n"
                       << "HOT: " << std::to_string(hatHotExtendedResponsePacket.HOT) << "\n"
                       << "Material Code: " << std::to_string(hatHotExtendedResponsePacket.materialCode) << "\n"
                       << "Normal Vector Azimuth: " << std::to_string(hatHotExtendedResponsePacket.normalVectorAzimuth) << "\n"
                       << "Normal Vector Elevation: " << std::to_string(hatHotExtendedResponsePacket.normalVectorElevation);
  Event::Raise<HostCigiEvent>(msgArgs);

  HostCigiHatHotExtendedResponseEventArgs hatHotArgs;
  hatHotArgs.hatHotExtendedResponse.bValid = hatHotExtendedResponsePacket.valid != 0;
  hatHotArgs.hatHotExtendedResponse.hostFrameLSN = hatHotExtendedResponsePacket.hostFrameNumberLSN;
  hatHotArgs.hatHotExtendedResponse.HATHOTID = HATHOTID(hatHotExtendedResponsePacket.HATHOTId);
  hatHotArgs.hatHotExtendedResponse.heightAboveTerrain = hatHotExtendedResponsePacket.HAT;
  hatHotArgs.hatHotExtendedResponse.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(hatHotExtendedResponsePacket.HOT);
  hatHotArgs.hatHotExtendedResponse.materialCode = MaterialID(hatHotExtendedResponsePacket.materialCode);
  hatHotArgs.hatHotExtendedResponse.normalVectorAzimuth = Degrees180(hatHotExtendedResponsePacket.normalVectorAzimuth);
  hatHotArgs.hatHotExtendedResponse.normalVectorElevation = Degrees90(hatHotExtendedResponsePacket.normalVectorElevation);
  Event::Raise<HostCigiEvent>(hatHotArgs);
}

void CHostSessionV3_3::ParseLineOfSightResponsePacket(uint8_t* buffer)
{
  CIGI::V33::LineOfSightResponse lineOfSightResponsePacket;
  memcpy(&lineOfSightResponsePacket, buffer, sizeof(CIGI::V33::LineOfSightResponse));

  if (m_bByteSwap)
  {
    lineOfSightResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::LINE_OF_SIGHT_RESPONSE) << "\n"
                       << "LOS ID: " << std::to_string(lineOfSightResponsePacket.LOSId) << "\n"
                       << "Entity ID: " << std::to_string(lineOfSightResponsePacket.entityId) << "\n"
                       << "Valid: " << ValidToString(lineOfSightResponsePacket.valid) << "\n"
                       << "Entity ID Valid: " << ValidToString(lineOfSightResponsePacket.entityIdValid) << "\n"
                       << "Visible: " << VisibleToString(lineOfSightResponsePacket.visible) << "\n"
                       << "Host Frame Number LSN: " << std::to_string(lineOfSightResponsePacket.hostFrameNumberLSN) << "\n"
                       << "Response Count: " << std::to_string(lineOfSightResponsePacket.responseCount) << "\n"
                       << "Range: " << std::to_string(lineOfSightResponsePacket.range) << "\n";
  Event::Raise<HostCigiEvent>(msgArgs);

  if (lineOfSightResponsePacket.entityIdValid)
  {
    HostCigiLineOfSightEntityResponseEventArgs lineOfSightArgs;
    lineOfSightArgs.lineOfSightEntityResponse.lineOfSightRequestID = LineOfSightRequestID(lineOfSightResponsePacket.LOSId);
    lineOfSightArgs.lineOfSightEntityResponse.entityID = EntityID(lineOfSightResponsePacket.entityId);
    lineOfSightArgs.lineOfSightEntityResponse.bValid = lineOfSightResponsePacket.valid != 0;
    lineOfSightArgs.lineOfSightEntityResponse.bVisible = lineOfSightResponsePacket.visible != 0;
    lineOfSightArgs.lineOfSightEntityResponse.hostFrameLSN = lineOfSightResponsePacket.hostFrameNumberLSN;
    lineOfSightArgs.lineOfSightEntityResponse.responseCount = lineOfSightResponsePacket.responseCount;
    lineOfSightArgs.lineOfSightEntityResponse.dRange = lineOfSightResponsePacket.range;
    Event::Raise<HostCigiEvent>(lineOfSightArgs);
  }
  else
  {
    HostCigiLineOfSightResponseEventArgs lineOfSightArgs;
    lineOfSightArgs.lineOfSightResponse.lineOfSightRequestID = LineOfSightRequestID(lineOfSightResponsePacket.LOSId);
    lineOfSightArgs.lineOfSightResponse.bValid = lineOfSightResponsePacket.valid != 0;
    lineOfSightArgs.lineOfSightResponse.hostFrameLSN = lineOfSightResponsePacket.hostFrameNumberLSN;
    lineOfSightArgs.lineOfSightResponse.responseCount = lineOfSightResponsePacket.responseCount;
    lineOfSightArgs.lineOfSightResponse.dRange = lineOfSightResponsePacket.range;
    Event::Raise<HostCigiEvent>(lineOfSightArgs);
  }
}

void CHostSessionV3_3::ParseLineOfSightExtendedResponsePacket(uint8_t* buffer)
{
  CIGI::V33::LineOfSightExtendedResponse lineOfSightExtendedResponsePacket;
  memcpy(&lineOfSightExtendedResponsePacket, buffer, sizeof(CIGI::V33::LineOfSightExtendedResponse));

  if (m_bByteSwap)
  {
    lineOfSightExtendedResponsePacket.doByteSwapping();
  }

  const LineOfSightRequestID lineOfSightRequestID(lineOfSightExtendedResponsePacket.LOSId);
  const ETopLevelCoordinateSystem eResponseCoordinateSystem = GetLineOfSightRequestCoordinateSystem(lineOfSightRequestID);

  if (eResponseCoordinateSystem == ETopLevelCoordinateSystem::UNKNOWN)
  {
    g_HostCigiLibGlobals.pLogger->LogWarning("Received Line of Sight Extended Response with unknown requested coordinate system.");
    return;
  }

  const bool bGeodeticCoordinatesResponse = eResponseCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC || (lineOfSightExtendedResponsePacket.entityIdValid == 0);

  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::LINE_OF_SIGHT_EXTENDED_RESPONSE) << "\n"
                       << "LOS ID: " << std::to_string(lineOfSightExtendedResponsePacket.LOSId) << "\n"
                       << "Entity ID: " << std::to_string(lineOfSightExtendedResponsePacket.entityId) << "\n"
                       << "Valid: " << ValidToString(lineOfSightExtendedResponsePacket.valid) << "\n"
                       << "Entity ID Valid: " << ValidToString(lineOfSightExtendedResponsePacket.entityIdValid) << "\n"
                       << "Range Valid: " << ValidToString(lineOfSightExtendedResponsePacket.rangeValid) << "\n"
                       << "Visible: " << VisibleToString(lineOfSightExtendedResponsePacket.visible) << "\n"
                       << "Host Frame Number LSN: " << std::to_string(lineOfSightExtendedResponsePacket.hostFrameNumberLSN) << "\n"
                       << "Response Count: " << std::to_string(lineOfSightExtendedResponsePacket.responseCount) << "\n"
                       << "Range: " << std::to_string(lineOfSightExtendedResponsePacket.range) << "\n";

  if (bGeodeticCoordinatesResponse)
  {
    msgArgs.sDataMessage << "Latitude: " << std::to_string(lineOfSightExtendedResponsePacket.latitudeXOffset) << "\n"
                         << "Longitude: " << std::to_string(lineOfSightExtendedResponsePacket.longitudeYOffset) << "\n"
                         << "Altitude: " << std::to_string(lineOfSightExtendedResponsePacket.altitudeZOffset) << "\n";
  }
  else
  {
    msgArgs.sDataMessage << "X Offset: " << std::to_string(lineOfSightExtendedResponsePacket.latitudeXOffset) << "\n"
                         << "Y Offset: " << std::to_string(lineOfSightExtendedResponsePacket.longitudeYOffset) << "\n"
                         << "Z Offset: " << std::to_string(lineOfSightExtendedResponsePacket.altitudeZOffset) << "\n";
  }

  msgArgs.sDataMessage << "Red: " << std::to_string(lineOfSightExtendedResponsePacket.red) << "\n"
                       << "Green: " << std::to_string(lineOfSightExtendedResponsePacket.green) << "\n"
                       << "Blue: " << std::to_string(lineOfSightExtendedResponsePacket.blue) << "\n"
                       << "Alpha: " << std::to_string(lineOfSightExtendedResponsePacket.alpha) << "\n"
                       << "Material Code: " << std::to_string(lineOfSightExtendedResponsePacket.materialCode) << "\n"
                       << "Normal Vector Azimuth: " << std::to_string(lineOfSightExtendedResponsePacket.normalVectorAzimuth) << "\n"
                       << "Normal Vector Elevation: " << std::to_string(lineOfSightExtendedResponsePacket.normalVectorElevation) << "\n";
  Event::Raise<HostCigiEvent>(msgArgs);

  if (lineOfSightExtendedResponsePacket.entityIdValid != 0)
  {
    if (bGeodeticCoordinatesResponse)
    {
      HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs lineOfSightArgs;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.lineOfSightRequestID = lineOfSightRequestID;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.entityID = EntityID(lineOfSightExtendedResponsePacket.entityId);
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid != 0;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid != 0;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible != 0;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.hostFrameLSN = lineOfSightExtendedResponsePacket.hostFrameNumberLSN;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.responseCount = lineOfSightExtendedResponsePacket.responseCount;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.dRange = lineOfSightExtendedResponsePacket.range;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.materialCode = MaterialID(lineOfSightExtendedResponsePacket.materialCode);
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.fNormalVectorAzimuth = lineOfSightExtendedResponsePacket.normalVectorAzimuth;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.fNormalVectorElevation = lineOfSightExtendedResponsePacket.normalVectorElevation;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.geodeticCoordinates.latitude = Latitude(lineOfSightExtendedResponsePacket.latitudeXOffset);
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.geodeticCoordinates.longitude = Longitude(lineOfSightExtendedResponsePacket.longitudeYOffset);
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(lineOfSightExtendedResponsePacket.altitudeZOffset);
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.surfaceColor = sbio::SColor32();
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.surfaceColor.r = lineOfSightExtendedResponsePacket.red;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.surfaceColor.g = lineOfSightExtendedResponsePacket.green;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.surfaceColor.b = lineOfSightExtendedResponsePacket.blue;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.surfaceColor.a = lineOfSightExtendedResponsePacket.alpha;
      Event::Raise<HostCigiEvent>(lineOfSightArgs);
    }
    else
    {
      HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs lineOfSightArgs;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.lineOfSightRequestID = lineOfSightRequestID;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.entityID = EntityID(lineOfSightExtendedResponsePacket.entityId);
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid != 0;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid != 0;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible != 0;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.hostFrameLSN = lineOfSightExtendedResponsePacket.hostFrameNumberLSN;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.responseCount = lineOfSightExtendedResponsePacket.responseCount;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.dRange = lineOfSightExtendedResponsePacket.range;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.materialCode = MaterialID(lineOfSightExtendedResponsePacket.materialCode);
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.fNormalVectorAzimuth = lineOfSightExtendedResponsePacket.normalVectorAzimuth;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.fNormalVectorElevation = lineOfSightExtendedResponsePacket.normalVectorElevation;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.offset[0] = lineOfSightExtendedResponsePacket.latitudeXOffset;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.offset[1] = lineOfSightExtendedResponsePacket.longitudeYOffset;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.offset[2] = lineOfSightExtendedResponsePacket.altitudeZOffset;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor = sbio::SColor32();
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.r = lineOfSightExtendedResponsePacket.red;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.g = lineOfSightExtendedResponsePacket.green;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.b = lineOfSightExtendedResponsePacket.blue;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.a = lineOfSightExtendedResponsePacket.alpha;
      Event::Raise<HostCigiEvent>(lineOfSightArgs);
    }
  }
  else
  {
    if (bGeodeticCoordinatesResponse)
    {
      HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs lineOfSightArgs;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.lineOfSightRequestID = lineOfSightRequestID;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid != 0;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid != 0;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible != 0;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.hostFrameLSN = lineOfSightExtendedResponsePacket.hostFrameNumberLSN;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.responseCount = lineOfSightExtendedResponsePacket.responseCount;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.dRange = lineOfSightExtendedResponsePacket.range;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.materialCode = MaterialID(lineOfSightExtendedResponsePacket.materialCode);
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.fNormalVectorAzimuth = lineOfSightExtendedResponsePacket.normalVectorAzimuth;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.fNormalVectorElevation = lineOfSightExtendedResponsePacket.normalVectorElevation;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.geodeticCoordinates.latitude = Latitude(lineOfSightExtendedResponsePacket.latitudeXOffset);
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.geodeticCoordinates.longitude = Longitude(lineOfSightExtendedResponsePacket.longitudeYOffset);
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(lineOfSightExtendedResponsePacket.altitudeZOffset);
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.surfaceColor = sbio::SColor32();
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.surfaceColor.r = lineOfSightExtendedResponsePacket.red;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.surfaceColor.g = lineOfSightExtendedResponsePacket.green;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.surfaceColor.b = lineOfSightExtendedResponsePacket.blue;
      lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.surfaceColor.a = lineOfSightExtendedResponsePacket.alpha;
      Event::Raise<HostCigiEvent>(lineOfSightArgs);
    }
    else
    {
      HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs lineOfSightArgs;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.lineOfSightRequestID = lineOfSightRequestID;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.entityID = UnknownEntityID;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid != 0;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid != 0;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible != 0;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.hostFrameLSN = lineOfSightExtendedResponsePacket.hostFrameNumberLSN;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.responseCount = lineOfSightExtendedResponsePacket.responseCount;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.dRange = lineOfSightExtendedResponsePacket.range;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.materialCode = MaterialID(lineOfSightExtendedResponsePacket.materialCode);
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.fNormalVectorAzimuth = lineOfSightExtendedResponsePacket.normalVectorAzimuth;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.fNormalVectorElevation = lineOfSightExtendedResponsePacket.normalVectorElevation;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.offset[0] = lineOfSightExtendedResponsePacket.latitudeXOffset;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.offset[1] = lineOfSightExtendedResponsePacket.longitudeYOffset;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.offset[2] = lineOfSightExtendedResponsePacket.altitudeZOffset;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor = sbio::SColor32();
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.r = lineOfSightExtendedResponsePacket.red;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.g = lineOfSightExtendedResponsePacket.green;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.b = lineOfSightExtendedResponsePacket.blue;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.surfaceColor.a = lineOfSightExtendedResponsePacket.alpha;
      Event::Raise<HostCigiEvent>(lineOfSightArgs);
    }
  }
}

const char* ConvertSensorStatusToStringV32(uint8_t sensorStatus)
{
  switch (static_cast<CIGI::V32::SensorResponse::SensorStatus>(sensorStatus))
  {
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget:
    return "Searching For Target";
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget:
    return "Tracking Target";
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock:
    return "Impending Breaklock";
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_Breaklock:
    return "Breaklock";
  default:
    return "Unknown";
  }
}

ESensorStatus ConvertSensorStatusV32(uint8_t sensorStatus)
{
  switch (static_cast<CIGI::V32::SensorResponse::SensorStatus>(sensorStatus))
  {
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget:
    return ESensorStatus::SEARCHING_FOR_TARGET;
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget:
    return ESensorStatus::TRACKING_TARGET;
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock:
    return ESensorStatus::IMPENDING_BREAKLOCK;
  case CIGI::V32::SensorResponse::SensorStatus::eSensorStatus_Breaklock:
    return ESensorStatus::BREAKLOCK;
  default:
    return ESensorStatus::UKNOWN;
  }
}

void CHostSessionV3_3::ParseSensorResponsePacket(uint8_t* buffer)
{
  CIGI::V33::SensorResponse sensorResponsePacket;
  memcpy(&sensorResponsePacket, buffer, sizeof(CIGI::V33::SensorResponse));

  if (m_bByteSwap)
  {
    sensorResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::SENSOR_RESPONSE) << "\n"
                    << "Sensor ID: " << std::to_string(sensorResponsePacket.sensorId) << "\n"
                    << "Sensor Status: " << ConvertSensorStatusToStringV32(sensorResponsePacket.sensorStatus) << "\n"
                    << "View ID: " << std::to_string(sensorResponsePacket.viewId) << "\n"
                    << "Gate X Size: " << std::to_string(sensorResponsePacket.gateXSize) << "\n"
                    << "Gate Y Size: " << std::to_string(sensorResponsePacket.gateYSize) << "\n"
                    << "Gate X Position: " << std::to_string(sensorResponsePacket.gateXPosition) << "\n"
                    << "Gate Y Position: " << std::to_string(sensorResponsePacket.gateYPosition) << "\n"
                    << "Host Frame Number: " << std::to_string(sensorResponsePacket.hostFrameCounter);
  Event::Raise<HostCigiEvent>(args);

  HostCigiSensorResponseEventArgs sensorArgs;
  sensorArgs.sensorResponse.sensorID = SensorID(sensorResponsePacket.sensorId);
  sensorArgs.sensorResponse.viewID = ViewID(sensorResponsePacket.viewId);
  sensorArgs.sensorResponse.gateSize[0] = static_cast<float>(sensorResponsePacket.gateXSize);
  sensorArgs.sensorResponse.gateSize[1] = static_cast<float>(sensorResponsePacket.gateYSize);
  sensorArgs.sensorResponse.gatePosition[0] = sensorResponsePacket.gateXPosition;
  sensorArgs.sensorResponse.gatePosition[1] = sensorResponsePacket.gateYPosition;
  sensorArgs.sensorResponse.hostFrameNumber = FrameNumber(sensorResponsePacket.hostFrameCounter);
  sensorArgs.sensorResponse.eSensorStatus = ConvertSensorStatusV32(sensorResponsePacket.sensorStatus);
  Event::Raise<HostCigiEvent>(sensorArgs);
}

void CHostSessionV3_3::ParseSensorExtendedResponsePacket(uint8_t* buffer)
{
  CIGI::V33::SensorExtendedResponse sensorExtendedResponsePacket;
  memcpy(&sensorExtendedResponsePacket, buffer, sizeof(CIGI::V33::SensorExtendedResponse));

  if (m_bByteSwap)
  {
    sensorExtendedResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::SENSOR_EXTENDED_RESPONSE) << "\n"
                    << "View ID: " << std::to_string(sensorExtendedResponsePacket.viewId) << "\n"
                    << "Entity ID: " << std::to_string(sensorExtendedResponsePacket.entityId) << "\n"
                    << "Sensor ID: " << std::to_string(sensorExtendedResponsePacket.sensorId) << "\n"
                    << "Sensor Status: " << ConvertSensorStatusToStringV32(sensorExtendedResponsePacket.sensorStatus) << "\n"
                    << "Entity ID Valid: " << ValidToString(sensorExtendedResponsePacket.entityIdValid) << "\n"
                    << "Gate X Size: " << std::to_string(sensorExtendedResponsePacket.gateXSize) << "\n"
                    << "Gate Y Size: " << std::to_string(sensorExtendedResponsePacket.gateYSize) << "\n"
                    << "Gate X Position: " << std::to_string(sensorExtendedResponsePacket.gateXPosition) << "\n"
                    << "Gate Y Position: " << std::to_string(sensorExtendedResponsePacket.gateYPosition) << "\n"
                    << "Host Frame Number: " << std::to_string(sensorExtendedResponsePacket.hostFrameCounter) << "\n"
                    << "Track Point Latitude: " << std::to_string(sensorExtendedResponsePacket.trackPointLatitude) << "\n"
                    << "Track Point Longitude: " << std::to_string(sensorExtendedResponsePacket.trackPointLongitude) << "\n"
                    << "Track Point Altitude: " << std::to_string(sensorExtendedResponsePacket.trackPointAltitude);
  Event::Raise<HostCigiEvent>(args);

  if (sensorExtendedResponsePacket.entityIdValid)
  {
    HostCigiSensorExtendedEntityResponseEventArgs sensorArgs;
    sensorArgs.sensorExtendedEntityResponse.viewID = ViewID(sensorExtendedResponsePacket.viewId);
    sensorArgs.sensorExtendedEntityResponse.entityID = EntityID(sensorExtendedResponsePacket.entityId);
    sensorArgs.sensorExtendedEntityResponse.sensorID = SensorID(sensorExtendedResponsePacket.sensorId);
    sensorArgs.sensorExtendedEntityResponse.gateSize[0] = static_cast<float>(sensorExtendedResponsePacket.gateXSize);
    sensorArgs.sensorExtendedEntityResponse.gateSize[1] = static_cast<float>(sensorExtendedResponsePacket.gateYSize);
    sensorArgs.sensorExtendedEntityResponse.gatePosition[0] = sensorExtendedResponsePacket.gateXPosition;
    sensorArgs.sensorExtendedEntityResponse.gatePosition[1] = sensorExtendedResponsePacket.gateYPosition;
    sensorArgs.sensorExtendedEntityResponse.hostFrameNumber = FrameNumber(sensorExtendedResponsePacket.hostFrameCounter);
    sensorArgs.sensorExtendedEntityResponse.trackPoint.latitude = Latitude(sensorExtendedResponsePacket.trackPointLatitude);
    sensorArgs.sensorExtendedEntityResponse.trackPoint.longitude = Longitude(sensorExtendedResponsePacket.trackPointLongitude);
    sensorArgs.sensorExtendedEntityResponse.trackPoint.altitude = HeightRelativeToWGS84Ellipsoid(sensorExtendedResponsePacket.trackPointAltitude);
    sensorArgs.sensorExtendedEntityResponse.eSensorStatus = ConvertSensorStatusV32(sensorExtendedResponsePacket.sensorStatus);
    Event::Raise<HostCigiEvent>(sensorArgs);
  }
  else
  {
    HostCigiSensorExtendedResponseEventArgs sensorArgs;
    sensorArgs.sensorExtendedResponse.viewID = ViewID(sensorExtendedResponsePacket.viewId);
    sensorArgs.sensorExtendedResponse.sensorID = SensorID(sensorExtendedResponsePacket.sensorId);
    sensorArgs.sensorExtendedResponse.gateSize[0] = static_cast<float>(sensorExtendedResponsePacket.gateXSize);
    sensorArgs.sensorExtendedResponse.gateSize[1] = static_cast<float>(sensorExtendedResponsePacket.gateYSize);
    sensorArgs.sensorExtendedResponse.gatePosition[0] = sensorExtendedResponsePacket.gateXPosition;
    sensorArgs.sensorExtendedResponse.gatePosition[1] = sensorExtendedResponsePacket.gateYPosition;
    sensorArgs.sensorExtendedResponse.hostFrameNumber = FrameNumber(sensorExtendedResponsePacket.hostFrameCounter);
    sensorArgs.sensorExtendedResponse.trackPoint.latitude = Latitude(sensorExtendedResponsePacket.trackPointLatitude);
    sensorArgs.sensorExtendedResponse.trackPoint.longitude = Longitude(sensorExtendedResponsePacket.trackPointLongitude);
    sensorArgs.sensorExtendedResponse.trackPoint.altitude = HeightRelativeToWGS84Ellipsoid(sensorExtendedResponsePacket.trackPointAltitude);
    sensorArgs.sensorExtendedResponse.eSensorStatus = ConvertSensorStatusV32(sensorExtendedResponsePacket.sensorStatus);
    Event::Raise<HostCigiEvent>(sensorArgs);
  }
}

const char* ConvertPositionObjectClassToStringV30(CIGI::V30::PositionResponse::ObjectClass objectClass)
{
  switch (objectClass)
  {
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_Entity:
    return "Entity";
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_ArticulatedPart:
    return "Articulated Part";
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_View:
    return "View";
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_ViewGroup:
    return "View Group";
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_MotionTracker:
    return "Motion Tracker";
  default:
    return "Unknown";
  }
}

EObjectClass ConvertPositionObjectClassV30(CIGI::V30::PositionResponse::ObjectClass objectClass)
{
  switch (objectClass)
  {
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_Entity:
    return EObjectClass::ENTITY;
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_ArticulatedPart:
    return EObjectClass::ARTICULATED_PART;
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_View:
    return EObjectClass::VIEW;
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_ViewGroup:
    return EObjectClass::VIEW_GROUP;
  case CIGI::V30::PositionResponse::ObjectClass::eObjectClass_MotionTracker:
    return EObjectClass::MOTION_TRACKER;
  default:
    return EObjectClass::UNKNOWN;
  }
}

const char* ConvertPositionCoordinateSystemToStringV30(CIGI::V30::PositionResponse::CoordinateSystem coordinateSystem)
{
  switch (coordinateSystem)
  {
  case CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic:
    return "Geodetic";
  case CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_ParentEntity:
    return "Parent Entity";
  case CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_Submodel:
    return "Submodel";
  default:
    return "Unknown";
  }
}

void CHostSessionV3_3::ParsePositionResponsePacket(uint8_t* buffer)
{
  CIGI::V33::PositionResponse positionResponsePacket;
  memcpy(&positionResponsePacket, buffer, sizeof(CIGI::V33::PositionResponse));

  if (m_bByteSwap)
  {
    positionResponsePacket.doByteSwapping();
  }

  CIGI::V30::PositionResponse::ObjectClass eObjectClass = static_cast<CIGI::V30::PositionResponse::ObjectClass>(positionResponsePacket.objectClass);
  CIGI::V30::PositionResponse::CoordinateSystem eCoordinateSystem = static_cast<CIGI::V30::PositionResponse::CoordinateSystem>(positionResponsePacket.coordinateSystem);

  {
    HostCigiDataMessageEventArgs args;
    args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::POSITION_RESPONSE) << "\n"
                      << "Articulated Part ID: " << std::to_string(positionResponsePacket.articulatedPartId) << "\n"
                      << "Object Class: " << ConvertPositionObjectClassToStringV30(eObjectClass) << "\n"
                      << "Coordinate System: " << ConvertPositionCoordinateSystemToStringV30(eCoordinateSystem) << "\n"
                      << "Object ID: " << std::to_string(positionResponsePacket.objectId) << "\n";

    if (eCoordinateSystem == CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic)
    {
      args.sDataMessage << "Latitude: " << std::to_string(positionResponsePacket.latitudeXOffset) << "\n"
                        << "Longitude: " << std::to_string(positionResponsePacket.longitudeYOffset) << "\n"
                        << "Altitude: " << std::to_string(positionResponsePacket.altitudeZOffset) << "\n";
    }
    else
    {
      args.sDataMessage << "X Offset: " << std::to_string(positionResponsePacket.latitudeXOffset) << "\n"
                        << "Y Offset: " << std::to_string(positionResponsePacket.longitudeYOffset) << "\n"
                        << "Z Offset: " << std::to_string(positionResponsePacket.altitudeZOffset) << "\n";
    }

    args.sDataMessage << "Roll: " << std::to_string(positionResponsePacket.roll) << "\n"
                      << "Pitch: " << std::to_string(positionResponsePacket.pitch) << "\n"
                      << "Yaw: " << std::to_string(positionResponsePacket.yaw);
    Event::Raise<HostCigiEvent>(args);
  }

  if (eCoordinateSystem == CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic)
  {
    SPositionResponseGeodeticCoordinates positionResponse;
    positionResponse.geodeticCoordinates.latitude = Latitude(positionResponsePacket.latitudeXOffset);
    positionResponse.geodeticCoordinates.longitude = Longitude(positionResponsePacket.longitudeYOffset);
    positionResponse.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(positionResponsePacket.altitudeZOffset);
    positionResponse.eObjectClass = ConvertPositionObjectClassV30(eObjectClass);
    positionResponse.objectID = positionResponsePacket.objectId;
    positionResponse.rotation.roll = Degrees(positionResponsePacket.roll);
    positionResponse.rotation.pitch = Degrees(positionResponsePacket.pitch);
    positionResponse.rotation.yaw = Degrees(positionResponsePacket.yaw);

    HostCigiPositionResponseEventArgs args(positionResponse);
    args.ePositionResponseType = EPositionResponseType::GEODETIC;
    Event::Raise<HostCigiEvent>(args);
  }
  else if (eCoordinateSystem == CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_ParentEntity)
  {
    SPositionResponseParentEntityCoordinates positionResponse;
    positionResponse.offset[0] = positionResponsePacket.latitudeXOffset;
    positionResponse.offset[1] = positionResponsePacket.longitudeYOffset;
    positionResponse.offset[2] = positionResponsePacket.altitudeZOffset;
    positionResponse.eObjectClass = ConvertPositionObjectClassV30(eObjectClass);
    positionResponse.objectID = positionResponsePacket.objectId;
    positionResponse.rotation.roll = Degrees(positionResponsePacket.roll);
    positionResponse.rotation.pitch = Degrees(positionResponsePacket.pitch);
    positionResponse.rotation.yaw = Degrees(positionResponsePacket.yaw);

    HostCigiPositionResponseEventArgs args(positionResponse);
    args.ePositionResponseType = EPositionResponseType::PARENT;
    Event::Raise<HostCigiEvent>(args);
  }
  else if (eCoordinateSystem == CIGI::V30::PositionResponse::CoordinateSystem::eCoordinateSystem_Submodel)
  {
    SPositionResponseArticulatedPartCoordinates positionResponse;
    positionResponse.offset[0] = positionResponsePacket.latitudeXOffset;
    positionResponse.offset[1] = positionResponsePacket.longitudeYOffset;
    positionResponse.offset[2] = positionResponsePacket.altitudeZOffset;
    positionResponse.articulatedPartID = ArticulatedPartID(positionResponsePacket.articulatedPartId);
    positionResponse.eObjectClass = ConvertPositionObjectClassV30(eObjectClass);
    positionResponse.objectID = positionResponsePacket.objectId;
    positionResponse.rotation.roll = Degrees(positionResponsePacket.roll);
    positionResponse.rotation.pitch = Degrees(positionResponsePacket.pitch);
    positionResponse.rotation.yaw = Degrees(positionResponsePacket.yaw);

    HostCigiPositionResponseEventArgs args(positionResponse);
    args.ePositionResponseType = EPositionResponseType::ARTICULATED;
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::ParseWeatherConditionsResponsePacket(uint8_t* buffer)
{
  CIGI::V33::WeatherConditionsResponse weatherConditionsResponsePacket;
  memcpy(&weatherConditionsResponsePacket, buffer, sizeof(CIGI::V33::WeatherConditionsResponse));

  if (m_bByteSwap)
  {
    weatherConditionsResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::WEATHER_CONDITIONS_RESPONSE) << "\n"
                    << "Request ID: " << std::to_string(weatherConditionsResponsePacket.requestId) << "\n"
                    << "Humidity: " << std::to_string(weatherConditionsResponsePacket.humidity) << "%\n"
                    << "Air Temperature: " << std::to_string(weatherConditionsResponsePacket.airTemperature) << "°C\n"
                    << "Visibility Range: " << std::to_string(weatherConditionsResponsePacket.visibilityRange) << "\n"
                    << "Horizontal Wind Speed: " << std::to_string(weatherConditionsResponsePacket.horizontalWindSpeed) << "\n"
                    << "Vertical Wind Speed: " << std::to_string(weatherConditionsResponsePacket.verticalWindSpeed) << "\n"
                    << "Wind Direction: " << std::to_string(weatherConditionsResponsePacket.windDirection) << "\n"
                    << "Barometric Pressure: " << std::to_string(weatherConditionsResponsePacket.barometricPressure);
  Event::Raise<HostCigiEvent>(args);

  HostCigiWeatherConditionsResponseEventArgs responseArgs;
  responseArgs.weatherConditionsResponse.requestID = weatherConditionsResponsePacket.requestId;
  responseArgs.weatherConditionsResponse.humidity = Percentage(static_cast<float>(weatherConditionsResponsePacket.humidity) / 100.0f);
  responseArgs.weatherConditionsResponse.fAirTemperature = weatherConditionsResponsePacket.airTemperature;
  responseArgs.weatherConditionsResponse.fVisibilityRange = weatherConditionsResponsePacket.visibilityRange;
  responseArgs.weatherConditionsResponse.windSpeedHorVer.horizontalWindSpeed = weatherConditionsResponsePacket.horizontalWindSpeed;
  responseArgs.weatherConditionsResponse.windSpeedHorVer.verticalWindSpeed = weatherConditionsResponsePacket.verticalWindSpeed;
  responseArgs.weatherConditionsResponse.fWindDirection = weatherConditionsResponsePacket.windDirection;
  responseArgs.weatherConditionsResponse.fBarometricPressure = weatherConditionsResponsePacket.barometricPressure;
  Event::Raise<HostCigiEvent>(responseArgs);
}

void CHostSessionV3_3::ParseAerosolConcentrationResponsePacket(uint8_t* buffer)
{
  CIGI::V33::AerosolConcentrationResponse aerosolConcentrationResponsePacket;
  memcpy(&aerosolConcentrationResponsePacket, buffer, sizeof(CIGI::V33::AerosolConcentrationResponse));

  if (m_bByteSwap)
  {
    aerosolConcentrationResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::AEROSOL_CONCENTRATION_RESPONSE) << "\n"
                    << "Request ID: " << std::to_string(aerosolConcentrationResponsePacket.requestId) << "\n"
                    << "Layer ID: " << std::to_string(aerosolConcentrationResponsePacket.layerId) << "\n"
                    << "Aerosol Concentration: " << std::to_string(aerosolConcentrationResponsePacket.aerosolConcentration);
  Event::Raise<HostCigiEvent>(args);

  HostCigiAerosolConcentrationResponseEventArgs responseArgs;
  responseArgs.aerosolConcentrationResponse.requestID = aerosolConcentrationResponsePacket.requestId;
  responseArgs.aerosolConcentrationResponse.layerID = aerosolConcentrationResponsePacket.layerId;
  responseArgs.aerosolConcentrationResponse.fAerosolConcentration = aerosolConcentrationResponsePacket.aerosolConcentration;
  Event::Raise<HostCigiEvent>(responseArgs);
}

void CHostSessionV3_3::ParseMaritimeSurfaceConditionsResponsePacket(uint8_t* buffer)
{
  CIGI::V33::MaritimeSurfaceConditionsResponse maritimeSurfaceConditionsResponsePacket;
  memcpy(&maritimeSurfaceConditionsResponsePacket, buffer, sizeof(CIGI::V33::MaritimeSurfaceConditionsResponse));

  if (m_bByteSwap)
  {
    maritimeSurfaceConditionsResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::MARITIME_SURFACE_CONDITIONS_RESPONSE) << "\n"
                    << "Request ID: " << std::to_string(maritimeSurfaceConditionsResponsePacket.requestId) << "\n"
                    << "Sea Surface Height: " << std::to_string(maritimeSurfaceConditionsResponsePacket.seaSurfaceHeight) << "\n"
                    << "Surface Water Temperature: " << std::to_string(maritimeSurfaceConditionsResponsePacket.surfaceWaterTemperature) << "\n"
                    << "Surface Clarity: " << std::to_string(maritimeSurfaceConditionsResponsePacket.surfaceClarity) << "%";
  Event::Raise<HostCigiEvent>(args);

  HostCigiMaritimeSurfaceConditionsResponseEventArgs responseArgs;
  responseArgs.maritimeSurfaceConditionsResponse.requestID = maritimeSurfaceConditionsResponsePacket.requestId;
  responseArgs.maritimeSurfaceConditionsResponse.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(maritimeSurfaceConditionsResponsePacket.seaSurfaceHeight);
  responseArgs.maritimeSurfaceConditionsResponse.fSurfaceWaterTemperature = TemperatureCelsius(maritimeSurfaceConditionsResponsePacket.surfaceWaterTemperature);
  responseArgs.maritimeSurfaceConditionsResponse.surfaceClarity = Percentage(maritimeSurfaceConditionsResponsePacket.surfaceClarity / 100.0f);
  Event::Raise<HostCigiEvent>(responseArgs);
}

void CHostSessionV3_3::ParseTerrestrialSurfaceConditionsResponsePacket(uint8_t* buffer)
{
  CIGI::V33::TerrestrialSurfaceConditionsResponse terrestrialSurfaceConditionsResponsePacket;
  memcpy(&terrestrialSurfaceConditionsResponsePacket, buffer, sizeof(CIGI::V33::TerrestrialSurfaceConditionsResponse));

  if (m_bByteSwap)
  {
    terrestrialSurfaceConditionsResponsePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE) << "\n"
                    << "Request ID: " << std::to_string(terrestrialSurfaceConditionsResponsePacket.requestId) << "\n"
                    << "Surface Condition ID: " << std::to_string(terrestrialSurfaceConditionsResponsePacket.surfaceConditionId) << "\n";
  Event::Raise<HostCigiEvent>(args);

  HostCigiTerrestrialSurfaceConditionsResponseEventArgs responseArgs;
  responseArgs.terrestrialSurfaceConditionsResponse.requestID = terrestrialSurfaceConditionsResponsePacket.requestId;
  responseArgs.terrestrialSurfaceConditionsResponse.surfaceConditionID = terrestrialSurfaceConditionsResponsePacket.surfaceConditionId;
  Event::Raise<HostCigiEvent>(responseArgs);
}

void CHostSessionV3_3::ParseCollisionDetectionSegmentNotificationPacket(uint8_t* buffer)
{
  CIGI::V33::CollisionDetectionSegmentNotification collisionDetectionSegmentNotificationPacket;
  memcpy(&collisionDetectionSegmentNotificationPacket, buffer, sizeof(CIGI::V33::CollisionDetectionSegmentNotification));

  if (m_bByteSwap)
  {
    collisionDetectionSegmentNotificationPacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_NOTIFICATION) << "\n"
                       << "Entity ID: " << std::to_string(collisionDetectionSegmentNotificationPacket.entityId) << "\n"
                       << "Contacted Entity ID: " << std::to_string(collisionDetectionSegmentNotificationPacket.contactedEntityId) << "\n"
                       << "Segment ID: " << std::to_string(collisionDetectionSegmentNotificationPacket.segmentId) << "\n";
  if (collisionDetectionSegmentNotificationPacket.collisionType == 0)
  {
    msgArgs.sDataMessage << "Collision Type: Non-Entity\n";
  }
  else
  {
    msgArgs.sDataMessage << "Collision Type: Entity\n";
  }
  msgArgs.sDataMessage << "Material Code: " << std::to_string(collisionDetectionSegmentNotificationPacket.materialCode) << "\n"
                       << "Intersection Distance: " << std::to_string(collisionDetectionSegmentNotificationPacket.intersectionDistance);
  Event::Raise<HostCigiEvent>(msgArgs);

  if (collisionDetectionSegmentNotificationPacket.collisionType == 0)
  {
    HostCigiCollisionDetectionSegmentNotificationEventArgs collisionSegmentArgs;
    collisionSegmentArgs.collisionDetectionSegmentNotification.entityID = EntityID(collisionDetectionSegmentNotificationPacket.entityId);
    collisionSegmentArgs.collisionDetectionSegmentNotification.segmentID = SegmentID(collisionDetectionSegmentNotificationPacket.segmentId);
    collisionSegmentArgs.collisionDetectionSegmentNotification.fIntersectionDistance = collisionDetectionSegmentNotificationPacket.intersectionDistance;
    collisionSegmentArgs.collisionDetectionSegmentNotification.materialCode = MaterialID(collisionDetectionSegmentNotificationPacket.materialCode);
    Event::Raise<HostCigiEvent>(collisionSegmentArgs);
  }
  else if (collisionDetectionSegmentNotificationPacket.collisionType == 1)
  {
    HostCigiCollisionDetectionSegmentEntityNotificationEventArgs collisionSegmentArgs;
    collisionSegmentArgs.collisionDetectionSegmentEntityNotification.entityID = EntityID(collisionDetectionSegmentNotificationPacket.entityId);
    collisionSegmentArgs.collisionDetectionSegmentEntityNotification.segmentID = SegmentID(collisionDetectionSegmentNotificationPacket.segmentId);
    collisionSegmentArgs.collisionDetectionSegmentEntityNotification.fIntersectionDistance = collisionDetectionSegmentNotificationPacket.intersectionDistance;
    collisionSegmentArgs.collisionDetectionSegmentEntityNotification.materialCode = MaterialID(collisionDetectionSegmentNotificationPacket.materialCode);
    collisionSegmentArgs.collisionDetectionSegmentEntityNotification.contactedEntityID = EntityID(collisionDetectionSegmentNotificationPacket.contactedEntityId);
    Event::Raise<HostCigiEvent>(collisionSegmentArgs);
  }
}

void CHostSessionV3_3::ParseCollisionDetectionVolumeNotificationPacket(uint8_t* buffer)
{
  CIGI::V33::CollisionDetectionVolumeNotification collisionDetectionVolumeNotificationPacket;
  memcpy(&collisionDetectionVolumeNotificationPacket, buffer, sizeof(CIGI::V33::CollisionDetectionVolumeNotification));

  if (m_bByteSwap)
  {
    collisionDetectionVolumeNotificationPacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_NOTIFICATION) << "\n"
                       << "Entity ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.entityId) << "\n"
                       << "Contacted Entity ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.contactedEntityId) << "\n"
                       << "Volume ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.volumeId) << "\n";
  if (collisionDetectionVolumeNotificationPacket.collisionType == 0)
  {
    msgArgs.sDataMessage << "Collision Type: Non-Entity\n";
  }
  else
  {
    msgArgs.sDataMessage << "Collision Type: Entity\n";
  }
  msgArgs.sDataMessage << "Contacted Volume ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.contactedVolumeId);
  Event::Raise<HostCigiEvent>(msgArgs);

  if (collisionDetectionVolumeNotificationPacket.collisionType == 0)
  {
    HostCigiCollisionDetectionVolumeNotificationEventArgs collisionVolumeArgs;
    collisionVolumeArgs.collisionDetectionVolumeNotification.entityID = EntityID(collisionDetectionVolumeNotificationPacket.entityId);
    collisionVolumeArgs.collisionDetectionVolumeNotification.volumeID = VolumeID(collisionDetectionVolumeNotificationPacket.volumeId);
    collisionVolumeArgs.collisionDetectionVolumeNotification.contactedVolumeID = VolumeID(collisionDetectionVolumeNotificationPacket.contactedVolumeId);
    Event::Raise<HostCigiEvent>(collisionVolumeArgs);
  }
  else if (collisionDetectionVolumeNotificationPacket.collisionType == 1)
  {
    HostCigiCollisionDetectionVolumeEntityNotificationEventArgs collisionVolumeArgs;
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.entityID = EntityID(collisionDetectionVolumeNotificationPacket.entityId);
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.volumeID = VolumeID(collisionDetectionVolumeNotificationPacket.volumeId);
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.contactedEntityID = EntityID(collisionDetectionVolumeNotificationPacket.contactedEntityId);
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.contactedVolumeID = VolumeID(collisionDetectionVolumeNotificationPacket.contactedVolumeId);
    Event::Raise<HostCigiEvent>(collisionVolumeArgs);
  }
}

void CHostSessionV3_3::ParseAnimationStopNotificationPacket(uint8_t* buffer)
{
  CIGI::V33::AnimationStopNotification animationStopNotificationPacket;
  memcpy(&animationStopNotificationPacket, buffer, sizeof(CIGI::V33::AnimationStopNotification));

  if (m_bByteSwap)
  {
    animationStopNotificationPacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::ANIMATION_STOP_NOTIFICATION) << "\n"
                    << "Entity ID: " << std::to_string(animationStopNotificationPacket.entityId);
  Event::Raise<HostCigiEvent>(args);

  HostCigiAnimationStopNotificationEventArgs animationArgs;
  animationArgs.animationStopNotification.entityID = EntityID(animationStopNotificationPacket.entityId);
  Event::Raise<HostCigiEvent>(animationArgs);
}

void CHostSessionV3_3::ParseEventNotificationPacket(uint8_t* buffer)
{
  CIGI::V33::EventNotification eventNotificationPacket;
  memcpy(&eventNotificationPacket, buffer, sizeof(CIGI::V33::EventNotification));

  if (m_bByteSwap)
  {
    eventNotificationPacket.doByteSwapping();
  }

  HostCigiEventNotificationEventArgs eventArgs;
  eventArgs.eventID = eventNotificationPacket.eventId;
  eventArgs.eventData[0] = eventNotificationPacket.eventData1;
  eventArgs.eventData[1] = eventNotificationPacket.eventData2;
  eventArgs.eventData[2] = eventNotificationPacket.eventData3;
  Event::Raise<HostCigiEvent>(eventArgs);
}

void CHostSessionV3_3::ParseImageGeneratorMessagePacket(uint8_t* buffer)
{
  SCigiPacketHeaderV3 packetHeader;
  memcpy(&packetHeader, buffer, sizeof(SCigiPacketHeaderV3));

  CIGI::V33::IGMessage imageGeneratorMessagePacket = {};
  int nPacketBytesToCopy = static_cast<int>(packetHeader.nPacketSize);
  if (nPacketBytesToCopy > static_cast<int>(sizeof(CIGI::V33::IGMessage)))
  {
    nPacketBytesToCopy = sizeof(CIGI::V33::IGMessage);
  }
  memcpy(&imageGeneratorMessagePacket, buffer, nPacketBytesToCopy);

  if (m_bByteSwap)
  {
    imageGeneratorMessagePacket.doByteSwapping();
  }

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV3::IMAGE_GENERATOR_MESSAGE);
  args.sDataMessage << "\nImage Generator Message ID " << imageGeneratorMessagePacket.messageId;

  Event::Raise<HostCigiEvent>(args);

  HostCigiImageGeneratorMessageEventArgs messageArgs;
  messageArgs.messageID = imageGeneratorMessagePacket.messageId;
  int nMessageLength = static_cast<int>(packetHeader.nPacketSize) - static_cast<int>(CIGI::V33::IGMessage::kBasePacketSize);
  if (nMessageLength > 0)
  {
    messageArgs.sMessage.assign(reinterpret_cast<const char*>(buffer) + CIGI::V33::IGMessage::kBasePacketSize, nMessageLength);
  }
  Event::Raise<HostCigiEvent>(messageArgs);
}

int CHostSessionV3_3::ProcessPacket(uint8_t* buffer, int nRemainingBytes)
{
  if (nRemainingBytes < static_cast<int>(sizeof(SCigiPacketHeaderV3)))
  {
    return 0;
  }

  SCigiPacketHeaderV3 packetHeader;
  memcpy(&packetHeader, buffer, sizeof(SCigiPacketHeaderV3));

  if (packetHeader.nPacketSize < sizeof(SCigiPacketHeaderV3) || packetHeader.nPacketSize > nRemainingBytes)
  {
    return 0;
  }

  auto itFunction = m_PacketHandlerFunctions.find(packetHeader.eOpCode);

  if (itFunction == m_PacketHandlerFunctions.end())
  {
    HostCigiMessageEventArgs args;
    args.sMessage = "Received ";
    args.sMessage += GetCigiOpCodeName(packetHeader.eOpCode);
    Event::Raise<HostCigiEvent>(args);
  }
  else
  {
    int nMinimumPacketSize = GetMinimumIncomingPacketSizeV3(packetHeader.eOpCode);
    if (nMinimumPacketSize > 0 && packetHeader.nPacketSize < nMinimumPacketSize)
    {
      HostCigiErrorEventArgs args;
      args.sError = "Received undersized " + GetCigiOpCodeName(packetHeader.eOpCode) + " packet.";
      Event::Raise<HostCigiEvent>(args);
      return packetHeader.nPacketSize;
    }

    auto pFunction = itFunction->second;
    (this->*pFunction)(buffer);
  }

  return packetHeader.nPacketSize;
}

int CHostSessionV3_3::GetMinimumIncomingPacketSizeV3(ECigiOpCodeV3 eOpCode) const
{
  switch (eOpCode)
  {
  case ECigiOpCodeV3::START_OF_FRAME:
    return sizeof(CIGI::V33::SoF);
  case ECigiOpCodeV3::HAT_HOT_RESPONSE:
    return sizeof(CIGI::V33::HATHOTResponse);
  case ECigiOpCodeV3::HAT_HOT_EXTENDED_RESPONSE:
    return sizeof(CIGI::V33::HATHOTExtendedResponse);
  case ECigiOpCodeV3::LINE_OF_SIGHT_RESPONSE:
    return sizeof(CIGI::V33::LineOfSightResponse);
  case ECigiOpCodeV3::LINE_OF_SIGHT_EXTENDED_RESPONSE:
    return sizeof(CIGI::V33::LineOfSightExtendedResponse);
  case ECigiOpCodeV3::SENSOR_RESPONSE:
    return sizeof(CIGI::V33::SensorResponse);
  case ECigiOpCodeV3::SENSOR_EXTENDED_RESPONSE:
    return sizeof(CIGI::V33::SensorExtendedResponse);
  case ECigiOpCodeV3::POSITION_RESPONSE:
    return sizeof(CIGI::V33::PositionResponse);
  case ECigiOpCodeV3::WEATHER_CONDITIONS_RESPONSE:
    return sizeof(CIGI::V33::WeatherConditionsResponse);
  case ECigiOpCodeV3::AEROSOL_CONCENTRATION_RESPONSE:
    return sizeof(CIGI::V33::AerosolConcentrationResponse);
  case ECigiOpCodeV3::MARITIME_SURFACE_CONDITIONS_RESPONSE:
    return sizeof(CIGI::V33::MaritimeSurfaceConditionsResponse);
  case ECigiOpCodeV3::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE:
    return sizeof(CIGI::V33::TerrestrialSurfaceConditionsResponse);
  case ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_NOTIFICATION:
    return sizeof(CIGI::V33::CollisionDetectionSegmentNotification);
  case ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_NOTIFICATION:
    return sizeof(CIGI::V33::CollisionDetectionVolumeNotification);
  case ECigiOpCodeV3::ANIMATION_STOP_NOTIFICATION:
    return sizeof(CIGI::V33::AnimationStopNotification);
  case ECigiOpCodeV3::EVENT_NOTIFICATION:
    return sizeof(CIGI::V33::EventNotification);
  case ECigiOpCodeV3::IMAGE_GENERATOR_MESSAGE:
    return CIGI::V33::IGMessage::kBasePacketSize;
  default:
    return 0;
  }
}

int CHostSessionV3_3::GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const
{
  if (nRemainingBytes < static_cast<int>(sizeof(SCigiPacketHeaderV3)))
  {
    return 0;
  }

  SCigiPacketHeaderV3 packetHeader;
  memcpy(&packetHeader, buffer, sizeof(SCigiPacketHeaderV3));

  if (packetHeader.nPacketSize < sizeof(SCigiPacketHeaderV3) || packetHeader.nPacketSize > nRemainingBytes)
  {
    return 0;
  }

  return packetHeader.nPacketSize;
}

void CHostSessionV3_3::Reset()
{
  CHostSession::Reset();

  m_Entities.clear();
}

void CHostSessionV3_3::SendIGControl(uint8_t*& pBuffer)
{
  try
  {
    m_IGControl.hostFrameNumber = m_HostFrameNumber.Value();
    m_IGControl.lastIGFrameNumber = m_LastReceivedIGFrame.Value();
    m_IGControl.timestamp = 0;
    m_IGControl.timestampValid = true;
    m_IGControl.byteSwapMagicNumber = m_bByteSwap ? CIGI::V33::IGCtrl::kByteSwap : CIGI::V33::IGCtrl::kNoByteSwap;

    if (m_eDatabaseState == EHostSessionDatabaseState::NO_DATABASE)
    {
      m_IGControl.dbNumber = 0;
    }
    else if (m_eDatabaseState == EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED)
    {
      m_IGControl.dbNumber = static_cast<int8_t>(m_DatabaseNumber.Value());
    }
    else if (m_eDatabaseState == EHostSessionDatabaseState::LOADING_ACKNOWLEDGED)
    {
      m_IGControl.dbNumber = 0;
    }
    else if (m_eDatabaseState == EHostSessionDatabaseState::LOADED)
    {
      m_IGControl.dbNumber = 0;
    }
    else if (m_eDatabaseState == EHostSessionDatabaseState::IG_CONTROLLED)
    {
      m_IGControl.dbNumber = 0;
    }

    CIGI::V33::IGCtrl tempIGControl;
    tempIGControl = m_IGControl;
    if (m_bByteSwap)
    {
      tempIGControl.doByteSwapping();
    }

    memcpy(pBuffer, &tempIGControl, sizeof(CIGI::V33::IGCtrl));
    pBuffer += sizeof(CIGI::V33::IGCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendArticulatedPartControl(const sbio::cigi::SCigiArticulatedPart& articulatedPartControl)
{
  try
  {
    CIGI::V33::ArticulatedPartCtrl artPartCtrl;
    artPartCtrl.entityId = articulatedPartControl.entityID.Value();
    artPartCtrl.articulatedPartId = articulatedPartControl.articulatedPartID.Value();
    artPartCtrl.articulatedPartEnable = articulatedPartControl.bEnabled;
    artPartCtrl.xOffsetEnable = articulatedPartControl.bOffsetEnabled[0];
    artPartCtrl.yOffsetEnable = articulatedPartControl.bOffsetEnabled[1];
    artPartCtrl.zOffsetEnable = articulatedPartControl.bOffsetEnabled[2];
    artPartCtrl.rollEnable = articulatedPartControl.bRollEnabled;
    artPartCtrl.pitchEnable = articulatedPartControl.bPitchEnabled;
    artPartCtrl.yawEnable = articulatedPartControl.bYawEnabled;
    artPartCtrl.xOffset = static_cast<float>(articulatedPartControl.offset[0]);
    artPartCtrl.yOffset = static_cast<float>(articulatedPartControl.offset[1]);
    artPartCtrl.zOffset = static_cast<float>(articulatedPartControl.offset[2]);
    artPartCtrl.roll = articulatedPartControl.rotation.roll.Value();
    artPartCtrl.pitch = articulatedPartControl.rotation.pitch.Value();
    artPartCtrl.yaw = articulatedPartControl.rotation.yaw.Value();

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::ARTICULATED_PART_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(articulatedPartControl.entityID.Value()) << "\n"
                        << "Articulated Part ID: " << std::to_string(articulatedPartControl.articulatedPartID.Value()) << "\n"
                        << "Articulated Part Enable: " << EnableToString(articulatedPartControl.bEnabled) << "\n"
                        << "X Offset Enable: " << EnableToString(articulatedPartControl.bOffsetEnabled[0]) << "\n"
                        << "Y Offset Enable: " << EnableToString(articulatedPartControl.bOffsetEnabled[1]) << "\n"
                        << "Z Offset Enable: " << EnableToString(articulatedPartControl.bOffsetEnabled[2]) << "\n"
                        << "Roll Enable: " << EnableToString(articulatedPartControl.bRollEnabled) << "\n"
                        << "Pitch Enable: " << EnableToString(articulatedPartControl.bPitchEnabled) << "\n"
                        << "Yaw Enable: " << EnableToString(articulatedPartControl.bYawEnabled) << "\n"
                        << "X Offset: " << std::to_string(static_cast<float>(articulatedPartControl.offset[0])) << "\n"
                        << "Y Offset: " << std::to_string(static_cast<float>(articulatedPartControl.offset[1])) << "\n"
                        << "Z Offset: " << std::to_string(static_cast<float>(articulatedPartControl.offset[2])) << "\n"
                        << "Roll: " << std::to_string(articulatedPartControl.rotation.fRoll.Value()) << "\n"
                        << "Pitch: " << std::to_string(articulatedPartControl.rotation.fPitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(articulatedPartControl.rotation.fYaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      artPartCtrl.doByteSwapping();
    }

    Pack(&artPartCtrl, sizeof(CIGI::V33::ArticulatedPartCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendTrajectoryDefinition(const sbio::cigi::SCigiEntityAcceleration& trajectoryDefinition)
{
  // Can use SCigiAcceleration for Trajectory Definition since they are the same
  try
  {
    CIGI::V33::TrajectoryDefinition trajectoryDef;
    trajectoryDef.entityId = trajectoryDefinition.entityID.Value();
    trajectoryDef.accelerationX = static_cast<float>(trajectoryDefinition.linearAcceleration[0]);
    trajectoryDef.accelerationY = static_cast<float>(trajectoryDefinition.linearAcceleration[1]);
    trajectoryDef.accelerationZ = static_cast<float>(trajectoryDefinition.linearAcceleration[2]);
    trajectoryDef.retardationRate = 0.0f;// removed in V4. not supported.
    trajectoryDef.terminalVelocity = 0.0f;// removed in V4. not supported.

    if (m_bByteSwap)
    {
      trajectoryDef.doByteSwapping();
    }

    Pack(&trajectoryDef, sizeof(CIGI::V33::TrajectoryDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendAtmosphereControl(const sbio::cigi::SCigiAtmosphereControl& atmosphereControl)
{
  try
  {
    CIGI::V33::AtmosphereCtrl atmosphereCtrl;
    atmosphereCtrl.atmosphericModelEnable = atmosphereControl.bAtmosphereModelEnable;
    atmosphereCtrl.globalHumidity = ConvertToCigiIntPercentage(atmosphereControl.globalHumidity).Value();
    atmosphereCtrl.globalAirTemperature = atmosphereControl.fGlobalAirTemp;
    atmosphereCtrl.globalVisibilityRange = atmosphereControl.fGlobalVisibility;
    atmosphereCtrl.globalHorizontalWindSpeed = atmosphereControl.fGlobalHorizontalWindSpeed;
    atmosphereCtrl.globalVerticalWindSpeed = atmosphereControl.fGlobalVerticalWindSpeed;
    atmosphereCtrl.globalWindDirection = atmosphereControl.globalWindDirection.Value();
    atmosphereCtrl.globalBarometricPressure = atmosphereControl.fGlobalBarometricPressure;

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::ATMOSPHERE_CONTROL) << "\n"
                        << "Atmospheric Model Enable: " << EnableToString(atmosphereControl.bAtmosphereModelEnable) << "\n"
                        << "Global Humidity: " << std::to_string(ConvertToCigiIntPercentage(atmosphereControl.globalHumidity).Value()) << "%" << "\n"
                        << "Global Air Temperature: " << std::to_string(atmosphereControl.fGlobalAirTemp) << "°C" << "\n"
                        << "Global Visibility Range: " << std::to_string(atmosphereControl.fGlobalVisibility) << "\n"
                        << "Global Horizontal Wind Speed: " << std::to_string(atmosphereControl.fGlobalHorizontalWindSpeed) << "\n"
                        << "Global Vertical Wind Speed: " << std::to_string(atmosphereControl.fGlobalVerticalWindSpeed) << "\n"
                        << "Global Wind Direction: " << std::to_string(atmosphereControl.globalWindDirection.Value()) << "\n"
                        << "Global Barometric Pressure: " << std::to_string(atmosphereControl.fGlobalHorizontalWindSpeed);
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      atmosphereCtrl.doByteSwapping();
    }

    Pack(&atmosphereCtrl, sizeof(CIGI::V33::AtmosphereCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendCelestialSphereControl(const sbio::cigi::SCigiCelestialSphereControl& celestialSphereControl)
{
  try
  {
    CIGI::V33::CelestialSphereCtrl celestialCtrl;
    celestialCtrl.hour = celestialSphereControl.hour.Value();
    celestialCtrl.minute = celestialSphereControl.minute.Value();
    celestialCtrl.continuousTimeOfDayEnable = celestialSphereControl.bContinuousTimeOfDayEnable;
    celestialCtrl.sunEnable = celestialSphereControl.bSunEnable;
    celestialCtrl.moonEnable = celestialSphereControl.bMoonEnable;
    celestialCtrl.starFieldEnable = celestialSphereControl.bStarFieldEnable;
    celestialCtrl.starFieldIntensity = ConvertToCigiFloatPercentage(celestialSphereControl.starFieldIntensity).Value();
    celestialCtrl.dateTimeValid = celestialSphereControl.bDateTimeValid;
    celestialCtrl.setDate(celestialSphereControl.year.Value(), ConvertToCigiMonth(celestialSphereControl.month).Value(), celestialSphereControl.day.Value());

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::CELESTIAL_SPHERE_CONTROL) << "\n"
                        << "Hour: " << std::to_string(celestialSphereControl.hour.Value()) << "\n"
                        << "Minute: " << std::to_string(celestialSphereControl.minute.Value()) << "\n"
                        << "Ephemeris Model Enable: " << EnableToString(celestialSphereControl.bContinuousTimeOfDayEnable) << "\n"
                        << "Sun Enable: " << EnableToString(celestialSphereControl.bSunEnable) << "\n"
                        << "Moon Enable: " << EnableToString(celestialSphereControl.bMoonEnable) << "\n"
                        << "Star Field Enable: " << EnableToString(celestialSphereControl.bStarFieldEnable) << "\n"
                        << "Date/Time Valid: " << ValidToString(celestialSphereControl.bDateTimeValid) << "\n"
                        << "Year: " << std::to_string(celestialSphereControl.year.Value()) << "\n"
                        << "Month: " << std::to_string(ConvertToCigiMonth(celestialSphereControl.month).Value()) << "\n"
                        << "Day: " << std::to_string(celestialSphereControl.day.Value()) << "\n"
                        << "Star Field Intensity: " << std::to_string(ConvertToCigiFloatPercentage(celestialSphereControl.starFieldIntensity).Value()) << "%" << "\n";
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      celestialCtrl.doByteSwapping();
    }

    Pack(&celestialCtrl, sizeof(CIGI::V33::CelestialSphereCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendChildEntityPosition(const sbio::cigi::SChildEntityPosition& childEntityPosition)
{
  try
  {
    SEntityControl entityControl;

    auto it = m_Entities.find(childEntityPosition.entityID);
    if (it == m_Entities.end())
    {
      return;
    }
    else
    {
      entityControl = it->second;
    }

    CIGI::V33::EntityCtrl entityCtrl;
    entityCtrl.entityId = entityControl.entityID.Value();
    entityCtrl.entityState = ConvertToEntityState(entityControl.eState);
    entityCtrl.attachState = ((entityControl.bHasParent) ? CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached : CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached);
    entityCtrl.collisionReportEnable = entityControl.bCollisionReportingEnabled;
    entityCtrl.inheritAlpha = entityControl.bInheritAlpha;

    // need to setup: Ground/Ocean Clamp, Animation Direction, Animation Loop Mode, and Animation State
    // entityCtrl.SetGrndClamp();
    // entityCtrl.SetAnimationDir();
    // entityCtrl.SetAnimationLoopMode();
    // entityCtrl.SetAnimationState();

    // Smoothing is te same as Linear Extrapolation/Interpolation Enable
    entityCtrl.extrapolationEnable = entityControl.bSmoothingEnabled;
    entityCtrl.alpha = entityControl.alpha;

    // For Entity type Extended and short do not exist it is just a uint16_t
    entityCtrl.entityType = entityControl.shortEntityTypeID.Value();

    entityCtrl.parentId = entityControl.parentID.Value();

    // Roll, Pitch, Yaw
    entityCtrl.roll = childEntityPosition.rotation.roll.Value();
    entityCtrl.pitch = childEntityPosition.rotation.pitch.Value();
    entityCtrl.yaw = childEntityPosition.rotation.yaw.Value();

    if (entityControl.bHasParent)
    {
      entityCtrl.latitudeXOffset = childEntityPosition.offset[0];
      entityCtrl.longitudeYOffset = childEntityPosition.offset[1];
      entityCtrl.altitudeZOffset = childEntityPosition.offset[2];
    }

    if (m_bByteSwap)
    {
      entityCtrl.doByteSwapping();
    }

    Pack(&entityCtrl, sizeof(CIGI::V33::EntityCtrl));
  }
  catch (const std::exception& ex)
  {
    std::cout << "Error sending Entity Control packet" << std::endl;
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendCollisionDetectionSegment(const sbio::cigi::SCollisionDetectionSegmentDefinition& collDetSegment)
{
  try
  {
    CIGI::V33::CollisionDetectionSegmentDefinition collDetSeg;
    collDetSeg.entityId = collDetSegment.entityID.Value();
    collDetSeg.segmentId = collDetSegment.segmentID.Value();
    collDetSeg.segmentEnable = collDetSegment.bSegmentEnabled;
    collDetSeg.x1 = (float)collDetSegment.beg[0];
    collDetSeg.y1 = (float)collDetSegment.beg[1];
    collDetSeg.z1 = (float)collDetSegment.beg[2];
    collDetSeg.x2 = (float)collDetSegment.end[0];
    collDetSeg.y2 = (float)collDetSegment.end[1];
    collDetSeg.z2 = (float)collDetSegment.end[2];
    collDetSeg.materialMask = collDetSegment.nMaterialMask;

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_DEFINITION) << "\n"
                        << "Entity ID: " << std::to_string(collDetSegment.entityID.Value()) << "\n"
                        << "Segment ID: " << std::to_string(collDetSegment.segmentID.Value()) << "\n"
                        << "Segment Enable: " << EnableToString(collDetSegment.bSegmentEnabled) << "\n"
                        << "X1: " << std::to_string(collDetSegment.beg[0]) << "\n"
                        << "Y1: " << std::to_string(collDetSegment.beg[1]) << "\n"
                        << "Z1: " << std::to_string(collDetSegment.beg[2]) << "\n"
                        << "X2: " << std::to_string(collDetSegment.end[0]) << "\n"
                        << "Y2: " << std::to_string(collDetSegment.end[1]) << "\n"
                        << "Z2: " << std::to_string(collDetSegment.end[2]) << "\n"
                        << "Material Mask: " << std::to_string(collDetSegment.nMaterialMask);
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      collDetSeg.doByteSwapping();
    }

    Pack(&collDetSeg, sizeof(CIGI::V33::CollisionDetectionSegmentDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendCollisionDetectionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collVolCuboid)
{
  try
  {
    CIGI::V33::CollisionDetectionVolumeDefinition collDetVol;
    collDetVol.volumeId = collVolCuboid.volumeID.Value();
    collDetVol.volumeEnable = collVolCuboid.bVolumeEnabled;
    collDetVol.volumeType = CIGI::V33::CollisionDetectionVolumeDefinition::Type::eType_Cuboid;
    collDetVol.entityId = collVolCuboid.entityID.Value();
    collDetVol.x = static_cast<float>(collVolCuboid.offset[0]);
    collDetVol.y = static_cast<float>(collVolCuboid.offset[1]);
    collDetVol.z = static_cast<float>(collVolCuboid.offset[2]);

    collDetVol.heightRadius = collVolCuboid.fHeight;

    collDetVol.width = collVolCuboid.fWidth;
    collDetVol.depth = collVolCuboid.fDepth;
    collDetVol.roll = collVolCuboid.rotation.roll.Value();
    collDetVol.pitch = collVolCuboid.rotation.pitch.Value();
    collDetVol.yaw = collVolCuboid.rotation.yaw.Value();

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_DEFINITION) << "\n"
                        << "Entity ID: " << std::to_string(collVolCuboid.entityID.Value()) << "\n"
                        << "Volume ID: " << std::to_string(collVolCuboid.volumeID.Value()) << "\n"
                        << "Volume Enable: " << EnableToString(collVolCuboid.bVolumeEnabled) << "\n"
                        << "Volume Type: " << "Cuboid" << "\n"
                        << "X: " << std::to_string(collVolCuboid.offset[0]) << "\n"
                        << "Y: " << std::to_string(collVolCuboid.offset[1]) << "\n"
                        << "Z: " << std::to_string(collVolCuboid.offset[2]) << "\n";

      args.sDataMessage << "Height: " << std::to_string(collVolCuboid.fHeight) << "\n";

      args.sDataMessage << "Width: " << std::to_string(collVolCuboid.fWidth) << "\n"
                        << "Depth: " << std::to_string(collVolCuboid.fDepth) << "\n"
                        << "Roll: " << std::to_string(collVolCuboid.fRoll.Value()) << "\n"
                        << "Pitch: " << std::to_string(collVolCuboid.fPitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(collVolCuboid.fYaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      collDetVol.doByteSwapping();
    }

    Pack(&collDetVol, sizeof(CIGI::V33::CollisionDetectionVolumeDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendCollisionDetectionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collVolSphere)
{
  try
  {
    CIGI::V33::CollisionDetectionVolumeDefinition collDetVol;
    collDetVol.volumeId = collVolSphere.volumeID.Value();
    collDetVol.volumeEnable = collVolSphere.bVolumeEnabled;
    collDetVol.volumeType = CIGI::V33::CollisionDetectionVolumeDefinition::Type::eType_Sphere;
    collDetVol.entityId = collVolSphere.entityID.Value();
    collDetVol.x = static_cast<float>(collVolSphere.offset[0]);
    collDetVol.y = static_cast<float>(collVolSphere.offset[1]);
    collDetVol.z = static_cast<float>(collVolSphere.offset[2]);

    collDetVol.heightRadius = collVolSphere.fRadius;

    if (m_bByteSwap)
    {
      collDetVol.doByteSwapping();
    }

    Pack(&collDetVol, sizeof(CIGI::V33::CollisionDetectionVolumeDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V33::ComponentCtrl::Class ConvertToCompClass(uint32_t classValue)
{
  if (classValue == 0)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_Entity;
  }
  else if (classValue == 1)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_View;
  }
  else if (classValue == 2)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_ViewGroup;
  }
  else if (classValue == 3)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_Sensor;
  }
  else if (classValue == 4)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_RegionalSeaSurface;
  }
  else if (classValue == 5)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_RegionalTerrainSurface;
  }
  else if (classValue == 6)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_RegionalLayeredSurface;
  }
  else if (classValue == 7)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_GlobalSeaSurface;
  }
  else if (classValue == 8)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_GlobalTerrainSurface;
  }
  else if (classValue == 9)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_GlobalLayeredSurface;
  }
  else if (classValue == 10)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_Atmosphere;
  }
  else if (classValue == 11)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_CelestialSphere;
  }
  else if (classValue == 12)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_Event;
  }
  else if (classValue == 13)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_System;
  }
  else if (classValue == 14)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_SymbolSurface;
  }
  else if (classValue == 15)
  {
    return CIGI::V33::ComponentCtrl::Class::eClass_Symbol;
  }
  return CIGI::V33::ComponentCtrl::Class::eClass_Reserved;
}

void CHostSessionV3_3::SendComponentControl(const sbio::cigi::SCigiComponentControl& componentControl)
{
  try
  {
    CIGI::V33::ComponentCtrl componentCtrl;
    componentCtrl.componentId = componentControl.key.componentID.Value();
    componentCtrl.instanceId = componentControl.key.nInstanceID;
    componentCtrl.componentClass = ConvertToCompClass(componentControl.key.componentClassID.Value());
    componentCtrl.componentState = componentControl.state.nComponentState;

    for (int n = 0; n < 6; ++n)
    {
      componentCtrl.componentData[n] = componentControl.state.componentData[n];
    }

    if (m_bByteSwap)
    {
      componentCtrl.doByteSwapping();
    }

    Pack(&componentCtrl, sizeof(CIGI::V33::ComponentCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V33::EntityCtrl::AnimationState ConvertToAnimationState(EAnimationState animState)
{
  if (animState == EAnimationState::PLAY)
  {
    return CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Play;
  }
  else if (animState == EAnimationState::STOP)
  {
    return CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Stop;
  }
  return CIGI::V33::EntityCtrl::AnimationState::eAnimationState_Stop;
}

CIGI::V33::EntityCtrl::GroundClamp ConvertToClamp(sbio::EClamp eClamp)
{
  switch (eClamp)
  {
  default:
  case EClamp::UNKNOWN:
  {
    return CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_NoClamp;
  }
  case EClamp::NONE:
  {
    return CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_NoClamp;
  }
  case EClamp::CONFORMAL:
  {
    return CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_Conformal;
  }
  case EClamp::NON_CONFORMAL:
  {
    return CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_NonConformal;
  }
  }
}

void CHostSessionV3_3::SendTopLevelEntityControl(const sbio::cigi::SEntityControl& entityControl, const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition, const sbio::cigi::SCigiAnimationControl& animationControl)
{
  try
  {
    m_Entities[entityControl.entityID] = entityControl;

    CIGI::V33::EntityCtrl entityCtrl;
    entityCtrl.entityId = entityControl.entityID.Value();
    entityCtrl.entityState = ConvertToEntityState(entityControl.eState);
    entityCtrl.attachState = ((entityControl.bHasParent) ? CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached : CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached);
    entityCtrl.collisionReportEnable = entityControl.bCollisionReportingEnabled;
    entityCtrl.inheritAlpha = entityControl.bInheritAlpha;

    //animation data
    entityCtrl.animationState = ConvertToAnimationState(animationControl.eAnimationState);
    entityCtrl.animationLoopMode = animationControl.eAnimationLoopMode == EAnimationLoopMode::CONTINUOUS ? true : false;
    entityCtrl.alpha = ConvertToCigiAlpha(animationControl.alpha);
    entityCtrl.entityId = animationControl.entityID.Value();

    // Smoothing is te same as Linear Extrapolation/Interpolation Enable
    entityCtrl.extrapolationEnable = entityControl.bSmoothingEnabled;
    entityCtrl.alpha = entityControl.alpha;

    // For Entity type Extended and short do not exist it is just a uint16_t
    entityCtrl.entityType = entityControl.shortEntityTypeID.Value();

    entityCtrl.parentId = entityControl.parentID.Value();

    //top level entity
    entityCtrl.attachState = (topLevelEntityPosition.bAttached) ? CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached : CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached;
    entityCtrl.groundOceanClamp = ConvertToClamp(topLevelEntityPosition.eClamp);
    entityCtrl.entityId = topLevelEntityPosition.entityID.Value();
    entityCtrl.roll = topLevelEntityPosition.rotation.roll.Value();
    entityCtrl.pitch = topLevelEntityPosition.rotation.pitch.Value();
    entityCtrl.yaw = topLevelEntityPosition.rotation.yaw.Value();
    entityCtrl.latitudeXOffset = topLevelEntityPosition.geodeticCoordinates.latitude.Value();
    entityCtrl.longitudeYOffset = topLevelEntityPosition.geodeticCoordinates.longitude.Value();
    entityCtrl.altitudeZOffset = topLevelEntityPosition.geodeticCoordinates.altitude.Value();

    if (m_bByteSwap)
    {
      entityCtrl.doByteSwapping();
    }

    Pack(&entityCtrl, sizeof(CIGI::V33::EntityCtrl));
  }
  catch (const std::exception& ex)
  {
    std::cout << "Error sending Entity Control packet" << std::endl;
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendChildEntityControl(const sbio::cigi::SEntityControl& entityControl, const sbio::cigi::SChildEntityPosition& childEntityPosition, const sbio::cigi::SCigiAnimationControl& animationControl)
{
  try
  {
    m_Entities[entityControl.entityID] = entityControl;

    CIGI::V33::EntityCtrl entityCtrl;
    entityCtrl.entityId = entityControl.entityID.Value();
    entityCtrl.entityState = ConvertToEntityState(entityControl.eState);
    entityCtrl.attachState = ((entityControl.bHasParent) ? CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached : CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached);
    entityCtrl.collisionReportEnable = entityControl.bCollisionReportingEnabled;
    entityCtrl.inheritAlpha = entityControl.bInheritAlpha;

    //animation data
    entityCtrl.animationState = ConvertToAnimationState(animationControl.eAnimationState);
    entityCtrl.animationLoopMode = animationControl.eAnimationLoopMode == EAnimationLoopMode::CONTINUOUS ? true : false;
    entityCtrl.alpha = ConvertToCigiAlpha(animationControl.alpha);
    entityCtrl.entityId = animationControl.entityID.Value();

    // Smoothing is te same as Linear Extrapolation/Interpolation Enable
    entityCtrl.extrapolationEnable = entityControl.bSmoothingEnabled;
    entityCtrl.alpha = entityControl.alpha;

    // For Entity type Extended and short do not exist it is just a uint16_t
    entityCtrl.entityType = entityControl.shortEntityTypeID.Value();

    entityCtrl.parentId = entityControl.parentID.Value();

    //child entity data
    entityCtrl.attachState = (childEntityPosition.bAttached) ? CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached : CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached;
    entityCtrl.groundOceanClamp = CIGI::V33::EntityCtrl::GroundClamp::eGroundClamp_NoClamp;// Clamp is not supported for child entities
    entityCtrl.entityId = childEntityPosition.entityID.Value();
    entityCtrl.roll = childEntityPosition.rotation.roll.Value();
    entityCtrl.pitch = childEntityPosition.rotation.pitch.Value();
    entityCtrl.yaw = childEntityPosition.rotation.yaw.Value();
    entityCtrl.latitudeXOffset = childEntityPosition.offset[0];
    entityCtrl.longitudeYOffset = childEntityPosition.offset[1];
    entityCtrl.altitudeZOffset = childEntityPosition.offset[2];

    if (m_bByteSwap)
    {
      entityCtrl.doByteSwapping();
    }

    Pack(&entityCtrl, sizeof(CIGI::V33::EntityCtrl));
  }
  catch (const std::exception& ex)
  {
    std::cout << "Error sending Entity Control packet" << std::endl;
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendConformalClampedEntityPosition(const sbio::cigi::SCigiConformalClampedEntityPosition& conformalClampedEntityControl)
{
  try
  {
    CIGI::V33::ConformalClampedEntityCtrl confClampEntity;
    confClampEntity.entityId = conformalClampedEntityControl.entityID.Value();
    confClampEntity.yaw = conformalClampedEntityControl.fYaw.Value();
    confClampEntity.latitude = conformalClampedEntityControl.latitude.Value();
    confClampEntity.longitude = conformalClampedEntityControl.longitude.Value();

    if (m_bByteSwap)
    {
      confClampEntity.doByteSwapping();
    }

    Pack(&confClampEntity, sizeof(CIGI::V33::ConformalClampedEntityCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEarthReferenceModelDefinition(const SCigiEarthReferenceModel& earthReferenceModel)
{
  try
  {
    CIGI::V33::EarthReferenceModelDefinition earthModelDef;
    earthModelDef.customERMEnable = earthReferenceModel.eEarthReferenceModel != EEarthReferenceModel::WGS84;
    earthModelDef.equatorialRadius = earthReferenceModel.fEquatorialRadius;
    earthModelDef.flattening = earthReferenceModel.fFlattening;

    if (m_bByteSwap)
    {
      earthModelDef.doByteSwapping();
    }

    Pack(&earthModelDef, sizeof(CIGI::V33::EarthReferenceModelDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendTopLevelEntityPosition(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition)
{
  try
  {
    SEntityControl entityControl;

    auto it = m_Entities.find(topLevelEntityPosition.entityID);
    if (it == m_Entities.end())
    {
      return;
    }
    else
    {
      entityControl = it->second;
    }

    CIGI::V33::EntityCtrl entityCtrl;
    entityCtrl.entityId = entityControl.entityID.Value();
    entityCtrl.entityState = ConvertToEntityState(entityControl.eState);
    entityCtrl.attachState = ((entityControl.bHasParent) ? CIGI::V33::EntityCtrl::AttachState::eAttachState_Attached : CIGI::V33::EntityCtrl::AttachState::eAttachState_Detached);
    entityCtrl.collisionReportEnable = entityControl.bCollisionReportingEnabled;
    entityCtrl.inheritAlpha = entityControl.bInheritAlpha;

    // need to setup: Ground/Ocean Clamp, Animation Direction, Animation Loop Mode, and Animation State
    // entityCtrl.SetGrndClamp();
    // entityCtrl.SetAnimationDir();
    // entityCtrl.SetAnimationLoopMode();
    // entityCtrl.SetAnimationState();

    // Smoothing is te same as Linear Extrapolation/Interpolation Enable
    entityCtrl.extrapolationEnable = entityControl.bSmoothingEnabled;
    entityCtrl.alpha = entityControl.alpha;

    // For Entity type Extended and short do not exist it is just a uint16_t
    entityCtrl.entityType = entityControl.shortEntityTypeID.Value();

    entityCtrl.parentId = entityControl.parentID.Value();

    // Roll, Pitch, Yaw
    entityCtrl.roll = topLevelEntityPosition.rotation.roll.Value();
    entityCtrl.pitch = topLevelEntityPosition.rotation.pitch.Value();
    entityCtrl.yaw = topLevelEntityPosition.rotation.yaw.Value();

    entityCtrl.latitudeXOffset = topLevelEntityPosition.geodeticCoordinates.latitude.Value();
    entityCtrl.longitudeYOffset = topLevelEntityPosition.geodeticCoordinates.longitude.Value();
    entityCtrl.altitudeZOffset = topLevelEntityPosition.geodeticCoordinates.altitude.Value();

    if (m_bByteSwap)
    {
      entityCtrl.doByteSwapping();
    }

    Pack(&entityCtrl, sizeof(CIGI::V33::EntityCtrl));
  }
  catch (const std::exception& ex)
  {
    std::cout << "Error sending Entity Control packet" << std::endl;
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEnvironmentalConditionsRequest(const SEnvironmentalConditionsRequest& environmentalConditionsRequest)
{
  try
  {
    CIGI::V33::EnvironmentalConditionsRequest envCondReq;

    if (environmentalConditionsRequest.bMaritimeSurfaceConditionsRequest)
    {
      envCondReq.requestType |= 1;
    }
    if (environmentalConditionsRequest.bTerrestrialSurfaceConditionsRequest)
    {
      envCondReq.requestType |= 2;
    }
    if (environmentalConditionsRequest.bWeatherConditionsRequest)
    {
      envCondReq.requestType |= 4;
    }
    if (environmentalConditionsRequest.bAerosolConcentrationsRequest)
    {
      envCondReq.requestType |= 8;
    }

    envCondReq.requestId = environmentalConditionsRequest.nRequestID;
    envCondReq.latitude = environmentalConditionsRequest.geodeticCoordinates.latitude.Value();
    envCondReq.longitude = environmentalConditionsRequest.geodeticCoordinates.longitude.Value();
    envCondReq.altitude = environmentalConditionsRequest.geodeticCoordinates.altitude.Value();

    if (m_bByteSwap)
    {
      envCondReq.doByteSwapping();
    }

    Pack(&envCondReq, sizeof(CIGI::V33::EnvironmentalConditionsRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEnvironmentalRegionControl(const SCigiEnvironmentalRegion& environmentalRegion)
{
  try
  {
    CIGI::V33::EnvironmentalRegionCtrl envRegion;
    envRegion.regionId = environmentalRegion.regionID.Value();
    envRegion.regionState = ConvertToRegionStateGrp(environmentalRegion.eRegionState);
    envRegion.mergeWeatherProperties = ConvertToMergeControlGrp(environmentalRegion.eMergeWeatherProperties);
    envRegion.mergeAerosolConcentrations = ConvertToMergeControlGrp(environmentalRegion.eMergeAerosolConcentrations);
    envRegion.mergeMaritimeSurfaceConditions = ConvertToMergeControlGrp(environmentalRegion.eMergeMaritimeSurfaceConditions);
    envRegion.mergeTerrestrialSurfaceConditions = ConvertToMergeControlGrp(environmentalRegion.eMergeTerrestrialSurfaceConditions);
    envRegion.latitude = environmentalRegion.latitude.Value();
    envRegion.longitude = environmentalRegion.longitude.Value();
    envRegion.sizeX = environmentalRegion.size[0];
    envRegion.sizeY = environmentalRegion.size[1];
    envRegion.cornerRadius = environmentalRegion.fCornerRadius;
    envRegion.rotation = environmentalRegion.fRotation.Value();
    envRegion.transitionPerimeter = environmentalRegion.fTransition;

    if (m_bByteSwap)
    {
      envRegion.doByteSwapping();
    }

    Pack(&envRegion, sizeof(CIGI::V33::EnvironmentalRegionCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendHatHotRequest(const SHATHOTGlobalRequest& hatHotRequest)
{
  try
  {
    CIGI::V33::HATHOTRequest request;
    request.HATHOTId = hatHotRequest.requestID.Value();
    request.entityId = 0;//no entity associated with global request
    request.requestType = ConvertToRequestType(hatHotRequest.eRequestType);
    request.coordinateSystem = CIGI::V33::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Geodetic;// Global requests are always geodetic
    request.updatePeriod = hatHotRequest.updatePeriod.Value();

    request.latitudeXOffset = hatHotRequest.geodeticCoordinates.latitude.Value();
    request.longitudeYOffset = hatHotRequest.geodeticCoordinates.longitude.Value();
    request.altitudeZOffset = hatHotRequest.geodeticCoordinates.altitude.Value();

    if (m_bByteSwap)
    {
      request.doByteSwapping();
    }

    Pack(&request, sizeof(CIGI::V33::HATHOTRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendHatHotRequest(const SHATHOTEntityRequest& hatHotRequest)
{
  try
  {
    CIGI::V33::HATHOTRequest request;
    request.HATHOTId = hatHotRequest.requestID.Value();
    request.entityId = hatHotRequest.entityID.Value();
    request.requestType = ConvertToRequestType(hatHotRequest.eRequestType);
    request.coordinateSystem = CIGI::V33::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Entity;// Entity requests are always in entity coordinate system
    request.updatePeriod = hatHotRequest.updatePeriod.Value();

    request.latitudeXOffset = hatHotRequest.offset[0];
    request.longitudeYOffset = hatHotRequest.offset[1];
    request.altitudeZOffset = hatHotRequest.offset[2];

    if (m_bByteSwap)
    {
      request.doByteSwapping();
    }

    Pack(&request, sizeof(CIGI::V33::HATHOTRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(const SLineOfSightSegmentRequestGeodeticToGeodeticBasic& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.entityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V32::LineOfSightSegmentRequest::Type::eType_Basic;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;//basic does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = false;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;
    losSegmentRequest.materialMask = losRequest.nMaterialMask;

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    if (losSegmentRequest.destinationCoordinateSystem == CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic)
    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationGeodeticCoordinates.latitude.Value();
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationGeodeticCoordinates.longitude.Value();
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationGeodeticCoordinates.altitude.Value();
    }

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    Pack(&losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestGeodeticToGeodeticExtended(const SLineOfSightSegmentRequestGeodeticToGeodeticExtended& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.entityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V32::LineOfSightSegmentRequest::Type::eType_Extended;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;//Extended does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = false;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;
    losSegmentRequest.materialMask = losRequest.nMaterialMask;

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    if (losSegmentRequest.destinationCoordinateSystem == CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic)
    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationGeodeticCoordinates.latitude.Value();
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationGeodeticCoordinates.longitude.Value();
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationGeodeticCoordinates.altitude.Value();
    }

    losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    Pack(&losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestGeodeticToEntityBasic(const SLineOfSightSegmentRequestGeodeticToEntityBasic& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.entityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V33::LineOfSightSegmentRequest::Type::eType_Basic;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;//basic does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = 1;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = losRequest.destinationEntityID.Value();

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationOffset[0];
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationOffset[1];
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationOffset[2];
    }

    losSegmentRequest.materialMask = losRequest.nMaterialMask;

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    Pack(&losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestGeodeticToEntityExtended(const SLineOfSightSegmentRequestGeodeticToEntityExtended& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.entityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V33::LineOfSightSegmentRequest::Type::eType_Extended;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;//Extended does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = 1;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = losRequest.destinationEntityID.Value();

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationOffset[0];
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationOffset[1];
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationOffset[2];
    }

    losSegmentRequest.materialMask = losRequest.nMaterialMask;
    losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    Pack(&losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestEntityToGeodeticBasic(const SLineOfSightSegmentRequestEntityToGeodeticBasic& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.requestType = CIGI::V33::LineOfSightSegmentRequest::Type::eType_Basic;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;//basic does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = 0;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceOffset[0];
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceOffset[1];
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceOffset[2];
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationGeodeticCoordinates.latitude.Value();
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationGeodeticCoordinates.longitude.Value();
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationGeodeticCoordinates.altitude.Value();
    }

    losSegmentRequest.materialMask = losRequest.nMaterialMask;

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
    m_nSendBufferLength += sizeof(CIGI::V33::LineOfSightSegmentRequest);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestEntityToGeodeticExtended(const SLineOfSightSegmentRequestEntityToGeodeticExtended& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.requestType = CIGI::V33::LineOfSightSegmentRequest::Type::eType_Extended;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;//Extended does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = 0;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceOffset[0];
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceOffset[1];
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceOffset[2];
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationGeodeticCoordinates.latitude.Value();
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationGeodeticCoordinates.longitude.Value();
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationGeodeticCoordinates.altitude.Value();
    }

    losSegmentRequest.materialMask = losRequest.nMaterialMask;
    losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    memcpy(&m_sendBuffer[m_nSendBufferLength], &losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
    m_nSendBufferLength += sizeof(CIGI::V33::LineOfSightSegmentRequest);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestEntityToEntityBasic(const SLineOfSightSegmentRequestEntityToEntityBasic& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.entityId = losRequest.sourceEntityID.Value();
    losSegmentRequest.requestType = CIGI::V33::LineOfSightSegmentRequest::Type::eType_Basic;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;//basic does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = losRequest.destinationEntityID != losRequest.sourceEntityID;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = losRequest.destinationEntityID.Value();

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceOffset[0];
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceOffset[1];
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceOffset[2];
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationOffset[0];
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationOffset[1];
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationOffset[2];
    }

    losSegmentRequest.materialMask = losRequest.nMaterialMask;

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    Pack(&losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightSegmentRequestEntityToEntityExtended(const SLineOfSightSegmentRequestEntityToEntityExtended& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.entityId = losRequest.sourceEntityID.Value();
    losSegmentRequest.requestType = CIGI::V33::LineOfSightSegmentRequest::Type::eType_Extended;
    losSegmentRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationCoordinateSystem = CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;//Extended does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = losRequest.destinationEntityID != losRequest.sourceEntityID;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = losRequest.destinationEntityID.Value();

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceOffset[0];
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceOffset[1];
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceOffset[2];
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationOffset[0];
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationOffset[1];
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationOffset[2];
    }

    losSegmentRequest.materialMask = losRequest.nMaterialMask;
    losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (m_bByteSwap)
    {
      losSegmentRequest.doByteSwapping();
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    Pack(&losSegmentRequest, sizeof(CIGI::V33::LineOfSightSegmentRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightVectorRequestGeodeticBasic(const SLineOfSightVectorRequestGeodeticBasic& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V33::LineOfSightVectorRequest::Type::eType_Basic;

    losVectorRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losVectorRequest.responseCoordinateSystem = 0;//basic does not have a response coordinate system field.
    losVectorRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losVectorRequest.entityId = 0;
    losVectorRequest.azimuth = losRequest.azimuth.Value();
    losVectorRequest.elevation = losRequest.elevation.Value();
    losVectorRequest.maximumRange = losRequest.fMaximumRange;
    losVectorRequest.minimumRange = losRequest.fMinimumRange;

    {
      losVectorRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losVectorRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losVectorRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    losVectorRequest.materialMask = losRequest.nMaterialMask;
    losVectorRequest.updatePeriod = losRequest.updatePeriod.Value();

    if (m_bByteSwap)
    {
      losVectorRequest.doByteSwapping();
    }

    Pack(&losVectorRequest, sizeof(CIGI::V33::LineOfSightVectorRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightVectorRequestGeodeticExtended(const SLineOfSightVectorRequestGeodeticExtended& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V33::LineOfSightVectorRequest::Type::eType_Extended;

    losVectorRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losVectorRequest.responseCoordinateSystem = 0;//Extended does not have a response coordinate system field.
    losVectorRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losVectorRequest.entityId = 0;
    losVectorRequest.azimuth = losRequest.azimuth.Value();
    losVectorRequest.elevation = losRequest.elevation.Value();
    losVectorRequest.maximumRange = losRequest.fMaximumRange;
    losVectorRequest.minimumRange = losRequest.fMinimumRange;

    {
      losVectorRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losVectorRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losVectorRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    losVectorRequest.materialMask = losRequest.nMaterialMask;
    losVectorRequest.updatePeriod = losRequest.updatePeriod.Value();
    losVectorRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (m_bByteSwap)
    {
      losVectorRequest.doByteSwapping();
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    Pack(&losVectorRequest, sizeof(CIGI::V33::LineOfSightVectorRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightVectorRequestEntityBasic(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V33::LineOfSightVectorRequest::Type::eType_Basic;

    losVectorRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losVectorRequest.responseCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::Type::eType_Basic;
    losVectorRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losVectorRequest.entityId = losRequest.sourceEntityID.Value();
    losVectorRequest.azimuth = losRequest.azimuth.Value();
    losVectorRequest.elevation = losRequest.elevation.Value();
    losVectorRequest.maximumRange = losRequest.fMaximumRange;
    losVectorRequest.minimumRange = losRequest.fMinimumRange;
    losVectorRequest.sourceLatitudeXOffset = losRequest.sourceOffset[0];
    losVectorRequest.sourceLongitudeYOffset = losRequest.sourceOffset[1];
    losVectorRequest.sourceAltitudeZOffset = losRequest.sourceOffset[2];
    losVectorRequest.materialMask = losRequest.nMaterialMask;
    losVectorRequest.updatePeriod = losRequest.updatePeriod.Value();
    losVectorRequest.responseCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;//entity basic does not have a response coordinate system

    if (m_bByteSwap)
    {
      losVectorRequest.doByteSwapping();
    }

    Pack(&losVectorRequest, sizeof(CIGI::V33::LineOfSightVectorRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendLineOfSightVectorRequestEntityExtended(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& losRequest)
{
  try
  {
    CIGI::V33::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V33::LineOfSightVectorRequest::Type::eType_Extended;

    losVectorRequest.sourceCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losVectorRequest.responseCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::Type::eType_Extended;
    losVectorRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losVectorRequest.entityId = losRequest.sourceEntityID.Value();
    losVectorRequest.azimuth = losRequest.azimuth.Value();
    losVectorRequest.elevation = losRequest.elevation.Value();
    losVectorRequest.maximumRange = losRequest.fMaximumRange;
    losVectorRequest.minimumRange = losRequest.fMinimumRange;
    losVectorRequest.sourceLatitudeXOffset = losRequest.sourceOffset[0];
    losVectorRequest.sourceLongitudeYOffset = losRequest.sourceOffset[1];
    losVectorRequest.sourceAltitudeZOffset = losRequest.sourceOffset[2];
    losVectorRequest.materialMask = losRequest.nMaterialMask;
    losVectorRequest.updatePeriod = losRequest.updatePeriod.Value();
    losVectorRequest.responseCoordinateSystem = CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;//entity Extended does not have a response coordinate system

    losVectorRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (m_bByteSwap)
    {
      losVectorRequest.doByteSwapping();
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    Pack(&losVectorRequest, sizeof(CIGI::V33::LineOfSightVectorRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEntityMaritimeSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  try
  {
    CIGI::V33::MaritimeSurfaceConditionsCtrl maritimeSurfaceCtrl;
    maritimeSurfaceCtrl.entityRegionId = entityID.Value();
    maritimeSurfaceCtrl.surfaceConditionsEnable = maritimeSurfaceCondition.bActive;
    maritimeSurfaceCtrl.whitecapEnable = maritimeSurfaceCondition.bWhitecapEnabled;
    maritimeSurfaceCtrl.scope = CIGI::V33::MaritimeSurfaceConditionsCtrl::eScope_Entity;
    maritimeSurfaceCtrl.seaSurfaceHeight = static_cast<float>(maritimeSurfaceCondition.fSeaSurfaceHeight.Value());
    maritimeSurfaceCtrl.surfaceWaterTemperature = maritimeSurfaceCondition.fSurfaceWaterTemperature.Value();
    maritimeSurfaceCtrl.surfaceClarity = ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value();

    if (m_bByteSwap)
    {
      maritimeSurfaceCtrl.doByteSwapping();
    }

    Pack(&maritimeSurfaceCtrl, sizeof(CIGI::V33::MaritimeSurfaceConditionsCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendRegionMaritimeSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  try
  {
    CIGI::V33::MaritimeSurfaceConditionsCtrl maritimeSurfaceCtrl;
    maritimeSurfaceCtrl.entityRegionId = regionID.Value();
    maritimeSurfaceCtrl.surfaceConditionsEnable = maritimeSurfaceCondition.bActive;
    maritimeSurfaceCtrl.whitecapEnable = maritimeSurfaceCondition.bWhitecapEnabled;
    maritimeSurfaceCtrl.scope = CIGI::V33::MaritimeSurfaceConditionsCtrl::eScope_Regional;
    maritimeSurfaceCtrl.seaSurfaceHeight = static_cast<float>(maritimeSurfaceCondition.fSeaSurfaceHeight.Value());
    maritimeSurfaceCtrl.surfaceWaterTemperature = maritimeSurfaceCondition.fSurfaceWaterTemperature.Value();
    maritimeSurfaceCtrl.surfaceClarity = ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value();

    if (m_bByteSwap)
    {
      maritimeSurfaceCtrl.doByteSwapping();
    }

    Pack(&maritimeSurfaceCtrl, sizeof(CIGI::V33::MaritimeSurfaceConditionsCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendGlobalMaritimeSurfaceConditionsControl(const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  try
  {
    CIGI::V33::MaritimeSurfaceConditionsCtrl maritimeSurfaceCtrl;
    maritimeSurfaceCtrl.surfaceConditionsEnable = maritimeSurfaceCondition.bActive;
    maritimeSurfaceCtrl.whitecapEnable = maritimeSurfaceCondition.bWhitecapEnabled;
    maritimeSurfaceCtrl.scope = CIGI::V33::MaritimeSurfaceConditionsCtrl::eScope_Global;
    maritimeSurfaceCtrl.seaSurfaceHeight = static_cast<float>(maritimeSurfaceCondition.fSeaSurfaceHeight.Value());
    maritimeSurfaceCtrl.surfaceWaterTemperature = maritimeSurfaceCondition.fSurfaceWaterTemperature.Value();
    maritimeSurfaceCtrl.surfaceClarity = ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value();

    if (m_bByteSwap)
    {
      maritimeSurfaceCtrl.doByteSwapping();
    }

    Pack(&maritimeSurfaceCtrl, sizeof(CIGI::V33::MaritimeSurfaceConditionsCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendMotionTrackerViewControl(const SMotionTrackerViewControl& motionTrackerControl)
{
  try
  {
    CIGI::V33::MotionTrackerCtrl motionTrack;
    motionTrack.boresightEnable = motionTrackerControl.bBoresightEnable;
    motionTrack.trackerEnable = motionTrackerControl.bEnable;
    motionTrack.trackerId = motionTrackerControl.motionTrackerID.Value();
    motionTrack.viewViewGroupId = motionTrackerControl.viewID.Value();
    motionTrack.xEnable = motionTrackerControl.bXEnable;
    motionTrack.yEnable = motionTrackerControl.bYEnable;
    motionTrack.zEnable = motionTrackerControl.bZEnable;
    motionTrack.rollEnable = motionTrackerControl.bRollEnable;
    motionTrack.pitchEnable = motionTrackerControl.bPitchEnable;
    motionTrack.yawEnable = motionTrackerControl.bYawEnable;

    if (m_bByteSwap)
    {
      motionTrack.doByteSwapping();
    }

    Pack(&motionTrack, sizeof(CIGI::V33::MotionTrackerCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendMotionTrackerViewGroupControl(const SMotionTrackerViewGroupControl& motionTrackerControl)
{
  try
  {
    CIGI::V33::MotionTrackerCtrl motionTrack;
    motionTrack.boresightEnable = motionTrackerControl.bBoresightEnable;
    motionTrack.trackerEnable = motionTrackerControl.bEnable;
    motionTrack.trackerId = motionTrackerControl.motionTrackerID.Value();
    motionTrack.viewViewGroupId = motionTrackerControl.viewGroupID.Value();
    motionTrack.xEnable = motionTrackerControl.bXEnable;
    motionTrack.yEnable = motionTrackerControl.bYEnable;
    motionTrack.zEnable = motionTrackerControl.bZEnable;
    motionTrack.rollEnable = motionTrackerControl.bRollEnable;
    motionTrack.pitchEnable = motionTrackerControl.bPitchEnable;
    motionTrack.yawEnable = motionTrackerControl.bYawEnable;

    if (m_bByteSwap)
    {
      motionTrack.doByteSwapping();
    }

    Pack(&motionTrack, sizeof(CIGI::V33::MotionTrackerCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendPositionRequest(const sbio::cigi::SPositionRequest& positionRequest)
{
  try
  {
    CIGI::V33::PositionRequest posRequest;
    posRequest.objectId = positionRequest.nObjectID;
    posRequest.articulatedPartId = positionRequest.articulatedPartID.Value();
    posRequest.updateMode = positionRequest.bContinuous ? CIGI::V33::PositionRequest::UpdateMode::eUpdateMode_Continuous : CIGI::V33::PositionRequest::UpdateMode::eUpdateMode_OneShot;
    posRequest.objectClass = GetCigiObjectClassGrp(positionRequest.eObjectClass);
    posRequest.coordinateSystem = ConvertToCoordSysGrp(positionRequest.eCoordinateSystem);

    if (m_bByteSwap)
    {
      posRequest.doByteSwapping();
    }

    Pack(&posRequest, sizeof(CIGI::V33::PositionRequest));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V33::SensorCtrl::Polarity ConvertToTrackPolarity(ESensorTrack eSensorTrack)
{
  if (eSensorTrack == ESensorTrack::WHITE)
  {
    return CIGI::V33::SensorCtrl::Polarity::ePolarity_WhiteHot;
  }
  else if (eSensorTrack == ESensorTrack::BLACK)
  {
    return CIGI::V33::SensorCtrl::Polarity::ePolarity_BlackHot;
  }

  return CIGI::V33::SensorCtrl::Polarity::ePolarity_WhiteHot;
}

void CHostSessionV3_3::SendSensorControl(const SCigiSensorControl& sensorControl)
{
  try
  {
    CIGI::V33::SensorCtrl sensorCtrl;
    sensorCtrl.viewId = sensorControl.viewID.Value();
    sensorCtrl.sensorId = sensorControl.sensorID.Value();
    sensorCtrl.trackMode = ConvertToTrackModeGrp(sensorControl.eTrackMode);
    sensorCtrl.sensorOnOff = sensorControl.bSensorOn;
    sensorCtrl.polarity = ConvertToPolarityGrp(sensorControl.ePolarity);
    sensorCtrl.lineByLineDropoutEnable = sensorControl.bLineByLineDropoutEnabled;
    sensorCtrl.automaticGain = sensorControl.bAutomaticGain;
    sensorCtrl.trackWhiteBlack = ConvertToTrackPolarity(sensorControl.eSensorTrack);

    if (sensorControl.bExtendedResponse)
    {
      sensorCtrl.responseType = CIGI::V33::SensorCtrl::ResponseType::eResponseType_Extended;
    }
    else
    {
      sensorCtrl.responseType = CIGI::V33::SensorCtrl::ResponseType::eResponseType_Normal;
    }

    sensorCtrl.gain = sensorControl.gain.Value();
    sensorCtrl.level = sensorControl.level.Value();
    sensorCtrl.acCoupling = sensorControl.fACCoupling;
    sensorCtrl.noise = sensorControl.noise.Value();

    if (m_bByteSwap)
    {
      sensorCtrl.doByteSwapping();
    }

    Pack(&sensorCtrl, sizeof(CIGI::V33::SensorCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendShortArticulatedPartControl(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart)
{
  try
  {
    CIGI::V30::ShortArticulatedPartCtrl shortArtPartCtrl;
    shortArtPartCtrl.entityId = shortArticulatedPart.entityID.Value();
    shortArtPartCtrl.articulatedPartId1 = shortArticulatedPart.articulatedPartID1.Value();
    shortArtPartCtrl.articulatedPartId2 = shortArticulatedPart.articulatedPartID2.Value();
    shortArtPartCtrl.dofSelect1 = ConvertToDOFSelect(shortArticulatedPart.eDOF1);
    shortArtPartCtrl.dofSelect2 = ConvertToDOFSelect(shortArticulatedPart.eDOF2);
    shortArtPartCtrl.articulatedPartEnable1 = shortArticulatedPart.bArticulatedPart1Enabled;
    shortArtPartCtrl.articulatedPartEnable2 = shortArticulatedPart.bArticulatedPart2Enabled;
    shortArtPartCtrl.dof1 = shortArticulatedPart.fDOF1;
    shortArtPartCtrl.dof2 = shortArticulatedPart.fDOF2;

    if (m_bByteSwap)
    {
      shortArtPartCtrl.doByteSwapping();
    }

    Pack(&shortArtPartCtrl, sizeof(CIGI::V30::ShortArticulatedPartCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendShortComponentControl(const SCigiShortComponentControl& shortComponentControl)
{
  try
  {
    CIGI::V33::ShortComponentCtrl shortCompCtrl;
    shortCompCtrl.componentId = shortComponentControl.componentID.Value();
    shortCompCtrl.instanceId = shortComponentControl.nInstanceID;
    shortCompCtrl.componentClass = ConvertToCompClass(shortComponentControl.componentClassID.Value());
    shortCompCtrl.componentState = shortComponentControl.nComponentState;
    shortCompCtrl.componentData[0] = shortComponentControl.componentData1;
    shortCompCtrl.componentData[1] = shortComponentControl.componentData2;

    if (m_bByteSwap)
    {
      shortCompCtrl.doByteSwapping();
    }

    Pack(&shortCompCtrl, sizeof(CIGI::V33::ShortComponentCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V33::ShortSymbolCtrl::AttributeSelect ConvertAttributeSelect(EAttributeSelect attribute)
{
  if (attribute == EAttributeSelect::NONE)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_None;
  }
  else if (attribute == EAttributeSelect::SURFACEID)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_SurfaceID;
  }
  else if (attribute == EAttributeSelect::PARENTSYMBOLID)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ParentSymbolID;
  }
  else if (attribute == EAttributeSelect::LAYER)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Layer;
  }
  else if (attribute == EAttributeSelect::FLASHDUTYCYCLEPERCENTAGE)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashDutyCyclePercentage;
  }
  else if (attribute == EAttributeSelect::FLASHPERIOD)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashPeriod;
  }
  else if (attribute == EAttributeSelect::POSITIONU)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionU;
  }
  else if (attribute == EAttributeSelect::POSITIONV)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionV;
  }
  else if (attribute == EAttributeSelect::ROTATION)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Rotation;
  }
  else if (attribute == EAttributeSelect::COLOR)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Color;
  }
  else if (attribute == EAttributeSelect::SCALEU)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleU;
  }
  else if (attribute == EAttributeSelect::SCALEV)
  {
    return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleV;
  }

  return CIGI::V33::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_None;
}

extern uint32_t SetAttributeDatum(EAttributeSelect attribute, SAttributeValue value);

void CHostSessionV3_3::SendShortSymbolControl(const SShortSymbolControl& shortSymbolControl)
{
  try
  {
    CIGI::V33::ShortSymbolCtrl shortSymbolCtrl;
    shortSymbolCtrl.symbolId = shortSymbolControl.symbolID.Value();
    shortSymbolCtrl.symbolState = ConvertToSymbolStateGrp(shortSymbolControl.eSymbolState);
    shortSymbolCtrl.attachState = ConvertToAttachState(shortSymbolControl.eAttachState);
    shortSymbolCtrl.flashControl = ConvertToFlashCtrlGrp(shortSymbolControl.eFlashControl);
    shortSymbolCtrl.inheritColor = ConvertToInheritColorGrp(shortSymbolControl.bInheritColor);

    shortSymbolCtrl.attributeSelect1 = static_cast<uint8_t>(ConvertAttributeSelect(shortSymbolControl.eAttributeSelect1));
    shortSymbolCtrl.attributeSelect2 = static_cast<uint8_t>(ConvertAttributeSelect(shortSymbolControl.eAttributeSelect2));
    shortSymbolCtrl.attributeValue1 = SetAttributeDatum(shortSymbolControl.eAttributeSelect1, shortSymbolControl.attributeValue1);
    shortSymbolCtrl.attributeValue2 = SetAttributeDatum(shortSymbolControl.eAttributeSelect2, shortSymbolControl.attributeValue2);

    if (m_bByteSwap)
    {
      shortSymbolCtrl.doByteSwapping();
    }

    Pack(&shortSymbolCtrl, sizeof(CIGI::V33::ShortSymbolCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendSymbolCircleDefinition(const SSymbolCircle& circleDef)
{
  try
  {
    CIGI::V33::SymbolCircleDefinition symbolCircle;
    symbolCircle.symbolId = circleDef.symbolID.Value();
    symbolCircle.drawingStyle = ConvertToDrawingStyleGrp(circleDef.eDrawingStyle);
    symbolCircle.stipplePattern = circleDef.stipplePattern;
    symbolCircle.lineWidth = circleDef.fLineWidth;
    symbolCircle.stipplePatternLength = circleDef.fStipplePatternLength;

    CIGI::VariableRecordHelper<CIGI::V33::SymbolCircleDefinition> helper(symbolCircle);

    for (const auto& circleProperties : circleDef.circles)
    {
      CIGI::V33::SymbolCircleDefinition::Circle circle;
      circle.centerU = circleProperties.centerUV.U;
      circle.centerV = circleProperties.centerUV.V;
      circle.radius = circleProperties.fRadius;
      circle.innerRadius = circleProperties.fInnerRadius;
      circle.startAngle = circleProperties.startAngle.Value();
      circle.endAngle = circleProperties.endAngle.Value();
      helper.push(circle);
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::SYMBOL_CIRCLE_DEFINITION) << "\n"
                        << " Symbol ID: " << std::to_string(circleDef.symbolID.Value()) << "\n";

      if (circleDef.eDrawingStyle == EDrawingStyle::LINE)
      {
        args.sDataMessage << "Drawing Style: "
                          << "Line"
                          << "\n"
                          << "Stipple Pattern: " << std::to_string(circleDef.stipplePattern) << "\n"
                          << "Line Width: " << std::to_string(circleDef.fLineWidth) << "\n"
                          << "Stipple Pattern Length: " << std::to_string(circleDef.fLineWidth) << "\n";
        /*<< "Center U: " << std::to_string(circleDef.fCenterU) << "\n"
        << "Center V: " << std::to_string(circleDef.fCenterV) << "\n"
        << "Radius: " << std::to_string(circleDef.fRadius) << "\n"
        << "Start Angle: " << std::to_string(circleDef.startAngle.Value()) << "\n"
        << "End Angle: " << std::to_string(circleDef.endAngle.Value());*/
      }
      else
      {
        args.sDataMessage << "Drawing Style: "
                          << "Fill"
                          << "\n";
        /*<< "Center U: " << std::to_string(circleDef.fCenterU) << "\n"
        << "Center V: " << std::to_string(circleDef.fCenterV) << "\n"
        << "Radius: " << std::to_string(circleDef.fRadius) << "\n"
        << "Inner Radius: " << std::to_string(circleDef.fCenterV) << "\n"
        << "Start Angle: " << std::to_string(circleDef.startAngle.Value()) << "\n"
        << "End Angle: " << std::to_string(circleDef.endAngle.Value());*/
      }
      Event::Raise<HostCigiEvent>(args);
    }

    if (m_bByteSwap)
    {
      symbolCircle.doByteSwapping();
    }

    int numRecords = CIGI::V33::SymbolCircleDefinition::RecordHelper::numRecords(symbolCircle);
    std::unique_ptr<TBuffer<char>> pPacketBuffer = std::make_unique<TBuffer<char>>(CIGI::V33::SymbolCircleDefinition::kBasePacketSize + numRecords * static_cast<int>(sizeof(CIGI::V33::SymbolCircleDefinition::Record)));
    memcpy(pPacketBuffer->GetBuffer(), &symbolCircle, CIGI::V33::SymbolCircleDefinition::kBasePacketSize);
    memcpy(pPacketBuffer->GetBuffer() + CIGI::V33::SymbolCircleDefinition::kBasePacketSize, symbolCircle.records, numRecords * sizeof(CIGI::V33::SymbolCircleDefinition::Record));
    Pack(pPacketBuffer->GetBuffer(), pPacketBuffer->GetSize());
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendSymbolClone(const SSymbolClone& symbolCloneStruct)
{
  try
  {
    CIGI::V33::SymbolClone symbolClone;
    symbolClone.symbolId = symbolCloneStruct.symbolID.Value();
    symbolClone.sourceId = symbolCloneStruct.sourceID.Value();

    if (symbolCloneStruct.eSymbolSourceType == ESymbolSourceType::SYMBOL)
    {
      symbolClone.sourceType = CIGI::V33::SymbolClone::SourceType::eSourceType_Symbol;
    }
    else
    {
      symbolClone.sourceType = CIGI::V33::SymbolClone::SourceType::eSourceType_SymbolTemplate;
    }

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::SYMBOL_CLONE) << "\n"
                        << "Symbol ID: " << std::to_string(symbolCloneStruct.symbolID.Value()) << "\n";

      if (symbolCloneStruct.eSymbolSourceType == ESymbolSourceType::SYMBOL)
      {
        args.sDataMessage << "Source Type: " << "Symbol" << "\n";
      }
      else
      {
        args.sDataMessage << "Source Type: " << "Symbol Template" << "\n";
      }

      args.sDataMessage << "Source ID: " << std::to_string(symbolCloneStruct.sourceID.Value()) << "\n";
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      symbolClone.doByteSwapping();
    }

    Pack(&symbolClone, sizeof(CIGI::V33::SymbolClone));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendSymbolControl(const SSymbolControl& symbolControl)
{
  try
  {
    CIGI::V33::SymbolCtrl symControl;
    symControl.symbolId = symbolControl.symbolID.Value();
    symControl.parentSymbolId = symbolControl.parentSymbolID.Value();
    symControl.surfaceId = symbolControl.surfaceID.Value();
    symControl.symbolState = ConvertToSymbolStateGrp(symbolControl.eSymbolState);
    symControl.attachState = ConvertToAttachState(symbolControl.eAttachState);
    symControl.flashControl = ConvertToFlashCtrlGrp(symbolControl.eFlashControl);
    symControl.inheritColor = ConvertToInheritColorGrp(symbolControl.bInheritColor);
    symControl.layer = symbolControl.nLayerID;
    symControl.flashDutyCyclePercentage = ConvertToCigiIntPercentage(symbolControl.flashDutyCyclePercentage).Value();
    symControl.flashPeriod = symbolControl.fFlashPeriod;
    symControl.positionU = symbolControl.fPositionU;
    symControl.positionV = symbolControl.fPositionV;
    symControl.rotation = symbolControl.fRotation.Value();
    symControl.red = symbolControl.color.r;
    symControl.green = symbolControl.color.g;
    symControl.blue = symbolControl.color.b;
    symControl.alpha = symbolControl.color.a;
    symControl.scaleU = symbolControl.fScaleU;
    symControl.scaleV = symbolControl.fScaleV;

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::SYMBOL_CONTROL) << "\n"
                        << "Symbol ID: " << std::to_string(symbolControl.symbolID.Value()) << "\n"
                        << "Symbol State: " << ConvertCigiSymbolStateToString(symbolControl.eSymbolState) << "\n";

      if (symbolControl.eAttachState == EAttachState::ATTACH)
      {
        args.sDataMessage << "Attach State: " << "Attach" << "\n";
      }
      else
      {
        args.sDataMessage << "Attach State: " << "Detach" << "\n";
      }

      if (symbolControl.eFlashControl == EFlashControl::RESET)
      {
        args.sDataMessage << "Flash Control: " << "Reset" << "\n";
      }
      else
      {
        args.sDataMessage << "Flash Control: " << "Continue" << "\n";
      }

      if (!(symbolControl.eAttachState == EAttachState::DETACH))
      {
        if (symbolControl.bInheritColor)
        {
          args.sDataMessage << "Inherit Color: " << "Inherited" << "\n";
        }
        else
        {
          args.sDataMessage << "Inherit Color: " << "Not Inherited" << "\n";
        }
      }
      args.sDataMessage << "Parent Symbol ID: " << std::to_string(symbolControl.parentSymbolID.Value()) << "\n"
                        << "Surface ID: " << std::to_string(symbolControl.surfaceID.Value()) << "\n"
                        << "Layer: " << std::to_string(symbolControl.nLayerID) << "\n"
                        << "Flash Duty Cycle Percentage: " << std::to_string(ConvertToCigiIntPercentage(symbolControl.flashDutyCyclePercentage).Value()) << "\n"
                        << "Flash Period: " << std::to_string(symbolControl.fFlashPeriod) << "\n"
                        << "Position U: " << std::to_string(symbolControl.fPositionU) << "\n"
                        << "Position V: " << std::to_string(symbolControl.fPositionV) << "\n"
                        << "Rotation: " << std::to_string(symbolControl.fRotation.Value()) << "\n";

      if (!(symbolControl.bInheritColor))
      {
        args.sDataMessage << "Red: " << std::to_string(symbolControl.color.r) << "\n"
                          << "Green: " << std::to_string(symbolControl.color.g) << "\n"
                          << "Blue: " << std::to_string(symbolControl.color.b) << "\n"
                          << "Alpha: " << std::to_string(symbolControl.color.a) << "\n";
      }
      args.sDataMessage << "Scale U: " << std::to_string(symbolControl.fScaleU) << "\n"
                        << "Scale V: " << std::to_string(symbolControl.fScaleV);
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      symControl.doByteSwapping();
    }

    Pack(&symControl, sizeof(CIGI::V33::SymbolCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendSymbolLineDefinition(const SSymbolPolygon& symbolLine)
{
  try
  {
    CIGI::V33::SymbolLineDefinition symbolLineDef;
    symbolLineDef.symbolId = symbolLine.symbolID.Value();

    if (symbolLine.ePrimitiveType == EPrimitiveType::POINT)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_Point;
    }
    else if (symbolLine.ePrimitiveType == EPrimitiveType::LINE)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_Line;
    }
    else if (symbolLine.ePrimitiveType == EPrimitiveType::LINE_STRIP)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_LineStrip;
    }
    else if (symbolLine.ePrimitiveType == EPrimitiveType::LINE_LOOP)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_LineLoop;
    }
    else if (symbolLine.ePrimitiveType == EPrimitiveType::TRIANGLE)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_Triangle;
    }
    else if (symbolLine.ePrimitiveType == EPrimitiveType::TRIANGLE_STRIP)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_TriangleStrip;
    }
    else if (symbolLine.ePrimitiveType == EPrimitiveType::TRIANGLE_FAN)
    {
      symbolLineDef.primitiveType = CIGI::V33::SymbolLineDefinition::PrimitiveType::ePrimitiveType_TriangleFan;
    }

    symbolLineDef.stipplePattern = symbolLine.nStipplePattern;
    symbolLineDef.lineWidth = symbolLine.fLineWidth;
    symbolLineDef.stipplePatternLength = symbolLine.fStipplePatternLength;

    CIGI::VariableRecordHelper<CIGI::V33::SymbolLineDefinition> helper(symbolLineDef);

    for (auto v : symbolLine.vertices)
    {
      CIGI::V33::SymbolLineDefinition::Vertex vertex;
      vertex.vertexU = v[0];
      vertex.vertexV = v[1];

      helper.push(vertex);
    }

    if (m_bByteSwap)
    {
      symbolLineDef.doByteSwapping();
    }

    int numRecords = CIGI::V33::SymbolLineDefinition::RecordHelper::numRecords(symbolLineDef);
    std::unique_ptr<TBuffer<char>> pPacketBuffer = std::make_unique<TBuffer<char>>(CIGI::V33::SymbolLineDefinition::kBasePacketSize + numRecords * static_cast<int>(sizeof(CIGI::V33::SymbolLineDefinition::Record)));
    memcpy(pPacketBuffer->GetBuffer(), &symbolLineDef, CIGI::V33::SymbolLineDefinition::kBasePacketSize);
    memcpy(pPacketBuffer->GetBuffer() + CIGI::V33::SymbolLineDefinition::kBasePacketSize, symbolLineDef.records, numRecords * sizeof(CIGI::V33::SymbolLineDefinition::Record));
    Pack(pPacketBuffer->GetBuffer(), pPacketBuffer->GetSize());
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEntitySymbolSurfaceDefinition(const SEntitySymbolSurfaceDefinition& symbolSurfaceDefinition)
{
  try
  {
    CIGI::V33::SymbolSurfaceDefinition symbolSurfaceDef;
    symbolSurfaceDef.surfaceId = symbolSurfaceDefinition.surfaceID.Value();
    symbolSurfaceDef.surfaceState = ConvertToSurfaceState(symbolSurfaceDefinition.eSurfaceState);
    symbolSurfaceDef.attachType = CIGI::V33::SymbolSurfaceDefinition::AttachType::eAttachType_Entity;
    symbolSurfaceDef.billboard = CIGI::V33::SymbolSurfaceDefinition::Billboard::eBillboard_NonBillboard;
    symbolSurfaceDef.entityIdViewId = symbolSurfaceDefinition.entityID.Value();
    symbolSurfaceDef.xOffsetLeft = symbolSurfaceDefinition.offsetToSurface[0];
    symbolSurfaceDef.yOffsetRight = symbolSurfaceDefinition.offsetToSurface[1];
    symbolSurfaceDef.zOffsetTop = symbolSurfaceDefinition.offsetToSurface[2];
    symbolSurfaceDef.yawBottom = symbolSurfaceDefinition.yaw.Value();
    symbolSurfaceDef.pitch = symbolSurfaceDefinition.pitch.Value();
    symbolSurfaceDef.roll = symbolSurfaceDefinition.roll.Value();
    symbolSurfaceDef.width = symbolSurfaceDefinition.width;
    symbolSurfaceDef.height = symbolSurfaceDefinition.height;
    symbolSurfaceDef.minU = symbolSurfaceDefinition.uvMin[0];
    symbolSurfaceDef.maxU = symbolSurfaceDefinition.uvMax[0];
    symbolSurfaceDef.minV = symbolSurfaceDefinition.uvMin[1];
    symbolSurfaceDef.maxV = symbolSurfaceDefinition.uvMax[1];

    if (m_bByteSwap)
    {
      symbolSurfaceDef.doByteSwapping();
    }

    Pack(&symbolSurfaceDef, sizeof(CIGI::V33::SymbolSurfaceDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEntityBillboardSymbolSurfaceDefinition(const SEntityBillboardSymbolSurfaceDefinition& symbolSurfaceDefinition)
{
  try
  {
    CIGI::V33::SymbolSurfaceDefinition symbolSurfaceDef;
    symbolSurfaceDef.surfaceId = symbolSurfaceDefinition.surfaceID.Value();
    symbolSurfaceDef.surfaceState = ConvertToSurfaceState(symbolSurfaceDefinition.eSurfaceState);
    symbolSurfaceDef.attachType = CIGI::V33::SymbolSurfaceDefinition::AttachType::eAttachType_Entity;
    symbolSurfaceDef.billboard = CIGI::V33::SymbolSurfaceDefinition::Billboard::eBillboard_Billboard;
    symbolSurfaceDef.perspectiveGrowthEnable = symbolSurfaceDefinition.bPerspectiveGrowthEnabled;
    symbolSurfaceDef.entityIdViewId = symbolSurfaceDefinition.entityID.Value();
    symbolSurfaceDef.minU = symbolSurfaceDefinition.uvMin[0];
    symbolSurfaceDef.maxU = symbolSurfaceDefinition.uvMax[0];
    symbolSurfaceDef.minV = symbolSurfaceDefinition.uvMin[1];
    symbolSurfaceDef.maxV = symbolSurfaceDefinition.uvMax[1];

    if (m_bByteSwap)
    {
      symbolSurfaceDef.doByteSwapping();
    }

    Pack(&symbolSurfaceDef, sizeof(CIGI::V33::SymbolSurfaceDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendViewSymbolSurfaceDefinition(const SViewSymbolSurfaceDefinition& symbolSurfaceDefinition)
{
  try
  {
    CIGI::V33::SymbolSurfaceDefinition symbolSurfaceDef;
    symbolSurfaceDef.surfaceId = symbolSurfaceDefinition.surfaceID.Value();
    symbolSurfaceDef.surfaceState = ConvertToSurfaceState(symbolSurfaceDefinition.eSurfaceState);
    symbolSurfaceDef.attachType = CIGI::V33::SymbolSurfaceDefinition::AttachType::eAttachType_View;
    symbolSurfaceDef.entityIdViewId = symbolSurfaceDefinition.viewID.Value();
    symbolSurfaceDef.xOffsetLeft = symbolSurfaceDefinition.fLeft;
    symbolSurfaceDef.yOffsetRight = symbolSurfaceDefinition.fRight;
    symbolSurfaceDef.zOffsetTop = symbolSurfaceDefinition.fTop;
    symbolSurfaceDef.yawBottom = symbolSurfaceDefinition.fBottom;
    symbolSurfaceDef.minU = symbolSurfaceDefinition.uvMin[0];
    symbolSurfaceDef.maxU = symbolSurfaceDefinition.uvMax[0];
    symbolSurfaceDef.minV = symbolSurfaceDefinition.uvMin[1];
    symbolSurfaceDef.maxV = symbolSurfaceDefinition.uvMax[1];

    if (m_bByteSwap)
    {
      symbolSurfaceDef.doByteSwapping();
    }

    Pack(&symbolSurfaceDef, sizeof(CIGI::V33::SymbolSurfaceDefinition));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendSymbolTextDefinition(const SSymbolTextDefinition& symbolTextDef)
{
  try
  {
    CIGI::V33::SymbolTextDefinition symTextDef;
    symTextDef.symbolId = symbolTextDef.symbolID.Value();
    symTextDef.alignment = ConvertToAlignment(symbolTextDef.eTextAlignment);
    symTextDef.orientation = ConvertToOrientation(symbolTextDef.eTextOrientation);
    symTextDef.fontId = symbolTextDef.fontID.Value();
    symTextDef.fontSize = symbolTextDef.fFontSize;
    memset(symTextDef.data, '\0', CIGI::V33::SymbolTextDefinition::kMaxDataLength);
    size_t nTextLength = symbolTextDef.sText.size();
    if (nTextLength >= CIGI::V33::SymbolTextDefinition::kMaxDataLength)
    {
      nTextLength = CIGI::V33::SymbolTextDefinition::kMaxDataLength - 1;
    }
    memcpy(symTextDef.data, symbolTextDef.sText.data(), nTextLength);

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::SYMBOL_TEXT_DEFINITION) << "\n"
                        << "Symbol ID: " << std::to_string(symbolTextDef.symbolID.Value()) << "\n"
                        << "Alignment: " << ConvertCigiAlignmentToString(symbolTextDef.eTextAlignment) << "\n"
                        << "Orientation: " << ConvertCigiOrientationToString(symbolTextDef.eTextOrientation) << "\n";

      if (symbolTextDef.fontID.Value() >= 1 && symbolTextDef.fontID.Value() <= 8)
      {
        args.sDataMessage << "Font ID: " << ConvertCigiFontIDToString(symbolTextDef.fontID.Value()) << "\n";
      }
      else if (symbolTextDef.fontID.Value() >= 9 && symbolTextDef.fontID.Value() <= 16)
      {
        args.sDataMessage << "Font ID: " << "Monospace:" << "\n"
                          << "- " << ConvertCigiFontIDToString(symbolTextDef.fontID.Value()) << "\n";
      }
      else if (symbolTextDef.fontID.Value() >= 17 && symbolTextDef.fontID.Value() <= 255)
      {
        args.sDataMessage << "Font ID: " << "IG-defined" << "\n";
      }
      else if (symbolTextDef.fontID.Value() == 0)
      {
        args.sDataMessage << "Font ID: " << "IG default" << "\n";
      }

      args.sDataMessage << "Font Size: " << std::to_string(symbolTextDef.fFontSize) << "\n"
                        << "Octet n:" << "\n" << symbolTextDef.sText;
      Event::Raise<HostCigiEvent>(args);
    }*/

    int nStringLength = static_cast<int>(strnlen(symTextDef.data, CIGI::V33::SymbolTextDefinition::kMaxDataLength)) + 1;
    int nSpare = nStringLength % 8;

    if (nStringLength < 3)
    {
      nSpare = 4 - nSpare;
    }
    else if (nSpare != 0)
    {
      nSpare = 8 - nSpare;
    }

    if (m_bByteSwap)
    {
      symTextDef.doByteSwapping();
    }

    symTextDef.size += static_cast<CIGI::ui8>(nStringLength + nSpare);
    memcpy(&m_sendBuffer[m_nSendBufferLength], &symTextDef, CIGI::V33::SymbolTextDefinition::kBasePacketSize);
    m_nSendBufferLength += CIGI::V33::SymbolTextDefinition::kBasePacketSize;

    memcpy(&m_sendBuffer[m_nSendBufferLength], symTextDef.data, nStringLength + nSpare);
    m_nSendBufferLength += nStringLength + nSpare;
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendEntityTerrestrialSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl)
{
  try
  {
    CIGI::V33::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceCtrl;
    terrestrialSurfaceCtrl.scope = ConvertToTerrestrialSurfaceScope(ECigiScope::ENTITY);
    terrestrialSurfaceCtrl.entityRegionId = entityID.Value();

    terrestrialSurfaceCtrl.surfaceConditionEnable = terrestrialSurfaceControl.bEnabled;
    terrestrialSurfaceCtrl.severity = ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value();
    terrestrialSurfaceCtrl.coverage = ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value();
    terrestrialSurfaceCtrl.surfaceConditionId = terrestrialSurfaceControl.surfaceConditionID.Value();

    if (m_bByteSwap)
    {
      terrestrialSurfaceCtrl.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &terrestrialSurfaceCtrl, sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl));
    m_nSendBufferLength += sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendRegionTerrestrialSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl)
{
  try
  {
    CIGI::V33::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceCtrl;
    terrestrialSurfaceCtrl.scope = ConvertToTerrestrialSurfaceScope(ECigiScope::REGIONAL);
    terrestrialSurfaceCtrl.entityRegionId = regionID.Value();

    terrestrialSurfaceCtrl.surfaceConditionEnable = terrestrialSurfaceControl.bEnabled;
    terrestrialSurfaceCtrl.severity = ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value();
    terrestrialSurfaceCtrl.coverage = ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value();
    terrestrialSurfaceCtrl.surfaceConditionId = terrestrialSurfaceControl.surfaceConditionID.Value();

    if (m_bByteSwap)
    {
      terrestrialSurfaceCtrl.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &terrestrialSurfaceCtrl, sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl));
    m_nSendBufferLength += sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendGlobalTerrestrialSurfaceConditionsControl(const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl)
{
  try
  {
    CIGI::V33::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceCtrl;
    terrestrialSurfaceCtrl.scope = ConvertToTerrestrialSurfaceScope(ECigiScope::GLOBAL);

    terrestrialSurfaceCtrl.surfaceConditionEnable = terrestrialSurfaceControl.bEnabled;
    terrestrialSurfaceCtrl.severity = ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value();
    terrestrialSurfaceCtrl.coverage = ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value();
    terrestrialSurfaceCtrl.surfaceConditionId = terrestrialSurfaceControl.surfaceConditionID.Value();

    if (m_bByteSwap)
    {
      terrestrialSurfaceCtrl.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &terrestrialSurfaceCtrl, sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl));
    m_nSendBufferLength += sizeof(CIGI::V33::TerrestrialSurfaceConditionsCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendRateControl(const SCigiEntityVelocityControl& rateControl)
{
  // Can use SCigiEntityVelocityControl for Rate Control since they are the same
  try
  {
    CIGI::V33::RateCtrl rateCtrl;
    rateCtrl.entityId = rateControl.entityID.Value();
    rateCtrl.applyToArticulatedPart = false;

    if (rateControl.coordinateSystem == EObjectCoordinateSystem::PARENT)
    {
      rateCtrl.coordinateSystem = CIGI::V33::RateCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
    }
    else if (rateControl.coordinateSystem == EObjectCoordinateSystem::LOCAL)
    {
      rateCtrl.coordinateSystem = CIGI::V33::RateCtrl::CoordinateSystem::eCoordinateSystem_Local;
    }
    else
    {
      rateCtrl.coordinateSystem = CIGI::V33::RateCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
    }
    rateCtrl.xLinearRate = static_cast<float>(rateControl.linearVelocity[0]);
    rateCtrl.yLinearRate = static_cast<float>(rateControl.linearVelocity[1]);
    rateCtrl.zLinearRate = static_cast<float>(rateControl.linearVelocity[2]);
    rateCtrl.rollAngularRate = rateControl.angularVelocity.roll.Value();
    rateCtrl.pitchAngularRate = rateControl.angularVelocity.pitch.Value();
    rateCtrl.yawAngularRate = rateControl.angularVelocity.yaw.Value();

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::RATE_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(rateControl.entityID.Value()) << "\n"
                        << "Apply to Articulated Part: " << "False" << "\n";

      if (rateControl.coordinateSystem == EObjectCoordinateSystem::PARENT)
      {
        args.sDataMessage << "Coordinate System: " << ConvertCigiObCoordSysToString(EObjectCoordinateSystem::PARENT) << "\n";
      }
      else if (rateControl.coordinateSystem == EObjectCoordinateSystem::LOCAL)
      {
        args.sDataMessage << "Coordinate System: " << ConvertCigiObCoordSysToString(EObjectCoordinateSystem::LOCAL) << "\n";
      }
      else
      {
        args.sDataMessage << "Coordinate System: " << ConvertCigiObCoordSysToString(EObjectCoordinateSystem::UNKNOWN) << "\n";
      }

      args.sDataMessage << "X Linear Velocity: " << std::to_string(static_cast<float>(rateControl.linearVelocity[0])) << "\n"
                        << "Y Linear Velocity: " << std::to_string(static_cast<float>(rateControl.linearVelocity[1])) << "\n"
                        << "Z Linear Velocity: " << std::to_string(static_cast<float>(rateControl.linearVelocity[2])) << "\n"
                        << "Roll Angular Velocity: " << std::to_string(rateControl.angularVelocity.fRoll.Value()) << "\n"
                        << "Pitch Angular Velocity: " << std::to_string(rateControl.angularVelocity.fPitch.Value()) << "\n"
                        << "Yaw Angular Velocity: " << std::to_string(rateControl.angularVelocity.fYaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      rateCtrl.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &rateCtrl, sizeof(CIGI::V33::RateCtrl));
    m_nSendBufferLength += sizeof(CIGI::V33::RateCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendRateControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& rateControl)
{
  // Can use SCigiArticulatedPartVelocityControl for Rate Control since they are the same
  try
  {
    CIGI::V33::RateCtrl rateCtrl;
    rateCtrl.entityId = rateControl.entityID.Value();
    rateCtrl.articulatedPartId = rateControl.articulatedPartID.Value();
    rateCtrl.applyToArticulatedPart = true;
    // Coordinate System shall be ignored if Apply to Articulated Part is set to True
    rateCtrl.coordinateSystem = rateCtrl.coordinateSystem = CIGI::V33::RateCtrl::CoordinateSystem::eCoordinateSystem_Local;
    rateCtrl.xLinearRate = static_cast<float>(rateControl.linearVelocity[0]);
    rateCtrl.yLinearRate = static_cast<float>(rateControl.linearVelocity[1]);
    rateCtrl.zLinearRate = static_cast<float>(rateControl.linearVelocity[2]);
    rateCtrl.rollAngularRate = rateControl.angularVelocity.roll.Value();
    rateCtrl.pitchAngularRate = rateControl.angularVelocity.pitch.Value();
    rateCtrl.yawAngularRate = rateControl.angularVelocity.yaw.Value();

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::RATE_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(rateControl.entityID.Value()) << "\n"
                        << "Articulated Part ID: " << std::to_string(rateControl.articulatedPartID.Value()) << "\n"
                        << "Apply to Articulated Part: " << "True" << "\n"
                        << "X Linear Velocity: " << std::to_string(static_cast<float>(rateControl.linearVelocity[0])) << "\n"
                        << "Y Linear Velocity: " << std::to_string(static_cast<float>(rateControl.linearVelocity[1])) << "\n"
                        << "Z Linear Velocity: " << std::to_string(static_cast<float>(rateControl.linearVelocity[2])) << "\n"
                        << "Roll Angular Velocity: " << std::to_string(rateControl.angularVelocity.fRoll.Value()) << "\n"
                        << "Pitch Angular Velocity: " << std::to_string(rateControl.angularVelocity.fPitch.Value()) << "\n"
                        << "Yaw Angular Velocity: " << std::to_string(rateControl.angularVelocity.fYaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      rateCtrl.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &rateCtrl, sizeof(CIGI::V33::RateCtrl));
    m_nSendBufferLength += sizeof(CIGI::V33::RateCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendViewControl(const sbio::cigi::SCigiViewControl& viewControl)
{
  try
  {
    CIGI::V33::ViewCtrl viewCtrl;
    viewCtrl.groupId = viewControl.viewGroupID.Value();
    viewCtrl.xOffsetEnable = viewControl.offsetEnabled[0];
    viewCtrl.yOffsetEnable = viewControl.offsetEnabled[1];
    viewCtrl.zOffsetEnable = viewControl.offsetEnabled[2];
    viewCtrl.rollEnable = viewControl.bRollEnabled;
    viewCtrl.pitchEnable = viewControl.bPitchEnabled;
    viewCtrl.yawEnable = viewControl.bYawEnabled;
    viewCtrl.viewId = viewControl.viewID.Value();
    viewCtrl.entityId = viewControl.entityID.Value();
    viewCtrl.xOffset = static_cast<float>(viewControl.offset[0]);
    viewCtrl.yOffset = static_cast<float>(viewControl.offset[1]);
    viewCtrl.zOffset = static_cast<float>(viewControl.offset[2]);
    viewCtrl.roll = viewControl.rotation.roll.Value();
    viewCtrl.pitch = viewControl.rotation.pitch.Value();
    viewCtrl.yaw = viewControl.rotation.yaw.Value();

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::VIEW_CONTROL) << "\n"
                        << "View ID: " << std::to_string(viewControl.viewID.Value()) << "\n"
                        << "Group ID: " << std::to_string(viewControl.viewGroupID.Value()) << "\n"
                        << "X Offset Enable: " << EnableToString(viewControl.offsetEnabled[0]) << "\n"
                        << "Y Offset Enable: " << EnableToString(viewControl.offsetEnabled[1]) << "\n"
                        << "Z Offset Enable: " << EnableToString(viewControl.offsetEnabled[2]) << "\n"
                        << "Roll Enable: " << EnableToString(viewControl.bRollEnabled) << "\n"
                        << "Pitch Enable: " << EnableToString(viewControl.bPitchEnabled) << "\n"
                        << "Yaw Enable: " << EnableToString(viewControl.bYawEnabled) << "\n"
                        << "Entity ID: " << std::to_string(viewControl.entityID.Value()) << "\n"
                        << "X Offset: " << std::to_string(viewControl.offset[0]) << "\n"
                        << "Y Offset: " << std::to_string(viewControl.offset[1]) << "\n"
                        << "Z Offset: " << std::to_string(viewControl.offset[2]) << "\n"
                        << "Roll: " << std::to_string(viewControl.rotation.fRoll.Value()) << "\n"
                        << "Pitch: " << std::to_string(viewControl.rotation.fPitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(viewControl.rotation.fYaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      viewCtrl.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &viewCtrl, sizeof(CIGI::V33::ViewCtrl));
    m_nSendBufferLength += sizeof(CIGI::V33::ViewCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendViewDefinition(const SCigiViewDefinition& viewDefinition)
{
  try
  {
    CIGI::V33::ViewDefinition viewDef;
    viewDef.viewId = viewDefinition.viewID.Value();
    viewDef.groupId = viewDefinition.viewGroupID.Value();
    viewDef.nearEnable = viewDefinition.bNearEnabled;
    viewDef.farEnable = viewDefinition.bFarEnabled;
    viewDef.leftEnable = viewDefinition.bLeftEnabled;
    viewDef.rightEnable = viewDefinition.bRightEnabled;
    viewDef.topEnable = viewDefinition.bTopEnabled;
    viewDef.bottomEnable = viewDefinition.bBottomEnabled;
    viewDef.mirrorMode = ConvertToMirrorModeGrp(viewDefinition.eMirrorMode);
    viewDef.pixelReplicationMode = static_cast<CIGI::V33::ViewDefinition::PixelReplicationMode>(viewDefinition.pixelReplicationMode.Value());

    if (viewDefinition.eProjectionMode == EProjectionMode::ORTHOGRAPHIC)
    {
      viewDef.projectionType = CIGI::V33::ViewDefinition::ProjectionType::eProjection_Orthographic;
    }
    else
    {
      viewDef.projectionType = CIGI::V33::ViewDefinition::ProjectionType::eProjection_Perspective;
    }

    viewDef.reorder = viewDefinition.bReorder;
    viewDef.viewType = viewDefinition.viewType.Value();
    viewDef.nearClip = viewDefinition.fNear;
    viewDef.farClip = viewDefinition.fFar;
    viewDef.left = viewDefinition.fLeft;
    viewDef.right = viewDefinition.fRight;
    viewDef.top = viewDefinition.fTop;
    viewDef.bottom = viewDefinition.fBottom;

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigaOpCodeV3::VIEW_DEFINITION) << "\n"
                        << "View ID: " << std::to_string(viewDefinition.viewID.Value()) << "\n"
                        << "Group ID: " << std::to_string(viewDefinition.viewGroupID.Value()) << "\n"
                        << "Near Enable: " << EnableToString(viewDefinition.bNearEnabled) << "\n"
                        << "Far Enable: " << EnableToString(viewDefinition.bFarEnabled) << "\n"
                        << "Left Enable: " << EnableToString(viewDefinition.bLeftEnabled) << "\n"
                        << "Right Enable: " << EnableToString(viewDefinition.bRightEnabled) << "\n"
                        << "Top Enable: " << EnableToString(viewDefinition.bTopEnabled) << "\n"
                        << "Bottom Enable: " << EnableToString(viewDefinition.bBottomEnabled) << "\n"
                        << "Mirror Mode: " << ConvertCigiMirrorModeToString(viewDefinition.eMirrorMode) << "\n"
                        << "Pixel Replication Mode: " << g_pScriptRuntime->GetPixelReplicationModeString(viewDefinition.pixelReplicationMode) << "\n";

      if (viewDefinition.eProjectionMode == EProjectionMode::ORTHOGRAPHIC)
      {
        args.sDataMessage << "Projection Type: " << "Perspective" << "\n";
      }
      else if (viewDefinition.eProjectionMode == EProjectionMode::PERSPECTIVE)
      {
        args.sDataMessage << "Projection Type: " << "Orthographic Parallel" << "\n";
      }
      else
      {
        args.sDataMessage << "Projection Type: " << "Unknown" << "\n";
      }

      if (viewDefinition.bReorder)
      {
        args.sDataMessage << "Reorder: " << "Bring to Top" << "\n";
      }
      else
      {
        args.sDataMessage << "Reorder: " << "No Reorder" << "\n";
      }

      args.sDataMessage << "View Type: " << std::to_string(viewDefinition.viewType.Value()) << "\n"
                        << "Near: " << std::to_string(viewDefinition.fNear) << "\n"
                        << "Far: " << std::to_string(viewDefinition.fFar) << "\n"
                        << "Left: " << std::to_string(viewDefinition.fLeft) << "\n"
                        << "Right: " << std::to_string(viewDefinition.fRight) << "\n"
                        << "Top: " << std::to_string(viewDefinition.fTop) << "\n"
                        << "Bottom: " << std::to_string(viewDefinition.fBottom);
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      viewDef.doByteSwapping();
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &viewDef, sizeof(CIGI::V33::ViewDefinition));
    m_nSendBufferLength += sizeof(CIGI::V33::ViewDefinition);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendWeatherControl(sbio::GlobalLayeredWeatherID globalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  // Global
  try
  {
    CIGI::V33::WeatherCtrl weatherCtrl;
    weatherCtrl.layerId = globalLayerWeatherID.Value();
    weatherCtrl.humidity = ConvertToCigiIntPercentage(weatherCondition.humidity).Value();
    weatherCtrl.weatherEnable = weatherCondition.bWeatherEnabled;
    weatherCtrl.randomWindsEnable = weatherCondition.bRandomWindsEnabled;
    weatherCtrl.randomLightningEnable = weatherCondition.bRandomLightningEnabled;
    weatherCtrl.cloudType = static_cast<CIGI::V33::WeatherCtrl::CloudType>(weatherCondition.cloudType.Value());
    weatherCtrl.scope = CIGI::V33::WeatherCtrl::Scope::eScope_Global;
    weatherCtrl.severity = weatherCondition.severity.Value();
    weatherCtrl.airTemperature = weatherCondition.fAirTemperature.Value();
    weatherCtrl.visibilityRange = weatherCondition.fVisibilityRange;
    weatherCtrl.coverage = ConvertToCigiFloatPercentage(weatherCondition.coverage).Value();
    weatherCtrl.baseElevation = spatialWeatherCondition.fBaseElevation;
    weatherCtrl.thickness = spatialWeatherCondition.fThickness;
    weatherCtrl.horizontalWindSpeed = weatherCondition.HorizontalWindSpeed;
    weatherCtrl.verticalWindSpeed = weatherCondition.VerticalWindSpeed;
    weatherCtrl.windDirection = weatherCondition.WindDirection;
    weatherCtrl.barometricPressure = weatherCondition.fBarometricPressure;
    weatherCtrl.aerosolConcentration = weatherCondition.fAerosolConcentration;

    if (m_bByteSwap)
    {
      weatherCtrl.doByteSwapping();
    }

    Pack(&weatherCtrl, sizeof(CIGI::V33::WeatherCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendWeatherControl(RegionID regionID, sbio::RegionalLayeredWeatherID regionalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  // Region
  try
  {
    CIGI::V33::WeatherCtrl weatherCtrl;
    weatherCtrl.entityRegionId = regionID.Value();
    weatherCtrl.layerId = regionalLayerWeatherID.Value();
    weatherCtrl.humidity = ConvertToCigiIntPercentage(weatherCondition.humidity).Value();
    weatherCtrl.weatherEnable = weatherCondition.bWeatherEnabled;
    weatherCtrl.randomWindsEnable = weatherCondition.bRandomWindsEnabled;
    weatherCtrl.randomLightningEnable = weatherCondition.bRandomLightningEnabled;
    weatherCtrl.cloudType = static_cast<CIGI::V33::WeatherCtrl::CloudType>(weatherCondition.cloudType.Value());
    weatherCtrl.scope = CIGI::V33::WeatherCtrl::Scope::eScope_Regional;
    weatherCtrl.severity = weatherCondition.severity.Value();
    weatherCtrl.airTemperature = weatherCondition.fAirTemperature.Value();
    weatherCtrl.visibilityRange = weatherCondition.fVisibilityRange;
    weatherCtrl.coverage = ConvertToCigiFloatPercentage(weatherCondition.coverage).Value();
    weatherCtrl.baseElevation = spatialWeatherCondition.fBaseElevation;
    weatherCtrl.thickness = spatialWeatherCondition.fThickness;
    weatherCtrl.horizontalWindSpeed = weatherCondition.HorizontalWindSpeed;
    weatherCtrl.verticalWindSpeed = weatherCondition.VerticalWindSpeed;
    weatherCtrl.windDirection = weatherCondition.WindDirection;
    weatherCtrl.barometricPressure = weatherCondition.fBarometricPressure;
    weatherCtrl.aerosolConcentration = weatherCondition.fAerosolConcentration;

    if (m_bByteSwap)
    {
      weatherCtrl.doByteSwapping();
    }

    Pack(&weatherCtrl, sizeof(CIGI::V33::WeatherCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV3_3::SendWeatherControl(sbio::EntityID entityID, const sbio::cigi::SCigiWeatherCondition& weatherCondition)
{
  // Entity
  try
  {
    CIGI::V33::WeatherCtrl weatherCtrl;
    weatherCtrl.entityRegionId = entityID.Value();
    weatherCtrl.humidity = ConvertToCigiIntPercentage(weatherCondition.humidity).Value();
    weatherCtrl.weatherEnable = weatherCondition.bWeatherEnabled;

    // need to setup: Scud Enable, since 3.3 doesnt use top and bottom
    // weatherCtrl.SetScudEn();

    weatherCtrl.randomWindsEnable = weatherCondition.bRandomWindsEnabled;
    weatherCtrl.randomLightningEnable = weatherCondition.bRandomLightningEnabled;
    weatherCtrl.cloudType = static_cast<CIGI::V33::WeatherCtrl::CloudType>(weatherCondition.cloudType.Value());
    weatherCtrl.scope = CIGI::V33::WeatherCtrl::Scope::eScope_Entity;
    weatherCtrl.severity = weatherCondition.severity.Value();
    weatherCtrl.airTemperature = weatherCondition.fAirTemperature.Value();
    weatherCtrl.visibilityRange = weatherCondition.fVisibilityRange;

    // need to setup: Scud Frequency, since 3.3 doesnt use top and bottom
    // weatherCtrl.SetScudFreq();

    weatherCtrl.coverage = ConvertToCigiFloatPercentage(weatherCondition.coverage).Value();

    weatherCtrl.horizontalWindSpeed = weatherCondition.HorizontalWindSpeed;
    weatherCtrl.verticalWindSpeed = weatherCondition.VerticalWindSpeed;
    weatherCtrl.windDirection = weatherCondition.WindDirection;
    weatherCtrl.barometricPressure = weatherCondition.fBarometricPressure;
    weatherCtrl.aerosolConcentration = weatherCondition.fAerosolConcentration;

    /*if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::WEATHER_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(entityID.Value()) << "\n"
                        << "Humidity: " << std::to_string(ConvertToCigiIntPercentage(weatherCondition.humidity).Value()) << "%" << "\n"
                        << "Weather Enable: " << EnableToString(weatherCondition.bWeatherEnabled) << "\n"
                        //<< "Scud Enable: " << "" << "\n"
                        << "Random Winds Enable: " << EnableToString(weatherCondition.bRandomWindsEnabled) << "\n"
                        << "Random Lightning Enable: " << EnableToString(weatherCondition.bRandomLightningEnabled) << "\n"
                        << "Cloud Type: " << g_pScriptRuntime->GetCloudTypeString(weatherCondition.cloudType) << "\n"
                        << "Scope: " << "Entity" << "\n"
                        << "Severity: " << std::to_string(weatherCondition.severity.Value()) << "\n"
                        << "Top Scud Enable: " << EnableToString(weatherCondition.bTopScudEnabled) << "\n"
                        << "Air Temperature: " << std::to_string(weatherCondition.fAirTemperature) << "°C" << "\n"
                        << "Visibility Range: " << std::to_string(weatherCondition.fVisibilityRange) << "\n"
                        //<< "Scud Frequency: " << "" << "\n"
                        << "Coverage: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.coverage).Value()) << "\n"
                        << "Horizontal Wind Speed: " << std::to_string(weatherCondition.HorizontalWindSpeed) << "\n"
                        << "Vertical Wind Speed: " << std::to_string(weatherCondition.VerticalWindSpeed) << "\n"
                        << "Wind Direction: " << std::to_string(weatherCondition.WindDirection) << "\n"
                        << "Barometric Pressure: " << std::to_string(weatherCondition.fBarometricPressure) << "\n"
                        << "Aerosol Concentration: " << std::to_string(weatherCondition.fAerosolConcentration);
      Event::Raise<HostCigiEvent>(args);
    }*/

    if (m_bByteSwap)
    {
      weatherCtrl.doByteSwapping();
    }

    Pack(&weatherCtrl, sizeof(CIGI::V33::WeatherCtrl));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

bool CHostSessionV3_3::SetIGControl(CigiDatabaseNumber databaseNumber, bool bEntityTypeSubstitutionEnabled, EIGMode eIGMode, bool bSmoothingEnabled)
{
  if (!CHostSession::SetIGControl(databaseNumber, bEntityTypeSubstitutionEnabled, eIGMode, bSmoothingEnabled))
  {
    return false;
  }

  m_IGControl.igMode = ToIGMode(m_DesiredIGMode);
  m_IGControl.extrapolationEnable = bSmoothingEnabled;

  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
