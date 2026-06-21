//Copyright SimBlocks LLC 2016-2026
/**
 * @file StrongVec3.h
 * @brief Strongly-typed 3D vector wrappers for type-safe mathematical operations.
 *
 * The StrongVec3 and StrongVec3f structures provide type-safe wrappers around 3D vectors (Vec3, Vec3f),
 * enabling mathematical operations that preserve the intended type semantics. This is useful for distinguishing
 * between different vector types at compile time, preventing accidental misuse in geospatial and simulation applications.
 *
 * See STRONG_VEC3 and STRONG_VEC3F macros for easy instantiation of strongly-typed 3D vectors.
 */
#pragma once
#ifndef SIMBLOCKS_MATH_STRONG_VEC3_H
#define SIMBLOCKS_MATH_STRONG_VEC3_H

#include "MathLib/MathTypes.h"

namespace sbio
{
  namespace math
  {
    /**
     * @brief Strongly-typed 3D vector wrapper for type-safe mathematical operations.
     *
     * The StrongVec3 template provides a type-safe wrapper around a 3D vector (Vec3),
     * enabling mathematical operations that preserve the intended type semantics.
     * This is useful for distinguishing between different vector types at compile time,
     * preventing accidental misuse.
     * See STRONG_VEC3 macro for easy instantiation.
     * @tparam T The strong type to associate with the vector components.
     */
    struct StrongVec3
    {
    public:
      /**
       * @brief Constructs a StrongVec3 from three double values.
       * @param x The X component.
       * @param y The Y component.
       * @param z The Z component.
       */
      explicit StrongVec3(double x, double y, double z) : m_v(x, y, z)
      {
      }

      /**
       * @brief Constructs a StrongVec3 from an existing sbio::math::Vec3.
       * @param _v The Vec3 to initialize from.
       */
      explicit StrongVec3(const sbio::math::Vec3& _v) : m_v(_v)
      {
      }

      /**
       * @brief Default constructor. Initializes the vector to (0, 0, 0).
       */
      StrongVec3() : m_v(0, 0, 0)
      {
      }

      /**
       * @brief Checks if the vector is the zero vector.
       * @return True if all components are zero, false otherwise.
       */
      bool isZero() const
      {
        return m_v.isZero();
      }

      /**
       * @brief Converts this StrongVec3 to an sbio::math::Vec3.
       * @return The underlying Vec3.
       */
      const sbio::math::Vec3& toVec3() const
      {
        return m_v;
      }

      /**
       * @brief Accesses a component of the vector by index.
       * @param i The component index (0 = X, 1 = Y, 2 = Z).
       * @return Reference to the component.
       */
      double& operator[](int i)
      {
        assert(i >= 0 && i <= 2);
        return m_v[i];
      }

      /**
       * @brief Accesses a component of the vector by index (const).
       * @param i The component index (0 = X, 1 = Y, 2 = Z).
       * @return Value of the component.
       */
      double operator[](int i) const
      {
        assert(i >= 0 && i <= 2);
        return m_v[i];
      }

      /**
       * @brief Sets the components of the vector.
       * @param x The X component.
       * @param y The Y component.
       * @param z The Z component.
       */
      void set(double x, double y, double z)
      {
        m_v[0] = x;
        m_v[1] = y;
        m_v[2] = z;
      }

    public:
      /**
       * @brief The underlying 3D vector.
       *
       * This is the actual vector representation used for mathematical operations.
       */
      sbio::math::Vec3 m_v;
    };

    /**
     * @brief Strongly-typed 3D vector wrapper for float components.
     *
     * The StrongVec3f template provides a type-safe wrapper around a 3D vector (Vec3f),
     * enabling mathematical operations that preserve the intended type semantics.
     * This is useful for distinguishing between different vector types at compile time,
     * preventing accidental misuse.
     * See STRONG_VEC3F macro for easy instantiation.
     */
    struct StrongVec3f
    {
    public:
      /**
       * @brief Constructs a StrongVec3f from three float values.
       * @param x The X component.
       * @param y The Y component.
       * @param z The Z component.
       */
      explicit StrongVec3f(float x, float y, float z) : m_v(x, y, z)
      {
      }

      /**
       * @brief Constructs a StrongVec3f from an existing sbio::math::Vec3f.
       * @param _v The Vec3f to initialize from.
       */
      explicit StrongVec3f(const sbio::math::Vec3f& _v) : m_v(_v)
      {
      }

      /**
       * @brief Default constructor. Initializes the vector to (0, 0, 0).
       */
      StrongVec3f() : m_v(0, 0, 0)
      {
      }

      /**
       * @brief Returns the underlying Vec3f.
       * @return Reference to the internal Vec3f.
       */
      const sbio::math::Vec3f& toVec3() const
      {
        return m_v;
      }

      /**
       * @brief Converts this StrongVec3f to an sbio::math::Vec3.
       * @return The underlying Vec3 as a double vector.
       */
      sbio::math::Vec3 toVec3d() const
      {
        return sbio::math::Vec3(m_v[0], m_v[1], m_v[2]);
      }

      /**
       * @brief Accesses a component of the vector by index.
       * @param i The component index (0 = X, 1 = Y, 2 = Z).
       * @return Reference to the component.
       */
      float& operator[](int i)
      {
        assert(i >= 0 && i <= 2);
        return m_v[i];
      }

      /**
       * @brief Accesses a component of the vector by index (const).
       * @param i The component index (0 = X, 1 = Y, 2 = Z).
       * @return Value of the component.
       */
      float operator[](int i) const
      {
        assert(i >= 0 && i <= 2);
        return m_v[i];
      }

    public:
      /**
       * @brief The underlying 3D vector.
       *
       * This is the actual vector representation used for mathematical operations.
       */
      sbio::math::Vec3f m_v;
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
