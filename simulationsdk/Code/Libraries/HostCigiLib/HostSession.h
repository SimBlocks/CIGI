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
#include <optional>
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
         * @brief Gets the tracked database request number.
         * @return Stored database number, not necessarily the database currently loaded by the IG.
         */
        sbio::cigi::CigiDatabaseNumber GetDatabaseNumber() const;
        /**
         * @brief Gets the host's database-loading state.
         * @return Current request/acknowledgment state or `IG_CONTROLLED` when applicable.
         */
        EHostSessionDatabaseState GetDatabaseState() const;
        /**
         * @brief Gets the last IG mode decoded from start-of-frame traffic.
         * @return Reported mode, or `UNKNOWN` before a report or after reset.
         */
        sbio::cigi::EIGMode GetActualIGMode() const;
        /**
         * @brief Gets the host's requested IG mode.
         * @return Stored desired mode; this does not confirm the IG has entered that mode.
         */
        sbio::cigi::EIGMode GetDesiredIGMode() const;
        /**
         * @brief Gets the outgoing host frame counter.
         * @return Counter advanced after each successful datagram send and cleared by `Reset()`.
         */
        int GetFrameCount() const;
        /**
         * @brief Reports the diagnostic logging flag.
         * @return `true` when optional packet diagnostics are enabled; errors may still be emitted when false.
         */
        bool GetLoggingEnabled() const;
        /**
         * @brief Reads elapsed time from the session stopwatch.
         * @return Elapsed seconds since the stopwatch was started by `Initialize()`.
         * @pre `Initialize()` has created the stopwatch.
         */
        double GetSessionTime() const;
        /**
         * @brief Gets the identifier attached to events raised by this session.
         * @return Stored session identifier; initially zero.
         */
        sbio::SessionID GetSessionID() const;
        /// @}

        /**
         * @brief Initializes sockets, timers, and other per-session state.
         *
         * Restarts session timing and replaces sockets using `hostSetupOptions`. Sends to the IG address
         * and host-to-IG port and listens on the IG-to-host port. Socket setup failures raise error events
         * and leave sockets unavailable; this call does not establish connectivity or clear queued packets.
         */
        void Initialize();

        /**
         * @brief Reports whether packets have recently been received from the IG.
         * @return Last tracked connection state, updated by packet processing, reset, and socket availability;
         *         this query does not poll the network or update the disconnection timeout.
         */
        bool IsConnected() const;

        /**
         * @brief Consumes one inbound packet from the receive buffer.
         * @param buffer Pointer to readable bytes starting at a CIGI packet header.
         * @param nRemainingBytes Number of bytes available at `buffer`.
         * @return Bytes to advance, or zero if the header or declared length cannot be consumed.
         *         A positive result can represent a skipped unsupported or undersized packet.
         */
        virtual int ProcessPacket(uint8_t* buffer, int nRemainingBytes) = 0;

        /**
         * @brief Determines how many bytes of queued outbound data form the next packet.
         * @param buffer Pointer to the next queued outbound bytes.
         * @param nRemainingBytes Number of unread bytes remaining in the send buffer.
         * @return Declared packet size in bytes, or zero if the header or declared length is invalid.
         */
        virtual int GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const = 0;

        /**
         * @brief Receives at most one datagram and dispatches its CIGI packets.
         * @return `true` when a datagram was received, including invalid data, so callers can drain the socket.
         *         Returns `false` when no data is available, the receive socket is missing, or reception throws
         *         a Poco exception. Updates connection timing and raises session-tagged events.
         */
        bool ProcessPackets();

        /**
         * @brief Clears transient session state while preserving configured connection options.
         *
         * Closes and releases both sockets, clears queues and response-coordinate mappings, resets frame
         * counters and database state, and requests reset mode. Keeps the session ID, options, logging,
         * byte-swap flag, and existing session stopwatch. Call `Initialize()` to recreate sockets.
         */
        virtual void Reset();

        /**
         * @brief Sends one datagram beginning with IG Control and as many complete queued packets as fit.
         *
         * Leaves deferred packets queued in order. On a successful send, advances the frame counter and
         * refills the active buffer from overflow. Missing send sockets clear the queues. A failed socket
         * send leaves queued bytes intact. Packets too large to coexist with IG Control are discarded
         * after a successful send and an error event is raised.
         */
        void SendPackets();

        /**
         * @brief Enables or disables packet logging for this session.
         * @param bEnabled `true` to enable optional diagnostics; does not suppress all events when false.
         */
        void SetLoggingEnabled(bool bEnabled);

        /**
         * @brief Configures the CIGI wire byte order and derives whether packet byte swapping is required.
         * @param bBigEndian `true` for big-endian wire data; `false` for little-endian wire data.
         *
         * The session compares the requested wire order with the platform's native byte order. Already queued
         * packet bytes are not converted by this setter.
         */
        void SetWireByteOrder(bool bBigEndian);

        /**
         * @brief Directly configures whether packet byte swapping is required for this session.
         * @param bEnabled `true` when wire byte order differs from native order; `false` otherwise.
         *                 Already queued packet bytes are not converted by this setter.
         *
         * Prefer SetWireByteOrder() when configuring a session from a declared wire byte order.
         */
        void SetByteSwapEnabled(bool bEnabled);

        /**
         * @brief Assigns the logical identifier used to reference this session.
         * @param sessionID Identifier to store and stamp onto subsequent session events.
         */
        void SetSessionID(sbio::SessionID sessionID);

        /**
         * @brief Updates the desired IG mode and database-selection state for the session.
         * @param databaseNumber Database request number; zero leaves selection unchanged except in reset mode.
         * @param bEntityTypeSubstitutionEnabled Substitution request for derived implementations; unused here.
         * @param eIGMode Requested `RESET`, `OPERATE`, or `DEBUG` mode.
         * @param bSmoothingEnabled Smoothing request for derived implementations; unused here.
         * @return `true` if accepted, even if no state changed; `false` for an unsupported mode or disconnected session.
         *
         * Reset mode clears transient session data and database selection, but does not close sockets.
         * Acceptance updates local state only; it does not transmit or confirm an IG acknowledgment.
         */
        virtual bool SetIGControl(sbio::cigi::CigiDatabaseNumber databaseNumber, bool bEntityTypeSubstitutionEnabled, sbio::cigi::EIGMode eIGMode, bool bSmoothingEnabled);

        /// @name Protocol-specific packet emission
        /// Derived classes override the following methods to serialize version-specific packets.
        /// Except for the pure virtual `SendIGControl()`, all base implementations in this group are no-ops.
        /// Implemented send helpers queue packet copies for `SendPackets()`; they do not confirm delivery.
        /// @{
        /**
         * @brief Serializes IG Control directly into the caller's output buffer.
         * @param pBuffer Writable cursor with room for the version-specific IG Control packet;
         *                advanced past the serialized packet. No buffer-capacity check is performed here.
         */
        virtual void SendIGControl(uint8_t*& pBuffer) = 0;
        /**
         * @brief Hook for entity identity, state, and rendering control.
         * @param entityControl Entity identifier and control fields to encode.
         */
        virtual void SendEntityControl(const sbio::cigi::SEntityControl& entityControl) {};
        /**
         * @brief Hook for articulated-part offsets, rotation, and enable flags.
         * @param articulatedPart Owning entity, part identifier, and articulation settings.
         */
        virtual void SendArticulatedPartControl(const sbio::cigi::SCigiArticulatedPart& articulatedPart) {};
        /**
         * @brief Hook for entity linear and angular acceleration.
         * @param accelerationControl Entity identifier, coordinate system, and acceleration values.
         */
        virtual void SendEntityAccelerationControl(const sbio::cigi::SCigiEntityAcceleration& accelerationControl) {};
        /**
         * @brief Hook for articulated-part linear and angular acceleration.
         * @param accelerationControl Entity and part identifiers with acceleration settings.
         */
        virtual void SendArticulatedPartAccelerationControl(const sbio::cigi::SCigiArticulatedPartAcceleration& accelerationControl) {};
        /**
         * @brief Hook for legacy trajectory definition.
         * @param trajectoryDefinition Entity acceleration data used to construct the trajectory packet.
         */
        virtual void SendTrajectoryDefinition(const sbio::cigi::SCigiEntityAcceleration& trajectoryDefinition) {};
        /**
         * @brief Hook for entity animation control.
         * @param animationControl Entity identifier and animation settings.
         */
        virtual void SendAnimationControl(const sbio::cigi::SCigiAnimationControl& animationControl) {};
        /**
         * @brief Hook for global atmosphere control.
         * @param atmosphereControl Atmosphere model and global atmospheric conditions.
         */
        virtual void SendAtmosphereControl(const sbio::cigi::SCigiAtmosphereControl& atmosphereControl) {};
        /**
         * @brief Hook for celestial sphere control.
         * @param celestialSphereControl Date, time, and celestial visibility settings.
         */
        virtual void SendCelestialSphereControl(const sbio::cigi::SCigiCelestialSphereControl& celestialSphereControl) {};
        /**
         * @brief Hook for positioning an entity relative to a parent.
         * @param childEntityPosition Entity and parent identifiers, attachment state, and local pose.
         */
        virtual void SendChildEntityPosition(const sbio::cigi::SChildEntityPosition& childEntityPosition) {};
        /**
         * @brief Hook for defining an entity collision-detection segment.
         * @param collDetSegment Segment identifier, endpoints, and collision settings.
         */
        virtual void SendCollisionDetectionSegment(const sbio::cigi::SCollisionDetectionSegmentDefinition& collDetSegment) {};
        /**
         * @brief Hook for defining a cuboid collision volume.
         * @param collVolCuboid Entity and volume identifiers, dimensions, pose, and enable state.
         */
        virtual void SendCollisionDetectionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collVolCuboid) {};
        /**
         * @brief Hook for defining a spherical collision volume.
         * @param collVolSphere Entity and volume identifiers, center, radius, and enable state.
         */
        virtual void SendCollisionDetectionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collVolSphere) {};
        /**
         * @brief Hook for component state and data control.
         * @param componentControl Component key, state, and data fields.
         */
        virtual void SendComponentControl(const sbio::cigi::SCigiComponentControl& componentControl) {};
        /**
         * @brief Hook for a conformally clamped entity position.
         * @param conformalClampedEntityPos Entity identifier, latitude, longitude, and yaw.
         */
        virtual void SendConformalClampedEntityPosition(const sbio::cigi::SCigiConformalClampedEntityPosition& conformalClampedEntityPos) {};
        /**
         * @brief Hook for selecting or defining the Earth reference model.
         * @param earthReferenceModel Model selection and ellipsoid parameters.
         */
        virtual void SendEarthReferenceModelDefinition(const sbio::cigi::SCigiEarthReferenceModel& earthReferenceModel) {};
        /**
         * @brief Hook for querying environmental conditions at a position.
         * @param environmentalConditionsRequest Request identifier, position, and requested condition flags.
         */
        virtual void SendEnvironmentalConditionsRequest(const sbio::cigi::SEnvironmentalConditionsRequest& environmentalConditionsRequest) {};
        /**
         * @brief Hook for environmental region geometry and merge behavior.
         * @param environmentalRegion Region identifier, bounds, and control settings.
         */
        virtual void SendEnvironmentalRegionControl(const sbio::cigi::SCigiEnvironmentalRegion& environmentalRegion) {};
        /**
         * @brief Hook for a geodetic height-above-terrain or height-of-terrain query.
         * @param hatHotRequest Request identifier, geodetic position, and response options.
         */
        virtual void SendHatHotRequest(const sbio::cigi::SHATHOTGlobalRequest& hatHotRequest) {};
        /**
         * @brief Hook for an entity-relative height-above-terrain or height-of-terrain query.
         * @param hatHotRequest Request identifier, entity-relative position, and response options.
         */
        virtual void SendHatHotRequest(const sbio::cigi::SHATHOTEntityRequest& hatHotRequest) {};
        /**
         * @brief Hook for a basic line-of-sight segment query between geodetic endpoints.
         * @param losRequest Request identifier, endpoints, and intersection options.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& losRequest) {};
        /**
         * @brief Hook for an extended line-of-sight query between geodetic endpoints.
         * @param losRequest Endpoints, intersection options, and requested response coordinate system.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToGeodeticExtended(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& losRequest) {};
        /**
         * @brief Hook for a basic line-of-sight query from a geodetic to an entity-relative endpoint.
         * @param losRequest Request identifier, endpoints, and intersection options.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToEntityBasic(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& losRequest) {};
        /**
         * @brief Hook for an extended line-of-sight query from geodetic to entity-relative coordinates.
         * @param losRequest Endpoints, intersection options, and requested response coordinate system.
         */
        virtual void SendLineOfSightSegmentRequestGeodeticToEntityExtended(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& losRequest) {};
        /**
         * @brief Hook for a basic line-of-sight query from an entity-relative to a geodetic endpoint.
         * @param losRequest Request identifier, endpoints, and intersection options.
         */
        virtual void SendLineOfSightSegmentRequestEntityToGeodeticBasic(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& losRequest) {};
        /**
         * @brief Hook for an extended line-of-sight query from entity-relative to geodetic coordinates.
         * @param losRequest Endpoints, intersection options, and requested response coordinate system.
         */
        virtual void SendLineOfSightSegmentRequestEntityToGeodeticExtended(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticExtended& losRequest) {};
        /**
         * @brief Hook for a basic line-of-sight segment query between entity-relative endpoints.
         * @param losRequest Request identifier, endpoints, and intersection options.
         */
        virtual void SendLineOfSightSegmentRequestEntityToEntityBasic(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& losRequest) {};
        /**
         * @brief Hook for an extended line-of-sight query between entity-relative endpoints.
         * @param losRequest Endpoints, intersection options, and requested response coordinate system.
         */
        virtual void SendLineOfSightSegmentRequestEntityToEntityExtended(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& losRequest) {};
        /**
         * @brief Hook for a basic line-of-sight vector query from a geodetic origin.
         * @param losRequest Origin, direction, range, and intersection options.
         */
        virtual void SendLineOfSightVectorRequestGeodeticBasic(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& losRequest) {};
        /**
         * @brief Hook for an extended line-of-sight vector query from a geodetic origin.
         * @param losRequest Vector query settings and requested response coordinate system.
         */
        virtual void SendLineOfSightVectorRequestGeodeticExtended(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& losRequest) {};
        /**
         * @brief Hook for a basic line-of-sight vector query from an entity-relative origin.
         * @param losRequest Origin, direction, range, and intersection options.
         */
        virtual void SendLineOfSightVectorRequestEntityBasic(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& losRequest) {};
        /**
         * @brief Hook for an extended line-of-sight vector query from an entity-relative origin.
         * @param losRequest Vector query settings and requested response coordinate system.
         */
        virtual void SendLineOfSightVectorRequestEntityExtended(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& losRequest) {};
        /**
         * @brief Hook for regional maritime surface conditions.
         * @param regionID Target region identifier.
         * @param maritimeSurfaceCondition Surface conditions to apply.
         */
        virtual void SendRegionMaritimeSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) {};
        /**
         * @brief Hook for entity maritime surface conditions.
         * @param entityID Target entity identifier.
         * @param maritimeSurfaceCondition Surface conditions to apply.
         */
        virtual void SendEntityMaritimeSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) {};
        /**
         * @brief Hook for global maritime surface conditions.
         * @param maritimeSurfaceCondition Surface conditions to apply globally.
         */
        virtual void SendGlobalMaritimeSurfaceConditionsControl(const sbio::cigi::SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition) {};
        /**
         * @brief Hook for motion-tracker control of a view.
         * @param motionTrackerViewControl View identifier and tracking settings.
         */
        virtual void SendMotionTrackerViewControl(const sbio::cigi::SMotionTrackerViewControl& motionTrackerViewControl) {};
        /**
         * @brief Hook for motion-tracker control of a view group.
         * @param motionTrackerViewGroupControl View-group identifier and tracking settings.
         */
        virtual void SendMotionTrackerViewGroupControl(const sbio::cigi::SMotionTrackerViewGroupControl& motionTrackerViewGroupControl) {};
        /**
         * @brief Hook for querying an object's position.
         * @param positionRequest Object selection and requested coordinate system.
         */
        virtual void SendPositionRequest(const sbio::cigi::SPositionRequest& positionRequest) {};
        /**
         * @brief Hook for legacy entity rate control.
         * @param rateControl Entity identifier and linear/angular velocity settings.
         */
        virtual void SendRateControl(const sbio::cigi::SCigiEntityVelocityControl& rateControl) {};
        /**
         * @brief Hook for legacy articulated-part rate control.
         * @param rateControl Entity and part identifiers with linear/angular velocity settings.
         */
        virtual void SendRateControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& rateControl) {};
        /**
         * @brief Hook for sensor control.
         * @param sensorControl Sensor and view identifiers with sensor settings.
         */
        virtual void SendSensorControl(const sbio::cigi::SCigiSensorControl& sensorControl) {};
        /**
         * @brief Hook for a short articulated-part update.
         * @param shortArticulatedPart Entity/part selection and selected degrees of freedom.
         */
        virtual void SendShortArticulatedPartControl(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart) {};
        /**
         * @brief Hook for a short component update.
         * @param shortComponentControl Component key, state, and short data fields.
         */
        virtual void SendShortComponentControl(const sbio::cigi::SCigiShortComponentControl& shortComponentControl) {};
        /**
         * @brief Hook for a short symbol update.
         * @param shortSymbolControl Symbol selection and control fields.
         */
        virtual void SendShortSymbolControl(const sbio::symbol::SShortSymbolControl& shortSymbolControl) {};
        /**
         * @brief Hook for a symbol circle definition.
         * @param circleDef Symbol identifier, circle geometry, and drawing settings.
         */
        virtual void SendSymbolCircleDefinition(const sbio::symbol::SSymbolCircle& circleDef) {};
        /**
         * @brief Hook for cloning a symbol.
         * @param symbolCloneStruct Source and destination symbol identifiers.
         */
        virtual void SendSymbolClone(const sbio::symbol::SSymbolClone& symbolCloneStruct) {};
        /**
         * @brief Hook for symbol state and transformation control.
         * @param symbolControl Symbol selection and control settings.
         */
        virtual void SendSymbolControl(const sbio::symbol::SSymbolControl& symbolControl) {};
        /**
         * @brief Hook for legacy symbol line definitions.
         * @param symbolLine Symbol identifier, primitive type, and vertices.
         */
        virtual void SendSymbolLineDefinition(const sbio::symbol::SSymbolPolygon& symbolLine) {};
        /**
         * @brief Hook for symbol polygon definitions.
         * @param symbolPolygon Symbol identifier, primitive type, and vertices.
         */
        virtual void SendSymbolPolygonDefinition(const sbio::symbol::SSymbolPolygon& symbolPolygon) {};
        /**
         * @brief Hook for a symbol surface attached to an entity.
         * @param entitySymbolSurfaceDefinition Surface identifier, entity attachment, and geometry.
         */
        virtual void SendEntitySymbolSurfaceDefinition(const sbio::symbol::SEntitySymbolSurfaceDefinition& entitySymbolSurfaceDefinition) {};
        /**
         * @brief Hook for an entity-attached billboard symbol surface.
         * @param entityBillboardSymbolSurfaceDefinition Surface identifier, attachment, and billboard settings.
         */
        virtual void SendEntityBillboardSymbolSurfaceDefinition(const sbio::symbol::SEntityBillboardSymbolSurfaceDefinition& entityBillboardSymbolSurfaceDefinition) {};
        /**
         * @brief Hook for a view-attached symbol surface.
         * @param viewSymbolSurfaceDefinition Surface identifier, view selection, and geometry.
         */
        virtual void SendViewSymbolSurfaceDefinition(const sbio::symbol::SViewSymbolSurfaceDefinition& viewSymbolSurfaceDefinition) {};
        /**
         * @brief Hook for a text symbol definition.
         * @param symbolTextDef Symbol identifier, text, font, and layout settings.
         */
        virtual void SendSymbolTextDefinition(const sbio::symbol::SSymbolTextDefinition& symbolTextDef) {};
        /**
         * @brief Hook for a textured-circle symbol definition.
         * @param symbolTexturedCircle Circle geometry and texture settings.
         */
        virtual void SendSymbolTexturedCircleDefinition(const sbio::symbol::SSymbolTexturedCircle& symbolTexturedCircle) {};
        /**
         * @brief Hook for a textured-polygon symbol definition.
         * @param symbolTexturedPolygon Polygon geometry and texture settings.
         */
        virtual void SendSymbolTexturedPolygonDefinition(const sbio::symbol::SSymbolTexturedPolygon& symbolTexturedPolygon) {};
        /**
         * @brief Hook for regional terrestrial surface conditions.
         * @param regionID Target region identifier.
         * @param terrestrialSurfaceControl Surface conditions to apply.
         */
        virtual void SendRegionTerrestrialSurfaceConditionsControl(RegionID regionID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) {};
        /**
         * @brief Hook for entity terrestrial surface conditions.
         * @param entityID Target entity identifier.
         * @param terrestrialSurfaceControl Surface conditions to apply.
         */
        virtual void SendEntityTerrestrialSurfaceConditionsControl(EntityID entityID, const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) {};
        /**
         * @brief Hook for global terrestrial surface conditions.
         * @param terrestrialSurfaceControl Surface conditions to apply globally.
         */
        virtual void SendGlobalTerrestrialSurfaceConditionsControl(const sbio::cigi::SCigiTerrestrialSurfaceCondition& terrestrialSurfaceControl) {};
        /**
         * @brief Hook for top-level entity positioning.
         * @param topLevelEntityPosition Entity identifier, geodetic pose, and clamp settings.
         */
        virtual void SendTopLevelEntityPosition(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition) {};
        /**
         * @brief Hook for entity velocity control.
         * @param entityVelocityControl Entity identifier, coordinate system, and linear/angular velocities.
         */
        virtual void SendVelocityControl(const sbio::cigi::SCigiEntityVelocityControl& entityVelocityControl) {};
        /**
         * @brief Hook for articulated-part velocity control.
         * @param velocityControl Entity and part identifiers with linear/angular velocities.
         */
        virtual void SendVelocityControl(const sbio::cigi::SCigiArticulatedPartVelocityControl& velocityControl) {};
        /**
         * @brief Hook for view pose and attachment control.
         * @param viewControl View selection and control settings.
         */
        virtual void SendViewControl(const sbio::cigi::SCigiViewControl& viewControl) {};
        /**
         * @brief Hook for view projection and frustum definition.
         * @param viewDefinition View identifier and projection settings.
         */
        virtual void SendViewDefinition(const sbio::cigi::SCigiViewDefinition& viewDefinition) {};
        /**
         * @brief Hook for entity wave control.
         * @param entityID Target entity identifier.
         * @param waveCondition Wave definition and state.
         */
        virtual void SendEntityWaveControl(EntityID entityID, const sbio::cigi::SCigiWaveCondition& waveCondition) {};
        /**
         * @brief Hook for regional wave control.
         * @param regionID Target region identifier.
         * @param waveCondition Wave definition and state.
         */
        virtual void SendRegionalWaveControl(RegionID regionID, const sbio::cigi::SCigiWaveCondition& waveCondition) {};
        /**
         * @brief Hook for global wave control.
         * @param waveCondition Wave definition and state to apply globally.
         */
        virtual void SendGlobalWaveControl(const sbio::cigi::SCigiWaveCondition& waveCondition) {};

        /**
         * @brief Hook for global layered weather control.
         * @param globalLayerWeatherID Target global weather layer.
         * @param weatherCondition Weather properties.
         * @param spatialWeatherCondition Spatial extent and transition settings.
         */
        virtual void SendWeatherControl(sbio::GlobalLayeredWeatherID globalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition,
                                        const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) {};
        /**
         * @brief Hook for regional layered weather control.
         * @param regionID Target region identifier.
         * @param regionlLayeredWeatherID Target weather layer within the region.
         * @param weatherCondition Weather properties.
         * @param spatialWeatherCondition Spatial extent and transition settings.
         */
        virtual void SendWeatherControl(sbio::RegionID regionID, sbio::RegionalLayeredWeatherID regionlLayeredWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition,
                                        const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) {};
        /**
         * @brief Hook for entity weather control.
         * @param entityID Target entity identifier.
         * @param weatherCondition Weather properties associated with the entity.
         */
        virtual void SendWeatherControl(sbio::EntityID entityID, const sbio::cigi::SCigiWeatherCondition& weatherCondition) {};
        /// @}

        /**
         * @brief Copies a serialized packet into the send buffer or bounded overflow queue.
         * @param packet Readable packet bytes in wire byte order; not retained after this call.
         * @param nSize Number of bytes to copy, in the range 1..65507.
         * @return `true` when copied into either queue; `false` for null data, invalid size, or queue limits.
         *         Acceptance does not guarantee delivery or room alongside IG Control in a datagram.
         */
        bool Pack(const void* packet, int nSize);

        /**
         * @brief Appends a base packet followed by its record payload to the send buffer.
         * @param basePacket Readable base packet bytes in wire byte order.
         * @param nBasePacketSize Number of base packet bytes.
         * @param recordsPpacket Readable record bytes in wire byte order.
         * @param nRecordsPacketSize Number of record bytes.
         * @return `true` when the combined bytes are accepted by `Pack()`; `false` for oversize data or rejection.
         *         If either block is null or has a nonpositive size, only the other block is passed to `Pack()`.
         */
        bool Pack(const void* basePacket, int nBasePacketSize, const void* recordsPpacket, int nRecordsPacketSize);

        SHostSetupOptions hostSetupOptions;///< Session configuration consumed by initialization and protocol handling.
        sbio::FrameNumber m_LastReceivedIGFrame = sbio::FrameNumber(0);///< Last decoded IG frame; cleared by reset.

      protected:
        /**
         * @brief Stamps the session ID onto event arguments and raises a host CIGI event.
         * @param args Mutable event payload; must match its `eEvent` discriminator.
         */
        void RaiseSessionEvent(HostCigiEventArgs& args) const;
        /**
         * @brief Updates database-loading state from a decoded start-of-frame database number.
         * @param reportedDatabaseNumber IG report: negative values indicate loading, -128 failure,
         *                               zero no database, and matching positive values completion.
         */
        void UpdateDatabaseState(sbio::cigi::CigiDatabaseNumber reportedDatabaseNumber);
        /**
         * @brief Clears outgoing queues, pending database notification, and line-of-sight coordinate mappings.
         *
         * Derived implementations may also clear version-specific cached state. Does not close sockets.
         */
        virtual void ClearSessionData();
        /**
         * @brief Converts elapsed session time to a CIGI timestamp.
         * @param timestamp Receives elapsed 10-microsecond ticks modulo 2^32, or zero when unavailable.
         * @return `true` if the session stopwatch exists and is running; otherwise `false`.
         */
        bool GetHostTimestamp(uint32_t& timestamp) const;

        /**
         * @brief Remembers the requested response coordinate system for a line-of-sight request.
         * @param requestID Request identifier; replaces any prior mapping with this identifier.
         * @param eResponseCoordinateSystem Coordinate system expected in the response.
         */
        void StoreLineOfSightRequestCoordinateSystem(sbio::LineOfSightRequestID requestID, sbio::ETopLevelCoordinateSystem eResponseCoordinateSystem);

        /**
         * @brief Looks up the coordinate system previously associated with a line-of-sight request.
         * @param requestID Request id value.
         * @return Stored coordinate system, or `ETopLevelCoordinateSystem::UNKNOWN` if not recorded.
         */
        sbio::ETopLevelCoordinateSystem GetLineOfSightRequestCoordinateSystem(sbio::LineOfSightRequestID requestID) const;

        /**
         * @brief Calls `SendPackets()` when the session is synchronous; otherwise does nothing.
         */
        void NotifyStartOfFrameReceived();

        /**
         * @brief Queues a packet for later transmission when it does not fit in the active send buffer.
         * @param packet Readable serialized packet bytes to copy.
         * @param nSize Number of bytes to copy.
         * @return `true` if queued; `false` for null data, nonpositive size, or exceeding 4 MiB/1024 entries.
         *         Queue-limit failures raise an error event. This helper does not check UDP packet size.
         */
        bool QueueOverflowPacket(const void* packet, int nSize);

        /**
         * @brief Moves complete queued packets to the active buffer in FIFO order until the next cannot fit.
         */
        void MoveQueuedPacketsToSendBuffer();

        /** @brief Discards both active send-buffer contents and all overflow packets. */
        void ClearQueuedPackets();

      protected:
        bool m_bConnected = false;
        bool m_bValidPacketReceived = false;
        bool m_bHasReportedWaitingForConnection = false;
        std::unique_ptr<sbio::utils::CStopWatch> m_pDisconnectedTimer;
        sbio::FrameNumber m_HostFrameNumber = sbio::FrameNumber(0);// host frame number is unique to each session
        sbio::SessionID m_SessionID = sbio::SessionID(0);

        sbio::cigi::EIGMode m_DesiredIGMode = sbio::cigi::EIGMode::UNKNOWN;
        sbio::cigi::EIGMode m_ActualIGMode = sbio::cigi::EIGMode::UNKNOWN;

        EHostSessionDatabaseState m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;

        bool m_bIGControlledDatabaseRequested = false;
        std::optional<sbio::DatabaseID> m_PendingDatabaseLoadedNotification;

        int m_nSendBufferLength = 0;
        char m_sendBuffer[MAX_UDP_SIZE];// holds packed packets to be sent
        std::list<std::unique_ptr<sbio::utils::TBuffer<char>>> m_OverflowBuffers;
        int m_nOverflowBytes = 0;

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
