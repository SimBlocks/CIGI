//Copyright SimBlocks LLC 2016-2026
#include "PacketSender.h"
#include "ImageGenerator.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/Buffer.h"
#include <memory>

using namespace sbio::utils;
using namespace sbio::cigi::ig;
using namespace sbio::cigi;
using namespace sbio;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiPacketSender::CCigiPacketSender(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort) : m_ImageGenerator(imageGenerator)
{
  m_pSocketIGToHost = make_unique<CUDPSendSocket>(hostIPAddress, igToHostPort);

  m_pBuffer = std::make_unique<CBufferWriter>(MAX_UDP_DATAGRAM_SIZE);
  m_pMessageBuffer = std::make_unique<CBufferWriter>(MAX_UDP_DATAGRAM_SIZE);
}

CCigiPacketSender::~CCigiPacketSender()
{
}

void CCigiPacketSender::SendPackets()
{
  // Get the size of the data in the message buffer before swapping.
  const size_t responseSize = static_cast<size_t>(m_pBuffer->GetInUseSize());

  // Swap the main buffer and the message buffer, then reset the main buffer for new data.
  std::swap(m_pBuffer, m_pMessageBuffer);
  m_pBuffer->Reset();

  // Send a start-of-frame packet with the current frame number.
  SendStartOfFramePacket(m_ImageGenerator.GetFrameNumber());

  // If the message buffer has data, attempt to write it to the main buffer. If writing fails, swap the buffers and reset the main buffer.
  if (responseSize > 0 && !m_pBuffer->Write(m_pMessageBuffer->GetBuffer(), responseSize))
  {
    std::swap(m_pBuffer, m_pMessageBuffer);
    m_pBuffer->Reset();
    return;
  }

  // Log the serialized packet data if logging is enabled.
  const int nMessageSize = static_cast<int>(m_pBuffer->GetInUseSize());
  if (m_ImageGenerator.GetSetupOptions().bLogPacketText)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(m_ImageGenerator.GetSetupOptions().eCigiVersion, reinterpret_cast<const uint8_t*>(m_pBuffer->GetBuffer()), nMessageSize);
  }

  // Send the serialized packet data over the UDP socket to the host.
  m_pSocketIGToHost->Send(m_pBuffer->GetBuffer(), static_cast<int>(m_pBuffer->GetInUseSize()));
  m_pMessageBuffer->Reset();
  std::swap(m_pBuffer, m_pMessageBuffer);
  m_pMessageBuffer->Reset();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
