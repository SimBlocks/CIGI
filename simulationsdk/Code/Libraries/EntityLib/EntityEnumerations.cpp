//Copyright SimBlocks LLC 2016-2026
#include "EntityEnumerations.h"
#include "EntityLib.h"
#include "EntityType.h"
#include "EntityTypes.h"
#include "tinyxml2.h"
#include <charconv>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

using namespace std;
using namespace sbio;
using namespace sbio::entity;
using namespace tinyxml2;

namespace
{
  // Helper function to read an integer attribute from an XML element and convert it to the specified type.
  template <typename TValue>
  bool ReadIntegerAttribute(const XMLElement* element, const char* name, TValue& value)
  {
    const char* text = element->Attribute(name);
    if (text == nullptr)
    {
      return false;
    }

    const std::string_view token(text);
    const auto result = std::from_chars(token.data(), token.data() + token.size(), value);
    return result.ec == std::errc() && result.ptr == token.data() + token.size();
  }

  // Helper function to read an identifier attribute from an XML element and convert it to the specified identifier type.
  template <typename TIdentifier>
  bool ReadIdentifierAttribute(const XMLElement* element, const char* name, TIdentifier& identifier)
  {
    decltype(identifier.Value()) value = 0;
    if (!ReadIntegerAttribute(element, name, value))
    {
      return false;
    }

    identifier = TIdentifier(value);
    return true;
  }

  // Helper function to read a string attribute from an XML element.
  bool ReadStringAttribute(const XMLElement* element, const char* name, std::string& value)
  {
    const char* text = element->Attribute(name);
    if (text == nullptr)
    {
      return false;
    }

    value = text;
    return true;
  }
}

// values defined in SISO-REF-010.xml
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
  const auto it = m_EntityEnums.find(SISO_PDU_TYPE);
  return it != m_EntityEnums.end() ? it->second.get() : nullptr;
}

std::string CEntityEnumerations::GetCountry(SisoEntityCountryID entityCountryID)
{
  const auto it = m_EntityEnums.find(SISO_COUNTRY_ID);
  return it != m_EntityEnums.end() ? GetEnumerationDescription(it->second.get(), entityCountryID.Value()) : "";
}

std::string CEntityEnumerations::GetEntityCategory(SisoEntityCategoryID entityCategoryID, const std::string& sEntityKind, const std::string& sEntityDomain)
{
  string s = sEntityKind + "-" + sEntityDomain + " Category";

  for (auto it = m_EntityEnums.begin(); it != m_EntityEnums.end(); ++it)
  {
    if (it->second != nullptr && it->second->sName == s)
    {
      return GetEnumerationDescription(it->second.get(), entityCategoryID.Value());
    }
  }

  return "";
}

std::string CEntityEnumerations::GetEntityKind(SisoEntityKindID entityKindID)
{
  const auto it = m_EntityEnums.find(SISO_ENTITY_KIND);
  return it != m_EntityEnums.end() ? GetEnumerationDescription(it->second.get(), entityKindID.Value()) : "";
}

std::string CEntityEnumerations::GetDescription(SEntityKindDomainCountry entityKindComainCountry, SisoEntityCategoryID entityCategoryID,
                                                SisoEntitySubCategoryID entitySubCategoryID, SisoEntitySpecificID entitySpecificID)
{
  string sDescription = "";
  const auto& it = m_EntityTypes.find(entityKindComainCountry);

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
  const auto it = m_EntityEnums.find(SISO_ENTITY_PLATFORM);
  return it != m_EntityEnums.end() ? GetEnumerationDescription(it->second.get(), entityDomainID.Value()) : "";
}

std::string CEntityEnumerations::GetEnumerationDescription(const SEntityEnumeration* pEnumeration, int value) const
{
  if (pEnumeration == nullptr)
  {
    return "";
  }

  const auto it = pEnumeration->entityDescriptions.find(value);
  return it != pEnumeration->entityDescriptions.end() ? it->second.sDescription : "";
}

