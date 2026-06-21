//Copyright SimBlocks LLC 2016-2026
/**
 * @file MathTypes.h
 * @brief Common mathematical types and strong types for geospatial and simulation applications.
 *
 * Defines vector, matrix, quaternion, and coordinate types using Eigen, as well as strong types for angles, positions, and rotations.
 * Includes utilities for geodetic, geocentric (ECEF), ENU, NED, and body coordinate systems.
 *
 * Types include:
 * - Eigen-based vector, matrix, and quaternion types
 * - Strong types for angles, positions, and rotations
 * - Strongly typed coordinate and rotation systems (ENU, ECEF, NED, body)
 * - Geodetic, UTM, and bounding box types
 * - Reference plane and transform types
 * - Euler rotation and transform templates
 *
 * @see sbio::math namespace
 */
#pragma once
#ifndef SIMBLOCKS_MATH_TYPES_H
#define SIMBLOCKS_MATH_TYPES_H

#include <ostream>

// Silence Eigen warnings about conditional expressions being constant
#pragma warning(push)
#pragma warning(disable : 4127)
#include "Eigen/Geometry"
#pragma warning(pop)

#include "GlobalHeaders/StrongTypes.h"

namespace sbio
{
  namespace math
  {
    /// @name Eigen-based Vector and Matrix Types
    ///@{
    typedef Eigen::Vector2f Vec2f;///< 2D float vector
    typedef Eigen::Vector2i Vec2n;///< 2D integer vector
    typedef Eigen::Vector2d Vec2d;///< 2D double vector
    typedef Eigen::Vector3f Vec3f;///< 3D float vector
    typedef Eigen::Vector3d Vec3;///< 3D double vector
    typedef Eigen::Vector4f Vec4f;///< 4D float vector
    typedef Eigen::Vector4d Vec4;///< 4D double vector
    typedef Eigen::Matrix3f Mat3f;///< 3x3 float matrix (column-major)
    typedef Eigen::Matrix3d Mat3;///< 3x3 double matrix (column-major)
    typedef Eigen::Matrix4f Mat4f;///< 4x4 float matrix (column-major)
    typedef Eigen::Matrix4d Mat4;///< 4x4 double matrix (column-major)
    typedef Eigen::AngleAxisd AxisRotation;///< 3D axis-angle rotation (double)
    typedef Eigen::Quaternionf Quaternion4f;///< 4D float quaternion
    typedef Eigen::Quaterniond Quaternion4d;///< 4D double quaternion
    ///@}

    /// @name Mathematical Constants
    ///@{
    const float SBIO_SINGLE_PI = 3.141592f;///< Single-precision PI
    const double SBIO_DOUBLE_PI = 3.14159265358979323846;///< Double-precision PI
    ///@}

    /// @name Strong Angle Types
    ///@{
    RANGED_STRONG_FLOAT(Degrees, float, 0, 360)///< Degrees in [0, 360]
    RANGED_STRONG_FLOAT(Degrees90, float, -90, 90)///< Degrees in [-90, 90]
    RANGED_STRONG_FLOAT(Degrees180, float, -180, 180)///< Degrees in [-180, 180]
    RANGED_STRONG_FLOAT(Degrees360, float, -360, 360)///< Degrees in [-360, 360]
    RANGED_STRONG_FLOAT(Radians, float, 0, 2 * SBIO_SINGLE_PI)///< Radians in [0, 2*PI]
    ///@}
  }
}

#include "StrongVec3.h"

#define STRONG_VEC3(TYPE) STRONG_TYPE3(TYPE, double, StrongVec3)
#define STRONG_VEC3F(TYPE) STRONG_TYPE3(TYPE, float, StrongVec3f)
#define STRONG_AXES(TYPE, AXIS0, AXIS1, AXIS2) STRONG_TYPE_AXES(TYPE, double, StrongVec3, AXIS0, AXIS1, AXIS2)

#include "StrongMat3.h"

namespace sbio
{
  namespace math
  {
    /**
     * @brief Quaternion with coordinate system type.
     * @tparam TCoordinates Coordinate system type.
     */
    template <typename TCoordinates>
    struct SQuaternion : public Quaternion4d
    {
      /**
       * @brief Constructs an identity quaternion.
       */
      SQuaternion() : Quaternion4d(Quaternion4d::Identity())
      {
      }

      /**
       * @brief Constructs a quaternion from scalar components.
       * @param w Scalar component.
       * @param x X vector component.
       * @param y Y vector component.
       * @param z Z vector component.
       */
      SQuaternion(const Scalar& w, const Scalar& x, const Scalar& y, const Scalar& z) : Quaternion4d(w, x, y, z)
      {
      }

