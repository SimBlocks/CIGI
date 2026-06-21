//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityType.h
 * @brief Declares entity type structures and the CEntityType class for entity classification.
 *
 * Provides structures for entity categories, subcategories, and specifics, as well as the CEntityType class
 * for managing the category hierarchy associated with a fixed `(kind, domain, country)` entity classification.
 *
 * @see sbio::entity::SEntityCategory
 * @see sbio::entity::SEntitySubCategory
 * @see sbio::entity::SEntitySpecific
 * @see sbio::entity::CEntityType
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_TYPE_H
#define SIMBLOCKS_ENTITY_TYPE_H

#include "GlobalHeaders/CommonTypes.h"
#include "EntityTypes.h"
#include <memory>
#include <unordered_map>

namespace sbio
{
  namespace entity
  {
    /**
     * @brief Leaf node in the loaded entity-type hierarchy.
     *
     * Invariants:
     * - `entitySpecificID` identifies the specific entry.
     * - `sDescription` stores the textual description parsed for that entry.
     */
    struct SEntitySpecific
    {
      sbio::SisoEntitySpecificID entitySpecificID = UnknownSisoEntitySpecificID;///< Specific ID
      std::string sDescription;///< Description of the specific entity
    };

    /**
     * @brief Groups entity specifics under a subcategory identifier.
     *
     * Ownership:
     * - `specifics` owns its `SEntitySpecific` children through `std::unique_ptr`.
     */
    struct SEntitySubCategory
    {
      sbio::SisoEntitySubCategoryID entitySubCategoryID = UnknownSisoEntitySubCategoryID;///< Subcategory ID
      std::string sDescription;///< Description of the subcategory
      std::unordered_map<sbio::SisoEntitySpecificID, std::unique_ptr<SEntitySpecific>, StrongTypeHash<sbio::SisoEntitySpecificID>> specifics;///< Map of specifics
    };

    /**
     * @brief Groups entity subcategories under a category identifier.
     *
     * Ownership:
     * - `subCategories` owns its `SEntitySubCategory` children through `std::unique_ptr`.
     */
    struct SEntityCategory
    {
      sbio::SisoEntityCategoryID entityCategoryID = UnknownSisoEntityCategoryID;///< Category ID
      std::string sDescription;///< Description of the category
      std::unordered_map<sbio::SisoEntitySubCategoryID, std::unique_ptr<SEntitySubCategory>, StrongTypeHash<sbio::SisoEntitySubCategoryID>> subCategories;///< Map of subcategories
    };

    /**
     * @brief Owns the category hierarchy for one `(kind, domain, country)` entity-type grouping.
     *
     * Invariants:
     * - The kind, domain, and country identifiers are fixed at construction time.
     *
     * Ownership:
     * - Categories are owned by this object through `std::unique_ptr`.
     * - Pointers returned by `GetCategory()` are non-owning and become invalid when the category is replaced
     *   or when this object is destroyed.
     */
    class CEntityType
    {
    public:
      /**
       * @brief Constructs an entity-type grouping for a fixed kind/domain/country triple.
       *
       * @param entityKindID Entity kind identifier.
       * @param entityDomainID Entity domain identifier.
       * @param entityCountryID Entity country identifier.
       *
       * Side effects:
       * - Stores the supplied identifiers for the lifetime of the object.
       */
      CEntityType(sbio::SisoEntityKindID entityKindID, sbio::SisoEntityDomainID entityDomainID, sbio::SisoEntityCountryID entityCountryID);

      /**
       * @brief Destructor.
       *
       * Side effects:
       * - Destroys all owned categories.
       */
      ~CEntityType();

      /**
       * @brief Adds or replaces a category in this entity-type grouping.
       *
       * @param pEntityCategory Unique pointer whose ownership is transferred to this object.
       *
       * Side effects:
       * - Stores the category under `pEntityCategory->entityCategoryID`.
       * - Replaces any existing category with the same identifier.
       *
       * Preconditions:
       * - `pEntityCategory` must not be null.
       */
      void AddCategory(std::unique_ptr<SEntityCategory>& pEntityCategory);

      /**
       * @brief Gets a category by identifier.
       *
       * @param entityCategoryID Category identifier to look up.
       * @return Non-owning pointer to the matching category, or `nullptr` when no category exists for that identifier.
       */
      SEntityCategory* GetCategory(sbio::SisoEntityCategoryID entityCategoryID) const;

    private:
      sbio::SisoEntityKindID m_entityKindID = UnknownSisoEntityKindID;///< Kind ID
      sbio::SisoEntityDomainID m_entityDomainID = UnknownSisoEntityDomainID;///< Domain ID
      sbio::SisoEntityCountryID m_entityCountryID = UnknownSisoEntityCountryID;///< Country ID

      std::unordered_map<sbio::SisoEntityCategoryID, std::unique_ptr<SEntityCategory>, StrongTypeHash<sbio::SisoEntityCategoryID>> m_EntityCategories;///< Map of categories
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
