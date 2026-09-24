//Copyright SimBlocks LLC 2016-2026
/**
 * @file EnvironmentalRegionHandler.h
 * @brief Declares the CCigiEnvironmentalRegionHandler class for SimBlocks CIGI IG environmental region management and querying.
 *
 * Provides the CCigiEnvironmentalRegionHandler class for managing, parsing, and querying environmental regions, weather, maritime, terrestrial, and wave conditions in the
 * SimBlocks IGCigiLib library. Integrates with SimBlocks CIGI, math, and engine types for simulation and environmental control. Supports spatial data structures for efficient
 * region queries and entity/global region management.
 *
 * @see sbio::cigi::ig::CCigiEnvironmentalRegionHandler
 * @see sbio::cigi::ig::CCigiEnvironmentalRegion
 * @see sbio::cigi::ig::CCigiRegionTree
 * @see SCigiEnvironmentalRegion
 * @see SCigiWeatherCondition
 * @see SCigiMaritimeSurfaceCondition
 * @see SCigiTerrestrialSurfaceCondition
 * @see SCigiWaveCondition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ENVIRONMENTAL_REGION_HANDLER
#define SIMBLOCKS_CIGI_ENVIRONMENTAL_REGION_HANDLER

#include "CigiLib/CigiTypes.h"
#include "CigiEvent.h"
#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "MathLib/MathTypes.h"
#include <map>
#include <unordered_map>
#include <memory>
#include <vector>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Owns the global, regional, and entity-scoped environmental-state model.
       */
      class CCigiEnvironmentalRegionHandler : public IIGCIGIEventListener
      {
      public:
        /**
         * @brief Constructs an environmental region handler.
         */
        CCigiEnvironmentalRegionHandler();
        /**
         * @brief Destroys the environmental region handler.
         */
        ~CCigiEnvironmentalRegionHandler();

        /** @brief Ignores database-loaded notifications. */
        virtual void OnDatabaseLoadedEvent() override {};
        /** @brief Ignores database-load failures. */
        virtual void OnDatabaseLoadingFailedEvent() override {};
        /**
         * @brief Removes entity-scoped conditions and, if any were removed, notifies the available event messenger.
         * @param entityID Identifier of the removed entity.
         */
        virtual void OnEntityRemoved(sbio::EntityID entityID) override;
        /**
         * @brief Ignores image-generator error notifications.
         * @param args Unused error details.
         */
        virtual void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) override {};

        /**
         * @brief Clears all environmental conditions and restores the initial global state.
         */
        void Reset();

        /**
         * @brief Handles parsing of Cigi Environmental Region packets.
         * @param environmentalRegion Environmental region packet state.
         */
        void Handle(const SCigiEnvironmentalRegion& environmentalRegion);
        /**
         * @brief Applies a global terrestrial surface condition.
         * @param terrestrialSurfaceCondition Terrestrial surface condition data.
         */
        void HandleGlobalTerrestrialSurfaceCondition(const SCigiTerrestrialSurfaceCondition& terrestrialSurfaceCondition);
        /**
         * @brief Applies an entity-scoped terrestrial surface condition.
         * @param entityID Entity receiving the condition.
         * @param terrestrialSurfaceCondition Terrestrial surface condition data.
         */
        void HandleEntityTerrestrialSurfaceCondition(sbio::EntityID entityID, const SCigiTerrestrialSurfaceCondition& terrestrialSurfaceCondition);
        /**
         * @brief Applies a regional terrestrial surface condition.
         * @param regionID Region receiving the condition.
         * @param terrestrialSurfaceCondition Terrestrial surface condition data.
         */
        void HandleRegionTerrestrialSurfaceCondition(sbio::RegionID regionID, const SCigiTerrestrialSurfaceCondition& terrestrialSurfaceCondition);
        /**
         * @brief Handles environmental conditions request packets.
         * @param environmentalConditionsRequest Environmental conditions request data.
         */
        void Handle(const SEnvironmentalConditionsRequest& environmentalConditionsRequest);

        // WaveControl

        /**
         * @brief Queries local weather at a position on the earth.
         * @param query Geodetic coordinates to query.
         * @return Weather condition at the query location.
         */
        SCigiWeatherCondition QueryWeather(const sbio::math::SGeodeticCoordinates& query);

        /**
         * @brief Queries aerosol concentrations with independent spatial blending per aerosol type.
         * @param query Geodetic location to sample.
         * @return Concentrations keyed by aerosol layer identifier.
         */
        std::map<uint8_t, float> QueryAerosolConcentrations(const sbio::math::SGeodeticCoordinates& query);

        /**
         * @brief Queries local maritime surface condition at a position on the earth.
         * @param query Geodetic coordinates to query.
         * @return Maritime surface condition at the query location.
         */
        SCigiMaritimeSurfaceCondition QueryMaritimeSurface(const sbio::math::SGeodeticCoordinates& query);
        /**
         * @brief Queries local terrestrial surface conditions at a position on the earth.
         * @param query Geodetic coordinates to query.
         * @return Merged terrestrial surface conditions at the query location, one per unique surface-condition ID.
         */
        std::vector<SCigiTerrestrialSurfaceCondition> QueryTerrestrialSurface(const sbio::math::SGeodeticCoordinates& query);

        /**
         * @brief Sets weather conditions for entities.
         * @param entityID Entity ID.
         * @param condition Weather condition.
         * @param spatialWeatherCondition Spatial weather condition.
         */
        void SetEntityWeatherCondition(EntityID entityID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition);
        /**
         * @brief Sets weather conditions for regions.
         * @param regionID Region ID.
         * @param layerID Weather layer ID.
         * @param condition Weather condition.
         * @param spatialWeatherCondition Spatial weather condition.
         */
        void SetRegionalWeatherCondition(sbio::RegionID regionID, RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition,
                                         const SCigiSpatialWeatherCondition& spatialWeatherCondition);
        /**
         * @brief Sets weather conditions globally.
         * @param layerID Global weather layer ID.
         * @param condition Weather condition.
         * @param spatialWeatherCondition Spatial weather condition.
         */
        void SetGlobalWeatherCondition(sbio::GlobalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition);

        /**
         * @brief Sets maritime surface conditions for entities.
         * @param entityID Entity ID.
         * @param condition Maritime surface condition.
         */
        void SetEntityMaritimeSurfaceCondition(EntityID entityID, const SCigiMaritimeSurfaceCondition& condition);
        /**
         * @brief Sets maritime surface conditions for regions.
         * @param regionID Regional sea surface ID.
         * @param condition Maritime surface condition.
         */
        void SetRegionMaritimeSurfaceCondition(sbio::RegionID regionID, const SCigiMaritimeSurfaceCondition& condition);
        /**
         * @brief Sets maritime surface conditions globally.
         * @param condition Maritime surface condition.
         */
        void SetGlobalMaritimeSurfaceCondition(const SCigiMaritimeSurfaceCondition& condition);

        /**
         * @brief Sets terrestrial surface conditions for entities.
         * @param entityID Entity ID.
         * @param condition Terrestrial surface condition.
         */
        void SetEntityTerrestrialSurfaceCondition(sbio::EntityID entityID, const SCigiTerrestrialSurfaceCondition& condition);
        /**
         * @brief Sets terrestrial surface conditions for regions.
         * @param regionID Regional terrain surface ID.
         * @param condition Terrestrial surface condition.
         */
        void SetRegionTerrestrialSurfaceCondition(sbio::RegionID regionID, const SCigiTerrestrialSurfaceCondition& condition);
        /**
         * @brief Sets terrestrial surface conditions globally.
         * @param condition Terrestrial surface condition.
         */
        void SetGlobalTerrestrialSurfaceCondition(const SCigiTerrestrialSurfaceCondition& condition);

        /**
         * @brief Sets wave conditions for entities.
         * @param entityID Entity ID.
         * @param waveID Entity wave ID.
         * @param condition Wave condition.
         */
        void SetEntityWaveCondition(sbio::EntityID entityID, EntityWaveID waveID, const SCigiWaveCondition& condition);
        /**
         * @brief Sets wave conditions for regions.
         * @param regionID Region ID.
         * @param waveID Regional wave ID.
         * @param condition Wave condition.
         */
        void SetRegionalWaveCondition(sbio::RegionID regionID, RegionalWaveID waveID, const SCigiWaveCondition& condition);
        /**
         * @brief Sets wave conditions globally.
         * @param waveID Global wave ID.
         * @param condition Wave condition.
         */
        void SetGlobalWaveCondition(sbio::GlobalWaveID waveID, const SCigiWaveCondition& condition);

      protected:
        /**
         * @brief Accumulates weighted terrestrial surface condition values during region merging.
         */
        struct STerrestrialSurfaceConditionAccumulator
        {
          float totalWeight = 0.0f;///< Sum of weights included in the accumulator.
          float weightedSeverity = 0.0f;///< Accumulated severity multiplied by weight.
          float weightedCoverage = 0.0f;///< Accumulated coverage multiplied by weight.
        };

        /**
         * @brief Adds one terrestrial surface condition to an accumulator map.
         * @param accumulators Accumulator map keyed by condition identifier.
         * @param condition Terrestrial surface condition to add.
         * @param weight Weight applied to the condition values.
         */
        void AccumulateTerrestrialSurfaceCondition(std::map<uint16_t, STerrestrialSurfaceConditionAccumulator>& accumulators, const SCigiTerrestrialSurfaceCondition& condition,
                                                   float weight);
        /**
         * @brief Builds merged terrestrial surface conditions from accumulator values.
         * @param accumulators Accumulator map keyed by condition identifier.
         * @return Merged terrestrial surface condition list.
         */
        std::vector<SCigiTerrestrialSurfaceCondition> BuildMergedTerrestrialSurfaceConditions(const std::map<uint16_t, STerrestrialSurfaceConditionAccumulator>& accumulators);

      private:
        /**
         * @brief Spatial index of regional environmental regions.
         */
        std::unique_ptr<CCigiRegionTree> m_Regions;
        /**
         * @brief Entity-scoped environmental regions keyed by entity identifier.
         */
        typedef std::unordered_map<EntityID, std::unique_ptr<CCigiEnvironmentalRegion>, StrongTypeHash<EntityID>> TEnvironmentalRegionEntities;
        TEnvironmentalRegionEntities m_EnvironmentalEntities;
        uint64_t m_NextRegionUpdateSequence = 1;
        /**
         * @brief Globally scoped environmental region.
         */
        std::unique_ptr<CCigiEnvironmentalRegion> m_GlobalRegion;
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
