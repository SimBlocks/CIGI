//Copyright SimBlocks LLC 2016-2026
#include "AnimationControlHandler.h"
#include "CigiEntity.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EntityLib/EntityManager.h"
#include "UtilitiesLib/Logger.h"
#include <sstream>

using namespace sbio;
using namespace sbio::entity;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace std;

extern SIGCigiLibGlobals g_CigiLibGlobals;

/// <summary>
/// Handle animation direction, animation loop mode, animation state of an entity.
/// </summary>
void CCigiAnimationControlHandler::HandleCigiEntityAnimation(const SCigiAnimationControl& animationControl)
{
  EntityID entityID(animationControl.entityID);
  auto pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(entityID));

  if (pCigiEntity == nullptr)
  {
    stringstream ss;
    ss << "Entity ID " << animationControl.entityID << " does not exist for Animation ID " << animationControl.animationID << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  pCigiEntity->UpdateAnimation(animationControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
