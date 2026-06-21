//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityDeclarations.h
 * @brief Forward declarations and type definitions for entity-related types in the sbio namespace.
 *
 * Provides forward declarations for entity IDs, enums, and core classes used throughout the entity system.
 * This header reduces dependencies and improves compile times by avoiding unnecessary includes in files
 * that only require type information for entities, articulated parts, and related types.
 *
 * @see sbio::entity namespace
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_DECLARATIONS_H
#define SIMBLOCKS_ENTITY_DECLARATIONS_H

namespace sbio
{
  struct ShortEntityTypeID;
  struct SisoEnumSetID;
  struct SisoEntityKindID;
  struct SisoEntityDomainID;
  struct SisoEntityCountryID;
  struct SisoEntityCategoryID;
  struct SisoEntitySubCategoryID;
  struct SisoEntitySpecificID;
  struct SisoEntityExtraID;

  enum class EClamp;
  enum class EObjectCoordinateSystem;
  enum class EAttachState;

  namespace entity
  {
    struct SEntityType;
    struct SEntityArticulatedPartKey;

    class CArticulatedPart;
    class CEntity;
    class CEntityEnumerations;
    class CEntityManager;
    class CEntityType;
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
