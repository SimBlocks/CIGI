//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPReceiveSocket.cpp
 * @brief Implements UDP datagram receiving functionality.
 *
 * This file provides the implementation of the CUDPReceiveSocket class, which extends
 * CUDPSocket to support receiving UDP datagrams and polling for data on a specified port.
 *
 * @see sbio::utils::CUDPReceiveSocket
 */
#include "UDPReceiveSocket.h"
#include "Poco/Exception.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "UtilitiesLib.h"
#include "Logger.h"
#include <exception>

using namespace std;
using namespace Poco::Net;
using namespace sbio::utils;

// Constructs a UDP receive socket bound to the specified port.
CUDPReceiveSocket::CUDPReceiveSocket(int nPort) : CUDPSocket(nPort)
{
  try
  {
    m_pSocketAddress = make_unique<SocketAddress>(static_cast<Poco::UInt16>(nPort));
    const SocketAddress& localSocketAddress = *m_pSocketAddress;
    m_pDatagramSocket = make_unique<DatagramSocket>(localSocketAddress, true);
    m_pDatagramSocket->setBlocking(false);
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket construction failed: " + exception.displayText());
    }
    m_pDatagramSocket.reset();
    m_pSocketAddress.reset();
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket construction failed: " + std::string(exception.what()));
    }
    m_pDatagramSocket.reset();
    m_pSocketAddress.reset();
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket construction failed with an unknown error.");
    }
    m_pDatagramSocket.reset();
    m_pSocketAddress.reset();
  }
}

// Destructor. Cleans up the UDP receive socket.
CUDPReceiveSocket::~CUDPReceiveSocket()
{
}

// Receives a datagram from the socket.
int CUDPReceiveSocket::Receive(char* buffer, int length)
{
  if (!m_pDatagramSocket || buffer == nullptr || length <= 0)
  {
    return -1;
  }

  try
  {
    return m_pDatagramSocket->receiveBytes(buffer, length);
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket receive failed: " + exception.displayText());
    }
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket receive failed: " + std::string(exception.what()));
    }
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket receive failed with an unknown error.");
    }
  }

  return -1;
}

// Polls the socket for available data.
bool CUDPReceiveSocket::Poll()
{
  if (!m_pDatagramSocket)
  {
    return false;
  }

  try
  {
    Poco::Timespan span(250000);
    return m_pDatagramSocket->poll(span, Socket::SELECT_READ);
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket poll failed: " + exception.displayText());
    }
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket poll failed: " + std::string(exception.what()));
    }
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP receive socket poll failed with an unknown error.");
    }
  }

  return false;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
