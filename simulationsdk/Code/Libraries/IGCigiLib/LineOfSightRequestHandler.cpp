//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightRequestHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "MathLib/CoordinateConversions.h"

using namespace sbio;
using namespace sbio::math;

CLineOfSightRequestHandler::CLineOfSightRequestHandler()
{
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

void CLineOfSightRequestHandler::HandleGeodeticCoordinateSystemResponse(sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& lineOfSightExtendedResponse, sbio::math::GeocentricCoordinates IntersectionPoint)
{
  lineOfSightExtendedResponse.geodeticCoordinates = sbio::cigi::ig::ConvertCigiWorldToGeodeticCoordinates(IntersectionPoint);
}

void CLineOfSightRequestHandler::SetLastHostFrameNumber(FrameNumber lastHostFrameNumber)
{
  GetRequestRef().lastHostFrameNumber = lastHostFrameNumber;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
