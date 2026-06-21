//Copyright SimBlocks LLC 2016-2026
#include "ArticulatedPart.h"
#include "MathLib/Math.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;

CArticulatedPart::CArticulatedPart(EntityID entityID, ArticulatedPartID articulatedPartID)
{
  m_EntityID = entityID;
  m_ArticulatedPartID = articulatedPartID;
}

CArticulatedPart::~CArticulatedPart()
{
}

void CArticulatedPart::Enable()
{
  m_bEnabled = true;
}

void CArticulatedPart::Disable()
{
  m_bEnabled = false;
}

sbio::math::TBodyEulerRotation CArticulatedPart::GetBodyEulerRotation() const
{
  return m_Rotation;
}

sbio::EntityID CArticulatedPart::GetEntityID() const
{
  return m_EntityID;
}

sbio::ArticulatedPartID CArticulatedPart::GetArticulatedPartID() const
{
  return m_ArticulatedPartID;
}

void CArticulatedPart::Update(double fDeltaTime)
{
  float t = static_cast<float>(fDeltaTime);

  if (t == 0)
  {
    return;
  }

  // Update the transformation rate based on the acceleration rate, then update the position and rotation based on the transformation rate.
  if (!m_TransformationRate.linearVelocity.isZero() || !m_AccelerationRate.linearAcceleration.isZero())
  {
    m_TransformationRate.linearVelocity += m_AccelerationRate.linearAcceleration * fDeltaTime;
    BodyCoordinates offset(m_TransformationRate.linearVelocity * fDeltaTime);
    SetPosition(m_Position + offset);
  }

  // Update the angular velocity based on the angular acceleration, then update the rotation based on the angular velocity.
  if (!m_AccelerationRate.angularAcceleration.yaw.IsZero() || !m_AccelerationRate.angularAcceleration.pitch.IsZero() || !m_AccelerationRate.angularAcceleration.roll.IsZero() || !m_TransformationRate.angularVelocity.yaw.IsZero() || !m_TransformationRate.angularVelocity.pitch.IsZero() ||
      !m_TransformationRate.angularVelocity.roll.IsZero())
  {
    m_TransformationRate.angularVelocity.yaw += (m_AccelerationRate.angularAcceleration.yaw * static_cast<float>(fDeltaTime));
    m_TransformationRate.angularVelocity.roll += (m_AccelerationRate.angularAcceleration.roll * static_cast<float>(fDeltaTime));
    m_TransformationRate.angularVelocity.pitch += (m_AccelerationRate.angularAcceleration.pitch * static_cast<float>(fDeltaTime));

    TBodyEulerRotation bodyEulerRotation;
    bodyEulerRotation = m_Rotation;
    bodyEulerRotation.yaw += m_TransformationRate.angularVelocity.yaw * (float)fDeltaTime;
    bodyEulerRotation.pitch += m_TransformationRate.angularVelocity.pitch * (float)fDeltaTime;
    bodyEulerRotation.roll += m_TransformationRate.angularVelocity.roll * (float)fDeltaTime;

    SetBodyEulerRotation(bodyEulerRotation);
  }
}

void CArticulatedPart::SetAcceleration(const SAccelerationRate<sbio::math::BodyCoordinates>& acceleration)
{
  m_AccelerationRate = acceleration;
}

void CArticulatedPart::SetBodyEulerRotation(const sbio::math::TBodyEulerRotation& rotation)
{
  m_Rotation = rotation;
}

void CArticulatedPart::SetPosition(const sbio::math::BodyCoordinates& position)
{
  m_Position = position;
}

void CArticulatedPart::SetTransformationRate(const sbio::entity::STransformationRate<sbio::math::BodyCoordinates>& transformationRate)
{
  m_TransformationRate = transformationRate;
}

TBodyTransform CArticulatedPart::GetLocalTransformation() const
{
  TBodyTransform transformation;
  transformation.pos = m_Position;

  // TBodyEulerRotation uses the same yaw/pitch/roll semantics as CIGI body Euler angles,
  // while TBodyRotation uses internal body axes (right, forward, up).
  // Build the quaternion in the Euler extraction frame and then remap it back into internal body coordinates
  // so this function is the inverse of ConvertBodyRotationToBodyEulerRotation.
  Quaternion4d q;
  q.setIdentity();
  q = q * AxisRotation(DegreesToRadians(-m_Rotation.yaw).Value(), Vec3::UnitZ());
  q = q * AxisRotation(DegreesToRadians(m_Rotation.pitch).Value(), Vec3::UnitX());
  q = q * AxisRotation(DegreesToRadians(m_Rotation.roll).Value(), Vec3::UnitY());
  transformation.rotation = q;

  return transformation;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
