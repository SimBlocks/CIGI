//Copyright SimBlocks LLC 2016-2026
/**
 * @file Math.cpp
 * @brief Implements mathematical utility functions for angle conversions, equality checks, and type conversions.
 *
 * This file provides the implementation of functions for converting between degrees and radians, checking floating-point equality,
 * and converting between vector and matrix types. Useful for geospatial, graphics, and simulation applications.
 *
 * Functions include:
 * - Degrees <-> Radians conversions (float, double, strong types)
 * - Floating-point equality checks
 * - Vec3 <-> Vec3f and Mat3 <-> Mat3f conversions
 *
 * @see sbio::math namespace
 */
#include "Math.h"
#include <Eigen/Geometry>
#include <limits>

#define _USE_MATH_DEFINES
#include <math.h>

namespace sbio
{
  namespace math
  {
    constexpr float FLOAT_ABSOLUTE_TOLERANCE = 1.0e-6f;
    constexpr float FLOAT_RELATIVE_TOLERANCE = 1.0e-7f;
    constexpr double DOUBLE_ABSOLUTE_TOLERANCE = 1.0e-8;
    constexpr double DOUBLE_RELATIVE_TOLERANCE = 1.0e-10;

    /**
     * @brief Converts degrees to radians.
     * @param degrees The angle in degrees.
     * @return The angle in radians.
     */
    float DegreesToRadians(float degrees)
    {
      return degrees * static_cast<float>(M_PI) / 180.0f;
    }

    /**
     * @brief Converts degrees to radians (strong type).
     * @param degrees The angle in degrees (strong type).
     * @return The angle in radians (strong type) in [0, 2*PI].
     */
    Radians DegreesToRadians(Degrees degrees)
    {
      return Radians(DegreesToRadians(degrees.Value()));
    }

    /**
     * @brief Converts radians to degrees.
     * @param radians The angle in radians.
     * @return The angle in degrees.
     */
    float RadiansToDegrees(float radians)
    {
      return radians * static_cast<float>(180.0f / (M_PI));
    }

    /**
     * @brief Converts radians to degrees (strong type).
     * @param radians The angle in radians (strong type).
     * @return The angle in degrees (strong type).
     */
    Degrees RadiansToDegrees(Radians radians)
    {
      return Degrees(RadiansToDegrees(radians.Value()));
    }

    /**
     * @brief Checks if two floating-point numbers are approximately equal (float).
     * @param a First number.
     * @param b Second number.
     * @return True if the numbers are approximately equal, false otherwise.
     */
    bool fequals(float a, float b)
    {
      if (a == b)
      {
        return true;
      }

      const float fDifference = std::fabs(a - b);
      const float fMaxMagnitude = std::fmax(std::fabs(a), std::fabs(b));
      const float fTolerance = std::fmax(FLOAT_ABSOLUTE_TOLERANCE, FLOAT_RELATIVE_TOLERANCE * fMaxMagnitude);
      return fDifference <= fTolerance;
    }

    /**
     * @brief Checks if two double-precision floating-point numbers are approximately equal.
     * @param a First number.
     * @param b Second number.
     * @return True if the numbers are approximately equal, false otherwise.
     */
    bool fequals(double a, double b)
    {
      if (a == b)
      {
        return true;
      }

      const double fDifference = std::fabs(a - b);
      const double fMaxMagnitude = std::fmax(std::fabs(a), std::fabs(b));
      const double fTolerance = std::fmax(DOUBLE_ABSOLUTE_TOLERANCE, DOUBLE_RELATIVE_TOLERANCE * fMaxMagnitude);
      return fDifference <= fTolerance;
    }

    /**
     * @brief Converts a Vec3 to a Vec3f.
     * @param vec The Vec3 to convert.
     * @return The converted Vec3f.
     */
    Vec3f ConvertToVec3f(const sbio::math::Vec3 vec)
    {
      Vec3f v;
      v[0] = static_cast<float>(vec[0]);
      v[1] = static_cast<float>(vec[1]);
      v[2] = static_cast<float>(vec[2]);

      return v;
    }

    /**
     * @brief Converts a Mat3 to a Mat3f.
     * @param matrix The Mat3 to convert.
     * @return The converted Mat3f.
     */
    Mat3f ConvertToMat3f(const sbio::math::Mat3 matrix)
    {
      Mat3f m;
      for (int r = 0; r < 3; ++r)
      {
        for (int c = 0; c < 3; ++c)
        {
          m(r, c) = static_cast<float>(matrix(r, c));
        }
      }

      return m;
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
