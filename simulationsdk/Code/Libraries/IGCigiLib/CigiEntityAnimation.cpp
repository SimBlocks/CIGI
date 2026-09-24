//Copyright SimBlocks LLC 2016-2026
#include "CigiEntityAnimation.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::cigi::ig;
using namespace sbio::ig::entity;
using namespace sbio::ig::animation;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiEntityAnimation::CCigiEntityAnimation(sbio::EntityID entityID, sbio::AnimationID animationID) : m_EntityID(entityID), m_AnimationID(animationID)
{
}

CCigiEntityAnimation::~CCigiEntityAnimation()
{
}

sbio::cigi::EAnimationLoopMode CCigiEntityAnimation::GetAnimationLoopMode() const
{
  return m_AnimationLoopMode;
}

float CCigiEntityAnimation::GetAnimationSpeed() const
{
  return m_AnimationSpeed;
}

sbio::cigi::EAnimationState CCigiEntityAnimation::GetAnimationState() const
{
  return m_AnimationState;
}

void CCigiEntityAnimation::SetAnimationDirection(bool bForward, bool bForce)
{
  if (m_bAnimationDirectionForward != bForward || bForce)
  {
    m_bAnimationDirectionForward = bForward;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      SSetAnimationDirectionMessage data;
      data.EntityID = m_EntityID;
      data.AnimationID = m_AnimationID;
      data.isForward = bForward;
      g_CigiLibGlobals.pEventMessenger->SendSetAnimationDirectionMessage(data);
    }
  }
}

void CCigiEntityAnimation::SetAnimationLoopMode(sbio::cigi::EAnimationLoopMode eAnimationLoopMode, bool bForce)
{
  if (m_AnimationLoopMode != eAnimationLoopMode || bForce)
  {
    m_AnimationLoopMode = eAnimationLoopMode;

    if (eAnimationLoopMode == EAnimationLoopMode::CONTINUOUS)
    {
      if (g_CigiLibGlobals.pEventMessenger != nullptr)
      {
        SSetAnimationLoopModeMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        data.Continuous = true;
        g_CigiLibGlobals.pEventMessenger->SendSetAnimationLoopModeMessage(data);
      }
    }
    else
    {
      if (g_CigiLibGlobals.pEventMessenger != nullptr)
      {
        SSetAnimationLoopModeMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        data.Continuous = false;
        g_CigiLibGlobals.pEventMessenger->SendSetAnimationLoopModeMessage(data);
      }
    }
  }
}

void CCigiEntityAnimation::SetAnimationSpeed(float fAnimationSpeed, bool bForce)
{
  if (m_AnimationSpeed != fAnimationSpeed || bForce)
  {
    m_AnimationSpeed = fAnimationSpeed;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      SSetAnimationSpeedMessage data;
      data.Speed = abs(fAnimationSpeed);
      data.EntityID = m_EntityID;
      data.AnimationID = m_AnimationID;

      g_CigiLibGlobals.pEventMessenger->SendSetAnimationSpeedMessage(data);
    }
  }
}

