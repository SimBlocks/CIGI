//Copyright SimBlocks LLC 2016-2026
#include "HATHOTGlobalRequestHandler.h"

using namespace sbio;
using namespace sbio::cigi;

CHATHOTGlobalRequestHandler::CHATHOTGlobalRequestHandler(const sbio::cigi::SHATHOTGlobalRequest& request) : CHATHOTRequestHandler(), m_Request(request)
{
}

sbio::ETopLevelCoordinateSystem CHATHOTGlobalRequestHandler::GetCoordinateSystem() const
{
  return ETopLevelCoordinateSystem::GEODETIC;
}

sbio::EntityID CHATHOTGlobalRequestHandler::GetEntityID() const
{
  return UnknownEntityID;
}

sbio::math::SGeodeticCoordinates CHATHOTGlobalRequestHandler::GetGeodeticCoordinates() const
{
  return m_Request.geodeticCoordinates;
}

sbio::math::Vec3 CHATHOTGlobalRequestHandler::GetOffset() const
{
  return {};
}

bool CHATHOTGlobalRequestHandler::Handle()
{
  return SendRequest(m_Request.geodeticCoordinates);
}

const sbio::cigi::SBaseHATHOTRequest& CHATHOTGlobalRequestHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SBaseHATHOTRequest& CHATHOTGlobalRequestHandler::GetRequestRef()
{
  return m_Request;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
