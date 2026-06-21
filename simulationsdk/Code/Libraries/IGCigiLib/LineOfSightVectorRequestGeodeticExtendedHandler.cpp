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
#include "LineOfSightVectorRequestGeodeticExtendedHandler.h"
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

CLineOfSightVectorRequestGeodeticExtendedHandler::CLineOfSightVectorRequestGeodeticExtendedHandler(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& lineOfSightRequest) : CLineOfSightRequestHandler(), m_Request(lineOfSightRequest)
{
}

bool CLineOfSightVectorRequestGeodeticExtendedHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightVectorRequestGeodeticHandler::Handle: IGCigiLib event messenger is not initialized.");
    }
    return false;
  }

  SLineOfSightVectorRequestExtendedMessage data;
  data.LosID = m_Request.requestID;
  data.AlphaThreshold = m_Request.nAlphaThreshold / 255.f;
  data.MaterialMask = m_Request.nMaterialMask;
  data.eResponseCoordinateSystem = m_Request.eResponseCoordinateSystem;

  if (!ResolveStartPoint(data.Start))
  {
    return false;
  }

  const auto rotation = GetRotation();

  Quaternion4d vectorDir;
  vectorDir.setIdentity();
  vectorDir = vectorDir * AxisRotation(DegreesToRadians(-m_Request.azimuth.Value()), Vec3::UnitZ());
  vectorDir = vectorDir * AxisRotation(DegreesToRadians(m_Request.elevation.Value()), Vec3::UnitX());

  const auto direction = GeocentricCoordinates(rotation * (vectorDir * Vec3::UnitY()));
  const auto origin = data.Start;
  data.Start = origin + direction * m_Request.fMinimumRange;
  data.End = origin + direction * m_Request.fMaximumRange;

  g_CigiLibGlobals.pEventMessenger->SendLineOfSightVectorRequestExtendedMessage(data);

  return true;
}

const sbio::cigi::SLineOfSightRequest& CLineOfSightVectorRequestGeodeticExtendedHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightVectorRequestGeodeticExtendedHandler::GetRequestRef()
{
  return m_Request;
}

Quaternion4d CLineOfSightVectorRequestGeodeticExtendedHandler::GetRotation() const
{
  // The rotation is determined based on the presence of an active database projection.
  // If a projection is active, the projected world basis rotation is used.
  if (HasActiveDatabaseProjection())
  {
    return GetProjectedWorldBasisRotation();
  }

  // Otherwise, the geocentric rotation is calculated from the source geodetic coordinates (latitude and longitude).
  return GetGeocentricRotation(m_Request.sourceGeodeticCoordinates.latitude, m_Request.sourceGeodeticCoordinates.longitude);
}

bool CLineOfSightVectorRequestGeodeticExtendedHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  point = ConvertCigiGeodeticToWorldCoordinates(m_Request.sourceGeodeticCoordinates);
  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
