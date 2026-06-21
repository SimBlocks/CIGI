//Copyright SimBlocks LLC 2016-2026
#include "CigiEntity.h"
#include "AnimationControlHandler.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "IGCigiLib/PacketHandler.h"
#include "IGCigiLib/TerrainHandler.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "UtilitiesLib/Logger.h"
#include "CigiEvent.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::entity;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::entity;
using namespace sbio::ig::animation;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

extern sbio::ig::SComponentData ConvertToComponentData(const uint32_t data[6]);

CCigiEntity::CCigiEntity(EntityID entityID, const SEntityControl& entityControl) : CEntity(entityID), m_ShortEntityTypeID(entityControl.shortEntityTypeID), m_EntityState(entityControl.eState), m_bEnableCollision(entityControl.bCollisionReportingEnabled)
{
  m_EntityType = entityControl.entityType;

  SCreateEntityMessage data;
  data.EntityID = entityControl.entityID;
  data.EntityType = m_EntityType;
  data.ShortEntityTypeID = m_ShortEntityTypeID;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendCreateEntityMessage(data);
  }

  SetEntityState(entityControl.eState, true);

  m_AttachState = entityControl.bHasParent ? EAttachState::ATTACH : EAttachState::DETACH;

  //A value of zero(0) corresponds to fully transparent; a value of 255 corresponds to fully opaque.
  //TODO: handle inherit alpha
  float fAlpha = entityControl.alpha / (float)255;
  SetAlpha(fAlpha);
  SetCollisionDetectionEnabled(entityControl.bCollisionReportingEnabled);
}

void CCigiEntity::Remove()
{
  CEntity::Remove();

  SEntityRemovedEventArgs args;
  args.entityID = m_EntityID;
  IGCIGIEvent cigiEvent;
  Event::Raise<IGCIGIEvent>(args);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    SDestroyEntityMessage data;
    data.EntityID = m_EntityID;
    g_CigiLibGlobals.pEventMessenger->SendDestroyEntityMessage(data);
  }
}

void CCigiEntity::SetAlpha(float alpha)
{
  if (m_fAlpha != alpha)
  {
    m_fAlpha = alpha;

    SSetEntityAlphaMessage data;
    data.Alpha = alpha;
    data.EntityID = m_EntityID;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetEntityAlphaMessage(data);
    }
  }
}

ShortEntityTypeID CCigiEntity::GetShortEntityType() const
{
  return m_ShortEntityTypeID;
}

void CCigiEntity::SetEntityState(sbio::cigi::EActiveState EntityState, bool bForce)
{
  if (bForce || (m_EntityState != EntityState))
  {
    SSetEntityActiveMessage data;
    data.EntityID = m_EntityID;
    data.isActive = EntityState == sbio::cigi::EActiveState::ACTIVE;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetEntityActiveMessage(data);
    }

    m_EntityState = EntityState;
  }
}

void CCigiEntity::Unattach()
{
  CEntity::Unattach();

  // When detaching, snapshot the current world transform as the new top-level transform.
  if (IsTopLevel())
  {
    m_GeodeticPosition = ConvertCigiWorldToGeodeticCoordinates(m_WorldTransform.pos);
    m_Rotation = ConvertCigiWorldRotationToBodyEulerRotation(m_WorldTransform);
  }

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    SSetEntityUnattachedMessage data;
    data.EntityID = m_EntityID;
    g_CigiLibGlobals.pEventMessenger->SendSetEntityUnattachedMessage(data);
  }
}

bool CCigiEntity::SetAttachState(EAttachState AttachState, EntityID parentID, bool inheritAlpha)
{
  if (m_AttachState != AttachState)
  {
    if (AttachState == EAttachState::DETACH)
    {
      Unattach();
    }

    m_AttachState = AttachState;
  }

  if (m_AttachState == EAttachState::ATTACH)
  {
    if (m_ParentID != parentID && m_ParentID != UnknownEntityID)
    {
      std::stringstream ss;
      ss << "Cannot attach child " << GetEntityID().Value() << " to entity " << parentID.Value() << " because child must first be detached from its existing parent " << m_ParentID.Value() << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
      return false;
    }

    CEntity* pEntity = g_CigiLibGlobals.pEntityManager->GetEntity(parentID);
    if (pEntity == nullptr)
    {
      std::stringstream ss;
      ss << "Cannot attach to Entity " << parentID.Value() << " because it does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
      return false;
    }

    AttachToEntity(pEntity);
  }

  return true;
}

