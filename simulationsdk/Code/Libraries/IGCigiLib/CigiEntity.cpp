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

CCigiEntity::CCigiEntity(EntityID entityID, const SEntityControl& entityControl) :
  CEntity(entityID), m_ShortEntityTypeID(entityControl.shortEntityTypeID), m_EntityState(entityControl.eState)
{
  SetInterpolationEnabled(entityControl.bSmoothingEnabled);

  // Seed the opposite collision state so the constructor's first update always notifies the engine.
  m_bEnableCollision = !entityControl.bCollisionReportingEnabled;

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

  // A value of zero(0) corresponds to fully transparent; a value of 255 corresponds to fully opaque.
  float fAlpha = entityControl.alpha / (float)255;
  SetAlpha(fAlpha, entityControl.bInheritAlpha);
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
  m_OwnAlpha = alpha;
  UpdateEffectiveAlpha();
}

void CCigiEntity::SetAlpha(float alpha, bool inheritAlpha)
{
  m_bInheritAlpha = inheritAlpha;
  SetAlpha(alpha);
}

bool CCigiEntity::GetInheritAlpha() const
{
  return m_bInheritAlpha;
}

void CCigiEntity::UpdateEffectiveAlpha()
{
  float alpha = m_OwnAlpha;
  auto* pParent = dynamic_cast<CCigiEntity*>(m_pParent);

  if (m_bInheritAlpha && pParent != nullptr)
  {
    alpha = pParent->m_fAlpha;
  }

  if (!m_bAlphaInitialized || m_fAlpha != alpha)
  {
    m_bAlphaInitialized = true;
    m_fAlpha = alpha;

    SSetEntityAlphaMessage data;
    data.Alpha = alpha;
    data.EntityID = m_EntityID;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetEntityAlphaMessage(data);
    }

    UpdateAnimationAlpha();

    for (EntityID childID : m_Children)
    {
      auto* pChild = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(childID));
      if (pChild != nullptr && pChild->m_pParent == this && pChild->m_bInheritAlpha)
      {
        pChild->UpdateEffectiveAlpha();
      }
    }
  }
}

ShortEntityTypeID CCigiEntity::GetShortEntityType() const
{
  return m_ShortEntityTypeID;
}

void CCigiEntity::UpdateAnimationAlpha()
{
  for (auto& animation : m_Animations)
  {
    animation.second->UpdateEffectiveAlpha(m_fAlpha);
  }
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

  UpdateEffectiveAlpha();
}

