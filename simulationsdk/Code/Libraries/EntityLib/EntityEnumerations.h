//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityEnumerations.h
 * @brief Declares entity enumeration and lookup classes for entity metadata and descriptions.
 *
 * Provides the SEntityKindDomainCountry struct for composite entity keys and the CEntityEnumerations class
 * for loading, querying, and describing entity types, categories, domains, and countries. Supports mapping
 * between entity IDs and human-readable descriptions, as well as loading enumeration data from files.
 *
 * @see sbio::entity::CEntityType
 * @see sbio::entity::SEntityEnumeration
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_ENUMERATIONS_H
#define SIMBLOCKS_ENTITY_ENUMERATIONS_H

#include "EntityLib/EntityDeclarations.h"
#include "EntityTypes.h"
#include <map>
#include <memory>
#include <unordered_map>
#include <filesystem>

namespace sbio
{
  namespace entity
  {
    /**
     * @brief Composite key for entity kind, domain, and country.
     */
    struct SEntityKindDomainCountry
    {
      sbio::SisoEntityKindID entityKindID;
      sbio::SisoEntityDomainID entityDomainID;
      sbio::SisoEntityCountryID entityCountryID;

      SEntityKindDomainCountry();

      /**
       * @brief Less-than operator for use in ordered containers.
       */
      bool operator<(const SEntityKindDomainCountry& rhs) const;
    };

    /**
     * @brief Provides entity enumeration and lookup functionality for entity metadata and descriptions.
     *
     * Supports loading enumeration data, querying entity categories, domains, kinds, and countries,
     * and mapping entity IDs to human-readable descriptions.
     */
    class CEntityEnumerations
    {
    public:
      /**
       * @brief Gets the entity PDU category enumeration.
       * @return Pointer to the entity enumeration.
       */
      SEntityEnumeration* GetEntityPduCategory();

      /**
       * @brief Gets the country name for a given country ID.
       * @param entityCountryID The country ID.
       * @return The country name.
       */
      std::string GetCountry(sbio::SisoEntityCountryID entityCountryID);

      /**
       * @brief Gets the entity category name for a given category ID, kind, and domain.
       * @param entityCategoryID The category ID.
       * @param sEntityKind The entity kind as a string.
       * @param sEntityDomain The entity domain as a string.
       * @return The entity category name.
       */
      std::string GetEntityCategory(sbio::SisoEntityCategoryID entityCategoryID, const std::string& sEntityKind, const std::string& sEntityDomain);

      /**
       * @brief Gets the entity domain name for a given domain ID.
       * @param entityDomainID The domain ID.
       * @return The entity domain name.
       */
      std::string GetEntityDomain(sbio::SisoEntityDomainID entityDomainID);

      /**
       * @brief Loads entity enumeration data from a file.
       * @param filePath The file to load from.
       * @return True if loading was successful, false otherwise.
       */
      bool Load(std::filesystem::path filePath);

      /**
       * @brief Gets the entity kind name for a given kind ID.
       * @param entityKindID The kind ID.
       * @return The entity kind name.
       */
      std::string GetEntityKind(sbio::SisoEntityKindID entityKindID);

      /**
       * @brief Gets a description for an entity based on kind, domain, country, category, subcategory, and specific ID.
       * @param ekdc Composite key for kind, domain, and country.
       * @param entityCategoryID The category ID.
       * @param entitySubCategoryID The subcategory ID.
       * @param entitySpecificID The specific ID.
       * @return The entity description.
       */
      std::string GetDescription(SEntityKindDomainCountry ekdc, sbio::SisoEntityCategoryID entityCategoryID, sbio::SisoEntitySubCategoryID entitySubCategoryID, sbio::SisoEntitySpecificID entitySpecificID);

    private:
      std::unordered_map<SisoEnumSetID, std::unique_ptr<SEntityEnumeration>, StrongTypeHash<SisoEnumSetID>> m_EntityEnums;///< Map of entity enumerations
      std::map<SEntityKindDomainCountry, std::unique_ptr<CEntityType>> m_EntityTypes;///< Map of entity types
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