void CCigiEntity::AttachToEntity(CEntity* pParent)
{
  EntityID prevEntityID = m_ParentID;
  CCigiEntity* pCigiParent = dynamic_cast<CCigiEntity*>(pParent);

  if (pCigiParent == nullptr)
  {
    return;
  }

  auto parentID = pCigiParent->m_EntityID;
  pCigiParent->m_Children.insert(m_EntityID);
  CEntity::AttachToEntity(pCigiParent);

  if (prevEntityID != parentID)
  {
    SSetEntityAttachedMessage data;
    data.EntityID = m_EntityID;
    data.ParentID = parentID;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetEntityAttachedMessage(data);
    }
  }
}

void CCigiEntity::UpdateAnimationDirection(const SCigiAnimationControl& control, bool bForce)
{
  auto itAnimation = m_Animations.find(control.animationID);
  if (itAnimation == m_Animations.end())
  {
    return;
  }

  CCigiEntityAnimation* pAnim = itAnimation->second.get();

  if (control.fAnimationSpeed > 0)
  {
    if (pAnim->GetAnimationSpeed() <= 0 || bForce)
    {
      pAnim->SetAnimationDirection(true, bForce);
    }
  }
  else if (control.fAnimationSpeed < 0)
  {
    if (pAnim->GetAnimationSpeed() >= 0 || bForce)
    {
      pAnim->SetAnimationDirection(false, bForce);
    }
  }
}

void CCigiEntity::UpdateAnimationSpeed(const SCigiAnimationControl& control, bool bForce)
{
  auto itAnimation = m_Animations.find(control.animationID);
  if (itAnimation == m_Animations.end())
  {
    return;
  }

  CCigiEntityAnimation* pAnim = itAnimation->second.get();
  pAnim->SetAnimationSpeed(control.fAnimationSpeed, bForce);
}

void CCigiEntity::UpdateAnimationState(const SCigiAnimationControl& control, bool bForce)
{
  auto itAnimation = m_Animations.find(control.animationID);
  if (itAnimation == m_Animations.end())
  {
    return;
  }

  CCigiEntityAnimation* pAnim = itAnimation->second.get();
  pAnim->SetAnimationState(control.eAnimationState, bForce);
}

void CCigiEntity::UpdateAnimationLoopMode(const SCigiAnimationControl& control, bool bForce)
{
  auto itAnimation = m_Animations.find(control.animationID);
  if (itAnimation == m_Animations.end())
  {
    return;
  }

  CCigiEntityAnimation* pAnim = itAnimation->second.get();
  pAnim->SetAnimationLoopMode(control.eAnimationLoopMode, bForce);
}

void CCigiEntity::UpdateAnimation(const SCigiAnimationControl& animationControl)
{
  bool bForce = false;
  if (m_Animations.find(animationControl.animationID) == m_Animations.end())
  {
    std::unique_ptr<CCigiEntityAnimation> pAnim = std::make_unique<CCigiEntityAnimation>(animationControl.entityID, animationControl.animationID);
    m_Animations[animationControl.animationID] = std::move(pAnim);

    bForce = true;
  }

  UpdateAnimationDirection(animationControl, bForce);
  UpdateAnimationSpeed(animationControl, bForce);
  UpdateAnimationLoopMode(animationControl, bForce);
  UpdateAnimationState(animationControl, bForce);
}

CCigiEntity::~CCigiEntity()
{
}

TCigiBodyTransform CCigiEntity::GetChildTransform() const
{
  return m_ChildTransform;
}

void CCigiEntity::SetTransformationRateCoordinateSystem(EObjectCoordinateSystem coordinateSystem)
{
  m_TransformationRateCoordinateSystem = coordinateSystem;
}

