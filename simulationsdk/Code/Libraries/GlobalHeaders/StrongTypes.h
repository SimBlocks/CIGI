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
#include <cstdint>
#include <limits>
#include <type_traits>

namespace sbio
{
  namespace detail
  {
    /**
     * @brief Compares two bounds while preserving signed and unsigned ordering.
     *
     * This helper is used by ranged strong-type validation. When the operand types
     * differ in signedness, it avoids converting a negative signed value to an
     * unsigned value before the comparison.
     *
     * @param lhs Left-hand bound.
     * @param rhs Right-hand bound.
     * @return `true` when `lhs` is less than or equal to `rhs`; otherwise `false`.
     */
    template <typename L, typename R>
    constexpr bool StrongTypeLessEqual(L lhs, R rhs)
    {
      if constexpr (std::is_integral<L>::value && std::is_integral<R>::value && (std::is_signed<L>::value != std::is_signed<R>::value))
      {
        if constexpr (std::is_signed<L>::value)
        {
          return lhs < 0 || static_cast<std::uintmax_t>(lhs) <= static_cast<std::uintmax_t>(rhs);
        }
        else
        {
          return rhs >= 0 && static_cast<std::uintmax_t>(lhs) <= static_cast<std::uintmax_t>(rhs);
        }
      }
      else
      {
        return lhs <= rhs;
      }
    }
  }

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
 * Constructors are inherited from `BASETYPE`. The generated type adds the following operations;
 * only `operator+=` modifies the current instance.
 *
 * Generated API | Parameters | Result
 * --- | --- | ---
 * `TYPE operator*(UNDERLYINGTYPE f) const` | `f`: scalar multiplier. | New type containing `toVec3() * f`.
 * `TYPE operator+(const TYPE& rhs) const` | `rhs`: vector to add. | New type containing the vector sum.
 * `TYPE operator-(const TYPE& rhs) const` | `rhs`: vector to subtract. | New type containing this vector minus `rhs`.
 * `TYPE& operator+=(const TYPE& rhs)` | `rhs`: vector to add in place. | Reference to the modified instance.
 * `bool equals(const TYPE& rhs) const` | `rhs`: vector to compare. | `true` if `sbio::math::fequals` succeeds for all three components; otherwise `false`.
 * `TYPE cross(const TYPE& v) const` | `v`: right operand of the cross product. | New type containing `toVec3().cross(v.toVec3())`.
 * `TYPE normalized() const` | None. | New type containing `toVec3().normalized()`.
 * `TYPE negated() const` | None. | New type with each of the three components negated.
 *
 * `BASETYPE` must expose `m_v`, indexed component access, `toVec3()`, and compatible constructors.
 * Numeric behavior, including normalization of a zero vector, is delegated to the underlying
 * vector implementation. This macro adds no domain or overflow checks.
 *
 * @param TYPE Name of the generated strong type.
 * @param UNDERLYINGTYPE Scalar type used by the multiplication operator.
 * @param BASETYPE Existing vector-like base type to derive from.
 */
#define STRONG_TYPE3(TYPE, UNDERLYINGTYPE, BASETYPE)                                                                                                                               \
  struct TYPE : public BASETYPE                                                                                                                                                    \
  {                                                                                                                                                                                \
    using BASETYPE::BASETYPE;                                                                                                                                                      \
    TYPE operator*(UNDERLYINGTYPE f) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return TYPE(toVec3() * f);                                                                                                                                                   \
    }                                                                                                                                                                              \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(toVec3() + rhs.toVec3());                                                                                                                                        \
    }                                                                                                                                                                              \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(toVec3() - rhs.toVec3());                                                                                                                                        \
    }                                                                                                                                                                              \
    TYPE& operator+=(const TYPE& rhs)                                                                                                                                              \
    {                                                                                                                                                                              \
      m_v += rhs.toVec3();                                                                                                                                                         \
      return *this;                                                                                                                                                                \
    }                                                                                                                                                                              \
    bool equals(const TYPE& rhs) const                                                                                                                                             \
    {                                                                                                                                                                              \
      return sbio::math::fequals(m_v[0], rhs[0]) && sbio::math::fequals(m_v[1], rhs[1]) && sbio::math::fequals(m_v[2], rhs[2]);                                                    \
    }                                                                                                                                                                              \
    TYPE cross(const TYPE& v) const                                                                                                                                                \
    {                                                                                                                                                                              \
      return TYPE(toVec3().cross(v.toVec3()));                                                                                                                                     \
    }                                                                                                                                                                              \
    TYPE normalized() const                                                                                                                                                        \
    {                                                                                                                                                                              \
      return TYPE(toVec3().normalized());                                                                                                                                          \
    }                                                                                                                                                                              \
    TYPE negated() const                                                                                                                                                           \
    {                                                                                                                                                                              \
      return TYPE(-m_v[0], -m_v[1], -m_v[2]);                                                                                                                                      \
    }                                                                                                                                                                              \
  };

