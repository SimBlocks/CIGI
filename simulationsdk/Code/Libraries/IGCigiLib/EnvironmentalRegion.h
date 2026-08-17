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
       * @brief Stores and evaluates environmental conditions for one CIGI region.
       *
       * Manages weather, maritime, terrestrial, and wave conditions, together with
       * the region geometry used for contribution calculations.
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
        void SetRotation(double rotation);

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

        /** @brief Gets the weather merge state.
         * @return Current weather merge state.
         */
        EMergeState GetMergeWeather() const;

        /** @brief Gets the aerosol merge state.
         * @return Current aerosol merge state.
         */
        EMergeState GetMergeAerosol() const;

        /** @brief Gets the maritime merge state.
         * @return Current maritime merge state.
         */
        EMergeState GetMergeMaritime() const;

        /** @brief Gets the terrestrial merge state.
         * @return Current terrestrial merge state.
         */
        EMergeState GetMergeTerrestrial() const;

        /** @brief Sets the region update sequence.
         * @param updateSequence Update sequence value.
         */
        void SetUpdateSequence(uint64_t updateSequence);

        /** @brief Gets the region update sequence.
         * @return Current update sequence value.
         */
        uint64_t GetUpdateSequence() const;

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
         * @param used Whether the maritime condition is active.
         */
        void QueryWeatherAtAltitude(sbio::math::HeightRelativeToWGS84Ellipsoid altitude, SCigiWeatherCondition& out, bool& used);

        /**
         * @brief Sets the maritime surface condition for the region.
         * @param condition Maritime surface condition.
         * @param used Whether the terrestrial condition is active.
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
        uint32_t m_regionID = 0;///< Region's unique ID.

        typedef std::unordered_map<RegionalLayeredWeatherID, CCigiWeatherLayer*, StrongTypeHash<RegionalLayeredWeatherID>> TRegionalWeatherLayers;
        TRegionalWeatherLayers m_WeatherLayers;///< Weather layers associated with the region.

        CCigiWeatherLayer* m_LastWeatherLayer = nullptr;///< Most recently added weather layer.

        CCigiMaritimeSurfaceCondition m_MaritimeSurfaceCondition;///< Maritime surface condition for the region.

        CTerrestrialSurfaceCondition m_TerrestrialSurfaceCondition;///< Terrestrial surface condition for the region.

        typedef std::unordered_map<RegionalWaveID, CCigiWaveLayer*, StrongTypeHash<RegionalWaveID>> TWaveLayers;
        TWaveLayers m_WaveLayers;///< Wave layers associated with the region.

        sbio::math::SGeodeticCoordinates m_Origin;///< Region origin in geodetic coordinates.

        float m_SizeX = 0;///< Half-size of the region along the X axis.
        float m_SizeY = 0;///< Half-size of the region along the Y axis.
        float m_CornerRadius = 0;///< Corner radius used by the region shape.
        double m_Rotation = 0;///< Rotation of the region.
        float m_TransitionPerimeter = 0;///< Perimeter width used for the transition.

        bool m_bActive = true;///< Indicates whether the region is active.
        EMergeState m_eMergeWeather = sbio::cigi::EMergeState::UNKNOWN;///< Weather merge state.
        EMergeState m_eMergeAerosol = sbio::cigi::EMergeState::UNKNOWN;///< Aerosol merge state.
        EMergeState m_eMergeMaritime = sbio::cigi::EMergeState::UNKNOWN;///< Maritime merge state.
        EMergeState m_eMergeTerrestrial = sbio::cigi::EMergeState::UNKNOWN;///< Terrestrial merge state.
        uint64_t m_UpdateSequence = 0;///< Region update sequence.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