bool CCigiEntity::SetAttachState(EAttachState AttachState, EntityID parentID, bool inheritAlpha)
{
  if (AttachState == EAttachState::ATTACH)
  {
    // Attempt to retrieve the parent entity from the entity manager.
    CEntity* pEntity = g_CigiLibGlobals.pEntityManager->GetEntity(parentID);

    // If the specified parent entity does not exist, log an error and return false.
    if (pEntity == nullptr)
    {
      std::stringstream ss;
      ss << "Cannot attach to Entity " << parentID.Value() << " because it does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
      return false;
    }

    // Attempt to attach to the specified parent entity.
    AttachToEntity(pEntity);

    // If the attach failed, return false.
    if (m_pParent != pEntity)
    {
      return false;
    }
  }
  else if (m_AttachState != AttachState && AttachState == EAttachState::DETACH)
  {
    // If the entity is currently attached, detach it.
    Unattach();
  }

  // Update the attach state after a successful attach or detach operation.
  m_AttachState = AttachState;
  m_bInheritAlpha = inheritAlpha;
  UpdateEffectiveAlpha();
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
  CEntity::AttachToEntity(pCigiParent);
  if (m_pParent != pCigiParent)
  {
    return;
  }

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

  UpdateEffectiveAlpha();
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
  pAnim->SetAnimationState(control.eAnimationState, control.eAnimationFramePositionReset, bForce);
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
  // Legacy CIGI 3 entity control has neither animation alpha nor a frame-reset field.
  if (animationControl.eAnimationFramePositionReset != EAnimationFramePositionReset::UNKNOWN)
  {
    m_Animations.at(animationControl.animationID)->SetAlpha(animationControl.alpha.Value(), animationControl.bInheritAlpha, m_fAlpha);
  }

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

void CCigiEntity::SetAccelerationRateCoordinateSystem(EObjectCoordinateSystem coordinateSystem)
{
  m_AccelerationRateCoordinateSystem = coordinateSystem;
  if (m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::UNKNOWN)
  {
    m_TransformationRateCoordinateSystem = coordinateSystem;
  }
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

SLocalBodyAccelerationRate CCigiEntity::GetAccelerationInVelocityCoordinates() const
{
  SLocalBodyAccelerationRate acceleration = m_LocalBodyAccelerationRate;
  if (IsTopLevel() && (m_AccelerationRateCoordinateSystem == EObjectCoordinateSystem::WORLD || m_AccelerationRateCoordinateSystem == EObjectCoordinateSystem::PARENT))
  {
    const auto& linear = m_CigiWorldAccelerationRate.linearAcceleration;
    acceleration.linearAcceleration = ConvertCigiBodyCoordinatesToBodyCoordinates(CigiBodyCoordinates(linear[0], linear[1], linear[2]));
    acceleration.angularAcceleration = m_CigiWorldAccelerationRate.angularAcceleration;
  }

  const bool accelerationIsLocal = m_AccelerationRateCoordinateSystem == EObjectCoordinateSystem::LOCAL;
  const bool velocityIsLocal = m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::LOCAL;
  if (m_AccelerationRateCoordinateSystem != EObjectCoordinateSystem::UNKNOWN && accelerationIsLocal != velocityIsLocal)
  {
    // The entity attitude maps local axes to the parent axes (or NED for top-level entities).
    TBodyRotation rotation = ConvertCigiBodyRotationToBodyRotation(SetupCigiObjectRotation(ConvertToCigiBodyEulerRotation(m_Rotation)));
    if (velocityIsLocal)
    {
      rotation = rotation.inverse();
    }

    acceleration.linearAcceleration = BodyCoordinates(rotation * acceleration.linearAcceleration.toVec3());
    const auto& angular = acceleration.angularAcceleration;
    const CigiBodyCoordinates angularAxes(angular.roll.Value(), angular.pitch.Value(), angular.yaw.Value());
    const auto rotatedAngular = ConvertBodyCoordinatesToCigiBodyCoordinates(BodyCoordinates(rotation * ConvertCigiBodyCoordinatesToBodyCoordinates(angularAxes).toVec3()));
    acceleration.angularAcceleration.roll = DegreesPerSecondSquared(rotatedAngular[0]);
    acceleration.angularAcceleration.pitch = DegreesPerSecondSquared(rotatedAngular[1]);
    acceleration.angularAcceleration.yaw = DegreesPerSecondSquared(rotatedAngular[2]);
  }

  return acceleration;
}

void CCigiEntity::Interpolate(double deltaTime)
{
  const auto acceleration = GetAccelerationInVelocityCoordinates();
  if ((m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::WORLD || m_TransformationRateCoordinateSystem == EObjectCoordinateSystem::PARENT) && IsTopLevel())
  {
    const auto linear = ConvertBodyCoordinatesToCigiBodyCoordinates(acceleration.linearAcceleration);
    m_CigiWorldTransformationRate.linearVelocity += CigiNEDCoordinates(linear[0], linear[1], linear[2]) * deltaTime;
    m_CigiWorldTransformationRate.angularVelocity.yaw += DegreesPerSecond(acceleration.angularAcceleration.yaw.Value() * deltaTime);
    m_CigiWorldTransformationRate.angularVelocity.roll += DegreesPerSecond(acceleration.angularAcceleration.roll.Value() * deltaTime);
    m_CigiWorldTransformationRate.angularVelocity.pitch += DegreesPerSecond(acceleration.angularAcceleration.pitch.Value() * deltaTime);

    // If the Host sets all rate components to zero, the entity or articulated part will become stationary.
    if (m_CigiWorldTransformationRate.angularVelocity.pitch.IsZero() && m_CigiWorldTransformationRate.angularVelocity.roll.IsZero() &&
        m_CigiWorldTransformationRate.angularVelocity.yaw.IsZero() && m_CigiWorldTransformationRate.linearVelocity.isZero())
    {
      return;
    }

    TCigiBodyEulerRotation angularRotation;
    angularRotation.yaw = m_Rotation.yaw += Degrees(m_CigiWorldTransformationRate.angularVelocity.yaw.Value() * deltaTime);
    angularRotation.pitch = m_Rotation.pitch += Degrees90(m_CigiWorldTransformationRate.angularVelocity.pitch.Value() * deltaTime);
    angularRotation.roll = m_Rotation.roll += Degrees180(m_CigiWorldTransformationRate.angularVelocity.roll.Value() * deltaTime);

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

  m_LocalBodyTransformationRate.linearVelocity += acceleration.linearAcceleration * deltaTime;
  m_LocalBodyTransformationRate.angularVelocity.yaw += DegreesPerSecond(acceleration.angularAcceleration.yaw.Value() * deltaTime);
  m_LocalBodyTransformationRate.angularVelocity.roll += DegreesPerSecond(acceleration.angularAcceleration.roll.Value() * deltaTime);
  m_LocalBodyTransformationRate.angularVelocity.pitch += DegreesPerSecond(acceleration.angularAcceleration.pitch.Value() * deltaTime);

  // If the Host sets all rate components to zero, the entity or articulated part will become stationary.
  if (m_LocalBodyTransformationRate.angularVelocity.pitch.IsZero() && m_LocalBodyTransformationRate.angularVelocity.roll.IsZero() &&
      m_LocalBodyTransformationRate.angularVelocity.yaw.IsZero() && m_LocalBodyTransformationRate.linearVelocity.isZero())
  {
    return;
  }

  TCigiBodyEulerRotation angularRotation;
  angularRotation.yaw = m_Rotation.yaw += Degrees(m_LocalBodyTransformationRate.angularVelocity.yaw.Value() * deltaTime);
  angularRotation.pitch = m_Rotation.pitch += Degrees90(m_LocalBodyTransformationRate.angularVelocity.pitch.Value() * deltaTime);
  angularRotation.roll = m_Rotation.roll += Degrees180(m_LocalBodyTransformationRate.angularVelocity.roll.Value() * deltaTime);

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
    // the rates are defined relative to the entity�s local coordinate system
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
  CEntity::Update(deltaTime);
  if (m_bInterpolationEnabled)
  {
    Interpolate(deltaTime);
  }

  SendUpdateMessage();
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
