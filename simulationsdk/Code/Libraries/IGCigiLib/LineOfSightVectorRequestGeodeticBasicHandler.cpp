//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightVectorRequestGeodeticHandler.cpp
 * @brief Implements geodetic line-of-sight vector request handling for `IGCigiLib`.
 *
 * Defines `CLineOfSightVectorRequestGeodeticHandler`, which resolves geodetic request start points,
 * computes source rotation from geodetic coordinates, and exposes the request through the vector-request handling interfaces.
 *
 * @see CLineOfSightVectorRequestGeodeticHandler
 * @see CLineOfSightVectorRequestHandler
 * @see sbio::cigi::SLineOfSightVectorRequestGeodetic
 */
#include "LineOfSightVectorRequestGeodeticBasicHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "UtilitiesLib/Logger.h"

using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CLineOfSightVectorRequestGeodeticBasicHandler::CLineOfSightVectorRequestGeodeticBasicHandler(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& lineOfSightRequest) : CLineOfSightRequestHandler(), m_Request(lineOfSightRequest)
{
}

bool CLineOfSightVectorRequestGeodeticBasicHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightVectorRequestGeodeticHandler::Handle: IGCigiLib event messenger is not initialized.");
    }
    return false;
  }

  SLineOfSightVectorRequestBasicMessage data;
  data.LosID = m_Request.requestID;
  data.AlphaThreshold = m_Request.nAlphaThreshold / 255.f;
  data.MaterialMask = m_Request.nMaterialMask;

  if (!ResolveStartPoint(data.Start))
  {
    return false;
  }

  const auto rotation = GetRotation();

  Quaternion4d vectorDir;
  vectorDir.setIdentity();
  vectorDir = vectorDir * AxisRotation(DegreesToRadians(Degrees(-m_Request.azimuth.Value())).Value(), Vec3::UnitZ());
  vectorDir = vectorDir * AxisRotation(DegreesToRadians(Degrees(m_Request.elevation.Value())).Value(), Vec3::UnitX());

  const auto direction = GeocentricCoordinates(rotation * (vectorDir * Vec3::UnitY()));
  const auto origin = data.Start;
  data.Start = origin + direction * m_Request.fMinimumRange;
  data.End = origin + direction * m_Request.fMaximumRange;

  g_CigiLibGlobals.pEventMessenger->SendLineOfSightVectorRequestBasicMessage(data);

  return true;
}

const sbio::cigi::SLineOfSightRequest& CLineOfSightVectorRequestGeodeticBasicHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightVectorRequestGeodeticBasicHandler::GetRequestRef()
{
  return m_Request;
}

Quaternion4d CLineOfSightVectorRequestGeodeticBasicHandler::GetRotation() const
{
  // If an active database projection is present, use the projected world basis rotation to align the source point with the projected coordinate system.
  if (HasActiveDatabaseProjection())
  {
    return GetProjectedWorldBasisRotation();
  }

  // If no active projection is present, compute the rotation based on the geodetic coordinates of the source point.
  return GetGeocentricRotation(m_Request.sourceGeodeticCoordinates.latitude, m_Request.sourceGeodeticCoordinates.longitude);
}

bool CLineOfSightVectorRequestGeodeticBasicHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  point = ConvertCigiGeodeticToWorldCoordinates(m_Request.sourceGeodeticCoordinates);
  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