/**
 * @brief Defines a three-component strong type with named mutable axis accessors.
 *
 * Construction and vector operations have the parameter and return contracts documented
 * for `STRONG_TYPE3`. The following parameterless accessors are also generated:
 *
 * Generated API | Return value
 * --- | ---
 * `UNDERLYINGTYPE AXIS0() const` | Copy of component zero.
 * `UNDERLYINGTYPE AXIS1() const` | Copy of component one.
 * `UNDERLYINGTYPE AXIS2() const` | Copy of component two.
 * `UNDERLYINGTYPE& AXIS0()` | Mutable reference to component zero.
 * `UNDERLYINGTYPE& AXIS1()` | Mutable reference to component one.
 * `UNDERLYINGTYPE& AXIS2()` | Mutable reference to component two.
 *
 * References refer to the inherited `m_v` storage and must not outlive it.
 * Assigning through them changes the corresponding component without validation.
 *
 * @param TYPE Name of the generated strong type.
 * @param UNDERLYINGTYPE Scalar type used by the arithmetic and axis accessors.
 * @param BASETYPE Existing vector-like base type to derive from.
 * @param AXIS0 Name of the accessor for component zero.
 * @param AXIS1 Name of the accessor for component one.
 * @param AXIS2 Name of the accessor for component two.
 * @see STRONG_TYPE3
 */
