//Copyright SimBlocks LLC 2016-2026
/**
 * @file IGResponseEventDispatcher.cpp
 * @brief Implements response forwarding from `IGCigiLib` to packet senders and terrain events.
 *
 * Defines `CIGResponseEventDispatcher` functions that route IG-generated responses either to the active packet-sender collection
 * or to the terrain event pipeline, depending on the response type.
 *
 * @see sbio::cigi::ig::CIGResponseEventDispatcher
 * @see sbio::cigi::ig::CCigiPacketSenders
 * @see sbio::cigi::ig::TerrainCigiEvent
 */
#include "IGResponseEventDispatcher.h"
#include "IGCigiLib/TerrainCigiEvent.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/PacketSender.h"
#include "IGCigiLib/PacketSenders.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio;
using namespace sbio::cigi::ig;

extern SIGCigiLibGlobals g_CigiLibGlobals;

void CIGResponseEventDispatcher::SendCollisionDetectionSegmentNotification(const SCollisionDetectionSegmentNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendCollisionDetectionSegmentNotificationResponse(data);
}

void CIGResponseEventDispatcher::SendCollisionDetectionSegmentEntityNotification(const SCollisionDetectionSegmentEntityNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendCollisionDetectionSegmentEntityNotificationResponse(data);
}

void CIGResponseEventDispatcher::SendCollisionDetectionVolumeNotification(const SCollisionDetectionVolumeNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendCollisionDetectionVolumeNotificationResponse(data);
}

void CIGResponseEventDispatcher::SendCollisionDetectionVolumeEntityNotification(const SCollisionDetectionVolumeEntityNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendCollisionDetectionVolumeEntityNotificationResponse(data);
}

void CIGResponseEventDispatcher::SendEntityAnimationStopNotification(const SAnimationStopNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendAnimationFinishedNotification(data);
}

void CIGResponseEventDispatcher::SendEventNotification(const sbio::cigi::SEventNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendEventNotification(data);
}

void CIGResponseEventDispatcher::SendHeightAboveTerrainResponse(const sbio::cigi::SHeightAboveTerrainResponse& data)
{
  sbio::cigi::ig::CigiHatResponseEventArgs args;
  args.hatResponse = data;
  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendHeightOfTerrainResponse(const sbio::cigi::SHeightOfTerrainResponse& data)
{
  sbio::cigi::ig::CigiHotResponseEventArgs args;
  args.hotResponse = data;
  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& data)
{
  sbio::cigi::ig::CigiHatHotExtendedResponseEventArgs args;
  args.hatHotExtendedResponse = data;
  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendImageGeneratorNotification(const sbio::cigi::SImageGeneratorNotification& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendImageGeneratorMessageNotification(data);
}

void CIGResponseEventDispatcher::SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& data)
{
  sbio::cigi::ig::CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs args;
  args.lineOfSightExtendedGeodeticCoordinatesResponse = data;
  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& data)
{
  sbio::cigi::ig::CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs args;
  args.lineOfSightExtendedEntityGeodeticCoordinatesResponse = data;
  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& data)
{
  sbio::cigi::ig::CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs args;
  args.lineOfSightExtendedEntityCoordinatesResponse = data;
  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& data)
{
  sbio::cigi::ig::CigiLineOfSightResponseEventArgs args;
  args.lineOfSightResponse = data;

  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& data)
{
  sbio::cigi::ig::CigiLineOfSightEntityResponseEventArgs args;
  args.lineOfSightEntityResponse = data;

  sbio::utils::Event::Raise<sbio::cigi::ig::TerrainCigiEvent>(args);
}

void CIGResponseEventDispatcher::SendSensorResponse(const SSensorResponse& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendSensorResponse(data);
}

void CIGResponseEventDispatcher::SendSensorExtendedResponse(const SSensorExtendedResponse& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendSensorExtendedResponse(data);
}

void CIGResponseEventDispatcher::SendSensorExtendedEntityResponse(const SSensorExtendedEntityResponse& data)
{
  g_CigiLibGlobals.pImageGenerator->GetPacketSenders()->SendSensorExtendedEntityResponse(data);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
