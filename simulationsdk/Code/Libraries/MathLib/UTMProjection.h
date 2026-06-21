//Copyright SimBlocks LLC 2016-2026
/**
 * @file UTMProjection.h
 * @brief UTM projection for converting geodetic coordinates to projected coordinates.
 *
 * Declares the CUTMProjection class, which implements the Universal Transverse Mercator (UTM) projection.
 * Suitable for large areas where Earth's curvature must be considered, this class projects geodetic coordinates
 * (latitude, longitude, altitude) onto a UTM grid defined by a zone. Inherits from CProjection and provides
 * methods for projection and initialization with a reference geodetic point.
 *
 * @see sbio::math::CProjection
 * @see sbio::math::SGeodeticCoordinates
 * @see sbio::math::ReferencePlaneCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_UTM_PROJECTION_H
#define SIMBLOCKS_UTM_PROJECTION_H

#include "Projection.h"

namespace sbio
{
  namespace math
  {
    /**
     * @brief UTM projection class for converting geodetic coordinates to projected coordinates.
     *
     * This class implements the Universal Transverse Mercator (UTM) projection,
     * which is suitable for large areas where the curvature of the Earth needs to be considered.
     * It projects geodetic coordinates (latitude, longitude) onto a UTM grid defined by a zone.
     */
    class CUTMProjection : public CProjection
    {
    public:
      /**
       * @brief Projects geodetic coordinates onto the UTM reference plane.
       *
       * Converts the given geodetic coordinates (latitude, longitude, altitude) to
       * reference plane coordinates using the UTM projection method.
       *
       * @param geodeticCoordinates The geodetic coordinates to project.
       * @return The projected coordinates on the UTM reference plane.
       */
      virtual ReferencePlaneCoordinates GetProjectedCoordinate(const SGeodeticCoordinates& geodeticCoordinates) const override;

      /**
       * @brief Gets geodetic coordinate.
       * @param referencePlaneCoordinates Coordinate value.
       * @return Geodetic coordinate value.
       */
      virtual SGeodeticCoordinates GetGeodeticCoordinate(const ReferencePlaneCoordinates& referencePlaneCoordinates) const override;

      /**
       * @brief Initializes the UTM projection with a reference geodetic point.
       *
       * Sets the UTM zone and hemisphere based on the provided geodetic coordinates.
       * All projected coordinates will be relative to this UTM zone.
       *
       * @param geodeticCoords The reference geodetic coordinates (latitude, longitude, altitude).
       */
      virtual void Init(const SGeodeticCoordinates& geodeticCoords) override;

    private:
      /**
       * @brief UTM zone number.
       */
      int m_zone = 0;

      /**
       * @brief Hemisphere flag (true if northern hemisphere, false if southern).
       */
      bool m_northp = false;

      /**
       * @brief Instance-owned projected origin in UTM coordinates.
       */
      sbio::math::SUTMCoordinates m_OriginPoint;
      sbio::math::HeightRelativeToWGS84Ellipsoid m_ReferenceAltitude = UnknownHeightRelativeToWGS84Ellipsoid;///< Reference altitude for the projection
      bool m_bInitialized = false;///< True once the projection origin has been initialized
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
