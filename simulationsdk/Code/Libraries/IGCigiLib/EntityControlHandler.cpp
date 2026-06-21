//Copyright SimBlocks LLC 2016-2026
#include "EntityControlHandler.h"
#include "CigiEntity.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "CigiLib/CigiTypeDeclarations.h"
#include "EntityLib/EntityManager.h"
#include "EntityLib/EntityTypes.h"
#include "EntityLib/ArticulatedPart.h"
#include "GlobalHeaders/CommonTypes.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/IGCigiLib.h"
#include "ImageGenerator.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "MathLib/MathTypes.h"
#include "UtilitiesLib/Logger.h"
#include "ViewLib/ViewManager.h"
#include "ViewLib/View.h"
#include "ViewLib/ViewGroup.h"
#include "IGCigiLib/CigiView.h"
#include "IGCigiLib/PacketHandler.h"
#include "IGCigiLib/TerrainHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "IGCigiLib/PacketSender.h"
#include "IGCigiLib/PacketSenders.h"
#include "CigiLib/CigiTypesIGToHost.h"
#include "CigiEntityAnimation.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <cctype>

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::view;
using namespace sbio::cigi::ig;
using namespace sbio::ig::entity;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiEntityControlHandler::CCigiEntityControlHandler(CCigiImageGenerator& ImageGenerator, CCigiTerrainHandler* pTerrainHandler) : m_ImageGenerator(ImageGenerator), m_pTerrainHandler(pTerrainHandler)
{
}

CCigiEntityControlHandler::~CCigiEntityControlHandler()
{
}

void CCigiEntityControlHandler::HandleCigiConformalClampedEntityPosition(const SCigiConformalClampedEntityPosition& conformalClampedEntityPosition)
{
  //If a non-existent entity is referenced by a Conformal Clamped Entity Control packet, the packet will be ignored.
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(conformalClampedEntityPosition.entityID))
  {
    stringstream ss;
    ss << "Entity ID " << conformalClampedEntityPosition.entityID << " does not exist for Conformal Clamped Entity Position. " << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(conformalClampedEntityPosition.entityID));

  //If this packet is applied to an unclamped or non-conformal clamped entity,
  //its current absolute roll, pitch, and altitude will be maintained
  TCigiBodyEulerRotation rotation = ConvertToCigiBodyEulerRotation(pCigiEntity->GetRotation());

  SGeodeticCoordinates geodeticCoordinates;
  geodeticCoordinates.altitude = pCigiEntity->GetGeodeticCoordinates().altitude;
  geodeticCoordinates.latitude = conformalClampedEntityPosition.latitude;
  geodeticCoordinates.longitude = conformalClampedEntityPosition.longitude;

  //An entity�s current roll, pitch, and altitude offsets
  //(specified in the last Entity Control packet referencing the entity)
  //will be maintained when the IG receives a Conformal Clamped Entity Control packet describing that entity.

  //The entity is clamped and its attitude conforms to the terrain
  //The Pitch and Roll parameters specify the entity�s pitch and roll relative to the slope of the terrain or water.

  if (pCigiEntity->IsChild())
  {
    //TODO: can conformal clamped entity control packet be applied to parented entities?
  }
  else
  {
    pCigiEntity->SetTopLevelTransform(geodeticCoordinates, rotation);
  }
}

bool CCigiEntityControlHandler::HandleCigiEntityControl(const SEntityControl& entityControl, bool bBlankEntity)
{
  CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(entityControl.entityID));

  if (pCigiEntity == nullptr)
  {
    if (entityControl.eState == sbio::cigi::EActiveState::DESTROYED)
    {
      return true;
    }

    stringstream ss;
    ss << "Creating entity: ID " << entityControl.entityID << endl;
    if (entityControl.entityType.IsValid())
    {
      ss << " Extended Entity Type " << entityControl.entityType.ToEnumerationString();
    }
    else
    {
      ss << " Short Entity Type " << entityControl.shortEntityTypeID << endl;
    }

    g_CigiLibGlobals.pLogger->LogDebug(ss.str());

    std::unique_ptr<CCigiEntity> pNewCigiEntity = std::make_unique<CCigiEntity>(entityControl.entityID, entityControl);
    pCigiEntity = pNewCigiEntity.get();
    g_CigiLibGlobals.pEntityManager->AddEntity(entityControl.entityID, std::move(pNewCigiEntity));
  }
  else
  {
    if (entityControl.eState == EActiveState::DESTROYED)
    {
      pCigiEntity->Remove();
      g_CigiLibGlobals.pEntityManager->RemoveEntity(entityControl.entityID);
      return true;
    }

    pCigiEntity->SetEntityState(entityControl.eState);
    //A value of zero(0) corresponds to fully transparent; a value of 255 corresponds to fully opaque.
    //TODO: handle inherit alpha
    float fAlpha = entityControl.alpha / (float)255;
    pCigiEntity->SetAlpha(fAlpha);
    pCigiEntity->SetCollisionDetectionEnabled(entityControl.bCollisionReportingEnabled);
  }

  return true;
}

