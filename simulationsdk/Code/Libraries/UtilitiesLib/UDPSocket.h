//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPSocket.h
 * @brief Declares a base class for UDP socket communication.
 *
 * Defines the CUDPSocket class, which provides basic functionality for UDP socket
 * communication, including construction, closing, and blocking mode control. Used as a
 * base class for sending and receiving UDP datagrams.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_LIB_UDP_SOCKET_H
#define SIMBLOCKS_UTILITIES_LIB_UDP_SOCKET_H

#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>
#include <string>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Common state and utility operations for UDP socket wrappers.
     *
     * `CUDPSocket` stores the configured address information and the Poco socket objects used by derived
     * send and receive classes. The base constructors only record the requested address or port; actual
     * socket creation is performed by derived classes.
     *
     * @invariant `m_sIP` and `m_nPort` reflect the values supplied to construction.
     * @invariant `m_pSocketAddress` and `m_pDatagramSocket` are owning pointers and may remain null if derived construction fails.
     */
    class CUDPSocket
    {
    public:
      /**
       * @brief Initializes base state for a socket associated with a remote address.
       * @param sIP IP address string associated with the socket.
       * @param nPort Port number associated with the socket.
       */
      CUDPSocket(const std::string& sIP, int nPort);

      /**
       * @brief Initializes base state for a socket associated with a local port.
       * @param nPort Port number associated with the socket.
       */
      CUDPSocket(int nPort);

      /**
       * @brief Destroys the socket wrapper.
       *
       * Calls `Close()`.
       */
      virtual ~CUDPSocket();

      /**
       * @brief Closes the underlying UDP socket and clears owned socket objects.
       *
       * @sideeffects Closes the active Poco datagram socket, resets `m_pDatagramSocket`, and resets `m_pSocketAddress`.
       * @failurecases If no socket is open, the function returns without modification.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through `g_UtilitiesGlobals.pLogger` when available.
       */
      void Close();

      /**
       * @brief Returns the configured IP address string.
       * @return Address string supplied to construction, which may be empty for locally bound receive sockets.
       */
      std::string GetAddress() const;

      /**
       * @brief Returns the configured port number.
       * @return Port number supplied to construction.
       */
      int GetPort() const;

      /**
       * @brief Changes the blocking mode of the underlying datagram socket.
       * @param bBlocking `true` for blocking mode, `false` for non-blocking mode.
       *
       * @sideeffects Updates socket behavior when a datagram socket has been created successfully.
       * @failurecases If no socket is available, the function returns without modification.
       * @failurecases Exceptions thrown by Poco or the standard library are caught and logged through `g_UtilitiesGlobals.pLogger` when available.
       */
      void SetBlocking(bool bBlocking);

    protected:
      std::string m_sIP;///< Configured IP address string. Empty when the socket was constructed with only a port.
      int m_nPort = 0;///< Configured port number.

      std::unique_ptr<Poco::Net::SocketAddress> m_pSocketAddress;///< Owning pointer to the Poco socket address created by derived classes.
      std::unique_ptr<Poco::Net::DatagramSocket> m_pDatagramSocket;///< Owning pointer to the Poco UDP socket created by derived classes.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
