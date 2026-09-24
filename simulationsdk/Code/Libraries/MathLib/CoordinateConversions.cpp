//Copyright SimBlocks LLC 2016-2026
/**
 * @file CoordinateConversions.cpp
 * @brief Implements coordinate conversion functions between geodetic, geocentric (ECEF), and local reference plane systems.
 *
 * This file provides a set of functions for converting between various coordinate systems commonly used in geospatial applications,
 * including geodetic (latitude, longitude, altitude), geocentric (ECEF), and local reference plane (NED/ENU).
 * It also includes utilities for rotation conversions and spatial reference frame creation.
 *
 * Functions include:
 * - Geodetic <-> Geocentric (ECEF) conversions
 * - Geocentric <-> Reference plane conversions
 * - Geocentric <-> Body rotation and Euler angle conversions
 * - Reference frame and NED (North, East, Down) frame creation
 * - Surface normal and calculations
 * - Matrix and rotation conversions for geospatial math
 *
 * @see sbio::math namespace
 */
#include "CoordinateConversions.h"
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <limits>
#include <vector>
#define _USE_MATH_DEFINES
#include "MathLib/Math.h"
#include <math.h>

using namespace std;
using namespace sbio::math;
using namespace GeographicLib;

namespace sbio
{
  namespace math
  {
    Mat3 BuildNEDRotationMatrix(const std::vector<double>& enuRotationMatrix)
    {
      Mat3 nedRotation;

      for (int r = 0; r < 3; ++r)
      {
        nedRotation(r, 0) = enuRotationMatrix[r * 3 + 1];
        nedRotation(r, 1) = enuRotationMatrix[r * 3 + 0];
        nedRotation(r, 2) = -enuRotationMatrix[r * 3 + 2];
      }

      return nedRotation;
    }

    TGeocentricRotation BuildNEDGeocentricRotation(Latitude latitude, Longitude longitude, double altitude)
    {
      std::vector<double> rotationMatrix;
      rotationMatrix.resize(9);

      double x = 0;
      double y = 0;
      double z = 0;

      GeographicLib::Geocentric::WGS84().Forward(latitude.Value(), longitude.Value(), altitude, x, y, z, rotationMatrix);
      return TGeocentricRotation(BuildNEDRotationMatrix(rotationMatrix));
    }

    TGeocentricRotation GetBodyGeocentricRotation(Latitude latitude, Longitude longitude)
    {
      // The body frame is defined as:
      //   forward = y
      //   right = x
      //   down = -z
      Mat3 bodyToNed;

      // clang-format off
      bodyToNed << 0, 1, 0,
                   1, 0, 0,
                   0, 0, -1;
      // clang-format on

      return TGeocentricRotation(BuildNEDGeocentricRotation(latitude, longitude, 0) * Quaternion4d(bodyToNed));
    }

    SReferencePlaneCoordinateSystem InitReferencePlaneCoordinates(const SGeodeticCoordinates& geodeticCoords)
    {
      vector<double> m;
      m.resize(9);

      SReferencePlaneCoordinateSystem referencePlane;
      GeographicLib::Geocentric::WGS84().Forward(geodeticCoords.latitude.Value(),
                                                 geodeticCoords.longitude.Value(),
                                                 geodeticCoords.altitude.Value(),
                                                 referencePlane.referencePoint[0],
                                                 referencePlane.referencePoint[1],
                                                 referencePlane.referencePoint[2],
                                                 m);

      const Mat3 mat = BuildNEDRotationMatrix(m);
      referencePlane.rotation = Quaternion4d(mat);
      referencePlane.inverseRotation = TReferencePlaneRotation(referencePlane.rotation.inverse());
      return referencePlane;
    }

    sbio::math::Mat4 BuildReferencePlaneTransform(const sbio::math::SReferencePlaneCoordinateSystem& referencePlane)
    {
      sbio::math::Mat4 transform = sbio::math::Mat4::Identity();
      const auto rotationMatrix = referencePlane.rotation.toRotationMatrix();

      for (int r = 0; r < 3; ++r)
      {
        for (int c = 0; c < 3; ++c)
        {
          transform(r, c) = rotationMatrix(r, c);
        }
      }

      transform(0, 3) = referencePlane.referencePoint[0];
      transform(1, 3) = referencePlane.referencePoint[1];
      transform(2, 3) = referencePlane.referencePoint[2];

      return transform;
    }