void CCigiEntity::SetLocalBodyTransformationRate(const SLocalBodyTransformationRate& transformationRate)
{
  m_LocalBodyTransformationRate = transformationRate;
}

void CCigiEntity::SetCigiWorldTransformationRate(const SCigiWorldTransformationRate& transformationRate)
{
  m_CigiWorldTransformationRate = transformationRate;
}

void CCigiEntity::SetLocalBodyAccelerationRate(const SLocalBodyAccelerationRate& accelerationRate)
{
  m_LocalBodyAccelerationRate = accelerationRate;
}

void CCigiEntity::SetCigiWorldAccelerationRate(const SCigiWorldAccelerationRate& accelerationRate)
{
  m_CigiWorldAccelerationRate = accelerationRate;
}

void CCigiEntity::Interpolate(double deltaTime)
{
  if ((m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::WORLD || m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::PARENT) && IsTopLevel())
  {
    m_CigiWorldTransformationRate.linearVelocity += m_CigiWorldAccelerationRate.linearAcceleration * deltaTime;
    m_CigiWorldTransformationRate.angularVelocity.yaw += (m_CigiWorldAccelerationRate.angularAcceleration.yaw * static_cast<float>(deltaTime));
    m_CigiWorldTransformationRate.angularVelocity.roll += (m_CigiWorldAccelerationRate.angularAcceleration.roll * static_cast<float>(deltaTime));
    m_CigiWorldTransformationRate.angularVelocity.pitch += (m_CigiWorldAccelerationRate.angularAcceleration.pitch * static_cast<float>(deltaTime));

    //If the Host sets all rate components to zero, the entity or articulated part will become stationary.
    if (m_CigiWorldTransformationRate.angularVelocity.pitch.IsZero() && m_CigiWorldTransformationRate.angularVelocity.roll.IsZero() && m_CigiWorldTransformationRate.angularVelocity.yaw.IsZero() && m_CigiWorldTransformationRate.linearVelocity.isZero())
    {
      return;
    }

    TCigiBodyEulerRotation angularRotation;
    angularRotation.yaw = m_Rotation.yaw += m_CigiWorldTransformationRate.angularVelocity.yaw * (float)deltaTime;
    angularRotation.pitch = m_Rotation.pitch += m_CigiWorldTransformationRate.angularVelocity.pitch * (float)deltaTime;
    angularRotation.roll = m_Rotation.roll += m_CigiWorldTransformationRate.angularVelocity.roll * (float)deltaTime;

    CigiNEDCoordinates offset = m_CigiWorldTransformationRate.linearVelocity;
    offset[0] *= deltaTime;
    offset[1] *= deltaTime;
    offset[2] *= deltaTime;

    TGeocentricTransform worldTransform = GetWorldTransform();
    worldTransform.pos += ConvertCigiWorldRateOffset(m_GeodeticPosition, offset);
    m_GeodeticPosition = ConvertCigiWorldToGeodeticCoordinates(worldTransform.pos);
    worldTransform.rotation = SetupCigiTopLevelWorldRotation(angularRotation, m_GeodeticPosition.latitude, m_GeodeticPosition.longitude);
    SetTopLevelWorldTransform(worldTransform);
    return;
  }

  m_LocalBodyTransformationRate.linearVelocity += m_LocalBodyAccelerationRate.linearAcceleration * deltaTime;
  m_LocalBodyTransformationRate.angularVelocity.yaw += (m_LocalBodyAccelerationRate.angularAcceleration.yaw * static_cast<float>(deltaTime));
  m_LocalBodyTransformationRate.angularVelocity.roll += (m_LocalBodyAccelerationRate.angularAcceleration.roll * static_cast<float>(deltaTime));
  m_LocalBodyTransformationRate.angularVelocity.pitch += (m_LocalBodyAccelerationRate.angularAcceleration.pitch * static_cast<float>(deltaTime));

  //If the Host sets all rate components to zero, the entity or articulated part will become stationary.
  if (m_LocalBodyTransformationRate.angularVelocity.pitch.IsZero() && m_LocalBodyTransformationRate.angularVelocity.roll.IsZero() && m_LocalBodyTransformationRate.angularVelocity.yaw.IsZero() && m_LocalBodyTransformationRate.linearVelocity.isZero())
  {
    return;
  }

  TCigiBodyEulerRotation angularRotation;
  angularRotation.yaw = m_Rotation.yaw += m_LocalBodyTransformationRate.angularVelocity.yaw * (float)deltaTime;
  angularRotation.pitch = m_Rotation.pitch += m_LocalBodyTransformationRate.angularVelocity.pitch * (float)deltaTime;
  angularRotation.roll = m_Rotation.roll += m_LocalBodyTransformationRate.angularVelocity.roll * (float)deltaTime;

  BodyCoordinates offset = m_LocalBodyTransformationRate.linearVelocity;
  offset[0] *= deltaTime;
  offset[1] *= deltaTime;
  offset[2] *= deltaTime;

  if (m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::WORLD || m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::PARENT)
  {
    if (!IsTopLevel())
    {
      // For child entities in world/parent coordinate system, the offset is applied directly to the child transform since it's already in body coordinates.
      TCigiBodyTransform childTransform = GetChildTransform();
      childTransform.pos += ConvertBodyCoordinatesToCigiBodyCoordinates(offset);
      childTransform.rotation = SetupCigiObjectRotation(angularRotation);
      SetChildTransform(childTransform);
    }
  }
  else if (m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::LOCAL)
  {
    //the rates are defined relative to the entity�s local coordinate system
    if (IsTopLevel())
    {
      // For top-level entities in local coordinate system, the offset is provided in body coordinates but needs to be applied in world coordinates.
      TGeocentricTransform worldTransform = GetWorldTransform();
      const TBodyToWorldRotation bodyToWorldRotation = MakeBodyToWorldRotation(worldTransform.rotation);
      worldTransform.pos += RotateBodyOffsetToWorld(bodyToWorldRotation, offset);
      m_GeodeticPosition = ConvertCigiWorldToGeodeticCoordinates(worldTransform.pos);
      worldTransform.rotation = SetupCigiTopLevelWorldRotation(angularRotation, m_GeodeticPosition.latitude, m_GeodeticPosition.longitude);
      SetTopLevelWorldTransform(worldTransform);
    }
    else
    {
      // For child entities in local coordinate system, the offset is applied directly to the child transform since it's already in body coordinates.
      // The rotation is also applied in local space.
      TCigiBodyTransform cigiBodyTransform = GetChildTransform();
      cigiBodyTransform.rotation = SetupCigiObjectRotation(angularRotation);
      cigiBodyTransform.pos += cigiBodyTransform.rotation * ConvertBodyCoordinatesToCigiBodyCoordinates(offset);
      SetChildTransform(cigiBodyTransform);
    }
  }
}

