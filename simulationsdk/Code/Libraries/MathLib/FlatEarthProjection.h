//Copyright SimBlocks LLC 2016-2026
/**
 * @file FlatEarthProjection.h
 * @brief Flat Earth projection for converting geodetic coordinates to projected coordinates.
 *
 * Declares the CFlatEarthProjection class, which implements a simple flat Earth projection suitable for small areas
 * where the curvature of the Earth can be ignored. Projects geodetic coordinates (latitude, longitude, altitude)
 * onto a flat plane defined by a reference point. Inherits from CProjection and provides methods for projection
 * and initialization with a reference geodetic point.
 *
 * @see sbio::math::CProjection
 * @see sbio::math::SGeodeticCoordinates
 * @see sbio::math::ReferencePlaneCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_FLAT_EARTH_PROJECTION_H
#define SIMBLOCKS_FLAT_EARTH_PROJECTION_H

#include "Projection.h"

namespace sbio
{
  namespace math
  {
    /**
     * @brief Flat Earth projection class for converting geodetic coordinates to projected coordinates.
     *
     * This class implements a simple flat Earth projection, which is suitable for small areas
     * where the curvature of the Earth can be ignored. It projects geodetic coordinates (latitude,
     * longitude) onto a flat plane defined by a reference point.
     */
    class CFlatEarthProjection : public CProjection
    {
    public:
      /**
       * @brief Projects geodetic coordinates onto the flat reference plane.
       *
       * Converts the given geodetic coordinates (latitude, longitude, altitude) to
       * reference plane coordinates using the flat Earth approximation.
       *
       * @param geodeticCoordinates The geodetic coordinates to project.
       * @return The projected coordinates on the reference plane.
       */
      virtual ReferencePlaneCoordinates GetProjectedCoordinate(const SGeodeticCoordinates& geodeticCoordinates) const override;

      /**
       * @brief Gets geodetic coordinate.
       * @param referencePlaneCoordinates Coordinate value.
       * @return Geodetic coordinate value.
       */
      virtual SGeodeticCoordinates GetGeodeticCoordinate(const ReferencePlaneCoordinates& referencePlaneCoordinates) const override;

      /**
       * @brief Initializes the projection with a reference geodetic point.
       * 
       * Sets the reference latitude and longitude for the flat Earth projection.
       * All projected coordinates will be relative to this reference point.
       *
       * @param geodeticCoords The reference geodetic coordinates (latitude, longitude, altitude).
       */
      virtual void Init(const SGeodeticCoordinates& geodeticCoords) override;

    private:
      sbio::math::Latitude m_ReferenceLatitude = sbio::math::UnknownLatitude;///< Reference latitude for the projection
      sbio::math::Longitude m_ReferenceLongitude = sbio::math::UnknownLongitude;///< Reference longitude for the projection
      sbio::math::HeightRelativeToWGS84Ellipsoid m_ReferenceAltitude = sbio::math::UnknownHeightRelativeToWGS84Ellipsoid;///< Reference altitude for the projection
      bool m_bInitialized = false;///< True once the projection reference point has been initialized
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