    GeocentricCoordinates ConvertGeodeticToGeocentricCoordinates(const SGeodeticCoordinates& geodeticCoords)
    {
      GeocentricCoordinates pos;
      GeographicLib::Geocentric::WGS84().Forward(geodeticCoords.latitude.Value(), geodeticCoords.longitude.Value(), geodeticCoords.altitude.Value(), pos[0], pos[1], pos[2]);

      return pos;
    }

    sbio::math::TBodyEulerRotation ConvertGeocentricRotationToBodyEulerRotation(const sbio::math::TGeocentricTransform& geocentricTransform)
    {
      return ConvertBodyRotationToBodyEulerRotation(ConvertGeocentricRotationToBodyRotation(geocentricTransform));
    }

    TBodyRotation ConvertEulerRotationToBodyRotation(const TBodyEulerRotation& rotation)
    {
      Quaternion4d q;
      q.setIdentity();

      // TBodyEulerRotation uses the same yaw/pitch/roll semantics as CIGI body Euler angles,
      // while TBodyRotation uses internal body axes (right, forward, up). Build the quaternion
      // in the Euler extraction frame and then remap it back into internal body coordinates so
      // this function is the inverse of ConvertBodyRotationToBodyEulerRotation.
      q = q * AxisRotation(DegreesToRadians(rotation.yaw).Value(), Vec3::UnitZ());
      q = q * AxisRotation(DegreesToRadians(rotation.pitch).Value(), Vec3::UnitY());
      q = q * AxisRotation(DegreesToRadians(rotation.roll).Value(), Vec3::UnitX());

      return TBodyRotation(q.w(), q.y(), q.x(), -q.z());
    }

    sbio::math::TBodyRotation ConvertGeocentricRotationToBodyRotation(const sbio::math::TGeocentricTransform& geocentricTransform)
    {
      auto geod = ConvertGeocentricToGeodeticCoordinates(geocentricTransform.pos);
      return TBodyRotation(GetBodyGeocentricRotation(geod.latitude, geod.longitude).inverse() * geocentricTransform.rotation.Rotation());
    }

    sbio::math::TBodyEulerRotation ConvertBodyRotationToBodyEulerRotation(const sbio::math::TBodyRotation& bodyRotation)
    {
      // NED     enu
      // forward   x        y
      // right     y        x
      // down     -z       -z
      Quaternion4d q = Quaternion4d(bodyRotation.w(), bodyRotation.y(), bodyRotation.x(), -bodyRotation.z());
      TBodyEulerRotation rotation;

      // Convert quaternion to Euler angles (yaw, pitch, roll) using the Tait-Bryan angles convention (Z-Y-X)
      double sinr_cosp = 2 * (q.w() * q.x() + q.y() * q.z());
      double cosr_cosp = 1 - 2 * (q.x() * q.x() + q.y() * q.y());
      double sinp = 2 * (q.w() * q.y() - q.z() * q.x());
      double cosp = std::hypot(sinr_cosp, cosr_cosp);
      double yaw;

      // At gimbal lock, choose zero roll and recover the coupled yaw from the remaining matrix entries.
      if (cosp <= 16 * std::numeric_limits<double>::epsilon())
      {
        rotation.roll = Degrees180(0);
        rotation.pitch = Degrees90(std::copysign(90.0, sinp));
        yaw = RadiansToDegrees(std::atan2(2 * (q.w() * q.z() - q.x() * q.y()), 1 - 2 * (q.x() * q.x() + q.z() * q.z())));
      }
      else
      {
        rotation.roll = Degrees180(RadiansToDegrees(std::atan2(sinr_cosp, cosr_cosp)));
        rotation.pitch = Degrees90(RadiansToDegrees(std::atan2(sinp, cosp)));
        double siny_cosp = 2 * (q.w() * q.z() + q.x() * q.y());
        double cosy_cosp = 1 - 2 * (q.y() * q.y() + q.z() * q.z());
        yaw = RadiansToDegrees(std::atan2(siny_cosp, cosy_cosp));
      }

      // Ensure yaw is in the range [0, 360)
      if (yaw < 0)
      {
        yaw += 360.0;
      }

      rotation.yaw = Degrees(yaw >= 360.0 ? 0.0 : yaw);
      return rotation;
    }

