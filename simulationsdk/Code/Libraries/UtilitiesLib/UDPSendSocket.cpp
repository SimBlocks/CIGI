//Copyright SimBlocks LLC 2016-2026
/**
 * @file UDPSendSocket.cpp
 * @brief Implements UDP datagram sending functionality.
 *
 * This file provides the implementation of the CUDPSendSocket class, which extends
 * CUDPSocket to support sending UDP datagrams to a specified IP address and port.
 *
 * @see sbio::utils::CUDPSendSocket
 */
#include "UDPSendSocket.h"
#include "Poco/Exception.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "UtilitiesLib.h"
#include "Logger.h"
#include <exception>

using namespace std;
using namespace Poco::Net;
using namespace sbio::utils;

namespace sbio
{
  namespace utils
  {
    extern SUtilitiesGlobals g_UtilitiesGlobals;
  }
}

namespace
{
  void LogSocketError(const std::string& sText)
  {
    if (sbio::utils::g_UtilitiesGlobals.pLogger != nullptr)
    {
      sbio::utils::g_UtilitiesGlobals.pLogger->LogError(sText);
    }
  }

  void LogSocketException(const std::string& sOperation, const Poco::Exception& exception)
  {
    LogSocketError("UDP send socket " + sOperation + " failed: " + exception.displayText());
  }

  void LogSocketException(const std::string& sOperation, const std::exception& exception)
  {
    LogSocketError("UDP send socket " + sOperation + " failed: " + std::string(exception.what()));
  }
}

// Constructs a UDP send socket for the specified IP and port.
CUDPSendSocket::CUDPSendSocket(const std::string& sIP, int nPort) : CUDPSocket(sIP, nPort)
{
  try
  {
    m_pSocketAddress = make_unique<SocketAddress>(sIP, static_cast<Poco::UInt16>(nPort));
    m_pDatagramSocket = make_unique<DatagramSocket>(SocketAddress::IPv4);
    m_pDatagramSocket->setBlocking(false);
  }
  catch (const Poco::Exception& exception)
  {
    LogSocketException("construction", exception);
    m_pDatagramSocket.reset();
    m_pSocketAddress.reset();
  }
  catch (const std::exception& exception)
  {
    LogSocketException("construction", exception);
    m_pDatagramSocket.reset();
    m_pSocketAddress.reset();
  }
  catch (...)
  {
    LogSocketError("UDP send socket construction failed with an unknown error.");
    m_pDatagramSocket.reset();
    m_pSocketAddress.reset();
  }
}

// Sends a datagram using a buffer.
void CUDPSendSocket::Send(const char* buffer, int nLength) const
{
  if (!m_pDatagramSocket || !m_pSocketAddress || buffer == nullptr || nLength <= 0)
  {
    return;
  }

  try
  {
    m_pDatagramSocket->sendTo(buffer, nLength, *m_pSocketAddress);
  }
  catch (const Poco::Exception& exception)
  {
    LogSocketException("send", exception);
  }
  catch (const std::exception& exception)
  {
    LogSocketException("send", exception);
  }
  catch (...)
  {
    LogSocketError("UDP send socket send failed with an unknown error.");
  }
}

// Sends a datagram using a string.
void CUDPSendSocket::Send(const std::string& s) const
{
  if (!m_pDatagramSocket || !m_pSocketAddress)
  {
    return;
  }

  try
  {
    m_pDatagramSocket->sendTo(s.c_str(), static_cast<int>(s.length()), *m_pSocketAddress);
  }
  catch (const Poco::Exception& exception)
  {
    LogSocketException("send", exception);
  }
  catch (const std::exception& exception)
  {
    LogSocketException("send", exception);
  }
  catch (...)
  {
    LogSocketError("UDP send socket send failed with an unknown error.");
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
