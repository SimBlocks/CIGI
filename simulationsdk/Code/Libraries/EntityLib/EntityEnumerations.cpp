//Copyright SimBlocks LLC 2016-2026
#include "EntityEnumerations.h"
#include "EntityLib.h"
#include "EntityType.h"
#include "EntityTypes.h"
#include "tinyxml2.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace sbio;
using namespace sbio::entity;
using namespace tinyxml2;

//values defined in SISO-REF-010.xml
SisoEnumSetID SISO_PDU_TYPE = SisoEnumSetID(4);
SisoEnumSetID SISO_ENTITY_KIND = SisoEnumSetID(7);
SisoEnumSetID SISO_ENTITY_PLATFORM = SisoEnumSetID(8);
SisoEnumSetID SISO_COUNTRY_ID = SisoEnumSetID(29);
SisoEnumSetID SISO_ENTITY_TYPES_ID = SisoEnumSetID(30);

SEntityKindDomainCountry::SEntityKindDomainCountry() : entityKindID(UnknownSisoEntityKindID), entityDomainID(UnknownSisoEntityDomainID), entityCountryID(UnknownSisoEntityCountryID)
{
}

bool SEntityKindDomainCountry::operator<(const SEntityKindDomainCountry& rhs) const
{
  if (entityKindID == rhs.entityKindID)
  {
    if (entityDomainID == rhs.entityDomainID)
    {
      return entityCountryID < rhs.entityCountryID;
    }

    return entityDomainID < rhs.entityDomainID;
  }

  return entityKindID < rhs.entityKindID;
}

SEntityEnumeration* CEntityEnumerations::GetEntityPduCategory()
{
  return m_EntityEnums[SISO_PDU_TYPE].get();
}

std::string CEntityEnumerations::GetCountry(SisoEntityCountryID entityCountryID)
{
  return m_EntityEnums[SISO_COUNTRY_ID]->entityDescriptions[entityCountryID.Value()].sDescription;
}

std::string CEntityEnumerations::GetEntityCategory(SisoEntityCategoryID entityCategoryID, const std::string& sEntityKind, const std::string& sEntityDomain)
{
  string s = sEntityKind + "-" + sEntityDomain + " Category";

  for (auto it = m_EntityEnums.begin(); it != m_EntityEnums.end(); ++it)
  {
    if (it->second->sName == s)
    {
      return it->second->entityDescriptions[entityCategoryID.Value()].sDescription;
    }
  }

  return "";
}

std::string CEntityEnumerations::GetEntityKind(SisoEntityKindID entityKindID)
{
  return m_EntityEnums[SISO_ENTITY_KIND]->entityDescriptions[entityKindID.Value()].sDescription;
}

std::string CEntityEnumerations::GetDescription(SEntityKindDomainCountry ekdc, SisoEntityCategoryID entityCategoryID, SisoEntitySubCategoryID entitySubCategoryID, SisoEntitySpecificID entitySpecificID)
{
  string sDescription = "";
  const auto& it = m_EntityTypes.find(ekdc);

  // If the entity type exists, attempt to find the category, subcategory, and specific descriptions.
  if (it != m_EntityTypes.end())
  {
    auto& entityType = it->second;
    SEntityCategory* category = nullptr;
    category = entityType->GetCategory(entityCategoryID);
    if (category != nullptr)
    {
      sDescription += category->sDescription;
      const auto& it2 = category->subCategories.find(entitySubCategoryID);
      if (it2 != category->subCategories.end())
      {
        auto& subcategory = it2->second;
        sDescription += " " + subcategory->sDescription;
        const auto& it3 = subcategory->specifics.find(entitySpecificID);
        if (it3 != subcategory->specifics.end())
        {
          auto& specific = it3->second;
          sDescription += " " + specific->sDescription;
        }
      }
    }
  }
  return sDescription;
}

std::string CEntityEnumerations::GetEntityDomain(SisoEntityDomainID entityDomainID)
{
  return m_EntityEnums[SISO_ENTITY_PLATFORM]->entityDescriptions[entityDomainID.Value()].sDescription;
}