      /**
       * @brief Constructs a strong quaternion from an Eigen quaternion.
       * @param q Quaternion value to copy.
       */
      SQuaternion(const Quaternion4d& q) : Quaternion4d(q.w(), q.x(), q.y(), q.z())
      {
      }

      /**
       * @brief Constructs a quaternion from a strong rotation matrix.
       * @param m Rotation matrix to convert.
       */
      SQuaternion(StrongMat3<TCoordinates> m) : Quaternion4d(m.toMat3())
      {
      }

      using Quaternion4d::operator*=;
      using Quaternion4d::operator*;
      using Quaternion4d::operator=;

      /**
       * @brief Converts quaternion to a strong-typed rotation matrix.
       * @return StrongMat3<TCoordinates> Rotation matrix.
       */
      StrongMat3<TCoordinates> toRotationMatrix() const
      {
        return StrongMat3<TCoordinates>(Quaternion4d::toRotationMatrix());
      }
    };

    /**
     * @brief Stores a strongly typed quaternion rotation between coordinate frames.
     * @tparam TFromCoordinates Coordinate type accepted by `Rotate()`.
     * @tparam TToCoordinates Coordinate type returned by `Rotate()`.
     */
    template <typename TFromCoordinates, typename TToCoordinates>
    class TRotation
    {
    public:
      /**
       * @brief Constructs an identity rotation.
       */
      TRotation() : m_Rotation()
      {
      }

      /**
       * @brief Constructs a rotation from a target-coordinate quaternion.
       * @param rotation Quaternion value to store.
       */
      explicit TRotation(const SQuaternion<TToCoordinates>& rotation) : m_Rotation(rotation)
      {
      }

      /**
       * @brief Constructs a rotation from a target-coordinate rotation matrix.
       * @param rotation Rotation matrix to convert and store.
       */
      explicit TRotation(StrongMat3<TToCoordinates> rotation) : m_Rotation(rotation)
      {
      }

      /**
       * @brief Replaces the stored rotation with a quaternion.
       * @param rotation Quaternion value to assign.
       * @return Reference to this rotation.
       */
      TRotation& operator=(const SQuaternion<TToCoordinates>& rotation)
      {
        m_Rotation = rotation;
        return *this;
      }

      /**
       * @brief Sets the stored rotation to identity.
       */
      void setIdentity()
      {
        m_Rotation.setIdentity();
      }

      /**
       * @brief Rotates coordinates from the source frame into the target frame.
       * @param coordinates Coordinates in the source frame.
       * @return Coordinates in the target frame.
       */
      TToCoordinates Rotate(const TFromCoordinates& coordinates) const
      {
        return TToCoordinates(m_Rotation * coordinates.toVec3());
      }

      /**
       * @brief Rotates coordinates from the source frame into the target frame.
       * @param coordinates Coordinates in the source frame.
       * @return Coordinates in the target frame.
       */
      TToCoordinates operator*(const TFromCoordinates& coordinates) const
      {
        return Rotate(coordinates);
      }

      /**
       * @brief Composes this rotation with a source-frame quaternion.
       * @param rhs Quaternion to apply before this rotation.
       * @return Composed rotation from the original source frame to the target frame.
       */
      TRotation<TFromCoordinates, TToCoordinates> operator*(const SQuaternion<TFromCoordinates>& rhs) const
      {
        return TRotation<TFromCoordinates, TToCoordinates>(m_Rotation * rhs);
      }

      /**
       * @brief Composes this rotation with another typed rotation.
       * @tparam TNewFromCoordinates Source coordinate type of `rhs`.
       * @param rhs Rotation to apply before this rotation.
       * @return Composed rotation from `TNewFromCoordinates` to `TToCoordinates`.
       */
      template <typename TNewFromCoordinates>
      TRotation<TNewFromCoordinates, TToCoordinates> operator*(const TRotation<TNewFromCoordinates, TFromCoordinates>& rhs) const
      {
        return TRotation<TNewFromCoordinates, TToCoordinates>(m_Rotation * rhs.Rotation());
      }

      /**
       * @brief Converts the rotation to its stored quaternion representation.
       * @return Stored quaternion.
       */
      operator SQuaternion<TToCoordinates>() const
      {
        return m_Rotation;
      }

      /**
       * @brief Computes the inverse rotation.
       * @return Quaternion representing the inverse rotation in the source coordinate type.
       */
      SQuaternion<TFromCoordinates> inverse() const
      {
        return SQuaternion<TFromCoordinates>(m_Rotation.inverse());
      }