void CCigiEntityAnimation::SetAnimationState(sbio::cigi::EAnimationState eAnimationState, bool bForce)
{
  if (m_AnimationState != eAnimationState || bForce)
  {
    switch (m_AnimationState)
    {
    case (EAnimationState::STOP):
    {
      switch (eAnimationState)
      {
      case (EAnimationState::PLAY):
      case (EAnimationState::CONTINUE):
      {
        SRestartEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendRestartEntityAnimationMessage(data);
        break;
      }
      }
      break;
    }
    case (EAnimationState::PAUSE):
    {
      switch (eAnimationState)
      {
      case (EAnimationState::STOP):
      {
        // stop -- kill all frames
        SStopEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendStopEntityAnimationMessage(data);
        break;
      }
      case (EAnimationState::PLAY):
      {
        SRestartEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendRestartEntityAnimationMessage(data);
        break;
      }
      case (EAnimationState::CONTINUE):
      {
        SPlayEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendPlayEntityAnimationMessage(data);
        break;
      }
      }
      break;
    }
    case (EAnimationState::CONTINUE):
    case (EAnimationState::PLAY):
    {
      switch (eAnimationState)
      {
      case (EAnimationState::STOP):
      {
        // stops animation
        SStopAtCurrentFrameEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendStopAtCurrentFrameEntityAnimationMessage(data);
        break;
      }
      case (EAnimationState::PAUSE):
      {
        // stops animation
        SPauseEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendPauseEntityAnimationMessage(data);
        break;
      }
      case (EAnimationState::PLAY):
      {
        SRestartEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendRestartEntityAnimationMessage(data);
        break;
      }
      }
      break;
    }
    case (EAnimationState::UNKNOWN):
    {
      switch (eAnimationState)
      {
      case (EAnimationState::STOP):
      {
        // stops animation
        SStopAtCurrentFrameEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendStopAtCurrentFrameEntityAnimationMessage(data);
        break;
      }
      case (EAnimationState::PAUSE):
      {
        // stops animation
        SPauseEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendPauseEntityAnimationMessage(data);
        break;
      }
      case (EAnimationState::PLAY):
      case (EAnimationState::CONTINUE):
      {
        SRestartEntityAnimationMessage data;
        data.EntityID = m_EntityID;
        data.AnimationID = m_AnimationID;
        g_CigiLibGlobals.pEventMessenger->SendRestartEntityAnimationMessage(data);
        break;
      }
      }
    }
    }

    m_AnimationState = eAnimationState;
  }
}

void CCigiEntityAnimation::SetAnimationState(sbio::cigi::EAnimationState eAnimationState, sbio::cigi::EAnimationFramePositionReset frameReset, bool bForce)
{
  if (frameReset == EAnimationFramePositionReset::UNKNOWN)
  {
    SetAnimationState(eAnimationState, bForce);
    return;
  }

  if (eAnimationState != EAnimationState::STOP && eAnimationState != EAnimationState::PLAY)
  {
    return;
  }

  const bool restart = eAnimationState == EAnimationState::PLAY && frameReset == EAnimationFramePositionReset::RESET;
  if (g_CigiLibGlobals.pEventMessenger != nullptr && (restart || bForce || m_AnimationState != eAnimationState))
  {
    if (eAnimationState == EAnimationState::STOP)
    {
      SStopAtCurrentFrameEntityAnimationMessage data;
      data.EntityID = m_EntityID;
      data.AnimationID = m_AnimationID;
      g_CigiLibGlobals.pEventMessenger->SendStopAtCurrentFrameEntityAnimationMessage(data);
    }
    else if (restart)
    {
      SRestartEntityAnimationMessage data;
      data.EntityID = m_EntityID;
      data.AnimationID = m_AnimationID;
      g_CigiLibGlobals.pEventMessenger->SendRestartEntityAnimationMessage(data);
    }
    else
    {
      SPlayEntityAnimationMessage data;
      data.EntityID = m_EntityID;
      data.AnimationID = m_AnimationID;
      g_CigiLibGlobals.pEventMessenger->SendPlayEntityAnimationMessage(data);
    }
  }

  m_AnimationState = eAnimationState;
}

void CCigiEntityAnimation::SetAlpha(float alpha, bool inheritAlpha, float entityAlpha)
{
  m_OwnAlpha = alpha;
  m_bInheritAlpha = inheritAlpha;
  m_bAlphaConfigured = true;
  UpdateEffectiveAlpha(entityAlpha);
}

void CCigiEntityAnimation::UpdateEffectiveAlpha(float entityAlpha)
{
  if (!m_bAlphaConfigured)
  {
    return;
  }

  float inheritedAlpha = 1.0f;
  if (m_bInheritAlpha)
  {
    inheritedAlpha = entityAlpha;
  }

  const float alpha = m_OwnAlpha * inheritedAlpha;
  if ((!m_bAlphaInitialized || m_EffectiveAlpha != alpha) && g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    SSetAnimationAlphaMessage data;
    data.EntityID = m_EntityID;
    data.AnimationID = m_AnimationID;
    data.Alpha = alpha;
    g_CigiLibGlobals.pEventMessenger->SendSetAnimationAlphaMessage(data);
    m_EffectiveAlpha = alpha;
    m_bAlphaInitialized = true;
  }
}

//Usage of this software is prohibited except through a Software Licensing Agreement with SimBlocks LLC.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
