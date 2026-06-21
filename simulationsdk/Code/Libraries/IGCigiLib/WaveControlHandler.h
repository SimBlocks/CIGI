//Copyright SimBlocks LLC 2016-2026
/**
 * @file WaveControlHandler.h
 * @brief Declares the CCigiWaveControlHandler class for handling CIGI wave condition packets in SimBlocks CIGI IG library.
 *
 * Provides the CCigiWaveControlHandler class for processing wave condition packets and updating environmental region handler state in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI environmental region handler for simulation and wave management.
 *
 * @see sbio::cigi::ig::CCigiWaveControlHandler
 * @see sbio::cigi::ig::CCigiEnvironmentalRegionHandler
 * @see SCigiWaveCondition
 * @see ECigiScope
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_WAVE_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_WAVE_CONTROL_HANDLER_H

#include "IGCigiLib/IGCigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Routes parsed wave-condition packets into the environmental-region model.
       */
      class CCigiWaveControlHandler
      {
      public:
        /**
         * @brief Constructs a wave-control handler.
         * @param pEnvironmentalRegionHandler Non-owning handler that receives wave updates.
         */
        CCigiWaveControlHandler(CCigiEnvironmentalRegionHandler* pEnvironmentalRegionHandler);

        /**
         * @brief Applies one wave-condition packet.
         * @param waveCondition Parsed wave condition state.
         * @param eScope Scope that determines whether the target is global, regional, or entity-scoped.
         * @param nID Scope-specific entity or region identifier.
         */
        void Handle(const SCigiWaveCondition& waveCondition, ECigiScope eScope, int nID);

      private:
        CCigiEnvironmentalRegionHandler* m_pEnvironmentalRegionHandler = nullptr;///< Non-owning target for forwarded wave updates.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