      /**
       * @brief Converts the stored quaternion to a strong rotation matrix.
       * @return Rotation matrix in the target coordinate type.
       */
      StrongMat3<TToCoordinates> toRotationMatrix() const
      {
        return m_Rotation.toRotationMatrix();
      }

      /**
       * @brief Gets the stored quaternion.
       * @return Const reference to the stored quaternion.
       */
      const SQuaternion<TToCoordinates>& Rotation() const
      {
        return m_Rotation;
      }

    private:
      /**
       * @brief Constructs a rotation from an untyped Eigen quaternion.
       * @param rotation Quaternion value to store.
       */
      explicit TRotation(const Quaternion4d& rotation) : m_Rotation(rotation)
      {
      }

      SQuaternion<TToCoordinates> m_Rotation;
    };

    /**
     * @brief Transform with position and rotation, both strongly typed.
     * @tparam TPositionCoordinates Position coordinate type.
     * @tparam TRotationCoordinates Rotation coordinate type.
     */
    template <typename TPositionCoordinates, typename TRotationFromCoordinates, typename TRotationToCoordinates = TRotationFromCoordinates>
    struct STransform
    {
      TPositionCoordinates pos;///< Position
      TRotation<TRotationFromCoordinates, TRotationToCoordinates> rotation;///< Rotation

      /**
       * @brief Constructs a transform with an identity rotation.
       */
      STransform()
      {
        rotation.setIdentity();
      }
    };

    /**
     * @brief Euler rotation angles (yaw, pitch, roll) in degrees.
     * @tparam TCoordinates Coordinate system type.
     */
    template <typename TCoordinates>
    struct SEulerRotation
    {
      Degrees yaw = Degrees(0);///< Yaw angle
      Degrees pitch = Degrees(0);///< Pitch angle
      Degrees roll = Degrees(0);///< Roll angle
    };

    /// @name Strong Geodetic Types
    ///@{
    RANGED_STRONG_DOUBLE(Latitude, double, -90, 90)///< Latitude in degrees [-90, 90]
    RANGED_STRONG_DOUBLE(Longitude, double, -180, 180)///< Longitude in degrees [-180, 180]
    STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(HeightRelativeToWGS84Ellipsoid, double, std::numeric_limits<double>::lowest())///< Height above the WGS84 ellipsoid in meters

    ///@}

    /**
     * @brief Latitude/Longitude pair.
     */
    struct SLatLon
    {
      Latitude latitude = Latitude(0);///< Latitude
      Longitude longitude = Longitude(0);///< Longitude

      /**
       * @brief Constructs a `SLatLon` instance.
       */
      SLatLon();
      /**
       * @brief Constructs a `SLatLon` instance.
       * @param latitude Latitude value.
       * @param longitude Longitude value.
       */
      SLatLon(Latitude latitude, Longitude longitude);
    };

    /**
     * @brief Geodetic coordinates (latitude, longitude, altitude).
     */
    struct SGeodeticCoordinates
    {
      Latitude latitude = Latitude(0);///< Latitude
      Longitude longitude = Longitude(0);///< Longitude
      HeightRelativeToWGS84Ellipsoid altitude = HeightRelativeToWGS84Ellipsoid(0);///< Height above the WGS84 ellipsoid in meters

      /**
       * @brief Constructs a `SGeodeticCoordinates` instance.
       */
      SGeodeticCoordinates() = default;

      /**
       * @brief Constructs a `SGeodeticCoordinates` instance.
       * @param latitude Latitude value.
       * @param longitude Longitude value.
       * @param fAlt Alt floating-point value.
       */
      SGeodeticCoordinates(Latitude latitude, Longitude longitude, HeightRelativeToWGS84Ellipsoid fAlt);
      /**
       * @brief Constructs a `SGeodeticCoordinates` instance.
       * @param latitude Latitude value.
       * @param longitude Longitude value.
       * @param fAlt Alt floating-point value.
       */
      SGeodeticCoordinates(Latitude latitude, Longitude longitude, double fAlt);
    };

    /**
     * @brief UTM (Universal Transverse Mercator) coordinates.
     */
    struct SUTMCoordinates
    {
      double northing = 0;///< Northing in meters
      double easting = 0;///< Easting in meters

      /**
       * @brief Constructs a `SUTMCoordinates` instance.
       */
      SUTMCoordinates() = default;

      /**
       * @brief Returns the result of `FromEastingNorthing`.
       * @param easting Easting value.
       * @param northing Northing value.
       * @return UTM coordinate initialized from easting and northing values.
       */
      static SUTMCoordinates FromEastingNorthing(double easting, double northing);
    };