void ParseEntityTypes(XMLElement* pEntityTypesXml, std::map<SEntityKindDomainCountry, std::unique_ptr<CEntityType>>& entityTypes)
{
  XMLElement* pEntityXml = pEntityTypesXml->FirstChildElement("entity");

  while (pEntityXml != nullptr)
  {
    SEntityKindDomainCountry entityType;

    entityType.entityKindID = SisoEntityKindID(static_cast<uint8_t>(atoi(pEntityXml->Attribute("kind"))));
    entityType.entityDomainID = SisoEntityDomainID(static_cast<uint8_t>(atoi(pEntityXml->Attribute("domain"))));
    entityType.entityCountryID = SisoEntityCountryID(static_cast<uint16_t>(atoi(pEntityXml->Attribute("country"))));

    unique_ptr<CEntityType> pEntityType = make_unique<CEntityType>(entityType.entityKindID, entityType.entityDomainID, entityType.entityCountryID);

    XMLElement* pCategoryXml = pEntityXml->FirstChildElement("category");
    while (pCategoryXml != nullptr)
    {
      unique_ptr<SEntityCategory> pEntityCategory = make_unique<SEntityCategory>();
      pEntityCategory->entityCategoryID = SisoEntityCategoryID(static_cast<uint8_t>(atoi(pCategoryXml->Attribute("value"))));
      pEntityCategory->sDescription = pCategoryXml->Attribute("description");

      XMLElement* pSubCategoryXml = pCategoryXml->FirstChildElement("subcategory");
      while (pSubCategoryXml != nullptr)
      {
        unique_ptr<SEntitySubCategory> pEntitySubCategory = make_unique<SEntitySubCategory>();
        pEntitySubCategory->entitySubCategoryID = SisoEntitySubCategoryID(static_cast<uint8_t>(atoi(pSubCategoryXml->Attribute("value"))));
        pEntitySubCategory->sDescription = pSubCategoryXml->Attribute("description");

        XMLElement* pEntitySecificXml = pSubCategoryXml->FirstChildElement("specific");
        while (pEntitySecificXml != nullptr)
        {
          unique_ptr<SEntitySpecific> pEntitySpecific = make_unique<SEntitySpecific>();
          pEntitySpecific->entitySpecificID = SisoEntitySpecificID(static_cast<uint8_t>(atoi(pEntitySecificXml->Attribute("value"))));
          pEntitySpecific->sDescription = pEntitySecificXml->Attribute("description");

          pEntitySubCategory->specifics[pEntitySpecific->entitySpecificID] = std::move(pEntitySpecific);

          pEntitySecificXml = pEntitySecificXml->NextSiblingElement("specific");
        }

        pEntityCategory->subCategories[pEntitySubCategory->entitySubCategoryID] = std::move(pEntitySubCategory);

        pSubCategoryXml = pSubCategoryXml->NextSiblingElement("subcategory");
      }

      pEntityType->AddCategory(pEntityCategory);

      pCategoryXml = pCategoryXml->NextSiblingElement("category");
    }

    entityTypes[entityType] = move(pEntityType);

    pEntityXml = pEntityXml->NextSiblingElement("entity");
  }
}

// Parse entity enumerations XML file.
bool CEntityEnumerations::Load(std::filesystem::path filePath)
{
  XMLDocument doc;
  if (doc.LoadFile(filePath.string().c_str()) != XML_SUCCESS)
  {
    return false;
  }

  XMLElement* pRoot = doc.RootElement();

  //ready dictionary of acronyms
  XMLElement* pDict = pRoot->FirstChildElement("dict");

  XMLElement* pEnum = pDict->NextSiblingElement("enum");
  while (pEnum != nullptr)
  {
    unique_ptr<SEntityEnumeration> pEntityCategory = make_unique<SEntityEnumeration>();
    pEntityCategory->enumSetID = SisoEnumSetID(static_cast<uint8_t>(atoi(pEnum->Attribute("uid"))));
    pEntityCategory->sName = pEnum->Attribute("name");

    // parse enumeration values
    XMLElement* pEnumrow = pEnum->FirstChildElement("enumrow");
    while (pEnumrow != nullptr)
    {
      SEntityValueDescription entityDescription;
      entityDescription.nValue = atoi(pEnumrow->Attribute("value"));
      entityDescription.sDescription = pEnumrow->Attribute("description");

      pEnumrow = pEnumrow->NextSiblingElement("enumrow");

      pEntityCategory->entityDescriptions[entityDescription.nValue] = entityDescription;
    }

    m_EntityEnums[pEntityCategory->enumSetID] = std::move(pEntityCategory);

    pEnum = pEnum->NextSiblingElement("enum");
  }

  // ready entity types
  XMLElement* pcetrow = pDict->NextSiblingElement("cet");
  while (pcetrow != nullptr)
  {
    unique_ptr<SEntityEnumeration> pEntityCategory = make_unique<SEntityEnumeration>();
    pEntityCategory->enumSetID = SisoEnumSetID(static_cast<uint8_t>(atoi(pcetrow->Attribute("uid"))));
    pEntityCategory->sName = pcetrow->Attribute("name");

    if (pEntityCategory->enumSetID == SISO_ENTITY_TYPES_ID)
    {
      ParseEntityTypes(pcetrow, m_EntityTypes);
    }

    pcetrow = pcetrow->NextSiblingElement("cet");
  }

  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
