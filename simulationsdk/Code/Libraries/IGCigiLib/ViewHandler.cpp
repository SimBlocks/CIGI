//Copyright SimBlocks LLC 2016-2026
#include "ViewHandler.h"
#include "CigiView.h"
#include "ViewLib/ViewManager.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/CigiViewGroup.h"
#include "ImageGenerator.h"
#include "UtilitiesLib/Logger.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include <sstream>

using namespace std;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::engine;
using namespace sbio::entity;
using namespace sbio::view;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiViewHandler::CCigiViewHandler(CCigiImageGenerator* pImageGenerator) : m_pImageGenerator(pImageGenerator)
{
}

void CCigiViewHandler::Handle(const SCigiViewControl& viewControl)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(viewControl);
  }

  if (viewControl.viewGroupID == ViewGroupID(0))
  {
    CCigiView* pView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewControl.viewID));

    if (!pView)
    {
      stringstream ss;
      ss << "View " << viewControl.viewID << " has not been created. View ID is invalid." << endl;
      g_CigiLibGlobals.pLogger->LogWarning(ss.str());
      return;
    }

    // Only update an individual View's attached entity if the individual view is NOT a mebmer of a valid view group
    if (!g_CigiLibGlobals.pViewManager->GetViewGroup(pView->GetViewGroupID()))
    {
      // If the view is being attached and the specified entity does not exist, log a warning
      if (!g_CigiLibGlobals.pEntityManager->HasEntity(viewControl.entityID))
      {
        stringstream ss;
        ss << "Entity " << viewControl.entityID << " has not been created. Cannot attach view to entity." << endl;
        g_CigiLibGlobals.pLogger->LogWarning(ss.str());
      }
      else
      {
        pView->SetAttachedEntityID(viewControl.entityID);
      }
    }

    CigiBodyCoordinates bodyOffset = pView->GetOffset();
    if (viewControl.offsetEnabled[0])
    {
      bodyOffset[0] = viewControl.offset[0];
    }
    if (viewControl.offsetEnabled[1])
    {
      bodyOffset[1] = viewControl.offset[1];
    }
    if (viewControl.offsetEnabled[2])
    {
      bodyOffset[2] = viewControl.offset[2];
    }

    pView->SetOffset(bodyOffset);

    if (viewControl.bYawEnabled)
    {
      pView->SetYaw(viewControl.rotation.yaw);
    }
    if (viewControl.bPitchEnabled)
    {
      pView->SetPitch(viewControl.rotation.pitch);
    }
    if (viewControl.bRollEnabled)
    {
      pView->SetRoll(viewControl.rotation.roll);
    }
  }
  else
  {
    CCigiViewGroup* pViewGroup = dynamic_cast<CCigiViewGroup*>(g_CigiLibGlobals.pViewManager->GetViewGroup(viewControl.viewGroupID));

    if (!pViewGroup)
    {
      stringstream ss;
      ss << "View group " << viewControl.viewGroupID << " has not been created." << endl;
      g_CigiLibGlobals.pLogger->LogWarning(ss.str());
      return;
    }

    // If the view is being attached and the specified entity does not exist, log a warning
    if (!g_CigiLibGlobals.pEntityManager->HasEntity(viewControl.entityID))
    {
      stringstream ss;
      ss << "Entity " << viewControl.entityID << " has not been created. Cannot attach view to entity." << endl;
      g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    }
    else
    {
      pViewGroup->SetAttachedEntityID(viewControl.entityID);
    }

    CigiBodyCoordinates bodyOffset = pViewGroup->GetOffset();
    if (viewControl.offsetEnabled[0])
    {
      bodyOffset[0] = viewControl.offset[0];
    }
    if (viewControl.offsetEnabled[1])
    {
      bodyOffset[1] = viewControl.offset[1];
    }
    if (viewControl.offsetEnabled[2])
    {
      bodyOffset[2] = viewControl.offset[2];
    }

    if (viewControl.offsetEnabled[0] || viewControl.offsetEnabled[1] || viewControl.offsetEnabled[2])
    {
      pViewGroup->SetOffset(bodyOffset);
    }

    if (viewControl.bYawEnabled)
    {
      pViewGroup->SetYaw(viewControl.rotation.yaw);
    }
    if (viewControl.bPitchEnabled)
    {
      pViewGroup->SetPitch(viewControl.rotation.pitch);
    }
    if (viewControl.bRollEnabled)
    {
      pViewGroup->SetRoll(viewControl.rotation.roll);
    }
  }
}

void CCigiViewHandler::Handle(const SCigiViewDefinition& viewDefinition)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(viewDefinition);
  }

  CCigiView* pView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewDefinition.viewID));
  if (pView == nullptr)
  {
    //View definition is used to override the default parameters for a view.
    //The standard does not specify that a new view should be created if the view does not already exist.
    //Assume that it is the IG's responsibility to setup and assign views to channels
    stringstream ss;
    ss.str("");
    ss.clear();
    ss << "No CIGI View Found: " << viewDefinition.viewID << endl;
    g_CigiLibGlobals.pLogger->LogError(ss);

    return;
  }

  if (viewDefinition.bNearEnabled)
  {
    pView->SetNearPlane(viewDefinition.fNear);
  }

  if (viewDefinition.bFarEnabled)
  {
    pView->SetFarPlane(viewDefinition.fFar);
  }

  if (viewDefinition.bLeftEnabled)
  {
    pView->SetLeftHalfAngle(viewDefinition.fLeft);
  }

  if (viewDefinition.bRightEnabled)
  {
    pView->SetRightHalfAngle(viewDefinition.fRight);
  }

  if (viewDefinition.bTopEnabled)
  {
    pView->SetTopHalfAngle(viewDefinition.fTop);
  }

  if (viewDefinition.bBottomEnabled)
  {
    pView->SetBottomHalfAngle(viewDefinition.fBottom);
  }

  pView->SetProjectionMode(viewDefinition.eProjectionMode);
  pView->SetMirrorMode(viewDefinition.eMirrorMode);
  if (viewDefinition.bReorder)
  {
    pView->BringToTop();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
