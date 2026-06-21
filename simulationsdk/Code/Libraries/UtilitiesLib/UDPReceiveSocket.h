//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPReceiveSocket.h
 * @brief Declares a class for receiving UDP datagrams.
 *
 * Defines the CUDPReceiveSocket class, which extends CUDPSocket to provide methods
 * for receiving UDP datagrams and polling for data on a specified port.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_LIB_UDP_RECEIVE_SOCKET_H
#define SIMBLOCKS_UTILITIES_LIB_UDP_RECEIVE_SOCKET_H

#include "UDPSocket.h"

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Non-blocking UDP socket wrapper bound to a local port.
     *
     * Construction creates a `Poco::Net::DatagramSocket` bound to the requested port and configures it
     * for non-blocking operation.
     *
     * @failurecases Construction failures are caught internally, logged through the global logger when available, and leave the object inactive.
     */
    class CUDPReceiveSocket : public CUDPSocket
    {
    public:
      /**
       * @brief Creates a UDP receive socket bound to a local port.
       * @param nPort Port number to bind.
       *
       * @sideeffects Allocates and binds Poco socket objects on success.
       * @failurecases On construction failure, socket state is reset and later receive operations fail gracefully.
       */
      CUDPReceiveSocket(int nPort);

      /**
       * @brief Destroys the receive socket wrapper.
       */
      ~CUDPReceiveSocket();

      /**
       * @brief Receives one datagram payload.
       * @param buffer Caller-provided destination buffer.
       * @param length Maximum number of bytes to receive.
       * @return Number of bytes received, or `-1` when no valid socket is available or an error occurs.
       *
       * @ownership `buffer` remains owned by the caller.
       * @sideeffects Writes received bytes into `buffer` on success.
       * @failurecases If the socket is inactive, `buffer` is `nullptr`, or `length <= 0`, returns `-1`.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through the global logger when available, and the function returns `-1`.
       */
      int Receive(char* buffer, int length);

      /**
       * @brief Polls for readable data.
       * @return `true` if the socket reports readable data within the implementation's 250 ms poll interval; otherwise `false`.
       *
       * @failurecases If the socket is inactive, returns `false`.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through the global logger when available, and the function returns `false`.
       */
      bool Poll();
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