#define STRONG_TYPE_AXES(TYPE, UNDERLYINGTYPE, BASETYPE, AXIS0, AXIS1, AXIS2)                                                                                                      \
  struct TYPE : public BASETYPE                                                                                                                                                    \
  {                                                                                                                                                                                \
    using BASETYPE::BASETYPE;                                                                                                                                                      \
    TYPE operator*(UNDERLYINGTYPE f) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return TYPE(toVec3() * f);                                                                                                                                                   \
    }                                                                                                                                                                              \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(toVec3() + rhs.toVec3());                                                                                                                                        \
    }                                                                                                                                                                              \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(toVec3() - rhs.toVec3());                                                                                                                                        \
    }                                                                                                                                                                              \
    TYPE& operator+=(const TYPE& rhs)                                                                                                                                              \
    {                                                                                                                                                                              \
      m_v += rhs.toVec3();                                                                                                                                                         \
      return *this;                                                                                                                                                                \
    }                                                                                                                                                                              \
    bool equals(const TYPE& rhs) const                                                                                                                                             \
    {                                                                                                                                                                              \
      return sbio::math::fequals(m_v[0], rhs[0]) && sbio::math::fequals(m_v[1], rhs[1]) && sbio::math::fequals(m_v[2], rhs[2]);                                                    \
    }                                                                                                                                                                              \
    TYPE cross(const TYPE& v) const                                                                                                                                                \
    {                                                                                                                                                                              \
      return TYPE(toVec3().cross(v.toVec3()));                                                                                                                                     \
    }                                                                                                                                                                              \
    TYPE normalized() const                                                                                                                                                        \
    {                                                                                                                                                                              \
      return TYPE(toVec3().normalized());                                                                                                                                          \
    }                                                                                                                                                                              \
    TYPE negated() const                                                                                                                                                           \
    {                                                                                                                                                                              \
      return TYPE(-m_v[0], -m_v[1], -m_v[2]);                                                                                                                                      \
    }                                                                                                                                                                              \
    UNDERLYINGTYPE AXIS0() const                                                                                                                                                   \
    {                                                                                                                                                                              \
      return m_v[0];                                                                                                                                                               \
    }                                                                                                                                                                              \
    UNDERLYINGTYPE AXIS1() const                                                                                                                                                   \
    {                                                                                                                                                                              \
      return m_v[1];                                                                                                                                                               \
    }                                                                                                                                                                              \
    UNDERLYINGTYPE AXIS2() const                                                                                                                                                   \
    {                                                                                                                                                                              \
      return m_v[2];                                                                                                                                                               \
    }                                                                                                                                                                              \
    UNDERLYINGTYPE& AXIS0()                                                                                                                                                        \
    {                                                                                                                                                                              \
      return m_v[0];                                                                                                                                                               \
    }                                                                                                                                                                              \
    UNDERLYINGTYPE& AXIS1()                                                                                                                                                        \
    {                                                                                                                                                                              \
      return m_v[1];                                                                                                                                                               \
    }                                                                                                                                                                              \
    UNDERLYINGTYPE& AXIS2()                                                                                                                                                        \
    {                                                                                                                                                                              \
      return m_v[2];                                                                                                                                                               \
    }                                                                                                                                                                              \
  };

/**
 * @brief Defines a single-value strong type with sentinel-based unknown handling.
 *
 * Derives from `BASETYPE<T>` and inherits its constructors, except that default construction is
 * deleted. `UnknownTYPE` is a constant in the enclosing namespace initialized by `UnknownValue()`.
 * No range or sentinel validation is performed, and arithmetic treats the sentinel as an ordinary
 * value. Overflow and underflow follow the underlying C++ operations without additional checks.
 *
 * Generated API | Parameters | Result or effect
 * --- | --- | ---
 * `explicit TYPE(T nValue)` | `nValue`: underlying value to store. | Initializes the base with `nValue`.
 * `explicit TYPE(const BASETYPE<T>& rhs)` | `rhs`: base value to copy. | Copy-constructs the base from `rhs`.
 * `static TYPE UnknownValue()` | None. | Returns a new type containing `UNKNOWN_VALUE` converted to `T`.
 * `bool operator<(const TYPE& rhs) const` | `rhs`: value to compare. | Returns `Value() < rhs.Value()`.
 * `bool operator==(const TYPE& rhs) const` | `rhs`: value to compare. | Returns exact underlying equality, with no special sentinel handling.
 * `bool operator!=(const TYPE& rhs) const` | `rhs`: value to compare. | Returns the negation of `operator==`.
 * `void operator+=(const TYPE& rhs)` | `rhs`: value to add. | Adds to this instance; returns nothing.
 * `void operator-=(const TYPE& rhs)` | `rhs`: value to subtract. | Subtracts from this instance; returns nothing.
 * `TYPE operator-(const TYPE& rhs) const` | `rhs`: subtrahend. | Returns a new type containing this value minus `rhs.Value()`.
 * `TYPE operator+(const TYPE& rhs) const` | `rhs`: addend. | Returns a new type containing the sum.
 * `TYPE operator*(const T& rhs) const` | `rhs`: scalar multiplier. | Returns a new type containing the product.
 * `void operator++()` | None. | Increments this instance; returns nothing.
 * `TYPE operator++(int)` | Unnamed argument: postfix discriminator, unused. | Increments this instance and returns its previous value by copy.
 *
 * `Value()`, in-place scaling, and streaming are provided by the base when it supports them.
 *
 * @param TYPE Name of the generated strong type.
 * @param BASETYPE Base template providing storage and `Value()` access.
 * @param T Underlying scalar type.
 * @param UNKNOWN_VALUE Value passed to the constructor by `UnknownValue()`; it is not a default initializer.
 */
