//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPReceiveMulticastSocket.cpp
 * @brief Implements UDP multicast datagram receiving functionality.
 *
 * This file provides the implementation of the CUDPReceiveMulticastSocket class, which
 * manages joining a multicast group and receiving UDP datagrams using Poco::Net::MulticastSocket.
 * Supports specifying a network interface and non-blocking receive operations.
 *
 * @see sbio::utils::CUDPReceiveMulticastSocket
 */
#include "UDPReceiveMulticastSocket.h"
#include "Poco/Exception.h"
#include "UtilitiesLib.h"
#include "Logger.h"
#include <exception>

using namespace Poco::Net;
using namespace sbio::utils;

// Constructs a multicast socket and joins the specified group.
CUDPReceiveMulticastSocket::CUDPReceiveMulticastSocket(const std::string& sIPAddress, int nPort, const std::string& sNetworkInterface)
{
  try
  {
    IPAddress ipAddress = Poco::Net::IPAddress(sIPAddress);
    m_pMulticastSocket = std::make_unique<MulticastSocket>();
    m_pMulticastSocket->bind(SocketAddress(IPAddress(), static_cast<Poco::UInt16>(nPort)), true);

    bool bJoinedGroup = false;

    if (!sNetworkInterface.empty())
    {
      const NetworkInterface::Map map = NetworkInterface::map();

      for (auto it = map.begin(); it != map.end(); ++it)
      {
        const NetworkInterface& networkInterface = it->second;

        if (networkInterface.name() == sNetworkInterface)
        {
          m_pMulticastSocket->joinGroup(ipAddress, networkInterface);
          bJoinedGroup = true;
          break;
        }
      }
    }

    if (!bJoinedGroup)
    {
      m_pMulticastSocket->joinGroup(ipAddress);
    }

    m_pMulticastSocket->setBlocking(false);
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP multicast receive socket construction failed: " + exception.displayText());
    }
    m_pMulticastSocket.reset();
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP multicast receive socket construction failed: " + std::string(exception.what()));
    }
    m_pMulticastSocket.reset();
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP multicast receive socket construction failed with an unknown error.");
    }
    m_pMulticastSocket.reset();
  }
}

// Receives a datagram from the multicast group.
int CUDPReceiveMulticastSocket::Receive(char* buffer, int length)
{
  if (!m_pMulticastSocket || buffer == nullptr || length <= 0)
  {
    return -1;
  }

  try
  {
    return m_pMulticastSocket->receiveBytes(buffer, length);
  }
  catch (const Poco::Exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP multicast receive socket receive failed: " + exception.displayText());
    }
  }
  catch (const std::exception& exception)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP multicast receive socket receive failed: " + std::string(exception.what()));
    }
  }
  catch (...)
  {
    if (g_UtilitiesGlobals.pLogger != nullptr)
    {
      g_UtilitiesGlobals.pLogger->LogError("UDP multicast receive socket receive failed with an unknown error.");
    }
  }

  return -1;
}

// Destructor. Cleans up the multicast socket.
CUDPReceiveMulticastSocket::~CUDPReceiveMulticastSocket()
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
