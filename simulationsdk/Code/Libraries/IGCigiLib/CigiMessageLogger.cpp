//Copyright SimBlocks LLC 2016-2026
#include "CigiMessageLogger.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>

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

  std::string ToLogString(uint8_t value)
  {
    return std::to_string(static_cast<unsigned int>(value));
  }

  std::string ToLogString(int8_t value)
  {
    return std::to_string(static_cast<int>(value));
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

  std::string ToLogString(const sbio::math::Vec3& value)
  {
    std::ostringstream ss;
    ss << "[" << value[0] << ", " << value[1] << ", " << value[2] << "]";
    return ss.str();
  }

  std::string ToLogString(const sbio::math::Vec3f& value)
  {
    std::ostringstream ss;
    ss << "[" << value[0] << ", " << value[1] << ", " << value[2] << "]";
    return ss.str();
  }

  std::string ToLogString(const sbio::STextureCoordinateUV& value)
  {
    std::ostringstream ss;
    ss << "U = " << value.U << ", V = " << value.V;
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

  typedef std::vector<std::pair<std::string, std::string>> TLogFields;

  template <typename T>
  void AddField(TLogFields& fields, const std::string& name, const T& value)
  {
    fields.push_back(std::make_pair(name, ToLogString(value)));
  }

  template <typename T>
  void AddEnumField(TLogFields& fields, const std::string& name, T value)
  {
    fields.push_back(std::make_pair(name, EnumValueToLogString(value)));
  }

  void AddVec2Fields(TLogFields& fields, const std::string& name, const sbio::math::Vec2f& value)
  {
    AddField(fields, name + "[0]", value[0]);
    AddField(fields, name + "[1]", value[1]);
  }

  void AddVec3Fields(TLogFields& fields, const std::string& name, const sbio::math::Vec3& value)
  {
    AddField(fields, name + "[0]", value[0]);
    AddField(fields, name + "[1]", value[1]);
    AddField(fields, name + "[2]", value[2]);
  }

  void AddVec3Fields(TLogFields& fields, const std::string& name, const sbio::math::Vec3f& value)
  {
    AddField(fields, name + "[0]", value[0]);
    AddField(fields, name + "[1]", value[1]);
    AddField(fields, name + "[2]", value[2]);
  }

  void AddCigiBodyCoordinateFields(TLogFields& fields, const std::string& name, const sbio::cigi::CigiBodyCoordinates& value)
  {
    AddField(fields, name + "[0]", value[0]);
    AddField(fields, name + "[1]", value[1]);
    AddField(fields, name + "[2]", value[2]);
  }

  void AddRotationFields(TLogFields& fields, const std::string& name, const sbio::cigi::TCigiBodyEulerRotation& value)
  {
    AddField(fields, name + ".yaw", value.yaw);
    AddField(fields, name + ".pitch", value.pitch);
    AddField(fields, name + ".roll", value.roll);
  }

  void AddRotationFields(TLogFields& fields, const std::string& name, const sbio::cigi::TCigiNEDEulerRotation& value)
  {
    AddField(fields, name + ".yaw", value.yaw);
    AddField(fields, name + ".pitch", value.pitch);
    AddField(fields, name + ".roll", value.roll);
  }

  void AddGeodeticFields(TLogFields& fields, const std::string& name, const sbio::math::SGeodeticCoordinates& value)
  {
    AddField(fields, name + ".latitude", value.latitude);
    AddField(fields, name + ".longitude", value.longitude);
    AddField(fields, name + ".altitude", value.altitude);
  }

  void AddTextureCoordinateFields(TLogFields& fields, const std::string& name, const sbio::STextureCoordinateUV& value)
  {
    AddField(fields, name + ".U", value.U);
    AddField(fields, name + ".V", value.V);
  }

  void AddColorFields(TLogFields& fields, const std::string& name, const sbio::SColor32& value)
  {
    AddField(fields, name + ".r", static_cast<unsigned int>(value.r));
    AddField(fields, name + ".g", static_cast<unsigned int>(value.g));
    AddField(fields, name + ".b", static_cast<unsigned int>(value.b));
    AddField(fields, name + ".a", static_cast<unsigned int>(value.a));
  }

  void AddHATHOTRequestFields(TLogFields& fields, const sbio::cigi::SBaseHATHOTRequest& message)
  {
    AddField(fields, "requestID", message.requestID);
    AddField(fields, "updatePeriod", message.updatePeriod);
    AddField(fields, "lastHostFrameNumber", message.lastHostFrameNumber);
    AddEnumField(fields, "eRequestType", message.eRequestType);
  }

  void AddLineOfSightRequestFields(TLogFields& fields, const sbio::cigi::SLineOfSightRequest& message)
  {
    AddField(fields, "requestID", message.requestID);
    AddField(fields, "updatePeriod", message.updatePeriod);
    AddField(fields, "nAlphaThreshold", message.nAlphaThreshold);
    AddField(fields, "nMaterialMask", message.nMaterialMask);
    AddField(fields, "lastHostFrameNumber", message.lastHostFrameNumber);
  }

  void AddLineOfSightVectorRequestFields(TLogFields& fields, const sbio::cigi::SLineOfSightVectorRequest& message)
  {
    AddLineOfSightRequestFields(fields, message);
    AddField(fields, "azimuth", message.azimuth);
    AddField(fields, "elevation", message.elevation);
    AddField(fields, "fMinimumRange", message.fMinimumRange);
    AddField(fields, "fMaximumRange", message.fMaximumRange);
  }

  void AddMotionTrackerControlFields(TLogFields& fields, const sbio::cigi::SMotionTrackerControl& message)
  {
    AddField(fields, "motionTrackerID", message.motionTrackerID);
    AddField(fields, "bEnable", message.bEnable);
    AddField(fields, "bBoresightEnable", message.bBoresightEnable);
    AddField(fields, "bXEnable", message.bXEnable);
    AddField(fields, "bYEnable", message.bYEnable);
    AddField(fields, "bZEnable", message.bZEnable);
    AddField(fields, "bRollEnable", message.bRollEnable);
    AddField(fields, "bPitchEnable", message.bPitchEnable);
    AddField(fields, "bYawEnable", message.bYawEnable);
  }

  void AddSymbolSurfaceDefinitionFields(TLogFields& fields, const sbio::symbol::SSymbolSurfaceDefinition& message)
  {
    AddField(fields, "surfaceID", message.surfaceID);
    AddEnumField(fields, "eSurfaceState", message.eSurfaceState);
    AddField(fields, "width", message.width);
    AddField(fields, "height", message.height);
    AddVec2Fields(fields, "uvMin", message.uvMin);
    AddVec2Fields(fields, "uvMax", message.uvMax);
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

void CCigiMessageLogger::LogExtractedFields(const std::string& messageName, const std::vector<std::pair<std::string, std::string>>& fields) const
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

void CCigiMessageLogger::LogMessageFromHostToIG(const STopLevelEntityPosition& message) const
{
  TLogFields fields;
  AddField(fields, "entityID", message.entityID);
  AddField(fields, "bAttached", message.bAttached);
  AddEnumField(fields, "eClamp", message.eClamp);
  AddGeodeticFields(fields, "geodeticCoordinates", message.geodeticCoordinates);
  AddRotationFields(fields, "rotation", message.rotation);
  LogExtractedFields("STopLevelEntityPosition", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SChildEntityPosition& message) const
{
  TLogFields fields;
  AddField(fields, "entityID", message.entityID);
  AddField(fields, "bAttached", message.bAttached);
  AddField(fields, "parentID", message.parentID);
  AddVec3Fields(fields, "offset", message.offset);
  AddRotationFields(fields, "rotation", message.rotation);
  LogExtractedFields("SChildEntityPosition", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SAtmosphere& message) const
{
  LogExtractedFields("SAtmosphere", {{"fHumidity", ToLogString(message.fHumidity)},
                                     {"fAirTemperature", ToLogString(message.fAirTemperature)},
                                     {"fVisibilityRange", ToLogString(message.fVisibilityRange)},
                                     {"fHorizontalWindSpeed", ToLogString(message.fHorizontalWindSpeed)},
                                     {"fVerticalWindSpeed", ToLogString(message.fVerticalWindSpeed)},
                                     {"fWindDirection", ToLogString(message.fWindDirection)},
                                     {"fBarometricPressure", ToLogString(message.fBarometricPressure)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCelestialSphere& message) const
{
  LogExtractedFields("SCelestialSphere", {{"bEphemerisEnabled", ToLogString(message.bEphemerisEnabled)},
                                          {"bSunEnabled", ToLogString(message.bSunEnabled)},
                                          {"bMoonEnabled", ToLogString(message.bMoonEnabled)},
                                          {"bStarsEnabled", ToLogString(message.bStarsEnabled)},
                                          {"fStarIntensity", ToLogString(message.fStarIntensity)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiArticulatedPart& message) const
{
  TLogFields fields;
  AddField(fields, "entityID", message.entityID);
  AddField(fields, "articulatedPartID", message.articulatedPartID);
  AddField(fields, "bEnabled", message.bEnabled);
  AddField(fields, "bOffsetEnabled[0]", message.bOffsetEnabled[0]);
  AddField(fields, "bOffsetEnabled[1]", message.bOffsetEnabled[1]);
  AddField(fields, "bOffsetEnabled[2]", message.bOffsetEnabled[2]);
  AddField(fields, "bRollEnabled", message.bRollEnabled);
  AddField(fields, "bPitchEnabled", message.bPitchEnabled);
  AddField(fields, "bYawEnabled", message.bYawEnabled);
  AddCigiBodyCoordinateFields(fields, "offset", message.offset);
  AddRotationFields(fields, "rotation", message.rotation);
  LogExtractedFields("SCigiArticulatedPart", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiComponentControl& message) const
{
  LogExtractedFields("SCigiComponentControl", {{"key.componentID", ToLogString(message.key.componentID)},
                                               {"key.componentClassID", ToLogString(message.key.componentClassID)},
                                               {"key.nInstanceID", ToLogString(message.key.nInstanceID)},
                                               {"state.nComponentState", ToLogString(message.state.nComponentState)},
                                               {"state.componentData[0]", ToLogString(message.state.componentData[0])},
                                               {"state.componentData[1]", ToLogString(message.state.componentData[1])},
                                               {"state.componentData[2]", ToLogString(message.state.componentData[2])},
                                               {"state.componentData[3]", ToLogString(message.state.componentData[3])},
                                               {"state.componentData[4]", ToLogString(message.state.componentData[4])},
                                               {"state.componentData[5]", ToLogString(message.state.componentData[5])}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiEarthReferenceModel& message) const
{
  LogExtractedFields("SCigiEarthReferenceModel", {{"eEarthReferenceModel", EnumValueToLogString(message.eEarthReferenceModel)},
                                                  {"fEquatorialRadius", ToLogString(message.fEquatorialRadius)},
                                                  {"fFlattening", ToLogString(message.fFlattening)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiEnvironmentalRegion& message) const
{
  TLogFields fields;
  AddEnumField(fields, "eRegionState", message.eRegionState);
  AddEnumField(fields, "eMergeWeatherProperties", message.eMergeWeatherProperties);
  AddEnumField(fields, "eMergeAerosolConcentrations", message.eMergeAerosolConcentrations);
  AddEnumField(fields, "eMergeMaritimeSurfaceConditions", message.eMergeMaritimeSurfaceConditions);
  AddEnumField(fields, "eMergeTerrestrialSurfaceConditions", message.eMergeTerrestrialSurfaceConditions);
  AddField(fields, "regionID", message.regionID);
  AddField(fields, "latitude", message.latitude);
  AddField(fields, "longitude", message.longitude);
  AddVec2Fields(fields, "size", message.size);
  AddField(fields, "fCornerRadius", message.fCornerRadius);
  AddField(fields, "fRotation", message.fRotation);
  AddField(fields, "fTransition", message.fTransition);
  LogExtractedFields("SCigiEnvironmentalRegion", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiSensorControl& message) const
{
  LogExtractedFields("SCigiSensorControl", {{"sensorID", ToLogString(message.sensorID)},
                                            {"eTrackMode", EnumValueToLogString(message.eTrackMode)},
                                            {"bSensorOn", ToLogString(message.bSensorOn)},
                                            {"ePolarity", EnumValueToLogString(message.ePolarity)},
                                            {"bLineByLineDropoutEnabled", ToLogString(message.bLineByLineDropoutEnabled)},
                                            {"bAutomaticGain", ToLogString(message.bAutomaticGain)},
                                            {"eSensorTrack", EnumValueToLogString(message.eSensorTrack)},
                                            {"bExtendedResponse", ToLogString(message.bExtendedResponse)},
                                            {"viewID", ToLogString(message.viewID)},
                                            {"gain", ToLogString(message.gain)},
                                            {"level", ToLogString(message.level)},
                                            {"fACCoupling", ToLogString(message.fACCoupling)},
                                            {"noise", ToLogString(message.noise)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiShortArticulatedPart& message) const
{
  LogExtractedFields("SCigiShortArticulatedPart", {{"entityID", ToLogString(message.entityID)},
                                                   {"articulatedPartID1", ToLogString(message.articulatedPartID1)},
                                                   {"articulatedPartID2", ToLogString(message.articulatedPartID2)},
                                                   {"eDOF1", EnumValueToLogString(message.eDOF1)},
                                                   {"eDOF2", EnumValueToLogString(message.eDOF2)},
                                                   {"bArticulatedPart1Enabled", ToLogString(message.bArticulatedPart1Enabled)},
                                                   {"bArticulatedPart2Enabled", ToLogString(message.bArticulatedPart2Enabled)},
                                                   {"fDOF1", ToLogString(message.fDOF1)},
                                                   {"fDOF2", ToLogString(message.fDOF2)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiViewControl& message) const
{
  TLogFields fields;
  AddField(fields, "groupID", message.groupID);
  AddField(fields, "viewGroupID", message.viewGroupID);
  AddField(fields, "offsetEnabled[0]", message.offsetEnabled[0]);
  AddField(fields, "offsetEnabled[1]", message.offsetEnabled[1]);
  AddField(fields, "offsetEnabled[2]", message.offsetEnabled[2]);
  AddField(fields, "bYawEnabled", message.bYawEnabled);
  AddField(fields, "bPitchEnabled", message.bPitchEnabled);
  AddField(fields, "bRollEnabled", message.bRollEnabled);
  AddField(fields, "viewID", message.viewID);
  AddField(fields, "entityID", message.entityID);
  AddCigiBodyCoordinateFields(fields, "offset", message.offset);
  AddRotationFields(fields, "rotation", message.rotation);
  LogExtractedFields("SCigiViewControl", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiViewDefinition& message) const
{
  LogExtractedFields("SCigiViewDefinition", {{"viewID", ToLogString(message.viewID)},
                                             {"viewGroupID", ToLogString(message.viewGroupID)},
                                             {"bNearEnabled", ToLogString(message.bNearEnabled)},
                                             {"bFarEnabled", ToLogString(message.bFarEnabled)},
                                             {"bLeftEnabled", ToLogString(message.bLeftEnabled)},
                                             {"bRightEnabled", ToLogString(message.bRightEnabled)},
                                             {"bTopEnabled", ToLogString(message.bTopEnabled)},
                                             {"bBottomEnabled", ToLogString(message.bBottomEnabled)},
                                             {"eMirrorMode", EnumValueToLogString(message.eMirrorMode)},
                                             {"pixelReplicationMode", ToLogString(message.pixelReplicationMode)},
                                             {"eProjectionMode", EnumValueToLogString(message.eProjectionMode)},
                                             {"bReorder", ToLogString(message.bReorder)},
                                             {"viewType", ToLogString(message.viewType)},
                                             {"fNear", ToLogString(message.fNear)},
                                             {"fFar", ToLogString(message.fFar)},
                                             {"fLeft", ToLogString(message.fLeft)},
                                             {"fRight", ToLogString(message.fRight)},
                                             {"fTop", ToLogString(message.fTop)},
                                             {"fBottom", ToLogString(message.fBottom)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SCigiWaveCondition& message) const
{
  LogExtractedFields("SCigiWaveCondition", {{"waveID", ToLogString(message.waveID)},
                                            {"bWaveEnabled", ToLogString(message.bWaveEnabled)},
                                            {"eBreakerType", EnumValueToLogString(message.eBreakerType)},
                                            {"fWaveHeight", ToLogString(message.fWaveHeight)},
                                            {"fWavelength", ToLogString(message.fWavelength)},
                                            {"fPeriod", ToLogString(message.fPeriod)},
                                            {"direction", ToLogString(message.direction)},
                                            {"phaseOffset", ToLogString(message.phaseOffset)},
                                            {"leading", ToLogString(message.leading)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SEnvironmentalConditionsRequest& message) const
{
  TLogFields fields;
  AddField(fields, "bMaritimeSurfaceConditionsRequest", message.bMaritimeSurfaceConditionsRequest);
  AddField(fields, "bTerrestrialSurfaceConditionsRequest", message.bTerrestrialSurfaceConditionsRequest);
  AddField(fields, "bWeatherConditionsRequest", message.bWeatherConditionsRequest);
  AddField(fields, "bAerosolConcentrationsRequest", message.bAerosolConcentrationsRequest);
  AddField(fields, "nRequestID", message.nRequestID);
  AddGeodeticFields(fields, "geodeticCoordinates", message.geodeticCoordinates);
  LogExtractedFields("SEnvironmentalConditionsRequest", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SHATHOTEntityRequest& message) const
{
  TLogFields fields;
  AddHATHOTRequestFields(fields, message);
  AddField(fields, "entityID", message.entityID);
  AddVec3Fields(fields, "offset", message.offset);
  LogExtractedFields("SHATHOTEntityRequest", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SHATHOTGlobalRequest& message) const
{
  TLogFields fields;
  AddHATHOTRequestFields(fields, message);
  AddGeodeticFields(fields, "geodeticCoordinates", message.geodeticCoordinates);
  LogExtractedFields("SHATHOTGlobalRequest", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestEntityToEntityBasic& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddField(fields, "sourceEntityID", message.sourceEntityID);
  AddCigiBodyCoordinateFields(fields, "sourceOffset", message.sourceOffset);
  AddField(fields, "destinationEntityID", message.destinationEntityID);
  AddCigiBodyCoordinateFields(fields, "destinationOffset", message.destinationOffset);
  LogExtractedFields("SLineOfSightSegmentRequestEntityToEntityBasic", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestEntityToEntityExtended& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddField(fields, "sourceEntityID", message.sourceEntityID);
  AddCigiBodyCoordinateFields(fields, "sourceOffset", message.sourceOffset);
  AddField(fields, "destinationEntityID", message.destinationEntityID);
  AddCigiBodyCoordinateFields(fields, "destinationOffset", message.destinationOffset);
  AddEnumField(fields, "eResponseCoordinateSystem", message.eResponseCoordinateSystem);
  LogExtractedFields("SLineOfSightSegmentRequestEntityToEntityExtended", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestEntityToGeodeticBasic& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddField(fields, "sourceEntityID", message.sourceEntityID);
  AddCigiBodyCoordinateFields(fields, "sourceOffset", message.sourceOffset);
  AddGeodeticFields(fields, "destinationGeodeticCoordinates", message.destinationGeodeticCoordinates);
  LogExtractedFields("SLineOfSightSegmentRequestEntityToGeodeticBasic", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestEntityToGeodeticExtended& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddField(fields, "sourceEntityID", message.sourceEntityID);
  AddCigiBodyCoordinateFields(fields, "sourceOffset", message.sourceOffset);
  AddGeodeticFields(fields, "destinationGeodeticCoordinates", message.destinationGeodeticCoordinates);
  AddEnumField(fields, "eResponseCoordinateSystem", message.eResponseCoordinateSystem);
  LogExtractedFields("SLineOfSightSegmentRequestEntityToGeodeticExtended", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestGeodeticToEntityBasic& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddGeodeticFields(fields, "sourceGeodeticCoordinates", message.sourceGeodeticCoordinates);
  AddField(fields, "destinationEntityID", message.destinationEntityID);
  AddVec3Fields(fields, "destinationOffset", message.destinationOffset);
  LogExtractedFields("SLineOfSightSegmentRequestGeodeticToEntityBasic", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestGeodeticToEntityExtended& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddGeodeticFields(fields, "sourceGeodeticCoordinates", message.sourceGeodeticCoordinates);
  AddField(fields, "destinationEntityID", message.destinationEntityID);
  AddVec3Fields(fields, "destinationOffset", message.destinationOffset);
  AddEnumField(fields, "eResponseCoordinateSystem", message.eResponseCoordinateSystem);
  LogExtractedFields("SLineOfSightSegmentRequestGeodeticToEntityExtended", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestGeodeticToGeodeticBasic& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddGeodeticFields(fields, "sourceGeodeticCoordinates", message.sourceGeodeticCoordinates);
  AddGeodeticFields(fields, "destinationGeodeticCoordinates", message.destinationGeodeticCoordinates);
  LogExtractedFields("SLineOfSightSegmentRequestGeodeticToGeodeticBasic", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightSegmentRequestGeodeticToGeodeticExtended& message) const
{
  TLogFields fields;
  AddLineOfSightRequestFields(fields, message);
  AddGeodeticFields(fields, "sourceGeodeticCoordinates", message.sourceGeodeticCoordinates);
  AddGeodeticFields(fields, "destinationGeodeticCoordinates", message.destinationGeodeticCoordinates);
  AddEnumField(fields, "eResponseCoordinateSystem", message.eResponseCoordinateSystem);
  LogExtractedFields("SLineOfSightSegmentRequestGeodeticToGeodeticExtended", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightVectorRequestEntityBasic& message) const
{
  TLogFields fields;
  AddLineOfSightVectorRequestFields(fields, message);
  AddField(fields, "sourceEntityID", message.sourceEntityID);
  AddCigiBodyCoordinateFields(fields, "sourceOffset", message.sourceOffset);
  LogExtractedFields("SLineOfSightVectorRequestEntityBasic", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightVectorRequestEntityExtended& message) const
{
  TLogFields fields;
  AddLineOfSightVectorRequestFields(fields, message);
  AddField(fields, "sourceEntityID", message.sourceEntityID);
  AddCigiBodyCoordinateFields(fields, "sourceOffset", message.sourceOffset);
  AddEnumField(fields, "eResponseCoordinateSystem", message.eResponseCoordinateSystem);
  LogExtractedFields("SLineOfSightVectorRequestEntityExtended", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightVectorRequestGeodeticBasic& message) const
{
  TLogFields fields;
  AddLineOfSightVectorRequestFields(fields, message);
  AddGeodeticFields(fields, "sourceGeodeticCoordinates", message.sourceGeodeticCoordinates);
  LogExtractedFields("SLineOfSightVectorRequestGeodeticBasic", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SLineOfSightVectorRequestGeodeticExtended& message) const
{
  TLogFields fields;
  AddLineOfSightVectorRequestFields(fields, message);
  AddGeodeticFields(fields, "sourceGeodeticCoordinates", message.sourceGeodeticCoordinates);
  AddEnumField(fields, "eResponseCoordinateSystem", message.eResponseCoordinateSystem);
  LogExtractedFields("SLineOfSightVectorRequestGeodeticExtended", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SMotionTrackerViewControl& message) const
{
  TLogFields fields;
  AddMotionTrackerControlFields(fields, message);
  AddField(fields, "viewID", message.viewID);
  LogExtractedFields("SMotionTrackerViewControl", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SMotionTrackerViewGroupControl& message) const
{
  TLogFields fields;
  AddMotionTrackerControlFields(fields, message);
  AddField(fields, "viewGroupID", message.viewGroupID);
  LogExtractedFields("SMotionTrackerViewGroupControl", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SEntityBillboardSymbolSurfaceDefinition& message) const
{
  TLogFields fields;
  AddSymbolSurfaceDefinitionFields(fields, message);
  AddField(fields, "entityID", message.entityID);
  AddField(fields, "bPerspectiveGrowthEnabled", message.bPerspectiveGrowthEnabled);
  AddVec3Fields(fields, "offsetToEntity", message.offsetToEntity);
  LogExtractedFields("SEntityBillboardSymbolSurfaceDefinition", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SEntitySymbolSurfaceDefinition& message) const
{
  TLogFields fields;
  AddSymbolSurfaceDefinitionFields(fields, message);
  AddField(fields, "entityID", message.entityID);
  AddVec3Fields(fields, "offsetToSurface", message.offsetToSurface);
  AddField(fields, "yaw", message.yaw);
  AddField(fields, "pitch", message.pitch);
  AddField(fields, "roll", message.roll);
  LogExtractedFields("SEntitySymbolSurfaceDefinition", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolCircle& message) const
{
  TLogFields fields;
  AddField(fields, "symbolID", message.symbolID);
  AddField(fields, "stipplePattern", message.stipplePattern);
  AddEnumField(fields, "eDrawingStyle", message.eDrawingStyle);
  AddField(fields, "fLineWidth", message.fLineWidth);
  AddField(fields, "fStipplePatternLength", message.fStipplePatternLength);
  AddField(fields, "circles.size", message.circles.size());
  for (size_t n = 0; n < message.circles.size(); ++n)
  {
    const std::string prefix = "circles[" + std::to_string(n) + "]";
    AddTextureCoordinateFields(fields, prefix + ".centerUV", message.circles[n].centerUV);
    AddField(fields, prefix + ".fRadius", message.circles[n].fRadius);
    AddField(fields, prefix + ".fInnerRadius", message.circles[n].fInnerRadius);
    AddField(fields, prefix + ".startAngle", message.circles[n].startAngle);
    AddField(fields, prefix + ".endAngle", message.circles[n].endAngle);
  }
  LogExtractedFields("SSymbolCircle", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolClone& message) const
{
  LogExtractedFields("SSymbolClone", {{"symbolID", ToLogString(message.symbolID)},
                                      {"sourceID", ToLogString(message.sourceID)},
                                      {"eSymbolSourceType", EnumValueToLogString(message.eSymbolSourceType)}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolControl& message) const
{
  TLogFields fields;
  AddField(fields, "symbolID", message.symbolID);
  AddEnumField(fields, "eSymbolState", message.eSymbolState);
  AddField(fields, "parentSymbolID", message.parentSymbolID);
  AddEnumField(fields, "eAttachState", message.eAttachState);
  AddEnumField(fields, "eFlashControl", message.eFlashControl);
  AddField(fields, "bInheritColor", message.bInheritColor);
  AddField(fields, "surfaceID", message.surfaceID);
  AddField(fields, "nLayerID", message.nLayerID);
  AddField(fields, "flashDutyCyclePercentage", message.flashDutyCyclePercentage);
  AddField(fields, "fFlashPeriod", message.fFlashPeriod);
  AddField(fields, "fPositionU", message.fPositionU);
  AddField(fields, "fPositionV", message.fPositionV);
  AddField(fields, "fRotation", message.fRotation);
  AddColorFields(fields, "color", message.color);
  AddField(fields, "fScaleU", message.fScaleU);
  AddField(fields, "fScaleV", message.fScaleV);
  AddField(fields, "bPositionSet", message.bPositionSet);
  AddField(fields, "bScaleSet", message.bScaleSet);
  AddField(fields, "bSetColor", message.bSetColor);
  LogExtractedFields("SSymbolControl", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolPolygon& message) const
{
  TLogFields fields;
  AddField(fields, "symbolID", message.symbolID);
  AddField(fields, "nStipplePattern", message.nStipplePattern);
  AddEnumField(fields, "ePrimitiveType", message.ePrimitiveType);
  AddField(fields, "fLineWidth", message.fLineWidth);
  AddField(fields, "fStipplePatternLength", message.fStipplePatternLength);
  AddField(fields, "vertices.size", message.vertices.size());
  for (size_t n = 0; n < message.vertices.size(); ++n)
  {
    AddVec2Fields(fields, "vertices[" + std::to_string(n) + "]", message.vertices[n]);
  }
  LogExtractedFields("SSymbolPolygon", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolTextDefinition& message) const
{
  LogExtractedFields("SSymbolTextDefinition", {{"symbolID", ToLogString(message.symbolID)},
                                               {"fontID", ToLogString(message.fontID)},
                                               {"fFontSize", ToLogString(message.fFontSize)},
                                               {"eTextAlignment", EnumValueToLogString(message.eTextAlignment)},
                                               {"eTextOrientation", EnumValueToLogString(message.eTextOrientation)},
                                               {"sText", message.sText}});
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolTexturedCircle& message) const
{
  TLogFields fields;
  AddField(fields, "symbolID", message.symbolID);
  AddField(fields, "textureID", message.textureID);
  AddEnumField(fields, "eTextureFilter", message.eTextureFilter);
  AddEnumField(fields, "eTextureWrap", message.eTextureWrap);
  AddField(fields, "circles.size", message.circles.size());
  for (size_t n = 0; n < message.circles.size(); ++n)
  {
    const std::string prefix = "circles[" + std::to_string(n) + "]";
    AddVec2Fields(fields, prefix + ".centerUV", message.circles[n].centerUV);
    AddField(fields, prefix + ".fRadius", message.circles[n].fRadius);
    AddField(fields, prefix + ".fInnerRadius", message.circles[n].fInnerRadius);
    AddField(fields, prefix + ".startAngle", message.circles[n].startAngle);
    AddField(fields, prefix + ".endAngle", message.circles[n].endAngle);
    AddVec2Fields(fields, prefix + ".centerTextureST", message.circles[n].centerTextureST);
    AddField(fields, prefix + ".fTextureMapRadius", message.circles[n].fTextureMapRadius);
    AddField(fields, prefix + ".fTextureMapRotation", message.circles[n].fTextureMapRotation);
  }
  LogExtractedFields("SSymbolTexturedCircle", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SSymbolTexturedPolygon& message) const
{
  TLogFields fields;
  AddField(fields, "symbolID", message.symbolID);
  AddField(fields, "textureID", message.textureID);
  AddEnumField(fields, "ePrimitiveType", message.ePrimitiveType);
  AddEnumField(fields, "eTextureFilterMode", message.eTextureFilterMode);
  AddEnumField(fields, "eTextureWrapMode", message.eTextureWrapMode);
  AddField(fields, "vertices.size", message.vertices.size());
  for (size_t n = 0; n < message.vertices.size(); ++n)
  {
    const std::string prefix = "vertices[" + std::to_string(n) + "]";
    AddVec2Fields(fields, prefix + ".uv", message.vertices[n].uv);
    AddVec2Fields(fields, prefix + ".textureCoordinateST", message.vertices[n].textureCoordinateST);
  }
  LogExtractedFields("SSymbolTexturedPolygon", fields);
}

void CCigiMessageLogger::LogMessageFromHostToIG(const SViewSymbolSurfaceDefinition& message) const
{
  TLogFields fields;
  AddSymbolSurfaceDefinitionFields(fields, message);
  AddField(fields, "viewID", message.viewID);
  AddField(fields, "fLeft", message.fLeft);
  AddField(fields, "fRight", message.fRight);
  AddField(fields, "fTop", message.fTop);
  AddField(fields, "fBottom", message.fBottom);
  LogExtractedFields("SViewSymbolSurfaceDefinition", fields);
}

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

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
