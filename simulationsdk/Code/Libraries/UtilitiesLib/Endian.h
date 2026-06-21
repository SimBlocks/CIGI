//Copyright SimBlocks LLC 2016-2026
/**
 * @file Endian.h
 * @brief Declares byte order (endianness) utilities for primitive types.
 *
 * Defines the CEndian class, which provides static methods to swap the byte order
 * of various integer and floating-point types. Useful for converting between
 * little-endian and big-endian representations.
 */
#pragma once

#ifndef SIMBLOCKS_UTILITIES_ENDIAN_H
#define SIMBLOCKS_UTILITIES_ENDIAN_H

#include <cstdint>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief In-place byte-order conversion helpers for primitive values.
     *
     * All `Swap()` overloads modify the supplied value in place and do not allocate memory.
     * For one-byte types and `bool`, the current implementation is a no-op.
     */
    class CEndian
    {
    public:
      /** @brief Swaps the byte order of an 8-bit signed integer in place. The current implementation leaves the value unchanged. */
      static void Swap(int8_t& n);
      /** @brief Swaps the byte order of an 8-bit unsigned integer in place. The current implementation leaves the value unchanged. */
      static void Swap(uint8_t& n);
      /** @brief Swaps the byte order of a 16-bit signed integer in place. */
      static void Swap(int16_t& n);
      /** @brief Swaps the byte order of a 16-bit unsigned integer in place. */
      static void Swap(uint16_t& n);
      /** @brief Swaps the byte order of a 32-bit signed integer in place. */
      static void Swap(int32_t& n);
      /** @brief Swaps the byte order of a 32-bit unsigned integer in place. */
      static void Swap(uint32_t& n);
      /** @brief Swaps the byte order of a boolean value in place. The current implementation leaves the value unchanged. */
      static void Swap(bool& b);
      /** @brief Swaps the byte order of a 32-bit floating-point value in place. */
      static void Swap(float& f);
      /** @brief Swaps the byte order of a 64-bit floating-point value in place. */
      static void Swap(double& d);
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
