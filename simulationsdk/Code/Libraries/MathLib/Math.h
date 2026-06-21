//Copyright SimBlocks LLC 2016-2026
/**
 * @file Math.h
 * @brief Mathematical utility functions for angle conversions, equality checks, and type conversions.
 *
 * Provides functions for converting between degrees and radians, checking floating-point equality,
 * and converting between vector and matrix types. Useful for geospatial, graphics, and simulation applications.
 *
 * Functions include:
 * - Degrees <-> Radians conversions (float, double, strong types)
 * - Floating-point equality checks
 * - Vec3 <-> Vec3f and Mat3 <-> Mat3f conversions
 *
 * @see sbio::math namespace
 */
#pragma once
#ifndef SIMBLOCKS_MATH_CONVERSIONS_H
#define SIMBLOCKS_MATH_CONVERSIONS_H

#include "MathTypes.h"

namespace sbio
{
  namespace math
  {
    class CProjection;

    /**
     * @brief Converts degrees to radians.
     * @param fDegrees The angle in degrees.
     * @return The angle in radians.
     */
    float DegreesToRadians(float fDegrees);

    /**
     * @brief Converts degrees to radians.
     * @param fDegrees The angle in degrees.
     * @return The angle in radians.
     */
    Radians DegreesToRadians(Degrees fDegrees);

    /**
     * @brief Converts radians to degrees.
     * @param fRadians The angle in radians.
     * @return The angle in degrees.
     */
    float RadiansToDegrees(float fRadians);

    /**
     * @brief Converts radians to degrees.
     * @param fRadians The angle in radians.
     * @return The angle in degrees.
     */
    Degrees RadiansToDegrees(Radians fRadians);

    /**
     * @brief Checks if two floating-point numbers are approximately equal.
     * @param a First number.
     * @param b Second number.
     * @return True if the numbers are approximately equal, false otherwise.
     */
    bool fequals(float a, float b);

    /**
     * @brief Checks if two double-precision floating-point numbers are approximately equal.
     * @param a First number.
     * @param b Second number.
     * @return True if the numbers are approximately equal, false otherwise.
     */
    bool fequals(double a, double b);

    /**
     * @brief Converts a Vec3 to a Vec3f.
     * @param vec The Vec3 to convert.
     * @return The converted Vec3f.
     */
    Vec3f ConvertToVec3f(const sbio::math::Vec3 vec);

    /**
     * @brief Converts a Mat3 to a Mat3f.
     * @param matrix The Mat3 to convert.
     * @return The converted Mat3f.
     */
    Mat3f ConvertToMat3f(const sbio::math::Mat3 matrix);
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
