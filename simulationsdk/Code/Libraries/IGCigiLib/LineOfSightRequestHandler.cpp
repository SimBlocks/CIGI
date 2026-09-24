//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightRequestHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "MathLib/CoordinateConversions.h"
#include <atomic>

using namespace sbio;
using namespace sbio::math;

CLineOfSightRequestHandler::CLineOfSightRequestHandler(double rangeOffset, sbio::EntityID sourceEntityID, sbio::EntityID destinationEntityID) :
  m_RangeOffset(rangeOffset), m_SourceEntityID(sourceEntityID), m_DestinationEntityID(destinationEntityID)
{
  static std::atomic<uint64_t> nextGeneration{0};
  m_RequestGeneration = nextGeneration.fetch_add(1, std::memory_order_relaxed) + 1;
}

CLineOfSightRequestHandler::~CLineOfSightRequestHandler()
{
}

sbio::FrameNumber CLineOfSightRequestHandler::GetLastHostFrameNumber() const
{
  return GetRequest().lastHostFrameNumber;
}

sbio::cigi::UpdatePeriod CLineOfSightRequestHandler::GetUpdatePeriod() const
{
  return GetRequest().updatePeriod;
}

void CLineOfSightRequestHandler::HandleGeodeticCoordinateSystemResponse(sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& lineOfSightExtendedResponse,
                                                                        sbio::math::GeocentricCoordinates IntersectionPoint)
{
  lineOfSightExtendedResponse.geodeticCoordinates = sbio::cigi::ig::ConvertCigiWorldToGeodeticCoordinates(IntersectionPoint);
}

void CLineOfSightRequestHandler::SetLastHostFrameNumber(FrameNumber lastHostFrameNumber)
{
  GetRequestRef().lastHostFrameNumber = lastHostFrameNumber;
}

uint64_t CLineOfSightRequestHandler::GetRequestGeneration() const
{
  return m_RequestGeneration;
}

double CLineOfSightRequestHandler::GetRangeOffset() const
{
  return m_RangeOffset;
}

bool CLineOfSightRequestHandler::ReferencesEntity(sbio::EntityID entityID) const
{
  return entityID != sbio::UnknownEntityID && (entityID == m_SourceEntityID || entityID == m_DestinationEntityID);
}

uint8_t CLineOfSightRequestHandler::GetHostFrameLSN() const
{
  if (GetUpdatePeriod().Value() != 0)
  {
    return static_cast<uint8_t>(GetLastHostFrameNumber().Value() & 0x0f);
  }
  return 0;
}

void CLineOfSightRequestHandler::SetLastUpdateFrame(sbio::FrameNumber frame)
{
  m_LastUpdateFrame = frame.Value();
}

bool CLineOfSightRequestHandler::IsUpdateDue(sbio::FrameNumber frame) const
{
  return frame.Value() - m_LastUpdateFrame >= GetUpdatePeriod().Value();
}

bool CLineOfSightRequestHandler::RecordResponse(uint8_t responseCount)
{
  return ++m_ResponsesReceived >= responseCount;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
