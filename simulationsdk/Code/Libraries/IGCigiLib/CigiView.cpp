//Copyright SimBlocks LLC 2016-2026
#include "CigiView.h"
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
#include "UtilitiesLib/EventDispatcher.h"
#include "CigiEvent.h"
#include "ViewLib/ViewManager.h"
#include "CigiViewGroup.h"
#include "MathLib/Math.h"
#include <sstream>

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::view;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiView::CCigiView(const sbio::ViewID& viewID) : CView(viewID)
{
  //TODO: this is not set in the views configuration file
  m_eMirrorMode = sbio::EMirrorMode::NONE;

  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->RegisterListener<IGCIGIEvent>(this);
  }
}

CCigiView::~CCigiView()
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->UnregisterListener<IGCIGIEvent>(this);
  }
}

const TCigiBodyEulerRotation& CCigiView::GetRotation() const
{
  return m_Rotation;
}

void CCigiView::OnEntityRemoved(sbio::EntityID entityID)
{
  if (m_AttachedEntityID == entityID)
  {
    SetAttachedEntityID(UnknownEntityID);
  }
}

EntityID CCigiView::GetAttachedEntityID() const
{
  return m_AttachedEntityID;
}

sbio::math::TGeocentricTransform sbio::cigi::ig::CCigiView::GetWorldTransform() const
{
  CCigiEntity* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(m_AttachedEntityID));
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

TCigiBodyTransform sbio::cigi::ig::CCigiView::GetChildTransform() const
{
  CCigiEntity* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(m_AttachedEntityID));
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

void CCigiView::Reset()
{
  CView::Reset();

  m_AttachedEntityID = UnknownEntityID;
}

void CCigiView::SetAttachedEntityID(EntityID entityID)
{
  if (m_AttachedEntityID != entityID)
  {
    m_bTransformationDirty = true;
    m_AttachedEntityID = entityID;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      SSetCameraAttachedToEntityMessage data;
      data.ViewID = m_ViewID;
      data.EntityID = m_AttachedEntityID;

      g_CigiLibGlobals.pEventMessenger->SendSetCameraAttachedToEntityMessage(data);
    }
  }
}

void CCigiView::SetOffset(const CigiBodyCoordinates& vOffset)
{
  if (!m_vOffset.equals(vOffset))
  {
    m_bTransformationDirty = true;
    m_vOffset = vOffset;
  }
}

void CCigiView::SetYaw(Degrees fYaw)
{
  if (!fequals(m_Rotation.yaw.Value(), fYaw.Value()))
  {
    m_bTransformationDirty = true;
    m_Rotation.yaw = fYaw;
  }
}

void CCigiView::SetPitch(Degrees fPitch)
{
  if (!fequals(m_Rotation.pitch.Value(), fPitch.Value()))
  {
    m_bTransformationDirty = true;
    m_Rotation.pitch = fPitch;
  }
}

void CCigiView::SetRoll(Degrees fRoll)
{
  if (!fequals(m_Rotation.roll.Value(), fRoll.Value()))
  {
    m_bTransformationDirty = true;
    m_Rotation.roll = fRoll;
  }
}

void CCigiView::SetTransformationDirty(bool bDirty)
{
  m_bTransformationDirty = bDirty;
}

void sbio::cigi::ig::CCigiView::SetMirrorMode(sbio::EMirrorMode mirrorMode)
{
  m_eMirrorMode = mirrorMode;
}

void CCigiView::SetViewGroupID(sbio::ViewGroupID viewGroupID)
{
  m_ViewGroupID = viewGroupID;
}

sbio::ViewGroupID CCigiView::GetViewGroupID()
{
  return m_ViewGroupID;
}

void CCigiView::Update()
{
  CView::Update();

  UpdateTransformation();
  UpdateProjection();
}

