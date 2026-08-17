//Copyright SimBlocks LLC 2016-2026
#include "RegionTree.h"
#include "EnvironmentalRegion.h"
#include "EnvironmentalRegionHandler.h"
#include "IGCigiLib/IGCigiLib.h"
#include "UtilitiesLib/Logger.h"
#include <iterator>
#include <sstream>

using namespace std;
using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::math;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

bool SRegionSphereBound::ContainsPoint(GeocentricCoordinates& point)
{
  // find distance between point to center of sphere
  Vec3 difference = center.toVec3() - point.toVec3();
  double distance = difference.norm();

  // if distance is less than radius, inside of sphere, possible intersection, add to list
  return distance < radius;
}

Eigen::AlignedBox3d SRegionSphereBound::GetBoundingBox()
{
  Vec3 r(radius, radius, radius);
  Eigen::AlignedBox3d box(center.toVec3() - r, center.toVec3() + r);
  return box;
}

SRegionSphereBound::SRegionSphereBound(CCigiEnvironmentalRegion* r)
{
  center = r->GetOrigin();
  radius = r->GetRadius();
  region = r;
}

void CCigiRegionTree::AddRegion(RegionID regionID, std::unique_ptr<CCigiEnvironmentalRegion> region)
{
  stringstream ss;
  ss << "Adding region " << regionID << endl;
  g_CigiLibGlobals.pLogger->LogDebug(ss.str());

  // create a new bound for the region, get the bounding box, and store both in the map
  std::unique_ptr<SRegionSphereBound> bound = std::make_unique<SRegionSphereBound>(region.get());
  Eigen::AlignedBox3d boundingBox = bound->GetBoundingBox();
  m_Regions[regionID] = std::move(region);
  m_Bounds[regionID] = std::make_pair(boundingBox, std::move(bound));

  Rebuild();
}

void CCigiRegionTree::RemoveRegion(RegionID regionID)
{
  TRegionBounds::iterator it = m_Bounds.find(regionID);

  // could not find region, nothing to delete
  if (it == m_Bounds.end())
  {
    return;
  }

  // remove region reference and corresponding bounds
  m_Bounds.erase(regionID);
  m_Regions.erase(regionID);

  Rebuild();
}

CCigiEnvironmentalRegion* CCigiRegionTree::GetRegion(RegionID regionID)
{
  TRegionBounds::iterator it = m_Bounds.find(regionID);

  // didn't find region
  if (it == m_Bounds.end())
  {
    return nullptr;
  }

  // return region stored in key, second element of value, region field
  return it->second.second->region;
}

void CCigiRegionTree::Rebuild()
{
  // need two iterators, one over bounds, one over region, pass both to BVH
  std::vector<Eigen::AlignedBox3d> bounds;
  std::vector<SRegionSphereBound*> regions;

  std::transform(m_Bounds.begin(), m_Bounds.end(), std::back_inserter(bounds), [](const auto& keyValuePair) { return keyValuePair.second.first; });
  std::transform(m_Bounds.begin(), m_Bounds.end(), std::back_inserter(regions), [](const auto& keyValuePair) { return keyValuePair.second.second.get(); });

  m_Tree.init(regions.begin(), regions.end(), bounds.begin(), bounds.end());
}

std::vector<CCigiEnvironmentalRegion*> CCigiRegionTree::QueryRegions(GeocentricCoordinates& point)
{
  std::vector<CCigiEnvironmentalRegion*> regions;

  SRegionIntersector intersector;
  intersector.query = point;

  Eigen::BVIntersect(m_Tree, intersector);

  return intersector.hits;
}

CCigiRegionTree::CCigiRegionTree()
{
}

CCigiRegionTree::~CCigiRegionTree()
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
