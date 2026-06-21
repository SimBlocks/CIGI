//Copyright SimBlocks LLC 2016-2026
/**
 * @file WaveLayer.h
 * @brief Declares the CCigiWaveLayer class for SimBlocks CIGI IG wave condition management.
 *
 * Provides the CCigiWaveLayer class for managing wave conditions in the SimBlocks CIGI IG library, including active state, breaker type, and condition data.
 * Integrates with SimBlocks CIGI types for simulation and wave control.
 *
 * @see sbio::cigi::ig::CCigiWaveLayer
 * @see SCigiWaveCondition
 * @see EWaveBreakerType
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_WAVE_LAYER_H
#define SIMBLOCKS_CIGI_WAVE_LAYER_H

#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Wave layer class for SimBlocks CIGI IG integration.
       *
       * Manages wave condition state, breaker type, and condition data for simulation.
       */
      class CCigiWaveLayer
      {
      public:
        /**
         * @brief Returns the stored wave condition.
         * @return Reference to the stored condition value.
         */
        const SCigiWaveCondition& GetCondition() const;
        /**
         * @brief Replaces the stored wave condition.
         * @param condition Condition value to store.
         */
        void SetCondition(const SCigiWaveCondition& condition);

      private:
        bool m_bActive = false;///< Cached active state for the stored wave condition.
        sbio::cigi::EWaveBreakerType m_eBreakerType = sbio::cigi::EWaveBreakerType::UNKNOWN;///< Cached breaker type for the stored wave condition.
        SCigiWaveCondition m_Condition;///< Stored wave condition.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
