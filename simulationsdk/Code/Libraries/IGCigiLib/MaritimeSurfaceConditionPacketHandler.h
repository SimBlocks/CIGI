//Copyright SimBlocks LLC 2016-2026
/**
 * @file MaritimeSurfaceConditionPacketHandler.h
 * @brief Declares the CCigiMaritimeSurfaceConditionPacketHandler class for handling CIGI maritime surface condition packets.
 *
 * Provides the CCigiMaritimeSurfaceConditionPacketHandler class for processing maritime surface condition packets and updating environmental region handler state in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI environmental region handler for simulation and maritime surface management.
 *
 * @see sbio::cigi::ig::CCigiMaritimeSurfaceConditionPacketHandler
 * @see sbio::cigi::ig::CCigiEnvironmentalRegionHandler
 * @see SCigiMaritimeSurfaceCondition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_MARITIME_SURFACE_CONDITION_PACKET_HANDLER_H
#define SIMBLOCKS_CIGI_MARITIME_SURFACE_CONDITION_PACKET_HANDLER_H

#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "EntityLib/EntityTypes.h"
#include "CigiLib/CigiTypes.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Routes parsed maritime-surface packets into the environmental-region model.
       */
      class CCigiMaritimeSurfaceConditionPacketHandler
      {
      public:
        /**
         * @brief Constructs a maritime-surface packet handler.
         * @param pEnvironmentalRegionHandler Non-owning handler that receives forwarded updates.
         */
        CCigiMaritimeSurfaceConditionPacketHandler(CCigiEnvironmentalRegionHandler* pEnvironmentalRegionHandler);

        /**
         * @brief Applies one maritime-surface condition packet.
         * @param maritimeSurfaceCondition Parsed maritime-surface condition state.
         * @param entityRegionID Scope-specific entity or region identifier.
         */
        void HandleEntityMartiimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition, sbio::EntityID entityID);
        void HandleRegionMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition, sbio::RegionID regionID);
        void HandleGlobalMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& maritimeSurfaceCondition);

      private:
        CCigiEnvironmentalRegionHandler* m_pEnvironmentalRegionHandler = nullptr;///< Non-owning target for forwarded maritime updates.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
