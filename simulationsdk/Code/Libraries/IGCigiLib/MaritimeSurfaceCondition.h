//Copyright SimBlocks LLC 2016-2026
/**
 * @file MaritimeSurfaceCondition.h
 * @brief Declares the CCigiMaritimeSurfaceCondition class for SimBlocks CIGI IG maritime surface condition management.
 *
 * Provides the CCigiMaritimeSurfaceCondition class for managing maritime surface conditions in the SimBlocks CIGI IG library, including active state and condition data.
 * Integrates with SimBlocks CIGI types for simulation and maritime surface control.
 *
 * @see sbio::cigi::ig::CCigiMaritimeSurfaceCondition
 * @see SCigiMaritimeSurfaceCondition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_MARITIME_SURFACE_CONDITION_H
#define SIMBLOCKS_CIGI_MARITIME_SURFACE_CONDITION_H

#include "CigiLib/CigiTypesHostToIG.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Maritime surface condition class for SimBlocks CIGI IG integration.
       *
       * Manages maritime surface condition state and data for simulation.
       */
      class CCigiMaritimeSurfaceCondition
      {
      public:
        /**
         * @brief Constructs an inactive condition container.
         */
        CCigiMaritimeSurfaceCondition();
        /**
         * @brief Destroys the container.
         */
        ~CCigiMaritimeSurfaceCondition();

        /**
         * @brief Sets whether the stored condition is active.
         * @param val `true` to mark the condition active; otherwise `false`.
         */
        void setActive(bool val);
        /**
         * @brief Reports whether the stored condition is active.
         * @return `true` when the stored condition should contribute to queries; otherwise `false`.
         */
        bool IsActive() const;

        /**
         * @brief Replaces the stored maritime-surface condition.
         * @param condition Condition value to store.
         */
        void SetCondition(SCigiMaritimeSurfaceCondition condition);
        /**
         * @brief Returns the stored maritime-surface condition.
         * @return A copy of the stored condition value.
         */
        SCigiMaritimeSurfaceCondition GetCondition() const;

      private:
        bool m_bActive = false;///< Indicates whether the stored condition currently contributes to queries.
        SCigiMaritimeSurfaceCondition m_Condition;///< Stored maritime-surface condition.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
