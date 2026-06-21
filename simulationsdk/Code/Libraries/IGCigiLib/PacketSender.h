//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketSender.h
 * @brief Declares the CCigiPacketSender class for SimBlocks CIGI IG packet sending and network communication.
 *
 * Provides the CCigiPacketSender class for sending simulation packets over UDP in the SimBlocks CIGI IG library.
 * Inherits from IPacketSender and integrates with SimBlocks CIGI, image generator, and utility types for simulation messaging and network communication.
 * Supports packet sending, buffer management, and socket communication for simulation interoperability.
 *
 * @see sbio::cigi::ig::CCigiPacketSender
 * @see sbio::cigi::ig::IPacketSender
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::utils::CUDPSendSocket
 * @see sbio::utils::CBufferWriter
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LIB_PACKET_SENDER_H
#define SIMBLOCKS_CIGI_LIB_PACKET_SENDER_H

#include "IPacketSender.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/BufferWriter.h"
#include <memory>
#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Base class for one IG-to-host UDP sender.
       *
       * Manages the transport socket and shared serialization buffer used by concrete CIGI
       * version-specific packet senders.
       */
      class CCigiPacketSender : public IPacketSender
      {
      public:
        /**
         * @brief Constructs a packet sender.
         * @param imageGenerator Reference to the image generator.
         * @param hostIPAddress Host IP address string.
         * @param igToHostPort IG-to-host port number.
         */
        CCigiPacketSender(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort);
        /**
         * @brief Destroys the packet sender.
         */
        virtual ~CCigiPacketSender();

        /**
         * @brief Sends simulation packets over UDP.
         *
         * Concrete subclasses determine which logical response packets are serialized before this
         * transport step is invoked.
         */
        void SendPackets();

      protected:
        CCigiImageGenerator& m_ImageGenerator;///< Non-owning image generator supplying outbound state.
        std::unique_ptr<sbio::utils::CUDPSendSocket> m_pSocketIGToHost;///< Owned UDP socket used for IG-to-host traffic.
        std::unique_ptr<sbio::utils::CBufferWriter> m_pBuffer;///< Owned serialization buffer used to build outbound packets.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
