//Copyright SimBlocks LLC 2016-2026
/**
 * @file UTMProjection.cpp
 * @brief Implements the UTM projection for geodetic to projected coordinate conversion.
 *
 * This file provides the implementation of the CUTMProjection class, which uses the Universal Transverse Mercator (UTM)
 * projection to convert geodetic coordinates (latitude, longitude, altitude) to a UTM reference plane. Suitable for large
 * areas where Earth's curvature must be considered. Inherits from CProjection and supports initialization with a reference
 * geodetic point and zone.
 *
 * @see sbio::math::CUTMProjection
 * @see sbio::math::SGeodeticCoordinates
 * @see sbio::math::ReferencePlaneCoordinates
 */
#include "UTMProjection.h"
#include <GeographicLib/UTMUPS.hpp>

using namespace sbio::math;
using namespace GeographicLib;

/**
 * @brief Projects geodetic coordinates onto the UTM reference plane using UTMUPS.
 * @param geodeticCoordinates The geodetic coordinates to project.
 * @return The projected coordinates on the UTM reference plane.
 */
ReferencePlaneCoordinates CUTMProjection::GetProjectedCoordinate(const SGeodeticCoordinates& geodeticCoordinates) const
{
  if (!m_bInitialized)
  {
    return ReferencePlaneCoordinates();
  }

  double x = 0;
  double y = 0;
  int zone = m_zone;
  bool northp = m_northp;

  UTMUPS::Forward(geodeticCoordinates.latitude.Value(), geodeticCoordinates.longitude.Value(), zone, northp, x, y, m_zone);

  if (northp != m_northp)
  {
    int transferredZone = m_zone;
    UTMUPS::Transfer(m_zone, northp, x, y, m_zone, m_northp, x, y, transferredZone);
  }

  x -= m_OriginPoint.easting;
  y -= m_OriginPoint.northing;

  sbio::math::ReferencePlaneCoordinates entityPos;
  entityPos[0] = y;
  entityPos[1] = x;
  entityPos[2] = -(geodeticCoordinates.altitude - m_ReferenceAltitude).Value();
  return entityPos;
}

SGeodeticCoordinates CUTMProjection::GetGeodeticCoordinate(const ReferencePlaneCoordinates& referencePlaneCoordinates) const
{
  // If the projection is not initialized, return default geodetic coordinates (0, 0, 0).
  if (!m_bInitialized)
  {
    return SGeodeticCoordinates();
  }

  // Use the UTMUPS projection to convert from reference plane coordinates back to geodetic coordinates.
  // The input referencePlaneCoordinates are relative to the origin, so add the origin's easting and northing back
  // to get the absolute UTM coordinates before converting to geodetic.
  double latitude = 0;
  double longitude = 0;
  const double x = referencePlaneCoordinates[1] + m_OriginPoint.easting;
  const double y = referencePlaneCoordinates[0] + m_OriginPoint.northing;
  UTMUPS::Reverse(m_zone, m_northp, x, y, latitude, longitude);

  // Adjust the altitude by adding the reference altitude back to the negative of the input Z coordinate,
  // because the input Z is defined as negative altitude relative to the reference plane.
  SGeodeticCoordinates geodeticCoordinates;
  geodeticCoordinates.latitude = Latitude(latitude);
  geodeticCoordinates.longitude = Longitude(longitude);
  geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(m_ReferenceAltitude.Value() - referencePlaneCoordinates[2]);
  return geodeticCoordinates;
}

/**
 * @brief Initializes the UTM projection with a reference geodetic point and sets the UTM zone and hemisphere.
 * @param geodeticCoords The reference geodetic coordinates (latitude, longitude, altitude).
 */
void CUTMProjection::Init(const SGeodeticCoordinates& geodeticCoords)
{
  UTMUPS::Forward(geodeticCoords.latitude.Value(), geodeticCoords.longitude.Value(), m_zone, m_northp, m_OriginPoint.easting, m_OriginPoint.northing);
  m_ReferenceAltitude = geodeticCoords.altitude;
  m_bInitialized = true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
