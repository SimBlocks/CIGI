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

  m_pBuffer = std::make_unique<CBufferWriter>(65535);
}

CCigiPacketSender::~CCigiPacketSender()
{
}

void CCigiPacketSender::SendPackets()
{
  const int nMessageSize = static_cast<int>(m_pBuffer->GetInUseSize());
  if (m_ImageGenerator.GetSetupOptions().bLogPacketText)
  {
    m_ImageGenerator.GetCigiMessageLogger()->LogMessageFromIGToHost(m_ImageGenerator.GetSetupOptions().eCigiVersion, reinterpret_cast<const uint8_t*>(m_pBuffer->GetBuffer()), nMessageSize);
  }

  m_pSocketIGToHost->Send(m_pBuffer->GetBuffer(), static_cast<int>(m_pBuffer->GetInUseSize()));
  m_pBuffer->Reset();

  //Start of Frame packet must be first in message
  SendStartOfFramePacket();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
