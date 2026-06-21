//Copyright SimBlocks LLC 2016-2026
/**
 * @file CommonTypes.cpp
 * @brief Implements utility methods for color structures and related types.
 *
 * This file provides the implementation of methods for SColor32 and SColor3f structures,
 * including color component setting, conversion, and comparison operations.
 *
 * @see sbio::SColor32
 * @see sbio::SColor3f
 */
#include "CommonTypes.h"
#include "MathLib/Math.h"

using namespace sbio;
using namespace sbio::math;

// Sets the RGBA components of the color.
void SColor32::Set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
{
  r = _r;
  g = _g;
  b = _b;
  a = _a;
}

// Sets the color from a 32-bit unsigned integer.
void SColor32::Set(uint32_t color)
{
  r = (color >> 24) & 0xFF;
  g = (color >> 16) & 0xFF;
  b = (color >> 8) & 0xFF;
  a = color & 0xFF;
}

void SColor16::Set(uint8_t c1, uint8_t c2)
{
  channel1 = c1;
  channel2 = c2;
}

// Converts the color to a 32-bit unsigned integer.
uint32_t SColor32::To_uint32_t() const
{
  return (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) | (static_cast<uint32_t>(b) << 8) | (static_cast<uint32_t>(a));
}

bool SColor32::operator==(const SColor32& c) const
{
  return r == c.r && g == c.g && b == c.b && a == c.a;
}

bool SColor32::operator<(const SColor32& c) const
{
  return *((uint32_t*)this) < *((uint32_t*)&c);
}

void SColor3f::ScaleBy(SColor3f scale)
{
  r *= scale.r;
  g *= scale.g;
  b *= scale.b;
}

// Sets the RGB components of the color.
void SColor3f::Set(float _r, float _g, float _b)
{
  r = _r;
  g = _g;
  b = _b;
}

// Sets the color from an SColor32 instance, converting to float.
void SColor3f::SetFrom(SColor32 color)
{
  r = color.r / 255.0f;
  g = color.g / 255.0f;
  b = color.b / 255.0f;
}

bool SColor3f::operator==(const SColor3f& c) const
{
  return fequals(r, c.r) && fequals(g, c.g) && fequals(b, c.b);
}

bool SColor3f::operator<(const SColor3f& c) const
{
  if (r < c.r)
  {
    return true;
  }
  else if (r > c.r)
  {
    return false;
  }
  if (g < c.g)
  {
    return true;
  }
  else if (g > c.g)
  {
    return false;
  }
  if (b < c.b)
  {
    return true;
  }
  else if (b > c.b)
  {
    return false;
  }

  return false;
}

SColor4f::SColor4f() : r(0), g(0), b(0), a(0)
{
}

SColor4f::SColor4f(SColor3f color, float alpha) : r(color.r), g(color.g), b(color.b), a(alpha)
{
}

void SColor4f::ScaleBy(SColor4f scale)
{
  r *= scale.r;
  g *= scale.g;
  b *= scale.b;
  a *= scale.a;
}

// Sets the RGB components of the color.
void SColor4f::Set(float _r, float _g, float _b, float _a)
{
  r = _r;
  g = _g;
  b = _b;
  a = _a;
}

// Sets the color from an SColor32 instance, converting to float.
void SColor4f::SetFrom(SColor32 color)
{
  r = color.r / 255.0f;
  g = color.g / 255.0f;
  b = color.b / 255.0f;
  a = color.a / 255.0f;
}

bool SColor4f::operator==(const SColor4f& c) const
{
  return (r == c.r && g == c.g && b == c.b && a == c.a);
}

STextureCoordinateUV::STextureCoordinateUV() : U(0), V(0)
{
}

STextureCoordinateUV::STextureCoordinateUV(int u, int v)
{
  U = static_cast<float>(u);
  V = static_cast<float>(v);
}

STextureCoordinateUV::STextureCoordinateUV(double u, double v) : U(static_cast<float>(u)), V(static_cast<float>(v))
{
}

STextureCoordinateUV::STextureCoordinateUV(float u, float v) : U(u), V(v)
{
}

STextureCoordinateUV STextureCoordinateUV::operator+(const STextureCoordinateUV& rhs) const
{
  return STextureCoordinateUV(U + rhs.U, V + rhs.V);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