#define STRONG_SINGLE_VALUE_TYPE(TYPE, BASETYPE, T, UNKNOWN_VALUE)                                                                                                                 \
  struct TYPE : public BASETYPE<T>                                                                                                                                                 \
  {                                                                                                                                                                                \
    using BASETYPE<T>::BASETYPE;                                                                                                                                                   \
    TYPE() = delete;                                                                                                                                                               \
    explicit TYPE(T nValue) : BASETYPE<T>(nValue)                                                                                                                                  \
    {                                                                                                                                                                              \
    }                                                                                                                                                                              \
    explicit TYPE(const BASETYPE<T>& rhs) : BASETYPE<T>(rhs)                                                                                                                       \
    {                                                                                                                                                                              \
    }                                                                                                                                                                              \
    static TYPE UnknownValue()                                                                                                                                                     \
    {                                                                                                                                                                              \
      return TYPE(UNKNOWN_VALUE);                                                                                                                                                  \
    }                                                                                                                                                                              \
    bool operator<(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return Value() < rhs.Value();                                                                                                                                                \
    }                                                                                                                                                                              \
    bool operator==(const TYPE& rhs) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return Value() == rhs.Value();                                                                                                                                               \
    }                                                                                                                                                                              \
    bool operator!=(const TYPE& rhs) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return !(*this == rhs);                                                                                                                                                      \
    }                                                                                                                                                                              \
    void operator+=(const TYPE& rhs)                                                                                                                                               \
    {                                                                                                                                                                              \
      m_nValue += rhs.Value();                                                                                                                                                     \
    }                                                                                                                                                                              \
    void operator-=(const TYPE& rhs)                                                                                                                                               \
    {                                                                                                                                                                              \
      m_nValue -= rhs.Value();                                                                                                                                                     \
    }                                                                                                                                                                              \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(m_nValue - rhs.Value());                                                                                                                                         \
    }                                                                                                                                                                              \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(m_nValue + rhs.Value());                                                                                                                                         \
    }                                                                                                                                                                              \
    TYPE operator*(const T& rhs) const                                                                                                                                             \
    {                                                                                                                                                                              \
      return TYPE(m_nValue * rhs);                                                                                                                                                 \
    }                                                                                                                                                                              \
    void operator++()                                                                                                                                                              \
    {                                                                                                                                                                              \
      ++m_nValue;                                                                                                                                                                  \
    }                                                                                                                                                                              \
    TYPE operator++(int)                                                                                                                                                           \
    {                                                                                                                                                                              \
      TYPE t(m_nValue);                                                                                                                                                            \
      ++m_nValue;                                                                                                                                                                  \
      return t;                                                                                                                                                                    \
    }                                                                                                                                                                              \
  };                                                                                                                                                                               \
  const TYPE Unknown##TYPE(TYPE::UnknownValue());

/**
 * @brief Defines a ranged single-value strong type with sentinel-based unknown handling and range helpers.
 *
 * Generates the constructors, operators, and validation functions documented by
 * `RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL`, plus `static TYPE UnknownValue()` and a constant
 * `UnknownTYPE` in the enclosing namespace. Construction and arithmetic do not validate the range;
 * callers must invoke `CheckValid()` explicitly. Default construction is deleted.
 *
 * `UnknownValue()` takes no arguments and returns the sentinel selected by
 * `RANGED_STRONG_UNKNOWN_VALUE`: the lowest value of `T` if below `MINVALUE`, otherwise the maximum
 * value of `T` if above `MAXVALUE`, otherwise `MINVALUE`. The fallback is a valid range endpoint,
 * not a distinct unknown value. Arithmetic does not preserve the sentinel.
 *
 * @param TYPE Name of the generated strong type.
 * @param BASETYPE Base template providing storage and `Value()` access.
 * @param T Underlying stored scalar type.
 * @param LARGE_TYPE Default validation input type when it cannot be deduced from the argument.
 * @param MINVALUE Inclusive lower bound for numeric validation.
 * @param MAXVALUE Inclusive maximum valid numeric value.
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL
 * @see RANGED_STRONG_UNKNOWN_VALUE
 */
