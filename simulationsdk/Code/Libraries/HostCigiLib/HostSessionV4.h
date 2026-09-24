//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostSessionV4.h
 * @brief Declares the CHostSessionV4 class for managing CIGI 4.0 host sessions and protocol communication.
 *
 * Provides the CHostSessionV4 class for managing host-to-IG sessions, packet processing, and protocol communication
 * using the CIGI 4.0 protocol. Supports session initialization, packet sending/receiving, entity and environment control,
 * and event-driven integration with the host emulator. Includes CIGI 4.0-specific packet parsing and handler logic.
 *
 * @see sbio::cigi::host::CHostSessionV4
 * @see sbio::cigi::host::CHostSession
 * @see sbio::cigi::host::IHostCigiEventListener
 * @see sbio::cigi::host::SHostSetupOptions
 */
#pragma once

#ifndef SIMBLOCKS_CIGI_HOST_SESSION_V4_H
#define SIMBLOCKS_CIGI_HOST_SESSION_V4_H

#include "HostCigiLib/HostCigiEvent.h"
#include "HostSession.h"
#include "libCIGI/Packets/4_0/IGCtrl.h"
#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Manages a CIGI 4.0 host session, packet processing, and protocol communication.
       *
       * Supports session initialization, packet sending/receiving, entity and environment control, and event-driven integration with the host emulator.
       * Includes CIGI 4.0-specific packet parsing and handler logic.
       * This specialization implements the packet layouts and capabilities required for
       * communication with CIGI 4.0 image generators.
       *
       * Send helpers serialize data into the session queues; `SendPackets()` performs UDP transmission.
       * `SendIGControl()` instead writes directly to a supplied buffer. Input references are not retained;
       * void send helpers do not report queue acceptance or delivery. Methods not overridden here retain
       * the base class's no-op behavior, including legacy rate, trajectory, and symbol-line helpers.
       */
      class CHostSessionV4 : public CHostSession
      {
      public:
        /**
         * @brief Constructs a `CHostSessionV4` instance.
         */
        CHostSessionV4();

        /// @name Packet parsing and serialization overrides
        /// @{
        /**
         * @brief Validates a CIGI 4.0 header and dispatches one recognized response packet.
         * @param buffer Readable packet bytes beginning with the header; not retained.
         * @param nRemainingBytes Number of bytes available at `buffer`.
         * @return Declared packet size, or zero for a missing header or invalid declared length.
         *         Unsupported opcodes and undersized known payloads are skipped with a positive size
         *         and a diagnostic event; only recognized, sufficiently sized packets mark valid traffic.
         */
        virtual int ProcessPacket(uint8_t* buffer, int nRemainingBytes) override;
        /**
         * @brief Reads the length of a queued CIGI 4.0 packet using the configured byte order.
         * @param buffer Readable packet bytes beginning with the header.
         * @param nRemainingBytes Number of available bytes.
         * @return Declared size, or zero if the header is incomplete, the size is below the header size,
         *         or the size exceeds available bytes. Does not validate opcode-specific payload fields.
         */
        virtual int GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const override;
        /**
         * @brief Resets the CIGI 4.0 session state.
         *
         * Performs the base reset (including socket closure) and restores the cached IG Control packet,
         * explicitly clearing its reserved fields.
         */
        virtual void Reset() override;
        /**
         * @brief Writes CIGI 4.0 IG Control with current frame counters, timestamp, and database handshake state.
         * @param pBuffer Writable cursor with room for `sizeof(CIGI::V40::IGCtrl)` bytes; advanced by that size.
         *                Applies the configured byte order; does not queue or transmit the packet.
         */
        virtual void SendIGControl(uint8_t*& pBuffer) override;
        /// @}

        /// @name Response packet parsers
        /// These helpers decode individual inbound CIGI 4.0 packets and raise host events.
        /// Callers must supply a complete packet, including its header and declared variable data.
        /// These helpers do not validate buffer capacity; use `ProcessPacket()` for length checks.
        /// Packet bytes are copied and decoded using the session byte-swap flag, not retained.
        /// @{
        /**
         * @brief Decodes start-of-frame state, raises its event, updates database state, and handles synchronous sending.
         * @param buffer Complete CIGI 4.0 Start of Frame packet.
         */
        void ParseStartOfFramePacket(uint8_t* buffer);
        /**
         * @brief Decodes a terrain-height response and raises a HAT or HOT event.
         * @param buffer Complete CIGI 4.0 HAT/HOT Response packet.
         */
        void ParseHatHotResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes terrain heights and surface data and raises an extended HAT/HOT event.
         * @param buffer Complete CIGI 4.0 HAT/HOT Extended Response packet.
         */
        void ParseHatHotExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes a line-of-sight response and selects the entity or non-entity event variant.
         * @param buffer Complete CIGI 4.0 Line of Sight Response packet.
         */
        void ParseLineOfSightResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes an extended line-of-sight response using the recorded request coordinate system.
         * @param buffer Complete CIGI 4.0 Line of Sight Extended Response packet.
         *
         * A response with no recorded coordinate system is ignored, with a warning when a logger is available.
         */
        void ParseLineOfSightExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes sensor tracking state and raises a sensor-response event.
         * @param buffer Complete CIGI 4.0 Sensor Response packet.
         */
        void ParseSensorResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes extended tracking data and selects the entity or non-entity sensor event variant.
         * @param buffer Complete CIGI 4.0 Sensor Extended Response packet.
         */
        void ParseSensorExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes object position and raises a position event with a borrowed response payload.
         * @param buffer Complete CIGI 4.0 Position Response packet.
         */
        void ParsePositionResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried weather conditions and raises the corresponding response event.
         * @param buffer Complete CIGI 4.0 Weather Conditions Response packet.
         */
        void ParseWeatherConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried aerosol concentration and raises the corresponding response event.
         * @param buffer Complete CIGI 4.0 Aerosol Concentration Response packet.
         */
        void ParseAerosolConcentrationResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried maritime conditions and raises the corresponding response event.
         * @param buffer Complete CIGI 4.0 Maritime Surface Conditions Response packet.
         */
        void ParseMaritimeSurfaceConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried terrestrial conditions and raises the corresponding response event.
         * @param buffer Complete CIGI 4.0 Terrestrial Surface Conditions Response packet.
         */
        void ParseTerrestrialSurfaceConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes a segment collision and selects the entity or non-entity notification variant.
         * @param buffer Complete CIGI 4.0 Collision Detection Segment Notification packet.
         */
        void ParseCollisionDetectionSegmentNotificationPacket(uint8_t* buffer);
        /**
         * @brief Decodes a volume collision and selects the entity or non-entity notification variant.
         * @param buffer Complete CIGI 4.0 Collision Detection Volume Notification packet.
         */
        void ParseCollisionDetectionVolumeNotificationPacket(uint8_t* buffer);
        /**
         * @brief Decodes an entity animation-stop notification and raises its event.
         * @param buffer Complete CIGI 4.0 Animation Stop Notification packet.
         */
        void ParseAnimationStopNotificationPacket(uint8_t* buffer);
        /**
         * @brief Decodes an event identifier and three data words and raises an IG event notification.
         * @param buffer Complete CIGI 4.0 Event Notification packet.
         */
        void ParseEventNotificationPacket(uint8_t* buffer);
        /**
         * @brief Copies an IG message identifier and declared text bytes into a message event.
         * @param buffer Complete CIGI 4.0 IG Message packet, including any text payload.
         */
        void ParseImageGeneratorMessagePacket(uint8_t* buffer);
        /// @}

        /**
         * @brief Sends entity control.
         * @param entityControl Entity control value.
         */
        virtual void SendEntityControl(const sbio::cigi::SEntityControl& entityControl) override;
        /**
         * @brief Sends articulated part control.
         * @param articulatedPart Articulated part value.
         */
        virtual void SendArticulatedPartControl(const sbio::cigi::SCigiArticulatedPart& articulatedPart) override;
        /**
         * @brief Sends entity acceleration control.
         * @param accelerationControl Acceleration control value.
         */
        virtual void SendEntityAccelerationControl(const sbio::cigi::SCigiEntityAcceleration& accelerationControl) override;
        /**
         * @brief Sends articulated part acceleration control.
         * @param accelerationControl Acceleration control value.
         */
        virtual void SendArticulatedPartAccelerationControl(const sbio::cigi::SCigiArticulatedPartAcceleration& accelerationControl) override;
        /**
         * @brief Sends animation control.
         * @param animationControl Animation control value.
         */
        virtual void SendAnimationControl(const sbio::cigi::SCigiAnimationControl& animationControl) override;
        /**
         * @brief Sends atmosphere control.
         * @param atmosphereControl Atmosphere control value.
         */
        virtual void SendAtmosphereControl(const sbio::cigi::SCigiAtmosphereControl& atmosphereControl) override;
        /**
         * @brief Sends celestial sphere control.
         * @param celestialSphereControl Celestial sphere control value.
         */
        virtual void SendCelestialSphereControl(const sbio::cigi::SCigiCelestialSphereControl& celestialSphereControl) override;
        /**
         * @brief Sends child entity position.
         * @param childEntityPosition Child entity position value.
         */
        virtual void SendChildEntityPosition(const sbio::cigi::SChildEntityPosition& childEntityPosition) override;
        /**
         * @brief Sends collision detection segment.
         * @param collDetSegment Coll det segment value.
         */
        virtual void SendCollisionDetectionSegment(const sbio::cigi::SCollisionDetectionSegmentDefinition& collDetSegment) override;
        /**
         * @brief Sends collision detection cuboid volume.
         * @param collVolCuboid Coll vol cuboid value.
         */
        virtual void SendCollisionDetectionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collVolCuboid) override;
        /**
         * @brief Sends collision detection sphere volume.
         * @param collVolSphere Coll vol sphere value.
         */
        virtual void SendCollisionDetectionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collVolSphere) override;
        /**
         * @brief Sends component control.
         * @param componentControl Component control value.
         */
        virtual void SendComponentControl(const sbio::cigi::SCigiComponentControl& componentControl) override;
        /**
         * @brief Sends conformal clamped entity position.
         * @param conformalClampedEntityPos Conformal clamped entity pos value.
         */
        virtual void SendConformalClampedEntityPosition(const sbio::cigi::SCigiConformalClampedEntityPosition& conformalClampedEntityPos) override;
        /**
         * @brief Sends earth reference model definition.
         * @param earthReferenceModel Earth reference model value.
         */
        virtual void SendEarthReferenceModelDefinition(const sbio::cigi::SCigiEarthReferenceModel& earthReferenceModel) override;
        /**
         * @brief Sends environmental conditions request.
         * @param environmentalConditionsRequest Environmental conditions request value.
         */
        virtual void SendEnvironmentalConditionsRequest(const sbio::cigi::SEnvironmentalConditionsRequest& environmentalConditionsRequest) override;
        /**
         * @brief Sends environmental region control.
         * @param environmentalRegion Environmental region value.
         */
        virtual void SendEnvironmentalRegionControl(const sbio::cigi::SCigiEnvironmentalRegion& environmentalRegion) override;
        /**
         * @brief Sends hat hot request.
         * @param hatHotRequest Hat hot request value.
         */
        virtual void SendHatHotRequest(const sbio::cigi::SHATHOTGlobalRequest& hatHotRequest) override;
        /**
         * @brief Sends hat hot request.
         * @param hatHotRequest Hat hot request value.
         */
        virtual void SendHatHotRequest(const sbio::cigi::SHATHOTEntityRequest& hatHotRequest) override;
        /**
         * @brief Sends line of sight segment request geodetic to geodetic basic.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& losRequest) override;
        /**
         * @brief Sends line of sight segment request geodetic to geodetic extended.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToGeodeticExtended(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& losRequest) override;
        /**
         * @brief Sends line of sight segment request geodetic to entity basic.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToEntityBasic(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& losRequest) override;
        /**
         * @brief Sends line of sight segment request geodetic to entity extended.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToEntityExtended(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& losRequest) override;
        /**
         * @brief Sends line of sight segment request entity to geodetic basic.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestEntityToGeodeticBasic(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& losRequest) override;
        /**
         * @brief Sends line of sight segment request entity to geodetic extended.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestEntityToGeodeticExtended(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticExtended& losRequest) override;
        /**
         * @brief Sends line of sight segment request entity to entity basic.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestEntityToEntityBasic(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& losRequest) override;
        /**
         * @brief Sends line of sight segment request entity to entity extended.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightSegmentRequestEntityToEntityExtended(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& losRequest) override;
        /**
         * @brief Sends line of sight vector request geodetic basic.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightVectorRequestGeodeticBasic(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& losRequest) override;
        /**
         * @brief Sends line of sight vector request geodetic extended.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightVectorRequestGeodeticExtended(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& losRequest) override;
        /**
         * @brief Sends line of sight vector request entity basic.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightVectorRequestEntityBasic(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& losRequest) override;
        /**
         * @brief Sends line of sight vector request entity extended.
         * @param losRequest Los request value.
         */
        virtual void SendLineOfSightVectorRequestEntityExtended(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& losRequest) override;
        /**
         * @brief Sends region maritime surface conditions control.
         * @param regionID Region id value.
         * @param maritimeSurfaceCondition Maritime surface condition value.
         */
        virtual void SendRegionMaritimeSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) override;
        /**
         * @brief Sends entity maritime surface conditions control.
         * @param entityID Entity id value.
         * @param maritimeSurfaceCondition Maritime surface condition value.
         */
        virtual void SendEntityMaritimeSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) override;
        /**
         * @brief Sends global maritime surface conditions control.
         * @param maritimeSurfaceCondition Maritime surface condition value.
         */
        virtual void SendGlobalMaritimeSurfaceConditionsControl(const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) override;
        /**
         * @brief Sends motion tracker view control.
         * @param motionTrackerViewControl Motion tracker view control value.
         */
        virtual void SendMotionTrackerViewControl(const sbio::cigi::SMotionTrackerViewControl& motionTrackerViewControl) override;
        /**
         * @brief Sends motion tracker view group control.
         * @param motionTrackerViewGroupControl Motion tracker view group control value.
         */
        virtual void SendMotionTrackerViewGroupControl(const sbio::cigi::SMotionTrackerViewGroupControl& motionTrackerViewGroupControl) override;
        /**
         * @brief Sends position request.
         * @param positionRequest Position request value.
         */
        virtual void SendPositionRequest(const sbio::cigi::SPositionRequest& positionRequest) override;
        /**
         * @brief Sends sensor control.
         * @param sensorControl Sensor control value.
         */
        virtual void SendSensorControl(const sbio::cigi::SCigiSensorControl& sensorControl) override;
        /**
         * @brief Sends short articulated part control.
         * @param shortArticulatedPart Short articulated part value.
         */
        virtual void SendShortArticulatedPartControl(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart) override;
        /**
         * @brief Sends short component control.
         * @param shortComponentControl Short component control value.
         */
        virtual void SendShortComponentControl(const sbio::cigi::SCigiShortComponentControl& shortComponentControl) override;
        /**
         * @brief Sends short symbol control.
         * @param shortSymbolControl Short symbol control value.
         */
        virtual void SendShortSymbolControl(const sbio::symbol::SShortSymbolControl& shortSymbolControl) override;
        /**
         * @brief Sends symbol circle definition.
         * @param circleDef Circle def value.
         */
        virtual void SendSymbolCircleDefinition(const sbio::symbol::SSymbolCircle& circleDef) override;
        /**
         * @brief Sends symbol clone.
         * @param symbolCloneStruct Symbol clone struct value.
         */
        virtual void SendSymbolClone(const sbio::symbol::SSymbolClone& symbolCloneStruct) override;
        /**
         * @brief Sends symbol control.
         * @param symbolControl Symbol control value.
         */
        virtual void SendSymbolControl(const sbio::symbol::SSymbolControl& symbolControl) override;
        /**
         * @brief Sends symbol polygon definition.
         * @param symbolPolygon Symbol polygon value.
         */
        virtual void SendSymbolPolygonDefinition(const sbio::symbol::SSymbolPolygon& symbolPolygon) override;
        /**
         * @brief Sends entity symbol surface definition.
         * @param entitySymbolSurfaceDefinition Entity symbol surface definition value.
         */
        virtual void SendEntitySymbolSurfaceDefinition(const sbio::symbol::SEntitySymbolSurfaceDefinition& entitySymbolSurfaceDefinition) override;
        /**
         * @brief Sends entity billboard symbol surface definition.
         * @param entityBillboardSymbolSurfaceDefinition Entity billboard symbol surface definition value.
         */
        virtual void SendEntityBillboardSymbolSurfaceDefinition(const sbio::symbol::SEntityBillboardSymbolSurfaceDefinition& entityBillboardSymbolSurfaceDefinition) override;
        /**
         * @brief Sends view symbol surface definition.
         * @param viewSymbolSurfaceDefinition View symbol surface definition value.
         */
        virtual void SendViewSymbolSurfaceDefinition(const sbio::symbol::SViewSymbolSurfaceDefinition& viewSymbolSurfaceDefinition) override;
        /**
         * @brief Sends symbol text definition.
         * @param symbolTextDef Symbol text def value.
         */
        virtual void SendSymbolTextDefinition(const sbio::symbol::SSymbolTextDefinition& symbolTextDef) override;
        /**
         * @brief Sends symbol textured circle definition.
         * @param symbolTexturedCircle Symbol textured circle value.
         */
        virtual void SendSymbolTexturedCircleDefinition(const sbio::symbol::SSymbolTexturedCircle& symbolTexturedCircle) override;
        /**
         * @brief Sends symbol textured polygon definition.
         * @param symbolTexturedPolygon Symbol textured polygon value.
         */
        virtual void SendSymbolTexturedPolygonDefinition(const sbio::symbol::SSymbolTexturedPolygon& symbolTexturedPolygon) override;
        /**
         * @brief Sends region terrestrial surface conditions control.
         * @param regionID Region id value.
         * @param terrestrialSurfaceControl Terrestrial surface control value.
         */
        virtual void SendRegionTerrestrialSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) override;
        /**
         * @brief Sends entity terrestrial surface conditions control.
         * @param entityID Entity id value.
         * @param terrestrialSurfaceControl Terrestrial surface control value.
         */
        virtual void SendEntityTerrestrialSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) override;
        /**
         * @brief Sends global terrestrial surface conditions control.
         * @param terrestrialSurfaceControl Terrestrial surface control value.
         */
        virtual void SendGlobalTerrestrialSurfaceConditionsControl(const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) override;
        /**
         * @brief Sends top level entity position.
         * @param topLevelEntityPosition Top level entity position value.
         */
        virtual void SendTopLevelEntityPosition(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition) override;
        /**
         * @brief Sends velocity control.
         * @param entityVelocityControl Entity velocity control value.
         */
        virtual void SendVelocityControl(const sbio::cigi::SCigiEntityVelocityControl& entityVelocityControl) override;
        /**
         * @brief Sends velocity control.
         * @param velocityControl Velocity control value.
         */
        virtual void SendVelocityControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& velocityControl) override;
        /**
         * @brief Sends view control.
         * @param viewControl View control value.
         */
        virtual void SendViewControl(const sbio::cigi::SCigiViewControl& viewControl) override;
        /**
         * @brief Sends view definition.
         * @param viewDefinition View definition value.
         */
        virtual void SendViewDefinition(const sbio::cigi::SCigiViewDefinition& viewDefinition) override;
        /**
         * @brief Sends entity wave control.
         * @param entityID Entity id value.
         * @param waveCondition Wave condition value.
         */
        virtual void SendEntityWaveControl(EntityID entityID, const sbio::cigi::SCigiWaveCondition& waveCondition) override;
        /**
         * @brief Sends regional wave control.
         * @param regionID Region id value.
         * @param waveCondition Wave condition value.
         */
        virtual void SendRegionalWaveControl(RegionID regionID, const sbio::cigi::SCigiWaveCondition& waveCondition) override;
        /**
         * @brief Sends global wave control.
         * @param waveCondition Wave condition value.
         */
        virtual void SendGlobalWaveControl(const sbio::cigi::SCigiWaveCondition& waveCondition) override;

        /**
         * @brief Sends weather control.
         * @param globalLayerWeatherID Global layer weather id value.
         * @param weatherCondition Weather condition value.
         * @param spatialWeatherCondition Spatial weather condition value.
         */
        virtual void SendWeatherControl(sbio::GlobalLayeredWeatherID globalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition,
                                        const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) override;
        /**
         * @brief Sends weather control.
         * @param regionID Region id value.
         * @param regionlLayeredWeatherID Regionl layered weather id value.
         * @param weatherCondition Weather condition value.
         * @param spatialWeatherCondition Spatial weather condition value.
         */
        virtual void SendWeatherControl(sbio::RegionID regionID, sbio::RegionalLayeredWeatherID regionlLayeredWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition,
                                        const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) override;
        /**
         * @brief Sends weather control.
         * @param entityID Entity id value.
         * @param weatherCondition Weather condition value.
         */
        virtual void SendWeatherControl(sbio::EntityID entityID, const sbio::cigi::SCigiWeatherCondition& weatherCondition) override;

        /**
         * @brief Updates base IG-control state and cached CIGI 4.0 control flags.
         * @param databaseID Database request number passed to `CHostSession::SetIGControl()`.
         * @param bEntityTypeSubstitutionEnabled Enables the entity-type substitution flag when true.
         * @param eIGMode Requested reset, operate, or debug mode.
         * @param bSmoothingEnabled Enables the smoothing flag when true.
         * @return `true` if the base request is accepted; `false` if disconnected or the mode is unsupported.
         *         Does not transmit the cached packet.
         */
        virtual bool SetIGControl(sbio::cigi::CigiDatabaseNumber databaseID, bool bEntityTypeSubstitutionEnabled, sbio::cigi::EIGMode eIGMode, bool bSmoothingEnabled) override;
        /**
         * @brief Raises formatted sensor diagnostics when logging is enabled; otherwise does nothing.
         * @param args Sensor response to format; the diagnostic is tagged with this session's ID.
         */
        virtual void OnHostCigiSensorResponseEvent(const HostCigiSensorResponseEventArgs& args);

      protected:
        /**
         * @brief Gets the minimum complete packet size required by a supported CIGI 4.0 parser.
         * @param eOpCode Incoming opcode to check.
         * @return Minimum size in bytes, including the header, or zero for an unrecognized opcode.
         */
        int GetMinimumIncomingPacketSizeV4(ECigiOpCodeV4 eOpCode) const;

        /**
         * @brief Reads a CIGI 4.0 packet header and applies byte swapping when required.
         * @param buffer Readable memory containing at least `sizeof(SCigiPacketHeaderV4)` bytes.
         * @param bByteSwap Whether to swap the copied packet-size and opcode fields.
         * @return Copied header in the requested byte order; no length or opcode validation is performed.
         */
        SCigiPacketHeaderV4 GetPacketHeaderV4(const uint8_t* buffer, bool bByteSwap) const;

      protected:
        CIGI::V40::IGCtrl m_IGControl;

        /**
         * @brief Member-function pointer type for CIGI 4.0 packet handlers.
         */
        typedef void (CHostSessionV4::*TPacketHandlerFunction)(uint8_t*);
        typedef std::unordered_map<ECigiOpCodeV4, TPacketHandlerFunction> TPacketHandlerFunctions;
        TPacketHandlerFunctions m_PacketHandlerFunctions;
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
