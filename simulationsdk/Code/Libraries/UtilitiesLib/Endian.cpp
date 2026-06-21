//Copyright SimBlocks LLC 2016-2026
/**
 * @file Endian.cpp
 * @brief Implements byte order (endianness) conversion utilities.
 *
 * This file provides the implementation of functions for swapping the byte order
 * of various integer and floating-point types. These utilities are useful for
 * converting between little-endian and big-endian representations.
 *
 * @see sbio::utils::CEndian
 */
#include "Endian.h"
#include <string.h>

using namespace sbio::utils;

#define Swap2Bytes(val) ((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00))

#define Swap4Bytes(val) ((((val) >> 24) & 0x000000FF) | (((val) >> 8) & 0x0000FF00) | (((val) << 8) & 0x00FF0000) | (((val) << 24) & 0xFF000000))

#define Swap8Bytes(val)                                                                                                                                                                                                                                                                                    \
  ((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | (((val) >> 24) & 0x0000000000FF0000) | (((val) >> 8) & 0x00000000FF000000) | (((val) << 8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | (((val) << 40) & 0x00FF000000000000) |                          \
   (((val) << 56) & 0xFF00000000000000))

// Swaps the byte order of an 8-bit signed integer (no-op).
void swap(int8_t& n)
{
}

// Swaps the byte order of an 8-bit unsigned integer (no-op).
void swap(uint8_t& n)
{
}

// Swaps the byte order of a 16-bit signed integer.
void swap(int16_t& n)
{
  n = Swap2Bytes(n);
}

// Swaps the byte order of a 16-bit unsigned integer.
void swap(uint16_t& n)
{
  n = Swap2Bytes(n);
}

// Swaps the byte order of a 32-bit signed integer.
void swap(int32_t& n)
{
  n = Swap4Bytes(n);
}

// Swaps the byte order of a 32-bit unsigned integer.
void swap(uint32_t& n)
{
  n = Swap4Bytes(n);
}

// Swaps the byte order of a boolean value (no-op).
void swap(bool& b)
{
}

// Swaps the byte order of a 32-bit floating point value.
void swap(float& f)
{
  char in[4];
  char out[4];
  memcpy(in, &f, 4);
  out[0] = in[3];
  out[1] = in[2];
  out[2] = in[1];
  out[3] = in[0];
  memcpy(&f, out, 4);
}

// Swaps the byte order of a 64-bit floating point value.
void swap(double& d)
{
  char in[8];
  char out[8];
  memcpy(in, &d, 8);
  out[0] = in[7];
  out[1] = in[6];
  out[2] = in[5];
  out[3] = in[4];
  out[4] = in[3];
  out[5] = in[2];
  out[6] = in[1];
  out[7] = in[0];
  memcpy(&d, out, 8);
}

// Static interface implementations for CEndian
void CEndian::Swap(bool& b)
{
  swap(b);
}

void CEndian::Swap(int8_t& n)
{
  swap(n);
}

void CEndian::Swap(uint8_t& n)
{
  swap(n);
}

void CEndian::Swap(int16_t& n)
{
  swap(n);
}

void CEndian::Swap(uint16_t& n)
{
  swap(n);
}

void CEndian::Swap(int32_t& n)
{
  swap(n);
}

void CEndian::Swap(uint32_t& n)
{
  swap(n);
}

void CEndian::Swap(float& f)
{
  swap(f);
}

void CEndian::Swap(double& d)
{
  swap(d);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
