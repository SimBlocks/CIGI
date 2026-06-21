//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
#include "HostSessionV4.h"
#include "GlobalHeaders/CommonTypes.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiEvent.h"
#include "HostCigiLib/HostCigiLib.h"
#include "UtilitiesLib/Buffer.h"
#include "UtilitiesLib/Endian.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "libCIGI/Packets/4_0/AccelerationCtrl.h"
#include "libCIGI/Packets/4_0/AerosolConcentrationResponse.h"
#include "libCIGI/Packets/4_0/AnimationCtrl.h"
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
#include "libCIGI/Packets/4_0/EntityPosition.h"
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
#include "libCIGI/Packets/4_0/SymbolTexturedCircleDefinition.h"
#include "libCIGI/Packets/4_0/SymbolTexturedPolygonDefinition.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/VelocityCtrl.h"
#include "libCIGI/Packets/4_0/ViewCtrl.h"
#include "libCIGI/Packets/4_0/ViewDefinition.h"
#include "libCIGI/Packets/4_0/WaveCtrl.h"
#include "libCIGI/Packets/4_0/WeatherConditionsResponse.h"
#include "libCIGI/Packets/4_0/WeatherCtrl.h"
#include "ScriptRuntime.h"
#include "UtilitiesLib/Logger.h"

using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;
using namespace sbio::symbol;
using namespace std;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;
extern CScriptRuntime* g_pScriptRuntime;

template <typename TPacket>
TPacket GetPacketV4(const uint8_t* buffer, bool bByteSwap)
{
  TPacket packet;
  memcpy(&packet, buffer, sizeof(TPacket));

  if (bByteSwap)
  {
    packet.doByteSwapping();
  }

  return packet;
}

template <typename TPacket>
void ByteSwapPacketV4(TPacket& packet)
{
  CEndian::Swap(packet.size);
  uint16_t nPacketId = packet.id;
  CEndian::Swap(nPacketId);
  packet.id = nPacketId;
  packet.doByteSwapping();
}

template <typename TPacket>
void PackPacketV4(CHostSession* pSession, const TPacket& packet, bool bByteSwap)
{
  TPacket tempPacket = packet;

  if (bByteSwap)
  {
    ByteSwapPacketV4(tempPacket);
  }

  pSession->Pack(&tempPacket, sizeof(TPacket));
}

CHostSessionV4::CHostSessionV4()
{
  m_IGControl.dbNumber = hostSetupOptions.defaultDatabaseID.Value();
  m_DesiredIGMode = EIGMode::RESET;
  m_IGControl.smoothingEnable = false;

  m_PacketHandlerFunctions[ECigiOpCodeV4::START_OF_FRAME] = &CHostSessionV4::ParseStartOfFramePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::HAT_HOT_RESPONSE] = &CHostSessionV4::ParseHatHotResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::HAT_HOT_EXTENDED_RESPONSE] = &CHostSessionV4::ParseHatHotExtendedResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::LINE_OF_SIGHT_RESPONSE] = &CHostSessionV4::ParseLineOfSightResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::LINE_OF_SIGHT_EXTENDED_RESPONSE] = &CHostSessionV4::ParseLineOfSightExtendedResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SENSOR_RESPONSE] = &CHostSessionV4::ParseSensorResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::SENSOR_EXTENDED_RESPONSE] = &CHostSessionV4::ParseSensorExtendedResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::POSITION_RESPONSE] = &CHostSessionV4::ParsePositionResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::WEATHER_CONDITIONS_RESPONSE] = &CHostSessionV4::ParseWeatherConditionsResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::AEROSOL_CONCENTRATION_RESPONSE] = &CHostSessionV4::ParseAerosolConcentrationResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_RESPONSE] = &CHostSessionV4::ParseMaritimeSurfaceConditionsResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE] = &CHostSessionV4::ParseTerrestrialSurfaceConditionsResponsePacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_NOTIFICATION] = &CHostSessionV4::ParseCollisionDetectionSegmentNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_NOTIFICATION] = &CHostSessionV4::ParseCollisionDetectionVolumeNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::ANIMATION_STOP_NOTIFICATION] = &CHostSessionV4::ParseAnimationStopNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::EVENT_NOTIFICATION] = &CHostSessionV4::ParseEventNotificationPacket;
  m_PacketHandlerFunctions[ECigiOpCodeV4::IMAGE_GENERATOR_MESSAGE] = &CHostSessionV4::ParseImageGeneratorMessagePacket;
}

void CHostSessionV4::ParseStartOfFramePacket(uint8_t* buffer)
{
  CIGI::V40::SoF startOfFramePacket = GetPacketV4<CIGI::V40::SoF>(buffer, m_bByteSwap);

  m_ActualIGMode = ConvertToIGMode(static_cast<CIGI::V40::SoF::IGMode>(startOfFramePacket.igMode));
  m_LastReceivedIGFrame = FrameNumber(startOfFramePacket.igFrameNumber);
  DatabaseID databaseID = DatabaseID(startOfFramePacket.dbNumber);

  HostCigiStartOfFrameEventArgs startOfFrameArgs;
  startOfFrameArgs.startOfFrame.eVersion = ECigiVersion::VERSION_4_0;
  startOfFrameArgs.startOfFrame.databaseID = CigiDatabaseNumber(startOfFramePacket.dbNumber);
  startOfFrameArgs.startOfFrame.eIGMode = m_ActualIGMode;
  startOfFrameArgs.startOfFrame.bTimestampValid = startOfFramePacket.timestampValid != 0;
  startOfFrameArgs.startOfFrame.bEarthReferenceModel = startOfFramePacket.earthReferenceModel == CIGI::V40::SoF::EarthReferenceModel::eEarthReferenceModel_HostDefined;
  startOfFrameArgs.startOfFrame.igFrameNumber = m_LastReceivedIGFrame;
  startOfFrameArgs.startOfFrame.microseconds = Microsecond(startOfFramePacket.timestamp);
  startOfFrameArgs.startOfFrame.lastHostFrameNumber = FrameNumber(startOfFramePacket.lastHostFrameNumber);
  startOfFrameArgs.startOfFrame.bOverframing = startOfFramePacket.overframing != 0;
  startOfFrameArgs.startOfFrame.bPagingTerrain = startOfFramePacket.paging != 0;
  startOfFrameArgs.startOfFrame.bExcessiveVariableLengthData = startOfFramePacket.IGCondition3 != 0;
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

void CHostSessionV4::ParseHatHotResponsePacket(uint8_t* buffer)
{
  CIGI::V40::HATHOTResponse hatHotResponsePacket = GetPacketV4<CIGI::V40::HATHOTResponse>(buffer, m_bByteSwap);

  // Raise a message event to log and display the response
  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::HAT_HOT_RESPONSE) << "\n"
                       << "Valid: " << ValidToString(hatHotResponsePacket.valid) << "\n"
                       << "Response Type: " << ConvertResponseTypeToString(static_cast<CIGI::V40::HATHOTResponse::Type>(hatHotResponsePacket.responseType)) << "\n"
                       << "Host Frame Number LSN: " << std::to_string(hatHotResponsePacket.hostFrameNumberLSN) << "\n"
                       << "HAT/HOT ID: " << std::to_string(hatHotResponsePacket.HATHOTId) << "\n"
                       << "HAT/HOT: ";

  if (hatHotResponsePacket.responseType == CIGI::V40::HATHOTResponse::Type::eType_HAT)
  {
    msgArgs.sDataMessage << std::to_string(hatHotResponsePacket.eType_HAT);
  }
  else if (hatHotResponsePacket.responseType == CIGI::V40::HATHOTResponse::Type::eType_HOT)
  {
    msgArgs.sDataMessage << std::to_string(hatHotResponsePacket.eType_HOT);
  }
  msgArgs.sDataMessage << "\nHeight: " << hatHotResponsePacket.height;
  Event::Raise<HostCigiEvent>(msgArgs);

  // Raise the response event so that host chai scripts can use this data
  if (hatHotResponsePacket.responseType == CIGI::V40::HATHOTResponse::Type::eType_HAT)
  {
    HostCigiHatResponseEventArgs hatArgs;
    hatArgs.hatResponse.bValid = hatHotResponsePacket.valid;
    hatArgs.hatResponse.hostFrameLSN = hatHotResponsePacket.hostFrameNumberLSN;
    hatArgs.hatResponse.HATHOTID = sbio::HATHOTID(hatHotResponsePacket.HATHOTId);
    hatArgs.hatResponse.heightAboveTerrain = hatHotResponsePacket.height;
    Event::Raise<HostCigiEvent>(hatArgs);
  }
  else
  {
    HostCigiHotResponseEventArgs hotArgs;
    hotArgs.hotResponse.bValid = hatHotResponsePacket.valid;
    hotArgs.hotResponse.hostFrameLSN = hatHotResponsePacket.hostFrameNumberLSN;
    hotArgs.hotResponse.HATHOTID = sbio::HATHOTID(hatHotResponsePacket.HATHOTId);
    hotArgs.hotResponse.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(hatHotResponsePacket.height);
    Event::Raise<HostCigiEvent>(hotArgs);
  }
}

void CHostSessionV4::ParseHatHotExtendedResponsePacket(uint8_t* buffer)
{
  CIGI::V40::HATHOTExtendedResponse hatHotExtendedResponsePacket = GetPacketV4<CIGI::V40::HATHOTExtendedResponse>(buffer, m_bByteSwap);

  // Raise a message event to log and display the response
  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::HAT_HOT_EXTENDED_RESPONSE) << "\n"
                       << "Valid: " << ValidToString(hatHotExtendedResponsePacket.valid) << "\n"
                       << "Host Frame Number LSN: " << std::to_string(hatHotExtendedResponsePacket.hostFrameNumberLSN) << "\n"
                       << "HAT/HOT ID: " << std::to_string(hatHotExtendedResponsePacket.HATHOTId) << "\n"
                       << "HAT: " << std::to_string(hatHotExtendedResponsePacket.HAT) << "\n"
                       << "HOT: " << std::to_string(hatHotExtendedResponsePacket.HOT) << "\n"
                       << "Material Code: " << std::to_string(hatHotExtendedResponsePacket.materialCode) << "\n"
                       << "Normal Vector Azimuth: " << std::to_string(hatHotExtendedResponsePacket.normalVectorAzimuth) << "\n"
                       << "Normal Vector Elevation" << std::to_string(hatHotExtendedResponsePacket.normalVectorElevation);

  Event::Raise<HostCigiEvent>(msgArgs);

  // Raise the response event so that host chai scripts can use this data
  HostCigiHatHotExtendedResponseEventArgs hatHotArgs;
  hatHotArgs.hatHotExtendedResponse.bValid = hatHotExtendedResponsePacket.valid;
  hatHotArgs.hatHotExtendedResponse.hostFrameLSN = hatHotExtendedResponsePacket.hostFrameNumberLSN;
  hatHotArgs.hatHotExtendedResponse.HATHOTID = sbio::HATHOTID(hatHotExtendedResponsePacket.HATHOTId);
  hatHotArgs.hatHotExtendedResponse.heightAboveTerrain = hatHotExtendedResponsePacket.HAT;
  hatHotArgs.hatHotExtendedResponse.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(hatHotExtendedResponsePacket.HOT);
  hatHotArgs.hatHotExtendedResponse.materialCode = MaterialID(hatHotExtendedResponsePacket.materialCode);
  hatHotArgs.hatHotExtendedResponse.normalVectorAzimuth = Degrees180(hatHotExtendedResponsePacket.normalVectorAzimuth);
  hatHotArgs.hatHotExtendedResponse.normalVectorElevation = Degrees90(hatHotExtendedResponsePacket.normalVectorElevation);
  Event::Raise<HostCigiEvent>(hatHotArgs);
}

void CHostSessionV4::ParseLineOfSightResponsePacket(uint8_t* buffer)
{
  CIGI::V40::LineOfSightResponse lineOfSightResponsePacket = GetPacketV4<CIGI::V40::LineOfSightResponse>(buffer, m_bByteSwap);

  // Raise a message event to log and display the response
  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_RESPONSE) << "\n"
                       << "LOS ID: " << std::to_string(lineOfSightResponsePacket.LOSId) << "\n"
                       << "Entity ID: " << std::to_string(lineOfSightResponsePacket.entityId) << "\n"
                       << "Valid: " << ValidToString(lineOfSightResponsePacket.valid) << "\n"
                       << "Entity ID Valid: " << ValidToString(lineOfSightResponsePacket.entityIdValid) << "\n"
                       << "Visible: " << VisibleToString(lineOfSightResponsePacket.visible) << "\n"
                       << "Host Frame Number LSN: " << std::to_string(lineOfSightResponsePacket.hostFrameNumberLSN) << "\n"
                       << "Response Count: " << std::to_string(lineOfSightResponsePacket.responseCount) << "\n"
                       << "Range: " << std::to_string(lineOfSightResponsePacket.range) << "\n";
  Event::Raise<HostCigiEvent>(msgArgs);

  if (lineOfSightResponsePacket.entityIdValid == 1)
  {
    HostCigiLineOfSightEntityResponseEventArgs lineOfSightArgs;
    lineOfSightArgs.lineOfSightEntityResponse.lineOfSightRequestID = LineOfSightRequestID(lineOfSightResponsePacket.LOSId);
    lineOfSightArgs.lineOfSightEntityResponse.entityID = EntityID(lineOfSightResponsePacket.entityId);
    lineOfSightArgs.lineOfSightEntityResponse.bValid = lineOfSightResponsePacket.valid == 1;
    lineOfSightArgs.lineOfSightEntityResponse.bVisible = lineOfSightResponsePacket.visible == CIGI::V40::LineOfSightResponse::Visible::eVisible_Visible;
    lineOfSightArgs.lineOfSightEntityResponse.hostFrameLSN = lineOfSightResponsePacket.hostFrameNumberLSN;
    lineOfSightArgs.lineOfSightEntityResponse.responseCount = lineOfSightResponsePacket.responseCount;
    lineOfSightArgs.lineOfSightEntityResponse.dRange = lineOfSightResponsePacket.range;
    Event::Raise<HostCigiEvent>(lineOfSightArgs);
  }
  else
  {
    HostCigiLineOfSightResponseEventArgs lineOfSightArgs;
    lineOfSightArgs.lineOfSightResponse.lineOfSightRequestID = LineOfSightRequestID(lineOfSightResponsePacket.LOSId);
    lineOfSightArgs.lineOfSightResponse.bValid = lineOfSightResponsePacket.valid == 1;
    lineOfSightArgs.lineOfSightResponse.hostFrameLSN = lineOfSightResponsePacket.hostFrameNumberLSN;
    lineOfSightArgs.lineOfSightResponse.responseCount = lineOfSightResponsePacket.responseCount;
    lineOfSightArgs.lineOfSightResponse.dRange = lineOfSightResponsePacket.range;
    Event::Raise<HostCigiEvent>(lineOfSightArgs);
  }
}

void CHostSessionV4::ParseLineOfSightExtendedResponsePacket(uint8_t* buffer)
{
  CIGI::V40::LineOfSightExtendedResponse lineOfSightExtendedResponsePacket = GetPacketV4<CIGI::V40::LineOfSightExtendedResponse>(buffer, m_bByteSwap);
  const LineOfSightRequestID lineOfSightRequestID(lineOfSightExtendedResponsePacket.LOSId);
  const ETopLevelCoordinateSystem eResponseCoordinateSystem = GetLineOfSightRequestCoordinateSystem(lineOfSightRequestID);

  if (eResponseCoordinateSystem == ETopLevelCoordinateSystem::UNKNOWN)
  {
    g_HostCigiLibGlobals.pLogger->LogWarning("Received Line of Sight Extended Response with unknown requested coordinate system.");
    return;
  }

  const bool bGeodeticCoordinatesResponse = eResponseCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC || (lineOfSightExtendedResponsePacket.entityIdValid == 0);

  // Raise a message event to log and display the response
  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_EXTENDED_RESPONSE) << "\n"
                       << "LOS ID: " << std::to_string(lineOfSightExtendedResponsePacket.LOSId) << "\n"
                       << "Entity ID: " << std::to_string(lineOfSightExtendedResponsePacket.entityId) << "\n"
                       << "Valid: " << ValidToString(lineOfSightExtendedResponsePacket.valid) << "\n"
                       << "Entity ID Valid: " << ValidToString(lineOfSightExtendedResponsePacket.entityIdValid) << "\n"
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
                       << "Normal Vector Elevation: " << std::to_string(lineOfSightExtendedResponsePacket.normalVectorElevation);
  Event::Raise<HostCigiEvent>(msgArgs);

  if (lineOfSightExtendedResponsePacket.entityIdValid == 1)
  {
    if (bGeodeticCoordinatesResponse)
    {
      HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs lineOfSightArgs;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.lineOfSightRequestID = lineOfSightRequestID;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.entityID = EntityID(lineOfSightExtendedResponsePacket.entityId);
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid == 1;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid == 1;
      lineOfSightArgs.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible == CIGI::V40::LineOfSightExtendedResponse::Visible::eVisible_Visible;
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
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid == 1;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid == 1;
      lineOfSightArgs.lineOfSightExtendedEntityCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible == CIGI::V40::LineOfSightExtendedResponse::Visible::eVisible_Visible;
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
    // no valid entity. reponse will always be in geodetic coordinates
    HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs lineOfSightArgs;
    lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.lineOfSightRequestID = lineOfSightRequestID;
    lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.bValid = lineOfSightExtendedResponsePacket.valid == 1;
    lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.bRangeValid = lineOfSightExtendedResponsePacket.rangeValid == 1;
    lineOfSightArgs.lineOfSightExtendedGeodeticCoordinatesResponse.bVisible = lineOfSightExtendedResponsePacket.visible == CIGI::V40::LineOfSightExtendedResponse::Visible::eVisible_Visible;
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
}

void CHostSessionV4::ParseSensorResponsePacket(uint8_t* buffer)
{
  CIGI::V40::SensorResponse sensorResponse = GetPacketV4<CIGI::V40::SensorResponse>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::SENSOR_RESPONSE) << "\n"
                    << "Sensor ID: " << std::to_string(sensorResponse.sensorId) << "\n"
                    << "Sensor Status: " << ConvertSensorStatusToString(static_cast<CIGI::V40::SensorResponse::SensorStatus>(sensorResponse.sensorStatus)) << "\n"
                    << "View ID: " << std::to_string(sensorResponse.viewId) << "\n"
                    << "Gate X Size: " << std::to_string(sensorResponse.gateXSize) << "\n"
                    << "Gate Y Size: " << std::to_string(sensorResponse.gateYSize) << "\n"
                    << "Gate X Position: " << std::to_string(sensorResponse.gateXPosition) << "\n"
                    << "Gate Y Position: " << std::to_string(sensorResponse.gateYPosition) << "\n"
                    << "Host Frame Number: " << std::to_string(sensorResponse.hostFrameCounter);

  Event::Raise<HostCigiEvent>(args);

  HostCigiSensorResponseEventArgs sensorArgs;
  sensorArgs.sensorResponse.sensorID = SensorID(sensorResponse.sensorId);
  sensorArgs.sensorResponse.viewID = ViewID(sensorResponse.viewId);
  sensorArgs.sensorResponse.gateSize[0] = static_cast<float>(sensorResponse.gateXSize);
  sensorArgs.sensorResponse.gateSize[1] = static_cast<float>(sensorResponse.gateYSize);
  sensorArgs.sensorResponse.gatePosition[0] = sensorResponse.gateXPosition;
  sensorArgs.sensorResponse.gatePosition[1] = sensorResponse.gateYPosition;
  sensorArgs.sensorResponse.hostFrameNumber = FrameNumber(sensorResponse.hostFrameCounter);

  if (sensorResponse.sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget)
  {
    sensorArgs.sensorResponse.eSensorStatus = ESensorStatus::SEARCHING_FOR_TARGET;
  }
  else if (sensorResponse.sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget)
  {
    sensorArgs.sensorResponse.eSensorStatus = ESensorStatus::TRACKING_TARGET;
  }
  else if (sensorResponse.sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock)
  {
    sensorArgs.sensorResponse.eSensorStatus = ESensorStatus::IMPENDING_BREAKLOCK;
  }
  else if (sensorResponse.sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_Breaklock)
  {
    sensorArgs.sensorResponse.eSensorStatus = ESensorStatus::BREAKLOCK;
  }

  Event::Raise<HostCigiEvent>(sensorArgs);
}

void CHostSessionV4::ParseSensorExtendedResponsePacket(uint8_t* buffer)
{
  CIGI::V40::SensorExtendedResponse sensorExtendedResponsePacket = GetPacketV4<CIGI::V40::SensorExtendedResponse>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::SENSOR_EXTENDED_RESPONSE) << "\n"
                    << "View ID: " << std::to_string(sensorExtendedResponsePacket.viewId) << "\n"
                    << "Entity ID: " << std::to_string(sensorExtendedResponsePacket.entityId) << "\n"
                    << "Sensor ID: " << std::to_string(sensorExtendedResponsePacket.sensorId) << "\n"
                    << "Sensor Status: " << ConvertSensorStatusToString(static_cast<CIGI::V40::SensorResponse::SensorStatus>(sensorExtendedResponsePacket.sensorStatus)) << "\n"
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
    sensorArgs.sensorExtendedEntityResponse.sensorID = SensorID(sensorExtendedResponsePacket.sensorId);
    sensorArgs.sensorExtendedEntityResponse.gateSize[0] = static_cast<float>(sensorExtendedResponsePacket.gateXSize);
    sensorArgs.sensorExtendedEntityResponse.gateSize[1] = static_cast<float>(sensorExtendedResponsePacket.gateYSize);
    sensorArgs.sensorExtendedEntityResponse.gatePosition[0] = sensorExtendedResponsePacket.gateXPosition;
    sensorArgs.sensorExtendedEntityResponse.gatePosition[1] = sensorExtendedResponsePacket.gateYPosition;
    sensorArgs.sensorExtendedEntityResponse.hostFrameNumber = FrameNumber(sensorExtendedResponsePacket.hostFrameCounter);
    sensorArgs.sensorExtendedEntityResponse.trackPoint.latitude = Latitude(sensorExtendedResponsePacket.trackPointLatitude);
    sensorArgs.sensorExtendedEntityResponse.trackPoint.longitude = Longitude(sensorExtendedResponsePacket.trackPointLongitude);
    sensorArgs.sensorExtendedEntityResponse.trackPoint.altitude = HeightRelativeToWGS84Ellipsoid(sensorExtendedResponsePacket.trackPointAltitude);

    if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_SearchingForTarget)
    {
      sensorArgs.sensorExtendedEntityResponse.eSensorStatus = ESensorStatus::SEARCHING_FOR_TARGET;
    }
    else if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_TrackingTarget)
    {
      sensorArgs.sensorExtendedEntityResponse.eSensorStatus = ESensorStatus::TRACKING_TARGET;
    }
    else if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_ImpendingBreaklock)
    {
      sensorArgs.sensorExtendedEntityResponse.eSensorStatus = ESensorStatus::IMPENDING_BREAKLOCK;
    }
    else if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_Breaklock)
    {
      sensorArgs.sensorExtendedEntityResponse.eSensorStatus = ESensorStatus::BREAKLOCK;
    }

    sensorArgs.sensorExtendedEntityResponse.entityID = EntityID(sensorExtendedResponsePacket.entityId);

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

    if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_SearchingForTarget)
    {
      sensorArgs.sensorExtendedResponse.eSensorStatus = ESensorStatus::SEARCHING_FOR_TARGET;
    }
    else if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_TrackingTarget)
    {
      sensorArgs.sensorExtendedResponse.eSensorStatus = ESensorStatus::TRACKING_TARGET;
    }
    else if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_ImpendingBreaklock)
    {
      sensorArgs.sensorExtendedResponse.eSensorStatus = ESensorStatus::IMPENDING_BREAKLOCK;
    }
    else if (sensorExtendedResponsePacket.sensorStatus == CIGI::V40::SensorExtendedResponse::SensorStatus::eSensorStatus_Breaklock)
    {
      sensorArgs.sensorExtendedResponse.eSensorStatus = ESensorStatus::BREAKLOCK;
    }

    Event::Raise<HostCigiEvent>(sensorArgs);
  }
}