void CCigiEntityControlHandler::HandleCigiEntityPosition(EntityID entityID, EntityID parentID, EAttachState attachState, EClamp eGrndClamp, const sbio::math::Vec3& position, const TCigiBodyEulerRotation& rotation)
{
  CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(entityID));
  if (pCigiEntity == nullptr)
  {
    stringstream ss;
    ss << "Entity ID " << entityID << " does not exist for Entity Position." << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  if (!pCigiEntity->SetAttachState(attachState, parentID, false))
  {
    return;
  }

  if (attachState != EAttachState::ATTACH)
  {
    STopLevelEntityPosition topLevelEntityPosition;
    topLevelEntityPosition.entityID = entityID;
    topLevelEntityPosition.rotation.yaw = rotation.yaw;
    topLevelEntityPosition.rotation.pitch = rotation.pitch;
    topLevelEntityPosition.rotation.roll = rotation.roll;
    topLevelEntityPosition.geodeticCoordinates.latitude = Latitude(position[0]);
    topLevelEntityPosition.geodeticCoordinates.longitude = Longitude(position[1]);
    topLevelEntityPosition.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(position[2]);
    topLevelEntityPosition.eClamp = eGrndClamp;
    HandleCigiTopLevelEntityPosition(pCigiEntity, topLevelEntityPosition);
  }
  else
  {
    if (!g_CigiLibGlobals.pEntityManager->HasEntity(parentID))
    {
      stringstream ss;
      ss << "Parent Entity ID " << parentID << " does not exist for Entity Position." << endl;
      g_CigiLibGlobals.pLogger->LogWarning(ss.str());
      return;
    }

    SChildEntityPosition childEntityPosition;
    childEntityPosition.entityID = entityID;
    childEntityPosition.rotation.yaw = rotation.yaw;
    childEntityPosition.rotation.pitch = rotation.pitch;
    childEntityPosition.rotation.roll = rotation.roll;
    childEntityPosition.offset = position;
    childEntityPosition.parentID = parentID;
    HandleCigiChildEntityPosition(pCigiEntity, childEntityPosition);
  }

  if (eGrndClamp != EClamp::CONFORMAL && eGrndClamp != EClamp::NON_CONFORMAL)
  {
    pCigiEntity->SendUpdateMessage();
  }
}