#define RANGED_STRONG_SINGLE_VALUE_VALIDATOR(TYPE, BASETYPE, T, LARGE_TYPE, MINVALUE, MAXVALUE)                                                                                    \
  RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL(TYPE, BASETYPE, T, LARGE_TYPE, MINVALUE, MAXVALUE, RANGED_STRONG_UNKNOWN_VALUE(TYPE, T, MINVALUE, MAXVALUE))                           \
  const TYPE Unknown##TYPE(TYPE::UnknownValue());

/**
 * @brief Generates `static TYPE UnknownValue()` for a ranged strong type.
 *
 * The factory takes no arguments. It returns `numeric_limits<T>::lowest()` wrapped in `TYPE`
 * if that value is below `MINVALUE`, otherwise `numeric_limits<T>::max()` if above `MAXVALUE`,
 * otherwise `MINVALUE`. The fallback is not distinguishable from a valid range endpoint.
 * This macro does not generate an `UnknownTYPE` constant.
 *
 * @param TYPE Enclosing strong type constructed by the factory.
 * @param T Underlying scalar type with a `std::numeric_limits` specialization.
 * @param MINVALUE Inclusive lower range bound.
 * @param MAXVALUE Inclusive upper range bound.
 */
#define RANGED_STRONG_UNKNOWN_VALUE(TYPE, T, MINVALUE, MAXVALUE)                                                                                                                   \
  static TYPE UnknownValue()                                                                                                                                                       \
  {                                                                                                                                                                                \
    if ((std::numeric_limits<T>::lowest)() < MINVALUE)                                                                                                                             \
    {                                                                                                                                                                              \
      return TYPE((std::numeric_limits<T>::lowest)());                                                                                                                             \
    }                                                                                                                                                                              \
    if ((std::numeric_limits<T>::max)() > MAXVALUE)                                                                                                                                \
    {                                                                                                                                                                              \
      return TYPE((std::numeric_limits<T>::max)());                                                                                                                                \
    }                                                                                                                                                                              \
    return TYPE(MINVALUE);                                                                                                                                                         \
  }

