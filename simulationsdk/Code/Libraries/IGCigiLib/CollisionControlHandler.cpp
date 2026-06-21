//Copyright SimBlocks LLC 2016-2026
#include "CollisionControlHandler.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib.h"
#include "IGCigiLib/CigiEntity.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include <sstream>

using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::physics;
using namespace std;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

sbio::cigi::ig::CCigiCollisionControlHandler::CCigiCollisionControlHandler()
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->RegisterListener<IGCIGIEvent>(this);
  }
}

sbio::cigi::ig::CCigiCollisionControlHandler::~CCigiCollisionControlHandler()
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->UnregisterListener<IGCIGIEvent>(this);
  }
}

void CCigiCollisionControlHandler::Reset()
{
  m_CollisionCuboidDefinitions.clear();
  m_CollisionSphereDefinitions.clear();
  m_CollisionSegmentDefinitions.clear();
}

void CCigiCollisionControlHandler::OnEntityRemoved(sbio::EntityID entityID)
{
  RemoveEntityCollisionState(entityID);
}

void CCigiCollisionControlHandler::RemoveEntityCollisionState(sbio::EntityID entityID)
{
  for (auto it = m_CollisionCuboidDefinitions.begin(); it != m_CollisionCuboidDefinitions.end();)
  {
    if (std::get<0>(it->first) == entityID)
    {
      it = m_CollisionCuboidDefinitions.erase(it);
    }
    else
    {
      ++it;
    }
  }

  for (auto it = m_CollisionSphereDefinitions.begin(); it != m_CollisionSphereDefinitions.end();)
  {
    if (std::get<0>(it->first) == entityID)
    {
      it = m_CollisionSphereDefinitions.erase(it);
    }
    else
    {
      ++it;
    }
  }

  for (auto it = m_CollisionSegmentDefinitions.begin(); it != m_CollisionSegmentDefinitions.end();)
  {
    if (std::get<0>(it->first) == entityID)
    {
      it = m_CollisionSegmentDefinitions.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

sbio::ig::physics::SSetCollisionVolumeMessage CreateCollisionVolumeMessage(const sbio::cigi::SCollisionDetectionSphereDefinition& collisionVolumeDefinition)
{
  sbio::ig::physics::SSetCollisionVolumeMessage data;
  data.EntityID = collisionVolumeDefinition.entityID;
  data.VolumeID = collisionVolumeDefinition.volumeID;
  data.Radius = collisionVolumeDefinition.fRadius;
  data.Offset = ConvertCigiBodyCoordinatesToBodyCoordinates(collisionVolumeDefinition.offset);
  data.Rotation.Forward[0] = 0.0;
  data.Rotation.Forward[1] = 1.0;
  data.Rotation.Forward[2] = 0.0;
  data.Rotation.Up[0] = 0.0;
  data.Rotation.Up[1] = 0.0;
  data.Rotation.Up[2] = 1.0;
  return data;
}

sbio::ig::physics::SSetCollisionVolumeMessage CreateCollisionVolumeMessage(const sbio::cigi::SCollisionDetectionCuboidDefinition& collisionVolumeDefinition)
{
  sbio::ig::physics::SSetCollisionVolumeMessage data;
  data.EntityID = collisionVolumeDefinition.entityID;
  data.VolumeID = collisionVolumeDefinition.volumeID;
  data.Offset = ConvertCigiBodyCoordinatesToBodyCoordinates(collisionVolumeDefinition.offset);
  data.Height = collisionVolumeDefinition.fHeight;
  data.Width = collisionVolumeDefinition.fWidth;
  data.Depth = collisionVolumeDefinition.fDepth;

  //rotate clockwise around z (yaw)
  Eigen::AngleAxisd yawAngleAxis(DegreesToRadians(collisionVolumeDefinition.rotation.yaw).Value(), Vec3::UnitZ());

  //rotate clockwise around y (pitch)
  Eigen::AngleAxisd pitchAngleAxis(DegreesToRadians(collisionVolumeDefinition.rotation.pitch).Value(), Vec3::UnitY());

  //rotate clockwise around x (roll)
  Eigen::AngleAxisd rollAngleAxis(DegreesToRadians(collisionVolumeDefinition.rotation.roll).Value(), Vec3::UnitX());
  Eigen::Quaterniond q = rollAngleAxis * pitchAngleAxis * yawAngleAxis;

  Mat3 m = q.toRotationMatrix();
  data.Rotation.Forward = BodyCoordinates(m.col(1));
  data.Rotation.Up = BodyCoordinates(m.col(2));
  return data;
}

void sbio::cigi::ig::CCigiCollisionControlHandler::CreateNewCollisionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collisionVolumeDefinition, std::tuple<sbio::EntityID, sbio::VolumeID> pair)
{
  SCreateCollisionVolumeCuboidMessage data;
  data.EntityID = collisionVolumeDefinition.entityID;
  data.VolumeID = collisionVolumeDefinition.volumeID;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendCreateCollisionVolumeCuboidMessage(data);
    g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeMessage(CreateCollisionVolumeMessage(collisionVolumeDefinition));
  }

  m_CollisionCuboidDefinitions[pair] = collisionVolumeDefinition;

  SSetCollisionVolumeEnabledMessage enabledMessage;
  enabledMessage.EntityID = collisionVolumeDefinition.entityID;
  enabledMessage.VolumeID = collisionVolumeDefinition.volumeID;
  enabledMessage.Enabled = collisionVolumeDefinition.bVolumeEnabled;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeEnabledMessage(enabledMessage);
  }
}

