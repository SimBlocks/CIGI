//Copyright SimBlocks LLC 2016-2026
#include "PacketSenders.h"
#include "PacketSenderV3.h"
#include "PacketSenderV4.h"

using namespace std;
using namespace sbio;
using namespace sbio::cigi::ig;

void CCigiPacketSenders::AddHost(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort, ECigiVersion eCigiVersion)
{
  if (eCigiVersion == ECigiVersion::VERSION_3_3)
  {
    //g_CigiLibGlobals.pLogger->LogInformation("Initializing Image Generator CIGI Version 3.3");
    unique_ptr<CCigiPacketSenderV3> pPacketSender = make_unique<CCigiPacketSenderV3>(imageGenerator, hostIPAddress, igToHostPort);
    m_pPacketSenders.push_back(std::move(pPacketSender));
  }
  else if (eCigiVersion == ECigiVersion::VERSION_4_0)
  {
    //g_CigiLibGlobals.pLogger->LogInformation("Initializing Image Generator CIGI Version 4.0");
    unique_ptr<CCigiPacketSenderV4> pPacketSender = make_unique<CCigiPacketSenderV4>(imageGenerator, hostIPAddress, igToHostPort);
    m_pPacketSenders.push_back(std::move(pPacketSender));
  }
}

void CCigiPacketSenders::SendStartOfFramePacket()
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendStartOfFramePacket();
  }
}

void CCigiPacketSenders::SendAnimationFinishedNotification(const sbio::cigi::SAnimationStopNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendAnimationFinishedNotification(response);
  }
}

void CCigiPacketSenders::SendHatResponse(const sbio::cigi::SHeightAboveTerrainResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendHatResponse(response);
  }
}

void CCigiPacketSenders::SendHotResponse(const sbio::cigi::SHeightOfTerrainResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendHotResponse(response);
  }
}

void CCigiPacketSenders::SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendHatHotExtendedResponse(response);
  }
}

void CCigiPacketSenders::SendCollisionDetectionSegmentNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendCollisionDetectionSegmentNotificationResponse(response);
  }
}

void CCigiPacketSenders::SendCollisionDetectionSegmentEntityNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendCollisionDetectionSegmentEntityNotificationResponse(response);
  }
}

void CCigiPacketSenders::SendCollisionDetectionVolumeNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendCollisionDetectionVolumeNotificationResponse(response);
  }
}

void CCigiPacketSenders::SendCollisionDetectionVolumeEntityNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendCollisionDetectionVolumeEntityNotificationResponse(response);
  }
}

void CCigiPacketSenders::SendEventNotification(const sbio::cigi::SEventNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendEventNotification(response);
  }
}

void CCigiPacketSenders::SendImageGeneratorMessageNotification(const sbio::cigi::SImageGeneratorNotification& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendImageGeneratorMessageNotification(response);
  }
}

void CCigiPacketSenders::SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendLineOfSightExtendedGeodeticCoordinatesResponse(response);
  }
}

void CCigiPacketSenders::SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendLineOfSightExtendedEntityCoordinatesResponse(response);
  }
}

void CCigiPacketSenders::SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(response);
  }
}

void CCigiPacketSenders::SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendLineOfSightResponse(response);
  }
}

void CCigiPacketSenders::SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendLineOfSightEntityResponse(response);
  }
}

void CCigiPacketSenders::SendPositionResponse(const SBasePositionResponse* response, sbio::cigi::EPositionResponseType ePositionResponseType)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendPositionResponse(response, ePositionResponseType);
  }
}

void CCigiPacketSenders::SendWeatherConditionsResponse(const SWeatherConditionsResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendWeatherConditionsResponse(response);
  }
}

void CCigiPacketSenders::SendAerosolConcentrationResponse(const SAerosolConcentrationResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendAerosolConcentrationResponse(response);
  }
}

void CCigiPacketSenders::SendMaritimeSurfaceConditionsResponse(const SMaritimeSurfaceConditionsResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendMaritimeSurfaceConditionsResponse(response);
  }
}

void CCigiPacketSenders::SendTerrestrialSurfaceConditionsResponse(const STerrestrialSurfaceConditionsResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendTerrestrialSurfaceConditionsResponse(response);
  }
}

void CCigiPacketSenders::SendSensorResponse(const SSensorResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendSensorResponse(response);
  }
}

void CCigiPacketSenders::SendSensorExtendedResponse(const SSensorExtendedResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendSensorExtendedResponse(response);
  }
}

void CCigiPacketSenders::SendSensorExtendedEntityResponse(const SSensorExtendedEntityResponse& response)
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendSensorExtendedEntityResponse(response);
  }
}

void CCigiPacketSenders::SendPackets()
{
  for (auto&& pPacketSender : m_pPacketSenders)
  {
    pPacketSender->SendPackets();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
