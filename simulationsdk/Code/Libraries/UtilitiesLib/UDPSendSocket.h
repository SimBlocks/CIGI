//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPSendSocket.h
 * @brief Declares a class for sending UDP datagrams.
 *
 * Defines the CUDPSendSocket class, which extends CUDPSocket to provide methods
 * for sending UDP datagrams to a specified IP address and port.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_LIB_UDP_SEND_SOCKET_H
#define SIMBLOCKS_UTILITIES_LIB_UDP_SEND_SOCKET_H

#include "UDPSocket.h"

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Non-blocking UDP socket wrapper for datagram transmission.
     *
     * Construction creates a `Poco::Net::DatagramSocket`, configures the destination `SocketAddress`,
     * and sets the socket to non-blocking mode.
     *
     * @failurecases Construction failures are caught internally, logged through the global logger when available, and leave the object inactive.
     */
    class CUDPSendSocket : public CUDPSocket
    {
    public:
      /**
       * @brief Creates a UDP send socket for a destination endpoint.
       * @param sIP Destination IP address.
       * @param nPort Destination port number.
       *
       * @sideeffects Allocates and configures Poco socket objects on success.
       * @failurecases On construction failure, socket state is reset and later `Send()` calls become no-ops.
       */
      CUDPSendSocket(const std::string& sIP, int nPort);

      /**
       * @brief Sends a datagram from a raw byte buffer.
       * @param buffer Pointer to the bytes to send.
       * @param nLength Number of bytes to send.
       *
       * @ownership `buffer` remains owned by the caller.
       * @sideeffects Writes a datagram to the configured destination when the socket is active.
       * @failurecases If the socket is inactive, `buffer` is `nullptr`, or `nLength <= 0`, the function returns without sending.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through the global logger when available.
       */
      void Send(const char* buffer, int nLength) const;

      /**
       * @brief Sends a datagram from a string.
       * @param s String whose bytes are sent without a terminating null character.
       *
       * @ownership `s` remains owned by the caller.
       * @sideeffects Writes a datagram to the configured destination when the socket is active.
       * @failurecases If the socket is inactive, the function returns without sending.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through the global logger when available.
       */
      void Send(const std::string& s) const;
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
