//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiProjectionConversions.h
 * @brief Converts CIGI positions, orientations, and offsets using the active database projection when present.
 *
 * World values use GeocentricCoordinates as storage even when they contain projected coordinates.
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_PROJECTION_CONVERSIONS_H
#define SIMBLOCKS_CIGI_PROJECTION_CONVERSIONS_H

#include "CigiLib/CigiConversions.h"
#include "IGCigiLib/IGCigiLib.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Projection.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /** @brief Rotation with explicit SDK body-coordinate input and world-coordinate output types. */
      using TBodyToWorldRotation = sbio::math::TRotation<sbio::math::BodyCoordinates, sbio::math::GeocentricCoordinates>;

      /**
       * @brief Tests whether the library has a database projection.
       * @return True if the global projection pointer is non-null.
       */
      bool HasActiveDatabaseProjection();

      /**
       * @brief Builds the fixed right-forward-up to projected north-east-down basis rotation.
       * @return Rotation exchanging X and Y and reversing Z.
       */
      sbio::math::TGeocentricRotation GetProjectedWorldBasisRotation();

      /**
       * @brief Converts a geodetic position into the current world frame.
       * @param geodeticCoordinates Position to convert.
       * @return Database-projected coordinates when a projection exists; otherwise geocentric coordinates.
       */
      sbio::math::GeocentricCoordinates ConvertCigiGeodeticToWorldCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoordinates);

      /**
       * @brief Converts a current-world position into geodetic coordinates.
       * @param worldCoordinates Projected coordinates if a database projection is active; otherwise geocentric coordinates.
       * @return Geodetic position obtained using the corresponding inverse conversion.
       */
      sbio::math::SGeodeticCoordinates ConvertCigiWorldToGeodeticCoordinates(const sbio::math::GeocentricCoordinates& worldCoordinates);

      /**
       * @brief Builds a top-level world orientation from CIGI body Euler angles.
       * @param rotation CIGI body Euler orientation.
       * @param latitude Local-frame latitude, used only without a database projection.
       * @param longitude Local-frame longitude, used only without a database projection.
       * @return Orientation in the projected world basis or the local geocentric basis.
       */
      sbio::math::TGeocentricRotation SetupCigiTopLevelWorldRotation(const sbio::cigi::TCigiBodyEulerRotation& rotation, sbio::math::Latitude latitude,
                                                                     sbio::math::Longitude longitude);

      /**
       * @brief Builds a position and orientation in the current world frame.
       * @param geodeticCoordinates Position and local-frame origin.
       * @param rotation CIGI body Euler orientation.
       * @return Transform using the active database projection, or geocentric conversions when none exists.
       */
      sbio::math::TGeocentricTransform SetupCigiTopLevelWorldTransform(const sbio::math::SGeodeticCoordinates& geodeticCoordinates,
                                                                       const sbio::cigi::TCigiBodyEulerRotation& rotation);

      /**
       * @brief Extracts SDK body Euler angles from a current-world transform.
       * @param worldTransform World orientation and position; position establishes the local frame without a projection.
       * @return Body Euler angles after removing the projected or geocentric world basis.
       */
      sbio::math::TBodyEulerRotation ConvertCigiWorldRotationToBodyEulerRotation(const sbio::math::TGeocentricTransform& worldTransform);

      /**
       * @brief Gives a world rotation an explicit SDK body-to-world coordinate mapping.
       * @param worldRotation Rotation to wrap without changing its orientation.
       * @return Rotation accepting SDK BodyCoordinates offsets.
       */
      TBodyToWorldRotation MakeBodyToWorldRotation(const sbio::math::TGeocentricRotation& worldRotation);

      /**
       * @brief Rotates an SDK body offset into the world frame without translation.
       * @param bodyToWorldRotation Explicit body-to-world rotation.
       * @param offset Offset in SDK body coordinates, not CIGI body or NED coordinates.
       * @return Rotated world-space offset.
       */
      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::math::BodyCoordinates& offset);

      /** @brief Disallows rotating body offsets without an explicit body-to-world rotation type. */
      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const sbio::math::TGeocentricRotation& worldRotation, const sbio::math::BodyCoordinates& offset) = delete;

      /** @brief Disallows implicit conversion of both the world rotation type and CIGI body coordinates. */
      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const sbio::math::TGeocentricRotation& worldRotation, const sbio::cigi::CigiBodyCoordinates& offset) = delete;

      /** @brief Disallows using a generic world rotation to treat CIGI NED offsets as body offsets. */
      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const sbio::math::TGeocentricRotation& worldRotation, const sbio::cigi::CigiNEDCoordinates& offset) = delete;

      /** @brief Requires explicit conversion from CIGI body coordinates to SDK body coordinates. */
      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::cigi::CigiBodyCoordinates& offset) = delete;

      /** @brief Rejects NED offsets where SDK body coordinates are required. */
      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::cigi::CigiNEDCoordinates& offset) = delete;

      /**
       * @brief Converts a CIGI north-east-down rate offset into the current world frame.
       * @param geodeticCoordinates Local-frame origin, used only without a database projection.
       * @param offset North-east-down offset to convert.
       * @return Unchanged components in projected mode; otherwise the offset rotated into the geocentric frame.
       */
      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::CigiNEDCoordinates& offset);

      /** @brief Rejects SDK body offsets where CIGI north-east-down coordinates are required. */
      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::math::BodyCoordinates& offset) = delete;

      /** @brief Rejects CIGI body offsets where CIGI north-east-down coordinates are required. */
      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates,
                                                                   const sbio::cigi::CigiBodyCoordinates& offset) = delete;
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
