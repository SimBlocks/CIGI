//Copyright SimBlocks LLC 2016-2026
/**
 * @file CoordinateConversions.h
 * @brief Declares functions for coordinate system conversions in geospatial applications.
 *
 * Provides functions for converting between geodetic, geocentric (ECEF), reference plane,
 * and body coordinate systems. Includes utilities for rotation, transformation, and
 * reference frame initialization, supporting geospatial and simulation applications.
 *
 * Functions include:
 * - Geodetic <-> Geocentric (ECEF) conversions
 * - Geocentric <-> Reference plane conversions
 * - Geocentric <-> Body rotation and Euler angle conversions
 * - Reference frame and NED (North, East, Down) frame creation
 * - Surface normal and calculations
 * - Matrix and rotation conversions for geospatial math
 */
#pragma once
#ifndef SIMBLOCKS_COORDINATE_CONVERSIONS_H
#define SIMBLOCKS_COORDINATE_CONVERSIONS_H

#include "GlobalHeaders/CommonTypes.h"
#include <vector>

namespace sbio
{
  namespace math
  {
    /**
     * @brief Converts geodetic coordinates (latitude, longitude, altitude) to geocentric (ECEF) coordinates.
     * @param geodeticCoords Geodetic coordinates to convert.
     * @return GeocentricCoordinates ECEF position.
     */
    sbio::math::GeocentricCoordinates ConvertGeodeticToGeocentricCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoords);

    /**
     * @brief Converts a geocentric transform to a body Euler rotation.
     * @param geocentricTransform Geocentric transform containing position and rotation.
     * @return TBodyEulerRotation Euler angles (roll, pitch, yaw).
     */
    sbio::math::TBodyEulerRotation ConvertGeocentricRotationToBodyEulerRotation(const sbio::math::TGeocentricTransform& geocentricTransform);

    /**
     * @brief Converts body Euler angles to a body rotation quaternion.
     * @param rotation Body Euler angles (roll, pitch, yaw).
     * @return TBodyRotation Body rotation quaternion.
     */
    sbio::math::TBodyRotation ConvertEulerRotationToBodyRotation(const TBodyEulerRotation& rotation);

    /**
     * @brief Converts a geocentric transform to a body rotation.
     * @param geocentricTransform Geocentric transform containing position and rotation.
     * @return TBodyRotation Body rotation quaternion.
     */
    sbio::math::TBodyRotation ConvertGeocentricRotationToBodyRotation(const sbio::math::TGeocentricTransform& geocentricTransform);

    /**
     * @brief Converts a geocentric rotation to a body rotation.
     * @param geocentricRotation Geocentric rotation quaternion.
     * @return TBodyRotation Body rotation quaternion.
      * @param bodyRotation Rotation value.
     */
    sbio::math::TBodyEulerRotation ConvertBodyRotationToBodyEulerRotation(const sbio::math::TBodyRotation& bodyRotation);

    /**
     * @brief Converts geocentric coordinates (ECEF) to geodetic coordinates (latitude, longitude, altitude).
     * @param geocentricPos Geocentric coordinates to convert.
     * @return SGeodeticCoordinates Geodetic coordinates.
     */
    sbio::math::SGeodeticCoordinates ConvertGeocentricToGeodeticCoordinates(const sbio::math::GeocentricCoordinates& geocentricPos);

    /**
     * @brief Converts geocentric (ECEF) coordinates to reference plane coordinates using the specified reference plane.
     * @param geocentricPos Geocentric coordinates to convert.
     * @param referencePlane Reference plane coordinate system.
     * @return ReferencePlaneCoordinates Reference plane coordinates.
     */
    sbio::math::ReferencePlaneCoordinates ConvertGeocentricToReferencePlaneCoordinates(const sbio::math::GeocentricCoordinates& geocentricPos, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane);

    /**
     * @brief Converts geocentric coordinates (ECEF) to reference plane coordinates using a geodetic origin.
     * @param geocentricPos Geocentric coordinates to convert.
     * @param origin Geodetic origin of the reference plane.
     * @return ReferencePlaneCoordinates Reference plane coordinates.
     */
    sbio::math::ReferencePlaneCoordinates ConvertGeocentricToReferencePlaneCoordinates(const sbio::math::GeocentricCoordinates& geocentricPos, const sbio::math::SGeodeticCoordinates& origin);

    /**
     * @brief Converts a geocentric transform to reference plane coordinates using the specified reference plane.
     * @param geocentricTransform Geocentric transform to convert.
     * @param referencePlane Reference plane coordinate system.
     * @return TReferencePlaneTransform Reference plane transform.
     */
    sbio::math::TReferencePlaneTransform ConvertGeocentricToReferencePlaneCoordinates(const sbio::math::TGeocentricTransform& geocentricTransform, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane);

    /**
     * @brief Converts a geocentric rotation to reference plane rotation using the specified reference plane.
     * @param geocentricTransform Geocentric rotation to convert.
     * @param referencePlane Reference plane coordinate system.
     * @return TReferencePlaneRotation Reference plane rotation.
     */
    sbio::math::TReferencePlaneRotation ConvertGeocentricToReferencePlaneRotation(const sbio::math::TGeocentricRotation& geocentricTransform, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane);

