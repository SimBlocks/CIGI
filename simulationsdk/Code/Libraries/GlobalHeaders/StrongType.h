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
#include <type_traits>

/**
 * @brief Stores a value for use by domain-specific strong types.
 *
 * `StrongType<T>` is the base used by the strong-type macros in `StrongTypes.h`. It gives derived
 * types a dedicated type identity while preserving a small set of arithmetic and streaming helpers.
 *
 * @tparam T Stored type. Must support initialization from `0` for default construction,
 * copying for value access, and the arithmetic or stream operations used by the caller.
 * @see StrongTypeHash
 */
template <typename T>
struct StrongType
{
public:
  /**
   * @brief Initializes the stored value.
   * @param nValue Value to wrap.
   */
  explicit StrongType(T nValue) : m_nValue(nValue)
  {
  }

  /** @brief Initializes the stored value from `0`. */
  StrongType() : m_nValue(0)
  {
  }

  /**
   * @brief Adds a raw value without modifying this instance.
   * @param rhs Value to add.
   * @return Sum converted to `T`, not a strong-type wrapper.
   */
  T operator+(const T& rhs) const
  {
    return m_nValue + rhs;
  }

  /**
   * @brief Divides the stored value in place without validation.
   * @param rhs Divisor
   */
  void operator/=(const T& rhs)
  {
    m_nValue /= rhs;
  }

  /**
   * @brief Multiplies the stored value in place without overflow checking.
   * @param rhs Multiplier applied to the stored value.
   */
  void operator*=(const T& rhs)
  {
    m_nValue *= rhs;
  }

  /**
   * @brief Reads the underlying value.
   * @return Copy of the stored value.
   */
  T Value() const
  {
    return m_nValue;
  }

  /**
   * @brief Inserts the stored value into an output stream.
   *
   * `char`, `signed char`, and `unsigned char` are inserted as `int`, not characters.
   * Other types use their own insertion operator. Stream formatting is respected;
   * stream errors and exceptions are not intercepted.
   *
   * @param stream Destination stream.
   * @param t Strong value to write.
   * @return Reference to `stream`.
   */
  friend std::ostream& operator<<(std::ostream& stream, StrongType<T> t)
  {
    // For character types, cast to int to avoid printing as a character.
    if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
    {
      stream << static_cast<int>(t.Value());
    }
    else
    {
      stream << t.Value();
    }

    return stream;
  }

  /**
   * @brief Inserts the stored value into a string stream.
   *
   * `char`, `signed char`, and `unsigned char` are inserted as `int`, not characters.
   * Other types use their own insertion operator. Stream formatting is respected;
   * stream errors and exceptions are not intercepted.
   *
   * @param stream Destination string stream.
   * @param t Strong value to write.
   * @return Reference to `stream`.
   */
  friend std::stringstream& operator<<(std::stringstream& stream, StrongType<T> t)
  {
    // For character types, cast to int to avoid printing as a character.
    if constexpr (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
    {
      stream << static_cast<int>(t.Value());
    }
    else
    {
      stream << t.Value();
    }

    return stream;
  }

protected:
  T m_nValue = 0;///< Stored underlying value
};

/**
 * @brief Hash functor for strong types.
 *
 * This functor hashes a strong type by returning its underlying value, making it suitable for strong
 * types whose `Value()` is already a stable hashable integral value.
 *
 * @tparam T Type exposing a const `Value()` member with a result implicitly convertible to `size_t`.
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
