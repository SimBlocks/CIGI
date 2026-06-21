//Copyright SimBlocks LLC 2016-2026
#include "CigiMessageLogger.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::symbol;

namespace
{
  template <typename T>
  std::string ToLogString(const T& value)
  {
    std::ostringstream ss;
    ss << value;
    return ss.str();
  }

  std::string ToLogString(bool value)
  {
    return value ? "true" : "false";
  }

  template <typename T>
  std::string EnumValueToLogString(T value)
  {
    return std::to_string(static_cast<int>(value));
  }

  template <typename T>
  std::string NamedEnumToLogString(const std::string& name, T value)
  {
    if (name.empty())
    {
      return EnumValueToLogString(value);
    }

    return name + " (" + EnumValueToLogString(value) + ")";
  }

  template <typename T>
  std::string OpCodeToLogString(T value)
  {
    return NamedEnumToLogString(GetCigiOpCodeName(value), value);
  }

  std::string ToLogString(const sbio::math::SGeodeticCoordinates& value)
  {
    std::ostringstream ss;
    ss << "latitude = " << value.latitude << ", longitude = " << value.longitude << ", altitude = " << value.altitude;
    return ss.str();
  }

  std::string ToLogString(const sbio::math::Vec2f& value)
  {
    std::ostringstream ss;
    ss << "[" << value[0] << ", " << value[1] << "]";
    return ss.str();
  }

  std::string ToLogString(const sbio::cigi::CigiBodyCoordinates& value)
  {
    std::ostringstream ss;
    ss << "[" << value[0] << ", " << value[1] << ", " << value[2] << "]";
    return ss.str();
  }

  std::string ToLogString(const sbio::SColor32& value)
  {
    std::ostringstream ss;
    ss << "r = " << static_cast<int>(value.r) << ", g = " << static_cast<int>(value.g) << ", b = " << static_cast<int>(value.b) << ", a = " << static_cast<int>(value.a);
    return ss.str();
  }

  std::initializer_list<std::pair<std::string, std::string>> BaseHatHotFields(const sbio::cigi::SBaseHATHOTResponse& message)
  {
    return {{"HATHOTID", ToLogString(message.HATHOTID)}, {"bValid", ToLogString(message.bValid)}, {"hostFrameLSN", std::to_string(message.hostFrameLSN)}};
  }
}

CCigiMessageLogger::CCigiMessageLogger(const std::filesystem::path& applicationDataPath) : m_LogFilePath(applicationDataPath / "IGMessageLog.log")
{
}

const std::filesystem::path& CCigiMessageLogger::GetLogFilePath() const
{
  return m_LogFilePath;
}

void CCigiMessageLogger::ClearLog() const
{
  std::error_code ec;
  std::filesystem::remove(m_LogFilePath, ec);
}

void CCigiMessageLogger::SetFrameNumber(FrameNumber frameNumber)
{
  m_FrameNumber = frameNumber;
}

void CCigiMessageLogger::LogMessageFromHostToIG(ECigiVersion eCigiVersion, const std::string& endpoint, const uint8_t* pBuffer, int nMessageSize) const
{
  LogMessage("Host->IG", eCigiVersion, endpoint, pBuffer, nMessageSize);
}

void CCigiMessageLogger::LogMessageFromIGToHost(ECigiVersion eCigiVersion, const uint8_t* pBuffer, int nMessageSize) const
{
  LogMessage("IG->Host", eCigiVersion, "", pBuffer, nMessageSize);
}

void CCigiMessageLogger::LogMessage(const std::string& direction, ECigiVersion eCigiVersion, const std::string& endpoint, const uint8_t* pBuffer, int nMessageSize) const
{
  if (nMessageSize <= 0)
  {
    return;
  }

  std::ofstream logFile(m_LogFilePath, std::ios::app);
  if (!logFile.is_open())
  {
    return;
  }

  logFile << GetTimestampString() << " " << direction << " frame = " << m_FrameNumber.Value();
  if (!endpoint.empty())
  {
    logFile << " endpoint = " << endpoint;
  }
  logFile << " bytes = " << nMessageSize << std::endl;

  LogPackets(logFile, eCigiVersion, pBuffer, nMessageSize);
  logFile << "  raw = " << ToHexString(pBuffer, nMessageSize) << std::endl;
}

