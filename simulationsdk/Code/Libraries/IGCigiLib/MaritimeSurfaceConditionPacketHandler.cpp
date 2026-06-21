//Copyright SimBlocks LLC 2016-2026
#include "MaritimeSurfaceConditionPacketHandler.h"
#include "CigiLib/CigiTypes.h"
#include "EnvironmentalRegionHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"

using namespace sbio::cigi;
using namespace sbio::cigi::ig;

CCigiMaritimeSurfaceConditionPacketHandler::CCigiMaritimeSurfaceConditionPacketHandler(CCigiEnvironmentalRegionHandler* pEnvironmentalRegionHandler) : m_pEnvironmentalRegionHandler(pEnvironmentalRegionHandler)
{
}

void CCigiMaritimeSurfaceConditionPacketHandler::HandleGlobalMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition)
{
  m_pEnvironmentalRegionHandler->SetGlobalMaritimeSurfaceCondition(maritimeSurfaceCondition);
}

void CCigiMaritimeSurfaceConditionPacketHandler::HandleEntityMartiimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition, EntityID entityID)
{
  m_pEnvironmentalRegionHandler->SetEntityMaritimeSurfaceCondition(entityID, maritimeSurfaceCondition);
}

void CCigiMaritimeSurfaceConditionPacketHandler::HandleRegionMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition, RegionID regionID)
{
  m_pEnvironmentalRegionHandler->SetRegionMaritimeSurfaceCondition(regionID, maritimeSurfaceCondition);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
