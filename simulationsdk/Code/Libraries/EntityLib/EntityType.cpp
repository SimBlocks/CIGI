//Copyright SimBlocks LLC 2016-2026
#include "EntityType.h"

using namespace std;
using namespace sbio;
using namespace sbio::entity;

CEntityType::CEntityType(SisoEntityKindID entityKindID, SisoEntityDomainID entityDomainID, SisoEntityCountryID entityCountryID) : m_entityKindID(entityKindID), m_entityDomainID(entityDomainID), m_entityCountryID(entityCountryID)
{
}

CEntityType::~CEntityType()
{
}

void CEntityType::AddCategory(unique_ptr<SEntityCategory>& pEntityCategory)
{
  m_EntityCategories[pEntityCategory->entityCategoryID] = std::move(pEntityCategory);
}

SEntityCategory* CEntityType::GetCategory(SisoEntityCategoryID entityCategoryID) const
{
  const auto& it = m_EntityCategories.find(entityCategoryID);
  if (it != m_EntityCategories.end())
  {
    return (it->second).get();
  }

  return nullptr;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