void CCigiMessageLogger::LogExtractedFields(const std::string& messageName, std::initializer_list<std::pair<std::string, std::string>> fields) const
{
  std::ofstream logFile(m_LogFilePath, std::ios::app);
  if (!logFile.is_open())
  {
    return;
  }

  logFile << GetTimestampString() << " Host->IG frame = " << m_FrameNumber.Value() << " message = " << messageName << std::endl;
  for (const auto& field : fields)
  {
    logFile << "    " << field.first << " = " << field.second << std::endl;
  }
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiIgControl& igControl) const
{
  LogExtractedFields("IG_CONTROL", {{"databaseNumber", std::to_string(igControl.databaseNumber.Value())},
                                    {"bEntityTypeSubstitutionEnabled", igControl.bEntityTypeSubstitutionEnabled ? "true" : "false"},
                                    {"eIgMode", ConvertEIGModeToString(igControl.eIgMode)},
                                    {"bTimestampValid", igControl.bTimestampValid ? "true" : "false"},
                                    {"bSmoothingEnabled", igControl.bSmoothingEnabled ? "true" : "false"},
                                    {"hostFrameNumber", std::to_string(igControl.hostFrameNumber.Value())},
                                    {"lastIgFrameNumber", std::to_string(igControl.lastIgFrameNumber.Value())},
                                    {"timestamp", std::to_string(igControl.timestamp)}});
}

#define DEFINE_EXTRACTED_MESSAGE_LOGGER(TYPE)                                                                                                                                                                                                                                                              \
  void CCigiMessageLogger::LogMessageFromHostToIG(const TYPE& message) const                                                                                                                                                                                                                               \
  {                                                                                                                                                                                                                                                                                                        \
    LogExtractedMessageImpl(#TYPE, message);                                                                                                                                                                                                                                                               \
  }

DEFINE_EXTRACTED_MESSAGE_LOGGER(SAtmosphere)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCelestialSphere)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiArticulatedPart)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiComponentControl)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiEarthReferenceModel)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiEnvironmentalRegion)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiSensorControl)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiShortArticulatedPart)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiViewControl)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiViewDefinition)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SCigiWaveCondition)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SEnvironmentalConditionsRequest)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SHATHOTEntityRequest)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SHATHOTGlobalRequest)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestEntityToEntityBasic)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestEntityToEntityExtended)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestEntityToGeodeticBasic)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestEntityToGeodeticExtended)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestGeodeticToEntityBasic)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestGeodeticToEntityExtended)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestGeodeticToGeodeticBasic)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightSegmentRequestGeodeticToGeodeticExtended)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightVectorRequestEntityBasic)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightVectorRequestEntityExtended)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightVectorRequestGeodeticBasic)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SLineOfSightVectorRequestGeodeticExtended)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SMotionTrackerViewControl)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SMotionTrackerViewGroupControl)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SEntityBillboardSymbolSurfaceDefinition)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SEntitySymbolSurfaceDefinition)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolCircle)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolClone)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolControl)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolPolygon)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolTextDefinition)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolTexturedCircle)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SSymbolTexturedPolygon)
DEFINE_EXTRACTED_MESSAGE_LOGGER(SViewSymbolSurfaceDefinition)

#undef DEFINE_EXTRACTED_MESSAGE_LOGGER

