//Copyright SimBlocks LLC 2016-2026
/**
 * @file StrongTypes.h
 * @brief Declares macros for defining strong types and ranged strong types.
 *
 * Provides preprocessor helpers for generating domain-specific wrapper structs on top of
 * `StrongType<T>`. These macros are used throughout the SDK to prevent accidental mixing of raw
 * numeric values that represent different concepts.
 */
#pragma once
#ifndef SIMBLOCKS_COMMON_STRONG_TYPES_H
#define SIMBLOCKS_COMMON_STRONG_TYPES_H

#include "StrongType.h"
#include <limits>

namespace sbio
{
  namespace math
  {
    /**
     * @brief Returns the result of `fequals`.
     * @param a A value.
     * @param b B value.
     * @return `true` when the values are approximately equal; otherwise `false`.
     */
    extern bool fequals(float a, float b);
    /**
     * @brief Returns the result of `fequals`.
     * @param a A value.
     * @param b B value.
     * @return `true` when the values are approximately equal; otherwise `false`.
     */
    extern bool fequals(double a, double b);
  }
}

/**
 * @brief Defines a three-component strong type derived from an existing vector-like base type.
 *
 * The generated type forwards construction to `BASETYPE`, then adds convenience operators for
 * scaling, addition, accumulation, approximate equality, cross products, normalization, and sign
 * inversion.
 *
 * Generated API:
 * - `TYPE operator*(UNDERLYINGTYPE f) const`
 * - `TYPE operator+(const TYPE& rhs) const`
 * - `TYPE& operator+=(const TYPE& rhs)`
 * - `bool equals(const TYPE& rhs) const`
 * - `TYPE cross(const TYPE& v) const`
 * - `TYPE normalized() const`
 * - `TYPE negated() const`
 *
 * Invariants:
 * - The generated type stores its state in the inherited `BASETYPE` representation.
 * - Approximate equality uses `sbio::math::fequals` on the three stored components.
 *
 * Ownership:
 * - The generated type owns only its inherited value state and introduces no dynamic ownership.
 *
 * Side effects:
 * - `operator+=` modifies the current instance.
 *
 * Failure cases:
 * - No normalization, overflow, or domain validation is performed.
 * - Correctness depends on `BASETYPE` exposing `m_v`, `operator[]`, and `toVec3()` with compatible
 *   semantics.
 * 
 * NOTES: if receiving a compile error: "attempting to reference a deleted function", then initialize the variable in the constructor
 *
 * @param TYPE Name of the generated strong type.
 * @param UNDERLYINGTYPE Scalar type used by the multiplication operator.
 * @param BASETYPE Existing vector-like base type to derive from.
 */
#define STRONG_TYPE3(TYPE, UNDERLYINGTYPE, BASETYPE)                                                                                                                                                                                                                                                       \
  struct TYPE : public BASETYPE                                                                                                                                                                                                                                                                            \
  {                                                                                                                                                                                                                                                                                                        \
    using BASETYPE::BASETYPE;                                                                                                                                                                                                                                                                              \
    TYPE operator*(UNDERLYINGTYPE f) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3() * f);                                                                                                                                                                                                                                                                           \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3() + rhs.toVec3());                                                                                                                                                                                                                                                                \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3() - rhs.toVec3());                                                                                                                                                                                                                                                                \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE& operator+=(const TYPE& rhs)                                                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                                                                      \
      m_v += rhs.toVec3();                                                                                                                                                                                                                                                                                 \
      return *this;                                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    bool equals(const TYPE& rhs) const                                                                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                                                                      \
      return sbio::math::fequals(m_v[0], rhs[0]) && sbio::math::fequals(m_v[1], rhs[1]) && sbio::math::fequals(m_v[2], rhs[2]);                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE cross(const TYPE& v) const                                                                                                                                                                                                                                                                        \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3().cross(v.toVec3()));                                                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE normalized() const                                                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3().normalized());                                                                                                                                                                                                                                                                  \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE negated() const                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(-m_v[0], -m_v[1], -m_v[2]);                                                                                                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                                                                      \
  };

