//Copyright SimBlocks LLC 2016-2026
#include "HATHOTRequestHandler.h"
#include "IGCigiLib.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "MathLib/CoordinateConversions.h"
#include "UtilitiesLib/Logger.h"
#include <atomic>

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CHATHOTRequestHandler::CHATHOTRequestHandler()
{
  static std::atomic<uint64_t> nextGeneration{0};
  m_RequestGeneration = nextGeneration.fetch_add(1, std::memory_order_relaxed) + 1;
}

CHATHOTRequestHandler::~CHATHOTRequestHandler()
{
}

uint8_t CHATHOTRequestHandler::GetHostFrameLSN() const
{
  if (GetUpdatePeriod().Value() != 0)
  {
    return static_cast<uint8_t>(GetLastHostFrameNumber().Value() & 0x0f);
  }
  return 0;
}

sbio::FrameNumber CHATHOTRequestHandler::GetLastHostFrameNumber() const
{
  return GetRequest().lastHostFrameNumber;
}

sbio::cigi::UpdatePeriod CHATHOTRequestHandler::GetUpdatePeriod() const
{
  return GetRequest().updatePeriod;
}

sbio::cigi::ERequestType CHATHOTRequestHandler::GetRequestType() const
{
  return GetRequest().eRequestType;
}

sbio::HATHOTID CHATHOTRequestHandler::GetRequestID() const
{
  return GetRequest().requestID;
}

uint64_t CHATHOTRequestHandler::GetRequestGeneration() const
{
  return m_RequestGeneration;
}

bool CHATHOTRequestHandler::IsUpdateDue(sbio::FrameNumber frame) const
{
  return frame.Value() - m_LastUpdateFrame >= GetUpdatePeriod().Value();
}

void CHATHOTRequestHandler::SetLastHostFrameNumber(FrameNumber lastHostFrameNumber)
{
  GetRequestRef().lastHostFrameNumber = lastHostFrameNumber;
}

void CHATHOTRequestHandler::SetLastUpdateFrame(sbio::FrameNumber frame)
{
  m_LastUpdateFrame = frame.Value();
}

bool CHATHOTRequestHandler::SendRequest(const sbio::math::SGeodeticCoordinates& resolvedPoint)
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CHATHOTRequestHandler::SendRequest: IGCigiLib event messenger is not initialized.");
    }
    return false;
  }

  if (GetRequest().eRequestType == ERequestType::HAT)
  {
    SHeightAboveTerrainRequestMessage data;
    data.HatHotID = GetRequestID();
    data.RequestGeneration = GetRequestGeneration();
    data.HostFrameLSN = GetHostFrameLSN();
    data.Point = resolvedPoint;
    // data.UpDir = BodyCoordinates(GetGeocentricRotation(resolvedPoint.latitude, resolvedPoint.longitude).toRotationMatrix().toMat3().col(2));
    data.isExtendedRequest = false;
    g_CigiLibGlobals.pEventMessenger->SendHeightAboveTerrainRequestMessage(data);
    return true;
  }
  else if (GetRequest().eRequestType == ERequestType::HOT)
  {

    SHeightOfTerrainRequestMessage data;
    data.HatHotID = GetRequestID();
    data.RequestGeneration = GetRequestGeneration();
    data.HostFrameLSN = GetHostFrameLSN();
    data.Point = resolvedPoint;
    // data.UpDir = BodyCoordinates(GetGeocentricRotation(resolvedPoint.latitude, resolvedPoint.longitude).toRotationMatrix().toMat3().col(2));
    data.isExtendedRequest = GetRequestType() == ERequestType::EXTENDED;

    g_CigiLibGlobals.pEventMessenger->SendHeightOfTerrainRequestMessage(data);
    return true;
  }
  else if (GetRequest().eRequestType == ERequestType::EXTENDED)
  {
    SHeightOfTerrainRequestMessage data;
    data.HatHotID = GetRequestID();
    data.RequestGeneration = GetRequestGeneration();
    data.HostFrameLSN = GetHostFrameLSN();
    data.Point = resolvedPoint;
    // data.UpDir = BodyCoordinates(GetGeocentricRotation(resolvedPoint.latitude, resolvedPoint.longitude).toRotationMatrix().toMat3().col(2));
    data.isExtendedRequest = true;
    g_CigiLibGlobals.pEventMessenger->SendHeightOfTerrainRequestMessage(data);
    return true;
  }
  else
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CHATHOTRequestHandler::SendRequest: Unsupported request type " << static_cast<int>(GetRequest().eRequestType) << " for request ID " << GetRequestID().Value() << "."
         << std::endl;
      g_CigiLibGlobals.pLogger->LogError(ss);
    }
    return false;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
