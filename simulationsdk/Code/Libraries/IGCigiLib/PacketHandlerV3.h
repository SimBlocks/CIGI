//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketHandlerV3.h
 * @brief Declares the CCigiPacketHandlerV3 class for SimBlocks CIGI IG packet parsing and processing (version 3).
 *
 * Provides the CCigiPacketHandlerV3 class for parsing and processing CIGI IG packets (version 3) in the SimBlocks CIGI IG library.
 * Inherits from CCigiPacketHandler and integrates with SimBlocks CIGI, image generator, and handler types for simulation and packet management.
 * Supports parsing of various control, definition, request, and notification packets for simulation interoperability.
 *
 * @see sbio::cigi::ig::CCigiPacketHandlerV3
 * @see sbio::cigi::ig::CCigiPacketHandler
 * @see sbio::cigi::ig::CCigiImageGenerator
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_PACKET_HANDLER_V3_H
#define SIMBLOCKS_CIGI_PACKET_HANDLER_V3_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "PacketHandler.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief CIGI 3.3 packet handler implementation.
       */
      class CCigiPacketHandlerV3 : public CCigiPacketHandler
      {
      public:
        /**
         * @brief Constructs a packet handler (version 3).
         * @param imageGenerator Reference to the image generator.
         * @param sHostIP Host IP address string.
         * @param nHostToIgPort Host-to-IG port number.
         * @param nIgToHostPort IG-to-host port number.
         */
        CCigiPacketHandlerV3(CCigiImageGenerator& imageGenerator, const std::string& sHostIP, int nHostToIgPort, int nIgToHostPort);
        /**
         * @brief Destroys the packet handler (version 3).
         */
        ~CCigiPacketHandlerV3();

        /// @name Packet parsing helpers
        /// These helpers decode one CIGI 3.3 packet payload and dispatch the resulting SDK struct.
        /// @{
        virtual void ParseArticulatedPartControlPacket(uint8_t* pBuffer);
        virtual void ParseAtmosphereControlPacket(uint8_t* pBuffer);
        virtual void ParseCelestialSphereControlPacket(uint8_t* pBuffer);
        virtual void ParseCollisionDetectionSegmentDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseCollisionDetectionVolumeDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseComponentControlPacket(uint8_t* pBuffer);
        virtual void ParseConformalClampedEntityControlPacket(uint8_t* pBuffer);
        virtual void ParseEarthReferenceModelDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseEntityControlPacket(uint8_t* pBuffer);
        virtual void ParseEnvironmentalRegionControl(uint8_t* pBuffer);
        virtual void ParseHatHotRequestPacket(uint8_t* pBuffer);
        virtual void ParseIgControlPacket(uint8_t* pBuffer);
        virtual void ParseLineOfSightSegmentRequestPacket(uint8_t* pBuffer);
        virtual void ParseLineOfSightVectorRequestPacket(uint8_t* pBuffer);
        virtual void ParseMaritimeSurfaceConditionsControlPacket(uint8_t* pBuffer);
        virtual void ParseSensorControlPacket(uint8_t* pBuffer);
        virtual void ParseMotionTrackerControlPacket(uint8_t* pBuffer);
        virtual void ParseShortArticulatedPartControlPacket(uint8_t* pBuffer);
        virtual void ParseShortComponentControlPacket(uint8_t* pBuffer);
        virtual void ParseShortSymbolControlPacket(uint8_t* pBuffer);
        virtual void ParsePositionRequestPacket(uint8_t* pBuffer);
        virtual void ParseEnvironmentalConditionsRequestPacket(uint8_t* pBuffer);
        virtual void ParseSymbolSurfaceDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolTextDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolCircleDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolLineDefinitonPacket(uint8_t* pBuffer);
        virtual void ParseSymbolClonePacket(uint8_t* pBuffer);
        virtual void ParseSymbolControlPacket(uint8_t* pBuffer);
        virtual void ParseTerrestrialSurfaceConditionsControlPacket(uint8_t* pBuffer);
        virtual void ParseViewControlPacket(uint8_t* pBuffer);
        virtual void ParseViewDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseWaveControlPacket(uint8_t* pBuffer);
        virtual void ParseWeatherControlPacket(uint8_t* pBuffer);
        virtual void ParseTrajectoryDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseRateControlPacket(uint8_t* pBuffer);
        /// @}

        /**
         * @brief Processes incoming packets.
         */
        virtual void ProcessPackets() override;

      private:
        typedef void (CCigiPacketHandlerV3::*TPacketHandlerFunction)(uint8_t*);
        typedef std::unordered_map<ECigiOpCodeV3, TPacketHandlerFunction> TPacketHandlerFunctions;
        TPacketHandlerFunctions m_PacketHandlerFunctions;///< Opcode-to-parser dispatch table for CIGI 3.3 packets.
        uint8_t m_MessageBuffer[65535] = {0};///< Temporary storage for the inbound datagram currently being parsed.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
