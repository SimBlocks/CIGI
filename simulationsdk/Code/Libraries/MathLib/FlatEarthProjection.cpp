//Copyright SimBlocks LLC 2016-2026
/**
 * @file FlatEarthProjection.cpp
 * @brief Implements a flat Earth projection for geodetic to projected coordinate conversion.
 *
 * This file provides the implementation of the CFlatEarthProjection class, which uses an azimuthal equidistant
 * projection to convert geodetic coordinates (latitude, longitude, altitude) to a flat reference plane. Suitable
 * for small areas where Earth's curvature can be ignored. Inherits from CProjection and supports initialization
 * with a reference geodetic point.
 *
 * @see sbio::math::CFlatEarthProjection
 * @see sbio::math::SGeodeticCoordinates
 * @see sbio::math::ReferencePlaneCoordinates
 */
#include "FlatEarthProjection.h"
#include <GeographicLib/AzimuthalEquidistant.hpp>
#include <GeographicLib/Geodesic.hpp>

using namespace sbio::math;
using namespace GeographicLib;

namespace
{
  const Geodesic& Wgs84Geodesic()
  {
    static const Geodesic geodesic(Constants::WGS84_a(), Constants::WGS84_f());
    return geodesic;
  }

  const AzimuthalEquidistant& AzimuthalProjection()
  {
    static const AzimuthalEquidistant projection(Wgs84Geodesic());
    return projection;
  }
}

/**
 * @brief Projects geodetic coordinates onto the flat reference plane using azimuthal equidistant projection.
 * @param geodeticCoordinates The geodetic coordinates to project.
 * @return The projected coordinates on the reference plane.
 */
ReferencePlaneCoordinates CFlatEarthProjection::GetProjectedCoordinate(const SGeodeticCoordinates& geodeticCoordinates) const
{
  if (!m_bInitialized)
  {
    return ReferencePlaneCoordinates();
  }

  double x = 0;
  double y = 0;
  AzimuthalProjection().Forward(m_ReferenceLatitude.Value(), m_ReferenceLongitude.Value(), geodeticCoordinates.latitude.Value(), geodeticCoordinates.longitude.Value(), x, y);

  sbio::math::ReferencePlaneCoordinates entityPos;
  entityPos[0] = y;
  entityPos[1] = x;
  entityPos[2] = -(geodeticCoordinates.altitude - m_ReferenceAltitude).Value();
  return entityPos;
}

SGeodeticCoordinates CFlatEarthProjection::GetGeodeticCoordinate(const ReferencePlaneCoordinates& referencePlaneCoordinates) const
{
  // If the projection is not initialized, return default geodetic coordinates (0, 0, 0).
  if (!m_bInitialized)
  {
    return SGeodeticCoordinates();
  }

  // Use the azimuthal projection to convert from reference plane coordinates back to geodetic coordinates.
  double latitude = 0;
  double longitude = 0;
  AzimuthalProjection().Reverse(m_ReferenceLatitude.Value(), m_ReferenceLongitude.Value(), referencePlaneCoordinates[1], referencePlaneCoordinates[0], latitude, longitude);

  // Calculate the altitude by adjusting the reference altitude with the Z component of the reference plane coordinates.
  SGeodeticCoordinates geodeticCoordinates;
  geodeticCoordinates.latitude = Latitude(latitude);
  geodeticCoordinates.longitude = Longitude(longitude);
  geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(m_ReferenceAltitude.Value() - referencePlaneCoordinates[2]);
  return geodeticCoordinates;
}

/**
 * @brief Initializes the projection with a reference geodetic point.
 * @param geodeticCoords The reference geodetic coordinates (latitude, longitude, altitude).
 */
void CFlatEarthProjection::Init(const SGeodeticCoordinates& geodeticCoords)
{
  m_ReferenceLatitude = geodeticCoords.latitude;
  m_ReferenceLongitude = geodeticCoords.longitude;
  m_ReferenceAltitude = geodeticCoords.altitude;
  m_bInitialized = true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
