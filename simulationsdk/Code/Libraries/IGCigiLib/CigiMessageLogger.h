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
#include <vector>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Writes formatted CIGI message and packet information to a log file.
       *
       * Supports raw host-to-IG and IG-to-host packet logging as well as logging
       * extracted SDK message structures.
       */
      class CCigiMessageLogger
      {
      public:
        /**
         * @brief Constructs a message logger for an application data directory.
         * @param applicationDataPath Application data directory used to determine the log file path.
         */
        explicit CCigiMessageLogger(const std::filesystem::path& applicationDataPath);

        /** @brief Gets the path of the message log file.
         * @return Path of the message log file.
         */
        const std::filesystem::path& GetLogFilePath() const;
        /** @brief Clears the message log file.
         */
        void ClearLog() const;
        /** @brief Sets the frame number written with subsequent log entries.
         * @param frameNumber Frame number to associate with subsequent entries.
         */
        void SetFrameNumber(sbio::FrameNumber frameNumber);

        /** @brief Logs a raw host-to-IG message.
         * @param eCigiVersion CIGI protocol version of the message.
         * @param endpoint Host endpoint associated with the message.
         * @param pBuffer Message data to log.
         * @param nMessageSize Number of bytes in `pBuffer`.
         */
        void LogMessageFromHostToIG(sbio::cigi::ECigiVersion eCigiVersion, const std::string& endpoint, const uint8_t* pBuffer, int nMessageSize) const;
        /** @brief Logs a raw IG-to-host message.
         * @param eCigiVersion CIGI protocol version of the message.
         * @param pBuffer Message data to log.
         * @param nMessageSize Number of bytes in `pBuffer`.
         */
        void LogMessageFromIGToHost(sbio::cigi::ECigiVersion eCigiVersion, const uint8_t* pBuffer, int nMessageSize) const;

        /// @name Extracted host-to-IG message logging
        /// Logs the supplied host-to-IG SDK message structure.
        /// @{
        /** @brief Logs an IG control message.
         * @param igControl IG control message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiIgControl& igControl) const;
        /** @brief Logs a top-level entity position message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::STopLevelEntityPosition& message) const;
        /** @brief Logs a child entity position message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SChildEntityPosition& message) const;
        /** @brief Logs an atmosphere message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SAtmosphere& message) const;
        /** @brief Logs a celestial-sphere message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCelestialSphere& message) const;
        /** @brief Logs an articulated-part message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiArticulatedPart& message) const;
        /** @brief Logs a component-control message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiComponentControl& message) const;
        /** @brief Logs an earth-reference-model message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiEarthReferenceModel& message) const;
        /** @brief Logs an environmental-region message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiEnvironmentalRegion& message) const;
        /** @brief Logs a sensor-control message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiSensorControl& message) const;
        /** @brief Logs a short articulated-part message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiShortArticulatedPart& message) const;
        /** @brief Logs a view-control message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiViewControl& message) const;
        /** @brief Logs a view-definition message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiViewDefinition& message) const;
        /** @brief Logs a wave-condition message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SCigiWaveCondition& message) const;
        /** @brief Logs an environmental-conditions request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SEnvironmentalConditionsRequest& message) const;
        /** @brief Logs a HAT/HOT entity request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SHATHOTEntityRequest& message) const;
        /** @brief Logs a HAT/HOT global request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SHATHOTGlobalRequest& message) const;
        /** @brief Logs an entity-to-entity basic line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& message) const;
        /** @brief Logs an entity-to-entity extended line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& message) const;
        /** @brief Logs an entity-to-geodetic basic line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& message) const;
        /** @brief Logs an entity-to-geodetic extended line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticExtended& message) const;
        /** @brief Logs a geodetic-to-entity basic line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& message) const;
        /** @brief Logs a geodetic-to-entity extended line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& message) const;
        /** @brief Logs a geodetic-to-geodetic basic line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& message) const;
        /** @brief Logs a geodetic-to-geodetic extended line-of-sight segment request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& message) const;
        /** @brief Logs an entity-coordinate basic line-of-sight vector request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& message) const;
        /** @brief Logs an entity-coordinate extended line-of-sight vector request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& message) const;
        /** @brief Logs a geodetic-coordinate basic line-of-sight vector request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& message) const;
        /** @brief Logs a geodetic-coordinate extended line-of-sight vector request.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& message) const;
        /** @brief Logs a motion-tracker view-control message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SMotionTrackerViewControl& message) const;
        /** @brief Logs a motion-tracker view-group control message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::cigi::SMotionTrackerViewGroupControl& message) const;
        /** @brief Logs an entity billboard symbol-surface definition.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SEntityBillboardSymbolSurfaceDefinition& message) const;
        /** @brief Logs an entity symbol-surface definition.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SEntitySymbolSurfaceDefinition& message) const;
        /** @brief Logs a symbol-circle message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolCircle& message) const;
        /** @brief Logs a symbol-clone message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolClone& message) const;
        /** @brief Logs a symbol-control message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolControl& message) const;
        /** @brief Logs a symbol-polygon message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolPolygon& message) const;
        /** @brief Logs a symbol-text definition.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolTextDefinition& message) const;
        /** @brief Logs a textured symbol-circle message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolTexturedCircle& message) const;
        /** @brief Logs a textured symbol-polygon message.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SSymbolTexturedPolygon& message) const;
        /** @brief Logs a view symbol-surface definition.
         * @param message Message to log.
         */
        void LogMessageFromHostToIG(const sbio::symbol::SViewSymbolSurfaceDefinition& message) const;
        /// @}

        /// @name Extracted IG-to-host message logging
        /// Logs the supplied IG-to-host SDK message structure.
        /// @{
        /** @brief Logs a start-of-frame message.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SCigiStartOfFrame& message) const;
        /** @brief Logs an animation-stop notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SAnimationStopNotification& message) const;
        /** @brief Logs a height-above-terrain response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SHeightAboveTerrainResponse& message) const;
        /** @brief Logs a height-of-terrain response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SHeightOfTerrainResponse& message) const;
        /** @brief Logs a HAT/HOT extended response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SHATHOTExtendedResponse& message) const;
        /** @brief Logs a collision-detection segment notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionSegmentNotification& message) const;
        /** @brief Logs an entity collision-detection segment notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& message) const;
        /** @brief Logs a collision-detection volume notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionVolumeNotification& message) const;
        /** @brief Logs an entity collision-detection volume notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& message) const;
        /** @brief Logs an event notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SEventNotification& message) const;
        /** @brief Logs an image-generator notification.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SImageGeneratorNotification& message) const;
        /** @brief Logs a line-of-sight response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightResponse& message) const;
        /** @brief Logs an entity line-of-sight response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightEntityResponse& message) const;
        /** @brief Logs a geodetic-coordinate extended line-of-sight response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& message) const;
        /** @brief Logs an entity-coordinate extended line-of-sight response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& message) const;
        /** @brief Logs an entity-to-geodetic extended line-of-sight response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& message) const;
        /** @brief Logs a geodetic-coordinate position response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SPositionResponseGeodeticCoordinates& message) const;
        /** @brief Logs a parent-entity-coordinate position response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SPositionResponseParentEntityCoordinates& message) const;
        /** @brief Logs an articulated-part-coordinate position response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SPositionResponseArticulatedPartCoordinates& message) const;
        /** @brief Logs a weather-conditions response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SWeatherConditionsResponse& message) const;
        /** @brief Logs an aerosol-concentration response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SAerosolConcentrationResponse& message) const;
        /** @brief Logs a maritime-surface-conditions response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SMaritimeSurfaceConditionsResponse& message) const;
        /** @brief Logs a terrestrial-surface-conditions response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::STerrestrialSurfaceConditionsResponse& message) const;
        /** @brief Logs a sensor response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SSensorResponse& message) const;
        /** @brief Logs an extended sensor response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SSensorExtendedResponse& message) const;
        /** @brief Logs an extended entity sensor response.
         * @param message Message to log.
         */
        void LogMessageFromIGToHost(const sbio::cigi::SSensorExtendedEntityResponse& message) const;
        /// @}

      private:
        /**
         * @brief Defaults to false when const T cannot be inserted into an output stream.
         * @tparam T Message type tested for stream insertion.
         */
        template <typename T, typename = void>
        struct IsStreamable : std::false_type
        {
        };

        /**
         * @brief Selects true when stream insertion of const T is well-formed.
         * @tparam T Streamable message type.
         */
        template <typename T>
        struct IsStreamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>> : std::true_type
        {
        };

        /** @brief Logs an extracted message.
         * @tparam T Message type
         * @param messageName Name of the message.
         * @param message The message to log.
         */
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

          logFile << GetTimestampString() << " Host->IG frame = " << m_FrameNumber.Value() << " message = " << messageName << std::endl;
          logFile << NormalizeNameValueSpacing(ss.str()) << std::endl;
        }

        /** @brief Gets the current timestamp as a string.
         * @return The current timestamp string.
         */
        std::string GetTimestampString() const;

        /** @brief Converts a buffer to a hexadecimal string.
         * @param pBuffer Pointer to the buffer.
         * @param nSize Size of the buffer.
         * @return The hexadecimal string representation of the buffer.
         */
        std::string ToHexString(const uint8_t* pBuffer, int nSize) const;

        /** @brief Normalizes the spacing between names and values in a string.
         * @param value The string to normalize.
         * @return The normalized string.
         */
        std::string NormalizeNameValueSpacing(const std::string& value) const;

        /** @brief Logs a message to the log file.
         * @param direction Direction of the message.
         * @param eCigiVersion Cigi version.
         * @param endpoint Endpoint of the message.
         * @param pBuffer Pointer to the message buffer.
         * @param nMessageSize Size of the message buffer.
         */
        void LogMessage(const std::string& direction, sbio::cigi::ECigiVersion eCigiVersion, const std::string& endpoint, const uint8_t* pBuffer, int nMessageSize) const;

        /** @brief Logs extracted message fields to the log file.
         * @param messageName Name of the message.
         * @param fields Initializer list of name-value pairs representing the extracted fields.
         */
        void LogExtractedFields(const std::string& messageName, std::initializer_list<std::pair<std::string, std::string>> fields) const;

        /** @brief Logs extracted message fields to the log file.
         * @param messageName Name of the message.
         * @param fields Vector of name-value pairs representing the extracted fields.
         */
        void LogExtractedFields(const std::string& messageName, const std::vector<std::pair<std::string, std::string>>& fields) const;

        /** @brief Logs raw packet data to the log file.
         * @param logFile Output file stream.
         * @param eCigiVersion Cigi version.
         * @param pBuffer Pointer to the packet data.
         * @param nMessageSize Size of the packet data.
         */
        void LogPackets(std::ofstream& logFile, sbio::cigi::ECigiVersion eCigiVersion, const uint8_t* pBuffer, int nMessageSize) const;

        std::filesystem::path m_LogFilePath;///< Path of the message log file.
        sbio::FrameNumber m_FrameNumber = sbio::UnknownFrameNumber;///< Frame number associated with log entries.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
