//Copyright SimBlocks LLC 2016-2026
#include "TerrestrialSurfaceCondition.h"

using namespace sbio::cigi;
using namespace sbio::cigi::ig;

CTerrestrialSurfaceCondition::CTerrestrialSurfaceCondition()
{
}

CTerrestrialSurfaceCondition::~CTerrestrialSurfaceCondition()
{
}

void CTerrestrialSurfaceCondition::SetActive(bool value)
{
  m_bActive = value;
}

bool CTerrestrialSurfaceCondition::IsActive() const
{
  return m_bActive;
}

void CTerrestrialSurfaceCondition::SetCondition(const SCigiTerrestrialSurfaceCondition& condition)
{
  m_Condition = condition;
}

const SCigiTerrestrialSurfaceCondition& CTerrestrialSurfaceCondition::GetCondition() const
{
  return m_Condition;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