    /**
     * @brief Converts a geocentric transform to a geodetic transform.
     * @param geocentricTransform Geocentric transform containing position and rotation.
     * @return TGeodeticTransform Geodetic transform.
     */
    sbio::math::TGeodeticTransform ConvertGeocentricToGeodeticTransform(const sbio::math::TGeocentricTransform& geocentricTransform);

    /**
     * @brief Converts a geodetic transform to a geocentric transform.
     * @param geodeticTransform Geodetic transform containing position and rotation.
     * @return TGeocentricTransform Geocentric transform.
     */
    sbio::math::TGeocentricTransform ConvertGeodeticToGeocentricTransform(const sbio::math::TGeodeticTransform& geodeticTransform);

    /**
     * @brief Converts body Euler angles to a geocentric transform.
     * @param geodeticPosition Geodetic position for the transform.
     * @param rotation Body Euler angles (roll, pitch, yaw).
     * @return TGeocentricTransform Geocentric transform with position and rotation.
     */
    TGeocentricTransform ConvertGeodeticToGeocentricTransform(const SGeodeticCoordinates& geodeticPosition, const TBodyEulerRotation& rotation);

    /**
     * @brief Converts a geocentric rotation to a geocentric reference plane (NED axes).
     * @param geocentricRotation Geocentric rotation to convert.
     * @return TGeocentricReferencePlane Reference plane axes (north, east, down).
     */
    sbio::math::TGeocentricReferencePlane ConvertGeocentricToReferencePlane(const TGeocentricRotation& geocentricRotation);

    /**
     * @brief Converts reference plane to geodetic coordinates.
     * @param referencePlaneCoords Reference plane coords value.
     * @param referencePlane Reference plane value.
     * @return Converted value.
     */
    sbio::math::SGeodeticCoordinates ConvertReferencePlaneToGeodeticCoordinates(const sbio::math::ReferencePlaneCoordinates& referencePlaneCoords, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane);

    /**
     * @brief Converts reference plane to geocentric coordinates.
     * @param referencePlaneCoords Reference plane coords value.
     * @param referencePlane Reference plane value.
     * @return Converted value.
     */
    sbio::math::GeocentricCoordinates ConvertReferencePlaneToGeocentricCoordinates(const sbio::math::ReferencePlaneCoordinates& referencePlaneCoords, const sbio::math::SReferencePlaneCoordinateSystem& referencePlane);

    /**
     * @brief Creates a local NED (North, East, Down) spatial reference frame based on geodetic coordinates.
     * @param geodeticCoordinates Geodetic coordinates (latitude, longitude, altitude) of the reference origin.
     * @return TGeocentricReferencePlane Local NED reference plane.
     */
    TGeocentricReferencePlane CreateNEDSpatialReferenceFrame(SGeodeticCoordinates geodeticCoordinates);

    /**
     * @brief Returns a matrix representing the local NED (north, east, down) coordinate space at a point on the WGS84 ellipsoid.
     * @param latitude Latitude of the point.
     * @param longitude Longitude of the point.
     * @return TGeocentricReferencePlane Local NED axes.
     */
    TGeocentricReferencePlane GetEllipsoidTangentialPlane(sbio::math::Latitude latitude, sbio::math::Longitude longitude);

    /**
     * @brief Builds the reference plane transformation matrices and rotations based on a geodetic origin.
     * @param geodeticCoords Geodetic coordinates (latitude, longitude, altitude) of the reference origin.
     * @return SReferencePlaneCoordinateSystem Reference plane coordinate system.
     */
    SReferencePlaneCoordinateSystem InitReferencePlaneCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoords);

    /**
     * @brief Returns the transformation matrix from the reference plane to geocentric coordinates.
     * @param referencePlane Reference plane coordinate system.
     * @return Mat4 Transformation matrix.
     */
    sbio::math::Mat4 GetReferencePlaneTransformation(const sbio::math::SReferencePlaneCoordinateSystem& referencePlane);

    /**
     * @brief Returns a rotation matrix for a given latitude and longitude.
     * @param lat Latitude in degrees.
     * @param lon Longitude in degrees.
     * @return std::vector<double> 3x3 rotation matrix as a flat vector.
     */
    std::vector<double> GetGeocentricRotationMatrix(double lat, double lon);

    /**
     * @brief Returns a geocentric rotation object for a given latitude and longitude.
     * @param latitude Latitude in degrees.
     * @param longitude Longitude in degrees.
     * @return TGeocentricRotation Geocentric rotation matrix.
     */
    TGeocentricRotation GetGeocentricRotation(Latitude latitude, Longitude longitude);

    /**
     * @brief Converts a rotation matrix from ENU (East, North, Up) to NED (North, East, Down) coordinate system.
     * @param mat Rotation matrix to convert.
     */
    void ToggleENU_NED(sbio::math::Mat3& mat);
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