#define STRONG_TYPE_AXES(TYPE, UNDERLYINGTYPE, BASETYPE, AXIS0, AXIS1, AXIS2)                                                                                                                                                                                                                              \
  struct TYPE : public BASETYPE                                                                                                                                                                                                                                                                            \
  {                                                                                                                                                                                                                                                                                                        \
    using BASETYPE::BASETYPE;                                                                                                                                                                                                                                                                              \
    TYPE operator*(UNDERLYINGTYPE f) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3() * f);                                                                                                                                                                                                                                                                           \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3() + rhs.toVec3());                                                                                                                                                                                                                                                                \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3() - rhs.toVec3());                                                                                                                                                                                                                                                                \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE& operator+=(const TYPE& rhs)                                                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                                                                      \
      m_v += rhs.toVec3();                                                                                                                                                                                                                                                                                 \
      return *this;                                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    bool equals(const TYPE& rhs) const                                                                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                                                                      \
      return sbio::math::fequals(m_v[0], rhs[0]) && sbio::math::fequals(m_v[1], rhs[1]) && sbio::math::fequals(m_v[2], rhs[2]);                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE cross(const TYPE& v) const                                                                                                                                                                                                                                                                        \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3().cross(v.toVec3()));                                                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE normalized() const                                                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(toVec3().normalized());                                                                                                                                                                                                                                                                  \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE negated() const                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(-m_v[0], -m_v[1], -m_v[2]);                                                                                                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                                                                      \
    UNDERLYINGTYPE AXIS0() const                                                                                                                                                                                                                                                                           \
    {                                                                                                                                                                                                                                                                                                      \
      return m_v[0];                                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    UNDERLYINGTYPE AXIS1() const                                                                                                                                                                                                                                                                           \
    {                                                                                                                                                                                                                                                                                                      \
      return m_v[1];                                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    UNDERLYINGTYPE AXIS2() const                                                                                                                                                                                                                                                                           \
    {                                                                                                                                                                                                                                                                                                      \
      return m_v[2];                                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    UNDERLYINGTYPE& AXIS0()                                                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                                                                      \
      return m_v[0];                                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    UNDERLYINGTYPE& AXIS1()                                                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                                                                      \
      return m_v[1];                                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    UNDERLYINGTYPE& AXIS2()                                                                                                                                                                                                                                                                                \
    {                                                                                                                                                                                                                                                                                                      \
      return m_v[2];                                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
  };

/**
 * @brief Defines a single-value strong type with sentinel-based unknown handling.
 *
 * The generated type derives from `BASETYPE<T>`, disables default construction, exposes ordering and
 * equality comparisons against the same strong type, supports in-place addition and subtraction,
 * provides increment operators, and declares a global `UnknownTYPE` constant initialized from
 * `UNKNOWN_VALUE`.
 *
 * Generated API:
 * - `TYPE(T nValue)`
 * - `TYPE(const BASETYPE<T>& rhs)`
 * - `static TYPE UnknownValue()`
 * - `bool operator<(const TYPE& rhs) const`
 * - `bool operator==(const TYPE& rhs) const`
 * - `bool operator!=(const TYPE& rhs) const`
 * - `void operator+=(const TYPE& rhs)`
 * - `void operator-=(const TYPE& rhs)`
 * - `void operator++()`
 * - `TYPE operator++(int)`
 *
 * Invariants:
 * - The stored value is exactly the `m_nValue` inherited from `BASETYPE<T>`.
 * - `UnknownValue()` and `UnknownTYPE` always hold `UNKNOWN_VALUE`.
 * - For types without a dedicated out-of-range unknown value, `UNKNOWN_VALUE` is the default value and
 *   is not distinguishable from a valid stored value.
 *
 * Ownership:
 * - Generated values own only their contained scalar state.
 *
 * Side effects:
 * - `operator+=`, `operator-=`, and both increment operators modify the current instance.
 *
 * Failure cases:
 * - No validation is performed for arithmetic overflow or underflow.
 * - Default construction is intentionally disallowed.
 *
 * @param TYPE Name of the generated strong type.
 * @param BASETYPE Base template providing storage and `Value()` access.
 * @param T Underlying scalar type.
 * @param UNKNOWN_VALUE Value used to initialize the generated `UnknownTYPE` constant and to identify
 * the default unknown state.
 */
#define STRONG_SINGLE_VALUE_TYPE(TYPE, BASETYPE, T, UNKNOWN_VALUE)                                                                                                                                                                                                                                         \
  struct TYPE : public BASETYPE<T>                                                                                                                                                                                                                                                                         \
  {                                                                                                                                                                                                                                                                                                        \
    using BASETYPE<T>::BASETYPE;                                                                                                                                                                                                                                                                           \
    TYPE() = delete;                                                                                                                                                                                                                                                                                       \
    explicit TYPE(T nValue) : BASETYPE<T>(nValue)                                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE(const BASETYPE<T>& rhs) : BASETYPE<T>(rhs)                                                                                                                                                                                                                                                        \
    {                                                                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                                                                      \
    static TYPE UnknownValue()                                                                                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(UNKNOWN_VALUE);                                                                                                                                                                                                                                                                          \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator<(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() < rhs.Value();                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator==(const TYPE& rhs) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() == rhs.Value();                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator!=(const TYPE& rhs) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return !(*this == rhs);                                                                                                                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                                                                      \
    void operator+=(const TYPE& rhs)                                                                                                                                                                                                                                                                       \
    {                                                                                                                                                                                                                                                                                                      \
      m_nValue += rhs.Value();                                                                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                                                                      \
    void operator-=(const TYPE& rhs)                                                                                                                                                                                                                                                                       \
    {                                                                                                                                                                                                                                                                                                      \
      m_nValue -= rhs.Value();                                                                                                                                                                                                                                                                             \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(m_nValue - rhs.Value());                                                                                                                                                                                                                                                                 \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(m_nValue + rhs.Value());                                                                                                                                                                                                                                                                 \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator*(const T& rhs) const                                                                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(m_nValue * rhs);                                                                                                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                                                                      \
    void operator++()                                                                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                                                                      \
      ++m_nValue;                                                                                                                                                                                                                                                                                          \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator++(int)                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                      \
      TYPE t(m_nValue);                                                                                                                                                                                                                                                                                    \
      ++m_nValue;                                                                                                                                                                                                                                                                                          \
      return t;                                                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                                      \
  };                                                                                                                                                                                                                                                                                                       \
  const TYPE Unknown##TYPE(TYPE::UnknownValue());

/**
 * @brief Defines a ranged single-value strong type with sentinel-based unknown handling and range helpers.
 *
 * The generated type derives from `BASETYPE<T>`, disables default construction, uses `MINVALUE` as
 * the default value, adds comparison and arithmetic helpers, and provides validation helpers for
 * checking whether a value lies inside `[MINVALUE, MAXVALUE]` while excluding the default value.
 *
 * Generated API:
 * - `TYPE(T nValue)`
 * - `TYPE(const BASETYPE<T>& rhs)`
 * - `static TYPE UnknownValue()`
 * - `bool operator<(const TYPE& rhs) const`
 * - `bool operator<=(const TYPE& rhs) const`
 * - `bool operator>(const TYPE& rhs) const`
 * - `bool operator>=(const TYPE& rhs) const`
 * - `bool operator==(const TYPE& rhs) const`
 * - `bool operator!=(const TYPE& rhs) const`
 * - `TYPE operator*(const T& rhs) const`
 * - `TYPE operator/(const TYPE& rhs) const`
 * - `TYPE operator/(const T& rhs) const`
 * - `TYPE operator+(const TYPE& rhs) const`
 * - `TYPE operator+=(const TYPE& rhs)`
 * - `TYPE operator-() const`
 * - `TYPE operator-(const TYPE& rhs) const`
 * - `TYPE operator-=(const TYPE& rhs)`
 * - `void operator++()`
 * - `TYPE operator++(int)`
 * - `static bool CheckValid(LARGE_TYPE value)`
 * - `bool CheckValid()`
 * - `bool IsZero()`
 *
 * Invariants:
 * - The stored value is exactly the `m_nValue` inherited from `BASETYPE<T>`.
 * - `UnknownValue()` and `UnknownTYPE` always hold `MINVALUE`.
 * - Unknown uses a default value outside of the valid range for the type when available.
 * - When no dedicated out-of-range value exists, the default value is not distinguishable from a
 *   valid stored value.
 *
 * Ownership:
 * - Generated values own only their contained scalar state.
 *
 * Side effects:
 * - `operator+=`, `operator-=`, and both increment operators modify the current instance.
 * - Arithmetic operators that detect the default value return `*this` unchanged.
 *
 * Failure cases:
 * - No overflow, underflow, or divide-by-zero validation is performed.
 * - Default construction is intentionally disallowed.
 *
 * @param TYPE Name of the generated strong type.
 * @param BASETYPE Base template providing storage and `Value()` access.
 * @param T Underlying stored scalar type.
 * @param LARGE_TYPE Wider type used by the static validation helper.
 * @param MINVALUE Inclusive lower bound for numeric validation and value used to initialize the
 * generated `UnknownTYPE` constant.
 * @param MAXVALUE Inclusive maximum valid numeric value.
 */
#define RANGED_STRONG_SINGLE_VALUE_VALIDATOR(TYPE, BASETYPE, T, LARGE_TYPE, MINVALUE, MAXVALUE)                                                                                                                                                                                                            \
  struct TYPE : public BASETYPE<T>                                                                                                                                                                                                                                                                         \
  {                                                                                                                                                                                                                                                                                                        \
    using BASETYPE<T>::BASETYPE;                                                                                                                                                                                                                                                                           \
    TYPE() = delete;                                                                                                                                                                                                                                                                                       \
    explicit TYPE(T nValue) : BASETYPE<T>(nValue)                                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                                                                      \
    }                                                                                                                                                                                                                                                                                                      \
    static TYPE UnknownValue()                                                                                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(MINVALUE);                                                                                                                                                                                                                                                                               \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator<(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() < rhs.Value();                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator<=(const TYPE& rhs) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() <= rhs.Value();                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator>(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() > rhs.Value();                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator>=(const TYPE& rhs) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() >= rhs.Value();                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator==(const TYPE& rhs) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return Value() == rhs.Value();                                                                                                                                                                                                                                                                       \
    }                                                                                                                                                                                                                                                                                                      \
    bool operator!=(const TYPE& rhs) const                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return !(*this == rhs);                                                                                                                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator*(const T& rhs) const                                                                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(Value() * rhs);                                                                                                                                                                                                                                                                          \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator/(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(Value() / rhs.Value());                                                                                                                                                                                                                                                                  \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator/(const T& rhs) const                                                                                                                                                                                                                                                                     \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(Value() / rhs);                                                                                                                                                                                                                                                                          \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(Value() + rhs.Value());                                                                                                                                                                                                                                                                  \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator+=(const TYPE& rhs)                                                                                                                                                                                                                                                                       \
    {                                                                                                                                                                                                                                                                                                      \
      m_nValue += rhs.Value();                                                                                                                                                                                                                                                                             \
      return *this;                                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator-() const                                                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(-m_nValue);                                                                                                                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                                                                                                                                                  \
    {                                                                                                                                                                                                                                                                                                      \
      return TYPE(m_nValue - rhs.Value());                                                                                                                                                                                                                                                                 \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator-=(const TYPE& rhs)                                                                                                                                                                                                                                                                       \
    {                                                                                                                                                                                                                                                                                                      \
      m_nValue -= rhs.Value();                                                                                                                                                                                                                                                                             \
      return *this;                                                                                                                                                                                                                                                                                        \
    }                                                                                                                                                                                                                                                                                                      \
    void operator++()                                                                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                                                                      \
      ++m_nValue;                                                                                                                                                                                                                                                                                          \
    }                                                                                                                                                                                                                                                                                                      \
    TYPE operator++(int)                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                      \
      TYPE t(m_nValue);                                                                                                                                                                                                                                                                                    \
      ++m_nValue;                                                                                                                                                                                                                                                                                          \
      return t;                                                                                                                                                                                                                                                                                            \
    }                                                                                                                                                                                                                                                                                                      \
    static bool CheckValid(LARGE_TYPE value)                                                                                                                                                                                                                                                               \
    {                                                                                                                                                                                                                                                                                                      \
      if ((value < MINVALUE) || (value > MAXVALUE) || (value == MINVALUE))                                                                                                                                                                                                                                 \
      {                                                                                                                                                                                                                                                                                                    \
        return false;                                                                                                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                                                                                    \
      return true;                                                                                                                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                                                                      \
    bool CheckValid()                                                                                                                                                                                                                                                                                      \
    {                                                                                                                                                                                                                                                                                                      \
      if ((m_nValue < MINVALUE) || (m_nValue > MAXVALUE))                                                                                                                                                                                                                                                  \
      {                                                                                                                                                                                                                                                                                                    \
        return false;                                                                                                                                                                                                                                                                                      \
      }                                                                                                                                                                                                                                                                                                    \
      return true;                                                                                                                                                                                                                                                                                         \
    }                                                                                                                                                                                                                                                                                                      \
    bool IsZero()                                                                                                                                                                                                                                                                                          \
    {                                                                                                                                                                                                                                                                                                      \
      return (m_nValue == 0);                                                                                                                                                                                                                                                                              \
    }                                                                                                                                                                                                                                                                                                      \
  };                                                                                                                                                                                                                                                                                                       \
  const TYPE Unknown##TYPE(TYPE::UnknownValue());

/**
 * @brief Convenience macro for a single-value strong type with an unknown sentinel of `0`.
 *
 * The generated type uses `0` as its sentinel unknown value.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying scalar type.
 */
#define STRONG_TYPE(NAME, TYPE) STRONG_SINGLE_VALUE_TYPE(NAME, StrongType, TYPE, 0)
/**
 * @brief Convenience macro for a single-value strong type with a caller-specified unknown sentinel.
 *
 * The generated type uses `UNKNOWN_VALUE` as its sentinel unknown value.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying scalar type.
 * @param UNKNOWN_VALUE Sentinel value used to initialize the generated `UnknownNAME` constant.
 */
#define STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(NAME, TYPE, UNKNOWN_VALUE) STRONG_SINGLE_VALUE_TYPE(NAME, StrongType, TYPE, UNKNOWN_VALUE)
/**
 * @brief Convenience macro for an integer ranged strong type.
 *
 * The generated type uses `MINVALUE` as both the lower validation bound and the unknown sentinel.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying integer type.
 * @param MINVALUE Inclusive minimum valid value and unknown sentinel.
 * @param MAXVALUE Inclusive maximum valid value.
 */
#define RANGED_STRONG_INT(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR(NAME, StrongType, TYPE, TYPE, MINVALUE, MAXVALUE)
/**
 * @brief Convenience macro for a floating-point ranged strong type validated with `float` precision.
 *
 * The generated type uses `MINVALUE` as both the lower validation bound and the unknown sentinel.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying floating-point type.
 * @param MINVALUE Inclusive minimum valid value and unknown sentinel.
 * @param MAXVALUE Inclusive maximum valid value.
 */
#define RANGED_STRONG_FLOAT(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR(NAME, StrongType, TYPE, float, MINVALUE, MAXVALUE)
/**
 * @brief Convenience macro for a floating-point ranged strong type validated with `double` precision.
 *
 * The generated type uses `MINVALUE` as both the lower validation bound and the unknown sentinel.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying floating-point type.
 * @param MINVALUE Inclusive minimum valid value and unknown sentinel.
 * @param MAXVALUE Inclusive maximum valid value.
 */
#define RANGED_STRONG_DOUBLE(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR(NAME, StrongType, TYPE, double, MINVALUE, MAXVALUE)

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
