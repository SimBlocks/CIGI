//Copyright SimBlocks LLC 2016-2026
#include "CigiViewGroup.h"
#include "CigiEntity.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/IGCigiLib.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "UtilitiesLib/Logger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/CigiView.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "CigiEvent.h"
#include "ViewLib/ViewManager.h"
#include <sstream>

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::view;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiViewGroup::CCigiViewGroup(sbio::ViewGroupID viewGroupID) : CViewGroup(viewGroupID)
{
}

CCigiViewGroup::~CCigiViewGroup()
{
}

void CCigiViewGroup::AddViewID(sbio::ViewID viewID)
{
  // Do not accept UnknownViewID as a valid viewID.
  if (viewID == UnknownViewID)
  {
    return;
  }

  CViewGroup::AddViewID(viewID);

  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewID));

  if (!pCigiView)
  {
    return;
  }

  pCigiView->SetViewGroupID(m_ViewGroupID);
}

void CCigiViewGroup::RemoveViewID(sbio::ViewID viewID)
{
  // Do not accept UnknownViewID as a valid viewID. (Things will definitely break)
  if (viewID == UnknownViewID)
  {
    return;
  }

  CViewGroup::RemoveViewID(viewID);

  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewID));

  if (!pCigiView)
  {
    return;
  }

  pCigiView->SetViewGroupID(UnknownViewGroupID);
}

void CCigiViewGroup::SetAttachedEntityID(sbio::EntityID entityID)
{
  // Update the attached entities for the whole group
  for (sbio::ViewID viewID : m_ViewIDs)
  {
    CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewID));
    if (pCigiView)
    {
      pCigiView->SetAttachedEntityID(entityID);
    }
  }

  // Make sure that the CenterViewID is updated, even if it is not part of the group.
  if (m_ViewIDs.find(m_CenterViewID) == m_ViewIDs.end())
  {
    CCigiView* pCenterView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(m_CenterViewID));
    if (pCenterView)
    {
      pCenterView->SetAttachedEntityID(entityID);
    }
  }
}

sbio::EntityID CCigiViewGroup::GetAttachedEntityID()
{
  CCigiView* pCenterView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(m_CenterViewID));

  if (!pCenterView)
  {
    return UnknownEntityID;
  }

  return pCenterView->GetAttachedEntityID();
}

const TCigiBodyEulerRotation& CCigiViewGroup::GetRotation() const
{
  return m_Rotation;
}

sbio::math::TGeocentricTransform sbio::cigi::ig::CCigiViewGroup::GetWorldTransform() const
{
  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(m_CenterViewID));

  if (!pCigiView)
  {
    return sbio::math::TGeocentricTransform();
  }

  CCigiEntity* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(pCigiView->GetAttachedEntityID()));
  if (!pEntity)
  {
    return sbio::math::TGeocentricTransform();
  }
  else
  {
    auto worldTransform = pEntity->GetWorldTransform();
    worldTransform.pos += worldTransform.rotation * ConvertCigiBodyCoordinatesToBodyCoordinates(m_vOffset);
    worldTransform.rotation = worldTransform.rotation * ConvertCigiBodyRotationToBodyRotation(SetupCigiObjectRotation(m_Rotation));
    return worldTransform;
  }
}

TCigiBodyTransform sbio::cigi::ig::CCigiViewGroup::GetChildTransform() const
{
  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(m_CenterViewID));

  if (!pCigiView)
  {
    return TCigiBodyTransform();
  }

  CCigiEntity* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(pCigiView->GetAttachedEntityID()));
  if (!pEntity)
  {
    return TCigiBodyTransform();
  }
  else
  {
    auto childTransform = TCigiBodyTransform();
    childTransform.pos = m_vOffset;
    childTransform.rotation = SetupCigiObjectRotation(m_Rotation);
    return childTransform;
  }
}

void CCigiViewGroup::SetOffset(const CigiBodyCoordinates& vOffset)
{
  m_bTransformationDirty = true;
  m_vOffset = vOffset;
}

void CCigiViewGroup::SetYaw(Degrees fYaw)
{
  m_bTransformationDirty = true;
  m_Rotation.yaw = fYaw;
}

void CCigiViewGroup::SetPitch(Degrees fPitch)
{
  m_bTransformationDirty = true;
  m_Rotation.pitch = fPitch;
}

void CCigiViewGroup::SetRoll(Degrees fRoll)
{
  m_bTransformationDirty = true;
  m_Rotation.roll = fRoll;
}

void CCigiViewGroup::Update()
{
  CViewGroup::Update();

  UpdateTransformation();
}

void CCigiViewGroup::UpdateTransformation()
{
  if (!m_bTransformationDirty)
  {
    return;
  }

  m_bTransformationDirty = false;

  for (auto it : m_ViewIDs)
  {
    CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(it));

    if (!pCigiView)
    {
      continue;
    }

    pCigiView->SetTransformationDirty(true);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