bool ParseEntityTypes(XMLElement* pEntityTypesXml, std::map<SEntityKindDomainCountry, std::unique_ptr<CEntityType>>& entityTypes)
{
  XMLElement* pEntityXml = pEntityTypesXml->FirstChildElement("entity");

  while (pEntityXml != nullptr)
  {
    SEntityKindDomainCountry entityType;

    if (!ReadIdentifierAttribute(pEntityXml, "kind", entityType.entityKindID) || !ReadIdentifierAttribute(pEntityXml, "domain", entityType.entityDomainID) ||
        !ReadIdentifierAttribute(pEntityXml, "country", entityType.entityCountryID))
    {
      return false;
    }

    unique_ptr<CEntityType> pEntityType = make_unique<CEntityType>(entityType.entityKindID, entityType.entityDomainID, entityType.entityCountryID);

    XMLElement* pCategoryXml = pEntityXml->FirstChildElement("category");
    while (pCategoryXml != nullptr)
    {
      unique_ptr<SEntityCategory> pEntityCategory = make_unique<SEntityCategory>();
      if (!ReadIdentifierAttribute(pCategoryXml, "value", pEntityCategory->entityCategoryID) || !ReadStringAttribute(pCategoryXml, "description", pEntityCategory->sDescription))
      {
        return false;
      }

      XMLElement* pSubCategoryXml = pCategoryXml->FirstChildElement("subcategory");
      while (pSubCategoryXml != nullptr)
      {
        unique_ptr<SEntitySubCategory> pEntitySubCategory = make_unique<SEntitySubCategory>();
        if (!ReadIdentifierAttribute(pSubCategoryXml, "value", pEntitySubCategory->entitySubCategoryID) ||
            !ReadStringAttribute(pSubCategoryXml, "description", pEntitySubCategory->sDescription))
        {
          return false;
        }

        XMLElement* pEntitySecificXml = pSubCategoryXml->FirstChildElement("specific");
        while (pEntitySecificXml != nullptr)
        {
          unique_ptr<SEntitySpecific> pEntitySpecific = make_unique<SEntitySpecific>();
          if (!ReadIdentifierAttribute(pEntitySecificXml, "value", pEntitySpecific->entitySpecificID) ||
              !ReadStringAttribute(pEntitySecificXml, "description", pEntitySpecific->sDescription))
          {
            return false;
          }

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

  return true;
}

// Parse entity enumerations XML file.
bool CEntityEnumerations::Load(std::filesystem::path filePath)
{
  // Open the XML file for reading in binary mode.
  std::ifstream file(filePath, std::ios::binary);

  // Check if the file was successfully opened.
  if (!file.is_open())
  {
    return false;
  }

  // Read the entire contents of the file into a string.
  const std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  if (file.bad())
  {
    return false;
  }

  // Parse the XML contents using TinyXML2.
  XMLDocument doc;
  if (doc.Parse(contents.data(), contents.size()) != XML_SUCCESS)
  {
    return false;
  }

  // Check if the root element is valid and has the expected name "ebv".
  XMLElement* pRoot = doc.RootElement();
  if (pRoot == nullptr || std::string_view(pRoot->Name()) != "ebv")
  {
    return false;
  }

  // ready dictionary of acronyms
  XMLElement* pDict = pRoot->FirstChildElement("dict");
  if (pDict == nullptr)
  {
    return false;
  }

  decltype(m_EntityEnums) entityEnums;
  decltype(m_EntityTypes) entityTypes;

  XMLElement* pEnum = pDict->NextSiblingElement("enum");
  while (pEnum != nullptr)
  {
    unique_ptr<SEntityEnumeration> pEntityCategory = make_unique<SEntityEnumeration>();
    if (!ReadIdentifierAttribute(pEnum, "uid", pEntityCategory->enumSetID) || !ReadStringAttribute(pEnum, "name", pEntityCategory->sName))
    {
      return false;
    }

    // parse enumeration values
    XMLElement* pEnumrow = pEnum->FirstChildElement("enumrow");
    while (pEnumrow != nullptr)
    {
      SEntityValueDescription entityDescription;
      if (!ReadIntegerAttribute(pEnumrow, "value", entityDescription.nValue) || !ReadStringAttribute(pEnumrow, "description", entityDescription.sDescription))
      {
        return false;
      }

      pEnumrow = pEnumrow->NextSiblingElement("enumrow");

      pEntityCategory->entityDescriptions[entityDescription.nValue] = entityDescription;
    }

    entityEnums[pEntityCategory->enumSetID] = std::move(pEntityCategory);

    pEnum = pEnum->NextSiblingElement("enum");
  }

  // ready entity types
  XMLElement* pcetrow = pDict->NextSiblingElement("cet");
  while (pcetrow != nullptr)
  {
    unique_ptr<SEntityEnumeration> pEntityCategory = make_unique<SEntityEnumeration>();
    if (!ReadIdentifierAttribute(pcetrow, "uid", pEntityCategory->enumSetID) || !ReadStringAttribute(pcetrow, "name", pEntityCategory->sName))
    {
      return false;
    }

    if (pEntityCategory->enumSetID == SISO_ENTITY_TYPES_ID && !ParseEntityTypes(pcetrow, entityTypes))
    {
      return false;
    }

    pcetrow = pcetrow->NextSiblingElement("cet");
  }

  m_EntityEnums.swap(entityEnums);
  m_EntityTypes.swap(entityTypes);
  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
