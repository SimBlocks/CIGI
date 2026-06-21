//Copyright SimBlocks LLC 2016-2026
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
      using TBodyToWorldRotation = sbio::math::TRotation<sbio::cigi::CigiBodyCoordinates, sbio::math::GeocentricCoordinates>;

      bool HasActiveDatabaseProjection();

      sbio::math::TGeocentricRotation GetProjectedWorldBasisRotation();

      sbio::math::GeocentricCoordinates ConvertCigiGeodeticToWorldCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoordinates);

      sbio::math::SGeodeticCoordinates ConvertCigiWorldToGeodeticCoordinates(const sbio::math::GeocentricCoordinates& worldCoordinates);

      sbio::math::TGeocentricRotation SetupCigiTopLevelWorldRotation(const sbio::cigi::TCigiBodyEulerRotation& rotation, sbio::math::Latitude latitude, sbio::math::Longitude longitude);

      sbio::math::TGeocentricTransform SetupCigiTopLevelWorldTransform(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::TCigiBodyEulerRotation& rotation);

      sbio::math::TBodyEulerRotation ConvertCigiWorldRotationToBodyEulerRotation(const sbio::math::TGeocentricTransform& worldTransform);

      TBodyToWorldRotation MakeBodyToWorldRotation(const sbio::math::TGeocentricRotation& worldRotation);

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::math::BodyCoordinates& offset);

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const sbio::math::TGeocentricRotation& worldRotation, const sbio::math::BodyCoordinates& offset) = delete;

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const sbio::math::TGeocentricRotation& worldRotation, const sbio::cigi::CigiBodyCoordinates& offset) = delete;

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const sbio::math::TGeocentricRotation& worldRotation, const sbio::cigi::CigiNEDCoordinates& offset) = delete;

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::cigi::CigiBodyCoordinates& offset) = delete;

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::cigi::CigiNEDCoordinates& offset) = delete;

      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::CigiNEDCoordinates& offset);

      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::math::BodyCoordinates& offset) = delete;

      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::CigiBodyCoordinates& offset) = delete;
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
