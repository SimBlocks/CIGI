//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPReceiveMulticastSocket.h
 * @brief Declares a class for receiving UDP multicast datagrams.
 *
 * Defines the CUDPReceiveMulticastSocket class, which provides methods for joining
 * a multicast group and receiving UDP datagrams using Poco::Net::MulticastSocket.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_LIB_UDP_RECEIVE_MULTICAST_SOCKET_H
#define SIMBLOCKS_UTILITIES_LIB_UDP_RECEIVE_MULTICAST_SOCKET_H

#include "Poco/Net/MulticastSocket.h"
#include <memory>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Non-blocking multicast UDP receive socket.
     *
     * Construction creates and binds a `Poco::Net::MulticastSocket`, joins the requested multicast group,
     * and sets the socket to non-blocking mode. The socket and bind address match the group's IP family.
     * A non-empty interface name selects an interface supporting that family; an empty name uses Poco's default behavior.
     *
     * @invariant `m_pMulticastSocket == nullptr` means the object is inactive.
     */
    class CUDPReceiveMulticastSocket
    {
    public:
      /**
       * @brief Creates a multicast receive socket and joins a multicast group.
       * @param sIPAddress IPv4 or IPv6 multicast group address.
       * @param nPort Local port in [0, 65535]; zero requests an ephemeral port.
       * @param sNetworkInterface Optional Poco network-interface name to use when joining the group.
       *
       * @sideeffects Binds a UDP socket, joins a multicast group, and sets non-blocking mode on success.
       * @failurecases Invalid port numbers leave the socket inactive.
       * @failurecases A named interface that is missing or does not support the group family leaves the socket inactive.
       * @failurecases Construction failures are caught internally, logged through the global logger when available, and leave the object inactive.
       */
      CUDPReceiveMulticastSocket(const std::string& sIPAddress, int nPort, const std::string& sNetworkInterface = "");

      /**
       * @brief Destroys the multicast receive socket wrapper.
       */
      ~CUDPReceiveMulticastSocket();

      /**
       * @brief Receives one multicast datagram payload.
       * @param buffer Caller-provided destination buffer.
       * @param length Maximum number of bytes to receive.
       * @return Number of bytes received, or `-1` when the socket is inactive or an error occurs.
       *
       * @ownership `buffer` remains owned by the caller.
       * @sideeffects Writes received bytes into `buffer` on success.
       * @failurecases If the socket is inactive, `buffer` is `nullptr`, or `length <= 0`, returns `-1`.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through the global logger when available, and the function returns `-1`.
       */
      int Receive(char* buffer, int length);

    protected:
      std::unique_ptr<Poco::Net::MulticastSocket> m_pMulticastSocket;///< Owning pointer to the underlying multicast socket, or `nullptr` when construction failed.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