void CCigiEntity::Update(double deltaTime)
{
  if (m_bInterpolationEnabled)
  {
    CEntity::Update(deltaTime);
    Interpolate(deltaTime);
    SendUpdateMessage();
  }
}

void CCigiEntity::SetChildTransform(TCigiBodyTransform childTransform)
{
  TBodyTransform bodyTransform;
  bodyTransform.pos = ConvertCigiBodyCoordinatesToBodyCoordinates(childTransform.pos);
  bodyTransform.rotation = ConvertCigiBodyRotationToBodyRotation(childTransform.rotation);
  CEntity::SetChildTransform(bodyTransform);
  m_ChildTransform = childTransform;
}

void CCigiEntity::SetTopLevelTransform(const SGeodeticCoordinates& geodeticCoordinates, const TCigiBodyEulerRotation& rotation)
{
  // For top-level entities, the transform is defined by geodetic coordinates and body Euler rotation. Convert these to the world transform format and set it.
  m_GeodeticPosition = geodeticCoordinates;
  m_Rotation = ConvertToBodyEulerRotation(rotation);
  m_WorldTransform = SetupCigiTopLevelWorldTransform(geodeticCoordinates, rotation);
  m_bTransformChanged = true;
}

void CCigiEntity::SetTopLevelWorldTransform(const TGeocentricTransform& worldTransform)
{
  m_WorldTransform = worldTransform;

  // When setting the world transform directly for a top-level entity, also update the cached geodetic position and body Euler rotation
  // because those are used for interpolation and need to stay in sync with the world transform.
  if (IsTopLevel())
  {
    m_GeodeticPosition = ConvertCigiWorldToGeodeticCoordinates(m_WorldTransform.pos);
    m_Rotation = ConvertCigiWorldRotationToBodyEulerRotation(m_WorldTransform);
  }
  m_bTransformChanged = true;
}

