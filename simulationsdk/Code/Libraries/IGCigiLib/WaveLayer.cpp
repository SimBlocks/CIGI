//Copyright SimBlocks LLC 2016-2026
#include "WaveLayer.h"

using namespace sbio::cigi;
using namespace sbio::cigi::ig;

const SCigiWaveCondition& CCigiWaveLayer::GetCondition() const
{
  return m_Condition;
}

void CCigiWaveLayer::SetCondition(const SCigiWaveCondition& condition)
{
  m_Condition = condition;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