/**
 * @brief Generates scalar storage, arithmetic, comparisons, and explicit inclusive-range checks.
 *
 * Derives from `BASETYPE<T>` and inherits its constructors, except that default construction is
 * deleted. Construction, assignment, and arithmetic do not call `CheckValid()` or clamp values.
 * Arithmetic follows the underlying C++ operations; callers must avoid invalid operations such as
 * integral division by zero or signed overflow. Unknown values receive no special treatment.
 *
 * Generated API | Parameters | Result or effect
 * --- | --- | ---
 * `explicit TYPE(T nValue)` | `nValue`: underlying value to store. | Initializes the base without range validation.
 * `bool operator<(const TYPE& rhs) const` | `rhs`: value to compare. | Returns `Value() < rhs.Value()`.
 * `bool operator<=(const TYPE& rhs) const` | `rhs`: value to compare. | Returns `Value() <= rhs.Value()`.
 * `bool operator>(const TYPE& rhs) const` | `rhs`: value to compare. | Returns `Value() > rhs.Value()`.
 * `bool operator>=(const TYPE& rhs) const` | `rhs`: value to compare. | Returns `Value() >= rhs.Value()`.
 * `bool operator==(const TYPE& rhs) const` | `rhs`: value to compare. | Returns exact underlying equality, not approximate equality.
 * `bool operator!=(const TYPE& rhs) const` | `rhs`: value to compare. | Returns the negation of `operator==`.
 * `TYPE operator*(const T& rhs) const` | `rhs`: scalar multiplier. | Returns a new type containing the product.
 * `TYPE operator/(const TYPE& rhs) const` | `rhs`: wrapped divisor. | Returns a new type containing `Value() / rhs.Value()`.
 * `TYPE operator/(const T& rhs) const` | `rhs`: scalar divisor. | Returns a new type containing `Value() / rhs`.
 * `TYPE operator+(const TYPE& rhs) const` | `rhs`: addend. | Returns a new type containing the sum.
 * `TYPE operator+=(const TYPE& rhs)` | `rhs`: value to add in place. | Modifies this instance and returns its updated value by copy, not reference.
 * `TYPE operator-() const` | None. | Returns a new type containing the negated value converted to `T`.
 * `TYPE operator-(const TYPE& rhs) const` | `rhs`: subtrahend. | Returns a new type containing this value minus `rhs.Value()`.
 * `TYPE operator-=(const TYPE& rhs)` | `rhs`: value to subtract in place. | Modifies this instance and returns its updated value by copy, not reference.
 * `void operator++()` | None. | Increments this instance; returns nothing.
 * `TYPE operator++(int)` | Unnamed argument: postfix discriminator, unused. | Increments this instance and returns its previous value by copy.
 * `template <typename TValue = LARGE_TYPE> static bool CheckValid(TValue value)` | `TValue`: input type, normally deduced; `value`: candidate to test without first converting to
 * `T`. | Returns `true` if `MINVALUE <= value <= MAXVALUE`; otherwise `false`, including for NaN. `bool CheckValid() const` | None. | Returns the static range check applied to the
 * stored value. `bool IsZero() const` | None. | Returns exact equality with zero, regardless of range validity or sentinel status.
 *
 * Mixed signed/unsigned integral range checks use `sbio::detail::StrongTypeLessEqual`.
 * Other comparisons follow the usual C++ conversions. Validation does not modify the value.
 * Base members such as `Value()`, in-place scaling, and streaming remain available when supported.
 *
 * @param TYPE Name of the generated strong type.
 * @param BASETYPE Base template providing `m_nValue` storage and `Value()` access.
 * @param T Underlying scalar type.
 * @param LARGE_TYPE Default `TValue` for static validation when the input type is not deduced.
 * @param MINVALUE Inclusive minimum accepted by `CheckValid()`.
 * @param MAXVALUE Inclusive maximum accepted by `CheckValid()`.
 * @param UNKNOWN_MEMBERS Member declarations inserted after the constructor; may be empty.
 */
