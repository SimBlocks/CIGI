//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiEntityTypes.h
 * @brief Declares the CCigiEntityTypes class for CIGI-to-SISO entity type conversion and lookup.
 *
 * Provides the CCigiEntityTypes class for loading, converting, and querying entity types between CIGI protocol
 * and SISO standards. Supports CSV-based conversion file loading, entity type lookup, and existence checks for
 * simulation interoperability. Integrates with entity and engine type systems.
 *
 * @see sbio::cigi::CCigiEntityTypes
 * @see sbio::entity::SEntityType
 * @see sbio::ShortEntityTypeID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ENTITY_TYPES_H
#define SIMBLOCKS_CIGI_ENTITY_TYPES_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "EngineLib/EngineTypeDeclarations.h"
#include "EntityLib/EntityDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include <unordered_map>
#include <filesystem>

namespace sbio
{
  namespace cigi
  {
    /**
     * @brief Handles lookup between CIGI short entity type identifiers and SISO entity type enumerations.
     *
     * The conversion table is populated from a CSV file and then queried in both directions.
     * Entries are stored in-memory for the lifetime of the object. Missing mappings are tolerated and
     * unresolved lookups return default-initialized values rather than throwing.
     * This class is intended to be shared by higher-level host and IG components that need a consistent
     * entity-type translation policy.
     *
     * @invariant `m_CigiEntityTypes` contains at most one SISO entity type per CIGI short entity type.
     * @invariant All stored mappings originate from the most recent successful parse performed by
     * `LoadCigiToSisoConversionFileCsv` after the map was cleared.
     * @ownership The class owns its internal lookup table and does not retain ownership of external file or stream objects.
     */
    class CCigiEntityTypes
    {
    public:
      /**
       * @brief Constructs an empty CIGI entity type conversion table.
       *
       * @post No mappings are loaded.
       */
      CCigiEntityTypes();

      /**
       * @brief Destroys the conversion table.
       *
       * @sideeffects Releases all in-memory mappings owned by this instance.
       */
      ~CCigiEntityTypes();

      /**
       * @brief Loads CIGI-to-SISO entity type mappings from a CSV file.
       * @param filePath Path to the conversion file to parse.
       *
       * Each non-empty line is parsed as a short CIGI entity type followed by up to seven SISO entity type
       * components. Invalid or incomplete SISO component tokens on an otherwise valid line are treated as zero.
       * If the file cannot be opened, the table remains empty.
       *
       * @sideeffects Clears any previously loaded mappings before attempting to read `filePath`.
       * @sideeffects Writes status text to standard output.
       * @failurecases If `filePath` cannot be opened, no mappings are loaded.
       * @failurecases Lines whose first token cannot be parsed as a CIGI entity type are skipped.
       */
      void LoadCigiToSisoConversionFileCsv(const std::filesystem::path& filePath);

      /**
       * @brief Gets the SISO entity type mapped from a CIGI short entity type identifier.
       * @param entityTypeID CIGI short entity type identifier to resolve.
       * @return The mapped SISO entity type when a mapping exists; otherwise a default-initialized `sbio::entity::SEntityType`.
       *
       * @sideeffects None.
       * @failurecases Missing mappings are reported by returning a default-initialized value.
       */
      sbio::entity::SEntityType GetExtendedEntityType(sbio::ShortEntityTypeID entityTypeID);

      /**
       * @brief Gets the CIGI short entity type identifier associated with a SISO entity type.
       * @param entityType SISO entity type to search for.
       * @return The first matching CIGI short entity type identifier found in the loaded table; otherwise `ShortEntityTypeID(0)`.
       *
       * @sideeffects None.
       * @failurecases If no mapping matches `entityType`, the function returns `ShortEntityTypeID(0)`.
       */
      sbio::ShortEntityTypeID GetShortEntityType(const sbio::entity::SEntityType& entityType);

      /**
       * @brief Checks whether a CIGI short entity type identifier is present in the loaded table.
       * @param entityTypeID CIGI short entity type identifier to test.
       * @return `true` when a mapping exists for `entityTypeID`; otherwise `false`.
       *
       * @sideeffects None.
       */
      bool IfCigiEntityTypeExists(ShortEntityTypeID entityTypeID) const;

    private:
      typedef std::unordered_map<ShortEntityTypeID, sbio::entity::SEntityType, StrongTypeHash<ShortEntityTypeID>> TCigiToSisoEntityType;
      TCigiToSisoEntityType m_CigiEntityTypes;///< Map of CIGI short entity type IDs to SISO entity types
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
