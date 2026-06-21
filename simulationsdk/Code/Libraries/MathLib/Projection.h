//Copyright SimBlocks LLC 2016-2026
/**
 * @file Projection.h
 * @brief Base class for coordinate projections from geodetic to reference plane systems.
 *
 * Declares the CProjection abstract class, which defines the interface for projecting geodetic coordinates
 * (latitude, longitude, altitude) into a reference plane coordinate system. Derived classes should implement
 * the projection logic specific to their projection method (e.g., flat earth, UTM, etc.).
 *
 * @see sbio::math::SGeodeticCoordinates
 * @see sbio::math::ReferencePlaneCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_PROJECTION_H
#define SIMBLOCKS_PROJECTION_H

#include "MathTypes.h"

namespace sbio
{
  namespace math
  {
    /**
     * @brief Base class for coordinate projections.
     *
     * This class defines the interface for projecting geodetic coordinates
     * into a reference plane coordinate system. Derived classes should implement
     * the projection logic specific to their projection method.
     */
    class CProjection
    {
    public:
      /**
       * @brief Destroys CProjection instances.
       */
      virtual ~CProjection() = default;

      /**
       * @brief Projects geodetic coordinates onto the reference plane.
       * 
       * Converts the given geodetic coordinates (latitude, longitude, altitude) to
       * reference plane coordinates using the projection method defined in the derived class.
       *
       * @param geodeticCoordinates The geodetic coordinates to project.
       * @return The projected coordinates on the reference plane.
       */
      virtual ReferencePlaneCoordinates GetProjectedCoordinate(const SGeodeticCoordinates& geodeticCoordinates) const = 0;

      /**
       * @brief Gets geodetic coordinate.
       * @param referencePlaneCoordinates Coordinate value.
       * @return Geodetic coordinate value.
       */
      virtual SGeodeticCoordinates GetGeodeticCoordinate(const ReferencePlaneCoordinates& referencePlaneCoordinates) const = 0;

      /**
       * @brief Initializes the projection with a reference geodetic point.
       *
       * Sets up the projection parameters using the provided geodetic coordinates.
       * This is typically used to define the origin or reference point for the projection.
       *
       * @param geodeticCoords The reference geodetic coordinates (latitude, longitude, altitude).
       */
      virtual void Init(const SGeodeticCoordinates& geodeticCoords) = 0;
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