    SGeodeticCoordinates ConvertGeocentricToGeodeticCoordinates(const GeocentricCoordinates& geocentricPos)
    {
      SGeodeticCoordinates geodeticCoords;

      double fLatitude = 0;
      double fLongitude = 0;
      double altitude = 0;
      GeographicLib::Geocentric::WGS84().Reverse(geocentricPos[0], geocentricPos[1], geocentricPos[2], fLatitude, fLongitude, altitude);// lat, long, and alt passed by reference
      geodeticCoords.altitude = HeightRelativeToWGS84Ellipsoid(altitude);
      geodeticCoords.latitude = Latitude(fLatitude);
      geodeticCoords.longitude = Longitude(fLongitude);

      return geodeticCoords;
    }

    ReferencePlaneCoordinates ConvertGeocentricToReferencePlaneCoordinates(const GeocentricCoordinates& geocentricPos, const SReferencePlaneCoordinateSystem& referencePlane)
    {
      const Vec3 relativePosition = geocentricPos.toVec3() - referencePlane.referencePoint.toVec3();
      return ReferencePlaneCoordinates(referencePlane.inverseRotation.toRotationMatrix() * relativePosition);
    }

    TReferencePlaneTransform ConvertGeocentricToReferencePlaneCoordinates(const TGeocentricTransform& geocentricTransform, const SReferencePlaneCoordinateSystem& referencePlane)
    {
      TReferencePlaneTransform referencePlaneTransform;
      referencePlaneTransform.pos = ConvertGeocentricToReferencePlaneCoordinates(geocentricTransform.pos, referencePlane);
      referencePlaneTransform.rotation = ConvertGeocentricToReferencePlaneRotation(geocentricTransform.rotation, referencePlane);
      referencePlaneTransform.scale = geocentricTransform.scale;

      return referencePlaneTransform;
    }

    TReferencePlaneRotation ConvertGeocentricToReferencePlaneRotation(const TGeocentricRotation& geocentricRotation, const SReferencePlaneCoordinateSystem& referencePlane)
    {
      return referencePlane.inverseRotation * geocentricRotation;
    }

    sbio::math::TGeodeticTransform ConvertGeocentricToGeodeticTransform(const sbio::math::TGeocentricTransform& geocentricTransform)
    {
      TGeodeticTransform geodeticTransform;
      geodeticTransform.pos = ConvertGeocentricToGeodeticCoordinates(geocentricTransform.pos);
      geodeticTransform.rotation = geocentricTransform.rotation;
      geodeticTransform.scale = geocentricTransform.scale;
      return geodeticTransform;
    }

    sbio::math::TGeocentricTransform ConvertGeodeticToGeocentricTransform(const sbio::math::TGeodeticTransform& geodeticTransform)
    {
      TGeocentricTransform geocentricTransform;
      geocentricTransform.pos = ConvertGeodeticToGeocentricCoordinates(geodeticTransform.pos);
      geocentricTransform.rotation = geodeticTransform.rotation;
      geocentricTransform.scale = geodeticTransform.scale;
      return geocentricTransform;
    }

    TGeocentricTransform ConvertGeodeticToGeocentricTransform(const SGeodeticCoordinates& geodeticPosition, const TBodyEulerRotation& rotation)
    {
      TGeocentricTransform worldTransform;
      worldTransform.pos = ConvertGeodeticToGeocentricCoordinates(geodeticPosition);
      worldTransform.rotation = GetBodyGeocentricRotation(geodeticPosition.latitude, geodeticPosition.longitude) * ConvertEulerRotationToBodyRotation(rotation);
      return worldTransform;
    }

    TGeocentricReferencePlane ConvertGeocentricToReferencePlane(const TGeocentricRotation& geocentricRotation)
    {
      TGeocentricReferencePlane referencePlane;
      TGeocentricMatrix m = geocentricRotation.toRotationMatrix();
      referencePlane.north = m.getCol(0);
      referencePlane.east = m.getCol(1);
      referencePlane.down = m.getCol(2);
      return referencePlane;
    }

