//Copyright SimBlocks LLC 2016-2026
/**
 * @file AABB.h
 * @brief Axis-Aligned Bounding Box (AABB) structure for 3D spatial bounds.
 *
 * Represents a 3D axis-aligned bounding box defined by its minimum and maximum extents
 * along the X, Y, and Z axes. Provides methods for updating the bounds, computing the
 * center and extents, and expanding the box to encapsulate another AABB. Useful for
 * collision detection, spatial queries, and bounding volume calculations in graphics and simulation.
 */
#pragma once
#include <algorithm>
#include <limits>

/**
 * @brief Axis-Aligned Bounding Box (AABB) structure.
 *
 * Represents a 3D axis-aligned bounding box defined by its minimum and maximum extents
 * along the X, Y, and Z axes. Provides methods for updating the bounds, computing the
 * center and extents, and expanding the box to encapsulate another AABB.
 */
struct AABB
{
  // Minimum X coordinate of the bounding box.
  double minX = std::numeric_limits<double>::max();
  //Minimum Y coordinate of the bounding box.
  double minY = std::numeric_limits<double>::max();
  //Minimum Z coordinate of the bounding box.
  double minZ = std::numeric_limits<double>::max();
  //Maximum X coordinate of the bounding box.
  double maxX = std::numeric_limits<double>::lowest();
  //Maximum Y coordinate of the bounding box.
  double maxY = std::numeric_limits<double>::lowest();
  //Maximum Z coordinate of the bounding box.
  double maxZ = std::numeric_limits<double>::lowest();
  //Center X coordinate of the bounding box.
  double centerX = 0;
  //Center Y coordinate of the bounding box.
  double centerY = 0;
  //Center Z coordinate of the bounding box.
  double centerZ = 0;
  //Extent (half-size) along the X axis.
  double extentX = 0;
  //Extent (half-size) along the Y axis.
  double extentY = 0;
  //Extent (half-size) along the Z axis.
  double extentZ = 0;

  /**
   * @brief Determines whether empty.
   * @return `true` when the condition is met; otherwise `false`.
   */
  bool IsEmpty() const;

  /**
   * @brief Updates the bounding box to include the given point.
   * @param x X coordinate of the point.
   * @param y Y coordinate of the point.
   * @param z Z coordinate of the point.
   */
  void UpdateExtremum(double x, double y, double z);

  /**
   * @brief Computes the center and extents of the bounding box.
   */
  void ComputeAABB();

  /**
   * @brief Expands the bounding box to encapsulate another AABB.
   * @param other The AABB to encapsulate.
   */
  void Encapsulate(const AABB& other);
};

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
