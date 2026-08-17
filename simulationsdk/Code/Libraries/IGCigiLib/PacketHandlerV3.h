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
       * @brief Receives and dispatches CIGI 3.3 host-to-IG packets.
       *
       * Decodes the packet types supported by CIGI 3.3 and forwards their
       * contents to the corresponding SDK handlers.
       */
      class CCigiPacketHandlerV3 : public CCigiPacketHandler
      {
      public:
        /**
         * @brief Constructs a packet handler (version 3).
         * @param imageGenerator Reference to the image generator.
         * @param sHostIP Host IP address string used for IG-to-host traffic.
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
        /** @brief Parses an articulated-part control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseArticulatedPartControlPacket(uint8_t* pBuffer);
        /** @brief Parses an atmosphere control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseAtmosphereControlPacket(uint8_t* pBuffer);
        /** @brief Parses a celestial-sphere control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseCelestialSphereControlPacket(uint8_t* pBuffer);
        /** @brief Parses a collision-detection segment-definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseCollisionDetectionSegmentDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a collision-detection volume-definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseCollisionDetectionVolumeDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a component control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseComponentControlPacket(uint8_t* pBuffer);
        /** @brief Parses a conformal-clamped entity control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseConformalClampedEntityControlPacket(uint8_t* pBuffer);
        /** @brief Parses an earth-reference-model definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseEarthReferenceModelDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses an entity control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseEntityControlPacket(uint8_t* pBuffer);
        /** @brief Parses an environmental-region control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseEnvironmentalRegionControl(uint8_t* pBuffer);
        /** @brief Parses a HAT/HOT request packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseHatHotRequestPacket(uint8_t* pBuffer);
        /** @brief Parses an IG control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseIgControlPacket(uint8_t* pBuffer);
        /** @brief Parses a line-of-sight segment request packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseLineOfSightSegmentRequestPacket(uint8_t* pBuffer);
        /** @brief Parses a line-of-sight vector request packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseLineOfSightVectorRequestPacket(uint8_t* pBuffer);
        /** @brief Parses a maritime-surface-conditions control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseMaritimeSurfaceConditionsControlPacket(uint8_t* pBuffer);
        /** @brief Parses a sensor control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSensorControlPacket(uint8_t* pBuffer);
        /** @brief Parses a motion-tracker control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseMotionTrackerControlPacket(uint8_t* pBuffer);
        /** @brief Parses a short articulated-part control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseShortArticulatedPartControlPacket(uint8_t* pBuffer);
        /** @brief Parses a short component control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseShortComponentControlPacket(uint8_t* pBuffer);
        /** @brief Parses a short symbol control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseShortSymbolControlPacket(uint8_t* pBuffer);
        /** @brief Parses a position request packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParsePositionRequestPacket(uint8_t* pBuffer);
        /** @brief Parses an environmental-conditions request packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseEnvironmentalConditionsRequestPacket(uint8_t* pBuffer);
        /** @brief Parses a symbol-surface definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSymbolSurfaceDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a variable-length symbol-text definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSymbolTextDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a variable-length symbol-circle definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSymbolCircleDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a variable-length symbol-line definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSymbolLineDefinitonPacket(uint8_t* pBuffer);
        /** @brief Parses a symbol-clone packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSymbolClonePacket(uint8_t* pBuffer);
        /** @brief Parses a symbol control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseSymbolControlPacket(uint8_t* pBuffer);
        /** @brief Parses a terrestrial-surface-conditions control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseTerrestrialSurfaceConditionsControlPacket(uint8_t* pBuffer);
        /** @brief Parses a view control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseViewControlPacket(uint8_t* pBuffer);
        /** @brief Parses a view-definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseViewDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a wave control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseWaveControlPacket(uint8_t* pBuffer);
        /** @brief Parses a weather control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseWeatherControlPacket(uint8_t* pBuffer);
        /** @brief Parses a trajectory-definition packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseTrajectoryDefinitionPacket(uint8_t* pBuffer);
        /** @brief Parses a rate-control packet.
         * @param pBuffer Pointer to the beginning of the packet data.
         * @return No value.
         */
        virtual void ParseRateControlPacket(uint8_t* pBuffer);
        /// @}

        /**
         * @brief Receives, validates, and dispatches available CIGI 3.3 host-to-IG packets.
         * @return No value.
         */
        virtual void ProcessPackets() override;

      private:
        // Type definition for a member function pointer to a packet handler function.
        typedef void (CCigiPacketHandlerV3::*TPacketHandlerFunction)(uint8_t*);

        // Packet handler entry for opcode-to-parser dispatch table.
        struct SPacketHandlerEntry
        {
          TPacketHandlerFunction function;
          size_t minimumPacketSize;
          size_t maximumPacketSize;
        };

        // Opcode-to-parser dispatch table for CIGI 3.3 packets.
        typedef std::unordered_map<ECigiOpCodeV3, SPacketHandlerEntry> TPacketHandlerFunctions;

        // Registers a fixed-length packet with a specific size requirement.
        template <typename TPacket>
        void RegisterPacket(ECigiOpCodeV3 opCode, TPacketHandlerFunction function)
        {
          m_PacketHandlerFunctions[opCode] = {function, sizeof(TPacket), sizeof(TPacket)};
        }

        // Registers a variable-length packet with a minimum data length requirement.
        template <typename TPacket>
        void RegisterVariablePacket(ECigiOpCodeV3 opCode, TPacketHandlerFunction function, size_t minimumDataLength = 0)
        {
          m_PacketHandlerFunctions[opCode] = {function, TPacket::kBasePacketSize + minimumDataLength, sizeof(TPacket)};
        }

        TPacketHandlerFunctions m_PacketHandlerFunctions;///< Opcode-to-parser dispatch table for CIGI 3.3 packets.
        size_t m_CurrentPacketSize = 0;///< Validated size of the packet currently being dispatched.
        uint8_t m_MessageBuffer[MAX_UDP_DATAGRAM_SIZE] = {0};///< Temporary storage for the inbound datagram currently being parsed.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