    /**
     * @brief UTM rectangular extent represented by upper-left and lower-right corners.
     */
    struct SUTMExtent
    {
      SUTMCoordinates upperLeft;///< Upper-left UTM coordinate
      SUTMCoordinates lowerRight;///< Lower-right UTM coordinate

      /**
       * @brief Constructs a `SUTMExtent` instance.
       */
      SUTMExtent() = default;

      /**
       * @brief Constructs a `SUTMExtent` instance.
       * @param upperLeft Upper left value.
       * @param lowerRight Lower right value.
       */
      SUTMExtent(const SUTMCoordinates& upperLeft, const SUTMCoordinates& lowerRight);
    };

    /**
     * @brief Strongly typed EPSG code.
     *
     * Represents a specific EPSG geodetic parameter dataset code for a coordinate reference systems.
     * Used to identify geospatial reference systems in a strongly typed manner.
      * @param EPSG Psg enumeration value.
      * @param int Int value.
     */
    STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(EPSG, int, -1);

    /**
     * @brief Geographic extents (bounding box).
     */
    struct SExtents
    {
      Latitude North = UnknownLatitude;///< Northern latitude
      Latitude South = UnknownLatitude;///< Southern latitude
      Longitude East = UnknownLongitude;///< Eastern longitude
      Longitude West = UnknownLongitude;///< Western longitude
    };

    /**
     * @brief Reference plane axes (north, east, down).
     * @tparam T Coordinate type.
     */
    template <typename T>
    struct SReferencePlane
    {
      T north;///< North axis
      T east;///< East axis
      T down;///< Down axis
    };

    /// @name Strongly Typed 3D Coordinate Systems
    ///@{
    STRONG_AXES(ENUCoordinates, East, North, Up);///< East-North-Up (ENU), right-handed
    STRONG_AXES(GeocentricCoordinates, X, Y, Z);///< Earth-Centered, Earth-Fixed (ECEF), right-handed
    STRONG_AXES(ReferencePlaneCoordinates, North, East, Down);///< Tangential plane North-East-Down (NED), right-handed
    STRONG_AXES(BodyCoordinates, Right, Forward, Up);///< Body coordinates (right, forward, up), right-handed

    ///@}

    /**
     * @brief Output stream operator for GeocentricCoordinates.
     */
    inline std::ostream& operator<<(std::ostream& s, const GeocentricCoordinates& rhs)
    {
      s << rhs[0] << ", " << rhs[1] << ", " << rhs[2];
      return s;
    }

    /// @name Strongly Typed Rotations and Transforms
    ///@{
    typedef SQuaternion<GeocentricCoordinates> TGeocentricRotation;///< Geocentric rotation quaternion
    typedef SQuaternion<ReferencePlaneCoordinates> TReferencePlaneRotation;///< Reference plane rotation quaternion
    typedef SQuaternion<BodyCoordinates> TBodyRotation;///< Body rotation quaternion
    typedef SEulerRotation<BodyCoordinates> TBodyEulerRotation;///< Body Euler angles

    typedef SReferencePlane<GeocentricCoordinates> TGeocentricReferencePlane;///< Geocentric reference plane axes

    typedef STransform<BodyCoordinates, BodyCoordinates> TBodyTransform;///< Body transform
    typedef STransform<ReferencePlaneCoordinates, BodyCoordinates, ReferencePlaneCoordinates> TReferencePlaneTransform;///< Reference plane transform
    typedef STransform<GeocentricCoordinates, BodyCoordinates, GeocentricCoordinates> TGeocentricTransform;///< Geocentric transform
    typedef STransform<SGeodeticCoordinates, BodyCoordinates, GeocentricCoordinates> TGeodeticTransform;///< Geodetic transform

    typedef StrongMat3<BodyCoordinates> TBodyMatrix;///< Body rotation matrix
    typedef StrongMat3<GeocentricCoordinates> TGeocentricMatrix;///< Geocentric rotation matrix

    ///@}

    /**
     * @brief Reference-plane coordinate system definition.
     *
     * Stores the rotation between geocentric and reference-plane coordinates and the geocentric
     * reference point for the coordinate system.
     */
    struct SReferencePlaneCoordinateSystem
    {
      TGeocentricRotation rotation;///< Rotation from reference-plane coordinates to geocentric coordinates.
      TReferencePlaneRotation inverseRotation;///< Inverse rotation from geocentric coordinates to reference-plane coordinates.
      GeocentricCoordinates referencePoint;///< Reference point in geocentric coordinates.

      /**
       * @brief Constructs a `SReferencePlaneCoordinateSystem` instance.
       */
      SReferencePlaneCoordinateSystem();
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
