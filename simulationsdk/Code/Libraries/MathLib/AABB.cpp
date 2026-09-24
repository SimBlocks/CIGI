//Copyright SimBlocks LLC 2016-2026
/**
 * @file AABB.cpp
 * @brief Implements the Axis-Aligned Bounding Box (AABB) structure for 3D spatial bounds.
 *
 * This file provides the implementation of the AABB structure, which represents a 3D axis-aligned bounding box
 * defined by its minimum and maximum extents along the X, Y, and Z axes. Includes methods for updating the bounds
 * to include a point, computing the center and extents, and expanding the box to encapsulate another AABB.
 * Useful for collision detection, spatial queries, and bounding volume calculations in graphics and simulation.
 *
 * @see AABB
 */
#include "AABB.h"

#include <cmath>
#include <limits>

using namespace std;

double AABB::Midpoint(double a, double b)
{
  // Avoid overflow when computing the midpoint of two large values.
  constexpr double halfMax = (std::numeric_limits<double>::max)() / 2.0;
  if (std::abs(a) <= halfMax && std::abs(b) <= halfMax)
  {
    return (a + b) / 2.0;
  }

  return a / 2.0 + b / 2.0;
}

bool AABB::IsEmpty() const
{
  return minX > maxX || minY > maxY || minZ > maxZ;
}

/**
 * @brief Updates the bounding box to include the given point.
 * @param x X coordinate of the point.
 * @param y Y coordinate of the point.
 * @param z Z coordinate of the point.
 */
void AABB::UpdateExtremum(double x, double y, double z)
{
  minX = min(minX, x);
  minY = min(minY, y);
  minZ = min(minZ, z);
  maxX = max(maxX, x);
  maxY = max(maxY, y);
  maxZ = max(maxZ, z);
  ComputeAABB();
}

/**
 * @brief Computes the center and extents of the bounding box.
 */
void AABB::ComputeAABB()
{
  if (IsEmpty())
  {
    centerX = 0.0;
    centerY = 0.0;
    centerZ = 0.0;
    extentX = 0.0;
    extentY = 0.0;
    extentZ = 0.0;
    return;
  }

  centerX = Midpoint(minX, maxX);
  centerY = Midpoint(minY, maxY);
  centerZ = Midpoint(minZ, maxZ);
  extentX = Midpoint(maxX, -minX);
  extentY = Midpoint(maxY, -minY);
  extentZ = Midpoint(maxZ, -minZ);
}

/**
 * @brief Expands the bounding box to encapsulate another AABB.
 * @param other The AABB to encapsulate.
 */
void AABB::Encapsulate(const AABB& other)
{
  if (other.IsEmpty())
  {
    return;
  }

  minX = min(other.minX, minX);
  minY = min(other.minY, minY);
  minZ = min(other.minZ, minZ);
  maxX = max(other.maxX, maxX);
  maxY = max(other.maxY, maxY);
  maxZ = max(other.maxZ, maxZ);
  ComputeAABB();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
