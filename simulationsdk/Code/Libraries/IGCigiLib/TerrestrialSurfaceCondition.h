//Copyright SimBlocks LLC 2016-2026
/**
 * @file TerrestrialSurfaceCondition.h
 * @brief Declares the CTerrestrialSurfaceCondition class for SimBlocks CIGI IG terrestrial surface condition management.
 *
 * Provides the CTerrestrialSurfaceCondition class for representing and managing terrestrial surface conditions in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI types for surface condition state, activation, and condition data management.
 * Supports activation state, condition setting and retrieval, and encapsulates SCigiTerrestrialSurfaceCondition data for simulation.
 *
 * @see sbio::cigi::ig::CTerrestrialSurfaceCondition
 * @see sbio::cigi::SCigiTerrestrialSurfaceCondition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TERRESTRIAL_SURFACE_CONDITION_H
#define SIMBLOCKS_CIGI_TERRESTRIAL_SURFACE_CONDITION_H

#include "CigiLib/CigiTypesHostToIG.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Stores one terrestrial-surface condition together with an explicit active flag.
       */
      class CTerrestrialSurfaceCondition
      {
      public:
        /**
         * @brief Constructs an inactive condition container.
         */
        CTerrestrialSurfaceCondition();
        /**
         * @brief Destroys the container.
         */
        ~CTerrestrialSurfaceCondition();

        /**
         * @brief Sets whether the stored condition is active.
         * @param value `true` to mark the condition active; otherwise `false`.
         */
        void SetActive(bool value);
        /**
         * @brief Reports whether the stored condition is active.
         * @return `true` when the stored condition should contribute to queries; otherwise `false`.
         */
        bool IsActive() const;

        /**
         * @brief Replaces the stored terrestrial-surface condition.
         * @param condition Condition value to store.
         */
        void SetCondition(const SCigiTerrestrialSurfaceCondition& condition);
        /**
         * @brief Returns the stored terrestrial-surface condition.
         * @return Reference to the stored condition value.
         */
        const SCigiTerrestrialSurfaceCondition& GetCondition() const;

      private:
        SCigiTerrestrialSurfaceCondition m_Condition;///< Stored terrestrial-surface condition.
        bool m_bActive = false;///< Indicates whether the stored condition currently contributes to queries.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