void sbio::cigi::ig::CCigiEntity::SendUpdateMessage()
{
  if (m_bTransformChanged)
  {
    if (IsChild())
    {
      SendChildEntityTransformMessage();
    }
    else
    {
      SendTopLevelEntityTransformMessage();
    }

    m_bTransformChanged = false;
  }
}

void sbio::cigi::ig::CCigiEntity::SendTopLevelEntityTransformMessage() const
{
  SUpdateTopLevelEntityTransformMessage data;
  data.EntityID = m_EntityID;
  data.Clamp = (uint16_t)m_eClamp;
  SGeodeticCoordinates geodeticPos = ConvertCigiWorldToGeodeticCoordinates(m_WorldTransform.pos);
  data.Coordinate.latitude = geodeticPos.latitude;
  data.Coordinate.longitude = geodeticPos.longitude;
  data.Coordinate.altitude = m_eClamp == sbio::EClamp::NONE ? geodeticPos.altitude : HeightRelativeToWGS84Ellipsoid(0);
  auto referencePlaneTransform = ConvertGeocentricToReferencePlane(m_WorldTransform.rotation);
  data.Rotation.North = referencePlaneTransform.north;
  data.Rotation.East = referencePlaneTransform.east;
  data.Rotation.Down = referencePlaneTransform.down;
  auto bodyEulerRotation = ConvertCigiWorldRotationToBodyEulerRotation(m_WorldTransform);
  data.EulerRotation.roll = bodyEulerRotation.roll;
  data.EulerRotation.pitch = bodyEulerRotation.pitch;
  data.EulerRotation.yaw = bodyEulerRotation.yaw;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendUpdateTopLevelEntityTransformMessage(data);
  }
}

void sbio::cigi::ig::CCigiEntity::SendChildEntityTransformMessage() const
{
  SUpdateChildEntityTransformMessage data;
  data.EntityID = m_EntityID;
  auto childTransform = ConvertCigiBodyCoordinatesToBodyCoordinates(m_ChildTransform);
  data.Offset = childTransform.pos;
  auto m = childTransform.rotation.toRotationMatrix();
  data.Rotation.Forward = m.getCol(1);
  data.Rotation.Up = m.getCol(2);

  if (g_CigiLibGlobals.pEntityManager != nullptr && g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendUpdateChildEntityTransformMessage(data);
  }
}

void CCigiEntity::SetRenderEnabled(bool bRenderEnabled)
{
  SSetEntityActiveMessage data;
  data.EntityID = m_EntityID;
  data.isActive = bRenderEnabled;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetEntityActiveMessage(data);
  }
}

void CCigiEntity::SetCollisionDetectionEnabled(bool bCollisionDetectionEnabled)
{
  if (m_bEnableCollision != bCollisionDetectionEnabled)
  {
    m_bEnableCollision = bCollisionDetectionEnabled;

    SSetEntityCollisionDetectionEnabledMessage data;
    data.EntityID = m_EntityID;
    data.Enabled = bCollisionDetectionEnabled;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetEntityCollisionDetectionEnabledMessage(data);
    }
  }
}

void CCigiEntity::SetEntityComponent(sbio::cigi::SCigiComponentKey key, sbio::cigi::SCigiComponentControlState state)
{
  auto it = m_EntityComponents.find(key);

  if (it == m_EntityComponents.end())
  {
    m_EntityComponents[key] = state;
  }
  else
  {
    if (it->second == state)
    {
      return;
    }

    m_EntityComponents[key] = state;
  }

  SSetEntityComponentStateMessage data;
  data.EntityID = m_EntityID;
  data.ComponentID = key.componentID;
  data.ComponentState = state.nComponentState;
  data.ComponentData = ConvertToComponentData(state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetEntityComponentStateMessage(data);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