void sbio::cigi::ig::CCigiCollisionControlHandler::CreateNewCollisionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collisionVolumeDefinition, std::tuple<sbio::EntityID, sbio::VolumeID> pair)
{
  SCreateCollisionVolumeSphereMessage data;
  data.EntityID = collisionVolumeDefinition.entityID;
  data.VolumeID = collisionVolumeDefinition.volumeID;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendCreateCollisionVolumeSphereMessage(data);
    g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeMessage(CreateCollisionVolumeMessage(collisionVolumeDefinition));
  }

  m_CollisionSphereDefinitions[pair] = collisionVolumeDefinition;

  SSetCollisionVolumeEnabledMessage enabledMessage;
  enabledMessage.EntityID = collisionVolumeDefinition.entityID;
  enabledMessage.VolumeID = collisionVolumeDefinition.volumeID;
  enabledMessage.Enabled = collisionVolumeDefinition.bVolumeEnabled;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeEnabledMessage(enabledMessage);
  }
}

void CCigiCollisionControlHandler::HandleCigiCollisionDetectionSegmentDefinition(const sbio::cigi::SCollisionDetectionSegmentDefinition& collisionDetectionSegmentDefinition)
{
  auto pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(collisionDetectionSegmentDefinition.entityID));
  if (pCigiEntity == nullptr)
  {
    stringstream ss;
    ss << "Entity ID " << collisionDetectionSegmentDefinition.entityID << " does not exist for Collision Detection Segment Definition. " << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  auto pair = std::tuple<sbio::EntityID, sbio::SegmentID>(collisionDetectionSegmentDefinition.entityID, collisionDetectionSegmentDefinition.segmentID);
  auto currentCollisionSegmentDefinitionKeyValue = m_CollisionSegmentDefinitions.find(pair);
  const bool bIsNewSegment = currentCollisionSegmentDefinitionKeyValue == m_CollisionSegmentDefinitions.end();

  if (!bIsNewSegment)
  {
    const auto& currentCollisionSegmentDefinition = currentCollisionSegmentDefinitionKeyValue->second;
    if (currentCollisionSegmentDefinition.bSegmentEnabled == collisionDetectionSegmentDefinition.bSegmentEnabled && currentCollisionSegmentDefinition.nMaterialMask == collisionDetectionSegmentDefinition.nMaterialMask &&
        currentCollisionSegmentDefinition.beg.equals(collisionDetectionSegmentDefinition.beg) && currentCollisionSegmentDefinition.end.equals(collisionDetectionSegmentDefinition.end))
    {
      return;
    }
  }

  const bool bShouldSetSegmentProperties = bIsNewSegment || currentCollisionSegmentDefinitionKeyValue->second.nMaterialMask != collisionDetectionSegmentDefinition.nMaterialMask || !currentCollisionSegmentDefinitionKeyValue->second.beg.equals(collisionDetectionSegmentDefinition.beg) ||
                                           !currentCollisionSegmentDefinitionKeyValue->second.end.equals(collisionDetectionSegmentDefinition.end);

  if (bIsNewSegment)
  {
    SCreateCollisionDetectionSegmentMessage data;
    data.EntityID = collisionDetectionSegmentDefinition.entityID;
    data.SegmentID = collisionDetectionSegmentDefinition.segmentID;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendCreateCollisionDetectionSegmentMessage(data);
    }
  }

  if (bShouldSetSegmentProperties)
  {
    SSetCollisionDetectionSegmentMessage data;
    data.EntityID = collisionDetectionSegmentDefinition.entityID;
    data.SegmentID = collisionDetectionSegmentDefinition.segmentID;
    data.BeginPos = ConvertCigiBodyCoordinatesToBodyCoordinates(collisionDetectionSegmentDefinition.beg);
    data.EndPos = ConvertCigiBodyCoordinatesToBodyCoordinates(collisionDetectionSegmentDefinition.end);
    data.MaterialMask = collisionDetectionSegmentDefinition.nMaterialMask;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetCollisionDetectionSegmentMessage(data);
    }
  }

  const bool bShouldSetSegmentEnabled = bIsNewSegment || currentCollisionSegmentDefinitionKeyValue->second.bSegmentEnabled != collisionDetectionSegmentDefinition.bSegmentEnabled;

  if (bShouldSetSegmentEnabled)
  {
    SSetCollisionDetectionSegmentEnabledMessage enabledData;
    enabledData.EntityID = collisionDetectionSegmentDefinition.entityID;
    enabledData.SegmentID = collisionDetectionSegmentDefinition.segmentID;
    enabledData.Enabled = collisionDetectionSegmentDefinition.bSegmentEnabled;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetCollisionDetectionSegmentEnabledMessage(enabledData);
    }
  }

  m_CollisionSegmentDefinitions[pair] = collisionDetectionSegmentDefinition;
}