void CCigiEntityControlHandler::HandleCigiPositionRequest(const SPositionRequest& positionRequest)
{
  if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::LOCAL && positionRequest.eObjectClass != EObjectClass::ARTICULATED_PART)
  {
    //submodel only valid for articulated parts
    return;
  }

  switch (positionRequest.eObjectClass)
  {
  case EObjectClass::ARTICULATED_PART:
  {
    SEntityArticulatedPartKey key;
    key.entityID = EntityID(positionRequest.nObjectID);
    key.articulatedPartID = positionRequest.articulatedPartID;
    RequestArticulatedPartPosition(key);

    if (positionRequest.bContinuous)
    {
      m_ContinuousArticulatedPartIDPositionRequests.insert(key);
    }
    else
    {
      auto it = m_ContinuousArticulatedPartIDPositionRequests.find(key);
      if (it != m_ContinuousArticulatedPartIDPositionRequests.end())
      {
        m_ContinuousArticulatedPartIDPositionRequests.erase(it);
      }
    }

    break;
  }
  case EObjectClass::ENTITY:
  {
    EntityID entityID = EntityID(positionRequest.nObjectID);
    CCigiEntity* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(entityID));

    if (pEntity == nullptr || pEntity->IsTopLevel() && positionRequest.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
    {
      //invalid
      return;
    }

    if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::WORLD)
    {
      RequestEntityPosition(pEntity);
      if (positionRequest.bContinuous)
      {
        m_ContinuousEntityIDPositionRequests.insert(entityID);
      }
      else
      {
        auto it = m_ContinuousEntityIDPositionRequests.find(entityID);
        if (it != m_ContinuousEntityIDPositionRequests.end())
        {
          m_ContinuousEntityIDPositionRequests.erase(it);
        }
      }
    }
    else if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
    {
      RequestChildEntityPosition(pEntity);
      if (positionRequest.bContinuous)
      {
        m_ContinuousChildEntityIDPositionRequests.insert(entityID);
      }
      else
      {
        auto it = m_ContinuousChildEntityIDPositionRequests.find(entityID);
        if (it != m_ContinuousChildEntityIDPositionRequests.end())
        {
          m_ContinuousChildEntityIDPositionRequests.erase(it);
        }
      }
    }
    break;
  }
  case EObjectClass::MOTION_TRACKER:
  {
    MotionTrackerID motionTrackerID(static_cast<uint8_t>(positionRequest.nObjectID));
    RequestMotionTrackerPosition(motionTrackerID);

    if (positionRequest.bContinuous)
    {
      m_ContinuousMotionTrackerPositionRequests.insert(motionTrackerID);
    }
    else
    {
      auto it = m_ContinuousMotionTrackerPositionRequests.find(motionTrackerID);
      if (it != m_ContinuousMotionTrackerPositionRequests.end())
      {
        m_ContinuousMotionTrackerPositionRequests.erase(it);
      }
    }
    break;
  }
  case EObjectClass::VIEW:
  {
    ViewID viewID = ViewID(positionRequest.nObjectID);
    if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::WORLD)
    {
      RequestGeodeticViewPosition(viewID);

      if (positionRequest.bContinuous)
      {
        m_ContinuousGeodeticViewIDPositionRequests.insert(viewID);
      }
      else
      {
        auto it = m_ContinuousGeodeticViewIDPositionRequests.find(viewID);
        if (it != m_ContinuousGeodeticViewIDPositionRequests.end())
        {
          m_ContinuousGeodeticViewIDPositionRequests.erase(it);
        }
      }
    }
    else if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
    {
      RequestLocalViewPosition(viewID);

      if (positionRequest.bContinuous)
      {
        m_ContinuousLocalViewIDPositionRequests.insert(viewID);
      }
      else
      {
        auto it = m_ContinuousLocalViewIDPositionRequests.find(viewID);
        if (it != m_ContinuousLocalViewIDPositionRequests.end())
        {
          m_ContinuousLocalViewIDPositionRequests.erase(it);
        }
      }
    }
    break;
  }
  case EObjectClass::VIEW_GROUP:
  {
    ViewGroupID viewGroupID = ViewGroupID(static_cast<uint8_t>(positionRequest.nObjectID));
    if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::WORLD)
    {
      RequestGeodeticViewGroupPosition(viewGroupID);

      if (positionRequest.bContinuous)
      {
        m_ContinuousGeodeticViewGroupIDPositionRequests.insert(viewGroupID);
      }
      else
      {
        auto it = m_ContinuousGeodeticViewGroupIDPositionRequests.find(viewGroupID);
        if (it != m_ContinuousGeodeticViewGroupIDPositionRequests.end())
        {
          m_ContinuousGeodeticViewGroupIDPositionRequests.erase(it);
        }
      }
    }
    else if (positionRequest.eCoordinateSystem == EObjectCoordinateSystem::PARENT)
    {
      RequestLocalViewGroupPosition(viewGroupID);

      if (positionRequest.bContinuous)
      {
        m_ContinuousLocalViewGroupIDPositionRequests.insert(viewGroupID);
      }
      else
      {
        auto it = m_ContinuousLocalViewGroupIDPositionRequests.find(viewGroupID);
        if (it != m_ContinuousLocalViewGroupIDPositionRequests.end())
        {
          m_ContinuousLocalViewGroupIDPositionRequests.erase(it);
        }
      }
    }
    break;
  }
  }
}

void CCigiEntityControlHandler::RequestEntityPosition(CCigiEntity* pEntity)
{
  auto worldTransform = pEntity->GetWorldTransform();
  SPositionResponseGeodeticCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::ENTITY;
  positionResponse.objectID = pEntity->GetEntityID().Value();
  //Position shall be specified as a geodetic latitude, longitude, and altitude. Orientation shall be given with respect to the reference plane
  positionResponse.geodeticCoordinates = ConvertCigiWorldToGeodeticCoordinates(worldTransform.pos);
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(ConvertCigiWorldRotationToBodyEulerRotation(worldTransform));
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::GEODETIC);
}

void sbio::cigi::ig::CCigiEntityControlHandler::RequestChildEntityPosition(CCigiEntity* pEntity)
{
  SPositionResponseParentEntityCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::ENTITY;
  positionResponse.offset = pEntity->GetChildTransform().pos;
  positionResponse.objectID = pEntity->GetEntityID().Value();
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(pEntity->GetRotation());
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::PARENT);
}

