//Copyright SimBlocks LLC 2016-2026
/**
 * @file RegionTree.h
 * @brief Declares the CCigiRegionTree class and supporting types for SimBlocks CIGI IG region spatial queries.
 *
 * Provides the CCigiRegionTree class and supporting structs for managing and querying environmental regions using a BVH tree for efficient spatial queries in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, math, and Eigen BVH types for simulation and region management.
 * Supports region bounding, intersection tests, and efficient region lookup by spatial coordinates.
 *
 * @see sbio::cigi::ig::CCigiRegionTree
 * @see sbio::cigi::ig::SRegionSphereBound
 * @see sbio::cigi::ig::SRegionIntersector
 * @see sbio::cigi::ig::CCigiEnvironmentalRegion
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_REGION_TREE_H
#define SIMBLOCKS_CIGI_REGION_TREE_H

#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiTypeDeclarations.h"
#include "CigiLib/CigiTypes.h"
#include "MathLib/MathTypes.h"
#include <unsupported/Eigen/BVH>
#include <unordered_map>
#include <vector>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Bounding-sphere wrapper used to index one environmental region in the BVH.
       */
      struct SRegionSphereBound
      {
        sbio::math::GeocentricCoordinates center;///< Center of the cached spherical bound.
        double radius;///< Radius of the cached spherical bound.
        CCigiEnvironmentalRegion* region;///< Non-owning pointer to the associated region.

        /**
         * @brief Tests whether a point lies inside the cached spherical bound.
         * @param point Query point in geocentric coordinates.
         * @return `true` when `point` lies within `radius` of `center`; otherwise `false`.
         */
        bool ContainsPoint(sbio::math::GeocentricCoordinates& point);
        /**
         * @brief Returns the axis-aligned bounding box enclosing the spherical bound.
         * @return Bounding box enclosing the cached sphere.
         */
        Eigen::AlignedBox3d GetBoundingBox();
        /**
         * @brief Captures a spherical bound from one region.
         * @param r Non-owning region pointer used to initialize the cached bound.
         */
        SRegionSphereBound(CCigiEnvironmentalRegion* r);
      };

      /**
       * @brief Query helper passed to Eigen BVH traversal.
       */
      struct SRegionIntersector
      {
        sbio::math::GeocentricCoordinates query;///< Query point for the active traversal.
        std::vector<CCigiEnvironmentalRegion*> hits;///< Non-owning region pointers collected during traversal

        /**
         * @brief Calculates initial bounds intersection for BVH.
         */
        bool intersectVolume(const Eigen::AlignedBox3d& b)
        {
          Eigen::AlignedBox3d q(query.toVec3(), query.toVec3());
          return b.contains(q);
        }

        /**
         * @brief Calculates final bound intersection and collects hits.
         */
        bool intersectObject(SRegionSphereBound* region)
        {
          if (region->ContainsPoint(query))
          {
            hits.push_back(region->region);
          }
          return false;
        }
      };

      /**
       * @brief BVH-backed spatial index for environmental regions.
       */
      class CCigiRegionTree
      {
      public:
        /**
         * @brief Constructs a region tree.
         */
        CCigiRegionTree();
        /**
         * @brief Destroys the region tree.
         */
        ~CCigiRegionTree();

        /**
         * @brief Adds a region to the tree.
         * @param regionID Region identifier.
         * @param region Pointer to the region.
         */
        void AddRegion(RegionID regionID, CCigiEnvironmentalRegion* region);
        /**
         * @brief Removes a region from the tree.
         * @param regionID Region identifier.
         */
        void RemoveRegion(RegionID regionID);
        /**
         * @brief Gets a region by its identifier.
         * @param regionID Region identifier.
         * @return Pointer to the region.
         */
        CCigiEnvironmentalRegion* GetRegion(RegionID regionID);
        /**
         * @brief Rebuilds the BVH tree after region updates.
         */
        void Rebuild();
        /**
         * @brief Queries regions that could contain the query point.
         * @param point Geocentric coordinates to query.
         * @return List of regions that may contain the point.
         */
        std::vector<CCigiEnvironmentalRegion*> QueryRegions(sbio::math::GeocentricCoordinates& point);

      private:
        typedef std::unordered_map<RegionID, std::pair<Eigen::AlignedBox3d, SRegionSphereBound*>, StrongTypeHash<RegionID>> TRegionBounds;
        TRegionBounds m_Bounds;///< Owned bounding volumes keyed by region identifier.
        Eigen::KdBVH<double, 3, SRegionSphereBound*> m_Tree;///< BVH built from the cached region bounds.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
