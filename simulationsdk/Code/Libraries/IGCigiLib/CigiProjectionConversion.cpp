//Copyright SimBlocks LLC 2016-2026
#include "IGCigiLib/CigiProjectionConversions.h"

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {

      bool HasActiveDatabaseProjection()
      {
        return g_CigiLibGlobals.pProjection != nullptr;
      }

      sbio::math::TGeocentricRotation GetProjectedWorldBasisRotation()
      {
        // converts from the CIGI local convention into the projected world convention used when `g_CigiLibGlobals.pProjection` is active
        // swaps the horizontal `X` and `Y` axes. flips the vertical `Z` axis.
        sbio::math::Mat3 basis;
        basis << 0, 1, 0, 1, 0, 0, 0, 0, -1;
        return sbio::math::TGeocentricRotation(sbio::math::Quaternion4d(basis));
      }

      sbio::math::GeocentricCoordinates ConvertCigiGeodeticToWorldCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoordinates)
      {
        if (HasActiveDatabaseProjection())
        {
          return sbio::math::GeocentricCoordinates(g_CigiLibGlobals.pProjection->GetProjectedCoordinate(geodeticCoordinates).toVec3());
        }

        return sbio::math::ConvertGeodeticToGeocentricCoordinates(geodeticCoordinates);
      }

      sbio::math::SGeodeticCoordinates ConvertCigiWorldToGeodeticCoordinates(const sbio::math::GeocentricCoordinates& worldCoordinates)
      {
        if (HasActiveDatabaseProjection())
        {
          return g_CigiLibGlobals.pProjection->GetGeodeticCoordinate(sbio::math::ReferencePlaneCoordinates(worldCoordinates.toVec3()));
        }

        return sbio::math::ConvertGeocentricToGeodeticCoordinates(worldCoordinates);
      }

      sbio::math::TGeocentricRotation SetupCigiTopLevelWorldRotation(const sbio::cigi::TCigiBodyEulerRotation& rotation, sbio::math::Latitude latitude, sbio::math::Longitude longitude)
      {
        if (HasActiveDatabaseProjection())
        {
          return sbio::math::TGeocentricRotation(GetProjectedWorldBasisRotation() * ConvertCigiBodyRotationToBodyRotation(SetupCigiObjectRotation(rotation)));
        }

        return SetupTopLevelEntityRotation(rotation, latitude, longitude);
      }

      sbio::math::TGeocentricTransform SetupCigiTopLevelWorldTransform(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::TCigiBodyEulerRotation& rotation)
      {
        sbio::math::TGeocentricTransform transform;
        transform.pos = ConvertCigiGeodeticToWorldCoordinates(geodeticCoordinates);
        transform.rotation = SetupCigiTopLevelWorldRotation(rotation, geodeticCoordinates.latitude, geodeticCoordinates.longitude);
        return transform;
      }

      sbio::math::TBodyEulerRotation ConvertCigiWorldRotationToBodyEulerRotation(const sbio::math::TGeocentricTransform& worldTransform)
      {
        // The body rotation is the rotation that, when applied in the world frame, produces the same final orientation as the geocentric rotation.
        // When a database projection is active, the world frame is transformed by the projected world basis rotation
        if (HasActiveDatabaseProjection())
        {
          sbio::math::TBodyRotation bodyRotation(GetProjectedWorldBasisRotation().inverse() * worldTransform.rotation.Rotation());
          return sbio::math::ConvertBodyRotationToBodyEulerRotation(bodyRotation);
        }

        // When no database projection is active, the world frame is the geocentric frame,
        // so the body rotation is directly derived from the geocentric rotation
        return sbio::math::ConvertGeocentricRotationToBodyEulerRotation(worldTransform);
      }

      TBodyToWorldRotation MakeBodyToWorldRotation(const sbio::math::TGeocentricRotation& worldRotation)
      {
        return TBodyToWorldRotation(worldRotation);
      }

      sbio::math::GeocentricCoordinates RotateBodyOffsetToWorld(const TBodyToWorldRotation& bodyToWorldRotation, const sbio::math::BodyCoordinates& offset)
      {
        return bodyToWorldRotation.Rotate(ConvertBodyCoordinatesToCigiBodyCoordinates(offset));
      }

      sbio::math::GeocentricCoordinates ConvertCigiWorldRateOffset(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::CigiNEDCoordinates& offset)
      {
        if (HasActiveDatabaseProjection())
        {
          return sbio::math::GeocentricCoordinates(GetProjectedWorldBasisRotation() * offset.toVec3());
        }

        return sbio::math::GeocentricCoordinates(sbio::math::GetGeocentricRotation(geodeticCoordinates.latitude, geodeticCoordinates.longitude) * offset.toVec3());
      }
    }
  }
}

//Usage of this software is prohibited except through a Software Licensing Agreement with SimBlocks LLC.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026