void CHostSessionV4::ParsePositionResponsePacket(uint8_t* buffer)
{
  CIGI::V40::PositionResponse positionResponsePacket = GetPacketV4<CIGI::V40::PositionResponse>(buffer, m_bByteSwap);

  {
    HostCigiDataMessageEventArgs args;

    args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::POSITION_RESPONSE) << "\n"
                      << "Articulated Part ID: " << std::to_string(positionResponsePacket.articulatedPartId) << "\n"
                      << "Object Class: " << ConvertObjectClassToString(static_cast<CIGI::V40::PositionResponse::ObjectClass>(positionResponsePacket.objectClass)) << "\n"
                      << "Coordinate System: " << ConvertCoordSysGeoParSubToString(static_cast<CIGI::V40::PositionResponse::CoordinateSystem>(positionResponsePacket.coordinateSystem)) << "\n"
                      << "Object ID: " << std::to_string(positionResponsePacket.objectId) << "\n";

    if (positionResponsePacket.coordinateSystem == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic)
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

  if (positionResponsePacket.coordinateSystem == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic)
  {
    SPositionResponseGeodeticCoordinates positionResponse;
    HostCigiPositionResponseEventArgs args(positionResponse);
    args.ePositionResponseType = EPositionResponseType::GEODETIC;

    positionResponse.geodeticCoordinates.latitude = Latitude(positionResponsePacket.latitudeXOffset);
    positionResponse.geodeticCoordinates.longitude = Longitude(positionResponsePacket.longitudeYOffset);
    positionResponse.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(positionResponsePacket.altitudeZOffset);
    positionResponse.eObjectClass = ConvertObjectClass(static_cast<CIGI::V40::PositionResponse::ObjectClass>(positionResponsePacket.objectClass));
    positionResponse.objectID = positionResponsePacket.objectId;
    positionResponse.rotation.roll = Degrees(positionResponsePacket.roll);
    positionResponse.rotation.pitch = Degrees(positionResponsePacket.pitch);
    positionResponse.rotation.yaw = Degrees(positionResponsePacket.yaw);

    Event::Raise<HostCigiEvent>(args);
  }
  else if (positionResponsePacket.coordinateSystem == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_ParentEntity)
  {
    SPositionResponseParentEntityCoordinates positionResponse;
    HostCigiPositionResponseEventArgs args(positionResponse);
    args.ePositionResponseType = EPositionResponseType::PARENT;

    positionResponse.offset[0] = positionResponsePacket.latitudeXOffset;
    positionResponse.offset[1] = positionResponsePacket.longitudeYOffset;
    positionResponse.offset[2] = positionResponsePacket.altitudeZOffset;
    positionResponse.eObjectClass = ConvertObjectClass(static_cast<CIGI::V40::PositionResponse::ObjectClass>(positionResponsePacket.objectClass));
    positionResponse.objectID = positionResponsePacket.objectId;
    positionResponse.rotation.roll = Degrees(positionResponsePacket.roll);
    positionResponse.rotation.pitch = Degrees(positionResponsePacket.pitch);
    positionResponse.rotation.yaw = Degrees(positionResponsePacket.yaw);

    Event::Raise<HostCigiEvent>(args);
  }
  else if (positionResponsePacket.coordinateSystem == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Submodel)
  {
    SPositionResponseArticulatedPartCoordinates positionResponse;
    HostCigiPositionResponseEventArgs args(positionResponse);
    args.ePositionResponseType = EPositionResponseType::ARTICULATED;

    positionResponse.offset[0] = positionResponsePacket.latitudeXOffset;
    positionResponse.offset[1] = positionResponsePacket.longitudeYOffset;
    positionResponse.offset[2] = positionResponsePacket.altitudeZOffset;
    positionResponse.articulatedPartID = ArticulatedPartID(positionResponsePacket.articulatedPartId);
    positionResponse.eObjectClass = ConvertObjectClass(static_cast<CIGI::V40::PositionResponse::ObjectClass>(positionResponsePacket.objectClass));
    positionResponse.objectID = positionResponsePacket.objectId;
    positionResponse.rotation.roll = Degrees(positionResponsePacket.roll);
    positionResponse.rotation.pitch = Degrees(positionResponsePacket.pitch);
    positionResponse.rotation.yaw = Degrees(positionResponsePacket.yaw);

    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::ParseWeatherConditionsResponsePacket(uint8_t* buffer)
{
  CIGI::V40::WeatherConditionsResponse weatherConditionsResponsePacket = GetPacketV4<CIGI::V40::WeatherConditionsResponse>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::WEATHER_CONDITIONS_RESPONSE) << "\n"
                    << "Request ID: " << std::to_string(weatherConditionsResponsePacket.requestId) << "\n"
                    << "Humidity: " << std::to_string(weatherConditionsResponsePacket.humidity) << "%"
                    << "\n"
                    << "Air Temperature: " << std::to_string(weatherConditionsResponsePacket.airTemperature) << "�C"
                    << "\n"
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

void CHostSessionV4::ParseAerosolConcentrationResponsePacket(uint8_t* buffer)
{
  CIGI::V40::AerosolConcentrationResponse aerosolConcentrationResponsePacket = GetPacketV4<CIGI::V40::AerosolConcentrationResponse>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::AEROSOL_CONCENTRATION_RESPONSE) << "\n"
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

void CHostSessionV4::ParseMaritimeSurfaceConditionsResponsePacket(uint8_t* buffer)
{
  CIGI::V40::MaritimeSurfaceConditionsResponse maritimeSurfaceConditionsResponsePacket = GetPacketV4<CIGI::V40::MaritimeSurfaceConditionsResponse>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_RESPONSE) << "\n"
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

void CHostSessionV4::ParseTerrestrialSurfaceConditionsResponsePacket(uint8_t* buffer)
{
  CIGI::V40::TerrestrialSurfaceConditionsResponse terrestrialSurfaceConditionsResponsePacket = GetPacketV4<CIGI::V40::TerrestrialSurfaceConditionsResponse>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE) << "\n"
                    << "Request ID: " << std::to_string(terrestrialSurfaceConditionsResponsePacket.requestId) << "\n"
                    << "Surface Condition ID: " << std::to_string(terrestrialSurfaceConditionsResponsePacket.surfaceConditionId) << "\n";

  Event::Raise<HostCigiEvent>(args);

  HostCigiTerrestrialSurfaceConditionsResponseEventArgs responseArgs;
  responseArgs.terrestrialSurfaceConditionsResponse.requestID = terrestrialSurfaceConditionsResponsePacket.requestId;
  responseArgs.terrestrialSurfaceConditionsResponse.surfaceConditionID = terrestrialSurfaceConditionsResponsePacket.surfaceConditionId;
  Event::Raise<HostCigiEvent>(responseArgs);
}

void CHostSessionV4::ParseCollisionDetectionSegmentNotificationPacket(uint8_t* buffer)
{
  CIGI::V40::CollisionDetectionSegmentNotification collisionDetectionSegmentNotificationPacket = GetPacketV4<CIGI::V40::CollisionDetectionSegmentNotification>(buffer, m_bByteSwap);

  // Raise a message event to log and display the response
  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_NOTIFICATION) << "\n"
                       << "Entity ID: " << std::to_string(collisionDetectionSegmentNotificationPacket.entityId) << "\n"
                       << "Contacted Entity ID: " << std::to_string(collisionDetectionSegmentNotificationPacket.contactedEntityId) << "\n"
                       << "Segment ID: " << std::to_string(collisionDetectionSegmentNotificationPacket.segmentId) << "\n";
  if (collisionDetectionSegmentNotificationPacket.collisionType == CIGI::V40::CollisionDetectionSegmentNotification::CollisionType::eCollisionType_NonEntity)
  {
    msgArgs.sDataMessage << "Collision Type: "
                         << "Non-Entity"
                         << "\n";
  }
  else
  {
    msgArgs.sDataMessage << "Collision Type: "
                         << "Entity"
                         << "\n";
  }
  msgArgs.sDataMessage << "Material Code: " << std::to_string(collisionDetectionSegmentNotificationPacket.materialCode) << "\n"
                       << "Intersection Distance: " << std::to_string(collisionDetectionSegmentNotificationPacket.intersectionDistance);
  Event::Raise<HostCigiEvent>(msgArgs);

  // Raise the response event so that host chai scripts can use this data
  if (collisionDetectionSegmentNotificationPacket.collisionType == CIGI::V40::CollisionDetectionSegmentNotification::CollisionType::eCollisionType_NonEntity)
  {
    HostCigiCollisionDetectionSegmentNotificationEventArgs collisionSegmentArgs;
    collisionSegmentArgs.collisionDetectionSegmentNotification.entityID = EntityID(collisionDetectionSegmentNotificationPacket.entityId);
    collisionSegmentArgs.collisionDetectionSegmentNotification.segmentID = SegmentID(collisionDetectionSegmentNotificationPacket.segmentId);
    collisionSegmentArgs.collisionDetectionSegmentNotification.fIntersectionDistance = collisionDetectionSegmentNotificationPacket.intersectionDistance;
    collisionSegmentArgs.collisionDetectionSegmentNotification.materialCode = MaterialID(collisionDetectionSegmentNotificationPacket.materialCode);
    Event::Raise<HostCigiEvent>(collisionSegmentArgs);
  }
  else
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

void CHostSessionV4::ParseCollisionDetectionVolumeNotificationPacket(uint8_t* buffer)
{
  CIGI::V40::CollisionDetectionVolumeNotification collisionDetectionVolumeNotificationPacket = GetPacketV4<CIGI::V40::CollisionDetectionVolumeNotification>(buffer, m_bByteSwap);

  // Raise a message event to log and display the response
  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_NOTIFICATION) << "\n"
                       << "Entity ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.entityId) << "\n"
                       << "Contacted Entity ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.contactedEntityId) << "\n"
                       << "Volume ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.volumeId) << "\n";
  if (collisionDetectionVolumeNotificationPacket.collisionType == CIGI::V40::CollisionDetectionVolumeNotification::CollisionType::eCollisionType_NonEntity)
  {
    msgArgs.sDataMessage << "Collision Type: "
                         << "Non-Entity"
                         << "\n";
  }
  else
  {
    msgArgs.sDataMessage << "Collision Type: "
                         << "Entity"
                         << "\n";
  }
  msgArgs.sDataMessage << "Contacted Volume ID: " << std::to_string(collisionDetectionVolumeNotificationPacket.contactedVolumeId);
  Event::Raise<HostCigiEvent>(msgArgs);

  // Raise the response event so that host chai scripts can use this data
  if (collisionDetectionVolumeNotificationPacket.collisionType == CIGI::V40::CollisionDetectionVolumeNotification::CollisionType::eCollisionType_NonEntity)
  {
    HostCigiCollisionDetectionVolumeNotificationEventArgs collisionVolumeArgs;
    collisionVolumeArgs.collisionDetectionVolumeNotification.entityID = EntityID(collisionDetectionVolumeNotificationPacket.entityId);
    collisionVolumeArgs.collisionDetectionVolumeNotification.volumeID = VolumeID(collisionDetectionVolumeNotificationPacket.volumeId);
    collisionVolumeArgs.collisionDetectionVolumeNotification.contactedVolumeID = VolumeID(collisionDetectionVolumeNotificationPacket.contactedVolumeId);
    Event::Raise<HostCigiEvent>(collisionVolumeArgs);
  }
  else
  {
    HostCigiCollisionDetectionVolumeEntityNotificationEventArgs collisionVolumeArgs;
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.entityID = EntityID(collisionDetectionVolumeNotificationPacket.entityId);
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.volumeID = VolumeID(collisionDetectionVolumeNotificationPacket.volumeId);
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.contactedEntityID = EntityID(collisionDetectionVolumeNotificationPacket.contactedEntityId);
    collisionVolumeArgs.collisionDetectionVolumeEntityNotification.contactedVolumeID = VolumeID(collisionDetectionVolumeNotificationPacket.contactedVolumeId);
    Event::Raise<HostCigiEvent>(collisionVolumeArgs);
  }
}

void CHostSessionV4::ParseAnimationStopNotificationPacket(uint8_t* buffer)
{
  CIGI::V40::AnimationStopNotification animationStopNotificationPacket = GetPacketV4<CIGI::V40::AnimationStopNotification>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::ANIMATION_STOP_NOTIFICATION) << "\n"
                    << "Entity ID: " << std::to_string(animationStopNotificationPacket.entityId);

  Event::Raise<HostCigiEvent>(args);

  HostCigiAnimationStopNotificationEventArgs animationArgs;
  animationArgs.animationStopNotification.entityID = EntityID(animationStopNotificationPacket.entityId);
  Event::Raise<HostCigiEvent>(animationArgs);
}

void CHostSessionV4::ParseEventNotificationPacket(uint8_t* buffer)
{
  CIGI::V40::EventNotification eventNotificationPacket = GetPacketV4<CIGI::V40::EventNotification>(buffer, m_bByteSwap);

  HostCigiEventNotificationEventArgs eventArgs;
  eventArgs.eventID = eventNotificationPacket.eventId;
  eventArgs.eventData[0] = eventNotificationPacket.eventData1;
  eventArgs.eventData[1] = eventNotificationPacket.eventData2;
  eventArgs.eventData[2] = eventNotificationPacket.eventData3;
  Event::Raise<HostCigiEvent>(eventArgs);
}

void CHostSessionV4::ParseImageGeneratorMessagePacket(uint8_t* buffer)
{
  SCigiPacketHeaderV4 packetHeader = GetPacketHeaderV4(buffer, m_bByteSwap);
  CIGI::V40::IGMessage imageGeneratorMessagePacket = GetPacketV4<CIGI::V40::IGMessage>(buffer, m_bByteSwap);

  HostCigiDataMessageEventArgs args;
  args.sDataMessage << "Received: " << GetCigiOpCodeName(ECigiOpCodeV4::IMAGE_GENERATOR_MESSAGE);
  args.sDataMessage << "\nImage Generator Message ID " << imageGeneratorMessagePacket.messageId;

  Event::Raise<HostCigiEvent>(args);

  HostCigiImageGeneratorMessageEventArgs messageArgs;
  messageArgs.messageID = imageGeneratorMessagePacket.messageId;
  int nMessageLength = static_cast<int>(packetHeader.nPacketSize) - static_cast<int>(CIGI::V40::IGMessage::kBasePacketSize);
  if (nMessageLength > 0)
  {
    messageArgs.sMessage.assign(reinterpret_cast<const char*>(buffer) + CIGI::V40::IGMessage::kBasePacketSize, nMessageLength);
  }
  Event::Raise<HostCigiEvent>(messageArgs);
}

