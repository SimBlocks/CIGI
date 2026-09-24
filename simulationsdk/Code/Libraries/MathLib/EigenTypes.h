//Copyright SimBlocks LLC 2016-2026
/**
 * @file EigenTypes.h
 * @brief Defines foundational Eigen aliases shared by the math wrappers.
 */
#pragma once
#ifndef SIMBLOCKS_MATH_EIGEN_TYPES_H
#define SIMBLOCKS_MATH_EIGEN_TYPES_H

// Silence Eigen warnings about conditional expressions being constant
#pragma warning(push)
#pragma warning(disable : 4127)
#include "Eigen/Geometry"
#pragma warning(pop)

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
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
