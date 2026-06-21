//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_CIGI_MESSAGE_LOGGER_H
#define SIMBLOCKS_CIGI_MESSAGE_LOGGER_H

#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "CigiLib/CigiTypesIGToHost.h"
#include "GlobalHeaders/CommonTypes.h"
#include "SymbolLib/SymbolTypes.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      class CCigiMessageLogger
      {
      public:
        explicit CCigiMessageLogger(const std::filesystem::path& applicationDataPath);

        const std::filesystem::path& GetLogFilePath() const;
        void ClearLog() const;
        void SetFrameNumber(sbio::FrameNumber frameNumber);

        void LogMessageFromHostToIG(sbio::cigi::ECigiVersion eCigiVersion, const std::string& endpoint, const uint8_t* pBuffer, int nMessageSize) const;
        void LogMessageFromIGToHost(sbio::cigi::ECigiVersion eCigiVersion, const uint8_t* pBuffer, int nMessageSize) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiIgControl& igControl) const;
        void LogMessageFromHostToIG(const sbio::cigi::SAtmosphere& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCelestialSphere& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiArticulatedPart& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiComponentControl& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiEarthReferenceModel& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiEnvironmentalRegion& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiSensorControl& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiShortArticulatedPart& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiViewControl& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiViewDefinition& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SCigiWaveCondition& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SEnvironmentalConditionsRequest& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SHATHOTEntityRequest& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SHATHOTGlobalRequest& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticExtended& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SMotionTrackerViewControl& message) const;
        void LogMessageFromHostToIG(const sbio::cigi::SMotionTrackerViewGroupControl& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SEntityBillboardSymbolSurfaceDefinition& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SEntitySymbolSurfaceDefinition& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolCircle& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolClone& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolControl& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolPolygon& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolTextDefinition& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolTexturedCircle& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolTexturedPolygon& message) const;
        void LogMessageFromHostToIG(const sbio::symbol::SViewSymbolSurfaceDefinition& message) const;

        void LogMessageFromIGToHost(const sbio::cigi::SCigiStartOfFrame& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SAnimationStopNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SHeightAboveTerrainResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SHeightOfTerrainResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SHATHOTExtendedResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionSegmentNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionVolumeNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SEventNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SImageGeneratorNotification& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightEntityResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SPositionResponseGeodeticCoordinates& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SPositionResponseParentEntityCoordinates& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SPositionResponseArticulatedPartCoordinates& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SWeatherConditionsResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SAerosolConcentrationResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SMaritimeSurfaceConditionsResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::STerrestrialSurfaceConditionsResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SSensorResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SSensorExtendedResponse& message) const;
        void LogMessageFromIGToHost(const sbio::cigi::SSensorExtendedEntityResponse& message) const;

      private:
        template <typename T, typename = void>
        struct IsStreamable : std::false_type
        {
        };

        template <typename T>
        struct IsStreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> : std::true_type
        {
        };

        template <typename T>
        void LogExtractedMessageImpl(const std::string& messageName, const T& message) const
        {
          std::ofstream logFile(m_LogFilePath, std::ios::app);
          if (!logFile.is_open())
          {
            return;
          }

          std::ostringstream ss;
          if constexpr (IsStreamable<T>::value)
          {
            ss << message;
          }
          else
          {
            ss << "<no stream operator available for extracted message type>";
          }
          logFile << GetTimestampString() << " Host->IG frame = " << m_FrameNumber.Value() << " message = " << messageName << std::endl;
          logFile << ss.str() << std::endl;
        }

        std::string GetTimestampString() const;
        std::string ToHexString(const uint8_t* pBuffer, int nSize) const;
        void LogMessage(const std::string& direction, sbio::cigi::ECigiVersion eCigiVersion, const std::string& endpoint, const uint8_t* pBuffer, int nMessageSize) const;
        void LogExtractedFields(const std::string& messageName, std::initializer_list<std::pair<std::string, std::string>> fields) const;
        void LogPackets(std::ofstream& logFile, sbio::cigi::ECigiVersion eCigiVersion, const uint8_t* pBuffer, int nMessageSize) const;

        std::filesystem::path m_LogFilePath;
        sbio::FrameNumber m_FrameNumber = sbio::UnknownFrameNumber;
      };
    }
  }
}

#endif
//Usage of this software is prohibited except through a Software Licensing Agreement with SimBlocks LLC.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
