//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityManager.h
 * @brief Declares the CEntityManager class for managing entities and articulated parts in the simulation.
 *
 * The CEntityManager class owns entity and articulated-part instances, provides lookup by identifier,
 * coordinates per-frame updates, and exposes entity-type description lookups backed by loaded SISO
 * enumeration metadata.
 *
 * @see sbio::entity::CEntity
 * @see sbio::entity::CArticulatedPart
 * @see sbio::entity::CEntityEnumerations
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_MANAGER_H
#define SIMBLOCKS_ENTITY_MANAGER_H

#include "EntityDeclarations.h"
#include "EntityTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include <list>
#include <memory>
#include <set>
#include <unordered_map>

namespace sbio
{
  namespace entity
  {
    /**
     * @brief Hash functor for `SEntityArticulatedPartKey`.
     *
     * The hash combines the entity identifier and articulated-part identifier into a single `std::size_t`
     * value suitable for the articulated-part lookup table used by `CEntityManager`.
     */
    struct SEntityArticulatedPartHash
    {
      /**
       * @brief Computes a hash value for an articulated-part key.
       *
       * @param entityArticulatedPartKey Key to hash.
       * @return Hash value derived from the key's entity and articulated-part identifiers.
       */
      std::size_t operator()(const SEntityArticulatedPartKey& entityArticulatedPartKey) const;
    };

    /**
     * @brief Owns and coordinates entities and articulated parts used by EntityLib.
     *
     * The manager stores entities in a map keyed by `EntityID` and articulated parts in a map keyed by the
     * `(EntityID, ArticulatedPartID)` pair. It also owns the enumeration metadata object used to resolve
     * human-readable entity-type descriptions.
     *
     * Threading:
     * - The class provides no internal synchronization.
     */
    class CEntityManager
    {
    public:
      /** @brief Entity instances owned by the manager, keyed by entity identifier. */
      typedef std::unordered_map<sbio::EntityID, std::unique_ptr<CEntity>, StrongTypeHash<sbio::EntityID>> TEntities;

      /**
       * @brief Constructs an empty entity manager.
       *
       * Enumeration metadata is not loaded until `Init()` is called.
       */
      CEntityManager();

      /**
       * @brief Destroys the manager and all objects it owns.
       *
       * Calls `Reset()`, including each stored entity's `Remove()` hook.
       */
      ~CEntityManager();

      /**
       * @brief Checks whether an entity is present in the manager.
       *
       * @param entityID Entity identifier to look up.
       * @return `true` when an entry exists for `entityID`; otherwise `false`.
       */
      bool HasEntity(sbio::EntityID entityID) const;

      /**
       * @brief Checks whether a human-readable description exists for a given entity type.
       *
       * @param entityType Entity type to resolve.
       * @return `true` when enumeration metadata is loaded and the type resolves to a non-empty description;
       *         otherwise `false`.
       *
       * A category or subcategory description is sufficient even if a deeper identifier is absent.
       * The extra identifier is ignored; this is not an exact full-type membership test.
       */
      bool HasEntityType(const SEntityType& entityType) const;

      /**
       * @brief Adds or replaces an entity entry.
       *
       * @param entityID Storage key; must match `pEntity->GetEntityID()` for insertion to succeed.
       * @param pEntity Entity whose ownership is transferred to this call.
       *
       * Null pointers and mismatched IDs are rejected without changing stored entries. A rejected
       * non-null entity is destroyed on return. For a valid replacement, calls `RemoveEntity()` first,
       * detaching managed children and removing associated articulated parts. Rejections and duplicate
       * IDs are logged when a logger is available.
       */
      void AddEntity(sbio::EntityID entityID, std::unique_ptr<CEntity> pEntity);

      /**
       * @brief Adds or replaces an articulated part entry.
       *
       * @param pArticulatedPart Articulated part whose ownership is transferred to the manager.
       *
       * Uses the part's entity and articulated-part IDs as the key. Replacing an entry destroys the
       * previous part. Does not require the owning entity to be present in this manager.
       *
       * Preconditions:
       * - `pArticulatedPart` must not be null.
       */
      void AddArticulatedPart(std::unique_ptr<CArticulatedPart> pArticulatedPart);

