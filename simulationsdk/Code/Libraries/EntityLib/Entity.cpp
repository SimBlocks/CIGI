//Copyright SimBlocks LLC 2016-2026
#include "Entity.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;

CEntity::CEntity(EntityID entityID) : m_EntityID(entityID)
{
}

CEntity::~CEntity()
{
}

void CEntity::Update(double deltaTime)
{
}

void CEntity::AttachToEntity(CEntity* pParent)
{
  if (pParent == nullptr)
  {
    Unattach();
    return;
  }

  if (m_pParent == pParent)
  {
    return;
  }

  if (m_pParent != nullptr)
  {
    m_pParent->m_Children.erase(m_EntityID);
  }

  m_ParentID = pParent->m_EntityID;
  m_pParent = pParent;
  m_pParent->m_Children.insert(m_EntityID);
}

TGeocentricTransform CEntity::GetWorldTransform() const
{
  if (IsChild())
  {
    // For child entities, calculate the world transform by combining the local transform with the parent's world transform.
    TGeocentricTransform parentTransform = m_pParent->GetWorldTransform();
    TGeocentricTransform worldTransform = parentTransform;
    worldTransform.rotation = worldTransform.rotation * m_LocalTransform.rotation;
    worldTransform.pos += parentTransform.rotation * m_LocalTransform.pos;
    return worldTransform;
  }
  else
  {
    return m_WorldTransform;
  }
}

sbio::math::TBodyTransform CEntity::GetChildTransform() const
{
  return m_LocalTransform;
}

bool CEntity::IsChild() const
{
  return m_pParent != nullptr;
  ;
}

bool CEntity::IsTopLevel() const
{
  return m_pParent == nullptr;
}

void CEntity::SetChildTransform(const sbio::math::TBodyTransform& childTransform)
{
  m_LocalTransform = childTransform;
  m_bTransformChanged = true;
}

void CEntity::SetWorldTransform(TGeocentricTransform geocentricTransform)
{
  m_WorldTransform = geocentricTransform;
  if (IsTopLevel())
  {
    // For top-level entities, also update the geodetic position and rotation based on the new world transform.
    m_GeodeticPosition = ConvertGeocentricToGeodeticCoordinates(m_WorldTransform.pos);
    m_Rotation = ConvertGeocentricRotationToBodyEulerRotation(m_WorldTransform);
  }
  m_bTransformChanged = true;
}

void CEntity::SetWorldTransform(TGeodeticTransform geodeticTransform)
{
  m_GeodeticPosition = geodeticTransform.pos;
  m_WorldTransform = ConvertGeodeticToGeocentricTransform(geodeticTransform);

  if (IsTopLevel())
  {
    m_Rotation = ConvertGeocentricRotationToBodyEulerRotation(m_WorldTransform);
  }

  m_bTransformChanged = true;
}

const SAccelerationRate<sbio::math::BodyCoordinates>& CEntity::GetAccelerationRate() const
{
  return m_AccelerationRate;
}

sbio::EntityID CEntity::GetEntityID() const
{
  return m_EntityID;
}

sbio::EntityID CEntity::GetParentID() const
{
  return m_ParentID;
}

const sbio::math::TBodyEulerRotation& CEntity::GetRotation() const
{
  return m_Rotation;
}

const STransformationRate<sbio::math::BodyCoordinates>& CEntity::GetTransformationRate() const
{
  return m_TransformationRate;
}

sbio::math::SGeodeticCoordinates CEntity::GetGeodeticCoordinates() const
{
  if (IsChild())
  {
    return ConvertGeocentricToGeodeticCoordinates(GetWorldTransform().pos);
  }
  else
  {
    return m_GeodeticPosition;
  }
}

void CEntity::SetAccelerationRate(const SAccelerationRate<sbio::math::BodyCoordinates>& accelerationRate)
{
  m_AccelerationRate = accelerationRate;
}

void CEntity::SetClamp(EClamp eClamp)
{
  m_eClamp = eClamp;
  m_bTransformChanged = true;
}

void CEntity::SetGeodeticCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoordinates)
{
  m_GeodeticPosition = geodeticCoordinates;

  if (IsTopLevel())
  {
    m_WorldTransform = ConvertGeodeticToGeocentricTransform(m_GeodeticPosition, m_Rotation);
    m_bTransformChanged = true;
  }
}

void CEntity::SetRotation(const sbio::math::TBodyEulerRotation& rotation)
{
  m_Rotation = rotation;

  if (IsTopLevel())
  {
    m_WorldTransform = ConvertGeodeticToGeocentricTransform(m_GeodeticPosition, m_Rotation);
    m_bTransformChanged = true;
  }
}

void CEntity::SetTransformationRate(const STransformationRate<sbio::math::BodyCoordinates>& transformationRate)
{
  m_TransformationRate = transformationRate;
}

void CEntity::Unattach()
{
  if (m_pParent != nullptr)
  {
    // When un-attaching, preserve the world transform by converting the current local transform to world coordinates
    // using the parent's world transform.
    m_WorldTransform = GetWorldTransform();
    m_GeodeticPosition = ConvertGeocentricToGeodeticCoordinates(m_WorldTransform.pos);
    m_Rotation = ConvertGeocentricRotationToBodyEulerRotation(m_WorldTransform);
    m_pParent->m_Children.erase(m_EntityID);
  }

  m_ParentID = UnknownEntityID;
  m_pParent = nullptr;
  m_TransformationRate.eCoordinateSystem = EObjectCoordinateSystem::WORLD;
  m_bTransformChanged = true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