void CCigiMessageLogger::LogMessageFromIGToHost(const SCigiStartOfFrame& message) const
{
  LogExtractedFields("SCigiStartOfFrame", {{"eVersion", ConvertECigiVersionToString(message.eVersion)},
                                           {"databaseID", ToLogString(message.databaseID)},
                                           {"eIGMode", ConvertEIGModeToString(message.eIGMode)},
                                           {"bTimestampValid", ToLogString(message.bTimestampValid)},
                                           {"bEarthReferenceModel", ToLogString(message.bEarthReferenceModel)},
                                           {"igFrameNumber", ToLogString(message.igFrameNumber)},
                                           {"microseconds", ToLogString(message.microseconds)},
                                           {"lastHostFrameNumber", ToLogString(message.lastHostFrameNumber)},
                                           {"bOverframing", ToLogString(message.bOverframing)},
                                           {"bPagingTerrain", ToLogString(message.bPagingTerrain)},
                                           {"bExcessiveVariableLengthData", ToLogString(message.bExcessiveVariableLengthData)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SAnimationStopNotification& message) const
{
  LogExtractedFields("SAnimationStopNotification", {{"entityID", ToLogString(message.entityID)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SHeightAboveTerrainResponse& message) const
{
  LogExtractedFields("SHeightAboveTerrainResponse", {{"HATHOTID", ToLogString(message.HATHOTID)}, {"bValid", ToLogString(message.bValid)}, {"hostFrameLSN", std::to_string(message.hostFrameLSN)}, {"heightAboveTerrain", std::to_string(message.heightAboveTerrain)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SHeightOfTerrainResponse& message) const
{
  LogExtractedFields("SHeightOfTerrainResponse", {{"HATHOTID", ToLogString(message.HATHOTID)}, {"bValid", ToLogString(message.bValid)}, {"hostFrameLSN", std::to_string(message.hostFrameLSN)}, {"heightOfTerrain", ToLogString(message.heightOfTerrain)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SHATHOTExtendedResponse& message) const
{
  LogExtractedFields("SHATHOTExtendedResponse", {{"HATHOTID", ToLogString(message.HATHOTID)},
                                                 {"bValid", ToLogString(message.bValid)},
                                                 {"hostFrameLSN", std::to_string(message.hostFrameLSN)},
                                                 {"heightAboveTerrain", std::to_string(message.heightAboveTerrain)},
                                                 {"heightOfTerrain", ToLogString(message.heightOfTerrain)},
                                                 {"materialCode", ToLogString(message.materialCode)},
                                                 {"normalVectorAzimuth", ToLogString(message.normalVectorAzimuth)},
                                                 {"normalVectorElevation", ToLogString(message.normalVectorElevation)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SCollisionDetectionSegmentNotification& message) const
{
  LogExtractedFields("SCollisionDetectionSegmentNotification", {{"entityID", ToLogString(message.entityID)}, {"segmentID", ToLogString(message.segmentID)}, {"materialCode", ToLogString(message.materialCode)}, {"fIntersectionDistance", std::to_string(message.fIntersectionDistance)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SCollisionDetectionSegmentEntityNotification& message) const
{
  LogExtractedFields(
    "SCollisionDetectionSegmentEntityNotification",
    {{"entityID", ToLogString(message.entityID)}, {"contactedEntityID", ToLogString(message.contactedEntityID)}, {"segmentID", ToLogString(message.segmentID)}, {"materialCode", ToLogString(message.materialCode)}, {"fIntersectionDistance", std::to_string(message.fIntersectionDistance)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SCollisionDetectionVolumeNotification& message) const
{
  LogExtractedFields("SCollisionDetectionVolumeNotification", {{"entityID", ToLogString(message.entityID)}, {"volumeID", ToLogString(message.volumeID)}, {"contactedVolumeID", ToLogString(message.contactedVolumeID)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SCollisionDetectionVolumeEntityNotification& message) const
{
  LogExtractedFields("SCollisionDetectionVolumeEntityNotification", {{"entityID", ToLogString(message.entityID)}, {"volumeID", ToLogString(message.volumeID)}, {"contactedVolumeID", ToLogString(message.contactedVolumeID)}, {"contactedEntityID", ToLogString(message.contactedEntityID)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SEventNotification& message) const
{
  LogExtractedFields("SEventNotification", {{"EventID", ToLogString(message.EventID)}, {"EventData1", std::to_string(message.EventData1)}, {"EventData2", std::to_string(message.EventData2)}, {"EventData3", std::to_string(message.EventData3)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SImageGeneratorNotification& message) const
{
  LogExtractedFields("SImageGeneratorNotification", {{"MessageID", ToLogString(message.MessageID)}, {"sData", message.sData}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SLineOfSightResponse& message) const
{
  LogExtractedFields("SLineOfSightResponse",
                     {{"lineOfSightRequestID", ToLogString(message.lineOfSightRequestID)}, {"bValid", ToLogString(message.bValid)}, {"hostFrameLSN", std::to_string(message.hostFrameLSN)}, {"responseCount", std::to_string(message.responseCount)}, {"dRange", std::to_string(message.dRange)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SLineOfSightEntityResponse& message) const
{
  LogExtractedFields("SLineOfSightEntityResponse", {{"lineOfSightRequestID", ToLogString(message.lineOfSightRequestID)},
                                                    {"bValid", ToLogString(message.bValid)},
                                                    {"hostFrameLSN", std::to_string(message.hostFrameLSN)},
                                                    {"responseCount", std::to_string(message.responseCount)},
                                                    {"dRange", std::to_string(message.dRange)},
                                                    {"entityID", ToLogString(message.entityID)},
                                                    {"bVisible", ToLogString(message.bVisible)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SLineOfSightExtendedGeodeticCoordinatesResponse& message) const
{
  LogExtractedFields("SLineOfSightExtendedGeodeticCoordinatesResponse", {{"lineOfSightRequestID", ToLogString(message.lineOfSightRequestID)},
                                                                         {"bValid", ToLogString(message.bValid)},
                                                                         {"bRangeValid", ToLogString(message.bRangeValid)},
                                                                         {"bVisible", ToLogString(message.bVisible)},
                                                                         {"hostFrameLSN", std::to_string(message.hostFrameLSN)},
                                                                         {"responseCount", std::to_string(message.responseCount)},
                                                                         {"dRange", std::to_string(message.dRange)},
                                                                         {"surfaceColor", ToLogString(message.surfaceColor)},
                                                                         {"materialCode", ToLogString(message.materialCode)},
                                                                         {"fNormalVectorAzimuth", std::to_string(message.fNormalVectorAzimuth)},
                                                                         {"fNormalVectorElevation", std::to_string(message.fNormalVectorElevation)},
                                                                         {"geodeticCoordinates", ToLogString(message.geodeticCoordinates)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SLineOfSightExtendedEntityCoordinatesResponse& message) const
{
  LogExtractedFields("SLineOfSightExtendedEntityCoordinatesResponse", {{"lineOfSightRequestID", ToLogString(message.lineOfSightRequestID)},
                                                                       {"bValid", ToLogString(message.bValid)},
                                                                       {"bRangeValid", ToLogString(message.bRangeValid)},
                                                                       {"bVisible", ToLogString(message.bVisible)},
                                                                       {"hostFrameLSN", std::to_string(message.hostFrameLSN)},
                                                                       {"responseCount", std::to_string(message.responseCount)},
                                                                       {"dRange", std::to_string(message.dRange)},
                                                                       {"surfaceColor", ToLogString(message.surfaceColor)},
                                                                       {"materialCode", ToLogString(message.materialCode)},
                                                                       {"fNormalVectorAzimuth", std::to_string(message.fNormalVectorAzimuth)},
                                                                       {"fNormalVectorElevation", std::to_string(message.fNormalVectorElevation)},
                                                                       {"entityID", ToLogString(message.entityID)},
                                                                       {"offset", ToLogString(message.offset)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SLineOfSightExtendedEntityGeodeticCoordinatesResponse& message) const
{
  LogExtractedFields("SLineOfSightExtendedEntityGeodeticCoordinatesResponse", {{"lineOfSightRequestID", ToLogString(message.lineOfSightRequestID)},
                                                                               {"bValid", ToLogString(message.bValid)},
                                                                               {"bRangeValid", ToLogString(message.bRangeValid)},
                                                                               {"bVisible", ToLogString(message.bVisible)},
                                                                               {"hostFrameLSN", std::to_string(message.hostFrameLSN)},
                                                                               {"responseCount", std::to_string(message.responseCount)},
                                                                               {"dRange", std::to_string(message.dRange)},
                                                                               {"surfaceColor", ToLogString(message.surfaceColor)},
                                                                               {"materialCode", ToLogString(message.materialCode)},
                                                                               {"fNormalVectorAzimuth", std::to_string(message.fNormalVectorAzimuth)},
                                                                               {"fNormalVectorElevation", std::to_string(message.fNormalVectorElevation)},
                                                                               {"entityID", ToLogString(message.entityID)},
                                                                               {"geodeticCoordinates", ToLogString(message.geodeticCoordinates)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SPositionResponseGeodeticCoordinates& message) const
{
  LogExtractedFields("SPositionResponseGeodeticCoordinates", {{"eObjectClass", ConvertEObjectClassToString(message.eObjectClass)},
                                                              {"objectID", std::to_string(message.objectID)},
                                                              {"yaw", ToLogString(message.rotation.yaw)},
                                                              {"pitch", ToLogString(message.rotation.pitch)},
                                                              {"roll", ToLogString(message.rotation.roll)},
                                                              {"geodeticCoordinates", ToLogString(message.geodeticCoordinates)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SPositionResponseParentEntityCoordinates& message) const
{
  LogExtractedFields(
    "SPositionResponseParentEntityCoordinates",
    {{"eObjectClass", ConvertEObjectClassToString(message.eObjectClass)}, {"objectID", std::to_string(message.objectID)}, {"yaw", ToLogString(message.rotation.yaw)}, {"pitch", ToLogString(message.rotation.pitch)}, {"roll", ToLogString(message.rotation.roll)}, {"offset", ToLogString(message.offset)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SPositionResponseArticulatedPartCoordinates& message) const
{
  LogExtractedFields("SPositionResponseArticulatedPartCoordinates", {{"eObjectClass", ConvertEObjectClassToString(message.eObjectClass)},
                                                                     {"objectID", std::to_string(message.objectID)},
                                                                     {"yaw", ToLogString(message.rotation.yaw)},
                                                                     {"pitch", ToLogString(message.rotation.pitch)},
                                                                     {"roll", ToLogString(message.rotation.roll)},
                                                                     {"articulatedPartID", ToLogString(message.articulatedPartID)},
                                                                     {"offset", ToLogString(message.offset)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SWeatherConditionsResponse& message) const
{
  LogExtractedFields("SWeatherConditionsResponse", {{"requestID", std::to_string(message.requestID)},
                                                    {"humidity", ToLogString(message.humidity)},
                                                    {"fAirTemperature", std::to_string(message.fAirTemperature)},
                                                    {"fVisibilityRange", std::to_string(message.fVisibilityRange)},
                                                    {"horizontalWindSpeed", std::to_string(message.windSpeedHorVer.horizontalWindSpeed)},
                                                    {"verticalWindSpeed", std::to_string(message.windSpeedHorVer.verticalWindSpeed)},
                                                    {"fWindDirection", std::to_string(message.fWindDirection)},
                                                    {"fBarometricPressure", std::to_string(message.fBarometricPressure)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SAerosolConcentrationResponse& message) const
{
  LogExtractedFields("SAerosolConcentrationResponse", {{"requestID", std::to_string(message.requestID)}, {"layerID", std::to_string(message.layerID)}, {"fAerosolConcentration", std::to_string(message.fAerosolConcentration)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SMaritimeSurfaceConditionsResponse& message) const
{
  LogExtractedFields("SMaritimeSurfaceConditionsResponse",
                     {{"requestID", std::to_string(message.requestID)}, {"fSeaSurfaceHeight", ToLogString(message.fSeaSurfaceHeight)}, {"fSurfaceWaterTemperature", ToLogString(message.fSurfaceWaterTemperature)}, {"surfaceClarity", ToLogString(message.surfaceClarity)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const STerrestrialSurfaceConditionsResponse& message) const
{
  LogExtractedFields("STerrestrialSurfaceConditionsResponse", {{"requestID", std::to_string(message.requestID)}, {"surfaceConditionID", std::to_string(message.surfaceConditionID)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SSensorResponse& message) const
{
  LogExtractedFields("SSensorResponse", {{"sensorID", ToLogString(message.sensorID)},
                                         {"eSensorStatus", ConvertESensorStatusToString(message.eSensorStatus)},
                                         {"viewID", ToLogString(message.viewID)},
                                         {"gateSize", ToLogString(message.gateSize)},
                                         {"gatePosition", ToLogString(message.gatePosition)},
                                         {"hostFrameNumber", ToLogString(message.hostFrameNumber)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SSensorExtendedResponse& message) const
{
  LogExtractedFields("SSensorExtendedResponse", {{"viewID", ToLogString(message.viewID)},
                                                 {"sensorID", ToLogString(message.sensorID)},
                                                 {"eSensorStatus", ConvertESensorStatusToString(message.eSensorStatus)},
                                                 {"gateSize", ToLogString(message.gateSize)},
                                                 {"gatePosition", ToLogString(message.gatePosition)},
                                                 {"hostFrameNumber", ToLogString(message.hostFrameNumber)},
                                                 {"trackPoint", ToLogString(message.trackPoint)}});
}

void CCigiMessageLogger::LogMessageFromIGToHost(const SSensorExtendedEntityResponse& message) const
{
  LogExtractedFields("SSensorExtendedEntityResponse", {{"viewID", ToLogString(message.viewID)},
                                                       {"entityID", ToLogString(message.entityID)},
                                                       {"sensorID", ToLogString(message.sensorID)},
                                                       {"eSensorStatus", ConvertESensorStatusToString(message.eSensorStatus)},
                                                       {"gateSize", ToLogString(message.gateSize)},
                                                       {"gatePosition", ToLogString(message.gatePosition)},
                                                       {"hostFrameNumber", ToLogString(message.hostFrameNumber)},
                                                       {"trackPoint", ToLogString(message.trackPoint)}});
}

std::string CCigiMessageLogger::GetTimestampString() const
{
  const auto now = std::chrono::system_clock::now();
  const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
  std::tm localTime = {};
#ifdef _WIN32
  localtime_s(&localTime, &nowTime);
#else
  localtime_r(&nowTime, &localTime);
#endif

  std::ostringstream ss;
  ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
  return ss.str();
}

std::string CCigiMessageLogger::ToHexString(const uint8_t* pBuffer, int nSize) const
{
  std::ostringstream ss;
  ss << std::hex << std::uppercase << std::setfill('0');
  for (int n = 0; n < nSize; ++n)
  {
    if (n > 0)
    {
      ss << ' ';
    }
    ss << std::setw(2) << static_cast<unsigned int>(pBuffer[n]);
  }

  return ss.str();
}

void CCigiMessageLogger::LogPackets(std::ofstream& logFile, ECigiVersion eCigiVersion, const uint8_t* pBuffer, int nMessageSize) const
{
  const uint8_t* pCurrentBuffer = pBuffer;
  const uint8_t* pBufferEnd = pBuffer + nMessageSize;
  int nPacketIndex = 0;

  while (pCurrentBuffer < pBufferEnd)
  {
    const ptrdiff_t nRemainingBytes = pBufferEnd - pCurrentBuffer;

    if (eCigiVersion == ECigiVersion::VERSION_3_3)
    {
      if (nRemainingBytes < static_cast<ptrdiff_t>(sizeof(SCigiPacketHeaderV3)))
      {
        logFile << "  truncated-header bytesRemaining = " << nRemainingBytes << std::endl;
        break;
      }

      SCigiPacketHeaderV3 packetHeader;
      memcpy(&packetHeader, pCurrentBuffer, sizeof(SCigiPacketHeaderV3));
      const uint16_t nPacketSize = packetHeader.nPacketSize;
      logFile << "  packet = " << nPacketIndex << " opcode = " << OpCodeToLogString(packetHeader.eOpCode) << " size = " << nPacketSize << std::endl;

      if (nPacketSize == 0 || nPacketSize > nRemainingBytes)
      {
        break;
      }

      pCurrentBuffer += nPacketSize;
    }
    else if (eCigiVersion == ECigiVersion::VERSION_4_0)
    {
      if (nRemainingBytes < static_cast<ptrdiff_t>(sizeof(SCigiPacketHeaderV4)))
      {
        logFile << "  truncated-header bytesRemaining = " << nRemainingBytes << std::endl;
        break;
      }

      SCigiPacketHeaderV4 packetHeader;
      memcpy(&packetHeader, pCurrentBuffer, sizeof(SCigiPacketHeaderV4));
      const uint16_t nPacketSize = packetHeader.nPacketSize;
      logFile << "  packet = " << nPacketIndex << " opcode = " << OpCodeToLogString(packetHeader.eOpCode) << " size = " << nPacketSize << std::endl;

      if (nPacketSize == 0 || nPacketSize > nRemainingBytes)
      {
        break;
      }

      pCurrentBuffer += nPacketSize;
    }
    else
    {
      logFile << "  unknown-cigi-version" << std::endl;
      break;
    }

    ++nPacketIndex;
  }
}

//Usage of this software is prohibited except through a Software Licensing Agreement with SimBlocks LLC.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
