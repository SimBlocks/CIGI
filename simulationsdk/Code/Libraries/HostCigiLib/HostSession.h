//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostSession.h
 * @brief Declares the CHostSession class for managing CIGI host sessions and protocol communication.
 *
 * Provides the CHostSession class for managing host-to-IG sessions, packet processing, and protocol communication in a CIGI-based simulation.
 * Supports session initialization, packet sending/receiving, entity and environment control, and event-driven integration with the host emulator.
 * Includes configuration, connection, and logging management for simulation interoperability.
 *
 * @see sbio::cigi::host::CHostSession
 * @see sbio::cigi::host::IHostCigiEventListener
 * @see sbio::cigi::host::SHostSetupOptions
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HOST_SESSION_H
#define SIMBLOCKS_CIGI_HOST_SESSION_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostCigiLib/HostCigiLibTypes.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include "SymbolLib/SymbolTypes.h"
#include <list>
#include <memory>
#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      const int MAX_UDP_SIZE = 65507;

      /**
       * @brief Manages a CIGI host session, packet processing, and protocol communication.
       *
       * Supports session initialization, packet sending/receiving, entity and environment control, and event-driven integration with the host emulator.
       * The CIGI standard recommends having each session have a host communicate with 1 master IG.
       * A host may have multiple sessions.
       *
       * Responsibilities:
       * - Own transport sockets and packet timing helpers for one logical session.
       * - Track negotiated IG mode, database state, and frame counters.
       * - Expose version-independent send helpers that derived classes implement for a specific CIGI version.
       */
      class CHostSession : public sbio::cigi::host::IHostCigiEventListener
      {
      public:
        /**
         * @brief Constructs a `CHostSession` instance.
         */
        CHostSession();
        /**
         * @brief Destroys CHostSession instances.
         */
        virtual ~CHostSession();

        /// @name Session state queries
        /// @{
        /**
         * @brief Gets database number.
         * @return Database number value.
         */
        sbio::cigi::CigiDatabaseNumber GetDatabaseNumber() const;
        /**
         * @brief Gets database state.
         * @return Database state value.
         */
        EHostSessionDatabaseState GetDatabaseState() const;
        /**
         * @brief Gets actual igmode.
         * @return Actual igmode value.
         */
        sbio::cigi::EIGMode GetActualIGMode() const;
        /**
         * @brief Gets desired igmode.
         * @return Desired igmode value.
         */
        sbio::cigi::EIGMode GetDesiredIGMode() const;
        /**
         * @brief Gets frame count.
         * @return Frame count value.
         */
        int GetFrameCount() const;
        /**
         * @brief Gets logging enabled.
         * @return Logging enabled value.
         */
        bool GetLoggingEnabled() const;
        /**
         * @brief Gets session time.
         * @return Session time value.
         */
        double GetSessionTime() const;
        /**
         * @brief Gets session id.
         * @return Session id value.
         */
        sbio::SessionID GetSessionID() const;
        /// @}

        /**
         * @brief Initializes sockets, timers, and other per-session state.
         */
        void Initialize();

        /**
         * @brief Reports whether packets have recently been received from the IG.
          * @return `true` when the condition is met; otherwise `false`.
         */
        bool IsConnected() const;

        /**
         * @brief Consumes one inbound packet from the receive buffer.
         * @param buffer Pointer to the start of the packet payload.
         * @param nRemainingBytes Number of unread bytes remaining in the receive buffer.
         * @return Number of bytes consumed from `buffer`.
         */
        virtual int ProcessPacket(uint8_t* buffer, int nRemainingBytes) = 0;

        /**
         * @brief Determines how many bytes of queued outbound data form the next packet.
         * @param buffer Pointer to the next queued outbound bytes.
         * @param nRemainingBytes Number of unread bytes remaining in the send buffer.
         * @return Size in bytes of the next outbound packet.
         */
        virtual int GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const = 0;

        /**
         * @brief Receives and dispatches all currently available IG packets.
         * @return `true` when packet processing completes without a transport-level failure.
         */
        bool ProcessPackets();

        /**
         * @brief Clears transient session state while preserving configured connection options.
         */
        virtual void Reset();

        /**
         * @brief Sends the packets currently packed into the session's send buffer.
         */
        void SendPackets();

        /**
         * @brief Enables or disables packet logging for this session.
          * @param bEnabled Whether enabled.
         */
        void SetLoggingEnabled(bool bEnabled);

        /**
         * @brief Configures whether packet byte swapping is required for this session.
          * @param bEnabled Whether enabled.
         */
        void SetByteSwapEnabled(bool bEnabled);

        /**
         * @brief Assigns the logical identifier used to reference this session.
          * @param sessionID Session id value.
         */
        void SetSessionID(sbio::SessionID sessionID);

        /**
         * @brief Updates the desired IG mode and database-selection state for the session.
         * @return `true` when the request changed the tracked session state.
          * @param databaseNumber Database number value.
          * @param bEntityTypeSubstitutionEnabled Whether entity type substitution enabled.
          * @param eIGMode Igmode enumeration value.
          * @param bSmoothingEnabled Whether smoothing enabled.
         */
        virtual bool SetIGControl(sbio::cigi::CigiDatabaseNumber databaseNumber, bool bEntityTypeSubstitutionEnabled, sbio::cigi::EIGMode eIGMode, bool bSmoothingEnabled);

        /// @name Protocol-specific packet emission
        /// Derived classes override the following methods to serialize version-specific packets.
        /// @{
        /**
         * @brief Sends igcontrol.
         * @param pBuffer Buffer pointer used by the operation.
         */
        virtual void SendIGControl(uint8_t*& pBuffer) = 0;
        virtual void SendEntityControl(const sbio::cigi::SEntityControl& entityControl) {};
        virtual void SendArticulatedPartControl(const sbio::cigi::SCigiArticulatedPart& articulatedPart) {};
        virtual void SendEntityAccelerationControl(const sbio::cigi::SCigiEntityAcceleration& accelerationControl) {};
        virtual void SendArticulatedPartAccelerationControl(const sbio::cigi::SCigiArticulatedPartAcceleration& accelerationControl) {};
        virtual void SendTrajectoryDefinition(const sbio::cigi::SCigiEntityAcceleration& trajectoryDefinition) {};
        virtual void SendAnimationControl(const sbio::cigi::SCigiAnimationControl& animationControl) {};
        virtual void SendAtmosphereControl(const sbio::cigi::SCigiAtmosphereControl& atmosphereControl) {};
        virtual void SendCelestialSphereControl(const sbio::cigi::SCigiCelestialSphereControl& celestialSphereControl) {};
        virtual void SendChildEntityPosition(const sbio::cigi::SChildEntityPosition& childEntityPosition) {};
        virtual void SendCollisionDetectionSegment(const sbio::cigi::SCollisionDetectionSegmentDefinition& collDetSegment) {};
        virtual void SendCollisionDetectionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collVolCuboid) {};
        virtual void SendCollisionDetectionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collVolSphere) {};
        virtual void SendComponentControl(const sbio::cigi::SCigiComponentControl& componentControl) {};
        virtual void SendConformalClampedEntityPosition(const sbio::cigi::SCigiConformalClampedEntityPosition& conformalClampedEntityPos) {};
        virtual void SendEarthReferenceModelDefinition(const sbio::cigi::SCigiEarthReferenceModel& earthReferenceModel) {};
        virtual void SendEnvironmentalConditionsRequest(const sbio::cigi::SEnvironmentalConditionsRequest& environmentalConditionsRequest) {};
        virtual void SendEnvironmentalRegionControl(const sbio::cigi::SCigiEnvironmentalRegion& environmentalRegion) {};
        virtual void SendHatHotRequest(const sbio::cigi::SHATHOTGlobalRequest& hatHotRequest) {};
        virtual void SendHatHotRequest(const sbio::cigi::SHATHOTEntityRequest& hatHotRequest) {};
        virtual void SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& losRequest) {};
        virtual void SendLineOfSightSegmentRequestGeodeticToGeodeticExtended(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& losRequest) {};
        virtual void SendLineOfSightSegmentRequestGeodeticToEntityBasic(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& losRequest) {};
        virtual void SendLineOfSightSegmentRequestGeodeticToEntityExtended(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& losRequest) {};
        virtual void SendLineOfSightSegmentRequestEntityToGeodeticBasic(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& losRequest) {};
        virtual void SendLineOfSightSegmentRequestEntityToGeodeticExtended(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticExtended& losRequest) {};
        virtual void SendLineOfSightSegmentRequestEntityToEntityBasic(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& losRequest) {};
        virtual void SendLineOfSightSegmentRequestEntityToEntityExtended(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& losRequest) {};
        virtual void SendLineOfSightVectorRequestGeodeticBasic(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& losRequest) {};
        virtual void SendLineOfSightVectorRequestGeodeticExtended(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& losRequest) {};
        virtual void SendLineOfSightVectorRequestEntityBasic(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& losRequest) {};
        virtual void SendLineOfSightVectorRequestEntityExtended(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& losRequest) {};
        virtual void SendRegionMaritimeSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) {};
        virtual void SendEntityMaritimeSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) {};
        virtual void SendGlobalMaritimeSurfaceConditionsControl(const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) {};
        virtual void SendMotionTrackerViewControl(const sbio::cigi::SMotionTrackerViewControl& motionTrackerViewControl) {};
        virtual void SendMotionTrackerViewGroupControl(const sbio::cigi::SMotionTrackerViewGroupControl& motionTrackerViewGroupControl) {};
        virtual void SendPositionRequest(const sbio::cigi::SPositionRequest& positionRequest) {};
        virtual void SendRateControl(const sbio::cigi::SCigiEntityVelocityControl& rateControl) {};
        virtual void SendRateControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& rateControl) {};
        virtual void SendSensorControl(const sbio::cigi::SCigiSensorControl& sensorControl) {};
        virtual void SendShortArticulatedPartControl(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart) {};
        virtual void SendShortComponentControl(const sbio::cigi::SCigiShortComponentControl& shortComponentControl) {};
        virtual void SendShortSymbolControl(const sbio::symbol::SShortSymbolControl& shortSymbolControl) {};
        virtual void SendSymbolCircleDefinition(const sbio::symbol::SSymbolCircle& circleDef) {};
        virtual void SendSymbolClone(const sbio::symbol::SSymbolClone& symbolCloneStruct) {};
        virtual void SendSymbolControl(const sbio::symbol::SSymbolControl& symbolControl) {};
        virtual void SendSymbolLineDefinition(const sbio::symbol::SSymbolPolygon& symbolLine) {};
        virtual void SendSymbolPolygonDefinition(const sbio::symbol::SSymbolPolygon& symbolPolygon) {};
        virtual void SendEntitySymbolSurfaceDefinition(const sbio::symbol::SEntitySymbolSurfaceDefinition& entitySymbolSurfaceDefinition) {};
        virtual void SendEntityBillboardSymbolSurfaceDefinition(const sbio::symbol::SEntityBillboardSymbolSurfaceDefinition& entityBillboardSymbolSurfaceDefinition) {};
        virtual void SendViewSymbolSurfaceDefinition(const sbio::symbol::SViewSymbolSurfaceDefinition& viewSymbolSurfaceDefinition) {};
        virtual void SendSymbolTextDefinition(const sbio::symbol::SSymbolTextDefinition& symbolTextDef) {};
        virtual void SendSymbolTexturedCircleDefinition(const sbio::symbol::SSymbolTexturedCircle& symbolTexturedCircle) {};
        virtual void SendSymbolTexturedPolygonDefinition(const sbio::symbol::SSymbolTexturedPolygon& symbolTexturedPolygon) {};
        virtual void SendRegionTerrestrialSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) {};
        virtual void SendEntityTerrestrialSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) {};
        virtual void SendGlobalTerrestrialSurfaceConditionsControl(const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) {};
        virtual void SendTopLevelEntityPosition(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition) {};
        virtual void SendVelocityControl(const sbio::cigi::SCigiEntityVelocityControl& entityVelocityControl) {};
        virtual void SendVelocityControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& velocityControl) {};
        virtual void SendViewControl(const sbio::cigi::SCigiViewControl& viewControl) {};
        virtual void SendViewDefinition(const sbio::cigi::SCigiViewDefinition& viewDefinition) {};
        virtual void SendEntityWaveControl(EntityID entityID, const sbio::cigi::SCigiWaveCondition& waveCondition) {};
        virtual void SendRegionalWaveControl(RegionID regionID, const sbio::cigi::SCigiWaveCondition& waveCondition) {};
        virtual void SendGlobalWaveControl(const sbio::cigi::SCigiWaveCondition& waveCondition) {};

        virtual void SendWeatherControl(sbio::GlobalLayeredWeatherID globalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) {};
        virtual void SendWeatherControl(sbio::RegionID regionID, sbio::RegionalLayeredWeatherID regionlLayeredWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) {};
        virtual void SendWeatherControl(sbio::EntityID entityID, const sbio::cigi::SCigiWeatherCondition& weatherCondition) {};
        /// @}

        /**
         * @brief Appends a single packet to the send buffer.
         * @param packet Packet bytes to append.
         * @param nSize Number of bytes in `packet`.
         */
        void Pack(const void* packet, int nSize);

        /**
         * @brief Appends a base packet followed by its record payload to the send buffer.
          * @param basePacket CIGI packet value.
          * @param nBasePacketSize Base packet size numeric value.
          * @param recordsPpacket CIGI packet value.
          * @param nRecordsPacketSize Records packet size numeric value.
         */
        void Pack(const void* basePacket, int nBasePacketSize, const void* recordsPpacket, int nRecordsPacketSize);

        SHostSetupOptions hostSetupOptions;
        sbio::FrameNumber m_LastReceivedIGFrame = sbio::FrameNumber(0);

      protected:
        /**
         * @brief Remembers the requested response coordinate system for a line-of-sight request.
          * @param requestID Request id value.
          * @param eResponseCoordinateSystem Response coordinate system enumeration value.
         */
        void StoreLineOfSightRequestCoordinateSystem(sbio::LineOfSightRequestID requestID, sbio::ETopLevelCoordinateSystem eResponseCoordinateSystem);

        /**
         * @brief Looks up the coordinate system previously associated with a line-of-sight request.
          * @param requestID Request id value.
          * @return Line of sight request coordinate system value.
         */
        sbio::ETopLevelCoordinateSystem GetLineOfSightRequestCoordinateSystem(sbio::LineOfSightRequestID requestID) const;

        /**
         * @brief Dispatches the start-of-frame event after the corresponding packet is parsed.
         */
        void NotifyStartOfFrameReceived();

        /**
         * @brief Queues a packet for later transmission when it does not fit in the active send buffer.
          * @param packet CIGI packet value.
          * @param nSize Size numeric value.
         */
        void QueueOverflowPacket(const void* packet, int nSize);

        /**
         * @brief Moves previously overflowed packets back into the active send buffer.
         */
        void MoveQueuedPacketsToSendBuffer();

        bool m_bConnected = false;
        bool m_bHasReportedWaitingForConnection = false;
        std::unique_ptr<sbio::utils::CStopWatch> m_pDisconnectedTimer;
        sbio::FrameNumber m_HostFrameNumber = sbio::FrameNumber(0);//host frame number is unique to each session
        sbio::SessionID m_SessionID = sbio::SessionID(0);

        sbio::cigi::EIGMode m_DesiredIGMode = sbio::cigi::EIGMode::UNKNOWN;
        sbio::cigi::EIGMode m_ActualIGMode = sbio::cigi::EIGMode::UNKNOWN;

        EHostSessionDatabaseState m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;

        int m_nSendBufferLength = 0;
        char m_sendBuffer[MAX_UDP_SIZE];//holds packed packets to be sent
        std::list<std::unique_ptr<sbio::utils::TBuffer<char>>> m_OverflowBuffers;

        sbio::cigi::CigiDatabaseNumber m_DatabaseNumber = sbio::cigi::UnknownCigiDatabaseNumber;

        std::unique_ptr<sbio::utils::CUDPSendSocket> m_pSocketHostToIG;
        std::unique_ptr<sbio::utils::CUDPReceiveSocket> m_pSocketIGToHost;

        std::unique_ptr<sbio::utils::CStopWatch> m_pSessionStopWatch;

        bool m_bLoggingEnabled = true;
        bool m_bByteSwap = false;
        std::unordered_map<sbio::LineOfSightRequestID, sbio::ETopLevelCoordinateSystem, StrongTypeHash<sbio::LineOfSightRequestID>> m_LineOfSightRequestCoordinateSystems;
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
