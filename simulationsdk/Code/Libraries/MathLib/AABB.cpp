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

using namespace std;

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

  centerX = (minX + maxX) / 2.0;
  centerY = (minY + maxY) / 2.0;
  centerZ = (minZ + maxZ) / 2.0;
  extentX = std::abs(maxX - minX) / 2.0;
  extentY = std::abs(maxY - minY) / 2.0;
  extentZ = std::abs(maxZ - minZ) / 2.0;
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
