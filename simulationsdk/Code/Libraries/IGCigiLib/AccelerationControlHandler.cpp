//Copyright SimBlocks LLC 2016-2026
#include "AccelerationControlHandler.h"
#include "CigiEntity.h"
#include "EntityLib/EntityManager.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/CigiArticulatedPart.h"
#include "UtilitiesLib/Logger.h"
#include "CigiLib/CigiConversions.h"

using namespace sbio;
using namespace sbio::entity;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

/// <summary>
/// Handle Acceleration direction.
/// </summary>
void CCigiAccelerationControlHandler::HandleEntityAccelerationControl(const SCigiEntityAcceleration& acceleration)
{
  CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(acceleration.entityID));

  if (pCigiEntity == nullptr)
  {
    stringstream ss;
    ss << "Entity ID " << acceleration.entityID << " does not exist for Acceleration Control. " << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  pCigiEntity->SetTransformationRateCoordinateSystem(acceleration.eCoordinateSystem);

  const sbio::math::TBodyEulerRotation angularAcceleration = ConvertToBodyEulerRotation(acceleration.angularAcceleration);

  // Apply world and local acceleration updates as appropriate for the coordinate system.
  // Parent-relative updates are applied in both contexts since they are relative to both parent and world.
  if (acceleration.eCoordinateSystem == EObjectCoordinateSystem::WORLD || acceleration.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
  {
    SCigiWorldAccelerationRate cigiWorldAcceleration;
    cigiWorldAcceleration.linearAcceleration = CigiNEDCoordinates(acceleration.linearAcceleration[0], acceleration.linearAcceleration[1], acceleration.linearAcceleration[2]);
    cigiWorldAcceleration.angularAcceleration = angularAcceleration;
    pCigiEntity->SetCigiWorldAccelerationRate(cigiWorldAcceleration);
  }

  // Local acceleration updates are applied in the local body coordinate system, which is used for both local and parent-relative updates.
  if (acceleration.eCoordinateSystem == EObjectCoordinateSystem::LOCAL || acceleration.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
  {
    SLocalBodyAccelerationRate localBodyAcceleration;
    localBodyAcceleration.linearAcceleration = ConvertCigiBodyCoordinatesToBodyCoordinates(acceleration.linearAcceleration);
    localBodyAcceleration.angularAcceleration = angularAcceleration;
    pCigiEntity->SetLocalBodyAccelerationRate(localBodyAcceleration);
  }
}

void CCigiAccelerationControlHandler::HandleArticulatedPartAccelerationControl(const SCigiArticulatedPartAcceleration& acceleration)
{
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(acceleration.entityID))
  {
    stringstream ss;
    ss << "Acceleration Control no entity with ID " << acceleration.entityID << endl;
    g_CigiLibGlobals.pLogger->LogDebug(ss.str());
    return;
  }

  if (!g_CigiLibGlobals.pEntityManager->HasArticulatedPart(acceleration.entityID, acceleration.articulatedPartID))
  {
    std::unique_ptr<CArticulatedPart> pArticulatedPart = std::make_unique<CCigiArticulatedPart>(acceleration.entityID, acceleration.articulatedPartID);
    g_CigiLibGlobals.pEntityManager->AddArticulatedPart(std::move(pArticulatedPart));
  }

  CArticulatedPart* pArticulatedPart = g_CigiLibGlobals.pEntityManager->GetArticulatedPart(acceleration.entityID, acceleration.articulatedPartID);

  SAccelerationRate<BodyCoordinates> accelerationRate;
  accelerationRate.linearAcceleration = ConvertCigiBodyCoordinatesToBodyCoordinates(acceleration.linearAcceleration);
  accelerationRate.angularAcceleration = ConvertToBodyEulerRotation(acceleration.angularAcceleration);
  pArticulatedPart->SetAcceleration(accelerationRate);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
