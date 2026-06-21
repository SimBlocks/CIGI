//Copyright SimBlocks LLC 2016-2026
/**
 * @file ArticulatedPartHandler.h
 * @brief Declares the CCigiArticulatedPartHandler class for handling CIGI articulated part messages.
 *
 * Provides the CCigiArticulatedPartHandler class for processing articulated part and short articulated part messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, entity, math, and common types for simulation and articulated part management.
 *
 * @see sbio::cigi::ig::CCigiArticulatedPartHandler
 * @see sbio::cigi::SCigiArticulatedPart
 * @see sbio::cigi::SCigiShortArticulatedPart
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ARTICULATED_PART_HANDLER
#define SIMBLOCKS_CIGI_ARTICULATED_PART_HANDLER

#include "CigiLib/CigiTypeDeclarations.h"
#include "EntityLib/EntityDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "MathLib/MathTypes.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies articulated-part control packets to CIGI-managed entities.
       *
       * Responsibilities:
       * - Handles both full and short articulated-part packet forms.
       * - Updates the addressed articulated part's enabled state, offsets, and enabled degrees of freedom.
       *
       * Ownership:
       * - This type does not own entities or articulated parts.
       *
       * Invariants:
       * - The handler carries no persistent articulated-part state between calls.
       *
       * Failure cases:
       * - If the target entity or articulated part cannot be resolved, the requested update cannot be applied.
       *
       * Side effects:
       * - Successful calls mutate articulated-part state on objects managed elsewhere.
       */
      class CCigiArticulatedPartHandler
      {
      public:
        /**
         * @brief Constructs a stateless articulated-part handler.
         */
        CCigiArticulatedPartHandler();
        /**
         * @brief Destroys the handler.
         */
        ~CCigiArticulatedPartHandler();

        /**
         * @brief Applies a full articulated-part control update.
         * @param articulatedPart Parsed articulated-part state.
         */
        void Handle(const sbio::cigi::SCigiArticulatedPart& articulatedPart);
        /**
         * @brief Applies a short articulated-part control update.
         * @param shortArticulatedPart Parsed short articulated-part state.
         */
        void Handle(const sbio::cigi::SCigiShortArticulatedPart& shortArticulatedPart);

      private:
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
