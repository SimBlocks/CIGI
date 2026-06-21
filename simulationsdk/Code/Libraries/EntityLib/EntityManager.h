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
      typedef std::unordered_map<sbio::EntityID, std::unique_ptr<CEntity>, StrongTypeHash<sbio::EntityID>> TEntities;

      /**
       * @brief Constructs an empty entity manager.
       */
      CEntityManager();

      /**
       * @brief Destroys the manager and all objects it owns.
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
       */
      bool HasEntityType(const SEntityType& entityType) const;

      /**
       * @brief Adds or replaces an entity entry.
       *
       * @param entityID Key under which to store the entity.
       * @param pEntity Entity pointer to store.
       */
      void AddEntity(sbio::EntityID entityID, std::unique_ptr<CEntity> pEntity);

      /**
       * @brief Adds or replaces an articulated part entry.
       *
       * @param pArticulatedPart Articulated part pointer to store.
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
       */
      CArticulatedPart* GetArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID) const;

      /**
       * @brief Gets an entity by identifier.
       *
       * @param entityID Entity identifier.
       * @return Non-owning pointer to the matching entity, or `nullptr` when not found or when a null pointer was stored.
       */
      CEntity* GetEntity(sbio::EntityID entityID) const;

      /**
       * @brief Initializes entity-type enumeration support.
       */
      void Init();

      /**
       * @brief Looks up the human-readable description for an entity type.
       *
       * @param entityType Entity type to resolve.
       * @return Description string for the entity type, or an empty string when enumeration metadata is unavailable
       *         or the type is not found.
       */
      std::string Lookup(const SEntityType& entityType);

      /**
       * @brief Removes an entity and any articulated parts associated with that entity.
       * @param entityID Entity identifier to remove.
       */
      void RemoveEntity(sbio::EntityID entityID);

      /**
       * @brief Removes all stored entities and articulated parts.
       */
      void Reset();

      /**
       * @brief Updates all managed entities and articulated parts.
       *
       * @param bInterpolationEnabled Interpolation flag forwarded to `UpdateEntities()`.
       * @param deltaTime Elapsed simulation time, in seconds.
       */
      void Update(bool bInterpolationEnabled, double deltaTime);

      /**
       * @brief Updates all stored articulated parts.
       * @param fDeltaTime Elapsed simulation time, in seconds.
       */
      void UpdateArticulatedParts(double fDeltaTime);

      /**
       * @brief Updates all stored entities.
       *
       * @param bInterpolationEnabled Interpolation flag supplied by the caller.
       * @param deltaTime Elapsed simulation time, in seconds.
       *
       * Notes:
       * - The current implementation does not use `bInterpolationEnabled` directly.
       */
      void UpdateEntities(bool bInterpolationEnabled, double deltaTime);

      /**
       * @brief Exposes the manager's entity container.
       * @return Const reference to the internal entity map.
       */
      const TEntities& GetEntities() const;

    protected:
      /// <summary>
      /// Mapping between Entity IDs and Articulated Part IDs to Entity objects within EntityLib
      /// </summary>
      TEntities m_Entities;
      typedef std::unordered_map<SEntityArticulatedPartKey, std::unique_ptr<CArticulatedPart>, SEntityArticulatedPartHash> TArticulatedParts;

      /// <summary>
      /// Mapping between Articulated Parts on a specific Entity to an Articulated Part within EntityLib
      /// </summary>
      TArticulatedParts m_ArticulatedParts;

      std::unique_ptr<CEntityEnumerations> m_pEntityEnumerations;
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
