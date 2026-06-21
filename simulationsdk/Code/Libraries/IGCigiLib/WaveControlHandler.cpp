//Copyright SimBlocks LLC 2016-2026
#include "WaveControlHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EnvironmentalRegionHandler.h"

using namespace sbio::cigi;
using namespace sbio::cigi::ig;

CCigiWaveControlHandler::CCigiWaveControlHandler(CCigiEnvironmentalRegionHandler* pEnvironmentalRegionHandler) : m_pEnvironmentalRegionHandler(pEnvironmentalRegionHandler)
{
}

void CCigiWaveControlHandler::Handle(const SCigiWaveCondition& waveCondition, ECigiScope eScope, int nID)
{
  // apply wave condition on an entity, regional, or global level
  switch (eScope)
  {
  case ECigiScope::ENTITY:
  {
    EntityID entityID = EntityID(static_cast<uint16_t>(nID));
    EntityWaveID entityWaveID = EntityWaveID(waveCondition.waveID);
    m_pEnvironmentalRegionHandler->SetEntityWaveCondition(entityID, entityWaveID, waveCondition);
    break;
  }
  case ECigiScope::REGIONAL:
  {
    RegionID regionID = RegionID(static_cast<uint16_t>(nID));
    RegionalWaveID waveID = RegionalWaveID(waveCondition.waveID);
    m_pEnvironmentalRegionHandler->SetRegionalWaveCondition(regionID, waveID, waveCondition);
    break;
  }
  case ECigiScope::GLOBAL:
  {
    GlobalWaveID waveID = GlobalWaveID(waveCondition.waveID);
    m_pEnvironmentalRegionHandler->SetGlobalWaveCondition(waveID, waveCondition);
  }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