#define RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL(TYPE, BASETYPE, T, LARGE_TYPE, MINVALUE, MAXVALUE, UNKNOWN_MEMBERS)                                                              \
  struct TYPE : public BASETYPE<T>                                                                                                                                                 \
  {                                                                                                                                                                                \
    using BASETYPE<T>::BASETYPE;                                                                                                                                                   \
    TYPE() = delete;                                                                                                                                                               \
    explicit TYPE(T nValue) : BASETYPE<T>(nValue)                                                                                                                                  \
    {                                                                                                                                                                              \
    }                                                                                                                                                                              \
    UNKNOWN_MEMBERS                                                                                                                                                                \
    bool operator<(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return Value() < rhs.Value();                                                                                                                                                \
    }                                                                                                                                                                              \
    bool operator<=(const TYPE& rhs) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return Value() <= rhs.Value();                                                                                                                                               \
    }                                                                                                                                                                              \
    bool operator>(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return Value() > rhs.Value();                                                                                                                                                \
    }                                                                                                                                                                              \
    bool operator>=(const TYPE& rhs) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return Value() >= rhs.Value();                                                                                                                                               \
    }                                                                                                                                                                              \
    bool operator==(const TYPE& rhs) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return Value() == rhs.Value();                                                                                                                                               \
    }                                                                                                                                                                              \
    bool operator!=(const TYPE& rhs) const                                                                                                                                         \
    {                                                                                                                                                                              \
      return !(*this == rhs);                                                                                                                                                      \
    }                                                                                                                                                                              \
    TYPE operator*(const T& rhs) const                                                                                                                                             \
    {                                                                                                                                                                              \
      return TYPE(Value() * rhs);                                                                                                                                                  \
    }                                                                                                                                                                              \
    TYPE operator/(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(Value() / rhs.Value());                                                                                                                                          \
    }                                                                                                                                                                              \
    TYPE operator/(const T& rhs) const                                                                                                                                             \
    {                                                                                                                                                                              \
      return TYPE(Value() / rhs);                                                                                                                                                  \
    }                                                                                                                                                                              \
    TYPE operator+(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(Value() + rhs.Value());                                                                                                                                          \
    }                                                                                                                                                                              \
    TYPE operator+=(const TYPE& rhs)                                                                                                                                               \
    {                                                                                                                                                                              \
      m_nValue += rhs.Value();                                                                                                                                                     \
      return *this;                                                                                                                                                                \
    }                                                                                                                                                                              \
    TYPE operator-() const                                                                                                                                                         \
    {                                                                                                                                                                              \
      return TYPE(-m_nValue);                                                                                                                                                      \
    }                                                                                                                                                                              \
    TYPE operator-(const TYPE& rhs) const                                                                                                                                          \
    {                                                                                                                                                                              \
      return TYPE(m_nValue - rhs.Value());                                                                                                                                         \
    }                                                                                                                                                                              \
    TYPE operator-=(const TYPE& rhs)                                                                                                                                               \
    {                                                                                                                                                                              \
      m_nValue -= rhs.Value();                                                                                                                                                     \
      return *this;                                                                                                                                                                \
    }                                                                                                                                                                              \
    void operator++()                                                                                                                                                              \
    {                                                                                                                                                                              \
      ++m_nValue;                                                                                                                                                                  \
    }                                                                                                                                                                              \
    TYPE operator++(int)                                                                                                                                                           \
    {                                                                                                                                                                              \
      TYPE t(m_nValue);                                                                                                                                                            \
      ++m_nValue;                                                                                                                                                                  \
      return t;                                                                                                                                                                    \
    }                                                                                                                                                                              \
    template <typename TValue = LARGE_TYPE>                                                                                                                                        \
    static bool CheckValid(TValue value)                                                                                                                                           \
    {                                                                                                                                                                              \
      /*A valid range is inclusive between MINVALUE and MAXVALUE.*/                                                                                                                \
      return sbio::detail::StrongTypeLessEqual(MINVALUE, value) && sbio::detail::StrongTypeLessEqual(value, MAXVALUE);                                                             \
    }                                                                                                                                                                              \
    bool CheckValid() const                                                                                                                                                        \
    {                                                                                                                                                                              \
      return CheckValid(m_nValue);                                                                                                                                                 \
    }                                                                                                                                                                              \
    bool IsZero() const                                                                                                                                                            \
    {                                                                                                                                                                              \
      return (m_nValue == 0);                                                                                                                                                      \
    }                                                                                                                                                                              \
  };

/**
 * @brief Convenience macro for a single-value strong type with an unknown sentinel of `0`.
 *
 * Default construction is deleted. `UnknownNAME` and `NAME::UnknownValue()` store `0`.
 * See `STRONG_SINGLE_VALUE_TYPE` for constructor, operand, and return-value contracts.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying scalar type.
 * @see STRONG_SINGLE_VALUE_TYPE
 */
#define STRONG_TYPE(NAME, TYPE) STRONG_SINGLE_VALUE_TYPE(NAME, StrongType, TYPE, 0)
/**
 * @brief Convenience macro for a single-value strong type with a caller-specified unknown sentinel.
 *
 * Default construction is deleted. See `STRONG_SINGLE_VALUE_TYPE` for the generated API contracts.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying scalar type.
 * @param UNKNOWN_VALUE Sentinel value used to initialize the generated `UnknownNAME` constant.
 * @see STRONG_SINGLE_VALUE_TYPE
 */
