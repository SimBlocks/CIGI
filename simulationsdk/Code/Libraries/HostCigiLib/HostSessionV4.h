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
         * @brief Returns the result of `ProcessPacket`.
         * @param buffer Buffer containing the data to read or write.
         * @param nRemainingBytes Remaining bytes numeric value.
         * @return Number of bytes consumed while processing the packet.
         */
        virtual int ProcessPacket(uint8_t* buffer, int nRemainingBytes) override;
        /**
         * @brief Gets outgoing packet size.
         * @param buffer Buffer containing the data to read or write.
         * @param nRemainingBytes Remaining bytes numeric value.
         * @return Outgoing packet size value.
         */
        virtual int GetOutgoingPacketSize(const uint8_t* buffer, int nRemainingBytes) const override;
        /**
         * @brief Sends igcontrol.
         * @param pBuffer Buffer pointer used by the operation.
         */
        virtual void SendIGControl(uint8_t*& pBuffer) override;
        /// @}

        /// @name Response packet parsers
        /// These helpers decode individual inbound CIGI 4.0 packets and raise host events.
        /// @{
        /**
         * @brief Parses start of frame packet.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseStartOfFramePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseHatHotResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseHatHotResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseHatHotExtendedResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseHatHotExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseLineOfSightResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseLineOfSightResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseLineOfSightExtendedResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseLineOfSightExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseSensorResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseSensorResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseSensorExtendedResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseSensorExtendedResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParsePositionResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParsePositionResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseWeatherConditionsResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseWeatherConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseAerosolConcentrationResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseAerosolConcentrationResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseMaritimeSurfaceConditionsResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseMaritimeSurfaceConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseTerrestrialSurfaceConditionsResponsePacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseTerrestrialSurfaceConditionsResponsePacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseCollisionDetectionSegmentNotificationPacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseCollisionDetectionSegmentNotificationPacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseCollisionDetectionVolumeNotificationPacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseCollisionDetectionVolumeNotificationPacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseAnimationStopNotificationPacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseAnimationStopNotificationPacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseEventNotificationPacket` instance.
         * @param buffer Buffer containing the data to read or write.
         */
        void ParseEventNotificationPacket(uint8_t* buffer);
        /**
         * @brief Constructs a `ParseImageGeneratorMessagePacket` instance.
         * @param buffer Buffer containing the data to read or write.
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
        virtual void SendWeatherControl(sbio::GlobalLayeredWeatherID globalLayerWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) override;
        /**
         * @brief Sends weather control.
         * @param regionID Region id value.
         * @param regionlLayeredWeatherID Regionl layered weather id value.
         * @param weatherCondition Weather condition value.
         * @param spatialWeatherCondition Spatial weather condition value.
         */
        virtual void SendWeatherControl(sbio::RegionID regionID, sbio::RegionalLayeredWeatherID regionlLayeredWeatherID, const sbio::cigi::SCigiWeatherCondition& weatherCondition, const sbio::cigi::SCigiSpatialWeatherCondition& spatialWeatherCondition) override;
        /**
         * @brief Sends weather control.
         * @param entityID Entity id value.
         * @param weatherCondition Weather condition value.
         */
        virtual void SendWeatherControl(sbio::EntityID entityID, const sbio::cigi::SCigiWeatherCondition& weatherCondition) override;

        /**
         * @brief Sets igcontrol.
         * @param databaseID Database id value.
         * @param bEntityTypeSubstitutionEnabled Whether entity type substitution enabled.
         * @param eIGMode Igmode enumeration value.
         * @param bSmoothingEnabled Whether smoothing enabled.
         * @return `true` when the IG control packet state is updated; otherwise `false`.
         */
        virtual bool SetIGControl(sbio::cigi::CigiDatabaseNumber databaseID, bool bEntityTypeSubstitutionEnabled, sbio::cigi::EIGMode eIGMode, bool bSmoothingEnabled) override;
        /**
         * @brief Handles the host cigi sensor response event event.
         * @param args Event payload supplied with the dispatch.
         */
        virtual void OnHostCigiSensorResponseEvent(const HostCigiSensorResponseEventArgs& args);

      protected:
        /**
         * @brief Returns the minimum valid payload size for a CIGI 4.0 opcode.
          * @param eOpCode Op code enumeration value.
          * @return Minimum incoming packet size v4 value.
         */
        int GetMinimumIncomingPacketSizeV4(ECigiOpCodeV4 eOpCode) const;

        /**
         * @brief Reads a CIGI 4.0 packet header and applies byte swapping when required.
          * @param buffer Buffer containing the data to read or write.
          * @param bByteSwap Whether byte swap.
          * @return Packet header v4 value.
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
