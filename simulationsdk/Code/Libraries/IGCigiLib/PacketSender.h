//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketSender.h
 * @brief Declares the CCigiPacketSender class for SimBlocks CIGI IG packet sending and network communication.
 *
 * Provides the CCigiPacketSender class for sending simulation packets over UDP in the SimBlocks IGCigiLib library.
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
#include <cstddef>
#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>

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
         * @brief Sends one UDP datagram containing Start of Frame and complete queued response packets.
         *
         * Excess responses remain queued in FIFO order. A failed socket send retains all pending responses.
         * Successful transmission removes only the response packets included in that datagram.
         */
        void SendPackets();

        /** @brief Discards pending response packets at a session boundary. Call on the IG thread. */
        void ClearPendingResponses();

      protected:
        /** @brief Copies a complete serialized response into the bounded pending queue.
         * @param data Readable packet bytes; copied and not retained by pointer.
         * @param size Number of bytes to copy.
         * @return `true` if copied; `false` for null/empty data, a packet that cannot fit alongside the
         *         larger supported Start of Frame packet in 65507 bytes, or exceeding the 4 MiB queue limit.
         */
        bool QueuePacket(const void* data, std::size_t size);

        /** @brief Queues the object representation of a fixed-size packet.
         * @tparam TPacket Wire packet type whose complete representation occupies `sizeof(TPacket)` bytes.
         * @param packet Serialized packet to copy.
         * @return Result of the byte-oriented `QueuePacket()` overload; not a delivery acknowledgment.
         */
        template <typename TPacket>
        bool QueuePacket(const TPacket& packet)
        {
          return QueuePacket(&packet, sizeof(packet));
        }

      protected:
        CCigiImageGenerator& m_ImageGenerator;///< Non-owning image generator supplying outbound state.
        std::unique_ptr<sbio::utils::CUDPSendSocket> m_pSocketIGToHost;///< Owned UDP socket used for IG-to-host traffic.
        std::unique_ptr<sbio::utils::CBufferWriter> m_pBuffer;///< Owned serialization buffer used to build outbound packets.
        std::deque<std::vector<char>> m_PendingPackets;///< Complete response packets awaiting transmission.
        std::size_t m_nPendingBytes = 0;
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
