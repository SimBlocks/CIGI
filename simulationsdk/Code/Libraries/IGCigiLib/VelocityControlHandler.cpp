//Copyright SimBlocks LLC 2016-2026
#include "VelocityControlHandler.h"
#include "CigiArticulatedPart.h"
#include "CigiEntity.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypes.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void CCigiVelocityControlHandler::HandleEntityVelocityControl(const SCigiEntityVelocityControl& entityVelocityControl)
{
  CCigiEntity* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(entityVelocityControl.entityID));

  if (pEntity == nullptr)
  {
    return;
  }

  const TBodyEulerRotation angularVelocity = ConvertToBodyEulerRotation(entityVelocityControl.angularVelocity);
  pEntity->SetTransformationRateCoordinateSystem(entityVelocityControl.coordinateSystem);

  // Apply world and local velocity updates as appropriate for the coordinate system.
  if (entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::WORLD || entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::PARENT)
  {
    SCigiWorldTransformationRate cigiWorldRate;
    cigiWorldRate.linearVelocity = CigiNEDCoordinates(entityVelocityControl.linearVelocity.Forward(), entityVelocityControl.linearVelocity.Right(), entityVelocityControl.linearVelocity.Down());
    cigiWorldRate.angularVelocity = angularVelocity;
    pEntity->SetCigiWorldTransformationRate(cigiWorldRate);
  }

  // Local velocity updates are applied in the local body coordinate system, which is used for local, parent-relative,
  // and child world/parent updates. Child entities interpolate from the local-body rate even when the packet uses
  // World/Parent, matching the previous single-rate behavior.
  if (entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::LOCAL || entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::PARENT || entityVelocityControl.coordinateSystem == EObjectCoordinateSystem::WORLD)
  {
    SLocalBodyTransformationRate localBodyRate;
    localBodyRate.linearVelocity = ConvertCigiBodyCoordinatesToBodyCoordinates(entityVelocityControl.linearVelocity);
    localBodyRate.angularVelocity = angularVelocity;
    pEntity->SetLocalBodyTransformationRate(localBodyRate);
  }
}

void CCigiVelocityControlHandler::HandleArticulatedPartVelocityControl(const SCigiArticulatedPartVelocityControl& articulatedPartVelocityControl)
{
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(articulatedPartVelocityControl.entityID))
  {
    return;
  }
  if (!g_CigiLibGlobals.pEntityManager->HasArticulatedPart(articulatedPartVelocityControl.entityID, articulatedPartVelocityControl.articulatedPartID))
  {
    unique_ptr<CArticulatedPart> pArticulatedPart = std::make_unique<CCigiArticulatedPart>(articulatedPartVelocityControl.entityID, articulatedPartVelocityControl.articulatedPartID);
    g_CigiLibGlobals.pEntityManager->AddArticulatedPart(std::move(pArticulatedPart));
  }

  CArticulatedPart* pArticulatedPart = g_CigiLibGlobals.pEntityManager->GetArticulatedPart(articulatedPartVelocityControl.entityID, articulatedPartVelocityControl.articulatedPartID);

  STransformationRate<BodyCoordinates> transformationRate;
  transformationRate.angularVelocity = ConvertToBodyEulerRotation(articulatedPartVelocityControl.angularVelocity);
  transformationRate.linearVelocity = ConvertCigiBodyCoordinatesToBodyCoordinates(articulatedPartVelocityControl.linearVelocity);
  pArticulatedPart->SetTransformationRate(transformationRate);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
