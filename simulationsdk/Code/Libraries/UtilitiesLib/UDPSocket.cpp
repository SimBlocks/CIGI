//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPSocket.cpp
 * @brief Implements base functionality for UDP socket communication.
 *
 * This file provides the implementation of the CUDPSocket class, which offers
 * basic construction, closing, and blocking mode control for UDP sockets. Used as a
 * base class for sending and receiving UDP datagrams.
 *
 * @see sbio::utils::CUDPSocket
 */
#include "UtilitiesLib/UDPSocket.h"
#include "Poco/Exception.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "UtilitiesLib.h"
#include "Logger.h"
#include <boost/core/ignore_unused.hpp>
#include <exception>

using namespace sbio::utils;

// Constructs a UDP socket for the specified IP and port.
CUDPSocket::CUDPSocket(const std::string& sIP, int nPort) : m_sIP(sIP), m_nPort(nPort)
{
}

// Constructs a UDP socket bound to the specified port.
CUDPSocket::CUDPSocket(int nPort) : m_nPort(nPort)
{
}

// Destructor. Cleans up the UDP socket.
CUDPSocket::~CUDPSocket()
{
  Close();
}

// Closes the UDP socket and releases resources.
void CUDPSocket::Close()
{
  if (!m_pDatagramSocket)
  {
    return;
  }

  try
  {
    m_pDatagramSocket->close();
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP socket close failed: " + exception.displayText());
    }
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP socket close failed: " + std::string(exception.what()));
    }
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP socket close failed with an unknown error.");
    }
  }

  m_pDatagramSocket.reset();
  m_pSocketAddress.reset();
}

std::string CUDPSocket::GetAddress() const
{
  return m_sIP;
}

int CUDPSocket::GetPort() const
{
  return m_nPort;
}

// Sets the blocking mode of the UDP socket.
void CUDPSocket::SetBlocking(bool bBlocking)
{
  if (!m_pDatagramSocket)
  {
    return;
  }

  try
  {
    m_pDatagramSocket->setBlocking(bBlocking);
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP socket set blocking mode failed: " + exception.displayText());
    }
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP socket set blocking mode failed: " + std::string(exception.what()));
    }
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP socket set blocking mode failed with an unknown error.");
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