#define STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(NAME, TYPE, UNKNOWN_VALUE) STRONG_SINGLE_VALUE_TYPE(NAME, StrongType, TYPE, UNKNOWN_VALUE)
/**
 * @brief Defines an integer ranged strong type without an unknown value.
 *
 * Neither an `UnknownValue()` factory nor an `UnknownNAME` constant is generated.
 * Default construction is deleted. The range is checked only by explicit calls to `CheckValid()`.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying integer type.
 * @param MINVALUE Inclusive minimum valid value.
 * @param MAXVALUE Inclusive maximum valid value.
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL
 */
#define RANGED_STRONG_INT(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL(NAME, StrongType, TYPE, std::intmax_t, MINVALUE, MAXVALUE, )
/**
 * @brief Defines an integer ranged strong type with a distinct out-of-range unknown value.
 *
 * Compilation fails if the range leaves no representable unknown sentinel.
 * Generates both `UnknownValue()` and the `UnknownNAME` constant.
 * Default construction is deleted. The range is checked only by explicit calls to `CheckValid()`.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying integer type.
 * @param MINVALUE Inclusive minimum valid value.
 * @param MAXVALUE Inclusive maximum valid value.
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL
 */
#define RANGED_STRONG_INT_WITH_UNKNOWN(NAME, TYPE, MINVALUE, MAXVALUE)                                                                                                             \
  static_assert((std::numeric_limits<TYPE>::lowest)() < (MINVALUE) || (std::numeric_limits<TYPE>::max)() > (MAXVALUE),                                                             \
                "RANGED_STRONG_INT_WITH_UNKNOWN requires an out-of-range sentinel; use RANGED_STRONG_INT for a full-range type.");                                                 \
  RANGED_STRONG_SINGLE_VALUE_VALIDATOR(NAME, StrongType, TYPE, std::intmax_t, MINVALUE, MAXVALUE)
/**
 * @brief Defines a floating-point ranged strong type with an unknown sentinel.
 *
 * Generates `UnknownValue()` and `UnknownNAME`, using an out-of-range sentinel when available,
 * otherwise `MINVALUE`. Default construction is deleted. Validation is explicit; its input type
 * is normally deduced from the argument, with `double` as the template default.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying floating-point type.
 * @param MINVALUE Inclusive minimum valid value.
 * @param MAXVALUE Inclusive maximum valid value.
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL
 */
#define RANGED_STRONG_FLOAT(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR(NAME, StrongType, TYPE, double, MINVALUE, MAXVALUE)
/**
 * @brief Defines a floating-point ranged strong type with an unknown sentinel.
 *
 * Equivalent to `RANGED_STRONG_FLOAT`; the stored type is `TYPE`, not necessarily `double`.
 * Default construction is deleted and validation is explicit.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying floating-point type.
 * @param MINVALUE Inclusive minimum valid value.
 * @param MAXVALUE Inclusive maximum valid value.
 * @see RANGED_STRONG_FLOAT
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL
 */
#define RANGED_STRONG_DOUBLE_WITH_UNKNOWN(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR(NAME, StrongType, TYPE, double, MINVALUE, MAXVALUE)

/**
 * @brief Defines a floating-point ranged strong type without an unknown value.
 *
 * Neither an `UnknownValue()` factory nor an `UnknownNAME` constant is generated.
 * Default construction is deleted. Validation is explicit; its input type is normally deduced
 * from the argument, with `double` as the template default.
 *
 * @param NAME Name of the generated strong type.
 * @param TYPE Underlying floating-point type.
 * @param MINVALUE Inclusive minimum valid value.
 * @param MAXVALUE Inclusive maximum valid value.
 * @see RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL
 */
#define RANGED_STRONG_DOUBLE(NAME, TYPE, MINVALUE, MAXVALUE) RANGED_STRONG_SINGLE_VALUE_VALIDATOR_IMPL(NAME, StrongType, TYPE, double, MINVALUE, MAXVALUE, )

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
