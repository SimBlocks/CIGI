//Copyright SimBlocks LLC 2016-2026
/**
 * @file EnvironmentalRegion.h
 * @brief Declares the CCigiEnvironmentalRegion class for SimBlocks CIGI IG environmental region management.
 *
 * Provides the CCigiEnvironmentalRegion class for managing environmental regions in the SimBlocks CIGI IG library, including weather, maritime, terrestrial, and wave conditions.
 * Supports region shape, position, merging, and contribution calculations for simulation interoperability.
 * Integrates with SimBlocks CIGI, math, and engine types for simulation and environmental control.
 *
 * @see sbio::cigi::ig::CCigiEnvironmentalRegion
 * @see CCigiWeatherLayer
 * @see CCigiMaritimeSurfaceCondition
 * @see CTerrestrialSurfaceCondition
 * @see CCigiWaveLayer
 * @see SCigiWeatherCondition
 * @see SCigiMaritimeSurfaceCondition
 * @see SCigiTerrestrialSurfaceCondition
 * @see SCigiWaveCondition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ENVIRONMENTAL_REGION_H
#define SIMBLOCKS_CIGI_ENVIRONMENTAL_REGION_H

#include "CigiLib/CigiConversions.h"
#include "EnvironmentalRegionHandler.h"
#include "MaritimeSurfaceCondition.h"
#include "TerrestrialSurfaceCondition.h"
#include "WaveLayer.h"
#include "WeatherLayer.h"
#include "EngineLib/ImageGeneratorMessages.h"

#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Environmental region class for SimBlocks CIGI IG integration.
       *
       * Manages weather, maritime, terrestrial, and wave conditions for a simulation region.
       * Supports region shape, position, merging, and contribution calculations.
       *
       * Stores all environmental state for one global, regional, or entity-scoped region.
       */
      class CCigiEnvironmentalRegion
      {
      public:
        /**
         * @brief Constructs an environmental region.
         * @param regionID Unique region identifier.
         */
        CCigiEnvironmentalRegion(RegionID regionID);

        /**
         * @brief Sets the region active state.
         * @param active True to activate, false to deactivate.
         */
        void SetActive(bool active);
        /**
         * @brief Checks if the region is active.
         * @return True if active, false otherwise.
         */
        bool IsActive() const;
        /**
         * @brief Sets the origin of the region.
         * @param latitude Latitude value.
         * @param longitude Longitude value.
         */
        void SetOrigin(sbio::math::Latitude latitude, sbio::math::Longitude longitude);
        /**
         * @brief Gets the origin of the region.
         * @return Geocentric coordinates of the origin.
         */
        sbio::math::GeocentricCoordinates GetOrigin() const;
        /**
         * @brief Gets the radius of the region.
         * @return Radius value.
         */
        double GetRadius() const;
        /**
         * @brief Sets the dimensions of the region.
         * @param x Size X.
         * @param y Size Y.
         * @param radius Corner radius.
         * @param transitionPerimeter Transition perimeter.
         */
        void SetDimensions(float x, float y, float radius, float transitionPerimeter);
        /**
         * @brief Sets the rotation of the region.
         * @param rotation Rotation value.
         */
        void SetRotation(float rotation);
        /**
         * @brief Solves rounded rectangle cases for a point.
         * @param point Query point.
         * @param X Size X.
         * @param Y Size Y.
         * @param radius Corner radius.
         * @return Contribution value.
         */
        float SolveRoundedRectangleCases(Vec2d point, double X, double Y, double radius);

        /**
         * @brief Returns a float between 0 and 1 of how much the current region contributes to weather/sea/environment.
         * @param query Geodetic coordinates to query.
         * @return Contribution value.
         */
        float IntersectionTest(const sbio::math::SGeodeticCoordinates& query);

        /**
         * @brief Sets the weather condition for the region.
         * @param condition Weather condition.
         * @param spatialWeatherCondition Spatial weather condition.
         */
        void SetWeatherCondition(const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition);
        /**
         * @brief Sets the weather condition for a specific layer.
         * @param layerID Weather layer ID.
         * @param condition Weather condition.
         * @param spatialWeatherCondition Spatial weather condition.
         */
        void SetWeatherCondition(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition);
        /**
         * @brief Sets the merge state for weather.
         * @param eMergeState Merge state value.
         */
        void SetMergeWeather(EMergeState eMergeState);
        /**
         * @brief Sets the merge state for aerosol.
         * @param eMergeState Merge state value.
         */
        void SetMergeAerosol(EMergeState eMergeState);
        /**
         * @brief Sets the merge state for maritime.
         * @param eMergeState Merge state value.
         */
        void SetMergeMaritime(EMergeState eMergeState);
        /**
         * @brief Sets the merge state for terrestrial.
         * @param eMergeState Merge state value.
         */
        void SetMergeTerrestrial(EMergeState eMergeState);
        /**
         * @brief Adds a weather layer to the region.
         * @param layerID Weather layer ID.
         * @param condition Weather condition.
         * @param spatialWeatherCondition Spatial weather condition.
         */
        void AddWeatherLayer(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition, const SCigiSpatialWeatherCondition& spatialWeatherCondition);
        /**
         * @brief Removes a weather layer from the region.
         * @param layerID Weather layer ID.
         * @param condition Weather condition.
         */
        void RemoveWeatherLayer(RegionalLayeredWeatherID layerID, const SCigiWeatherCondition& condition);
        /**
         * @brief Queries weather at a specific altitude.
         * @param altitude Altitude value.
         * @param out Output weather condition.
         * @param used Output flag for usage.
         */
        void QueryWeatherAtAltitude(sbio::math::HeightRelativeToWGS84Ellipsoid altitude, SCigiWeatherCondition& out, bool& used);
        /**
         * @brief Sets the maritime surface condition for the region.
         * @param condition Maritime surface condition.
         * @param used Output flag for usage.
         */
        void SetMaritimeSurface(const SCigiMaritimeSurfaceCondition& condition, bool used);
        /**
         * @brief Queries the maritime surface condition.
         * @param out Output maritime surface condition.
         * @param used Output flag for usage.
         */
        void QueryMaritimeSurface(SCigiMaritimeSurfaceCondition& out, bool& used);
        /**
         * @brief Sets the terrestrial surface condition for the region.
         * @param condition Terrestrial surface condition.
         * @param used Output flag for usage.
         */
        void SetTerrestrialSurface(const SCigiTerrestrialSurfaceCondition& condition, bool used);
        /**
         * @brief Queries the terrestrial surface condition.
         * @param out Output terrestrial surface condition.
         * @param used Output flag for usage.
         */
        void QueryTerrestrialSurface(SCigiTerrestrialSurfaceCondition& out, bool& used);
        /**
         * @brief Adds a wave layer to the region.
         * @param waveID Wave layer ID.
         * @param condition Wave condition.
         */
        void AddWave(RegionalWaveID waveID, const SCigiWaveCondition& condition);
        /**
         * @brief Removes a wave layer from the region.
         * @param waveID Wave layer ID.
         */
        void RemoveWave(RegionalWaveID waveID);

        typedef std::unordered_map<RegionalWaveID, CCigiWaveLayer*, StrongTypeHash<RegionalWaveID>> TWaveResult;
        /**
         * @brief Queries wave layers in the region.
         * @param out Output wave result map.
         * @param used Output flag for usage.
         */
        void QueryWave(TWaveResult& out, bool& used);

        /**
         * @brief Gets the last weather layer added to the region.
         * @return Pointer to the last weather layer.
         */
        const CCigiWeatherLayer* GetLastWeatherLayer();

      private:
        /* ------------------------------------------------------------------------- *
         *                  Define the content of each region                        *
         * ------------------------------------------------------------------------- */

        /// <summary>
        /// The Region's unique ID
        /// </summary>
        uint32_t m_regionID = 0;

        typedef std::unordered_map<RegionalLayeredWeatherID, CCigiWeatherLayer*, StrongTypeHash<RegionalLayeredWeatherID>> TRegionalWeatherLayers;
        /// <summary>
        /// Each region contains upto 256 weather layers at different altitudes.
        /// </summary>
        TRegionalWeatherLayers m_WeatherLayers;

        CCigiWeatherLayer* m_LastWeatherLayer = nullptr;

        /// <summary>
        /// Each region contains one Maritime Surface Condition.
        /// </summary>
        CCigiMaritimeSurfaceCondition m_MaritimeSurfaceCondition;

        /// <summary>
        /// Each region contains one Terrestrial Surface Conditon.
        /// </summary>
        CTerrestrialSurfaceCondition m_TerrestrialSurfaceCondition;

        typedef std::unordered_map<RegionalWaveID, CCigiWaveLayer*, StrongTypeHash<RegionalWaveID>> TWaveLayers;
        /// <summary>
        /// Each region contains up to 256 waves.
        /// </summary>
        TWaveLayers m_WaveLayers;

        /// <summary>
        /// Origin of Region on earth in LLA.
        /// </summary>
        sbio::math::SGeodeticCoordinates m_Origin;

        /// <summary>
        /// Dimensions of the region.
        /// </summary>
        float m_SizeX = 0;
        float m_SizeY = 0;
        float m_CornerRadius = 0;
        float m_Rotation = 0;
        float m_TransitionPerimeter = 0;

        /// <summary>
        /// Merging properties of the region.
        /// </summary>
        bool m_bActive = true;
        EMergeState m_eMergeWeather = sbio::cigi::EMergeState::UNKNOWN;
        EMergeState m_eMergeAerosol = sbio::cigi::EMergeState::UNKNOWN;
        EMergeState m_eMergeMaritime = sbio::cigi::EMergeState::UNKNOWN;
        EMergeState m_eMergeTerrestrial = sbio::cigi::EMergeState::UNKNOWN;
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
