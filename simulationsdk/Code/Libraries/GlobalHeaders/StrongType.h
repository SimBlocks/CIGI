//Copyright SimBlocks LLC 2016-2026
/**
 * @file StrongType.h
 * @brief Declares templates for strong typedefs and hash functors.
 *
 * Provides the `StrongType` template for creating type-safe wrappers around arithmetic or
 * identifier-like values, and `StrongTypeHash` for using those wrappers in hash-based containers.
 */
#pragma once
#ifndef SIMBLOCKS_COMMON_STRONG_TYPE_H
#define SIMBLOCKS_COMMON_STRONG_TYPE_H

#include <iostream>
#include <sstream>

/**
 * @brief Stores a single underlying value behind a distinct type.
 *
 * `StrongType<T>` is the base used by the strong-type macros in `StrongTypes.h`. It gives derived
 * types a dedicated type identity while preserving a small set of arithmetic and streaming helpers.
 *
 * Invariants:
 * - `m_nValue` is always a directly stored `T` value.
 * - The template does not enforce domain-specific validation; any additional range or sentinel
 *   rules are the responsibility of derived strong types.
 *
 * Ownership:
 * - This type owns no dynamic resources and does not share ownership with other objects.
 *
 * Side effects:
 * - Constructors initialize `m_nValue`.
 * - `operator/=`, `operator*=`, and stream insertion modify only the destination object they act on.
 *
 * Failure cases:
 * - Arithmetic operators do not guard against overflow, underflow, or division by zero; behavior is
 *   the same as the corresponding operations on `T`.
 * - Stream insertion operators rely on the destination stream and may fail by setting the stream's
 *   error state.
 *
 * @tparam T Underlying stored value type. The template expects `T` to be default-constructible from
 * `0`, copyable, and usable with the arithmetic operations exposed below.
 */
template <typename T>
struct StrongType
{
public:
  /**
   * @brief Initializes the wrapper with an explicit underlying value.
   *
   * Side effects:
   * - Stores `nValue` in `m_nValue`.
   *
   * @param nValue Value to wrap.
   */
  explicit StrongType(T nValue) : m_nValue(nValue)
  {
  }

  /**
   * @brief Initializes the wrapper with the zero value of `T`.
   *
   * Invariants:
   * - After construction, `Value()` returns `0` converted to `T`.
   *
   * Side effects:
   * - Stores `0` in `m_nValue`.
   */
  StrongType() : m_nValue(0)
  {
  }

  /**
   * @brief Adds a raw value to the wrapped value.
   *
   * The result is returned as the underlying type rather than as another `StrongType` instance.
   *
   * @param rhs Value to add to the wrapped value.
   * @return Sum of `m_nValue` and `rhs`.
   */
  T operator+(const T& rhs) const
  {
    return m_nValue + rhs;
  }

  /**
   * @brief Divides the wrapped value in place.
   *
   * Side effects:
   * - Replaces `m_nValue` with `m_nValue / rhs`.
   *
   * Failure cases:
   * - No validation is performed. Division by zero and numeric overflow follow the rules of `T`.
   *
   * @param rhs Divisor applied to the wrapped value.
   */
  void operator/=(const T& rhs)
  {
    m_nValue /= rhs;
  }

  /**
   * @brief Multiplies the wrapped value in place.
   *
   * Side effects:
   * - Replaces `m_nValue` with `m_nValue * rhs`.
   *
   * Failure cases:
   * - No overflow checking is performed.
   *
   * @param rhs Multiplier applied to the wrapped value.
   */
  void operator*=(const T& rhs)
  {
    m_nValue *= rhs;
  }

  /**
   * @brief Returns the stored underlying value.
   *
   * Ownership:
   * - Returns a copy. The caller does not take ownership of internal storage.
   *
   * @return Current wrapped value.
   */
  T Value() const
  {
    return m_nValue;
  }

  /**
   * @brief Writes the wrapped value to an output stream.
   *
   * Side effects:
   * - Appends the formatted numeric value to `stream`.
   *
   * Failure cases:
   * - Any stream formatting or write failure is reflected in `stream`.
   *
   * @param stream Destination stream.
   * @param t Strong value to write.
   * @return Reference to `stream`.
   */
  friend std::ostream& operator<<(std::ostream& stream, StrongType<T> t)
  {
    stream << std::to_string(t.Value());
    return stream;
  }

  /**
   * @brief Writes the wrapped value to a string stream.
   *
   * Side effects:
   * - Appends the formatted numeric value to `stream`.
   *
   * Failure cases:
   * - Any stream formatting or write failure is reflected in `stream`.
   *
   * @param stream Destination string stream.
   * @param t Strong value to write.
   * @return Reference to `stream`.
   */
  friend std::stringstream& operator<<(std::stringstream& stream, StrongType<T> t)
  {
    stream << std::to_string(t.Value());
    return stream;
  }

protected:
  T m_nValue = 0;///< Stored underlying value. No additional validation is enforced by this base template.
};

/**
 * @brief Hash functor for strong types.
 *
 * This functor hashes a strong type by returning its underlying value, making it suitable for strong
 * types whose `Value()` is already a stable hashable integral value.
 *
 * Invariants:
 * - The hash depends only on `t.Value()`.
 *
 * Ownership:
 * - This stateless functor owns no resources.
 *
 * Side effects:
 * - None. Calling the functor reads the supplied strong type and returns a value.
 *
 * Failure cases:
 * - None are reported by the functor itself. Any narrowing or conversion behavior follows the rules
 *   of converting `t.Value()` to `size_t`.
 *
 * @tparam T Strong type that exposes `Value()`.
 */
template <typename T>
struct StrongTypeHash
{
  /**
   * @brief Returns the hash value for a strong type instance.
   *
   * @param t Strong type instance to hash.
   * @return `t.Value()` converted to `size_t`.
   */
  inline size_t operator()(const T& t) const
  {
    return t.Value();
  };
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
