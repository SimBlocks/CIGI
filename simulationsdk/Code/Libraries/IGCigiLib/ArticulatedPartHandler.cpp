//Copyright SimBlocks LLC 2016-2026
#include "ArticulatedPartHandler.h"
#include "CigiArticulatedPart.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EntityLib/EntityManager.h"
#include "GlobalHeaders/CommonTypes.h"
#include "GlobalHeaders/Globals.h"
#include "MathLib/Math.h"
#include "MathLib/MathTypes.h"
#include "UtilitiesLib/Logger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::entity;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiArticulatedPartHandler::CCigiArticulatedPartHandler()
{
}

CCigiArticulatedPartHandler::~CCigiArticulatedPartHandler()
{
}

void CCigiArticulatedPartHandler::Handle(const SCigiArticulatedPart& articulatedPart)
{
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(articulatedPart.entityID))
  {
    stringstream ss;
    ss << "Entity ID " << articulatedPart.entityID << " does not exist for Articulated Part ID " << articulatedPart.articulatedPartID << endl;
    g_CigiLibGlobals.pLogger->LogDebug(ss.str());
    return;
  }

  if (!g_CigiLibGlobals.pEntityManager->HasArticulatedPart(articulatedPart.entityID, articulatedPart.articulatedPartID))
  {
    unique_ptr<CArticulatedPart> pArticulatedPart = std::make_unique<CCigiArticulatedPart>(articulatedPart.entityID, articulatedPart.articulatedPartID);
    g_CigiLibGlobals.pEntityManager->AddArticulatedPart(std::move(pArticulatedPart));
  }

  auto pArticulatedPart = g_CigiLibGlobals.pEntityManager->GetArticulatedPart(articulatedPart.entityID, articulatedPart.articulatedPartID);
  CCigiArticulatedPart* pCigiArticulatedPart = dynamic_cast<CCigiArticulatedPart*>(pArticulatedPart);

  if (articulatedPart.bEnabled)
  {
    pArticulatedPart->Enable();
  }
  else
  {
    pArticulatedPart->Disable();
  }

  bool bApplyTransform = false;
  CigiBodyCoordinates offset = ConvertBodyCoordinatesToCigiBodyCoordinates(pArticulatedPart->GetLocalTransformation().pos);

  for (int n = 0; n < 3; ++n)
  {
    if (articulatedPart.bOffsetEnabled[n])
    {
      offset[n] = articulatedPart.offset[n];
      bApplyTransform = true;
    }
  }

  TCigiBodyEulerRotation rotation = ConvertToCigiBodyEulerRotation(pArticulatedPart->GetBodyEulerRotation());

  if (articulatedPart.bYawEnabled)
  {
    rotation.yaw = articulatedPart.rotation.yaw;
    bApplyTransform = true;
  }

  if (articulatedPart.bPitchEnabled)
  {
    rotation.pitch = articulatedPart.rotation.pitch;
    bApplyTransform = true;
  }

  if (articulatedPart.bRollEnabled)
  {
    rotation.roll = articulatedPart.rotation.roll;
    bApplyTransform = true;
  }

  if (bApplyTransform)
  {
    pCigiArticulatedPart->ApplyTransformation(offset, rotation);
  }
}

void CCigiArticulatedPartHandler::Handle(const SCigiShortArticulatedPart& shortArticulatedPart)
{
  // If the specified entity does not exist, do nothing
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(shortArticulatedPart.entityID))
  {
    return;
  }

  // Find the second Articulated Part
  if (!g_CigiLibGlobals.pEntityManager->HasArticulatedPart(shortArticulatedPart.entityID, shortArticulatedPart.articulatedPartID2))
  {
    unique_ptr<CArticulatedPart> pArticulatedPart = std::make_unique<CCigiArticulatedPart>(shortArticulatedPart.entityID, shortArticulatedPart.articulatedPartID2);
    g_CigiLibGlobals.pEntityManager->AddArticulatedPart(std::move(pArticulatedPart));
  }
  auto pArticulatedPart2 = g_CigiLibGlobals.pEntityManager->GetArticulatedPart(shortArticulatedPart.entityID, shortArticulatedPart.articulatedPartID2);
  CCigiArticulatedPart* pCigiArticulatedPart2 = dynamic_cast<CCigiArticulatedPart*>(pArticulatedPart2);

  // Apply DOF2. This is the "last-in" value, so it always takes precedence
  pCigiArticulatedPart2->ApplyDegreeOfFreedom(shortArticulatedPart.eDOF2, shortArticulatedPart.fDOF2);

  // Apply Part2Enabled. This is the "last-in" value, so it always takes precedence
  if (shortArticulatedPart.bArticulatedPart2Enabled)
  {
    pCigiArticulatedPart2->Enable();
  }
  else
  {
    pCigiArticulatedPart2->Disable();
  }

  if (shortArticulatedPart.articulatedPartID1 == shortArticulatedPart.articulatedPartID2)
  {
    // If the degrees of freedom are different, apply DOF1 to part 2 (otherwise ignore the DOF1 since it is overwritten by the DOF2)
    if (shortArticulatedPart.eDOF1 != shortArticulatedPart.eDOF2)
    {
      pCigiArticulatedPart2->ApplyDegreeOfFreedom(shortArticulatedPart.eDOF1, shortArticulatedPart.fDOF1);
    }
  }
  else
  {
    // Find the first part
    if (!g_CigiLibGlobals.pEntityManager->HasArticulatedPart(shortArticulatedPart.entityID, shortArticulatedPart.articulatedPartID1))
    {
      unique_ptr<CArticulatedPart> pArticulatedPart = std::make_unique<CCigiArticulatedPart>(shortArticulatedPart.entityID, shortArticulatedPart.articulatedPartID1);
      g_CigiLibGlobals.pEntityManager->AddArticulatedPart(std::move(pArticulatedPart));
    }
    auto pArticulatedPart1 = g_CigiLibGlobals.pEntityManager->GetArticulatedPart(shortArticulatedPart.entityID, shortArticulatedPart.articulatedPartID1);
    CCigiArticulatedPart* pCigiArticulatedPart1 = dynamic_cast<CCigiArticulatedPart*>(pArticulatedPart1);
    // Apply DOF1, since this is a different part
    pCigiArticulatedPart1->ApplyDegreeOfFreedom(shortArticulatedPart.eDOF1, shortArticulatedPart.fDOF1);
    // Apply Part1Enabled, since this is a different part
    if (shortArticulatedPart.bArticulatedPart1Enabled)
    {
      pCigiArticulatedPart1->Enable();
    }
    else
    {
      pCigiArticulatedPart1->Disable();
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
