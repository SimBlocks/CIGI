//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityEnumerations.h
 * @brief Declares entity enumeration and lookup classes for entity metadata and descriptions.
 *
 * Provides the SEntityKindDomainCountry struct for composite entity keys and the CEntityEnumerations class
 * for loading, querying, and describing entity types, categories, domains, and countries. Supports mapping
 * between SISO classification identifiers and human-readable descriptions loaded from XML files.
 *
 * @see sbio::entity::CEntityType
 * @see sbio::entity::SEntityEnumeration
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_ENUMERATIONS_H
#define SIMBLOCKS_ENTITY_ENUMERATIONS_H

#include "EntityLib/EntityDeclarations.h"
#include "EntityType.h"
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
      sbio::SisoEntityKindID entityKindID;///< SISO entity kind identifier.
      sbio::SisoEntityDomainID entityDomainID;///< SISO entity domain identifier.
      sbio::SisoEntityCountryID entityCountryID;///< SISO country identifier.

      /**
       * @brief Constructs a key with all identifiers set to their unknown sentinels.
       */
      SEntityKindDomainCountry();

      /**
       * @brief Orders keys by kind, then domain, then country.
       * @param rhs Key to compare against.
       * @return `true` when this key sorts before `rhs` in that order; otherwise `false`.
       */
      bool operator<(const SEntityKindDomainCountry& rhs) const;
    };

    /**
     * @brief Owns SISO enumeration metadata and entity classification hierarchies loaded from XML.
     *
     * Initially contains no metadata. `Load()` replaces the stored metadata only on success.
     * Lookups return empty strings or null pointers when the requested metadata is unavailable;
     * `GetDescription()` can return a partial description from matching hierarchy levels.
     */
    class CEntityEnumerations
    {
    public:
      /**
       * @brief Gets the PDU type enumeration (SISO enumeration set 4).
       * @return Non-owning pointer to the stored enumeration, or `nullptr` if it is unavailable.
       *
       * The pointer is invalidated by a successful `Load()` or destruction of this object.
       */
      SEntityEnumeration* GetEntityPduCategory();

      /**
       * @brief Gets the country name for a given country ID.
       * @param entityCountryID The country ID.
       * @return Stored country description, or an empty string if the enumeration or value is absent.
       */
      std::string GetCountry(sbio::SisoEntityCountryID entityCountryID);

      /**
       * @brief Gets the entity category name for a given category ID, kind, and domain.
       * @param entityCategoryID The category ID.
       * @param sEntityKind Kind text used to construct the enumeration name.
       * @param sEntityDomain Domain text used to construct the enumeration name.
       * @return Stored category description, or an empty string if the enumeration or value is absent.
       *
       * Matches the enumeration name `sEntityKind + "-" + sEntityDomain + " Category"` exactly,
       * including case.
       */
      std::string GetEntityCategory(sbio::SisoEntityCategoryID entityCategoryID, const std::string& sEntityKind, const std::string& sEntityDomain);

      /**
       * @brief Gets the entity domain name for a given domain ID.
       * @param entityDomainID The domain ID.
       * @return Stored domain description from SISO enumeration set 8, or an empty string if absent.
       */
      std::string GetEntityDomain(sbio::SisoEntityDomainID entityDomainID);

      /**
       * @brief Replaces the metadata with enumerations and entity types parsed from a SISO XML file.
       * @param filePath Path to an XML file with an `ebv` root and a `dict` child.
       * @return `true` after parsing and replacing the metadata; `false` on file open/read failure,
       *         malformed XML, missing required elements or attributes, or invalid numeric attributes.
       *
       * Reads `enum` and `cet` siblings following `dict`; entity hierarchies come from `cet` set 30.
       * Success does not require any particular enumeration set to be present. A `false` result
       * leaves existing metadata and pointers into it unchanged. Success invalidates those pointers.
       */
      bool Load(std::filesystem::path filePath);

      /**
       * @brief Gets the entity kind name for a given kind ID.
       * @param entityKindID The kind ID.
       * @return Stored kind description, or an empty string if the enumeration or value is absent.
       */
      std::string GetEntityKind(sbio::SisoEntityKindID entityKindID);

      /**
       * @brief Builds a description from matching category, subcategory, and specific entries.
       * @param entityKindComainCountry Composite key for kind, domain, and country.
       * @param entityCategoryID The category ID.
       * @param entitySubCategoryID The subcategory ID.
       * @param entitySpecificID The specific ID.
       * @return Category description with each matching subordinate description appended after a space,
       *         or an empty string when the grouping or category is absent.
       *
       * Lookup stops at the first missing hierarchy level and retains the descriptions already found.
       * No extra identifier is used, and empty stored descriptions are not filtered out.
       */
      std::string GetDescription(SEntityKindDomainCountry entityKindComainCountry, sbio::SisoEntityCategoryID entityCategoryID, sbio::SisoEntitySubCategoryID entitySubCategoryID,
                                 sbio::SisoEntitySpecificID entitySpecificID);

    protected:
      /**
       * @brief Looks up a numeric value in an enumeration's description table.
       * @param pEnumeration Non-owning enumeration pointer; may be `nullptr`.
       * @param value Numeric enumeration value to look up.
       * @return Stored description, or an empty string if `pEnumeration` is null or the value is absent.
       */
      std::string GetEnumerationDescription(const SEntityEnumeration* pEnumeration, int value) const;

    private:
      std::unordered_map<SisoEnumSetID, std::unique_ptr<SEntityEnumeration>, StrongTypeHash<SisoEnumSetID>> m_EntityEnums;///< Owned enumerations keyed by SISO set identifier.
      std::map<SEntityKindDomainCountry, std::unique_ptr<CEntityType>> m_EntityTypes;///< Owned hierarchies keyed by kind, domain, and country.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