void CCigiCollisionControlHandler::HandleCigiCollisionCuboidDefinition(const sbio::cigi::SCollisionDetectionCuboidDefinition& collisionVolumeDefinition)
{
  auto pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(collisionVolumeDefinition.entityID));
  if (pCigiEntity == nullptr)
  {
    stringstream ss;
    ss << "Entity ID " << collisionVolumeDefinition.entityID << " does not exist for Collision Detection Cuboid Definition. " << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  auto pair = std::tuple<sbio::EntityID, sbio::VolumeID>(collisionVolumeDefinition.entityID, collisionVolumeDefinition.volumeID);
  const auto& currentCollisionVolumeDefinitionKeyValue = m_CollisionCuboidDefinitions.find(pair);

  if (currentCollisionVolumeDefinitionKeyValue == m_CollisionCuboidDefinitions.end())
  {
    CreateNewCollisionCuboidVolume(collisionVolumeDefinition, pair);
    return;
  }

  auto& currentCollisionVolume = currentCollisionVolumeDefinitionKeyValue->second;
  const bool bShouldSetVolumeEnabled = currentCollisionVolume.bVolumeEnabled != collisionVolumeDefinition.bVolumeEnabled;

  if (bShouldSetVolumeEnabled)
  {
    SSetCollisionVolumeEnabledMessage data;
    data.EntityID = collisionVolumeDefinition.entityID;
    data.VolumeID = collisionVolumeDefinition.volumeID;
    data.Enabled = collisionVolumeDefinition.bVolumeEnabled;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeEnabledMessage(data);
    }

    currentCollisionVolume.bVolumeEnabled = collisionVolumeDefinition.bVolumeEnabled;
  }

  if (currentCollisionVolume.fHeight != collisionVolumeDefinition.fHeight || currentCollisionVolume.fWidth != collisionVolumeDefinition.fWidth || currentCollisionVolume.fDepth != collisionVolumeDefinition.fDepth || currentCollisionVolume.rotation.yaw != collisionVolumeDefinition.rotation.yaw ||
      currentCollisionVolume.rotation.pitch != collisionVolumeDefinition.rotation.pitch || currentCollisionVolume.rotation.roll != collisionVolumeDefinition.rotation.roll || currentCollisionVolume.offset[0] != collisionVolumeDefinition.offset[0] ||
      currentCollisionVolume.offset[1] != collisionVolumeDefinition.offset[1] || currentCollisionVolume.offset[2] != collisionVolumeDefinition.offset[2])
  {
    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeMessage(CreateCollisionVolumeMessage(collisionVolumeDefinition));
    }

    currentCollisionVolume.fHeight = collisionVolumeDefinition.fHeight;
    currentCollisionVolume.fWidth = collisionVolumeDefinition.fWidth;
    currentCollisionVolume.fDepth = collisionVolumeDefinition.fDepth;
    currentCollisionVolume.rotation.yaw = collisionVolumeDefinition.rotation.yaw;
    currentCollisionVolume.rotation.pitch = collisionVolumeDefinition.rotation.pitch;
    currentCollisionVolume.rotation.roll = collisionVolumeDefinition.rotation.roll;
    currentCollisionVolume.offset[0] = collisionVolumeDefinition.offset[0];
    currentCollisionVolume.offset[1] = collisionVolumeDefinition.offset[1];
    currentCollisionVolume.offset[2] = collisionVolumeDefinition.offset[2];
  }
}

