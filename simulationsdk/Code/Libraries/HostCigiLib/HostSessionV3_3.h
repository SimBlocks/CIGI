//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostSessionV3_3.h
 * @brief Declares the CHostSessionV3_3 class for managing CIGI 3.3 host sessions and protocol communication.
 *
 * Provides the CHostSessionV3_3 class for managing host-to-IG sessions, packet processing, and protocol communication
 * using the CIGI 3.3 protocol. Supports session initialization, packet sending/receiving, entity and environment control,
 * and event-driven integration with the host emulator. Includes CIGI 3.3-specific packet parsing and handler logic.
 *
 * @see sbio::cigi::host::CHostSessionV3_3
 * @see sbio::cigi::host::CHostSession
 * @see sbio::cigi::host::IHostCigiEventListener
 * @see sbio::cigi::host::SHostSetupOptions
 */
#pragma once

#ifndef SIMBLOCKS_CIGI_HOST_SESSION_V3_3_H
#define SIMBLOCKS_CIGI_HOST_SESSION_V3_3_H

#include "HostCigiLib/HostCigiEvent.h"
#include "HostSession.h"
#include "libCIGI/Packets/3_3/EntityCtrl.h"
#include "libCIGI/Packets/3_3/IGCtrl.h"

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Manages a CIGI 3.3 host session, packet processing, and protocol communication.
       *
       * Supports session initialization, packet sending/receiving, entity and environment control, and event-driven integration with the host emulator.
       * Includes CIGI 3.3-specific packet parsing and handler logic.
       * This specialization implements the packet formats and compatibility behavior required
       * when communicating with CIGI 3.3 image generators.
       *
       * Send helpers serialize or adapt data into the session queues; `SendPackets()` performs UDP
       * transmission. `SendIGControl()` instead writes directly to a supplied buffer. Input references
       * are not retained. Queue acceptance is not reported by the void send helpers.
       * Entity control, pose, and animation share a cached CIGI 3.3 Entity Control packet per entity.
       * Methods not overridden here retain the base class's no-op behavior.
       */
      class CHostSessionV3_3 : public CHostSession
      {
      public:
        /**
         * @brief Constructs a `CHostSessionV3_3` instance.
         */
        CHostSessionV3_3();

        /// @name Packet parsing and serialization overrides
        /// @{
        /**
         * @brief Validates a CIGI 3.3 header and dispatches one recognized response packet.
         * @param buffer Readable packet bytes beginning with the header; not retained.
         * @param nRemainingBytes Number of bytes available at `buffer`.
         * @return Declared packet size, or zero for a missing header or invalid declared length.
         *         Unsupported opcodes and undersized known payloads are skipped with a positive size
         *         and a diagnostic event; only recognized, sufficiently sized packets mark valid traffic.
         */
        virtual int ProcessPacket(uint8_t* buffer, int nRemainingBytes) override;
        /**
         * @brief Reads the length of a queued CIGI 3.3 packet.
         * @param buffer Readable packet bytes beginning with the header.
         * @param nRemainingBytes Number of available bytes.
         * @return Declared size, or zero if the header is incomplete, the size is below the header size,
         *         or the size exceeds available bytes. Does not validate opcode-specific payload fields.
         */
        virtual int GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const override;
        /**
         * @brief Writes CIGI 3.3 IG Control with current frame counters, timestamp, and database handshake state.
         * @param pBuffer Writable cursor with room for `sizeof(CIGI::V33::IGCtrl)` bytes; advanced by that size.
         *                Applies the configured byte order; does not queue or transmit the packet.
         */
        virtual void SendIGControl(uint8_t*& pBuffer) override;
        /// @}

        /// @name Response packet parsers
        /// These helpers decode individual inbound CIGI 3.3 packets and raise host events.
        /// Callers must supply a complete packet, including its header and declared variable data.
        /// These helpers do not validate buffer capacity; use `ProcessPacket()` for length checks.
        /// Packet bytes are copied and decoded using the session byte-swap flag, not retained.
        /// @{
        /**
         * @brief Decodes start-of-frame state, raises its event, updates database state, and handles synchronous sending.
         * @param buffer Complete CIGI 3.3 Start of Frame packet.
         */
        void ParseStartOfFramePacket(uint8_t* buffer);
        /**
         * @brief Decodes a terrain-height response and raises a HAT or HOT event.
         * @param buffer Complete CIGI 3.3 HAT/HOT Response packet.
         */
        void ParseHatHotResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes terrain heights and surface data and raises an extended HAT/HOT event.
         * @param buffer Complete CIGI 3.3 HAT/HOT Extended Response packet.
         */
        void ParseHatHotExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes a line-of-sight response and selects the entity or non-entity event variant.
         * @param buffer Complete CIGI 3.3 Line of Sight Response packet.
         */
        void ParseLineOfSightResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes an extended line-of-sight response using the recorded request coordinate system.
         * @param buffer Complete CIGI 3.3 Line of Sight Extended Response packet.
         *
         * A response with no recorded coordinate system is ignored, with a warning when a logger is available.
         */
        void ParseLineOfSightExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes sensor tracking state and raises a sensor-response event.
         * @param buffer Complete CIGI 3.3 Sensor Response packet.
         */
        void ParseSensorResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes extended tracking data and selects the entity or non-entity sensor event variant.
         * @param buffer Complete CIGI 3.3 Sensor Extended Response packet.
         */
        void ParseSensorExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes object position and raises a position event with a borrowed response payload.
         * @param buffer Complete CIGI 3.3 Position Response packet.
         */
        void ParsePositionResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried weather conditions and raises the corresponding response event.
         * @param buffer Complete CIGI 3.3 Weather Conditions Response packet.
         */
        void ParseWeatherConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried aerosol concentration and raises the corresponding response event.
         * @param buffer Complete CIGI 3.3 Aerosol Concentration Response packet.
         */
        void ParseAerosolConcentrationResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried maritime conditions and raises the corresponding response event.
         * @param buffer Complete CIGI 3.3 Maritime Surface Conditions Response packet.
         */
        void ParseMaritimeSurfaceConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes queried terrestrial conditions and raises the corresponding response event.
         * @param buffer Complete CIGI 3.3 Terrestrial Surface Conditions Response packet.
         */
        void ParseTerrestrialSurfaceConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Decodes a segment collision and selects the entity or non-entity notification variant.
         * @param buffer Complete CIGI 3.3 Collision Detection Segment Notification packet.
         */
        void ParseCollisionDetectionSegmentNotificationPacket(uint8_t* buffer);
        /**
         * @brief Decodes a volume collision and selects the entity or non-entity notification variant.
         * @param buffer Complete CIGI 3.3 Collision Detection Volume Notification packet.
         */
        void ParseCollisionDetectionVolumeNotificationPacket(uint8_t* buffer);
        /**
         * @brief Decodes an entity animation-stop notification and raises its event.
         * @param buffer Complete CIGI 3.3 Animation Stop Notification packet.
         */
        void ParseAnimationStopNotificationPacket(uint8_t* buffer);
        /**
         * @brief Decodes an event identifier and three data words and raises an IG event notification.
         * @param buffer Complete CIGI 3.3 Event Notification packet.
         */
        void ParseEventNotificationPacket(uint8_t* buffer);
        /**
         * @brief Copies an IG message identifier and declared text bytes into a message event.
         * @param buffer Complete CIGI 3.3 IG Message packet, including any text payload.
         */
        void ParseImageGeneratorMessagePacket(uint8_t* buffer);
        /// @}

        /**
         * @brief Resets the CIGI 3.3 session state.
         *
         * Performs the base reset (including socket closure), clears cached entity packets, and
         * replaces the cached IG Control packet with its default value.
         */
        virtual void Reset() override;

        /**
         * @brief Queues entity control while preserving any cached pose and animation fields.
         * @param entityControl Entity identity and control settings; extended entity types are rejected.
         */
        virtual void SendEntityControl(const sbio::cigi::SEntityControl& entityControl) override;
        /**
         * @brief Updates animation fields while preserving the cached entity control and pose.
         * @param animationControl Animation control value.
         *
         * Requires a cached entity-control packet; otherwise raises an error and queues nothing.
         */
        virtual void SendAnimationControl(const sbio::cigi::SCigiAnimationControl& animationControl) override;
        /**
         * @brief Sends conformal clamped entity position.
         * @param conformalClampedEntityControl Conformal clamped entity control value.
         */
        virtual void SendConformalClampedEntityPosition(const sbio::cigi::SCigiConformalClampedEntityPosition& conformalClampedEntityControl) override;
        /**
         * @brief Sends component control.
         * @param componentControl Component control value.
         */
        virtual void SendComponentControl(const sbio::cigi::SCigiComponentControl& componentControl) override;
        /**
         * @brief Sends short component control.
         * @param shortComponentControl Short component control value.
         */
        virtual void SendShortComponentControl(const sbio::cigi::SCigiShortComponentControl& shortComponentControl) override;
        /**
         * @brief Sends articulated part control.
         * @param articulatedPart Articulated part value.
         */
        virtual void SendArticulatedPartControl(const sbio::cigi::SCigiArticulatedPart& articulatedPart) override;
        /**
         * @brief Sends short articulated part control.
         * @param shortArticulatedPart Short articulated part value.
         */
        virtual void SendShortArticulatedPartControl(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart) override;
        /**
         * @brief Sends rate control.
         * @param rateControl Rate control value.
         */
        virtual void SendRateControl(const sbio::cigi::SCigiEntityVelocityControl& rateControl) override;
        /**
         * @brief Sends rate control.
         * @param rateControl Rate control value.
         */
        virtual void SendRateControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& rateControl) override;
        /**
         * @brief Sends entity velocity using a CIGI 3.3 rate control packet.
         * @param velocityControl Entity velocity control value.
         */
        virtual void SendVelocityControl(const sbio::cigi::SCigiEntityVelocityControl& velocityControl) override;
        /**
         * @brief Sends articulated-part velocity using a CIGI 3.3 rate control packet.
         * @param velocityControl Articulated-part velocity control value.
         */
        virtual void SendVelocityControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& velocityControl) override;
        /**
         * @brief Sends celestial sphere control.
         * @param celestialSphereControl Celestial sphere control value.
         */
        virtual void SendCelestialSphereControl(const sbio::cigi::SCigiCelestialSphereControl& celestialSphereControl) override;
        /**
         * @brief Queues child pose changes using cached entity-control state.
         * @param childEntityPosition Entity and parent identifiers, attachment state, and local pose.
         *
         * Does nothing when the entity has no cached control packet.
         */
        virtual void SendChildEntityPosition(const sbio::cigi::SChildEntityPosition& childEntityPosition) override;
        /**
         * @brief Sends atmosphere control.
         * @param atmosphereControl Atmosphere control value.
         */
        virtual void SendAtmosphereControl(const sbio::cigi::SCigiAtmosphereControl& atmosphereControl) override;
        /**
         * @brief Sends environmental region control.
         * @param environmentalRegion Environmental region value.
         */
        virtual void SendEnvironmentalRegionControl(const sbio::cigi::SCigiEnvironmentalRegion& environmentalRegion) override;
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
         * @brief Sends view control.
         * @param viewControl View control value.
         */
        virtual void SendViewControl(const sbio::cigi::SCigiViewControl& viewControl) override;
        /**
         * @brief Sends sensor control.
         * @param sensorControl Sensor control value.
         */
        virtual void SendSensorControl(const sbio::cigi::SCigiSensorControl& sensorControl) override;
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
         * @brief Sends earth reference model definition.
         * @param earthReferenceModel Earth reference model value.
         */
        virtual void SendEarthReferenceModelDefinition(const sbio::cigi::SCigiEarthReferenceModel& earthReferenceModel) override;
        /**
         * @brief Queues top-level pose changes using cached entity-control state.
         * @param topLevelEntityPosition Entity identifier, geodetic pose, and clamp settings.
         *
         * Does nothing when the entity has no cached control packet.
         */
        virtual void SendTopLevelEntityPosition(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition) override;
        /**
         * @brief Queues a CIGI 3.3 trajectory definition using entity linear acceleration.
         * @param trajectoryDefinition Entity identifier and linear acceleration; angular acceleration and
         *                             coordinate selector are not encoded. Retardation and terminal velocity are zero.
         */
        virtual void SendTrajectoryDefinition(const sbio::cigi::SCigiEntityAcceleration& trajectoryDefinition) override;
        /**
         * @brief Sends view definition.
         * @param viewDefinition View definition value.
         */
        virtual void SendViewDefinition(const sbio::cigi::SCigiViewDefinition& viewDefinition) override;
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
         * @brief Sends position request.
         * @param positionRequest Position request value.
         */
        virtual void SendPositionRequest(const sbio::cigi::SPositionRequest& positionRequest) override;
        /**
         * @brief Sends environmental conditions request.
         * @param environmentalConditionsRequest Environmental conditions request value.
         */
        virtual void SendEnvironmentalConditionsRequest(const sbio::cigi::SEnvironmentalConditionsRequest& environmentalConditionsRequest) override;
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
         * @brief Sends symbol circle definition.
         * @param circleDef Circle def value.
         */
        virtual void SendSymbolCircleDefinition(const sbio::symbol::SSymbolCircle& circleDef) override;
        /**
         * @brief Sends symbol line definition.
         * @param symbolLine Symbol line value.
         */
        virtual void SendSymbolLineDefinition(const sbio::symbol::SSymbolPolygon& symbolLine) override;
        /**
         * @brief Sends polygon primitives using the CIGI 3.3 symbol line definition packet.
         * @param symbolPolygon Symbol polygon value.
         */
        virtual void SendSymbolPolygonDefinition(const sbio::symbol::SSymbolPolygon& symbolPolygon) override;
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
         * @brief Sends short symbol control.
         * @param shortSymbolControl Short symbol control value.
         */
        virtual void SendShortSymbolControl(const sbio::symbol::SShortSymbolControl& shortSymbolControl) override;

        /**
         * @brief Updates base IG-control state and the cached CIGI 3.3 mode/extrapolation fields.
         * @param databaseNumber Database request number passed to `CHostSession::SetIGControl()`.
         * @param bEntityTypeSubstitutionEnabled Unused by the CIGI 3.3 implementation.
         * @param eIGMode Requested reset, operate, or debug mode.
         * @param bSmoothingEnabled Enables the CIGI 3.3 extrapolation flag when true.
         * @return `true` if the base request is accepted; `false` if disconnected or the mode is unsupported.
         *         Does not transmit the cached packet.
         */
        virtual bool SetIGControl(sbio::cigi::CigiDatabaseNumber databaseNumber, bool bEntityTypeSubstitutionEnabled, sbio::cigi::EIGMode eIGMode, bool bSmoothingEnabled) override;

      protected:
        /**
         * @brief Gets the minimum complete packet size required by a supported CIGI 3.3 parser.
         * @param eOpCode Incoming opcode to check.
         * @return Minimum size in bytes, including the header, or zero for an unrecognized opcode.
         */
        int GetMinimumIncomingPacketSizeV3(ECigiOpCodeV3 eOpCode) const;

        /**
         * @brief Combines entity, geodetic pose, and animation data into one CIGI 3.3 Entity Control packet.
         * @param entityControl Entity control value.
         * @param topLevelEntityPosition Top level entity position value.
         * @param animationControl Animation control value.
         */
        void SendTopLevelEntityControl(const sbio::cigi::SEntityControl& entityControl, const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition,
                                       const sbio::cigi::SCigiAnimationControl& animationControl);

        /**
         * @brief Combines entity, parent-relative pose, and animation data into one CIGI 3.3 Entity Control packet.
         * @param entityControl Entity control value.
         * @param childEtityPosition Child pose and parent attachment; clamping is disabled for this packet.
         * @param animationControl Animation control value.
         */
        void SendChildEntityControl(const sbio::cigi::SEntityControl& entityControl, const sbio::cigi::SChildEntityPosition& childEtityPosition,
                                    const sbio::cigi::SCigiAnimationControl& animationControl);

        /**
         * @brief Queues an entity-control packet and updates its native-order cache only if accepted.
         * @param entityControl Complete native-order packet; a copy is byte-swapped for the send queue.
         *
         * Cached Play state becomes Continue so later pose updates do not restart animation.
         */
        void SendEntityControlPacket(CIGI::V33::EntityCtrl entityControl);
        /** @brief Clears base transient data and all cached CIGI 3.3 entity-control packets. */
        void ClearSessionData() override;

      protected:
        CIGI::V33::IGCtrl m_IGControl;

        /**
         * @brief Member-function pointer type for CIGI 3.3 packet handlers.
         */
        typedef void (CHostSessionV3_3::*TPacketHandlerFunction)(uint8_t*);
        typedef std::unordered_map<ECigiOpCodeV3, TPacketHandlerFunction> TPacketHandlerFunctions;
        TPacketHandlerFunctions m_PacketHandlerFunctions;

        std::unordered_map<sbio::EntityID, CIGI::V33::EntityCtrl, StrongTypeHash<sbio::EntityID>> m_Entities;
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