      /**
       * @brief Checks whether an articulated part is present for a given entity and part identifier.
       *
       * @param entityID Entity identifier.
       * @param articulatedPartID Articulated-part identifier.
       * @return `true` when a matching articulated part exists; otherwise `false`.
       */
      bool HasArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID) const;

      /**
       * @brief Gets an articulated part by entity and part identifier.
       *
       * @param entityID Entity identifier.
       * @param articulatedPartID Articulated-part identifier.
       * @return Non-owning pointer to the matching articulated part, or `nullptr` when not found.
       *         Remains valid until the part is replaced, removed, or the manager is reset or destroyed.
       */
      CArticulatedPart* GetArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID) const;

      /**
       * @brief Gets an entity by identifier.
       *
       * @param entityID Entity identifier.
       * @return Non-owning pointer to the matching entity, or `nullptr` when not found or when a null pointer was stored.
       *         Remains valid until the entity is replaced, removed, or the manager is reset or destroyed.
       */
      CEntity* GetEntity(sbio::EntityID entityID) const;

      /**
       * @brief Replaces the enumeration metadata object and loads `SISO-REF-010.xml`.
       *
       * Reads from `g_EntityLibSettings.dataPath`. A load failure leaves the new metadata object empty
       * and logs a warning when a logger is available. Stored entities and articulated parts are unchanged.
       */
      void Init();

      /**
       * @brief Looks up the human-readable description for an entity type.
       *
       * @param entityType Entity type to resolve.
       * @return Space-separated descriptions from matching category, subcategory, and specific levels,
       *         or an empty string when metadata, the kind/domain/country grouping, or the category is absent.
       *
       * Stops at the first missing hierarchy level, retaining descriptions already found.
       * The extra identifier is ignored.
       */
      std::string Lookup(const SEntityType& entityType);

      /**
       * @brief Removes an entity and any articulated parts associated with that entity.
       * @param entityID Entity identifier to remove.
       *
       * If the entity exists, detaches its direct children stored in this manager, preserving their
       * world transforms, then calls the entity's `Remove()` hook and destroys it. Associated parts
       * are removed even if the entity is absent. Children outside this manager are not detached.
       */
      void RemoveEntity(sbio::EntityID entityID);

      /**
       * @brief Removes all stored entities and articulated parts.
       *
       * Calls `Remove()` on each entity before clearing the containers. Does not detach hierarchy
       * relationships individually and does not clear enumeration metadata.
       */
      void Reset();

      /**
       * @brief Updates all managed entities, then all articulated parts.
       *
       * @param bInterpolationEnabled Interpolation flag forwarded to `UpdateEntities()`.
       * @param deltaTime Elapsed simulation time, in seconds.
       */
      void Update(bool bInterpolationEnabled, double deltaTime);

      /**
       * @brief Updates all stored articulated parts.
       * @param fDeltaTime Elapsed simulation time, in seconds.
       *
       * Calls each part's `Update()` without checking its enabled state.
       */
      void UpdateArticulatedParts(double fDeltaTime);

      /**
       * @brief Updates all stored entities.
       *
       * @param bInterpolationEnabled Global switch; interpolation is enabled during an update only if this and the
       * entity's own interpolation setting are both true.
       * @param deltaTime Elapsed simulation time, in seconds.
       *
       * Temporarily masks each entity's interpolation setting, calls `Update()`, and restores the original setting
       * even if the update throws. Update exceptions propagate. All entities are updated regardless of interpolation
       * state, and no parent-before-child ordering is imposed on traversal of the unordered map.
       */
      void UpdateEntities(bool bInterpolationEnabled, double deltaTime);

      /**
       * @brief Exposes the manager's entity container.
       * @return Const reference to the internal entity map, valid for this manager's lifetime.
       *         Ownership remains with the manager; the contained entity objects are still mutable.
       */
      const TEntities& GetEntities() const;

    protected:
      TEntities m_Entities;///< Owned entities keyed by entity identifier.
      /** @brief Articulated parts owned by the manager, keyed by entity and part identifiers. */
      typedef std::unordered_map<SEntityArticulatedPartKey, std::unique_ptr<CArticulatedPart>, SEntityArticulatedPartHash> TArticulatedParts;

      TArticulatedParts m_ArticulatedParts;///< Owned articulated parts.

      std::unique_ptr<CEntityEnumerations> m_pEntityEnumerations;///< Owned metadata; null until `Init()`.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