void CCigiView::UpdateTransformation()
{
  if (!m_bTransformationDirty)
  {
    return;
  }

  m_bTransformationDirty = false;

  CCigiEntity* pCigiAttachEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(m_AttachedEntityID));

  //A view is always attached either directly to an entity, or to a view group that is attached to an entity
  if (!pCigiAttachEntity)
  {
    stringstream ss;
    ss << "View attached to unknown entity: " << m_AttachedEntityID.Value() << endl;
    g_CigiLibGlobals.pLogger->LogError(ss);
    return;
  }

  if (m_ViewGroupID == UnknownViewGroupID)
  {
    BodyCoordinates bodyOffset = ConvertCigiBodyCoordinatesToBodyCoordinates(m_vOffset);

    SUpdateAttachedCameraTransformMessage updateCameraTransformMessage;
    updateCameraTransformMessage.ViewID = m_ViewID;
    updateCameraTransformMessage.Offset = bodyOffset;

    Eigen::Matrix3d yawPitchRoll = Eigen::Matrix3d::Identity();
    Eigen::AngleAxisd yawAngle(DegreesToRadians(-m_Rotation.yaw).Value(), yawPitchRoll.col(2));
    yawPitchRoll = yawAngle * yawPitchRoll;
    Eigen::AngleAxisd pitchAngle(DegreesToRadians(m_Rotation.pitch).Value(), yawPitchRoll.col(0));
    yawPitchRoll = pitchAngle * yawPitchRoll;
    Eigen::AngleAxisd rollAngle(DegreesToRadians(m_Rotation.roll).Value(), yawPitchRoll.col(1));
    yawPitchRoll = rollAngle * yawPitchRoll;

    updateCameraTransformMessage.Rotation.Forward[0] = yawPitchRoll.col(1).x();
    updateCameraTransformMessage.Rotation.Forward[1] = yawPitchRoll.col(1).y();
    updateCameraTransformMessage.Rotation.Forward[2] = yawPitchRoll.col(1).z();
    updateCameraTransformMessage.Rotation.Up[0] = yawPitchRoll.col(2).x();
    updateCameraTransformMessage.Rotation.Up[1] = yawPitchRoll.col(2).y();
    updateCameraTransformMessage.Rotation.Up[2] = yawPitchRoll.col(2).z();

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendUpdateAttachedCameraTransformMessage(updateCameraTransformMessage);
    }
  }
  else
  {
    CCigiViewGroup* pViewGroup = dynamic_cast<CCigiViewGroup*>(g_CigiLibGlobals.pViewManager->GetViewGroup(m_ViewGroupID));

    if (!pViewGroup)
    {
      return;
    }

    BodyCoordinates bodyOffset = ConvertCigiBodyCoordinatesToBodyCoordinates(m_vOffset + pViewGroup->GetOffset());

    SUpdateAttachedCameraTransformMessage updateCameraTransformMessage;
    updateCameraTransformMessage.ViewID = m_ViewID;
    updateCameraTransformMessage.Offset = bodyOffset;

    Eigen::Matrix3d yawPitchRoll = Eigen::Matrix3d::Identity();
    Eigen::AngleAxisd yawAngle(DegreesToRadians(-(m_Rotation.yaw + pViewGroup->GetRotation().yaw)).Value(), yawPitchRoll.col(2));
    yawPitchRoll = yawAngle * yawPitchRoll;
    Eigen::AngleAxisd pitchAngle(DegreesToRadians(m_Rotation.pitch + pViewGroup->GetRotation().pitch).Value(), yawPitchRoll.col(0));
    yawPitchRoll = pitchAngle * yawPitchRoll;
    Eigen::AngleAxisd rollAngle(DegreesToRadians(m_Rotation.roll + pViewGroup->GetRotation().roll).Value(), yawPitchRoll.col(1));
    yawPitchRoll = rollAngle * yawPitchRoll;

    updateCameraTransformMessage.Rotation.Forward[0] = yawPitchRoll.col(1).x();
    updateCameraTransformMessage.Rotation.Forward[1] = yawPitchRoll.col(1).y();
    updateCameraTransformMessage.Rotation.Forward[2] = yawPitchRoll.col(1).z();
    updateCameraTransformMessage.Rotation.Up[0] = yawPitchRoll.col(2).x();
    updateCameraTransformMessage.Rotation.Up[1] = yawPitchRoll.col(2).y();
    updateCameraTransformMessage.Rotation.Up[2] = yawPitchRoll.col(2).z();

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendUpdateAttachedCameraTransformMessage(updateCameraTransformMessage);
    }
  }
}

void CCigiView::UpdateProjection()
{
  if (!m_bProjectionDirty)
  {
    return;
  }

  m_bProjectionDirty = false;

  SSetCameraProjectionMessage data;
  data.ViewID = m_ViewID;
  data.Near = m_fNear;
  data.Far = m_fFar;
  data.LeftHalfAngle = m_fLeftHalfAngle;
  data.RightHalfAngle = m_fRightHalfAngle;
  data.TopHalfAngle = m_fTopHalfAngle;
  data.BottomHalfAngle = m_fBottomHalfAngle;
  data.ProjectionMode = m_eProjectionMode;
  data.MirrorMode = m_eMirrorMode;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetCameraProjectionMessage(data);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
