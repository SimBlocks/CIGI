//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiEntityAnimation.h
 * @brief Declares the CCigiEntityAnimation struct for SimBlocks CIGI IG entity animation management.
 *
 * Provides the CCigiEntityAnimation struct for managing entity animation state, speed, direction, and loop mode in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, entity, and math types for simulation and animation control.
 *
 * @see sbio::cigi::ig::CCigiEntityAnimation
 * @see sbio::EntityID
 * @see sbio::AnimationID
 * @see sbio::cigi::EAnimationLoopMode
 * @see sbio::cigi::EAnimationState
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ENTITY_ANIMATION_H
#define SIMBLOCKS_CIGI_ENTITY_ANIMATION_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "CigiLib/CigiTypes.h"
#include "EntityLib/EntityDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Entity animation struct for SimBlocks CIGI IG integration.
       *
       * Manages animation state, speed, direction, and loop mode for simulation entities.
       */
      struct CCigiEntityAnimation
      {
        /**
         * @brief Constructs animation state for one entity animation channel.
         * @param entityID Entity that owns the animation.
         * @param animationID Animation channel identifier within the entity.
         */
        CCigiEntityAnimation(sbio::EntityID entityID, sbio::AnimationID animationID);
        /**
         * @brief Destroys the animation state object.
         */
        ~CCigiEntityAnimation();

        /**
         * @brief Returns the cached animation loop mode.
         * @return Current loop mode.
         */
        sbio::cigi::EAnimationLoopMode GetAnimationLoopMode() const;
        /**
         * @brief Returns the cached animation speed.
         * @return Current speed scalar.
         */
        float GetAnimationSpeed() const;
        /**
         * @brief Returns the cached animation state.
         * @return Current animation state.
         */
        sbio::cigi::EAnimationState GetAnimationState() const;

        /**
         * @brief Sets the cached animation direction.
         * @param bForward `true` for forward playback; `false` for reverse playback.
         * @param bForce When `true`, forces the implementation to emit an update even if the value is unchanged.
         */
        void SetAnimationDirection(bool bForward, bool bForce);
        /**
         * @brief Sets the cached animation loop mode.
         * @param eAnimationLoopMode Loop mode to store.
         * @param bForce When `true`, forces the implementation to emit an update even if the value is unchanged.
         */
        void SetAnimationLoopMode(sbio::cigi::EAnimationLoopMode eAnimationLoopMode, bool bForce);
        /**
         * @brief Sets the cached animation speed.
         * @param fAnimationSpeed Speed scalar to store.
         * @param bForce When `true`, forces the implementation to emit an update even if the value is unchanged.
         */
        void SetAnimationSpeed(float fAnimationSpeed, bool bForce);
        /**
         * @brief Sets the cached animation state.
         * @param eAnimationState Animation state to store.
         * @param bForce When `true`, forces the implementation to emit an update even if the value is unchanged.
         */
        void SetAnimationState(sbio::cigi::EAnimationState eAnimationState, bool bForce);

      private:
        sbio::EntityID m_EntityID = UnknownEntityID;///< Entity that owns this animation channel.
        sbio::AnimationID m_AnimationID = UnknownAnimationID;///< Animation channel identifier.
        sbio::cigi::EAnimationLoopMode m_AnimationLoopMode = sbio::cigi::EAnimationLoopMode::UNKNOWN;///< Cached loop mode.
        float m_AnimationSpeed = 0;///< Cached speed scalar.
        sbio::cigi::EAnimationState m_AnimationState = sbio::cigi::EAnimationState::UNKNOWN;///< Cached animation state.
        bool m_bAnimationDirectionForward = false;///< Cached playback direction flag.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
