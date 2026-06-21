//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketHandlerV4.h
 * @brief Declares the CCigiPacketHandlerV4 class for SimBlocks CIGI IG packet parsing and processing (version 4).
 *
 * Provides the CCigiPacketHandlerV4 class for parsing and processing CIGI IG packets (version 4) in the SimBlocks CIGI IG library.
 * Inherits from CCigiPacketHandler and integrates with SimBlocks CIGI, image generator, and handler types for simulation and packet management.
 * Supports parsing of various control, definition, request, and notification packets for simulation interoperability, including byte swap checking and extended packet types.
 *
 * @see sbio::cigi::ig::CCigiPacketHandlerV4
 * @see sbio::cigi::ig::CCigiPacketHandler
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::ig::SCigiPacketHeaderV4
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_PACKET_HANDLER_V4_H
#define SIMBLOCKS_CIGI_PACKET_HANDLER_V4_H

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
       * @brief CIGI 4.0 packet handler implementation.
       */
      class CCigiPacketHandlerV4 : public CCigiPacketHandler
      {
      public:
        /**
         * @brief Constructs a packet handler (version 4).
         * @param imageGenerator Reference to the image generator.
         * @param sHostIP Host IP address string.
         * @param nHostToIgPort Host-to-IG port number.
         * @param nIgToHostPort IG-to-host port number.
         */
        CCigiPacketHandlerV4(CCigiImageGenerator& imageGenerator, const std::string& sHostIP, int nHostToIgPort, int nIgToHostPort);
        /**
         * @brief Destroys the packet handler (version 4).
         */
        ~CCigiPacketHandlerV4();

        /**
         * @brief Updates byte-order handling for inbound packets.
         * @param packetHeader Parsed packet header used to detect endianness.
         */
        void CheckForByteSwap(const SCigiPacketHeaderV4& packetHeader);

        /// @name Packet parsing helpers
        /// These helpers decode one CIGI 4.0 packet payload and dispatch the resulting SDK struct.
        /// @{
        virtual void ParseAccelerationControlPacket(uint8_t* pBuffer);
        virtual void ParseAnimationControlPacket(uint8_t* pBuffer);
        virtual void ParseArticulatedPartControlPacket(uint8_t* pBuffer);
        virtual void ParseAtmosphereControlPacket(uint8_t* pBuffer);
        virtual void ParseCelestialSphereControlPacket(uint8_t* pBuffer);
        virtual void ParseCollisionDetectionSegmentDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseCollisionDetectionVolumeDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseComponentControlPacket(uint8_t* pBuffer);
        virtual void ParseConformalClampedEntityPositionPacket(uint8_t* pBuffer);
        virtual void ParseEarthReferenceModelDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseEntityControlPacket(uint8_t* pBuffer);
        virtual void ParseEntityPositionPacket(uint8_t* pBuffer);
        virtual void ParseEnvironmentalConditionsRequestPacket(uint8_t* pBuffer);
        virtual void ParseEnvironmentalRegionControlPacket(uint8_t* pBuffer);
        virtual void ParseHatHotRequestPacket(uint8_t* pBuffer);
        virtual void ParseIgControlPacket(uint8_t* pBuffer);
        virtual void ParseLineOfSightSegmentRequestPacket(uint8_t* pBuffer);
        virtual void ParseLineOfSightVectorRequestPacket(uint8_t* pBuffer);
        virtual void ParseMaritimeSurfaceConditionsControlPacket(uint8_t* pBuffer);
        virtual void ParseMotionTrackerControlPacket(uint8_t* pBuffer);
        virtual void ParsePositionRequestPacket(uint8_t* pBuffer);
        virtual void ParseSensorControlPacket(uint8_t* pBuffer);
        virtual void ParseShortArticulatedPartControlPacket(uint8_t* pBuffer);
        virtual void ParseShortComponentControlPacket(uint8_t* pBuffer);
        virtual void ParseShortSymbolControlPacket(uint8_t* pBuffer);
        virtual void ParseSymbolCircleDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolClonePacket(uint8_t* pBuffer);
        virtual void ParseSymbolControlPacket(uint8_t* pBuffer);
        virtual void ParseSymbolPolygonDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolSurfaceDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolTextDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolTexturedCircleDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseSymbolTexturedPolygonDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseTerrestrialSurfaceConditionsControlPacket(uint8_t* pBuffer);
        virtual void ParseVelocityControlPacket(uint8_t* pBuffer);
        virtual void ParseViewControlPacket(uint8_t* pBuffer);
        virtual void ParseViewDefinitionPacket(uint8_t* pBuffer);
        virtual void ParseWaveControlPacket(uint8_t* pBuffer);
        virtual void ParseWeatherControlPacket(uint8_t* pBuffer);
        /// @}

        /**
         * @brief Receives and parses the currently available CIGI 4.0 host traffic.
         */
        virtual void ProcessPackets() override;

      private:
        typedef void (CCigiPacketHandlerV4::*TPacketHandlerFunction)(uint8_t*);
        typedef std::unordered_map<ECigiOpCodeV4, TPacketHandlerFunction> TPacketHandlerFunctions;
        TPacketHandlerFunctions m_PacketHandlerFunctions;///< Opcode-to-parser dispatch table for CIGI 4.0 packets.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
