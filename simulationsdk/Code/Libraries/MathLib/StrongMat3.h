//Copyright SimBlocks LLC 2016-2026
/**
 * @file StrongMat3.h
 * @brief Strongly-typed 3x3 matrix wrapper for type-safe mathematical operations.
 *
 * The StrongMat3 template provides a type-safe wrapper around a 3x3 matrix (Mat3),
 * enabling mathematical operations that preserve the intended type semantics.
 * This is useful for distinguishing between different matrix types at compile time,
 * preventing accidental misuse in geospatial and simulation applications.
 *
 * @tparam T The strong type to associate with the matrix columns and multiplication results.
 */
#pragma once
#ifndef SIMBLOCKS_STRONG_MAT3_H
#define SIMBLOCKS_STRONG_MAT3_H

#include "MathLib/MathTypes.h"

namespace sbio
{
  namespace math
  {
    /**
     * @brief Strongly-typed 3x3 matrix wrapper for type-safe mathematical operations.
     *
     * The StrongMat3 template provides a type-safe wrapper around a 3x3 matrix (Mat3),
     * enabling mathematical operations that preserve the intended type semantics.
     * This is useful for distinguishing between different matrix types at compile time,
     * preventing accidental misuse.
     *
     * @tparam T The strong type to associate with the matrix columns and multiplication results.
     */
    template <typename T>
    struct StrongMat3
    {
    public:
      /**
       * @brief Constructs a StrongMat3 from an existing Mat3.
       * @param _m The Mat3 to initialize from.
       */
      StrongMat3(const Mat3& _m) : m(_m)
      {
      }

      /**
       * @brief Default constructor. Constructs a StrongMat3 with an identity matrix.
       */
      StrongMat3()
      {
        m.setIdentity();
      }

      /**
       * @brief Returns the underlying Mat3.
       * @return Reference to the internal Mat3.
       */
      const Mat3& toMat3() const
      {
        return m;
      }

      /**
       * @brief Multiplies this StrongMat3 by a Mat3.
       * @param rhs The right-hand side Mat3.
       * @return A new StrongMat3 resulting from the multiplication.
       */
      StrongMat3<T> operator*(const Mat3& rhs) const
      {
        return StrongMat3<T>(m * rhs);
      }

      /**
       * @brief Multiplies this StrongMat3 by another StrongMat3.
       * @param rhs The right-hand side StrongMat3.
       * @return A new StrongMat3 resulting from the multiplication.
       */
      StrongMat3<T> operator*(const StrongMat3<T>& rhs) const
      {
        return StrongMat3<T>(m * rhs.toMat3());
      }

      /**
       * @brief Multiplies this StrongMat3 by an Eigen AngleAxis rotation.
       * @param rhs The right-hand side Eigen AngleAxis rotation.
       * @return A new StrongMat3 resulting from the multiplication.
       */
      StrongMat3<T> operator*(const Eigen::AngleAxisd& rhs) const
      {
        return StrongMat3<T>(m * rhs);
      }

      /**
       * @brief Multiplies this StrongMat3 by a Vec3.
       * @param rhs The right-hand side Vec3.
       * @return A new T resulting from the multiplication.
       */
      T operator*(const Vec3& rhs) const
      {
        return T(m * rhs);
      }

      /**
       * @brief Multiplies this StrongMat3 by another StrongMat3 in-place.
       * @param rhs The right-hand side StrongMat3.
       * @return Reference to this StrongMat3 after multiplication.
       */
      StrongMat3<T>& operator*=(const StrongMat3<T>& rhs)
      {
        m *= rhs.toMat3();
        return *this;
      }

      /**
       * @brief Gets the column vector at index n.
       * @param n The column index (0, 1, or 2).
       * @return A T representing the column vector.
       */
      T getCol(int n) const
      {
        return T(m.col(n));
      }

      /**
       * @brief Sets the column vector at index n.
       * @param n The column index (0, 1, or 2).
       * @param v The vector to set as the column.
       */
      void setCol(int n, T v)
      {
        m(0, n) = v[0];
        m(1, n) = v[1];
        m(2, n) = v[2];
      }

      /**
       * @brief Accesses the matrix element at (r, c).
       * @param r The row index (0, 1, or 2).
       * @param c The column index (0, 1, or 2).
       * @return A reference to the matrix element.
       */
      double& operator()(int r, int c)
      {
        return m(r, c);
      }

      /**
       * @brief Accesses the matrix element at (r, c) as a const.
       * @param r The row index (0, 1, or 2).
       * @param c The column index (0, 1, or 2).
       * @return A const reference to the matrix element.
       */
      double operator()(int r, int c) const
      {
        return m(r, c);
      }

    protected:
      Mat3 m;// The underlying 3x3 matrix.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
