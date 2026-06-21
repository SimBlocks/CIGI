//Copyright SimBlocks LLC 2016-2026
#include "EntityManager.h"
#include "Entity.h"
#include "EntityEnumerations.h"
#include "EntityLib.h"
#include "EntityType.h"
#include "EntityLib/ArticulatedPart.h"
#include "UtilitiesLib/Logger.h"

using namespace std;
using namespace sbio;
using namespace sbio::entity;

std::size_t SEntityArticulatedPartHash::operator()(const SEntityArticulatedPartKey& entityArticulatedPartKey) const
{
  return (entityArticulatedPartKey.entityID.Value() << 16) | entityArticulatedPartKey.articulatedPartID.Value();
}

CEntityManager::CEntityManager()
{
}

CEntityManager::~CEntityManager()
{
}

bool CEntityManager::HasEntity(EntityID entityID) const
{
  if (m_Entities.find(entityID) == m_Entities.end())
  {
    return false;
  }

  return true;
}

void CEntityManager::AddEntity(EntityID entityID, unique_ptr<CEntity> pEntity)
{
  if (HasEntity(entityID))
  {
    if (g_EntityLibSettings.pLogger != nullptr)
    {
      const string sError = string("Adding entity with ID ") + to_string(entityID.Value()) + " before the existing entity was removed.";
      g_EntityLibSettings.pLogger->LogError(sError);
    }

    RemoveEntity(entityID);
  }

  m_Entities[entityID] = move(pEntity);
}

bool CEntityManager::HasEntityType(const SEntityType& entityType) const
{
  if (m_pEntityEnumerations == nullptr)
  {
    return false;
  }

  sbio::entity::SEntityKindDomainCountry ekdc;
  ekdc.entityKindID = entityType.entityKindID;
  ekdc.entityDomainID = entityType.entityDomainID;
  ekdc.entityCountryID = entityType.entityCountryID;
  string description = m_pEntityEnumerations->GetDescription(ekdc, entityType.entityCategoryID, entityType.entitySubCategoryID, entityType.entitySpecificID);

  if (description == "")
  {
    return false;
  }

  return true;
}

void CEntityManager::AddArticulatedPart(std::unique_ptr<CArticulatedPart> pArticulatedPart)
{
  SEntityArticulatedPartKey entityArticulatedPartKey;
  entityArticulatedPartKey.entityID = pArticulatedPart->GetEntityID();
  entityArticulatedPartKey.articulatedPartID = pArticulatedPart->GetArticulatedPartID();

  m_ArticulatedParts[entityArticulatedPartKey] = std::move(pArticulatedPart);
}

bool CEntityManager::HasArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID) const
{
  SEntityArticulatedPartKey entityArticulatedPartKey;
  entityArticulatedPartKey.entityID = entityID;
  entityArticulatedPartKey.articulatedPartID = articulatedPartID;

  TArticulatedParts::const_iterator it = m_ArticulatedParts.find(entityArticulatedPartKey);

  if (it == m_ArticulatedParts.end())
  {
    return false;
  }

  return true;
}

CArticulatedPart* CEntityManager::GetArticulatedPart(EntityID entityID, ArticulatedPartID articulatedPartID) const
{
  SEntityArticulatedPartKey entityArticulatedPartKey;
  entityArticulatedPartKey.entityID = entityID;
  entityArticulatedPartKey.articulatedPartID = articulatedPartID;

  TArticulatedParts::const_iterator it = m_ArticulatedParts.find(entityArticulatedPartKey);

  if (it == m_ArticulatedParts.end())
  {
    return nullptr;
  }

  return it->second.get();
}

CEntity* CEntityManager::GetEntity(EntityID entityID) const
{
  auto it = m_Entities.find(entityID);
  if (it == m_Entities.end())
  {
    return nullptr;
  }

  return it->second.get();
}

const CEntityManager::TEntities& CEntityManager::GetEntities() const
{
  return m_Entities;
}

void CEntityManager::Init()
{
  m_pEntityEnumerations = make_unique<CEntityEnumerations>();
  const auto entityEnumerationsPath = g_EntityLibSettings.dataPath / "SISO-REF-010.xml";
  if (!m_pEntityEnumerations->Load(entityEnumerationsPath))
  {
    const string sError = string("Failed to load entity enumerations metadata from \"") + entityEnumerationsPath.string() + "\".";
    if (g_EntityLibSettings.pLogger != nullptr)
    {
      g_EntityLibSettings.pLogger->LogWarning(sError);
    }

    return;
  }
}

std::string CEntityManager::Lookup(const SEntityType& entityType)
{
  if (m_pEntityEnumerations == nullptr)
  {
    return "";
  }

  sbio::entity::SEntityKindDomainCountry EKDC;
  EKDC.entityKindID = entityType.entityKindID;
  EKDC.entityDomainID = entityType.entityDomainID;
  EKDC.entityCountryID = entityType.entityCountryID;
  auto description = m_pEntityEnumerations->GetDescription(EKDC, entityType.entityCategoryID, entityType.entitySubCategoryID, entityType.entitySpecificID);
  return description;
}

void CEntityManager::RemoveEntity(EntityID entityID)
{
  auto itEntity = m_Entities.find(entityID);
  if (itEntity == m_Entities.end())
  {
    return;
  }

  for (auto& it : m_Entities)
  {
    if (it.first != entityID && it.second != nullptr && it.second->GetParentID() == entityID)
    {
      it.second->Unattach();
    }
  }

  if (itEntity->second != nullptr)
  {
    itEntity->second->Remove();
  }

  m_Entities.erase(itEntity);
  for (auto it = m_ArticulatedParts.begin(); it != m_ArticulatedParts.end();)
  {
    if (entityID == it->first.entityID)
    {
      m_ArticulatedParts.erase(it++);
    }
    else
    {
      ++it;
    }
  }
}

void CEntityManager::Reset()
{
  for (auto it = m_Entities.begin(); it != m_Entities.end(); it++)
  {
    it->second->Remove();
  }
  m_Entities.clear();

  m_ArticulatedParts.clear();
}

void CEntityManager::Update(bool bInterpolationEnabled, double fDeltaTime)
{
  UpdateEntities(bInterpolationEnabled, fDeltaTime);
  UpdateArticulatedParts(fDeltaTime);
}

void CEntityManager::UpdateArticulatedParts(double fDeltaTime)
{
  for (auto& itArticulatedPart : m_ArticulatedParts)
  {
    CArticulatedPart* pArticulatedPart = itArticulatedPart.second.get();
    pArticulatedPart->Update(fDeltaTime);
  }
}

void CEntityManager::UpdateEntities(bool bInterpolationEnabled, double deltaTime)
{
  for (auto& it : m_Entities)
  {
    auto& pEntity = it.second;
    pEntity->Update(deltaTime);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