void CCigiEntityControlHandler::RequestArticulatedPartPosition(SEntityArticulatedPartKey articulatedPartKey)
{
  CArticulatedPart* pArticulatedPart = g_CigiLibGlobals.pEntityManager->GetArticulatedPart(articulatedPartKey.entityID, articulatedPartKey.articulatedPartID);

  if (!pArticulatedPart)
  {
    return;
  }

  SPositionResponseArticulatedPartCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::ARTICULATED_PART;
  positionResponse.objectID = articulatedPartKey.entityID.Value();
  positionResponse.articulatedPartID = articulatedPartKey.articulatedPartID;
  positionResponse.offset = ConvertBodyCoordinatesToCigiBodyCoordinates(pArticulatedPart->GetLocalTransformation().pos);
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(ConvertBodyRotationToBodyEulerRotation(pArticulatedPart->GetLocalTransformation().rotation));
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::ARTICULATED);
}

void CCigiEntityControlHandler::RequestGeodeticViewPosition(sbio::ViewID viewID)
{
  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewID));

  if (!pCigiView)
  {
    return;
  }

  SPositionResponseGeodeticCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::VIEW;
  auto worldTransform = pCigiView->GetWorldTransform();
  positionResponse.geodeticCoordinates = ConvertCigiWorldToGeodeticCoordinates(worldTransform.pos);
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(ConvertCigiWorldRotationToBodyEulerRotation(worldTransform));
  positionResponse.objectID = pCigiView->GetViewID().Value();
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::GEODETIC);
}

void CCigiEntityControlHandler::RequestLocalViewPosition(sbio::ViewID viewID)
{
  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(viewID));

  if (!pCigiView)
  {
    return;
  }
  SPositionResponseParentEntityCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::VIEW;
  TCigiBodyTransform childTransform = pCigiView->GetChildTransform();
  positionResponse.offset = childTransform.pos;
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(ConvertBodyRotationToBodyEulerRotation(ConvertCigiBodyRotationToBodyRotation(childTransform.rotation)));
  positionResponse.objectID = pCigiView->GetViewID().Value();
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::PARENT);
}

void CCigiEntityControlHandler::RequestGeodeticViewGroupPosition(sbio::ViewGroupID viewGroupID)
{
  CViewGroup* pViewGroup = g_CigiLibGlobals.pViewManager->GetViewGroup(viewGroupID);

  if (!pViewGroup)
  {
    return;
  }

  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(pViewGroup->GetCenterViewID()));

  if (!pCigiView)
  {
    return;
  }
  SPositionResponseGeodeticCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::VIEW_GROUP;
  auto worldTransform = pCigiView->GetWorldTransform();
  positionResponse.geodeticCoordinates = ConvertCigiWorldToGeodeticCoordinates(worldTransform.pos);
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(ConvertCigiWorldRotationToBodyEulerRotation(worldTransform));
  positionResponse.objectID = pCigiView->GetViewID().Value();
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::GEODETIC);
}

void sbio::cigi::ig::CCigiEntityControlHandler::RequestLocalViewGroupPosition(sbio::ViewGroupID viewGroupID)
{
  CViewGroup* pViewGroup = g_CigiLibGlobals.pViewManager->GetViewGroup(viewGroupID);

  if (!pViewGroup)
  {
    return;
  }

  CCigiView* pCigiView = dynamic_cast<CCigiView*>(g_CigiLibGlobals.pViewManager->GetView(pViewGroup->GetCenterViewID()));

  if (!pCigiView)
  {
    return;
  }

  SPositionResponseParentEntityCoordinates positionResponse;
  positionResponse.eObjectClass = EObjectClass::VIEW;
  TCigiBodyTransform childTransform = pCigiView->GetChildTransform();
  positionResponse.offset = childTransform.pos;
  positionResponse.rotation = ConvertToCigiBodyEulerRotation(ConvertBodyRotationToBodyEulerRotation(ConvertCigiBodyRotationToBodyRotation(childTransform.rotation)));
  positionResponse.objectID = pCigiView->GetViewID().Value();
  m_ImageGenerator.GetPacketSenders()->SendPositionResponse(&positionResponse, sbio::cigi::EPositionResponseType::PARENT);
}

void CCigiEntityControlHandler::RequestMotionTrackerPosition(sbio::MotionTrackerID motionTrackerID)
{
}

