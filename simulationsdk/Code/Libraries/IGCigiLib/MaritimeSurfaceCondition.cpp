//Copyright SimBlocks LLC 2016-2026
#include "MaritimeSurfaceCondition.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::ocean;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiMaritimeSurfaceCondition::CCigiMaritimeSurfaceCondition()
{
}

CCigiMaritimeSurfaceCondition::~CCigiMaritimeSurfaceCondition()
{
}

void CCigiMaritimeSurfaceCondition::setActive(bool val)
{
  m_bActive = val;
}

bool CCigiMaritimeSurfaceCondition::IsActive() const
{
  return m_bActive;
}

void CCigiMaritimeSurfaceCondition::SetCondition(SCigiMaritimeSurfaceCondition condition)
{
  m_Condition = condition;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    SSetMaritimeSurfaceConditionsMessage setMaritimeSurfaceConditionsMessage;
    setMaritimeSurfaceConditionsMessage.SeaSurfaceHeight = static_cast<float>(m_Condition.fSeaSurfaceHeight.Value());
    setMaritimeSurfaceConditionsMessage.SurfaceClarity = m_Condition.surfaceClarity.Value();

    g_CigiLibGlobals.pEventMessenger->SendSetMaritimeSurfaceConditionsMessage(setMaritimeSurfaceConditionsMessage);
  }
}

SCigiMaritimeSurfaceCondition CCigiMaritimeSurfaceCondition::GetCondition() const
{
  return m_Condition;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