int CHostSessionV4::ProcessPacket(uint8_t* buffer, int nRemainingBytes)
{
  if (nRemainingBytes < static_cast<int>(sizeof(SCigiPacketHeaderV4)))
  {
    return 0;
  }

  SCigiPacketHeaderV4 packetHeader = GetPacketHeaderV4(buffer, m_bByteSwap);

  if (packetHeader.nPacketSize < sizeof(SCigiPacketHeaderV4) || packetHeader.nPacketSize > nRemainingBytes)
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
    int nMinimumPacketSize = GetMinimumIncomingPacketSizeV4(packetHeader.eOpCode);
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

int CHostSessionV4::GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const
{
  if (nRemainingBytes < static_cast<int>(sizeof(SCigiPacketHeaderV4)))
  {
    return 0;
  }

  SCigiPacketHeaderV4 packetHeader = GetPacketHeaderV4(buffer, m_bByteSwap);

  if (packetHeader.nPacketSize < sizeof(SCigiPacketHeaderV4) || packetHeader.nPacketSize > nRemainingBytes)
  {
    return 0;
  }

  return packetHeader.nPacketSize;
}

int CHostSessionV4::GetMinimumIncomingPacketSizeV4(ECigiOpCodeV4 eOpCode) const
{
  switch (eOpCode)
  {
  case ECigiOpCodeV4::START_OF_FRAME:
    return sizeof(CIGI::V40::SoF);
  case ECigiOpCodeV4::HAT_HOT_RESPONSE:
    return sizeof(CIGI::V40::HATHOTResponse);
  case ECigiOpCodeV4::HAT_HOT_EXTENDED_RESPONSE:
    return sizeof(CIGI::V40::HATHOTExtendedResponse);
  case ECigiOpCodeV4::LINE_OF_SIGHT_RESPONSE:
    return sizeof(CIGI::V40::LineOfSightResponse);
  case ECigiOpCodeV4::LINE_OF_SIGHT_EXTENDED_RESPONSE:
    return sizeof(CIGI::V40::LineOfSightExtendedResponse);
  case ECigiOpCodeV4::SENSOR_RESPONSE:
    return sizeof(CIGI::V40::SensorResponse);
  case ECigiOpCodeV4::SENSOR_EXTENDED_RESPONSE:
    return sizeof(CIGI::V40::SensorExtendedResponse);
  case ECigiOpCodeV4::POSITION_RESPONSE:
    return sizeof(CIGI::V40::PositionResponse);
  case ECigiOpCodeV4::WEATHER_CONDITIONS_RESPONSE:
    return sizeof(CIGI::V40::WeatherConditionsResponse);
  case ECigiOpCodeV4::AEROSOL_CONCENTRATION_RESPONSE:
    return sizeof(CIGI::V40::AerosolConcentrationResponse);
  case ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_RESPONSE:
    return sizeof(CIGI::V40::MaritimeSurfaceConditionsResponse);
  case ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE:
    return sizeof(CIGI::V40::TerrestrialSurfaceConditionsResponse);
  case ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_NOTIFICATION:
    return sizeof(CIGI::V40::CollisionDetectionSegmentNotification);
  case ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_NOTIFICATION:
    return sizeof(CIGI::V40::CollisionDetectionVolumeNotification);
  case ECigiOpCodeV4::ANIMATION_STOP_NOTIFICATION:
    return sizeof(CIGI::V40::AnimationStopNotification);
  case ECigiOpCodeV4::EVENT_NOTIFICATION:
    return sizeof(CIGI::V40::EventNotification);
  case ECigiOpCodeV4::IMAGE_GENERATOR_MESSAGE:
    return CIGI::V40::IGMessage::kBasePacketSize;
  default:
    return 0;
  }
}

SCigiPacketHeaderV4 CHostSessionV4::GetPacketHeaderV4(const uint8_t* buffer, bool bByteSwap) const
{
  SCigiPacketHeaderV4 packetHeader;
  memcpy(&packetHeader, buffer, sizeof(SCigiPacketHeaderV4));

  if (bByteSwap)
  {
    CEndian::Swap(packetHeader.nPacketSize);
    uint16_t nOpCode = static_cast<uint16_t>(packetHeader.eOpCode);
    CEndian::Swap(nOpCode);
    packetHeader.eOpCode = static_cast<ECigiOpCodeV4>(nOpCode);
  }

  return packetHeader;
}

void CHostSessionV4::SendIGControl(uint8_t*& pBuffer)
{
  try
  {
    m_IGControl.hostFrameNumber = m_HostFrameNumber.Value();
    m_IGControl.lastIGFrameNumber = m_LastReceivedIGFrame.Value();
    m_IGControl.timestamp = 0;
    m_IGControl.timestampValid = true;

    if (m_eDatabaseState == EHostSessionDatabaseState::NO_DATABASE)
    {
      m_IGControl.dbNumber = 0;
    }
    else if (m_eDatabaseState == EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED)
    {
      m_IGControl.dbNumber = m_DatabaseNumber.Value();
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

    CIGI::V40::IGCtrl tempIGControl = m_IGControl;
    if (m_bByteSwap)
    {
      ByteSwapPacketV4(tempIGControl);
    }

    memcpy(pBuffer, &tempIGControl, sizeof(CIGI::V40::IGCtrl));
    pBuffer += sizeof(CIGI::V40::IGCtrl);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEntityControl(const SEntityControl& entityControl)
{
  try
  {
    CIGI::V40::EntityCtrl entityCtrl;
    entityCtrl.entityId = entityControl.entityID.Value();
    entityCtrl.entityState = ConvertToEntityState(entityControl.eState);
    entityCtrl.collisionReportingEnable = entityControl.bCollisionReportingEnabled;
    entityCtrl.inheritAlpha = entityControl.bInheritAlpha;
    entityCtrl.smoothingEnable = entityControl.bSmoothingEnabled;
    entityCtrl.extendedEntityType = ConvertToExtendedEntityType(entityControl.eExtendedEntityType);
    entityCtrl.alpha = entityControl.alpha;

    if (entityControl.eExtendedEntityType == EExtendedEntityType::SHORT)
    {
      entityCtrl.entityCountryOrShortEntityType = entityControl.shortEntityTypeID.Value();
    }
    else if (entityControl.eExtendedEntityType == EExtendedEntityType::EXTENDED)
    {
      entityCtrl.entityKind = entityControl.entityType.entityKindID.Value();
      entityCtrl.entityDomain = entityControl.entityType.entityDomainID.Value();
      entityCtrl.entityCountryOrShortEntityType = entityControl.entityType.entityCountryID.Value();
      entityCtrl.entityCategory = entityControl.entityType.entityCategoryID.Value();
      entityCtrl.entitySubcategory = entityControl.entityType.entitySubCategoryID.Value();
      entityCtrl.entitySpecific = entityControl.entityType.entitySpecificID.Value();
      entityCtrl.entityExtra = entityControl.entityType.entityExtraID.Value();
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ENTITY_CONTROL) << "\n"
                        << "Entity State: " << ConvertCigiActiveStateToString(entityControl.eState) << "\n"
                        << "Collision Reporting Enable: " << EnabledToString(entityControl.bCollisionReportingEnabled) << "\n"
                        << "Inherit Alpha: " << ConvertInheritAlphaToString(entityControl.bInheritAlpha) << "\n"
                        << "Smoothing Enable: " << EnableToString(entityControl.bSmoothingEnabled) << "\n"
                        << "Extended Entity Type: " << ConvertCigiExtendedEntityTypeToString(entityControl.eExtendedEntityType) << "\n"
                        << "Alpha: " << std::to_string(entityControl.alpha) << "\n"
                        << "Entity ID: " << std::to_string(entityControl.entityID.Value()) << "\n";

      if (entityControl.eExtendedEntityType == EExtendedEntityType::SHORT)
      {
        args.sDataMessage << "Short Entity Type: " << std::to_string(entityControl.shortEntityTypeID.Value());
      }
      else if (entityControl.eExtendedEntityType == EExtendedEntityType::EXTENDED)
      {
        args.sDataMessage << "Entity Kind: " << std::to_string(entityControl.entityType.entityKindID.Value()) << "\n"
                          << "Entity Domain: " << std::to_string(entityControl.entityType.entityDomainID.Value()) << "\n"
                          << "Entity Country: " << std::to_string(entityControl.entityType.entityCountryID.Value()) << "\n"
                          << "Entity Category: " << std::to_string(entityControl.entityType.entityCategoryID.Value()) << "\n"
                          << "Entity Subcategory: " << std::to_string(entityControl.entityType.entitySubCategoryID.Value()) << "\n"
                          << "Entity Specific: " << std::to_string(entityControl.entityType.entitySpecificID.Value()) << "\n"
                          << "Entity Extra: " << std::to_string(entityControl.entityType.entityExtraID.Value());
      }
      else
      {
        args.sDataMessage << "No Entity Types Were Found";
      }
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, entityCtrl, m_bByteSwap);
  }
  catch (const std::exception& ex)
  {
    std::cout << "Error sending Entity Control packet" << std::endl;
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendArticulatedPartControl(const sbio::cigi::SCigiArticulatedPart& articulatedPartControl)
{
  try
  {
    CIGI::V40::ArticulatedPartCtrl artPartCtrl;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ARTICULATED_PART_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(articulatedPartControl.entityID.Value()) << "Articulated Part ID: " << std::to_string(articulatedPartControl.articulatedPartID.Value()) << "\n"
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
                        << "Roll: " << std::to_string(articulatedPartControl.rotation.roll.Value()) << "\n"
                        << "Pitch: " << std::to_string(articulatedPartControl.rotation.pitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(articulatedPartControl.rotation.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, artPartCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V40::AccelerationCtrl::CoordinateSystem ConvertToCoordinateSystem(sbio::EObjectCoordinateSystem coordSys)
{
  if (coordSys == sbio::EObjectCoordinateSystem::LOCAL)
  {
    return CIGI::V40::AccelerationCtrl::CoordinateSystem::eCoordinateSystem_Local;
  }
  else if (coordSys == sbio::EObjectCoordinateSystem::PARENT)
  {
    return CIGI::V40::AccelerationCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
  }
  else if (coordSys == sbio::EObjectCoordinateSystem::WORLD)
  {
    return CIGI::V40::AccelerationCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
  }
  return CIGI::V40::AccelerationCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
}

void CHostSessionV4::SendEntityAccelerationControl(const sbio::cigi::SCigiEntityAcceleration& accelerationControl)
{
  try
  {
    CIGI::V40::AccelerationCtrl accelerationCtrl;

    accelerationCtrl.entityId = accelerationControl.entityID.Value();
    accelerationCtrl.articulatedPartId = 0;
    accelerationCtrl.applyToArticulatedPart = false;
    accelerationCtrl.coordinateSystem = ConvertToCoordinateSystem(accelerationControl.eCoordinateSystem);
    accelerationCtrl.xLinearAcceleration = static_cast<float>(accelerationControl.linearAcceleration[0]);
    accelerationCtrl.yLinearAcceleration = static_cast<float>(accelerationControl.linearAcceleration[1]);
    accelerationCtrl.zLinearAcceleration = static_cast<float>(accelerationControl.linearAcceleration[2]);
    accelerationCtrl.rollAngularAcceleration = accelerationControl.angularAcceleration.roll.Value();
    accelerationCtrl.pitchAngularAcceleration = accelerationControl.angularAcceleration.pitch.Value();
    accelerationCtrl.yawAngularAcceleration = accelerationControl.angularAcceleration.yaw.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ACCELERATION_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(accelerationControl.entityID.Value()) << "\n";

      args.sDataMessage << "Apply to Articulated Part: "
                        << "False"
                        << "\n";
      if (accelerationControl.eCoordinateSystem == EObjectCoordinateSystem::WORLD || accelerationControl.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
      {
        args.sDataMessage << "Coordinate System: "
                          << "World/Parent"
                          << "\n";
      }
      else if (accelerationControl.eCoordinateSystem == EObjectCoordinateSystem::LOCAL)
      {
        args.sDataMessage << "Coordinate System: "
                          << "Local"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Coordinate System: "
                          << "Unknown"
                          << "\n";
      }

      args.sDataMessage << "X Linear Acceleration: " << std::to_string(accelerationControl.linearAcceleration[0]) << "\n"
                        << "Y Linear Acceleration: " << std::to_string(accelerationControl.linearAcceleration[1]) << "\n"
                        << "Z Linear Acceleration: " << std::to_string(accelerationControl.linearAcceleration[2]) << "\n"
                        << "Roll Angular Acceleration: " << std::to_string(accelerationControl.angularAcceleration.roll.Value()) << "\n"
                        << "Pitch Angular Acceleration: " << std::to_string(accelerationControl.angularAcceleration.pitch.Value()) << "\n"
                        << "Yaw Angular Acceleration: " << std::to_string(accelerationControl.angularAcceleration.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, accelerationCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendArticulatedPartAccelerationControl(const sbio::cigi::SCigiArticulatedPartAcceleration& accelerationControl)
{
  try
  {
    CIGI::V40::AccelerationCtrl accelerationCtrl;

    accelerationCtrl.entityId = accelerationControl.entityID.Value();
    accelerationCtrl.articulatedPartId = accelerationControl.articulatedPartID.Value();
    accelerationCtrl.applyToArticulatedPart = true;
    accelerationCtrl.coordinateSystem = ConvertToCoordinateSystem(accelerationControl.eCoordinateSystem);
    accelerationCtrl.xLinearAcceleration = static_cast<float>(accelerationControl.linearAcceleration[0]);
    accelerationCtrl.yLinearAcceleration = static_cast<float>(accelerationControl.linearAcceleration[1]);
    accelerationCtrl.zLinearAcceleration = static_cast<float>(accelerationControl.linearAcceleration[2]);
    accelerationCtrl.rollAngularAcceleration = accelerationControl.angularAcceleration.roll.Value();
    accelerationCtrl.pitchAngularAcceleration = accelerationControl.angularAcceleration.pitch.Value();
    accelerationCtrl.yawAngularAcceleration = accelerationControl.angularAcceleration.yaw.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ACCELERATION_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(accelerationControl.entityID.Value()) << "\n"
                        << "Articulated Part ID: " << std::to_string(accelerationControl.articulatedPartID.Value()) << "\n";

      args.sDataMessage << "Apply to Articulated Part: "
                        << "True"
                        << "\n";

      if (accelerationControl.eCoordinateSystem == EObjectCoordinateSystem::WORLD || accelerationControl.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
      {
        args.sDataMessage << "Coordinate System: "
                          << "World/Parent"
                          << "\n";
      }
      else if (accelerationControl.eCoordinateSystem == EObjectCoordinateSystem::LOCAL)
      {
        args.sDataMessage << "Coordinate System: "
                          << "Local"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Coordinate System: "
                          << "Unknown"
                          << "\n";
      }

      args.sDataMessage << "X Linear Acceleration: " << std::to_string(accelerationControl.linearAcceleration[0]) << "\n"
                        << "Y Linear Acceleration: " << std::to_string(accelerationControl.linearAcceleration[1]) << "\n"
                        << "Z Linear Acceleration: " << std::to_string(accelerationControl.linearAcceleration[2]) << "\n"
                        << "Roll Angular Acceleration: " << std::to_string(accelerationControl.angularAcceleration.roll.Value()) << "\n"
                        << "Pitch Angular Acceleration: " << std::to_string(accelerationControl.angularAcceleration.pitch.Value()) << "\n"
                        << "Yaw Angular Acceleration: " << std::to_string(accelerationControl.angularAcceleration.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, accelerationCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V40::AnimationCtrl::AnimationState ConvertToAnimationState(EAnimationState animState)
{
  if (animState == EAnimationState::PLAY)
  {
    return CIGI::V40::AnimationCtrl::AnimationState::eAnimationState_Play;
  }
  else if (animState == EAnimationState::STOP)
  {
    return CIGI::V40::AnimationCtrl::AnimationState::eAnimationState_Stop;
  }
  return CIGI::V40::AnimationCtrl::AnimationState::eAnimationState_Stop;
}

CIGI::V40::AnimationCtrl::AnimationFramePositionReset ConvertToAnimationResetGroup(EAnimationFramePositionReset reset)
{
  if (reset == EAnimationFramePositionReset::CONTINUE)
  {
    return CIGI::V40::AnimationCtrl::AnimationFramePositionReset::eAnimationFramePositionReset_Continue;
  }
  else if (reset == EAnimationFramePositionReset::RESET)
  {
    return CIGI::V40::AnimationCtrl::AnimationFramePositionReset::eAnimationFramePositionReset_Reset;
  }
  return CIGI::V40::AnimationCtrl::AnimationFramePositionReset::eAnimationFramePositionReset_Reset;
}

CIGI::V40::AnimationCtrl::AnimationLoopMode ConvertToAnimationLoopMode(EAnimationLoopMode loopMode)
{
  if (loopMode == EAnimationLoopMode::CONTINUOUS)
  {
    return CIGI::V40::AnimationCtrl::AnimationLoopMode::eAnimationLoopMode_Continuous;
  }
  else if (loopMode == EAnimationLoopMode::ONE_SHOT)
  {
    return CIGI::V40::AnimationCtrl::AnimationLoopMode::eAnimationLoopMode_OneShot;
  }
  return CIGI::V40::AnimationCtrl::AnimationLoopMode::eAnimationLoopMode_OneShot;
}

CIGI::V40::AnimationCtrl::InheritAlpha ConvertToInheritAlpha(bool inheritAlpha)
{
  if (inheritAlpha)
  {
    return CIGI::V40::AnimationCtrl::InheritAlpha::eInheritAlpha_Inherited;
  }
  return CIGI::V40::AnimationCtrl::InheritAlpha::eInheritAlpha_NotInherited;
}

void CHostSessionV4::SendAnimationControl(const sbio::cigi::SCigiAnimationControl& animationControl)
{
  try
  {
    CIGI::V40::AnimationCtrl animationCtrl;
    animationCtrl.animationState = ConvertToAnimationState(animationControl.eAnimationState);
    animationCtrl.animationFramePositionReset = ConvertToAnimationResetGroup(animationControl.eAnimationFramePositionReset);
    animationCtrl.animationLoopMode = ConvertToAnimationLoopMode(animationControl.eAnimationLoopMode);
    animationCtrl.inheritAlpha = ConvertToInheritAlpha(animationControl.bInheritAlpha);
    animationCtrl.alpha = ConvertToCigiAlpha(animationControl.alpha);
    animationCtrl.entityId = animationControl.entityID.Value();
    animationCtrl.animationId = animationControl.animationID.Value();
    animationCtrl.animationSpeed = animationControl.fAnimationSpeed;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ANIMATION_CONTROL) << "\n"
                        << "Animation State: " << ConvertCigiAnimationStateToString(animationControl.eAnimationState) << "\n"
                        << "Animation Frame Position Reset: " << ConvertCigiAnimationFramePositionResetToString(animationControl.eAnimationFramePositionReset) << "\n"
                        << "Animation Loop Mode: " << ConvertCigiAnimationLoopModeToString(animationControl.eAnimationLoopMode) << "\n"
                        << "Inherit Alpha: " << ConvertInheritAlphaToString(animationControl.bInheritAlpha) << "\n"
                        << "Alpha: " << std::to_string(ConvertToCigiAlpha(animationControl.alpha)) << "\n"
                        << "Entity ID: " << std::to_string(animationControl.entityID.Value()) << "\n"
                        << "Animation ID: " << std::to_string(animationControl.animationID.Value()) << "\n"
                        << "Animation Speed: " << std::to_string(animationControl.fAnimationSpeed);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, animationCtrl, m_bByteSwap);
  }

  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendAtmosphereControl(const sbio::cigi::SCigiAtmosphereControl& atmosphereControl)
{
  try
  {
    CIGI::V40::AtmosphereCtrl atmosphereCtrl;
    atmosphereCtrl.atmosphericModelEnable = atmosphereControl.bAtmosphereModelEnable;
    atmosphereCtrl.globalHumidity = ConvertToCigiIntPercentage(atmosphereControl.globalHumidity).Value();
    atmosphereCtrl.globalAirTemperature = atmosphereControl.fGlobalAirTemp;
    atmosphereCtrl.globalVisibilityRange = atmosphereControl.fGlobalVisibility;
    atmosphereCtrl.globalHorizontalWindSpeed = atmosphereControl.fGlobalHorizontalWindSpeed;
    atmosphereCtrl.globalVerticalWindSpeed = atmosphereControl.fGlobalVerticalWindSpeed;
    atmosphereCtrl.globalWindDirection = atmosphereControl.globalWindDirection.Value();
    atmosphereCtrl.globalBarometricPressure = atmosphereControl.fGlobalBarometricPressure;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ATMOSPHERE_CONTROL) << "\n"
                        << "Atmospheric Model Enable: " << EnableToString(atmosphereControl.bAtmosphereModelEnable) << "\n"
                        << "Global Humidity: " << std::to_string(ConvertToCigiIntPercentage(atmosphereControl.globalHumidity).Value()) << "%"
                        << "\n"
                        << "Global Air Temperature: " << std::to_string(atmosphereControl.fGlobalAirTemp) << "�C"
                        << "\n"
                        << "Global Visibility Range: " << std::to_string(atmosphereControl.fGlobalVisibility) << "\n"
                        << "Global Horizontal Wind Speed: " << std::to_string(atmosphereControl.fGlobalHorizontalWindSpeed) << "\n"
                        << "Global Vertical Wind Speed: " << std::to_string(atmosphereControl.fGlobalVerticalWindSpeed) << "\n"
                        << "Global Wind Direction: " << std::to_string(atmosphereControl.globalWindDirection.Value()) << "\n"
                        << "Global Barometric Pressure: " << std::to_string(atmosphereControl.fGlobalHorizontalWindSpeed);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, atmosphereCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendCelestialSphereControl(const sbio::cigi::SCigiCelestialSphereControl& celestialSphereControl)
{
  try
  {
    CIGI::V40::CelestialSphereCtrl celestialCtrl;
    celestialCtrl.hour = celestialSphereControl.hour.Value();
    celestialCtrl.minute = celestialSphereControl.minute.Value();
    celestialCtrl.seconds = celestialSphereControl.second.Value();
    celestialCtrl.continuousTimeOfDayEnable = celestialSphereControl.bContinuousTimeOfDayEnable;
    celestialCtrl.sunEnable = celestialSphereControl.bSunEnable;
    celestialCtrl.moonEnable = celestialSphereControl.bMoonEnable;
    celestialCtrl.starFieldEnable = celestialSphereControl.bStarFieldEnable;
    celestialCtrl.starFieldIntensity = ConvertToCigiFloatPercentage(celestialSphereControl.starFieldIntensity).Value();
    celestialCtrl.dateTimeValid = celestialSphereControl.bDateTimeValid;
    celestialCtrl.setDate(celestialSphereControl.year.Value(), ConvertToCigiMonth(celestialSphereControl.month).Value(), celestialSphereControl.day.Value());

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::CELESTIAL_SPHERE_CONTROL) << "\n"
                        << "Continuous Time-of-Day Enable: " << EnableToString(celestialSphereControl.bContinuousTimeOfDayEnable) << "\n"
                        << "Sun Enable: " << EnableToString(celestialSphereControl.bSunEnable) << "\n"
                        << "Moon Enable: " << EnableToString(celestialSphereControl.bMoonEnable) << "\n"
                        << "Star Field Enable: " << EnableToString(celestialSphereControl.bStarFieldEnable) << "\n"
                        << "Date/Time Valid: " << ValidToString(celestialSphereControl.bDateTimeValid) << "\n"
                        << "Hour: " << std::to_string(celestialSphereControl.hour.Value()) << "\n"
                        << "Minute: " << std::to_string(celestialSphereControl.minute.Value()) << "\n"
                        << "Seconds: " << std::to_string(celestialSphereControl.second.Value()) << "\n"
                        << "Year: " << std::to_string(celestialSphereControl.year.Value()) << "\n"
                        << "Month: " << std::to_string(ConvertToCigiMonth(celestialSphereControl.month).Value()) << "\n"
                        << "Day: " << std::to_string(celestialSphereControl.day.Value()) << "\n"
                        << "Star Field Intensity: " << std::to_string(ConvertToCigiFloatPercentage(celestialSphereControl.starFieldIntensity).Value()) << "%"
                        << "\n";
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, celestialCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendTopLevelEntityPosition(const STopLevelEntityPosition& topLevelEntityPosition)
{
  try
  {
    CIGI::V40::EntityPosition entityPos;
    entityPos.attachState = (topLevelEntityPosition.bAttached) ? CIGI::V40::EntityPosition::AttachState::eAttachState_Attach : CIGI::V40::EntityPosition::AttachState::eAttachState_Detach;
    entityPos.groundOceanClamp = ConvertToClamp(topLevelEntityPosition.eClamp);
    entityPos.entityId = topLevelEntityPosition.entityID.Value();
    entityPos.roll = topLevelEntityPosition.rotation.roll.Value();
    entityPos.pitch = topLevelEntityPosition.rotation.pitch.Value();
    entityPos.yaw = topLevelEntityPosition.rotation.yaw.Value();
    entityPos.latitudeXOffset = topLevelEntityPosition.geodeticCoordinates.latitude.Value();
    entityPos.longitudeYOffset = topLevelEntityPosition.geodeticCoordinates.longitude.Value();
    entityPos.altitudeZOffset = topLevelEntityPosition.geodeticCoordinates.altitude.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ENTITY_POSITION) << "\n"
                        << "Attach State: " << ConvertAttachStateToString(topLevelEntityPosition.bAttached) << "\n"
                        << "Ground/Ocean Clamp: " << ConvertCigiEClampToString(topLevelEntityPosition.eClamp) << "\n"
                        << "Entity ID: " << std::to_string(topLevelEntityPosition.entityID.Value()) << "\n"
                        << "Roll: " << std::to_string(topLevelEntityPosition.rotation.roll.Value()) << "\n"
                        << "Pitch: " << std::to_string(topLevelEntityPosition.rotation.pitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(topLevelEntityPosition.rotation.yaw.Value()) << "\n"
                        << "Latitude: " << std::to_string(topLevelEntityPosition.geodeticCoordinates.latitude.Value()) << "\n"
                        << "Longitude: " << std::to_string(topLevelEntityPosition.geodeticCoordinates.longitude.Value()) << "\n"
                        << "Altitude: " << std::to_string(topLevelEntityPosition.geodeticCoordinates.altitude.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, entityPos, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendChildEntityPosition(const SChildEntityPosition& childEntityPosition)
{
  try
  {
    CIGI::V40::EntityPosition entityPos;
    entityPos.attachState = (childEntityPosition.bAttached) ? CIGI::V40::EntityPosition::AttachState::eAttachState_Attach : CIGI::V40::EntityPosition::AttachState::eAttachState_Detach;
    entityPos.entityId = childEntityPosition.entityID.Value();
    entityPos.parentId = childEntityPosition.parentID.Value();
    entityPos.roll = childEntityPosition.rotation.roll.Value();
    entityPos.pitch = childEntityPosition.rotation.pitch.Value();
    entityPos.yaw = childEntityPosition.rotation.yaw.Value();
    entityPos.latitudeXOffset = childEntityPosition.offset[0];
    entityPos.longitudeYOffset = childEntityPosition.offset[1];
    entityPos.altitudeZOffset = childEntityPosition.offset[2];

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ENTITY_POSITION) << "\n"
                        << "Attach State: " << ConvertAttachStateToString(childEntityPosition.bAttached) << "\n"
                        << "Entity ID: " << std::to_string(childEntityPosition.entityID.Value()) << "\n"
                        << "Parent ID: " << std::to_string(childEntityPosition.parentID.Value()) << "\n"
                        << "Roll: " << std::to_string(childEntityPosition.rotation.roll.Value()) << "\n"
                        << "Pitch: " << std::to_string(childEntityPosition.rotation.pitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(childEntityPosition.rotation.yaw.Value()) << "\n"
                        << "X Offset: " << std::to_string(childEntityPosition.offset[0]) << "\n"
                        << "Y Offset: " << std::to_string(childEntityPosition.offset[1]) << "\n"
                        << "Z Offset: " << std::to_string(childEntityPosition.offset[2]);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, entityPos, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendCollisionDetectionSegment(const sbio::cigi::SCollisionDetectionSegmentDefinition& collDetSegment)
{
  try
  {
    CIGI::V40::CollisionDetectionSegmentDefinition collDetSeg;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_DEFINITION) << "\n"
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
    }

    PackPacketV4(this, collDetSeg, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendCollisionDetectionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collVolCuboid)
{
  try
  {
    CIGI::V40::CollisionDetectionVolumeDefinition collDetVol;
    collDetVol.volumeId = collVolCuboid.volumeID.Value();
    collDetVol.volumeEnable = collVolCuboid.bVolumeEnabled;
    collDetVol.volumeType = CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType::eVolumeType_Cuboid;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_DEFINITION) << "\n"
                        << "Entity ID: " << std::to_string(collVolCuboid.entityID.Value()) << "\n"
                        << "Volume ID: " << std::to_string(collVolCuboid.volumeID.Value()) << "\n"
                        << "Volume Enable: " << EnableToString(collVolCuboid.bVolumeEnabled) << "\n"
                        << "Volume Type: "
                        << "Cuboid"
                        << "\n"
                        << "X: " << std::to_string(collVolCuboid.offset[0]) << "\n"
                        << "Y: " << std::to_string(collVolCuboid.offset[1]) << "\n"
                        << "Z: " << std::to_string(collVolCuboid.offset[2]) << "\n";

      args.sDataMessage << "Height: " << std::to_string(collVolCuboid.fHeight) << "\n";

      args.sDataMessage << "Width: " << std::to_string(collVolCuboid.fWidth) << "\n"
                        << "Depth: " << std::to_string(collVolCuboid.fDepth) << "\n"
                        << "Roll: " << std::to_string(collVolCuboid.rotation.roll.Value()) << "\n"
                        << "Pitch: " << std::to_string(collVolCuboid.rotation.pitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(collVolCuboid.rotation.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, collDetVol, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendCollisionDetectionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collVolSphere)
{
  try
  {
    CIGI::V40::CollisionDetectionVolumeDefinition collDetVol;
    collDetVol.volumeId = collVolSphere.volumeID.Value();
    collDetVol.volumeEnable = collVolSphere.bVolumeEnabled;
    collDetVol.volumeType = CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType::eVolumeType_Sphere;
    collDetVol.entityId = collVolSphere.entityID.Value();
    collDetVol.x = static_cast<float>(collVolSphere.offset[0]);
    collDetVol.y = static_cast<float>(collVolSphere.offset[1]);
    collDetVol.z = static_cast<float>(collVolSphere.offset[2]);

    collDetVol.heightRadius = collVolSphere.fRadius;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_DEFINITION) << "\n"
                        << "Entity ID: " << std::to_string(collVolSphere.entityID.Value()) << "\n"
                        << "Volume ID: " << std::to_string(collVolSphere.volumeID.Value()) << "\n"
                        << "Volume Enable: " << EnableToString(collVolSphere.bVolumeEnabled) << "\n"
                        << "Volume Type: "
                        << "Sphere"
                        << "\n"
                        << "X: " << std::to_string(collVolSphere.offset[0]) << "\n"
                        << "Y: " << std::to_string(collVolSphere.offset[1]) << "\n"
                        << "Z: " << std::to_string(collVolSphere.offset[2]) << "\n";

      args.sDataMessage << "Height: " << std::to_string(collVolSphere.fRadius) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, collDetVol, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V40::ComponentCtrl::ComponentClass ConvertToCompClass(uint32_t classValue)
{
  if (classValue == 0)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_Entity;
  }
  else if (classValue == 1)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_View;
  }
  else if (classValue == 2)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_ViewGroup;
  }
  else if (classValue == 3)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_Sensor;
  }
  else if (classValue == 4)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_RegionalSeaSurface;
  }
  else if (classValue == 5)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_RegionalTerrainSurface;
  }
  else if (classValue == 6)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_RegionalLayeredWeather;
  }
  else if (classValue == 7)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_GlobalSeaSurface;
  }
  else if (classValue == 8)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_GlobalTerrainSurface;
  }
  else if (classValue == 9)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_GlobalLayeredWeather;
  }
  else if (classValue == 10)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_Atmosphere;
  }
  else if (classValue == 11)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_CelestialSphere;
  }
  else if (classValue == 12)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_Event;
  }
  else if (classValue == 13)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_System;
  }
  else if (classValue == 14)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_SymbolSurface;
  }
  else if (classValue == 15)
  {
    return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_Symbol;
  }
  return CIGI::V40::ComponentCtrl::ComponentClass::eComponentClass_Entity;
}

void CHostSessionV4::SendComponentControl(const sbio::cigi::SCigiComponentControl& componentControl)
{
  try
  {
    CIGI::V40::ComponentCtrl componentCtrl;
    componentCtrl.componentId = componentControl.key.componentID.Value();
    componentCtrl.instanceId = componentControl.key.nInstanceID;
    componentCtrl.componentClass = ConvertToCompClass(componentControl.key.componentClassID.Value());
    componentCtrl.componentState = componentControl.state.nComponentState;

    for (int n = 0; n < 6; ++n)
    {
      componentCtrl.componentData[n] = componentControl.state.componentData[n];
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::COMPONENT_CONTROL) << "\n"
                        << "Component ID: " << std::to_string(componentControl.key.componentID.Value()) << "\n"
                        << "Component Class: " << g_pScriptRuntime->GetComponentClassString(componentControl.key.componentClassID) << "\n"
                        << "Component State: " << std::to_string(componentControl.state.nComponentState) << "\n"
                        << "Instance ID: " << std::to_string(componentControl.key.nInstanceID) << "\n";
      for (int n = 0; n < 6; ++n)
      {
        args.sDataMessage << "Component Data " << std::to_string(n + 1) << ": " << std::to_string(componentControl.state.componentData[n]);
        if (n < 5)
        {
          args.sDataMessage << "\n";
        }
      }
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, componentCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendConformalClampedEntityPosition(const sbio::cigi::SCigiConformalClampedEntityPosition& conformalClampedEntityPos)
{
  try
  {
    CIGI::V40::ConformalClampedEntityPosition confClampEntity;
    confClampEntity.entityId = conformalClampedEntityPos.entityID.Value();
    confClampEntity.yaw = conformalClampedEntityPos.fYaw.Value();
    confClampEntity.latitude = conformalClampedEntityPos.latitude.Value();
    confClampEntity.longitude = conformalClampedEntityPos.longitude.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::CONFORMAL_CLAMPED_ENTITY_POSITION) << "\n"
                        << "Entity ID: " << std::to_string(conformalClampedEntityPos.entityID.Value()) << "\n"
                        << "Yaw: " << std::to_string(conformalClampedEntityPos.fYaw.Value()) << "\n"
                        << "Latitude: " << std::to_string(conformalClampedEntityPos.latitude.Value()) << "\n"
                        << "Longitude: " << std::to_string(conformalClampedEntityPos.longitude.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, confClampEntity, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEarthReferenceModelDefinition(const SCigiEarthReferenceModel& earthReferenceModel)
{
  try
  {
    CIGI::V40::EarthReferenceModelDefinition earthModelDef;
    earthModelDef.customERMEnable = earthReferenceModel.eEarthReferenceModel != EEarthReferenceModel::WGS84;
    earthModelDef.equatorialRadius = earthReferenceModel.fEquatorialRadius;
    earthModelDef.flattening = earthReferenceModel.fFlattening;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::EARTH_REFERENCE_MODEL_DEFINITION) << "\n"
                        << "Custom ERM Enable: " << EnableToString(earthModelDef.customERMEnable) << "\n"
                        << "Equatorial Radius: " << std::to_string(earthReferenceModel.fEquatorialRadius) << "\n"
                        << "Flattening: " << std::to_string(earthReferenceModel.fFlattening);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, earthModelDef, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEnvironmentalConditionsRequest(const SEnvironmentalConditionsRequest& environmentalConditionsRequest)
{
  try
  {
    CIGI::V40::EnvironmentalConditionsRequest envCondReq;

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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ENVIRONMENTAL_CONDITIONS_REQUEST) << "\n"
                        << "Request ID: " << std::to_string(environmentalConditionsRequest.nRequestID) << "\n"
                        << "Latitude: " << std::to_string(environmentalConditionsRequest.geodeticCoordinates.latitude.Value()) << "\n"
                        << "Longitude: " << std::to_string(environmentalConditionsRequest.geodeticCoordinates.longitude.Value()) << "\n"
                        << "Altitude: " << std::to_string(environmentalConditionsRequest.geodeticCoordinates.altitude.Value()) << "\n"
                        << "Maritime Surface Conditions Request: " << EnableToString(environmentalConditionsRequest.bMaritimeSurfaceConditionsRequest) << "\n"
                        << "Terrestrial Surface Conditions Request: " << EnableToString(environmentalConditionsRequest.bTerrestrialSurfaceConditionsRequest) << "\n"
                        << "Weather Conditions Request: " << EnableToString(environmentalConditionsRequest.bWeatherConditionsRequest) << "\n"
                        << "Aerosol Concentrations Request: " << EnableToString(environmentalConditionsRequest.bAerosolConcentrationsRequest) << "\n";
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, envCondReq, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEnvironmentalRegionControl(const SCigiEnvironmentalRegion& environmentalRegion)
{
  try
  {
    CIGI::V40::EnvironmentalRegionCtrl envRegion;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::ENVIRONMENTAL_REGION_CONTROL) << "\n"
                        << "Region ID: " << std::to_string(environmentalRegion.regionID.Value()) << "\n"
                        << "Region State: " << ConvertCigiActiveStateToString(environmentalRegion.eRegionState) << "\n"
                        << "Merge Weather Properties: " << ConvertCigiMergeStateToString(environmentalRegion.eMergeWeatherProperties) << "\n"
                        << "Merge Aerosol Properties: " << ConvertCigiMergeStateToString(environmentalRegion.eMergeAerosolConcentrations) << "\n"
                        << "Merge Maritime Properties: " << ConvertCigiMergeStateToString(environmentalRegion.eMergeMaritimeSurfaceConditions) << "\n"
                        << "Merge Terrestrial Properties: " << ConvertCigiMergeStateToString(environmentalRegion.eMergeTerrestrialSurfaceConditions) << "\n"
                        << "Latitude: " << std::to_string(environmentalRegion.latitude.Value()) << "\n"
                        << "Longitude: " << std::to_string(environmentalRegion.longitude.Value()) << "\n"
                        << "Size X: " << std::to_string(environmentalRegion.size[0]) << "\n"
                        << "Size Y: " << std::to_string(environmentalRegion.size[1]) << "\n"
                        << "Corner Radius: " << std::to_string(environmentalRegion.fCornerRadius) << "\n"
                        << "Rotation: " << std::to_string(environmentalRegion.fRotation.Value()) << "\n"
                        << "Transition Perimeter: " << std::to_string(environmentalRegion.fTransition);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, envRegion, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendHatHotRequest(const SHATHOTGlobalRequest& hatHotRequest)
{
  try
  {
    CIGI::V40::HATHOTRequest request;
    request.HATHOTId = hatHotRequest.requestID.Value();
    request.entityId = 0;// Entity ID is not applicable for global requests, set to 0 or a reserved value
    request.requestType = ConvertToRequestType(hatHotRequest.eRequestType);
    request.coordinateSystem = CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Geodetic;// Global requests use geodetic coordinates
    request.updatePeriod = hatHotRequest.updatePeriod.Value();

    request.latitudeXOffset = hatHotRequest.geodeticCoordinates.latitude.Value();
    request.longitudeYOffset = hatHotRequest.geodeticCoordinates.longitude.Value();
    request.altitudeZOffset = hatHotRequest.geodeticCoordinates.altitude.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::HAT_HOT_REQUEST) << "\n"
                        << "HAT/HOT ID: " << std::to_string(hatHotRequest.requestID.Value()) << "\n"
                        << "Request Type: " << ConvertCigiRequestTypeToString(hatHotRequest.eRequestType) << "\n"
                        << "Coordinate System: Geodetic\n"
                        << "Update Period: " << std::to_string(hatHotRequest.updatePeriod.Value()) << "\n";

      args.sDataMessage << "Latitude: " << std::to_string(hatHotRequest.geodeticCoordinates.latitude.Value()) << "\n"
                        << "Longitude: " << std::to_string(hatHotRequest.geodeticCoordinates.longitude.Value()) << "\n"
                        << "Altitude: " << std::to_string(hatHotRequest.geodeticCoordinates.altitude.Value()) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, request, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendHatHotRequest(const SHATHOTEntityRequest& hatHotRequest)
{
  try
  {
    CIGI::V40::HATHOTRequest request;
    request.HATHOTId = hatHotRequest.requestID.Value();
    request.entityId = hatHotRequest.entityID.Value();
    request.requestType = ConvertToRequestType(hatHotRequest.eRequestType);
    request.coordinateSystem = CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Entity;// Entity requests use entity coordinate system
    request.updatePeriod = hatHotRequest.updatePeriod.Value();

    request.latitudeXOffset = hatHotRequest.offset[0];
    request.longitudeYOffset = hatHotRequest.offset[1];
    request.altitudeZOffset = hatHotRequest.offset[2];

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::HAT_HOT_REQUEST) << "\n"
                        << "HAT/HOT ID: " << std::to_string(hatHotRequest.requestID.Value()) << "\n"
                        << "Entity ID: " << std::to_string(hatHotRequest.entityID.Value()) << "\n"
                        << "Request Type: " << ConvertCigiRequestTypeToString(hatHotRequest.eRequestType) << "\n"
                        << "Coordinate System: Entity\n"
                        << "Update Period: " << std::to_string(hatHotRequest.updatePeriod.Value()) << "\n";

      args.sDataMessage << "X Offset: " << std::to_string(hatHotRequest.offset[0]) << "\n"
                        << "Y Offset: " << std::to_string(hatHotRequest.offset[1]) << "\n"
                        << "Z Offset: " << std::to_string(hatHotRequest.offset[2]) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, request, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(const SLineOfSightSegmentRequestGeodeticToGeodeticBasic& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Basic;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;//basic does not have response coordinate system field, so set to 0.
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

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationGeodeticCoordinates.latitude.Value();
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationGeodeticCoordinates.longitude.Value();
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationGeodeticCoordinates.altitude.Value();
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Basic"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source Latitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Source Longitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Source Altitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.altitude.Value()) << "\n";
      }

      {
        args.sDataMessage << "Destination Latitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Destination Longitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Destination Altitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.altitude.Value()) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestGeodeticToGeodeticExtended(const SLineOfSightSegmentRequestGeodeticToGeodeticExtended& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Extended;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;//Extended does not have response coordinate system field, so set to 0.
    losSegmentRequest.destinationEntityIdValid = false;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;// geodetic destination does not have a destination entity id.
    losSegmentRequest.materialMask = losRequest.nMaterialMask;

    {
      losSegmentRequest.sourceLatitudeXOffset = losRequest.sourceGeodeticCoordinates.latitude.Value();
      losSegmentRequest.sourceLongitudeYOffset = losRequest.sourceGeodeticCoordinates.longitude.Value();
      losSegmentRequest.sourceAltitudeZOffset = losRequest.sourceGeodeticCoordinates.altitude.Value();
    }

    {
      losSegmentRequest.destinationLatitudeXOffset = losRequest.destinationGeodeticCoordinates.latitude.Value();
      losSegmentRequest.destinationLongitudeYOffset = losRequest.destinationGeodeticCoordinates.longitude.Value();
      losSegmentRequest.destinationAltitudeZOffset = losRequest.destinationGeodeticCoordinates.altitude.Value();
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Extended"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source Latitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Source Longitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Source Altitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.altitude.Value()) << "\n";
      }

      {
        args.sDataMessage << "Destination Latitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Destination Longitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Destination Altitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.altitude.Value()) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

      Event::Raise<HostCigiEvent>(args);
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestGeodeticToEntityBasic(const SLineOfSightSegmentRequestGeodeticToEntityBasic& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Basic;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;// basic does not have a response coordinate system field.
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Basic"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      args.sDataMessage << "Destination Entity ID Valid: 1\n"
                        << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source Latitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Source Longitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Source Altitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.altitude.Value()) << "\n";
      }

      {
        args.sDataMessage << "Destination X Offset: " << std::to_string(losRequest.destinationOffset[0]) << "\n"
                          << "Destination Y Offset: " << std::to_string(losRequest.destinationOffset[1]) << "\n"
                          << "Destination Z Offset: " << std::to_string(losRequest.destinationOffset[2]) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestGeodeticToEntityExtended(const SLineOfSightSegmentRequestGeodeticToEntityExtended& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = 0;//no source entity associated with geodetic source request
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Extended;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;// Extended does not have a response coordinate system field.
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Extended"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      args.sDataMessage << "Destination Entity ID Valid: 1\n"
                        << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source Latitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Source Longitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Source Altitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.altitude.Value()) << "\n";
      }

      {
        args.sDataMessage << "Destination X Offset: " << std::to_string(losRequest.destinationOffset[0]) << "\n"
                          << "Destination Y Offset: " << std::to_string(losRequest.destinationOffset[1]) << "\n"
                          << "Destination Z Offset: " << std::to_string(losRequest.destinationOffset[2]) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

      Event::Raise<HostCigiEvent>(args);
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestEntityToGeodeticBasic(const SLineOfSightSegmentRequestEntityToGeodeticBasic& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = losRequest.sourceEntityID.Value();
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Basic;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;// basic does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = false;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;// geodetic destination does not have a destination entity id.

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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Basic"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source X Offset: " << std::to_string(losRequest.sourceOffset[0]) << "\n"
                          << "Source Y Offset: " << std::to_string(losRequest.sourceOffset[1]) << "\n"
                          << "Source Z Offset: " << std::to_string(losRequest.sourceOffset[2]) << "\n";
      }

      {
        args.sDataMessage << "Destination Latitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Destination Longitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Destination Altitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.altitude.Value()) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestEntityToGeodeticExtended(const SLineOfSightSegmentRequestEntityToGeodeticExtended& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = losRequest.sourceEntityID.Value();
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Extended;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losSegmentRequest.responseCoordinateSystem = 0;// Extended does not have a response coordinate system field.
    losSegmentRequest.destinationEntityIdValid = false;
    losSegmentRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losSegmentRequest.updatePeriod = losRequest.updatePeriod.Value();
    losSegmentRequest.destinationEntityId = 0;// geodetic destination does not have a destination entity id.

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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Extended"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source X Offset: " << std::to_string(losRequest.sourceOffset[0]) << "\n"
                          << "Source Y Offset: " << std::to_string(losRequest.sourceOffset[1]) << "\n"
                          << "Source Z Offset: " << std::to_string(losRequest.sourceOffset[2]) << "\n";
      }

      {
        args.sDataMessage << "Destination Latitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Destination Longitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Destination Altitude: " << std::to_string(losRequest.destinationGeodeticCoordinates.altitude.Value()) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      losSegmentRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

      Event::Raise<HostCigiEvent>(args);
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestEntityToEntityBasic(const SLineOfSightSegmentRequestEntityToEntityBasic& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = losRequest.sourceEntityID.Value();
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Basic;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;// basic does not have a response coordinate system field.
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      {
        args.sDataMessage << "Source Entity ID: " << std::to_string(losRequest.sourceEntityID.Value()) << "\n";
      }

      args.sDataMessage << "Request Type: "
                        << "Basic"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      args.sDataMessage << "Destination Entity ID Valid: " << ValidToString(losRequest.destinationEntityID != losRequest.sourceEntityID) << "\n"
                        << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Destination Entity ID: " << std::to_string(losRequest.destinationEntityID.Value()) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source X Offset: " << std::to_string(losRequest.sourceOffset[0]) << "\n"
                          << "Source Y Offset: " << std::to_string(losRequest.sourceOffset[1]) << "\n"
                          << "Source Z Offset: " << std::to_string(losRequest.sourceOffset[2]) << "\n";
      }

      {
        args.sDataMessage << "Destination X Offset: " << std::to_string(losRequest.destinationOffset[0]) << "\n"
                          << "Destination Y Offset: " << std::to_string(losRequest.destinationOffset[1]) << "\n"
                          << "Destination Z Offset: " << std::to_string(losRequest.destinationOffset[2]) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightSegmentRequestEntityToEntityExtended(const SLineOfSightSegmentRequestEntityToEntityExtended& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightSegmentRequest losSegmentRequest;
    losSegmentRequest.LOSId = losRequest.requestID.Value();
    losSegmentRequest.sourceEntityId = losRequest.sourceEntityID.Value();
    losSegmentRequest.requestType = CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Extended;
    losSegmentRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.destinationPointCoordinateSystem = CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losSegmentRequest.responseCoordinateSystem = 0;// Extended does not have a response coordinate system field.
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      {
        args.sDataMessage << "Source Entity ID: " << std::to_string(losRequest.sourceEntityID.Value()) << "\n";
      }

      args.sDataMessage << "Request Type: "
                        << "Extended"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      {
        args.sDataMessage << "Destination Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      args.sDataMessage << "Destination Entity ID Valid: " << ValidToString(losRequest.destinationEntityID != losRequest.sourceEntityID) << "\n"
                        << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Destination Entity ID: " << std::to_string(losRequest.destinationEntityID.Value()) << "\n";

      args.sDataMessage << "Update Period: " << std::to_string(losRequest.updatePeriod.Value()) << "\n";

      {
        args.sDataMessage << "Source X Offset: " << std::to_string(losRequest.sourceOffset[0]) << "\n"
                          << "Source Y Offset: " << std::to_string(losRequest.sourceOffset[1]) << "\n"
                          << "Source Z Offset: " << std::to_string(losRequest.sourceOffset[2]) << "\n";
      }

      {
        args.sDataMessage << "Destination X Offset: " << std::to_string(losRequest.destinationOffset[0]) << "\n"
                          << "Destination Y Offset: " << std::to_string(losRequest.destinationOffset[1]) << "\n"
                          << "Destination Z Offset: " << std::to_string(losRequest.destinationOffset[2]) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    PackPacketV4(this, losSegmentRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightVectorRequestGeodeticBasic(const SLineOfSightVectorRequestGeodeticBasic& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V40::LineOfSightVectorRequest::RequestType::eRequestType_Basic;

    losVectorRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losVectorRequest.responseCoordinateSystem = 0;// basic does not have a response coordinate system field.
    losVectorRequest.alphaThreshold = losRequest.nAlphaThreshold;
    losVectorRequest.entityId = 0;//no source entity associated with geodetic source request
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::LINE_OF_SIGHT_VECTOR_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Basic"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Azimuth: " << std::to_string(losRequest.azimuth.Value()) << "\n"
                        << "Elevation: " << std::to_string(losRequest.elevation.Value()) << "\n"
                        << "Minimum Range: " << std::to_string(losRequest.fMaximumRange) << "\n"
                        << "Maximum Range: " << std::to_string(losRequest.fMinimumRange) << "\n";

      {
        args.sDataMessage << "Source Latitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Source Longitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Source Altitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.altitude.Value()) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n"
                        << "Update Period: " << std::to_string(losRequest.updatePeriod.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, losVectorRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightVectorRequestGeodeticExtended(const SLineOfSightVectorRequestGeodeticExtended& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V40::LineOfSightVectorRequest::RequestType::eRequestType_Extended;
    losVectorRequest.entityId = 0;//no source entity associated with geodetic source request
    losVectorRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    losVectorRequest.responseCoordinateSystem = 0;// Extended does not have a response coordinate system field.
    losVectorRequest.alphaThreshold = losRequest.nAlphaThreshold;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::LINE_OF_SIGHT_VECTOR_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Extended"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Geodetic"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      args.sDataMessage << "Azimuth: " << std::to_string(losRequest.azimuth.Value()) << "\n"
                        << "Elevation: " << std::to_string(losRequest.elevation.Value()) << "\n"
                        << "Minimum Range: " << std::to_string(losRequest.fMaximumRange) << "\n"
                        << "Maximum Range: " << std::to_string(losRequest.fMinimumRange) << "\n";

      {
        args.sDataMessage << "Source Latitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.latitude.Value()) << "\n"
                          << "Source Longitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.longitude.Value()) << "\n"
                          << "Source Altitude: " << std::to_string(losRequest.sourceGeodeticCoordinates.altitude.Value()) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n"
                        << "Update Period: " << std::to_string(losRequest.updatePeriod.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    PackPacketV4(this, losVectorRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightVectorRequestEntityBasic(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();

    losVectorRequest.requestType = CIGI::V40::LineOfSightVectorRequest::RequestType::eRequestType_Basic;

    losVectorRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losVectorRequest.responseCoordinateSystem = 0;// basic does not have a response coordinate system field.
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::LINE_OF_SIGHT_VECTOR_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Basic"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      {
        args.sDataMessage << "Entity ID: " << std::to_string(losRequest.sourceEntityID.Value()) << "\n";
      }

      args.sDataMessage << "Azimuth: " << std::to_string(losRequest.azimuth.Value()) << "\n"
                        << "Elevation: " << std::to_string(losRequest.elevation.Value()) << "\n"
                        << "Minimum Range: " << std::to_string(losRequest.fMaximumRange) << "\n"
                        << "Maximum Range: " << std::to_string(losRequest.fMinimumRange) << "\n";

      {
        args.sDataMessage << "Source X Offset: " << std::to_string(losRequest.sourceOffset[0]) << "\n"
                          << "Source Y Offset: " << std::to_string(losRequest.sourceOffset[1]) << "\n"
                          << "Source Z Offset: " << std::to_string(losRequest.sourceOffset[2]) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n"
                        << "Update Period: " << std::to_string(losRequest.updatePeriod.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, losVectorRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendLineOfSightVectorRequestEntityExtended(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& losRequest)
{
  try
  {
    CIGI::V40::LineOfSightVectorRequest losVectorRequest;
    losVectorRequest.LOSId = losRequest.requestID.Value();
    losVectorRequest.requestType = CIGI::V40::LineOfSightVectorRequest::RequestType::eRequestType_Extended;
    losVectorRequest.sourcePointCoordinateSystem = CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity;
    losVectorRequest.responseCoordinateSystem = 0;// Extended does not have a response coordinate system field.
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
    losVectorRequest.responseCoordinateSystem = ConvertToCoordSysGrp(losRequest.eResponseCoordinateSystem);

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV3::LINE_OF_SIGHT_VECTOR_REQUEST) << "\n"
                        << "LOS ID: " << std::to_string(losRequest.requestID.Value()) << "\n";

      args.sDataMessage << "Request Type: "
                        << "Extended"
                        << "\n";

      {
        args.sDataMessage << "Source Point Coordinate System: "
                          << "Entity"
                          << "\n";
      }

      args.sDataMessage << "Alpha Threshold: " << std::to_string(losRequest.nAlphaThreshold) << "\n";

      {
        args.sDataMessage << "Entity ID: " << std::to_string(losRequest.sourceEntityID.Value()) << "\n";
      }

      args.sDataMessage << "Azimuth: " << std::to_string(losRequest.azimuth.Value()) << "\n"
                        << "Elevation: " << std::to_string(losRequest.elevation.Value()) << "\n"
                        << "Minimum Range: " << std::to_string(losRequest.fMaximumRange) << "\n"
                        << "Maximum Range: " << std::to_string(losRequest.fMinimumRange) << "\n";

      {
        args.sDataMessage << "Source X Offset: " << std::to_string(losRequest.sourceOffset[0]) << "\n"
                          << "Source Y Offset: " << std::to_string(losRequest.sourceOffset[1]) << "\n"
                          << "Source Z Offset: " << std::to_string(losRequest.sourceOffset[2]) << "\n";
      }

      args.sDataMessage << "Material Mask: " << std::to_string(losRequest.nMaterialMask) << "\n"
                        << "Update Period: " << std::to_string(losRequest.updatePeriod.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    StoreLineOfSightRequestCoordinateSystem(losRequest.requestID, losRequest.eResponseCoordinateSystem);
    PackPacketV4(this, losVectorRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEntityMaritimeSurfaceConditionsControl(EntityID entityID, const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  try
  {
    HostCigiMessageEventArgs hostCigiMessageArgs;
    hostCigiMessageArgs.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL);
    Event::Raise<HostCigiEvent>(hostCigiMessageArgs);

    CIGI::V40::MaritimeSurfaceConditionsCtrl maritimeSurfaceCtrl;
    maritimeSurfaceCtrl.entityRegionId = entityID.Value();
    maritimeSurfaceCtrl.surfaceConditionsEnable = maritimeSurfaceCondition.bActive;
    maritimeSurfaceCtrl.whitecapEnable = maritimeSurfaceCondition.bWhitecapEnabled;
    maritimeSurfaceCtrl.scope = CIGI::V40::MaritimeSurfaceConditionsCtrl::eScope_Entity;
    maritimeSurfaceCtrl.seaSurfaceHeight = static_cast<float>(maritimeSurfaceCondition.fSeaSurfaceHeight.Value());
    maritimeSurfaceCtrl.surfaceWaterTemperature = maritimeSurfaceCondition.fSurfaceWaterTemperature.Value();
    maritimeSurfaceCtrl.surfaceClarity = ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL) << "\n"
                        << "Surface Conditions Enable: " << EnableToString(maritimeSurfaceCondition.bActive) << "\n"
                        << "Whitecap Enable: " << EnableToString(maritimeSurfaceCondition.bWhitecapEnabled) << "\n"
                        << "Scope: Entity\n"
                        << "Entity ID: " << std::to_string(entityID.Value()) << "\n"
                        << "Sea Surface Height: " << std::to_string(maritimeSurfaceCondition.fSeaSurfaceHeight.Value()) << "\n"
                        << "Surface Water Temperature: " << std::to_string(maritimeSurfaceCondition.fSurfaceWaterTemperature.Value()) << "\n"
                        << "Surface Clarity: " << std::to_string(ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value()) << "%";
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, maritimeSurfaceCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendRegionMaritimeSurfaceConditionsControl(RegionID regionID, const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  try
  {
    HostCigiMessageEventArgs hostCigiMessageArgs;
    hostCigiMessageArgs.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL);
    Event::Raise<HostCigiEvent>(hostCigiMessageArgs);

    CIGI::V40::MaritimeSurfaceConditionsCtrl maritimeSurfaceCtrl;
    maritimeSurfaceCtrl.entityRegionId = regionID.Value();
    maritimeSurfaceCtrl.surfaceConditionsEnable = maritimeSurfaceCondition.bActive;
    maritimeSurfaceCtrl.whitecapEnable = maritimeSurfaceCondition.bWhitecapEnabled;
    maritimeSurfaceCtrl.scope = CIGI::V40::MaritimeSurfaceConditionsCtrl::eScope_Regional;
    maritimeSurfaceCtrl.seaSurfaceHeight = static_cast<float>(maritimeSurfaceCondition.fSeaSurfaceHeight.Value());
    maritimeSurfaceCtrl.surfaceWaterTemperature = maritimeSurfaceCondition.fSurfaceWaterTemperature.Value();
    maritimeSurfaceCtrl.surfaceClarity = ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL) << "\n"
                        << "Surface Conditions Enable: " << EnableToString(maritimeSurfaceCondition.bActive) << "\n"
                        << "Whitecap Enable: " << EnableToString(maritimeSurfaceCondition.bWhitecapEnabled) << "\n"
                        << "Scope: Region\n"
                        << "Region ID: " << std::to_string(regionID.Value()) << "\n"
                        << "Sea Surface Height: " << std::to_string(maritimeSurfaceCondition.fSeaSurfaceHeight.Value()) << "\n"
                        << "Surface Water Temperature: " << std::to_string(maritimeSurfaceCondition.fSurfaceWaterTemperature.Value()) << "\n"
                        << "Surface Clarity: " << std::to_string(ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value()) << "%";
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, maritimeSurfaceCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendGlobalMaritimeSurfaceConditionsControl(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  try
  {

    CIGI::V40::MaritimeSurfaceConditionsCtrl maritimeSurfaceCtrl;
    maritimeSurfaceCtrl.surfaceConditionsEnable = maritimeSurfaceCondition.bActive;
    maritimeSurfaceCtrl.whitecapEnable = maritimeSurfaceCondition.bWhitecapEnabled;
    maritimeSurfaceCtrl.scope = CIGI::V40::MaritimeSurfaceConditionsCtrl::eScope_Global;
    maritimeSurfaceCtrl.seaSurfaceHeight = static_cast<float>(maritimeSurfaceCondition.fSeaSurfaceHeight.Value());
    maritimeSurfaceCtrl.surfaceWaterTemperature = maritimeSurfaceCondition.fSurfaceWaterTemperature.Value();
    maritimeSurfaceCtrl.surfaceClarity = ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL) << "\n"
                        << "Surface Conditions Enable: " << EnableToString(maritimeSurfaceCondition.bActive) << "\n"
                        << "Whitecap Enable: " << EnableToString(maritimeSurfaceCondition.bWhitecapEnabled) << "\n"
                        << "Scope: Global\n"
                        << "Sea Surface Height: " << std::to_string(maritimeSurfaceCondition.fSeaSurfaceHeight.Value()) << "\n"
                        << "Surface Water Temperature: " << std::to_string(maritimeSurfaceCondition.fSurfaceWaterTemperature.Value()) << "\n"
                        << "Surface Clarity: " << std::to_string(ConvertToCigiFloatPercentage(maritimeSurfaceCondition.surfaceClarity).Value()) << "%";
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, maritimeSurfaceCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendMotionTrackerViewControl(const SMotionTrackerViewControl& motionTrackerControl)
{
  try
  {
    CIGI::V40::MotionTrackerCtrl motionTrack;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::MOTION_TRACKER_CONTROL) << "\n"
                        << "Tracker ID: " << std::to_string(motionTrackerControl.motionTrackerID.Value()) << "\n"
                        << "Tracker Enable: " << EnableToString(motionTrackerControl.bEnable) << "\n"
                        << "Boresight Enable: " << EnableToString(motionTrackerControl.bBoresightEnable) << "\n"
                        << "X Enable: " << EnableToString(motionTrackerControl.bXEnable) << "\n"
                        << "Y Enable: " << EnableToString(motionTrackerControl.bYEnable) << "\n"
                        << "Z Enable: " << EnableToString(motionTrackerControl.bZEnable) << "\n"
                        << "Roll Enable: " << EnableToString(motionTrackerControl.bRollEnable) << "\n"
                        << "Pitch Enable: " << EnableToString(motionTrackerControl.bPitchEnable) << "\n"
                        << "Yaw Enable: " << EnableToString(motionTrackerControl.bYawEnable) << "\n";

      args.sDataMessage << "View/View Group Select: "
                        << "View"
                        << "\n"
                        << "View/View Group ID: " << std::to_string(motionTrackerControl.viewID.Value());

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, motionTrack, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendMotionTrackerViewGroupControl(const SMotionTrackerViewGroupControl& motionTrackerControl)
{
  try
  {
    CIGI::V40::MotionTrackerCtrl motionTrack;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::MOTION_TRACKER_CONTROL) << "\n"
                        << "Tracker ID: " << std::to_string(motionTrackerControl.motionTrackerID.Value()) << "\n"
                        << "Tracker Enable: " << EnableToString(motionTrackerControl.bEnable) << "\n"
                        << "Boresight Enable: " << EnableToString(motionTrackerControl.bBoresightEnable) << "\n"
                        << "X Enable: " << EnableToString(motionTrackerControl.bXEnable) << "\n"
                        << "Y Enable: " << EnableToString(motionTrackerControl.bYEnable) << "\n"
                        << "Z Enable: " << EnableToString(motionTrackerControl.bZEnable) << "\n"
                        << "Roll Enable: " << EnableToString(motionTrackerControl.bRollEnable) << "\n"
                        << "Pitch Enable: " << EnableToString(motionTrackerControl.bPitchEnable) << "\n"
                        << "Yaw Enable: " << EnableToString(motionTrackerControl.bYawEnable) << "\n";

      args.sDataMessage << "View/View Group Select: "
                        << "View Group"
                        << "\n"
                        << "View/View Group ID: " << std::to_string(motionTrackerControl.viewGroupID.Value());

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, motionTrack, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendPositionRequest(const sbio::cigi::SPositionRequest& positionRequest)
{
  try
  {
    CIGI::V40::PositionRequest posRequest;
    posRequest.objectId = positionRequest.nObjectID;
    posRequest.articulatedPartId = positionRequest.articulatedPartID.Value();
    posRequest.updateMode = positionRequest.bContinuous ? CIGI::V40::PositionRequest::UpdateMode::eUpdateMode_Continuous : CIGI::V40::PositionRequest::UpdateMode::eUpdateMode_OneShot;
    posRequest.objectClass = GetCigiObjectClassGrp(positionRequest.eObjectClass);
    posRequest.coordinateSystem = ConvertToCoordSysGrp(positionRequest.eCoordinateSystem);

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::POSITION_REQUEST) << "\n"
                        << "Articulated Part ID: " << std::to_string(positionRequest.articulatedPartID.Value()) << "\n";

      if (positionRequest.bContinuous)
      {
        args.sDataMessage << "Update Mode: "
                          << "Continuous"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Update Mode: "
                          << "One-Shot"
                          << "\n";
      }

      args.sDataMessage << "Object Class: " << ConvertObjectClassToString(positionRequest.eObjectClass) << "\n";

      if (!(positionRequest.eObjectClass == EObjectClass::MOTION_TRACKER))
      {
        args.sDataMessage << "Coordinate System: " << ConvertCoordSysGeoParSubToString(ConvertToCigiCoordSys(positionRequest.eCoordinateSystem)) << "\n";
      }

      args.sDataMessage << "Object ID: " << std::to_string(positionRequest.nObjectID) << "\n";

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, posRequest, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V40::SensorCtrl::Polarity ConvertToTrackPolarity(ESensorTrack eSensorTrack)
{
  if (eSensorTrack == ESensorTrack::WHITE)
  {
    return CIGI::V40::SensorCtrl::Polarity::ePolarity_WhiteHot;
  }
  else if (eSensorTrack == ESensorTrack::BLACK)
  {
    return CIGI::V40::SensorCtrl::Polarity::ePolarity_BlackHot;
  }

  return CIGI::V40::SensorCtrl::Polarity::ePolarity_WhiteHot;
}

/// <summary>
/// Takes a SCigiSensorControl struct
/// creates and packs a sensor control packet
/// </summary>
void CHostSessionV4::SendSensorControl(const SCigiSensorControl& sensorControl)
{
  try
  {
    CIGI::V40::SensorCtrl sensorCtrl;
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
      sensorCtrl.responseType = CIGI::V40::SensorCtrl::ResponseType::eResponseType_Extended;
    }
    else
    {
      sensorCtrl.responseType = CIGI::V40::SensorCtrl::ResponseType::eResponseType_Normal;
    }

    sensorCtrl.gain = sensorControl.gain.Value();
    sensorCtrl.level = sensorControl.level.Value();
    sensorCtrl.acCoupling = sensorControl.fACCoupling;
    sensorCtrl.noise = sensorControl.noise.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SENSOR_CONTROL) << "\n"
                        << "Sensor ID: " << std::to_string(sensorControl.sensorID.Value()) << "\n"
                        << "Track Mode: " << ConvertCigiTrackModeToString(sensorControl.eTrackMode) << "\n";

      if (sensorControl.bSensorOn)
      {
        args.sDataMessage << "Sensor: "
                          << "On"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Sensor: "
                          << "Off"
                          << "\n";
      }

      args.sDataMessage << "Polarity: " << ConvertCigiPolarityToString(sensorControl.ePolarity) << "\n"
                        << "Line-by-Line Dropout Enable: " << EnableToString(sensorControl.bLineByLineDropoutEnabled) << "\n"
                        << "Automatic Gain: " << EnableToString(sensorControl.bAutomaticGain) << "\n";

      if (sensorControl.eSensorTrack == ESensorTrack::BLACK)
      {
        args.sDataMessage << "Track White/Black: "
                          << "Black"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Track White/Black: "
                          << "White"
                          << "\n";
      }

      if (sensorControl.bExtendedResponse)
      {
        args.sDataMessage << "Response Type: "
                          << "Extended"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Response Type: "
                          << "Normal"
                          << "\n";
      }

      args.sDataMessage << "View ID: " << std::to_string(sensorControl.viewID.Value()) << "\n"
                        << "Gain: " << std::to_string(sensorControl.gain.Value()) << "\n"
                        << "Level: " << std::to_string(sensorControl.level.Value()) << "\n"
                        << "AC Coupling: " << std::to_string(sensorControl.fACCoupling) << "\n"
                        << "Noise: " << std::to_string(sensorControl.noise.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, sensorCtrl, m_bByteSwap);
  }
  catch (const std::exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendShortArticulatedPartControl(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart)
{
  try
  {
    CIGI::V40::ShortArticulatedPartCtrl shortArtPartCtrl;
    shortArtPartCtrl.entityId = shortArticulatedPart.entityID.Value();
    shortArtPartCtrl.articulatedPartId1 = shortArticulatedPart.articulatedPartID1.Value();
    shortArtPartCtrl.articulatedPartId2 = shortArticulatedPart.articulatedPartID2.Value();
    shortArtPartCtrl.dofSelect1 = ConvertToDOFSelect(shortArticulatedPart.eDOF1);
    shortArtPartCtrl.dofSelect2 = ConvertToDOFSelect(shortArticulatedPart.eDOF2);
    shortArtPartCtrl.articulatedPartEnable1 = shortArticulatedPart.bArticulatedPart1Enabled;
    shortArtPartCtrl.articulatedPartEnable2 = shortArticulatedPart.bArticulatedPart2Enabled;
    shortArtPartCtrl.dof1 = shortArticulatedPart.fDOF1;
    shortArtPartCtrl.dof2 = shortArticulatedPart.fDOF2;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SHORT_ARTICULATED_PART_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(shortArticulatedPart.entityID.Value()) << "\n"
                        << "Articulated Part ID 1: " << std::to_string(shortArticulatedPart.articulatedPartID1.Value()) << "\n"
                        << "Articulated Part ID 2: " << std::to_string(shortArticulatedPart.articulatedPartID2.Value()) << "\n"
                        << "DOF Select 1: " << ConvertCigiDOFSelectToString(shortArticulatedPart.eDOF1) << "\n"
                        << "DOF Select 2: " << ConvertCigiDOFSelectToString(shortArticulatedPart.eDOF2) << "\n"
                        << "Articulated Part Enable 1: " << EnableToString(shortArticulatedPart.bArticulatedPart1Enabled) << "\n"
                        << "Articulated Part Enable 2: " << EnableToString(shortArticulatedPart.bArticulatedPart2Enabled) << "\n"
                        << "DOF 1: " << std::to_string(shortArticulatedPart.fDOF1) << "\n"
                        << "DOF 2: " << std::to_string(shortArticulatedPart.fDOF2) << "\n";
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, shortArtPartCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendShortComponentControl(const SCigiShortComponentControl& shortComponentControl)
{
  try
  {
    CIGI::V40::ShortComponentCtrl shortCompCtrl;
    shortCompCtrl.componentId = shortComponentControl.componentID.Value();
    shortCompCtrl.instanceId = shortComponentControl.nInstanceID;
    shortCompCtrl.componentClass = ConvertToCompClass(shortComponentControl.componentClassID.Value());
    shortCompCtrl.componentState = shortComponentControl.nComponentState;
    shortCompCtrl.componentData[0] = shortComponentControl.componentData1;
    shortCompCtrl.componentData[1] = shortComponentControl.componentData2;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SHORT_COMPONENT_CONTROL) << "\n"
                        << "Component ID: " << std::to_string(shortComponentControl.componentID.Value()) << "\n"
                        << "Component Class: " << g_pScriptRuntime->GetComponentClassString(shortComponentControl.componentClassID) << "\n"
                        << "Component State: " << std::to_string(shortComponentControl.nComponentState) << "\n"
                        << "Instance ID: " << std::to_string(shortComponentControl.componentID.Value()) << "\n"
                        << "Component Data 1:" << std::to_string(shortComponentControl.componentData1) << "\n"
                        << "Component Data 2:" << std::to_string(shortComponentControl.componentData2);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, shortCompCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V40::ShortSymbolCtrl::AttributeSelect ConvertAttributeSelect(EAttributeSelect attribute)
{
  if (attribute == EAttributeSelect::NONE)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_None;
  }
  else if (attribute == EAttributeSelect::SURFACEID)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_SurfaceID;
  }
  else if (attribute == EAttributeSelect::PARENTSYMBOLID)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ParentSymbolID;
  }
  else if (attribute == EAttributeSelect::LAYER)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Layer;
  }
  else if (attribute == EAttributeSelect::FLASHDUTYCYCLEPERCENTAGE)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashDutyCyclePercentage;
  }
  else if (attribute == EAttributeSelect::FLASHPERIOD)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_FlashPeriod;
  }
  else if (attribute == EAttributeSelect::POSITIONU)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionU;
  }
  else if (attribute == EAttributeSelect::POSITIONV)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_PositionV;
  }
  else if (attribute == EAttributeSelect::ROTATION)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Rotation;
  }
  else if (attribute == EAttributeSelect::COLOR)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_Color;
  }
  else if (attribute == EAttributeSelect::SCALEU)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleU;
  }
  else if (attribute == EAttributeSelect::SCALEV)
  {
    return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_ScaleV;
  }

  return CIGI::V40::ShortSymbolCtrl::AttributeSelect::eAttributeSelect_None;
}

uint32_t SetAttributeDatum(EAttributeSelect attribute, SAttributeValue value)
{
  if (attribute == EAttributeSelect::NONE)
  {
    return 0;
  }
  else if (attribute == EAttributeSelect::SURFACEID)
  {
    return value.n;
  }
  else if (attribute == EAttributeSelect::PARENTSYMBOLID)
  {
    return value.n;
  }
  else if (attribute == EAttributeSelect::LAYER)
  {
    return value.n;
  }
  else if (attribute == EAttributeSelect::FLASHDUTYCYCLEPERCENTAGE)
  {
    return value.n;
  }
  else if (attribute == EAttributeSelect::FLASHPERIOD)
  {
    int n = 0;
    memcpy(&n, &value.f, sizeof(uint32_t));
    return n;
  }
  else if (attribute == EAttributeSelect::POSITIONU)
  {
    int n = 0;
    memcpy(&n, &value.f, sizeof(uint32_t));
    return n;
  }
  else if (attribute == EAttributeSelect::POSITIONV)
  {
    int n = 0;
    memcpy(&n, &value.f, sizeof(uint32_t));
    return n;
  }
  else if (attribute == EAttributeSelect::ROTATION)
  {
    int n = 0;
    memcpy(&n, &value.f, sizeof(uint32_t));
    return n;
  }
  else if (attribute == EAttributeSelect::COLOR)
  {
    SColor32 color;
    color.r = ((int)value.rgba.r >> 24) & 0x0F;
    color.g = ((int)value.rgba.g >> 16) & 0x0F;
    color.b = ((int)value.rgba.b >> 8) & 0x0F;
    color.a = value.rgba.a & 0x0F;

    uint32_t n;
    memcpy(&n, &color, sizeof(uint32_t));
    return n;
  }
  else if (attribute == EAttributeSelect::SCALEU)
  {
    int n = 0;
    memcpy(&n, &value.f, sizeof(uint32_t));
    return n;
  }
  else if (attribute == EAttributeSelect::SCALEV)
  {
    int n = 0;
    memcpy(&n, &value.f, sizeof(uint32_t));
    return n;
  }
  else
  {
    return 0;
  }
}

void CHostSessionV4::SendShortSymbolControl(const SShortSymbolControl& shortSymbolControl)
{
  try
  {
    CIGI::V40::ShortSymbolCtrl shortSymbolCtrl;
    shortSymbolCtrl.symbolId = shortSymbolControl.symbolID.Value();
    shortSymbolCtrl.symbolState = ConvertToSymbolStateGrp(shortSymbolControl.eSymbolState);
    shortSymbolCtrl.attachState = ConvertToAttachState(shortSymbolControl.eAttachState);
    shortSymbolCtrl.flashControl = ConvertToFlashCtrlGrp(shortSymbolControl.eFlashControl);
    shortSymbolCtrl.inheritColor = ConvertToInheritColorGrp(shortSymbolControl.bInheritColor);

    shortSymbolCtrl.attributeSelect1 = static_cast<uint8_t>(ConvertAttributeSelect(shortSymbolControl.eAttributeSelect1));
    shortSymbolCtrl.attributeSelect2 = static_cast<uint8_t>(ConvertAttributeSelect(shortSymbolControl.eAttributeSelect2));
    shortSymbolCtrl.attributeValue1 = SetAttributeDatum(shortSymbolControl.eAttributeSelect1, shortSymbolControl.attributeValue1);
    shortSymbolCtrl.attributeValue2 = SetAttributeDatum(shortSymbolControl.eAttributeSelect2, shortSymbolControl.attributeValue2);

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SHORT_SYMBOL_CONTROL) << "\n"
                        << "Symbol ID: " << std::to_string(shortSymbolControl.symbolID.Value()) << "\n"
                        << "Symbol State: " << ConvertCigiSymbolStateToString(shortSymbolControl.eSymbolState) << "\n";

      if (shortSymbolControl.eAttachState == EAttachState::ATTACH)
      {
        args.sDataMessage << "Attach State: "
                          << "Attach"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Attach State: "
                          << "Detach"
                          << "\n";
      }

      if (shortSymbolControl.eFlashControl == EFlashControl::RESET)
      {
        args.sDataMessage << "Flash Control: "
                          << "Reset"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Flash Control: "
                          << "Continue"
                          << "\n";
      }

      if (!(shortSymbolControl.eAttachState == EAttachState::DETACH))
      {

        if (shortSymbolControl.bInheritColor)
        {
          args.sDataMessage << "Inherit Color: "
                            << "Inherited"
                            << "\n";
        }
        else
        {
          args.sDataMessage << "Inherit Color: "
                            << "Not Inherited"
                            << "\n";
        }
      }

      // Attribute Select 1-------------------------------------------------------------------------
      args.sDataMessage << "Attribute Select 1: "
                        << "\n";
      if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::SURFACEID)
      {
        args.sDataMessage << "Surface ID: " << std::to_string(shortSymbolControl.attributeValue1.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::PARENTSYMBOLID)
      {
        args.sDataMessage << "Parent Symbol ID: " << std::to_string(shortSymbolControl.attributeValue1.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::LAYER)
      {
        args.sDataMessage << "Layer: " << std::to_string(shortSymbolControl.attributeValue1.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::FLASHDUTYCYCLEPERCENTAGE)
      {
        args.sDataMessage << "Flash Duty Cycle Percentage: " << std::to_string(shortSymbolControl.attributeValue1.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::FLASHPERIOD)
      {
        args.sDataMessage << "Flash Period: " << std::to_string(shortSymbolControl.attributeValue1.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::POSITIONU)
      {
        args.sDataMessage << "Position U: " << std::to_string(shortSymbolControl.attributeValue1.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::POSITIONV)
      {
        args.sDataMessage << "Position V: " << std::to_string(shortSymbolControl.attributeValue1.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::ROTATION)
      {
        args.sDataMessage << "Rotation: " << std::to_string(shortSymbolControl.attributeValue1.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::COLOR)
      {
        args.sDataMessage << "Color: "
                          << "\n"
                          << "- Red: " << std::to_string(shortSymbolControl.attributeValue1.rgba.r) << "\n"
                          << "- Green: " << std::to_string(shortSymbolControl.attributeValue1.rgba.g) << "\n"
                          << "- Blue: " << std::to_string(shortSymbolControl.attributeValue1.rgba.b) << "\n"
                          << "- Alpha: " << std::to_string(shortSymbolControl.attributeValue1.rgba.a) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::SCALEU)
      {
        args.sDataMessage << "Scale U: " << std::to_string(shortSymbolControl.attributeValue1.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect1 == EAttributeSelect::SCALEV)
      {
        args.sDataMessage << "Scale V: " << std::to_string(shortSymbolControl.attributeValue1.f) << "\n";
      }
      else
      {
        args.sDataMessage << "None: "
                          << "\n";
      }

      // Attribute Select 2-------------------------------------------------------------------------
      args.sDataMessage << "Attribute Select 2: "
                        << "\n";
      if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::SURFACEID)
      {
        args.sDataMessage << "Surface ID: " << std::to_string(shortSymbolControl.attributeValue2.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::PARENTSYMBOLID)
      {
        args.sDataMessage << "Parent Symbol ID: " << std::to_string(shortSymbolControl.attributeValue2.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::LAYER)
      {
        args.sDataMessage << "Layer: " << std::to_string(shortSymbolControl.attributeValue2.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::FLASHDUTYCYCLEPERCENTAGE)
      {
        args.sDataMessage << "Flash Duty Cycle Percentage: " << std::to_string(shortSymbolControl.attributeValue2.n) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::FLASHPERIOD)
      {
        args.sDataMessage << "Flash Period: " << std::to_string(shortSymbolControl.attributeValue2.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::POSITIONU)
      {
        args.sDataMessage << "Position U: " << std::to_string(shortSymbolControl.attributeValue2.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::POSITIONV)
      {
        args.sDataMessage << "Position V: " << std::to_string(shortSymbolControl.attributeValue2.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::ROTATION)
      {
        args.sDataMessage << "Rotation: " << std::to_string(shortSymbolControl.attributeValue2.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::COLOR)
      {
        args.sDataMessage << "Color: "
                          << "\n"
                          << "- Red: " << std::to_string(shortSymbolControl.attributeValue2.rgba.r) << "\n"
                          << "- Green: " << std::to_string(shortSymbolControl.attributeValue2.rgba.g) << "\n"
                          << "- Blue: " << std::to_string(shortSymbolControl.attributeValue2.rgba.b) << "\n"
                          << "- Alpha: " << std::to_string(shortSymbolControl.attributeValue2.rgba.a) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::SCALEU)
      {
        args.sDataMessage << "Scale U: " << std::to_string(shortSymbolControl.attributeValue2.f) << "\n";
      }
      else if (shortSymbolControl.eAttributeSelect2 == EAttributeSelect::SCALEV)
      {
        args.sDataMessage << "Scale V: " << std::to_string(shortSymbolControl.attributeValue2.f) << "\n";
      }
      else
      {
        args.sDataMessage << "None: "
                          << "\n";
      }
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, shortSymbolCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void Swap(CIGI::V40::SymbolCircleDefinition& symbolCircle, int numRecords)
{
  CEndian::Swap(symbolCircle.size);
  uint16_t nPacketId = symbolCircle.id;
  CEndian::Swap(nPacketId);
  symbolCircle.id = nPacketId;
  symbolCircle.doByteSwapping();
  CEndian::Swap(symbolCircle.lineWidth);
  CEndian::Swap(symbolCircle.stipplePatternLength);
  for (int i = 0; i < numRecords; ++i)
  {
    CEndian::Swap(symbolCircle.records[i].centerU);
    CEndian::Swap(symbolCircle.records[i].centerV);
    CEndian::Swap(symbolCircle.records[i].radius);
    CEndian::Swap(symbolCircle.records[i].innerRadius);
    CEndian::Swap(symbolCircle.records[i].startAngle);
    CEndian::Swap(symbolCircle.records[i].endAngle);
  }
}

void CHostSessionV4::SendSymbolCircleDefinition(const SSymbolCircle& circleDef)
{
  try
  {
    CIGI::V40::SymbolCircleDefinition symbolCircle;
    symbolCircle.symbolId = circleDef.symbolID.Value();
    symbolCircle.drawingStyle = ConvertToDrawingStyleGrp(circleDef.eDrawingStyle);
    symbolCircle.stipplePattern = circleDef.stipplePattern;
    symbolCircle.lineWidth = circleDef.fLineWidth;
    symbolCircle.stipplePatternLength = circleDef.fStipplePatternLength;

    CIGI::VariableRecordHelper<CIGI::V40::SymbolCircleDefinition> helper(symbolCircle);

    for (const auto& circleProperties : circleDef.circles)
    {
      CIGI::V40::SymbolCircleDefinition::Circle circle;
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
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_CIRCLE_DEFINITION) << "\n"
                        << " Symbol ID: " << std::to_string(circleDef.symbolID.Value()) << "\n";

      if (circleDef.eDrawingStyle == EDrawingStyle::LINE)
      {
        args.sDataMessage << "Stipple Pattern: " << std::to_string(circleDef.stipplePattern) << "\n"
                          << "Drawing Style: "
                          << "Line"
                          << "\n"
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

    int numRecords = CIGI::V40::SymbolCircleDefinition::RecordHelper::numRecords(symbolCircle);
    if (m_bByteSwap)
    {
      Swap(symbolCircle, numRecords);
    }

    std::unique_ptr<TBuffer<char>> pPacketBuffer = std::make_unique<TBuffer<char>>(static_cast<int>(CIGI::V40::SymbolCircleDefinition::kBasePacketSize + numRecords * sizeof(CIGI::V40::SymbolCircleDefinition::Record)));
    memcpy(pPacketBuffer->GetBuffer(), &symbolCircle, CIGI::V40::SymbolCircleDefinition::kBasePacketSize);
    memcpy(pPacketBuffer->GetBuffer() + CIGI::V40::SymbolCircleDefinition::kBasePacketSize, symbolCircle.records, numRecords * sizeof(CIGI::V40::SymbolCircleDefinition::Record));
    Pack(pPacketBuffer->GetBuffer(), pPacketBuffer->GetSize());
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendSymbolClone(const SSymbolClone& symbolCloneStruct)
{
  try
  {
    CIGI::V40::SymbolClone symbolClone;
    symbolClone.symbolId = symbolCloneStruct.symbolID.Value();
    symbolClone.sourceId = symbolCloneStruct.sourceID.Value();

    if (symbolCloneStruct.eSymbolSourceType == ESymbolSourceType::SYMBOL)
    {
      symbolClone.sourceType = CIGI::V40::SymbolClone::SourceType::eSourceType_Symbol;
    }
    else
    {
      symbolClone.sourceType = CIGI::V40::SymbolClone::SourceType::eSourceType_SymbolTemplate;
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_CLONE) << "\n"
                        << "Symbol ID: " << std::to_string(symbolCloneStruct.symbolID.Value()) << "\n"
                        << "Source ID: " << std::to_string(symbolCloneStruct.sourceID.Value()) << "\n";

      if (symbolCloneStruct.eSymbolSourceType == ESymbolSourceType::SYMBOL)
      {
        args.sDataMessage << "Source Type: "
                          << "Symbol"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Source Type: "
                          << "Symbol Template"
                          << "\n";
      }

      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, symbolClone, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendSymbolControl(const SSymbolControl& symbolControl)
{
  try
  {
    CIGI::V40::SymbolCtrl symControl;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_CONTROL) << "\n"
                        << "Symbol ID: " << std::to_string(symbolControl.symbolID.Value()) << "\n"
                        << "Parent Symbol ID: " << std::to_string(symbolControl.parentSymbolID.Value()) << "\n"
                        << "Symbol State: " << ConvertCigiSymbolStateToString(symbolControl.eSymbolState) << "\n";

      if (symbolControl.eAttachState == EAttachState::ATTACH)
      {
        args.sDataMessage << "Attach State: "
                          << "Attach"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Attach State: "
                          << "Detach"
                          << "\n";
      }

      if (symbolControl.eFlashControl == EFlashControl::RESET)
      {
        args.sDataMessage << "Flash Control: "
                          << "Reset"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Flash Control: "
                          << "Continue"
                          << "\n";
      }

      if (!(symbolControl.eAttachState == EAttachState::DETACH))
      {

        if (symbolControl.bInheritColor)
        {
          args.sDataMessage << "Inherit Color: "
                            << "Inherited"
                            << "\n";
        }
        else
        {
          args.sDataMessage << "Inherit Color: "
                            << "Not Inherited"
                            << "\n";
        }
      }
      args.sDataMessage << "Layer: " << std::to_string(symbolControl.nLayerID) << "\n"
                        << "Flash Duty Cycle Percentage: " << std::to_string(ConvertToCigiIntPercentage(symbolControl.flashDutyCyclePercentage).Value()) << "\n"
                        << "Surface ID: " << std::to_string(symbolControl.surfaceID.Value()) << "\n"
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
    }

    PackPacketV4(this, symControl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void Swap(CIGI::V40::SymbolPolygonDefinition& symbolPolygon, int numRecords)
{
  CEndian::Swap(symbolPolygon.size);
  uint16_t nPacketId = symbolPolygon.id;
  CEndian::Swap(nPacketId);
  symbolPolygon.id = nPacketId;
  symbolPolygon.doByteSwapping();
  CEndian::Swap(symbolPolygon.lineWidth);
  CEndian::Swap(symbolPolygon.stipplePatternLength);
  for (int i = 0; i < numRecords; ++i)
  {
    CEndian::Swap(symbolPolygon.records[i].vertexU);
    CEndian::Swap(symbolPolygon.records[i].vertexV);
  }
}

void CHostSessionV4::SendSymbolPolygonDefinition(const SSymbolPolygon& symbolPolygon)
{
  try
  {
    CIGI::V40::SymbolPolygonDefinition symbolPolygonDef;
    symbolPolygonDef.symbolId = symbolPolygon.symbolID.Value();

    if (symbolPolygon.ePrimitiveType == EPrimitiveType::POINT)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_Point;
    }
    else if (symbolPolygon.ePrimitiveType == EPrimitiveType::LINE)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_Line;
    }
    else if (symbolPolygon.ePrimitiveType == EPrimitiveType::LINE_STRIP)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_LineStrip;
    }
    else if (symbolPolygon.ePrimitiveType == EPrimitiveType::LINE_LOOP)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_LineLoop;
    }
    else if (symbolPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_Triangle;
    }
    else if (symbolPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_STRIP)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleStrip;
    }
    else if (symbolPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_FAN)
    {
      symbolPolygonDef.primitiveType = CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleFan;
    }

    symbolPolygonDef.stipplePattern = symbolPolygon.nStipplePattern;
    symbolPolygonDef.lineWidth = symbolPolygon.fLineWidth;
    symbolPolygonDef.stipplePatternLength = symbolPolygon.fStipplePatternLength;

    CIGI::VariableRecordHelper<CIGI::V40::SymbolPolygonDefinition> helper(symbolPolygonDef);

    for (const auto& v : symbolPolygon.vertices)
    {
      CIGI::V40::SymbolPolygonDefinition::Vertex vertex;
      vertex.vertexU = v[0];
      vertex.vertexV = v[1];

      helper.push(vertex);
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_POLYGON_DEFINITION) << "\n"
                        << "Symbol ID: " << std::to_string(symbolPolygon.symbolID.Value()) << "\n";

      if (symbolPolygon.ePrimitiveType == EPrimitiveType::POINT)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Point"
                          << "\n"
                          << "Line Width: " << std::to_string(symbolPolygon.fLineWidth) << "\n"
                          << "Stipple Pattern Length: " << std::to_string(symbolPolygon.fStipplePatternLength) << "\n";
      }
      else if (symbolPolygon.ePrimitiveType == EPrimitiveType::LINE)
      {
        args.sDataMessage << "Stipple Pattern: " << std::to_string(symbolPolygon.nStipplePattern) << "\n"
                          << "Primitive Type: "
                          << "Line"
                          << "\n"
                          << "Line Width: " << std::to_string(symbolPolygon.fLineWidth) << "\n"
                          << "Stipple Pattern Length: " << std::to_string(symbolPolygon.fStipplePatternLength) << "\n";
      }
      else if (symbolPolygon.ePrimitiveType == EPrimitiveType::LINE_STRIP)
      {
        args.sDataMessage << "Stipple Pattern: " << std::to_string(symbolPolygon.nStipplePattern) << "\n"
                          << "Primitive Type: "
                          << "Line Strip"
                          << "\n"
                          << "Line Width: " << std::to_string(symbolPolygon.fLineWidth) << "\n"
                          << "Stipple Pattern Length: " << std::to_string(symbolPolygon.fStipplePatternLength) << "\n";
      }
      else if (symbolPolygon.ePrimitiveType == EPrimitiveType::LINE_LOOP)
      {
        args.sDataMessage << "Stipple Pattern: " << std::to_string(symbolPolygon.nStipplePattern) << "\n"
                          << "Primitive Type: "
                          << "Line Loop"
                          << "\n"
                          << "Line Width: " << std::to_string(symbolPolygon.fLineWidth) << "\n"
                          << "Stipple Pattern Length: " << std::to_string(symbolPolygon.fStipplePatternLength) << "\n";
      }
      else if (symbolPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Triangle"
                          << "\n";
      }
      else if (symbolPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_STRIP)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Triangle Strip"
                          << "\n";
      }
      else if (symbolPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_FAN)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Triangle Fan"
                          << "\n";
      }

      args.sDataMessage << "Vertices U V: "
                        << "\n";

      for (const auto& v : symbolPolygon.vertices)
      {
        args.sDataMessage << "( " << std::to_string(v[0]) << ", " << std::to_string(v[1]) << ")"
                          << "\n";
      }

      Event::Raise<HostCigiEvent>(args);
    }

    int numRecords = CIGI::V40::SymbolPolygonDefinition::RecordHelper::numRecords(symbolPolygonDef);
    if (m_bByteSwap)
    {
      Swap(symbolPolygonDef, numRecords);
    }

    Pack(&symbolPolygonDef, CIGI::V40::SymbolPolygonDefinition::kBasePacketSize, symbolPolygonDef.records, numRecords * static_cast<int>(sizeof(CIGI::V40::SymbolPolygonDefinition::Record)));
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEntitySymbolSurfaceDefinition(const SEntitySymbolSurfaceDefinition& symbolSurfaceDefinition)
{
  try
  {
    CIGI::V40::SymbolSurfaceDefinition symbolSurfaceDef;
    symbolSurfaceDef.surfaceId = symbolSurfaceDefinition.surfaceID.Value();
    symbolSurfaceDef.surfaceState = ConvertToSurfaceState(symbolSurfaceDefinition.eSurfaceState);
    symbolSurfaceDef.attachType = CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_Entity;
    symbolSurfaceDef.billboard = CIGI::V40::SymbolSurfaceDefinition::Billboard::eBillboard_NonBillboard;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_SURFACE_DEFINITION) << "\n"
                        << "Surface ID: " << std::to_string(symbolSurfaceDefinition.surfaceID.Value()) << "\n"
                        << "Entity ID/View ID: " << std::to_string(symbolSurfaceDefinition.entityID.Value()) << "\n";

      if (symbolSurfaceDefinition.eSurfaceState == ESurfaceState::DESTROYED)
      {
        args.sDataMessage << "Surface State: "
                          << "Destroyed"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Surface State: "
                          << "Active"
                          << "\n";
      }

      args.sDataMessage << "Attach Type: "
                        << "Entity"
                        << "\n"
                        << "Billboard: "
                        << "Non-Billboard"
                        << "\n"
                        << "X Offset: " << std::to_string(symbolSurfaceDefinition.offsetToSurface[0]) << "\n"
                        << "Y Offset: " << std::to_string(symbolSurfaceDefinition.offsetToSurface[1]) << "\n"
                        << "Z Offset: " << std::to_string(symbolSurfaceDefinition.offsetToSurface[2]) << "\n"
                        << "Yaw: " << std::to_string(symbolSurfaceDefinition.yaw.Value()) << "\n"
                        << "Pitch: " << std::to_string(symbolSurfaceDefinition.pitch.Value()) << "\n"
                        << "Roll: " << std::to_string(symbolSurfaceDefinition.roll.Value()) << "\n"
                        << "Width: " << std::to_string(symbolSurfaceDefinition.width) << "\n"
                        << "Height: " << std::to_string(symbolSurfaceDefinition.height) << "\n"
                        << "Min U: " << std::to_string(symbolSurfaceDefinition.uvMin[0]) << "\n"
                        << "Max U: " << std::to_string(symbolSurfaceDefinition.uvMax[0]) << "\n"
                        << "Min V: " << std::to_string(symbolSurfaceDefinition.uvMin[1]) << "\n"
                        << "Max V: " << std::to_string(symbolSurfaceDefinition.uvMax[1]);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, symbolSurfaceDef, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEntityBillboardSymbolSurfaceDefinition(const SEntityBillboardSymbolSurfaceDefinition& symbolSurfaceDefinition)
{
  try
  {
    CIGI::V40::SymbolSurfaceDefinition symbolSurfaceDef;
    symbolSurfaceDef.surfaceId = symbolSurfaceDefinition.surfaceID.Value();
    symbolSurfaceDef.surfaceState = ConvertToSurfaceState(symbolSurfaceDefinition.eSurfaceState);
    symbolSurfaceDef.attachType = CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_Entity;
    symbolSurfaceDef.billboard = CIGI::V40::SymbolSurfaceDefinition::Billboard::eBillboard_Billboard;
    symbolSurfaceDef.perspectiveGrowthEnable = symbolSurfaceDefinition.bPerspectiveGrowthEnabled;
    symbolSurfaceDef.entityIdViewId = symbolSurfaceDefinition.entityID.Value();
    symbolSurfaceDef.xOffsetLeft = symbolSurfaceDefinition.offsetToEntity[0];
    symbolSurfaceDef.yOffsetRight = symbolSurfaceDefinition.offsetToEntity[1];
    symbolSurfaceDef.zOffsetTop = symbolSurfaceDefinition.offsetToEntity[2];
    symbolSurfaceDef.minU = symbolSurfaceDefinition.uvMin[0];
    symbolSurfaceDef.maxU = symbolSurfaceDefinition.uvMax[0];
    symbolSurfaceDef.minV = symbolSurfaceDefinition.uvMin[1];
    symbolSurfaceDef.maxV = symbolSurfaceDefinition.uvMax[1];
    symbolSurfaceDef.width = symbolSurfaceDefinition.width;
    symbolSurfaceDef.height = symbolSurfaceDefinition.height;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_SURFACE_DEFINITION) << "\n"
                        << "Surface ID: " << std::to_string(symbolSurfaceDefinition.surfaceID.Value()) << "\n"
                        << "Entity ID/View ID: " << std::to_string(symbolSurfaceDefinition.entityID.Value()) << "\n";

      if (symbolSurfaceDefinition.eSurfaceState == ESurfaceState::DESTROYED)
      {
        args.sDataMessage << "Surface State: "
                          << "Destroyed"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Surface State: "
                          << "Active"
                          << "\n";
      }

      args.sDataMessage << "Attach Type: "
                        << "Entity"
                        << "\n"
                        << "Billboard: "
                        << "Billboard"
                        << "\n"
                        << "Perspective Growth Enable: " << EnabledToString(symbolSurfaceDefinition.bPerspectiveGrowthEnabled) << "\n"
                        << "X Offset: " << std::to_string(symbolSurfaceDefinition.offsetToEntity[0]) << "\n"
                        << "Y Offset: " << std::to_string(symbolSurfaceDefinition.offsetToEntity[1]) << "\n"
                        << "Z Offset: " << std::to_string(symbolSurfaceDefinition.offsetToEntity[2]) << "\n"
                        << "Width: " << std::to_string(symbolSurfaceDefinition.width) << "\n"
                        << "Height: " << std::to_string(symbolSurfaceDefinition.height) << "\n"
                        << "Min U: " << std::to_string(symbolSurfaceDefinition.uvMin[0]) << "\n"
                        << "Max U: " << std::to_string(symbolSurfaceDefinition.uvMax[0]) << "\n"
                        << "Min V: " << std::to_string(symbolSurfaceDefinition.uvMin[1]) << "\n"
                        << "Max V: " << std::to_string(symbolSurfaceDefinition.uvMax[1]);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, symbolSurfaceDef, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendViewSymbolSurfaceDefinition(const SViewSymbolSurfaceDefinition& symbolSurfaceDefinition)
{
  try
  {
    CIGI::V40::SymbolSurfaceDefinition symbolSurfaceDef;
    symbolSurfaceDef.surfaceId = symbolSurfaceDefinition.surfaceID.Value();
    symbolSurfaceDef.surfaceState = ConvertToSurfaceState(symbolSurfaceDefinition.eSurfaceState);
    symbolSurfaceDef.attachType = CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_View;
    symbolSurfaceDef.entityIdViewId = symbolSurfaceDefinition.viewID.Value();
    symbolSurfaceDef.xOffsetLeft = symbolSurfaceDefinition.fLeft;
    symbolSurfaceDef.yOffsetRight = symbolSurfaceDefinition.fRight;
    symbolSurfaceDef.zOffsetTop = symbolSurfaceDefinition.fTop;
    symbolSurfaceDef.yawBottom = symbolSurfaceDefinition.fBottom;
    symbolSurfaceDef.minU = symbolSurfaceDefinition.uvMin[0];
    symbolSurfaceDef.maxU = symbolSurfaceDefinition.uvMax[0];
    symbolSurfaceDef.minV = symbolSurfaceDefinition.uvMin[1];
    symbolSurfaceDef.maxV = symbolSurfaceDefinition.uvMax[1];
    symbolSurfaceDef.width = symbolSurfaceDefinition.width;
    symbolSurfaceDef.height = symbolSurfaceDefinition.height;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_SURFACE_DEFINITION) << "\n"
                        << "Surface ID: " << std::to_string(symbolSurfaceDefinition.surfaceID.Value()) << "\n"
                        << "Entity ID/View ID: " << std::to_string(symbolSurfaceDefinition.viewID.Value()) << "\n";

      if (symbolSurfaceDefinition.eSurfaceState == ESurfaceState::DESTROYED)
      {
        args.sDataMessage << "Surface State: "
                          << "Destroyed"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Surface State: "
                          << "Active"
                          << "\n";
      }

      args.sDataMessage << "Attach Type: "
                        << "View"
                        << "\n"
                        << "Left: " << std::to_string(symbolSurfaceDefinition.fLeft) << "\n"
                        << "Right: " << std::to_string(symbolSurfaceDefinition.fRight) << "\n"
                        << "Top: " << std::to_string(symbolSurfaceDefinition.fTop) << "\n"
                        << "Bottom: " << std::to_string(symbolSurfaceDefinition.fBottom) << "\n"
                        << "Min U: " << std::to_string(symbolSurfaceDefinition.uvMin[0]) << "\n"
                        << "Max U: " << std::to_string(symbolSurfaceDefinition.uvMax[0]) << "\n"
                        << "Min V: " << std::to_string(symbolSurfaceDefinition.uvMin[1]) << "\n"
                        << "Max V: " << std::to_string(symbolSurfaceDefinition.uvMax[1]);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, symbolSurfaceDef, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendSymbolTextDefinition(const SSymbolTextDefinition& symbolTextDef)
{
  try
  {
    CIGI::V40::SymbolTextDefinition symTextDef;
    symTextDef.symbolId = symbolTextDef.symbolID.Value();
    symTextDef.alignment = ConvertToAlignment(symbolTextDef.eTextAlignment);
    symTextDef.orientation = ConvertToOrientation(symbolTextDef.eTextOrientation);
    symTextDef.fontId = symbolTextDef.fontID.Value();
    symTextDef.fontSize = symbolTextDef.fFontSize;

    memset(symTextDef.data, '\0', CIGI::V40::SymbolTextDefinition::kMaxDataLength);
    size_t nCopyLength = std::min(symbolTextDef.sText.size(), static_cast<size_t>(CIGI::V40::SymbolTextDefinition::kMaxDataLength - 1));
    memcpy(symTextDef.data, symbolTextDef.sText.c_str(), nCopyLength);

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_TEXT_DEFINITION) << "\n"
                        << "Alignment: " << ConvertCigiAlignmentToString(symbolTextDef.eTextAlignment) << "\n"
                        << "Orientation: " << ConvertCigiOrientationToString(symbolTextDef.eTextOrientation) << "\n";

      if (symbolTextDef.fontID.Value() >= 1 && symbolTextDef.fontID.Value() <= 8)
      {
        args.sDataMessage << "Font ID: "
                          << "Proportional:"
                          << "\n"
                          << "- " << ConvertCigiFontIDToString(symbolTextDef.fontID.Value()) << "\n";
      }
      else if (symbolTextDef.fontID.Value() >= 9 && symbolTextDef.fontID.Value() <= 16)
      {
        args.sDataMessage << "Font ID: "
                          << "Monospace:"
                          << "\n"
                          << "- " << ConvertCigiFontIDToString(symbolTextDef.fontID.Value()) << "\n";
      }
      else if (symbolTextDef.fontID.Value() >= 17 && symbolTextDef.fontID.Value() <= 255)
      {
        args.sDataMessage << "Font ID: "
                          << "IG-defined"
                          << "\n";
      }
      else if (symbolTextDef.fontID.Value() == 0)
      {
        args.sDataMessage << "Font ID: "
                          << "IG default"
                          << "\n";
      }

      args.sDataMessage << "Symbol ID: " << std::to_string(symbolTextDef.symbolID.Value()) << "\n"
                        << "Font Size: " << std::to_string(symbolTextDef.fFontSize) << "\n"
                        << "Octet n:"
                        << "\n"
                        << symbolTextDef.sText;

      Event::Raise<HostCigiEvent>(args);
    }

    int nStringLength = static_cast<int>(strnlen(symTextDef.data, CIGI::V40::SymbolTextDefinition::kMaxDataLength)) + 1;
    int nSpare = nStringLength % 8;

    if (nStringLength < 3)
    {
      nSpare = 4 - nSpare;
    }
    else if (nSpare != 0)
    {
      nSpare = 8 - nSpare;
    }

    symTextDef.size += static_cast<CIGI::ui8>(nStringLength + nSpare);
    CIGI::V40::SymbolTextDefinition tempSymTextDef = symTextDef;
    if (m_bByteSwap)
    {
      ByteSwapPacketV4(tempSymTextDef);
      CEndian::Swap(tempSymTextDef.fontSize);
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &tempSymTextDef, CIGI::V40::SymbolTextDefinition::kBasePacketSize);
    m_nSendBufferLength += CIGI::V40::SymbolTextDefinition::kBasePacketSize;

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

void Swap(CIGI::V40::SymbolTexturedCircleDefinition& symbolTexturedCircleDefinition, int nNumRecords)
{
  //swap textured circle definition

  CEndian::Swap(symbolTexturedCircleDefinition.size);
  uint16_t nPacketId = symbolTexturedCircleDefinition.id;
  CEndian::Swap(nPacketId);
  symbolTexturedCircleDefinition.id = nPacketId;
  symbolTexturedCircleDefinition.doByteSwapping();
  CEndian::Swap(symbolTexturedCircleDefinition.symbolId);
  //CEndian::Swap(symbolTexturedCircleDefinition.textureFilterMode);//no need to swap 1-bit field
  CEndian::Swap(symbolTexturedCircleDefinition.textureId);
  //CEndian::Swap(symbolTexturedCircleDefinition.textureRepeatOrClamp);//no need to swap 1-bit field

  for (int i = 0; i < nNumRecords; ++i)
  {
    CEndian::Swap(symbolTexturedCircleDefinition.records[i].centerU);
    CEndian::Swap(symbolTexturedCircleDefinition.records[i].centerV);
    CEndian::Swap(symbolTexturedCircleDefinition.records[i].radius);
    CEndian::Swap(symbolTexturedCircleDefinition.records[i].innerRadius);
    CEndian::Swap(symbolTexturedCircleDefinition.records[i].startAngle);
    CEndian::Swap(symbolTexturedCircleDefinition.records[i].endAngle);
  }
}

void CHostSessionV4::SendSymbolTexturedCircleDefinition(const SSymbolTexturedCircle& symbolTexturedCircle)
{
  try
  {
    CIGI::V40::SymbolTexturedCircleDefinition texturedCircleDef;
    texturedCircleDef.symbolId = symbolTexturedCircle.symbolID.Value();
    texturedCircleDef.textureFilterMode = ConvertToTextureFilterMode(symbolTexturedCircle.eTextureFilter);
    texturedCircleDef.textureId = symbolTexturedCircle.textureID.Value();
    texturedCircleDef.textureRepeatOrClamp = ConvertToTexturedCircleTextureWrapMode(symbolTexturedCircle.eTextureWrap);

    CIGI::VariableRecordHelper<CIGI::V40::SymbolTexturedCircleDefinition> helper(texturedCircleDef);

    for (const auto& c : symbolTexturedCircle.circles)
    {
      CIGI::V40::SymbolTexturedCircleDefinition::TexturedCircle texturedCircle;
      texturedCircle.centerU = c.centerUV[0];
      texturedCircle.centerV = c.centerUV[1];
      texturedCircle.radius = c.fRadius;
      texturedCircle.innerRadius = c.fInnerRadius;
      texturedCircle.startAngle = c.startAngle.Value();
      texturedCircle.endAngle = c.endAngle.Value();
      texturedCircle.textureCoordinateSAtCenterPoint = c.centerTextureST[0];
      texturedCircle.textureCoordinateTAtCenterPoint = c.centerTextureST[1];
      texturedCircle.textureMappingRadius = c.fTextureMapRadius;
      texturedCircle.textureMappingRotation = c.fTextureMapRotation;
      helper.push(texturedCircle);
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_TEXTURED_CIRICLE_DEFINITION) << "\n"
                        << "Symbol ID: " << std::to_string(symbolTexturedCircle.symbolID.Value()) << "\n"
                        << "Texture ID: " << std::to_string(symbolTexturedCircle.textureID.Value()) << "\n";

      if (symbolTexturedCircle.eTextureFilter == ETextureFilterMode::LINEAR)
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Linear"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Nearest"
                          << "\n";
      }

      if (symbolTexturedCircle.eTextureWrap == ETextureWrapMode::CLAMP)
      {
        args.sDataMessage << "Texture Wrap Mode: "
                          << "Clamp"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Texture Wrap Mode: "
                          << "Repeat"
                          << "\n";
      }

      args.sDataMessage << "Circles: "
                        << "\n";

      for (const auto& c : symbolTexturedCircle.circles)
      {
        args.sDataMessage << "- "
                          << "\n"
                          << "Center U: " << std::to_string(c.centerUV[0]) << "\n"
                          << "Center V: " << std::to_string(c.centerUV[1]) << "\n"
                          << "Radius: " << std::to_string(c.fRadius) << "\n"
                          << "Inner Radius: " << std::to_string(c.fInnerRadius) << "\n"
                          << "Start Angle: " << c.startAngle << "\n"
                          << "End Angle: " << c.endAngle << "\n"
                          << "Texture Coordinate S at Center Point: " << std::to_string(c.centerTextureST[0]) << "\n"
                          << "Texture Coordinate T at Center Point: " << std::to_string(c.centerTextureST[1]) << "\n"
                          << "Texture-mapping Radius: " << std::to_string(c.fTextureMapRadius) << "\n"
                          << "Texture-mapping Rotation: " << std::to_string(c.fTextureMapRotation) << "\n";
      }
      Event::Raise<HostCigiEvent>(args);
    }

    int numRecords = CIGI::V40::SymbolTexturedCircleDefinition::RecordHelper::numRecords(texturedCircleDef);
    if (m_bByteSwap)
    {
      Swap(texturedCircleDef, numRecords);
    }

    std::unique_ptr<TBuffer<char>> pPacketBuffer = std::make_unique<TBuffer<char>>(static_cast<int>(CIGI::V40::SymbolTexturedCircleDefinition::kBasePacketSize + numRecords * sizeof(CIGI::V40::SymbolTexturedCircleDefinition::Record)));
    memcpy(pPacketBuffer->GetBuffer(), &texturedCircleDef, CIGI::V40::SymbolTexturedCircleDefinition::kBasePacketSize);
    memcpy(pPacketBuffer->GetBuffer() + CIGI::V40::SymbolTexturedCircleDefinition::kBasePacketSize, texturedCircleDef.records, numRecords * sizeof(CIGI::V40::SymbolTexturedCircleDefinition::Record));
    Pack(pPacketBuffer->GetBuffer(), pPacketBuffer->GetSize());
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendSymbolTexturedPolygonDefinition(const SSymbolTexturedPolygon& symbolTexturedPolygon)
{
  try
  {
    HostCigiMessageEventArgs eventArgs;
    eventArgs.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_TEXTURED_POLYGON_DEFINITION);
    Event::Raise<HostCigiEvent>(eventArgs);

    CIGI::V40::SymbolTexturedPolygonDefinition symTextPolygon;
    symTextPolygon.symbolId = symbolTexturedPolygon.symbolID.Value();
    symTextPolygon.textureId = symbolTexturedPolygon.textureID.Value();

    if (symbolTexturedPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE)
    {
      symTextPolygon.primitiveType = CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType::ePrimitiveType_Triangle;
    }
    else if (symbolTexturedPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_STRIP)
    {
      symTextPolygon.primitiveType = CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleStrip;
    }
    else if (symbolTexturedPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_FAN)
    {
      symTextPolygon.primitiveType = CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleFan;
    }

    CIGI::VariableRecordHelper<CIGI::V40::SymbolTexturedPolygonDefinition> helper(symTextPolygon);

    for (const auto& v : symbolTexturedPolygon.vertices)
    {
      CIGI::V40::SymbolTexturedPolygonDefinition::TexturedVertex vertex;

      vertex.vertexU = v.uv[0];
      vertex.vertexV = v.uv[1];
      vertex.textureCoordinateS = v.textureCoordinateST[0];
      vertex.textureCoordinateT = v.textureCoordinateST[1];

      helper.push(vertex);
    }

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::SYMBOL_TEXTURED_CIRICLE_DEFINITION) << "\n"
                        << "Symbol ID: " << std::to_string(symbolTexturedPolygon.symbolID.Value()) << "\n"
                        << "Texture ID: " << std::to_string(symbolTexturedPolygon.textureID.Value()) << "\n";

      if (symbolTexturedPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Triangle"
                          << "\n";
      }
      else if (symbolTexturedPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_STRIP)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Triangle Strip"
                          << "\n";
      }
      else if (symbolTexturedPolygon.ePrimitiveType == EPrimitiveType::TRIANGLE_FAN)
      {
        args.sDataMessage << "Primitive Type: "
                          << "Triangle Fan"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Not Valid"
                          << "\n";
      }

      if (symbolTexturedPolygon.eTextureFilterMode == ETextureFilterMode::LINEAR)
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Linear"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Nearest"
                          << "\n";
      }

      if (symbolTexturedPolygon.eTextureWrapMode == ETextureWrapMode::CLAMP)
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Clamp"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Texture Filter Mode: "
                          << "Repeat"
                          << "\n";
      }

      args.sDataMessage << "Vertices: "
                        << "\n";

      for (const auto& v : symbolTexturedPolygon.vertices)
      {
        args.sDataMessage << "- "
                          << "\n"
                          << "Vertex U: " << std::to_string(v.uv[0]) << "\n"
                          << "Vertex V: " << std::to_string(v.uv[1]) << "\n"
                          << "Texture Coordinate S: " << std::to_string(v.textureCoordinateST[0]) << "\n"
                          << "Texture Coordinate T: " << std::to_string(v.textureCoordinateST[1]) << "\n";
      }
      Event::Raise<HostCigiEvent>(args);
    }

    symTextPolygon.textureFilterMode = ConvertToTextureFilter(symbolTexturedPolygon.eTextureFilterMode);
    symTextPolygon.textureRepeatOrClamp = ConvertToTexturedPolygonTextureWrapMode(symbolTexturedPolygon.eTextureWrapMode);

    int numRecords = CIGI::V40::SymbolTexturedPolygonDefinition::RecordHelper::numRecords(symTextPolygon);
    CIGI::V40::SymbolTexturedPolygonDefinition tempSymTextPolygon = symTextPolygon;
    if (m_bByteSwap)
    {
      ByteSwapPacketV4(tempSymTextPolygon);
      for (int i = 0; i < numRecords; ++i)
      {
        CEndian::Swap(tempSymTextPolygon.records[i].vertexU);
        CEndian::Swap(tempSymTextPolygon.records[i].vertexV);
        CEndian::Swap(tempSymTextPolygon.records[i].textureCoordinateS);
        CEndian::Swap(tempSymTextPolygon.records[i].textureCoordinateT);
      }
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], &tempSymTextPolygon, CIGI::V40::SymbolTexturedPolygonDefinition::kBasePacketSize);
    m_nSendBufferLength += CIGI::V40::SymbolTexturedPolygonDefinition::kBasePacketSize;
    memcpy(&m_sendBuffer[m_nSendBufferLength], tempSymTextPolygon.records, numRecords * sizeof(CIGI::V40::SymbolTexturedPolygonDefinition::Record));
    m_nSendBufferLength += numRecords * sizeof(CIGI::V40::SymbolTexturedPolygonDefinition::Record);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEntityTerrestrialSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl)
{
  try
  {
    HostCigiMessageEventArgs args;
    args.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL);
    Event::Raise<HostCigiEvent>(args);

    CIGI::V40::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceCtrl;
    terrestrialSurfaceCtrl.scope = CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Entity;
    terrestrialSurfaceCtrl.entityRegionId = entityID.Value();

    terrestrialSurfaceCtrl.surfaceConditionEnable = terrestrialSurfaceControl.bEnabled;
    terrestrialSurfaceCtrl.severity = ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value();
    terrestrialSurfaceCtrl.coverage = ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value();
    terrestrialSurfaceCtrl.surfaceConditionId = terrestrialSurfaceControl.surfaceConditionID.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs hostcigiDataMessageEventArgs;
      hostcigiDataMessageEventArgs.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL) << "\n"
                                                << "Entity ID: " << std::to_string(entityID.Value()) << "\n"
                                                << "Surface Conditions Enable: " << EnableToString(terrestrialSurfaceControl.bEnabled) << "\n"
                                                << "Scope: Entity" << "\n"
                                                << "Severity: " << std::to_string(ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value()) << "\n"
                                                << "Coverage: " << std::to_string(ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value()) << "\n"
                                                << "Surface Condition ID: " << std::to_string(terrestrialSurfaceControl.surfaceConditionID.Value());
      Event::Raise<HostCigiEvent>(hostcigiDataMessageEventArgs);
    }

    PackPacketV4(this, terrestrialSurfaceCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendRegionTerrestrialSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl)
{
  try
  {
    HostCigiMessageEventArgs args;
    args.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL);
    Event::Raise<HostCigiEvent>(args);

    CIGI::V40::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceCtrl;
    terrestrialSurfaceCtrl.scope = CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Regional;
    terrestrialSurfaceCtrl.entityRegionId = regionID.Value();

    terrestrialSurfaceCtrl.surfaceConditionEnable = terrestrialSurfaceControl.bEnabled;
    terrestrialSurfaceCtrl.severity = ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value();
    terrestrialSurfaceCtrl.coverage = ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value();
    terrestrialSurfaceCtrl.surfaceConditionId = terrestrialSurfaceControl.surfaceConditionID.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args2;
      args2.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL) << "\n"
                         << "Region ID: " << std::to_string(regionID.Value()) << "\n"
                         << "Surface Conditions Enable: " << EnableToString(terrestrialSurfaceControl.bEnabled) << "\n"
                         << "Scope: Region\n"
                         << "Severity: " << std::to_string(ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value()) << "\n"
                         << "Coverage: " << std::to_string(ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value()) << "\n"
                         << "Surface Condition ID: " << std::to_string(terrestrialSurfaceControl.surfaceConditionID.Value());
      Event::Raise<HostCigiEvent>(args2);
    }

    PackPacketV4(this, terrestrialSurfaceCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendGlobalTerrestrialSurfaceConditionsControl(const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl)
{
  try
  {
    HostCigiMessageEventArgs eventArgs;
    eventArgs.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL);
    Event::Raise<HostCigiEvent>(eventArgs);

    CIGI::V40::TerrestrialSurfaceConditionsCtrl terrestrialSurfaceCtrl;
    terrestrialSurfaceCtrl.scope = CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Global;

    terrestrialSurfaceCtrl.surfaceConditionEnable = terrestrialSurfaceControl.bEnabled;
    terrestrialSurfaceCtrl.severity = ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value();
    terrestrialSurfaceCtrl.coverage = ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value();
    terrestrialSurfaceCtrl.surfaceConditionId = terrestrialSurfaceControl.surfaceConditionID.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL) << "\n"
                        << "Surface Conditions Enable: " << EnableToString(terrestrialSurfaceControl.bEnabled) << "\n"
                        << "Scope: Global\n"
                        << "Severity: " << std::to_string(ConvertToCigiTerrestrailSurfaceConditionsSeverity(terrestrialSurfaceControl.severity).Value()) << "\n"
                        << "Coverage: " << std::to_string(ConvertToCigiIntPercentage(terrestrialSurfaceControl.coverage).Value()) << "\n"
                        << "Surface Condition ID: " << std::to_string(terrestrialSurfaceControl.surfaceConditionID.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, terrestrialSurfaceCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

CIGI::V40::VelocityCtrl::CoordinateSystem ConvertToVelocityCtrlCoordinateSystem(sbio::EObjectCoordinateSystem coordSys)
{
  if (coordSys == sbio::EObjectCoordinateSystem::LOCAL)
  {
    return CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_Local;
  }
  else if (coordSys == sbio::EObjectCoordinateSystem::PARENT)
  {
    return CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
  }
  else if (coordSys == sbio::EObjectCoordinateSystem::WORLD)
  {
    return CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
  }
  return CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
}

void CHostSessionV4::SendVelocityControl(const SCigiEntityVelocityControl& entityVelocityControl)
{
  try
  {
    CIGI::V40::VelocityCtrl velocityControl;
    velocityControl.entityId = entityVelocityControl.entityID.Value();
    velocityControl.applyToArticulatedPart = false;
    velocityControl.articulatedPartId = 0;
    velocityControl.coordinateSystem = ConvertToVelocityCtrlCoordinateSystem(entityVelocityControl.coordinateSystem);
    velocityControl.rollAngularVelocity = entityVelocityControl.angularVelocity.roll.Value();
    velocityControl.pitchAngularVelocity = entityVelocityControl.angularVelocity.pitch.Value();
    velocityControl.yawAngularVelocity = entityVelocityControl.angularVelocity.yaw.Value();
    velocityControl.xLinearVelocity = static_cast<float>(entityVelocityControl.linearVelocity[0]);
    velocityControl.yLinearVelocity = static_cast<float>(entityVelocityControl.linearVelocity[1]);
    velocityControl.zLinearVelocity = static_cast<float>(entityVelocityControl.linearVelocity[2]);

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::VELOCITY_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(entityVelocityControl.entityID.Value()) << "\n"
                        << "Apply to Articulated Part: "
                        << "False"
                        << "\n";

      if (entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::PARENT)
      {
        velocityControl.coordinateSystem = CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
        args.sDataMessage << "Coordinate System: " << ConvertCigiObCoordSysToString(EObjectCoordinateSystem::PARENT) << "\n";
      }
      else if (entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::LOCAL)
      {
        velocityControl.coordinateSystem = CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_Local;
        args.sDataMessage << "Coordinate System: " << ConvertCigiObCoordSysToString(EObjectCoordinateSystem::LOCAL) << "\n";
      }
      else
      {
        velocityControl.coordinateSystem = CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent;
        args.sDataMessage << "Coordinate System: " << ConvertCigiObCoordSysToString(EObjectCoordinateSystem::UNKNOWN) << "\n";
      }

      args.sDataMessage << "X Linear Velocity: " << std::to_string(static_cast<float>(entityVelocityControl.linearVelocity[0])) << "\n"
                        << "Y Linear Velocity: " << std::to_string(static_cast<float>(entityVelocityControl.linearVelocity[1])) << "\n"
                        << "Z Linear Velocity: " << std::to_string(static_cast<float>(entityVelocityControl.linearVelocity[2])) << "\n"
                        << "Roll Angular Velocity: " << std::to_string(entityVelocityControl.angularVelocity.roll.Value()) << "\n"
                        << "Pitch Angular Velocity: " << std::to_string(entityVelocityControl.angularVelocity.pitch.Value()) << "\n"
                        << "Yaw Angular Velocity: " << std::to_string(entityVelocityControl.angularVelocity.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, velocityControl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendVelocityControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& articulatedPartVelocityControl)
{
  try
  {
    CIGI::V40::VelocityCtrl velocityControl;
    velocityControl.entityId = articulatedPartVelocityControl.entityID.Value();
    velocityControl.articulatedPartId = articulatedPartVelocityControl.articulatedPartID.Value();
    velocityControl.applyToArticulatedPart = true;
    // Coordinate System shall be ignored if Apply to Articulated Part is set to True
    velocityControl.coordinateSystem = CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_Local;
    velocityControl.xLinearVelocity = static_cast<float>(articulatedPartVelocityControl.linearVelocity[0]);
    velocityControl.yLinearVelocity = static_cast<float>(articulatedPartVelocityControl.linearVelocity[1]);
    velocityControl.zLinearVelocity = static_cast<float>(articulatedPartVelocityControl.linearVelocity[2]);
    velocityControl.rollAngularVelocity = articulatedPartVelocityControl.angularVelocity.roll.Value();
    velocityControl.pitchAngularVelocity = articulatedPartVelocityControl.angularVelocity.pitch.Value();
    velocityControl.yawAngularVelocity = articulatedPartVelocityControl.angularVelocity.yaw.Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::VELOCITY_CONTROL) << "\n"
                        << "Entity ID: " << std::to_string(articulatedPartVelocityControl.entityID.Value()) << "\n"
                        << "Articulated Part ID: " << std::to_string(articulatedPartVelocityControl.articulatedPartID.Value()) << "\n"
                        << "Apply to Articulated Part: "
                        << "True"
                        << "\n"
                        << "X Linear Velocity: " << std::to_string(static_cast<float>(articulatedPartVelocityControl.linearVelocity[0])) << "\n"
                        << "Y Linear Velocity: " << std::to_string(static_cast<float>(articulatedPartVelocityControl.linearVelocity[1])) << "\n"
                        << "Z Linear Velocity: " << std::to_string(static_cast<float>(articulatedPartVelocityControl.linearVelocity[2])) << "\n"
                        << "Roll Angular Velocity: " << std::to_string(articulatedPartVelocityControl.angularVelocity.roll.Value()) << "\n"
                        << "Pitch Angular Velocity: " << std::to_string(articulatedPartVelocityControl.angularVelocity.pitch.Value()) << "\n"
                        << "Yaw Angular Velocity: " << std::to_string(articulatedPartVelocityControl.angularVelocity.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, velocityControl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendViewControl(const sbio::cigi::SCigiViewControl& viewControl)
{
  try
  {
    CIGI::V40::ViewCtrl viewCtrl;
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

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::VIEW_CONTROL) << "\n"
                        << "Group ID: " << std::to_string(viewControl.viewGroupID.Value()) << "\n"
                        << "X Offset Enable: " << EnableToString(viewControl.offsetEnabled[0]) << "\n"
                        << "Y Offset Enable: " << EnableToString(viewControl.offsetEnabled[1]) << "\n"
                        << "Z Offset Enable: " << EnableToString(viewControl.offsetEnabled[2]) << "\n"
                        << "Roll Enable: " << EnableToString(viewControl.bRollEnabled) << "\n"
                        << "Pitch Enable: " << EnableToString(viewControl.bPitchEnabled) << "\n"
                        << "Yaw Enable: " << EnableToString(viewControl.bYawEnabled) << "\n"
                        << "View ID: " << std::to_string(viewControl.viewID.Value()) << "\n"
                        << "Entity ID: " << std::to_string(viewControl.entityID.Value()) << "\n"
                        << "X Offset: " << std::to_string(viewControl.offset[0]) << "\n"
                        << "Y Offset: " << std::to_string(viewControl.offset[1]) << "\n"
                        << "Z Offset: " << std::to_string(viewControl.offset[2]) << "\n"
                        << "Roll: " << std::to_string(viewControl.rotation.roll.Value()) << "\n"
                        << "Pitch: " << std::to_string(viewControl.rotation.pitch.Value()) << "\n"
                        << "Yaw: " << std::to_string(viewControl.rotation.yaw.Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, viewCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendViewDefinition(const SCigiViewDefinition& viewDefinition)
{
  try
  {
    CIGI::V40::ViewDefinition viewDef;
    viewDef.viewId = viewDefinition.viewID.Value();
    viewDef.groupId = viewDefinition.viewGroupID.Value();
    viewDef.nearEnable = viewDefinition.bNearEnabled;
    viewDef.farEnable = viewDefinition.bFarEnabled;
    viewDef.leftEnable = viewDefinition.bLeftEnabled;
    viewDef.rightEnable = viewDefinition.bRightEnabled;
    viewDef.topEnable = viewDefinition.bTopEnabled;
    viewDef.bottomEnable = viewDefinition.bBottomEnabled;
    viewDef.mirrorMode = ConvertToMirrorModeGrp(viewDefinition.eMirrorMode);
    viewDef.pixelReplicationMode = static_cast<CIGI::V40::ViewDefinition::PixelReplicationMode>(viewDefinition.pixelReplicationMode.Value());

    if (viewDefinition.eProjectionMode == EProjectionMode::ORTHOGRAPHIC)
    {
      viewDef.projectionType = CIGI::V40::ViewDefinition::ProjectionType::eProjectionType_Orthographic;
    }
    else if (viewDefinition.eProjectionMode == EProjectionMode::PERSPECTIVE)
    {
      viewDef.projectionType = CIGI::V40::ViewDefinition::ProjectionType::eProjectionType_Perspective;
    }

    viewDef.reorder = viewDefinition.bReorder;
    viewDef.viewType = viewDefinition.viewType.Value();
    viewDef.nearClip = viewDefinition.fNear;
    viewDef.farClip = viewDefinition.fFar;
    viewDef.left = viewDefinition.fLeft;
    viewDef.right = viewDefinition.fRight;
    viewDef.top = viewDefinition.fTop;
    viewDef.bottom = viewDefinition.fBottom;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::VIEW_DEFINITION) << "\n"
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
        args.sDataMessage << "Projection Type: "
                          << "Orthographic Parallel"
                          << "\n";
      }
      else if (viewDefinition.eProjectionMode == EProjectionMode::PERSPECTIVE)
      {
        args.sDataMessage << "Projection Type: "
                          << "Perspective"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Projection Type: "
                          << "Unknown"
                          << "\n";
      }

      if (viewDefinition.bReorder)
      {
        args.sDataMessage << "Reorder: "
                          << "Bring to Top"
                          << "\n";
      }
      else
      {
        args.sDataMessage << "Reorder: "
                          << "No Reorder"
                          << "\n";
      }

      args.sDataMessage << "View Type: " << std::to_string(viewDefinition.viewType.Value()) << "\n"
                        << "Near: " << std::to_string(viewDefinition.fNear) << "\n"
                        << "Far: " << std::to_string(viewDefinition.fFar) << "\n"
                        << "Left: " << std::to_string(viewDefinition.fLeft) << "\n"
                        << "Right: " << std::to_string(viewDefinition.fRight) << "\n"
                        << "Top: " << std::to_string(viewDefinition.fTop) << "\n"
                        << "Bottom: " << std::to_string(viewDefinition.fBottom);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, viewDef, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendEntityWaveControl(EntityID entityID, const SCigiWaveCondition& waveCondition)
{
  try
  {
    HostCigiMessageEventArgs args;
    args.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::WAVE_CONTROL);
    Event::Raise<HostCigiEvent>(args);

    CIGI::V40::WaveCtrl waveCtrl;

    waveCtrl.waveId = waveCondition.waveID;
    waveCtrl.waveEnable = waveCondition.bWaveEnabled;
    waveCtrl.scope = CIGI::V40::WaveCtrl::Scope::eScope_Entity;
    waveCtrl.entityRegionId = entityID.Value();
    waveCtrl.breakerType = ConvertToBreakerGrp(waveCondition.eBreakerType);
    waveCtrl.waveHeight = waveCondition.fWaveHeight;
    waveCtrl.wavelength = waveCondition.fWavelength;
    waveCtrl.period = waveCondition.fPeriod;
    waveCtrl.direction = static_cast<float>(waveCondition.direction.Value());
    waveCtrl.phaseOffset = waveCondition.phaseOffset.Value();
    waveCtrl.leading = waveCondition.leading.Value();

    PackPacketV4(this, waveCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendRegionalWaveControl(RegionID regionID, const SCigiWaveCondition& waveCondition)
{
  try
  {
    HostCigiMessageEventArgs args;
    args.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::WAVE_CONTROL);
    Event::Raise<HostCigiEvent>(args);

    CIGI::V40::WaveCtrl waveCtrl;

    waveCtrl.waveId = waveCondition.waveID;
    waveCtrl.waveEnable = waveCondition.bWaveEnabled;
    waveCtrl.scope = CIGI::V40::WaveCtrl::Scope::eScope_Regional;
    waveCtrl.entityRegionId = regionID.Value();
    waveCtrl.breakerType = ConvertToBreakerGrp(waveCondition.eBreakerType);
    waveCtrl.waveHeight = waveCondition.fWaveHeight;
    waveCtrl.wavelength = waveCondition.fWavelength;
    waveCtrl.period = waveCondition.fPeriod;
    waveCtrl.direction = static_cast<float>(waveCondition.direction.Value());
    waveCtrl.phaseOffset = waveCondition.phaseOffset.Value();
    waveCtrl.leading = waveCondition.leading.Value();

    PackPacketV4(this, waveCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendGlobalWaveControl(const SCigiWaveCondition& waveCondition)
{
  try
  {
    HostCigiMessageEventArgs args;
    args.sMessage = "Sending " + GetCigiOpCodeName(ECigiOpCodeV4::WAVE_CONTROL);
    Event::Raise<HostCigiEvent>(args);

    CIGI::V40::WaveCtrl waveCtrl;

    waveCtrl.waveId = waveCondition.waveID;
    waveCtrl.waveEnable = waveCondition.bWaveEnabled;
    waveCtrl.scope = CIGI::V40::WaveCtrl::Scope::eScope_Global;
    waveCtrl.entityRegionId = 0;
    waveCtrl.breakerType = ConvertToBreakerGrp(waveCondition.eBreakerType);
    waveCtrl.waveHeight = waveCondition.fWaveHeight;
    waveCtrl.wavelength = waveCondition.fWavelength;
    waveCtrl.period = waveCondition.fPeriod;
    waveCtrl.direction = static_cast<float>(waveCondition.direction.Value());
    waveCtrl.phaseOffset = waveCondition.phaseOffset.Value();
    waveCtrl.leading = waveCondition.leading.Value();

    PackPacketV4(this, waveCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendWeatherControl(sbio::GlobalLayeredWeatherID globalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  try
  {
    CIGI::V40::WeatherCtrl weatherCtrl;
    weatherCtrl.entityRegionId = 0;
    weatherCtrl.layerId = globalLayerWeatherID.Value();
    weatherCtrl.humidity = ConvertToCigiIntPercentage(weatherCondition.humidity).Value();
    weatherCtrl.weatherEnable = weatherCondition.bWeatherEnabled;
    weatherCtrl.randomWindsEnable = weatherCondition.bRandomWindsEnabled;
    weatherCtrl.randomLightningEnable = weatherCondition.bRandomLightningEnabled;
    weatherCtrl.cloudType = static_cast<CIGI::V40::WeatherCtrl::CloudType>(weatherCondition.cloudType.Value());
    weatherCtrl.scope = CIGI::V40::WeatherCtrl::Scope::eScope_Global;
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

    weatherCtrl.topScudEnable = weatherCondition.bTopScudEnabled;
    weatherCtrl.bottomScudEnable = weatherCondition.bBottomScudEnabled;
    weatherCtrl.bottomScudFrequency = ConvertToCigiFloatPercentage(weatherCondition.bottomScudFrequency).Value();
    weatherCtrl.topScudFrequency = ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value();
    weatherCtrl.bottomTransitionBandThickness = spatialWeatherCondition.fBottomTransitionBandThickness;
    weatherCtrl.topTransitionBandThickness = spatialWeatherCondition.fTopTransitionBandThickness;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::WEATHER_CONTROL) << "\n"
                        << "Layer ID: " << g_pScriptRuntime->GetLayerIDString(globalLayerWeatherID.Value()) << "\n"
                        << "Humidity: " << std::to_string(ConvertToCigiIntPercentage(weatherCondition.humidity).Value()) << "%"
                        << "\n"
                        << "Weather Enable: " << EnableToString(weatherCondition.bWeatherEnabled) << "\n"
                        << "Bottom Scud Enable: " << EnableToString(weatherCondition.bBottomScudEnabled) << "\n"
                        << "Random Winds Enable: " << EnableToString(weatherCondition.bRandomWindsEnabled) << "\n"
                        << "Random Lightning Enable: " << EnableToString(weatherCondition.bRandomLightningEnabled) << "\n"
                        << "Cloud Type: " << g_pScriptRuntime->GetCloudTypeString(weatherCondition.cloudType) << "\n"
                        << "Scope: "
                        << "Global"
                        << "\n"
                        << "Severity: " << std::to_string(weatherCondition.severity.Value()) << "\n"
                        << "Top Scud Enable: " << EnableToString(weatherCondition.bTopScudEnabled) << "\n"
                        << "Air Temperature: " << std::to_string(weatherCondition.fAirTemperature.Value()) << "�C"
                        << "\n"
                        << "Visibility Range: " << std::to_string(weatherCondition.fVisibilityRange) << "\n"
                        << "Bottom Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.bottomScudFrequency).Value()) << "\n"
                        << "Coverage: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.coverage).Value()) << "\n"
                        << "Base Elevation: " << std::to_string(spatialWeatherCondition.fBaseElevation) << "\n"
                        << "Thickness: " << std::to_string(spatialWeatherCondition.fThickness) << "\n"
                        << "Bottom Transition Band Thickness: " << std::to_string(spatialWeatherCondition.fBottomTransitionBandThickness) << "\n"
                        << "Horizontal Wind Speed: " << std::to_string(weatherCondition.HorizontalWindSpeed) << "\n"
                        << "Vertical Wind Speed: " << std::to_string(weatherCondition.VerticalWindSpeed) << "\n"
                        << "Wind Direction: " << std::to_string(weatherCondition.WindDirection) << "\n"
                        << "Barometric Pressure: " << std::to_string(weatherCondition.fBarometricPressure) << "\n"
                        << "Aerosol Concentration: " << std::to_string(weatherCondition.fAerosolConcentration) << "\n"
                        << "Top Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value()) << "\n"
                        << "Top Transition Band Thickness: " << std::to_string(spatialWeatherCondition.fTopTransitionBandThickness);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, weatherCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendWeatherControl(RegionID regionID, sbio::RegionalLayeredWeatherID regionalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition)
{
  try
  {
    CIGI::V40::WeatherCtrl weatherCtrl;
    weatherCtrl.entityRegionId = regionID.Value();
    weatherCtrl.layerId = regionalLayerWeatherID.Value();
    weatherCtrl.humidity = ConvertToCigiIntPercentage(weatherCondition.humidity).Value();
    weatherCtrl.weatherEnable = weatherCondition.bWeatherEnabled;
    weatherCtrl.randomWindsEnable = weatherCondition.bRandomWindsEnabled;
    weatherCtrl.randomLightningEnable = weatherCondition.bRandomLightningEnabled;
    weatherCtrl.cloudType = static_cast<CIGI::V40::WeatherCtrl::CloudType>(weatherCondition.cloudType.Value());
    weatherCtrl.scope = CIGI::V40::WeatherCtrl::Scope::eScope_Regional;
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

    weatherCtrl.topScudEnable = weatherCondition.bTopScudEnabled;
    weatherCtrl.bottomScudEnable = weatherCondition.bBottomScudEnabled;
    weatherCtrl.bottomScudFrequency = ConvertToCigiFloatPercentage(weatherCondition.bottomScudFrequency).Value();
    weatherCtrl.topScudFrequency = ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value();
    weatherCtrl.bottomTransitionBandThickness = spatialWeatherCondition.fBottomTransitionBandThickness;
    weatherCtrl.topTransitionBandThickness = spatialWeatherCondition.fTopTransitionBandThickness;

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::WEATHER_CONTROL) << "\n"
                        << "Layer ID: " << g_pScriptRuntime->GetLayerIDString(regionalLayerWeatherID.Value()) << "\n"
                        << "Humidity: " << std::to_string(ConvertToCigiIntPercentage(weatherCondition.humidity).Value()) << "%"
                        << "\n"
                        << "Weather Enable: " << EnableToString(weatherCondition.bWeatherEnabled) << "\n"
                        << "Bottom Scud Enable: " << EnableToString(weatherCondition.bBottomScudEnabled) << "\n"
                        << "Random Winds Enable: " << EnableToString(weatherCondition.bRandomWindsEnabled) << "\n"
                        << "Random Lightning Enable: " << EnableToString(weatherCondition.bRandomLightningEnabled) << "\n"
                        << "Cloud Type: " << g_pScriptRuntime->GetCloudTypeString(weatherCondition.cloudType) << "\n"
                        << "Scope: "
                        << "Regional"
                        << "\n"
                        << "Severity: " << std::to_string(weatherCondition.severity.Value()) << "\n"
                        << "Top Scud Enable: " << EnableToString(weatherCondition.bTopScudEnabled) << "\n"
                        << "Air Temperature: " << std::to_string(weatherCondition.fAirTemperature.Value()) << "\n"
                        << "Visibility Range: " << std::to_string(weatherCondition.fVisibilityRange) << "\n"
                        << "Bottom Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.bottomScudFrequency).Value()) << "\n"
                        << "Coverage: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.coverage).Value()) << "\n"
                        << "Base Elevation: " << std::to_string(spatialWeatherCondition.fBaseElevation) << "\n"
                        << "Thickness: " << std::to_string(spatialWeatherCondition.fThickness) << "\n"
                        << "Bottom Transition Band Thickness: " << std::to_string(spatialWeatherCondition.fBottomTransitionBandThickness) << "\n"
                        << "Horizontal Wind Speed: " << std::to_string(weatherCondition.HorizontalWindSpeed) << "\n"
                        << "Vertical Wind Speed: " << std::to_string(weatherCondition.VerticalWindSpeed) << "\n"
                        << "Wind Direction: " << std::to_string(weatherCondition.WindDirection) << "\n"
                        << "Barometric Pressure: " << std::to_string(weatherCondition.fBarometricPressure) << "\n"
                        << "Aerosol Concentration: " << std::to_string(weatherCondition.fAerosolConcentration) << "\n"
                        << "Top Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value()) << "\n"
                        << "Top Transition Band Thickness: " << std::to_string(spatialWeatherCondition.fTopTransitionBandThickness);
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, weatherCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CHostSessionV4::SendWeatherControl(sbio::EntityID entityID, const sbio::cigi::SCigiWeatherCondition& weatherCondition)
{
  try
  {
    CIGI::V40::WeatherCtrl weatherCtrl;
    weatherCtrl.entityRegionId = entityID.Value();
    weatherCtrl.layerId = 0;
    weatherCtrl.humidity = ConvertToCigiIntPercentage(weatherCondition.humidity).Value();
    weatherCtrl.weatherEnable = weatherCondition.bWeatherEnabled;
    weatherCtrl.randomWindsEnable = weatherCondition.bRandomWindsEnabled;
    weatherCtrl.randomLightningEnable = weatherCondition.bRandomLightningEnabled;
    weatherCtrl.cloudType = static_cast<CIGI::V40::WeatherCtrl::CloudType>(weatherCondition.cloudType.Value());
    weatherCtrl.scope = CIGI::V40::WeatherCtrl::Scope::eScope_Entity;
    weatherCtrl.severity = weatherCondition.severity.Value();
    weatherCtrl.airTemperature = weatherCondition.fAirTemperature.Value();
    weatherCtrl.visibilityRange = weatherCondition.fVisibilityRange;
    weatherCtrl.coverage = ConvertToCigiFloatPercentage(weatherCondition.coverage).Value();
    weatherCtrl.horizontalWindSpeed = weatherCondition.HorizontalWindSpeed;
    weatherCtrl.verticalWindSpeed = weatherCondition.VerticalWindSpeed;
    weatherCtrl.windDirection = weatherCondition.WindDirection;
    weatherCtrl.barometricPressure = weatherCondition.fBarometricPressure;
    weatherCtrl.aerosolConcentration = weatherCondition.fAerosolConcentration;

    weatherCtrl.topScudEnable = weatherCondition.bTopScudEnabled;
    weatherCtrl.bottomScudEnable = weatherCondition.bBottomScudEnabled;
    weatherCtrl.bottomScudFrequency = ConvertToCigiFloatPercentage(weatherCondition.bottomScudFrequency).Value();
    weatherCtrl.topScudFrequency = ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value();

    if (GetLoggingEnabled())
    {
      HostCigiDataMessageEventArgs args;
      args.sDataMessage << "Sending: " << GetCigiOpCodeName(ECigiOpCodeV4::WEATHER_CONTROL) << "\n"
                        << "Humidity: " << std::to_string(ConvertToCigiIntPercentage(weatherCondition.humidity).Value()) << "%"
                        << "\n"
                        << "Weather Enable: " << EnableToString(weatherCondition.bWeatherEnabled) << "\n"
                        << "Bottom Scud Enable: " << EnableToString(weatherCondition.bBottomScudEnabled) << "\n"
                        << "Random Winds Enable: " << EnableToString(weatherCondition.bRandomWindsEnabled) << "\n"
                        << "Random Lightning Enable: " << EnableToString(weatherCondition.bRandomLightningEnabled) << "\n"
                        << "Cloud Type: " << g_pScriptRuntime->GetCloudTypeString(weatherCondition.cloudType) << "\n"
                        << "Scope: "
                        << "Entity"
                        << "\n"
                        << "Severity: " << std::to_string(weatherCondition.severity.Value()) << "\n"
                        << "Top Scud Enable: " << EnableToString(weatherCondition.bTopScudEnabled) << "\n"
                        << "Top Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value()) << "\n"
                        << "Region ID: " << std::to_string(entityID.Value()) << "\n"
                        << "Air Temperature: " << std::to_string(weatherCondition.fAirTemperature.Value()) << "\n"
                        << "Visibility Range: " << std::to_string(weatherCondition.fVisibilityRange) << "\n"
                        << "Bottom Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.bottomScudFrequency).Value()) << "\n"
                        << "Coverage: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.coverage).Value()) << "\n"
                        << "Horizontal Wind Speed: " << std::to_string(weatherCondition.HorizontalWindSpeed) << "\n"
                        << "Vertical Wind Speed: " << std::to_string(weatherCondition.VerticalWindSpeed) << "\n"
                        << "Wind Direction: " << std::to_string(weatherCondition.WindDirection) << "\n"
                        << "Barometric Pressure: " << std::to_string(weatherCondition.fBarometricPressure) << "\n"
                        << "Aerosol Concentration: " << std::to_string(weatherCondition.fAerosolConcentration) << "\n"
                        << "Top Scud Frequency: " << std::to_string(ConvertToCigiFloatPercentage(weatherCondition.topScudFrequency).Value());
      Event::Raise<HostCigiEvent>(args);
    }

    PackPacketV4(this, weatherCtrl, m_bByteSwap);
  }
  catch (const exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

bool CHostSessionV4::SetIGControl(CigiDatabaseNumber databaseNumber, bool bEntityTypeSubstitutionEnabled, EIGMode eIGMode, bool bSmoothingEnabled)
{
  if (!CHostSession::SetIGControl(databaseNumber, bEntityTypeSubstitutionEnabled, eIGMode, bSmoothingEnabled))
  {
    return false;
  }

  m_IGControl.entityTypeSubstitutionEnable = bEntityTypeSubstitutionEnabled;
  m_IGControl.igMode = ToIGMode(m_DesiredIGMode);
  m_IGControl.smoothingEnable = bSmoothingEnabled;

  return true;
}

void CHostSessionV4::OnHostCigiSensorResponseEvent(const HostCigiSensorResponseEventArgs& args)
{
  if (!GetLoggingEnabled())
  {
    return;
  }

  HostCigiDataMessageEventArgs msgArgs;
  msgArgs.sDataMessage << "Sensor response event:\n"
                       << "Sensor ID: " << std::to_string(args.sensorResponse.sensorID.Value()) << "\n"
                       << "View ID: " << std::to_string(args.sensorResponse.viewID.Value()) << "\n"
                       << "Gate X Size: " << std::to_string(args.sensorResponse.gateSize[0]) << "\n"
                       << "Gate Y Size: " << std::to_string(args.sensorResponse.gateSize[1]) << "\n"
                       << "Gate X Position: " << std::to_string(args.sensorResponse.gatePosition[0]) << "\n"
                       << "Gate Y Position: " << std::to_string(args.sensorResponse.gatePosition[1]) << "\n"
                       << "Host Frame Number: " << std::to_string(args.sensorResponse.hostFrameNumber.Value());

  Event::Raise<HostCigiEvent>(msgArgs);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