void CCigiEntityControlHandler::Update()
{
  for (auto it = m_ContinuousEntityIDPositionRequests.begin(); it != m_ContinuousEntityIDPositionRequests.end();)
  {
    CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(*it));
    if (pCigiEntity != nullptr)
    {
      RequestEntityPosition(pCigiEntity);
      ++it;
    }
    else
    {
      m_ContinuousEntityIDPositionRequests.erase(it++);
    }
  }

  for (auto it = m_ContinuousChildEntityIDPositionRequests.begin(); it != m_ContinuousChildEntityIDPositionRequests.end();)
  {
    CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(*it));
    if (pCigiEntity != nullptr)
    {
      RequestChildEntityPosition(pCigiEntity);
      ++it;
    }
    else
    {
      m_ContinuousChildEntityIDPositionRequests.erase(it++);
    }
  }

  for (auto articulatedPartKey : m_ContinuousArticulatedPartIDPositionRequests)
  {
    RequestArticulatedPartPosition(articulatedPartKey);
  }

  for (auto viewID : m_ContinuousGeodeticViewIDPositionRequests)
  {
    RequestGeodeticViewPosition(viewID);
  }

  for (auto viewID : m_ContinuousLocalViewIDPositionRequests)
  {
    RequestLocalViewPosition(viewID);
  }

  for (auto viewGroupID : m_ContinuousGeodeticViewGroupIDPositionRequests)
  {
    RequestGeodeticViewGroupPosition(viewGroupID);
  }

  for (auto viewGroupID : m_ContinuousLocalViewGroupIDPositionRequests)
  {
    RequestLocalViewGroupPosition(viewGroupID);
  }
}

void CCigiEntityControlHandler::HandleCigiTopLevelEntityPosition(CCigiEntity* pCigiEntity, const STopLevelEntityPosition& topLevelEntityPosition)
{
  pCigiEntity->SetGeodeticCoordinates(topLevelEntityPosition.geodeticCoordinates);
  pCigiEntity->SetClamp(topLevelEntityPosition.eClamp);
  TCigiBodyEulerRotation bodyRotation;
  bodyRotation.yaw = topLevelEntityPosition.rotation.yaw;
  bodyRotation.pitch = topLevelEntityPosition.rotation.pitch;
  bodyRotation.roll = topLevelEntityPosition.rotation.roll;
  pCigiEntity->SetRotation(ConvertToBodyEulerRotation(bodyRotation));

  if (topLevelEntityPosition.eClamp == EClamp::CONFORMAL || topLevelEntityPosition.eClamp == EClamp::NON_CONFORMAL)
  {
    m_pTerrainHandler->Handle(topLevelEntityPosition);
  }
  else
  {
    pCigiEntity->SetTopLevelTransform(topLevelEntityPosition.geodeticCoordinates, bodyRotation);
  }
}

void CCigiEntityControlHandler::HandleCigiChildEntityPosition(CCigiEntity* pCigiEntity, const SChildEntityPosition& childEntityPosition)
{
  if (pCigiEntity == nullptr)
  {
    return;
  }

  //If this parameter is set to Attach(1), the entity becomes or remains attached to the entity specified by the Parent ID parameter.
  //The parent must already exist, having been created in a prior frame or earlier in the current frame.

  //The value of Parent ID may be changed without first detaching the entity from its existing parent.
  // already attached and the parent ID has changed, unattach from old parent first.
  //if (entityProperties.bHasParent && entityProperties.parentID != childEntityPosition.parentID)
  //{
  //  pCigiEntity->Unattach();
  //}

  //The Yaw, Pitch, Roll, X Offset, Y Offset, and Z Offset parameters all specify the entity�s position relative to the parent�s coordinate system(see Section 3.4.2).
  TCigiBodyEulerRotation rotation;
  rotation.yaw = childEntityPosition.rotation.yaw;
  rotation.pitch = childEntityPosition.rotation.pitch;
  rotation.roll = childEntityPosition.rotation.roll;

  pCigiEntity->SetRotation(ConvertToBodyEulerRotation(rotation));
  SGeodeticCoordinates geodeticCoordinates = pCigiEntity->GetGeodeticCoordinates();
  geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(0);
  pCigiEntity->SetGeodeticCoordinates(geodeticCoordinates);
  pCigiEntity->SetClamp(EClamp::NONE);

  TCigiBodyTransform transformation = SetupChildEntityTransformation(childEntityPosition.parentID, CigiBodyCoordinates(childEntityPosition.offset), rotation);
  pCigiEntity->SetChildTransform(transformation);
}

TCigiBodyTransform CCigiEntityControlHandler::SetupChildEntityTransformation(EntityID parentID, const CigiBodyCoordinates& offset, const TCigiBodyEulerRotation& rotation)
{
  TCigiBodyTransform transformation;
  transformation.pos = offset;
  transformation.rotation = SetupCigiObjectRotation(rotation);

  return transformation;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