    TGeocentricReferencePlane ConvertGeocentricToReferencePlane(const TRotation<BodyCoordinates, GeocentricCoordinates>& bodyToGeocentricRotation)
    {
      TGeocentricReferencePlane referencePlane;
      const TGeocentricMatrix m = bodyToGeocentricRotation.toRotationMatrix();

      // The body frame is defined as:
      //   forward = y
      //   right = x
      //   down = -z
      referencePlane.north = m.getCol(1);
      referencePlane.east = m.getCol(0);
      referencePlane.down = m.getCol(2).negated();
      return referencePlane;
    }

    sbio::math::SGeodeticCoordinates ConvertReferencePlaneToGeodeticCoordinates(const sbio::math::ReferencePlaneCoordinates& referencePlaneCoords,
                                                                                const sbio::math::SReferencePlaneCoordinateSystem& referencePlane)
    {
      GeocentricCoordinates geocentricPos = ConvertReferencePlaneToGeocentricCoordinates(referencePlaneCoords, referencePlane);
      return ConvertGeocentricToGeodeticCoordinates(geocentricPos);
    }

    sbio::math::GeocentricCoordinates ConvertReferencePlaneToGeocentricCoordinates(const sbio::math::ReferencePlaneCoordinates& referencePlaneCoords,
                                                                                   const sbio::math::SReferencePlaneCoordinateSystem& referencePlane)
    {
      return GeocentricCoordinates((referencePlane.rotation.toRotationMatrix() * referencePlaneCoords.toVec3()).toVec3() + referencePlane.referencePoint.toVec3());
    }

    ReferencePlaneCoordinates ConvertGeocentricToReferencePlaneCoordinates(const GeocentricCoordinates& geocentricPos, const SGeodeticCoordinates& origin)
    {
      return ConvertGeocentricToReferencePlaneCoordinates(geocentricPos, InitReferencePlaneCoordinates(origin));
    }

    TGeocentricReferencePlane GetEllipsoidTangentialPlane(Latitude latitude, Longitude longitude)
    {
      return CreateNEDSpatialReferenceFrame(SGeodeticCoordinates(latitude, longitude, HeightRelativeToWGS84Ellipsoid(0)));
    }

    Mat4 GetReferencePlaneTransformation(const SReferencePlaneCoordinateSystem& referencePlane)
    {
      return BuildReferencePlaneTransform(referencePlane);
    }

    TGeocentricReferencePlane CreateNEDSpatialReferenceFrame(SGeodeticCoordinates geodeticCoordinates)
    {
      vector<double> m;
      m.resize(9);

      double x = 0;
      double y = 0;
      double z = 0;

      TGeocentricReferencePlane referencePlane;
      GeographicLib::Geocentric::WGS84().Forward(geodeticCoordinates.latitude.Value(), geodeticCoordinates.longitude.Value(), geodeticCoordinates.altitude.Value(), x, y, z, m);
      referencePlane.east.set(m[0], m[3], m[6]);
      referencePlane.north.set(m[1], m[4], m[7]);
      referencePlane.down.set(-m[2], -m[5], -m[8]);

      return referencePlane;
    }

    std::vector<double> GetGeocentricRotationMatrix(Latitude lat, Longitude lon)
    {
      std::vector<double> rotationMatrix;
      rotationMatrix.resize(9);

      const Mat3 nedRotation = BuildNEDGeocentricRotation(lat, lon, 0).toRotationMatrix().toMat3();

      int z = 0;
      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          rotationMatrix[z++] = nedRotation(i, j);
        }
      }

      return rotationMatrix;
    }

    TGeocentricRotation GetGeocentricRotation(Latitude latitude, Longitude longitude)
    {
      return BuildNEDGeocentricRotation(latitude, longitude, 0);
    }

    void ToggleENU_NED(sbio::math::Mat3& mat)
    {
      int remap[3];
      int sign[3];
      remap[0] = 1;
      remap[1] = 0;
      remap[2] = 2;
      sign[0] = 1;
      sign[1] = 1;
      sign[2] = -1;
      sbio::math::Mat3 tmp = mat;

      for (int i = 0; i < 3; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          mat(i, j) = sign[i] * sign[j] * tmp(remap[i], remap[j]);
        }
      }
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
