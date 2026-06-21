//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityControlHandler.h
 * @brief Declares the CCigiEntityControlHandler class for SimBlocks CIGI IG entity control and position management.
 *
 * Provides the CCigiEntityControlHandler class for handling entity control, position requests, and transformation setup in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, entity, math, view, and common types for simulation and entity management.
 * Supports continuous position requests, hierarchical entity relationships, and terrain handler integration.
 *
 * @see sbio::cigi::ig::CCigiEntityControlHandler
 * @see sbio::cigi::ig::CCigiEntity
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::ig::CCigiTerrainHandler
 * @see sbio::math::TCigiBodyTransform
 * @see sbio::cigi::SCigiComponentKey
 * @see sbio::cigi::SCigiComponentControlState
 * @see sbio::cigi::SCigiAnimationControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ENTITY_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_ENTITY_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "EntityLib/EntityTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "MathLib/MathTypes.h"
#include "ViewLib/ViewTypes.h"
#include "CigiLib/CigiTypes.h"
#include <string>
#include <unordered_map>
#include <set>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Handles CIGI entity control and position management.
       */
      class CCigiEntityControlHandler
      {
      public:
        /**
         * @brief Constructs an entity control handler.
         * @param ImageGenerator Reference to the image generator.
         * @param pTerrainHandler Pointer to the terrain handler.
         */
        CCigiEntityControlHandler(CCigiImageGenerator& ImageGenerator, CCigiTerrainHandler* pTerrainHandler);
        /**
         * @brief Destroys the entity control handler.
         */
        ~CCigiEntityControlHandler();

        /**
         * @brief Handles conformal clamped entity position messages.
         * @param conformalClampedEntityPosition Conformal clamped entity position data.
         */
        void HandleCigiConformalClampedEntityPosition(const SCigiConformalClampedEntityPosition& conformalClampedEntityPosition);
        /**
         * @brief Handles entity control messages.
         * @param entityControl Entity control data.
         * @param bBlankEntity Flag for blank entity.
         * @return True if handled successfully.
         */
        bool HandleCigiEntityControl(const SEntityControl& entityControl, bool bBlankEntity);
        /**
         * @brief Sets up child entity transformation.
         * @param parentID Parent entity ID.
         * @param offset Body coordinates offset.
         * @param rotation Euler rotation.
         * @return Child entity transform.
         */
        sbio::cigi::TCigiBodyTransform SetupChildEntityTransformation(EntityID parentID, const sbio::cigi::CigiBodyCoordinates& offset, const sbio::cigi::TCigiBodyEulerRotation& rotation);

        /**
         * @brief Handles entity position messages.
         * @param entityID Entity ID.
         * @param parentID Parent entity ID.
         * @param attachState Attach state.
         * @param eGrndClamp Ground clamp type.
         * @param position Position vector.
         * @param rotation Euler rotation.
         */
        void HandleCigiEntityPosition(EntityID entityID, EntityID parentID, sbio::EAttachState attachState, EClamp eGrndClamp, const sbio::math::Vec3& position, const sbio::cigi::TCigiBodyEulerRotation& rotation);
        /**
         * @brief Handles position request messages.
         * @param positionRequest Position request data.
         */
        void HandleCigiPositionRequest(const sbio::cigi::SPositionRequest& positionRequest);
        /**
         * @brief Requests position for an entity.
         * @param pCigiEntity Pointer to the entity.
         */
        void RequestEntityPosition(CCigiEntity* pCigiEntity);
        /**
         * @brief Requests position for a child entity.
         * @param pCigiEntity Pointer to the child entity.
         */
        void RequestChildEntityPosition(CCigiEntity* pCigiEntity);
        /**
         * @brief Requests position for an articulated part.
         * @param articulatedPartKey Articulated part key.
         */
        void RequestArticulatedPartPosition(sbio::entity::SEntityArticulatedPartKey articulatedPartKey);
        /**
         * @brief Requests geodetic position for a view.
         * @param viewID View ID.
         */
        void RequestGeodeticViewPosition(sbio::ViewID viewID);
        /**
         * @brief Requests local position for a view.
         * @param viewID View ID.
         */
        void RequestLocalViewPosition(sbio::ViewID viewID);
        /**
         * @brief Requests geodetic position for a view group.
         * @param viewGroupID View group ID.
         */
        void RequestGeodeticViewGroupPosition(sbio::ViewGroupID viewGroupID);
        /**
         * @brief Requests local position for a view group.
         * @param viewGroupID View group ID.
         */
        void RequestLocalViewGroupPosition(sbio::ViewGroupID viewGroupID);
        /**
         * @brief Requests position for a motion tracker.
         * @param motionTrackerID Motion tracker ID.
         */
        void RequestMotionTrackerPosition(sbio::MotionTrackerID motionTrackerID);

        /**
         * @brief Updates the entity control handler state.
         */
        void Update();

      private:
        /**
         * @brief Handles top-level entity position messages.
         * @param pCigiEntity Pointer to the entity.
         * @param topLevelEntityPosition Top-level entity position data.
         */
        void HandleCigiTopLevelEntityPosition(CCigiEntity* pCigiEntity, const STopLevelEntityPosition& topLevelEntityPosition);
        /**
         * @brief Handles child entity position messages.
         * @param pCigiEntity Pointer to the child entity.
         * @param childEntityPosition Child entity position data.
         */
        void HandleCigiChildEntityPosition(CCigiEntity* pCigiEntity, const SChildEntityPosition& childEntityPosition);

        std::set<EntityID> m_ContinuousEntityIDPositionRequests;///< Continuous entity position requests
        std::set<EntityID> m_ContinuousChildEntityIDPositionRequests;///< Continuous child entity position requests
        std::set<sbio::entity::SEntityArticulatedPartKey> m_ContinuousArticulatedPartIDPositionRequests;///< Continuous articulated part position requests
        std::set<ViewID> m_ContinuousGeodeticViewIDPositionRequests;///< Continuous geodetic view position requests
        std::set<ViewID> m_ContinuousLocalViewIDPositionRequests;///< Continuous local view position requests
        std::set<ViewGroupID> m_ContinuousLocalViewGroupIDPositionRequests;///< Continuous local view group position requests
        std::set<ViewGroupID> m_ContinuousGeodeticViewGroupIDPositionRequests;///< Continuous geodetic view group position requests
        std::set<MotionTrackerID> m_ContinuousMotionTrackerPositionRequests;///< Continuous motion tracker position requests

        CCigiImageGenerator& m_ImageGenerator;///< Non-owning image generator used to create responses and resolve state.
        CCigiTerrainHandler* m_pTerrainHandler;///< Non-owning terrain handler used for terrain-related positioning work.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