void CCigiCollisionControlHandler::HandleCigiCollisionSphereDefinition(const sbio::cigi::SCollisionDetectionSphereDefinition& collisionVolumeDefinition)
{
  auto pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(collisionVolumeDefinition.entityID));
  if (pCigiEntity == nullptr)
  {
    stringstream ss;
    ss << "Entity ID " << collisionVolumeDefinition.entityID << " does not exist for Collision Detection Sphere Definition. " << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  auto pair = std::tuple<sbio::EntityID, sbio::VolumeID>(collisionVolumeDefinition.entityID, collisionVolumeDefinition.volumeID);
  const auto& currentCollisionVolumeDefinitionKeyValue = m_CollisionSphereDefinitions.find(pair);

  if (currentCollisionVolumeDefinitionKeyValue == m_CollisionSphereDefinitions.end())
  {
    CreateNewCollisionSphereVolume(collisionVolumeDefinition, pair);
    return;
  }

  auto& currentCollisionVolume = currentCollisionVolumeDefinitionKeyValue->second;

  const bool bShouldSetVolumeEnabled = currentCollisionVolume.bVolumeEnabled != collisionVolumeDefinition.bVolumeEnabled;

  if (bShouldSetVolumeEnabled)
  {
    SSetCollisionVolumeEnabledMessage data;
    data.EntityID = collisionVolumeDefinition.entityID;
    data.VolumeID = collisionVolumeDefinition.volumeID;
    data.Enabled = collisionVolumeDefinition.bVolumeEnabled;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeEnabledMessage(data);
    }

    currentCollisionVolume.bVolumeEnabled = collisionVolumeDefinition.bVolumeEnabled;
  }

  if (currentCollisionVolume.fRadius != collisionVolumeDefinition.fRadius || currentCollisionVolume.offset[0] != collisionVolumeDefinition.offset[0] || currentCollisionVolume.offset[1] != collisionVolumeDefinition.offset[1] || currentCollisionVolume.offset[2] != collisionVolumeDefinition.offset[2])
  {
    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetCollisionVolumeMessage(CreateCollisionVolumeMessage(collisionVolumeDefinition));
    }

    currentCollisionVolume.fRadius = collisionVolumeDefinition.fRadius;
    currentCollisionVolume.offset[0] = collisionVolumeDefinition.offset[0];
    currentCollisionVolume.offset[1] = collisionVolumeDefinition.offset[1];
    currentCollisionVolume.offset[2] = collisionVolumeDefinition.offset[2];
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
