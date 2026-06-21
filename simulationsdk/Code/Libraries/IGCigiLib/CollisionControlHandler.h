//Copyright SimBlocks LLC 2016-2026
/**
 * @file CollisionControlHandler.h
 * @brief Declares the CCigiCollisionControlHandler class for handling CIGI collision control messages and definitions.
 *
 * Provides the CCigiCollisionControlHandler class for processing collision detection segment, cuboid, and sphere definitions in the SimBlocks CIGI IG library.
 * Utilizes tuple hashing and unordered maps for efficient collision volume and segment management.
 * Integrates with SimBlocks CIGI and common types for simulation and collision management.
 *
 * @see sbio::cigi::ig::CCigiCollisionControlHandler
 * @see sbio::cigi::SCollisionDetectionSegmentDefinition
 * @see sbio::cigi::SCollisionDetectionCuboidDefinition
 * @see sbio::cigi::SCollisionDetectionSphereDefinition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_COLLISION_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_COLLISION_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "GlobalHeaders/CommonTypes.h"
#include "CigiEvent.h"
#include <tuple>
#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Owns the collision-definition state derived from CIGI collision packets.
       */
      class CCigiCollisionControlHandler : public sbio::cigi::ig::IIGCIGIEventListener
      {
        /**
         * @brief Hashes `(EntityID, VolumeID)` tuples used as collision-volume keys.
         */
        struct VolumeTupleHash
        {
          inline size_t operator()(std::tuple<sbio::EntityID, sbio::VolumeID> tuple) const
          {
            return std::get<0>(tuple).Value() ^ std::get<1>(tuple).Value();
          };
        };

        /**
         * @brief Hashes `(EntityID, SegmentID)` tuples used as collision-segment keys.
         */
        struct SegmentTupleHash
        {
          inline size_t operator()(std::tuple<sbio::EntityID, sbio::SegmentID> tuple) const
          {
            return std::get<0>(tuple).Value() ^ std::get<1>(tuple).Value();
          };
        };

      public:
        /**
         * @brief Constructs a collision control handler.
         */
        CCigiCollisionControlHandler();
        /**
         * @brief Destroys the collision control handler.
         */
        ~CCigiCollisionControlHandler();

        virtual void OnDatabaseLoadedEvent() override {};
        virtual void OnDatabaseLoadingFailedEvent() override {};
        virtual void OnEntityRemoved(sbio::EntityID entityID) override;
        virtual void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) override {};

        /**
         * @brief Resets all cached collision state.
         */
        void Reset();
        /**
         * @brief Removes cached collision state for one entity.
         * @param entityID Entity whose cached segment and volume definitions should be removed.
         */
        void RemoveEntityCollisionState(sbio::EntityID entityID);

        /**
         * @brief Handles collision detection segment definition messages.
         * @param collisionDetectionSegmentDefinition Segment definition data.
         */
        void HandleCigiCollisionDetectionSegmentDefinition(const sbio::cigi::SCollisionDetectionSegmentDefinition& collisionDetectionSegmentDefinition);
        /**
         * @brief Handles collision cuboid definition messages.
         * @param collisionCuboidDefinition Cuboid definition data.
         */
        void HandleCigiCollisionCuboidDefinition(const sbio::cigi::SCollisionDetectionCuboidDefinition& collisionCuboidDefinition);
        /**
         * @brief Handles collision sphere definition messages.
         * @param collisionSphereDefinition Sphere definition data.
         */
        void HandleCigiCollisionSphereDefinition(const sbio::cigi::SCollisionDetectionSphereDefinition& collisionSphereDefinition);

      protected:
        /**
         * @brief Creates a new collision cuboid volume.
         * @param collisionVolumeDefinition Cuboid volume definition data.
         * @param pair Tuple of entity and volume IDs.
         */
        void CreateNewCollisionCuboidVolume(const sbio::cigi::SCollisionDetectionCuboidDefinition& collisionVolumeDefinition, std::tuple<sbio::EntityID, sbio::VolumeID> pair);
        /**
         * @brief Creates a new collision sphere volume.
         * @param collisionVolumeDefinition Sphere volume definition data.
         * @param pair Tuple of entity and volume IDs.
         */
        void CreateNewCollisionSphereVolume(const sbio::cigi::SCollisionDetectionSphereDefinition& collisionVolumeDefinition, std::tuple<sbio::EntityID, sbio::VolumeID> pair);

      private:
        std::unordered_map<std::tuple<sbio::EntityID, sbio::VolumeID>, sbio::cigi::SCollisionDetectionCuboidDefinition, VolumeTupleHash> m_CollisionCuboidDefinitions;///< Latest cuboid definitions keyed by entity and volume identifier.
        std::unordered_map<std::tuple<sbio::EntityID, sbio::VolumeID>, sbio::cigi::SCollisionDetectionSphereDefinition, VolumeTupleHash> m_CollisionSphereDefinitions;///< Latest sphere definitions keyed by entity and volume identifier.
        std::unordered_map<std::tuple<sbio::EntityID, sbio::SegmentID>, sbio::cigi::SCollisionDetectionSegmentDefinition, SegmentTupleHash> m_CollisionSegmentDefinitions;///< Latest segment definitions keyed by entity and segment identifier.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
