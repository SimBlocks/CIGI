//Copyright SimBlocks LLC 2016-2026
#include "PacketSender.h"
#include "ImageGenerator.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/Buffer.h"
#include "UtilitiesLib/Logger.h"
#include "libCIGI/Packets/3_3/SoF.h"
#include "libCIGI/Packets/4_0/SoF.h"
#include <memory>

using namespace sbio::utils;
using namespace sbio::cigi::ig;
using namespace sbio::cigi;
using namespace sbio;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

namespace
{
  constexpr int MAX_IG_UDP_PAYLOAD = 65507;
  constexpr std::size_t MAX_RESPONSE_QUEUE_BYTES = 4 * 1024 * 1024;
  constexpr std::size_t MAX_START_OF_FRAME_SIZE = sizeof(CIGI::V33::SoF) > sizeof(CIGI::V40::SoF) ? sizeof(CIGI::V33::SoF) : sizeof(CIGI::V40::SoF);
}

CCigiPacketSender::CCigiPacketSender(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort) : m_ImageGenerator(imageGenerator)
{
  m_pSocketIGToHost = make_unique<CUDPSendSocket>(hostIPAddress, igToHostPort);

  m_pBuffer = std::make_unique<CBufferWriter>(MAX_IG_UDP_PAYLOAD);
}

CCigiPacketSender::~CCigiPacketSender()
{
}

void CCigiPacketSender::ClearPendingResponses()
{
  m_PendingPackets.clear();
  m_nPendingBytes = 0;
  m_pBuffer->Reset();
}

bool CCigiPacketSender::QueuePacket(const void* data, std::size_t size)
{
  if (data == nullptr || size == 0)
  {
    return false;
  }

  if (size > MAX_IG_UDP_PAYLOAD - MAX_START_OF_FRAME_SIZE || size > MAX_RESPONSE_QUEUE_BYTES - m_nPendingBytes)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogWarning("IG response packet rejected: UDP payload or response queue limit exceeded.");
    }
    return false;
  }

  const auto* bytes = static_cast<const char*>(data);
  m_PendingPackets.emplace_back(bytes, bytes + size);
  m_nPendingBytes += size;
  return true;
}

void CCigiPacketSender::SendPackets()
{
  m_pBuffer->Reset();
  SendStartOfFramePacket(m_ImageGenerator.GetFrameNumber());

  // Append only complete packets, leaving excess responses for the next frame.
  std::size_t packetCount = 0;
  for (const auto& packet : m_PendingPackets)
  {
    if (!m_pBuffer->Write(packet.data(), packet.size()))
    {
      break;
    }
    ++packetCount;
  }

  const int nMessageSize = static_cast<int>(m_pBuffer->GetInUseSize());
  if (m_ImageGenerator.GetSetupOptions().bLogPacketText)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromIGToHost(
      m_ImageGenerator.GetSetupOptions().eCigiVersion, reinterpret_cast<const uint8_t*>(m_pBuffer->GetBuffer()), nMessageSize);
  }

  // Retain responses when the socket cannot accept the datagram.
  if (!m_pSocketIGToHost->Send(m_pBuffer->GetBuffer(), nMessageSize))
  {
    return;
  }

  for (std::size_t n = 0; n < packetCount; ++n)
  {
    m_nPendingBytes -= m_PendingPackets.front().size();
    m_PendingPackets.pop_front();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
