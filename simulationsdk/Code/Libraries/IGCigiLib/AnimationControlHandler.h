//Copyright SimBlocks LLC 2016-2026
/**
 * @file AnimationControlHandler.h
 * @brief Declares the CCigiAnimationControlHandler class for handling CIGI entity animation control.
 *
 * Provides the CCigiAnimationControlHandler class for processing animation control messages for entities in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI and common types for simulation and animation control.
 *
 * @see sbio::cigi::ig::CCigiAnimationControlHandler
 * @see SCigiAnimationControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ANIMATION_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_ANIMATION_CONTROL_HANDLER_H

#include "GlobalHeaders/CommonTypes.h"
#include "CigiLib/CigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies animation-control packets to CIGI-managed entities.
       *
       * Responsibilities:
       * - Interprets parsed animation-control state.
       * - Forwards animation state changes to the addressed entity animation objects.
       *
       * Ownership:
       * - This type owns no entities, animations, or global services.
       *
       * Invariants:
       * - The handler retains no persistent animation state.
       *
       * Failure cases:
       * - If the addressed entity or animation cannot be resolved, the update cannot take effect.
       *
       * Side effects:
       * - Successful calls may change animation play state, loop mode, alpha inheritance, or playback speed.
       */
      class CCigiAnimationControlHandler
      {
      public:
        /**
         * @brief Applies an animation-control update to the addressed entity animation.
         * @param animationControl Parsed animation-control state.
         */
        void HandleCigiEntityAnimation(const SCigiAnimationControl& animationControl);

      private:
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
