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

    TGeocentricRotation BuildNEDGeocentricRotation(double latitude, double longitude, double altitude)
    {
      std::vector<double> rotationMatrix;
      rotationMatrix.resize(9);

      double x = 0;
      double y = 0;
      double z = 0;

      GeographicLib::Geocentric::WGS84().Forward(latitude, longitude, altitude, x, y, z, rotationMatrix);
      return TGeocentricRotation(BuildNEDRotationMatrix(rotationMatrix));
    }

    /**
     * @brief Builds the reference plane transformation matrices and rotations based on a geodetic origin.
     * @param geodeticCoords Geodetic coordinates (latitude, longitude, altitude) of the reference origin.
     * @return Reference plane coordinate system.
     */
    SReferencePlaneCoordinateSystem InitReferencePlaneCoordinates(const SGeodeticCoordinates& geodeticCoords)
    {
      vector<double> m;
      m.resize(9);

      SReferencePlaneCoordinateSystem referencePlane;
      GeographicLib::Geocentric::WGS84().Forward(geodeticCoords.latitude.Value(), geodeticCoords.longitude.Value(), geodeticCoords.altitude.Value(), referencePlane.referencePoint[0], referencePlane.referencePoint[1], referencePlane.referencePoint[2], m);

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

    /**
     * @brief Converts geodetic coordinates (latitude, longitude, altitude) to geocentric coordinates (ECEF).
     * @param geodeticCoords Geodetic coordinates to convert.
     * @return Converted geocentric coordinates.
     */
    GeocentricCoordinates ConvertGeodeticToGeocentricCoordinates(const SGeodeticCoordinates& geodeticCoords)
    {
      GeocentricCoordinates pos;
      GeographicLib::Geocentric::WGS84().Forward(geodeticCoords.latitude.Value(), geodeticCoords.longitude.Value(), geodeticCoords.altitude.Value(), pos[0], pos[1], pos[2]);

      return pos;
    }

    /**
     * @brief Converts geocentric rotation transformation to body Euler angles rotation.
     * @param geocentricTransform Geocentric transformation to convert.
     * @return Converted body Euler angles rotation.
     */
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

    /**
     * @brief Converts geocentric rotation transformation to body rotation.
     * @param geocentricTransform Geocentric transformation to convert.
     * @return Converted body rotation.
     */
    sbio::math::TBodyRotation ConvertGeocentricRotationToBodyRotation(const sbio::math::TGeocentricTransform& geocentricTransform)
    {
      auto geod = ConvertGeocentricToGeodeticCoordinates(geocentricTransform.pos);
      return TBodyRotation(GetGeocentricRotation(geod.latitude, geod.longitude).inverse() * geocentricTransform.rotation.Rotation());
    }

    /**
     * @brief Converts body rotation to body Euler angles rotation.
     * @param bodyRotation Body rotation to convert.
     * @return Converted body Euler angles rotation.
     */
    sbio::math::TBodyEulerRotation ConvertBodyRotationToBodyEulerRotation(const sbio::math::TBodyRotation& bodyRotation)
    {
      //            NED     enu
      //  forward   x        y
      //  right     y        x
      //  down     -z       -z
      Quaternion4d q = Quaternion4d(bodyRotation.w(), bodyRotation.y(), bodyRotation.x(), -bodyRotation.z());
      TBodyEulerRotation rotation;

      double sinr_cosp = 2 * (q.w() * q.x() + q.y() * q.z());
      double cosr_cosp = 1 - 2 * (q.x() * q.x() + q.y() * q.y());
      rotation.roll = RadiansToDegrees(Radians(static_cast<float>(std::atan2(sinr_cosp, cosr_cosp))));

      double sinp = 2 * (q.w() * q.y() - q.z() * q.x());
      if (std::abs(sinp) >= 1)
      {
        rotation.pitch = RadiansToDegrees(Radians(static_cast<float>(std::copysign(M_PI / 2.0, sinp))));
      }
      else
      {
        rotation.pitch = RadiansToDegrees(Radians(static_cast<float>(std::asin(sinp))));
      }

      double siny_cosp = 2 * (q.w() * q.z() + q.x() * q.y());
      double cosy_cosp = 1 - 2 * (q.y() * q.y() + q.z() * q.z());
      rotation.yaw = RadiansToDegrees(Radians(static_cast<float>(std::atan2(siny_cosp, cosy_cosp))));
      return rotation;
    }

    /**
     * @brief Converts geocentric coordinates (ECEF) to geodetic coordinates (latitude, longitude, altitude).
     * @param geocentricPos Geocentric coordinates to convert.
     * @return Converted geodetic coordinates.
     */
    SGeodeticCoordinates ConvertGeocentricToGeodeticCoordinates(const GeocentricCoordinates& geocentricPos)
    {
      SGeodeticCoordinates geodeticCoords;

      double fLatitude = 0;
      double fLongitude = 0;
      double altitude = 0;
      GeographicLib::Geocentric::WGS84().Reverse(geocentricPos[0], geocentricPos[1], geocentricPos[2], fLatitude, fLongitude, altitude);//lat, long, and alt passed by reference
      geodeticCoords.altitude = HeightRelativeToWGS84Ellipsoid(altitude);
      geodeticCoords.latitude = Latitude(fLatitude);
      geodeticCoords.longitude = Longitude(fLongitude);

      return geodeticCoords;
    }

    /**
     * @brief Converts geocentric coordinates (ECEF) to reference plane coordinates.
     * @param geocentricPos Geocentric coordinates to convert.
     * @param referencePlane Reference plane coordinate system.
     * @return Converted reference plane coordinates.
     */
    ReferencePlaneCoordinates ConvertGeocentricToReferencePlaneCoordinates(const GeocentricCoordinates& geocentricPos, const SReferencePlaneCoordinateSystem& referencePlane)
    {
      const Vec3 relativePosition = geocentricPos.toVec3() - referencePlane.referencePoint.toVec3();
      return ReferencePlaneCoordinates(referencePlane.inverseRotation.toRotationMatrix() * relativePosition);
    }

    /**
     * @brief Converts geocentric transformation to reference plane transformation.
     * @param geocentricTransform Geocentric transformation to convert.
     * @param referencePlane Reference plane coordinate system.
     * @return Converted reference plane transformation.
     */
    TReferencePlaneTransform ConvertGeocentricToReferencePlaneCoordinates(const TGeocentricTransform& geocentricTransform, const SReferencePlaneCoordinateSystem& referencePlane)
    {
      TReferencePlaneTransform referencePlaneTransform;
      referencePlaneTransform.pos = ConvertGeocentricToReferencePlaneCoordinates(geocentricTransform.pos, referencePlane);
      referencePlaneTransform.rotation = ConvertGeocentricToReferencePlaneRotation(geocentricTransform.rotation, referencePlane);

      return referencePlaneTransform;
    }

    /**
     * @brief Converts geocentric rotation to reference plane rotation.
     * @param geocentricRotation Geocentric rotation to convert.
     * @param referencePlane Reference plane coordinate system.
     * @return Converted reference plane rotation.
     */
    TReferencePlaneRotation ConvertGeocentricToReferencePlaneRotation(const TGeocentricRotation& geocentricRotation, const SReferencePlaneCoordinateSystem& referencePlane)
    {
      return referencePlane.inverseRotation * geocentricRotation;
    }

    /**
     * @brief Converts geocentric transformation to geodetic transformation.
     * @param geocentricTransform Geocentric transformation to convert.
     * @return Converted geodetic transformation.
     */
    sbio::math::TGeodeticTransform ConvertGeocentricToGeodeticTransform(const sbio::math::TGeocentricTransform& geocentricTransform)
    {
      TGeodeticTransform geodeticTransform;
      geodeticTransform.pos = ConvertGeocentricToGeodeticCoordinates(geocentricTransform.pos);
      geodeticTransform.rotation = geocentricTransform.rotation;
      return geodeticTransform;
    }

    /**
     * @brief Converts geodetic transformation to geocentric transformation.
     * @param geodeticTransform Geodetic transformation to convert.
     * @return Converted geocentric transformation.
     */
    sbio::math::TGeocentricTransform ConvertGeodeticToGeocentricTransform(const sbio::math::TGeodeticTransform& geodeticTransform)
    {
      TGeocentricTransform geocentricTransform;
      geocentricTransform.pos = ConvertGeodeticToGeocentricCoordinates(geodeticTransform.pos);
      geocentricTransform.rotation = geodeticTransform.rotation;
      return geocentricTransform;
    }

    TGeocentricTransform ConvertGeodeticToGeocentricTransform(const SGeodeticCoordinates& geodeticPosition, const TBodyEulerRotation& rotation)
    {
      TGeocentricTransform worldTransform;
      worldTransform.pos = ConvertGeodeticToGeocentricCoordinates(geodeticPosition);
      worldTransform.rotation = GetGeocentricRotation(geodeticPosition.latitude, geodeticPosition.longitude) * ConvertEulerRotationToBodyRotation(rotation);
      return worldTransform;
    }

    /**
     * @brief Creates a geocentric reference plane from a geocentric rotation.
     * @param geocentricRotation Geocentric rotation to base the reference plane on.
     * @return Created geocentric reference plane.
     */
    TGeocentricReferencePlane ConvertGeocentricToReferencePlane(const TGeocentricRotation& geocentricRotation)
    {
      TGeocentricReferencePlane referencePlane;
      TGeocentricMatrix m = geocentricRotation.toRotationMatrix();
      referencePlane.north = m.getCol(1);
      referencePlane.east = m.getCol(0);
      referencePlane.down = m.getCol(2).negated();
      return referencePlane;
    }

    sbio::math::SGeodeticCoordinates ConvertReferencePlaneToGeodeticCoordinates(const sbio::math::ReferencePlaneCoordinates& referencePlaneCoords, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane)
    {
      GeocentricCoordinates geocentricPos = ConvertReferencePlaneToGeocentricCoordinates(referencePlaneCoords, referencePlane);
      return ConvertGeocentricToGeodeticCoordinates(geocentricPos);
    }

    sbio::math::GeocentricCoordinates ConvertReferencePlaneToGeocentricCoordinates(const sbio::math::ReferencePlaneCoordinates& referencePlaneCoords, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane)
    {
      return GeocentricCoordinates((referencePlane.rotation.toRotationMatrix() * referencePlaneCoords.toVec3()).toVec3() + referencePlane.referencePoint.toVec3());
    }

    /**
     * @brief Converts geocentric coordinates (ECEF) to reference plane coordinates, using a specified origin.
     * @param geocentricPos Geocentric coordinates to convert.
     * @param origin Origin of the reference system in geodetic coordinates.
     * @return Converted reference plane coordinates.
     */
    ReferencePlaneCoordinates ConvertGeocentricToReferencePlaneCoordinates(const GeocentricCoordinates& geocentricPos, const SGeodeticCoordinates& origin)
    {
      return ConvertGeocentricToReferencePlaneCoordinates(geocentricPos, InitReferencePlaneCoordinates(origin));
    }

    /**
     * @brief Calculates the tangential plane to the ellipsoid at a given latitude and longitude.
     * @param latitude Latitude at which to calculate the tangent plane.
     * @param longitude Longitude at which to calculate the tangent plane.
     * @return Calculated geocentric reference plane.
     */
    TGeocentricReferencePlane GetEllipsoidTangentialPlane(Latitude latitude, Longitude longitude)
    {
      return CreateNEDSpatialReferenceFrame(SGeodeticCoordinates(latitude, longitude, HeightRelativeToWGS84Ellipsoid(0)));
    }

    /**
     * @brief Gets the current reference plane transformation matrix.
     * @param referencePlane Reference plane coordinate system.
     * @return Current reference plane transformation matrix.
     */
    Mat4 GetReferencePlaneTransformation(const SReferencePlaneCoordinateSystem& referencePlane)
    {
      return BuildReferencePlaneTransform(referencePlane);
    }

    /**
     * @brief Creates a NED (North, East, Down) spatial reference frame based on geodetic coordinates.
     * @param geodeticCoordinates Geodetic coordinates for the reference frame origin.
     * @return Created NED spatial reference frame.
     */
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

    /**
     * @brief Gets the geocentric rotation matrix for a given latitude and longitude.
     * @param lat Latitude for which to get the rotation matrix.
     * @param lon Longitude for which to get the rotation matrix.
     * @return Geocentric rotation matrix.
     */
    std::vector<double> GetGeocentricRotationMatrix(double lat, double lon)
    {
      std::vector<double> rotationMatrix;
      rotationMatrix.resize(9);

      const Mat3& nedRotation = BuildNEDGeocentricRotation(lat, lon, 0).toRotationMatrix().toMat3();

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

    /**
     * @brief Gets the geocentric rotation for a given latitude and longitude.
     * @param latitude Latitude for which to get the geocentric rotation.
     * @param longitude Longitude for which to get the geocentric rotation.
     * @return Geocentric rotation.
     */
    TGeocentricRotation GetGeocentricRotation(Latitude latitude, Longitude longitude)
    {
      return BuildNEDGeocentricRotation(latitude.Value(), longitude.Value(), 0);
    }

    /**
     * @brief Toggles between ENU (East, North, Up) and NED (North, East, Down) coordinate systems.
     * @param mat Matrix to toggle between ENU and NED.
     */
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
