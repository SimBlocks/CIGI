//Copyright SimBlocks LLC 2016-2026
#include "CigiArticulatedPart.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib.h"
#include "MathLib/Math.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::math;
using namespace sbio::ig::entity;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiArticulatedPart::CCigiArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID) : CArticulatedPart(entityID, articulatedPartID)
{
}

CCigiArticulatedPart::~CCigiArticulatedPart()
{
}

void CCigiArticulatedPart::ApplyDegreeOfFreedom(EDegreeOfFreedom eDofSelect, float fDof)
{
  CigiBodyCoordinates offset = ConvertBodyCoordinatesToCigiBodyCoordinates(GetLocalTransformation().pos);
  TCigiBodyEulerRotation rotation = ConvertToCigiBodyEulerRotation(GetBodyEulerRotation());

  switch (eDofSelect)
  {
  case EDegreeOfFreedom::X_OFFSET:
  {
    offset[0] = fDof;
    break;
  }
  case EDegreeOfFreedom::Y_OFFSET:
  {
    offset[1] = fDof;
    break;
  }
  case EDegreeOfFreedom::Z_OFFSET:
  {
    offset[2] = fDof;
    break;
  }
  case EDegreeOfFreedom::YAW:
  {
    rotation.yaw = Degrees(fDof);
    break;
  }
  case EDegreeOfFreedom::PITCH:
  {
    rotation.pitch = Degrees(fDof);
    break;
  }
  case EDegreeOfFreedom::ROLL:
  {
    rotation.roll = Degrees(fDof);
    break;
  }
  }
  ApplyTransformation(offset, rotation);
}

void CCigiArticulatedPart::ApplyTransformation(const CigiBodyCoordinates& offset, const TCigiBodyEulerRotation& rotation)
{
  m_Position = ConvertCigiBodyCoordinatesToBodyCoordinates(offset);
  m_Rotation = ConvertToBodyEulerRotation(rotation);

  m_bDirty = true;
}

void CCigiArticulatedPart::Disable()
{
  if (m_bEnabled)
  {
    SSetArticulatedPartVisibleMessage data;
    data.ArticulatedPartID = m_ArticulatedPartID;
    data.EntityID = m_EntityID;
    data.Visible = false;

    g_CigiLibGlobals.pEventMessenger->SendSetArticulatedPartVisibleMessage(data);
  }

  CArticulatedPart::Disable();
}

void CCigiArticulatedPart::Enable()
{
  if (!m_bEnabled)
  {
    SSetArticulatedPartVisibleMessage data;
    data.ArticulatedPartID = m_ArticulatedPartID;
    data.EntityID = m_EntityID;
    data.Visible = true;

    g_CigiLibGlobals.pEventMessenger->SendSetArticulatedPartVisibleMessage(data);
  }

  CArticulatedPart::Enable();
}

void CCigiArticulatedPart::SetBodyEulerRotation(const sbio::math::TBodyEulerRotation& rotation)
{
  CArticulatedPart::SetBodyEulerRotation(rotation);

  m_bDirty = true;
}

void CCigiArticulatedPart::SetPosition(const sbio::math::BodyCoordinates& position)
{
  CArticulatedPart::SetPosition(position);

  m_Position = position;
  m_bDirty = true;
}

void CCigiArticulatedPart::Update(double fDeltaTime)
{
  CArticulatedPart::Update(fDeltaTime);

  if (m_bDirty)
  {
    SUpdateArticulatedPartTransformMessage data;
    data.ArticulatedPartID = m_ArticulatedPartID;
    data.EntityID = m_EntityID;

    data.Offset[0] = m_Position[0];
    data.Offset[1] = m_Position[1];
    data.Offset[2] = m_Position[2];

    // CIGI body rotation is defined as a rotation from the internal body axes (right, forward, up) to the world axes.
    TBodyMatrix m = GetLocalTransformation().rotation.toRotationMatrix();
    data.Rotation.Forward = m.getCol(1);
    data.Rotation.Up = m.getCol(2);

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendUpdateArticulatedPartTransformMessage(data);
    }

    m_bDirty = false;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
