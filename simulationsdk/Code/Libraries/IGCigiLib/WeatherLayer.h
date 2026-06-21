//Copyright SimBlocks LLC 2016-2026
/**
 * @file WeatherLayer.h
 * @brief Declares the CCigiWeatherLayer class for SimBlocks CIGI IG weather layer management and simulation.
 *
 * Provides the CCigiWeatherLayer class for representing and managing weather layers in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI types for weather condition state, spatial condition, activation, and weather simulation.
 * Supports activation state, scud, random winds, random lightning, weather condition setting and retrieval, spatial condition, and contribution calculation for simulation.
 *
 * @see sbio::cigi::ig::CCigiWeatherLayer
 * @see sbio::cigi::SCigiWeatherCondition
 * @see sbio::cigi::SCigiSpatialWeatherCondition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_WEATHER_LAYER_H
#define SIMBLOCKS_CIGI_WEATHER_LAYER_H

#include "CigiLib/CigiTypesHostToIG.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Stores one weather layer together with its spatial envelope and enable flags.
       */
      class CCigiWeatherLayer
      {
      public:
        /**
         * @brief Constructs an inactive weather layer.
         */
        CCigiWeatherLayer();

        /**
         * @brief Reports whether the layer is active.
         * @return `true` when the layer should contribute to queries; otherwise `false`.
         */
        bool GetActive() const;
        /**
         * @brief Reports whether scud rendering is enabled for the layer.
         * @return `true` when scud is enabled; otherwise `false`.
         */
        bool GetScud() const;
        /**
         * @brief Reports whether random winds are enabled for the layer.
         * @return `true` when random winds are enabled; otherwise `false`.
         */
        bool GetRandomWinds() const;
        /**
         * @brief Reports whether random lightning is enabled for the layer.
         * @return `true` when random lightning is enabled; otherwise `false`.
         */
        bool GetRandomLightening() const;
        /**
         * @brief Sets whether the layer is active.
         * @param val `true` to activate the layer; otherwise `false`.
         */
        void SetActive(bool val);
        /**
         * @brief Sets whether scud is enabled for the layer.
         * @param val `true` to enable scud; otherwise `false`.
         */
        void SetScud(bool val);
        /**
         * @brief Sets whether random winds are enabled for the layer.
         * @param val `true` to enable random winds; otherwise `false`.
         */
        void SetRandomWinds(bool val);
        /**
         * @brief Sets whether random lightning is enabled for the layer.
         * @param val `true` to enable random lightning; otherwise `false`.
         */
        void SetRandomLightening(bool val);

        /**
         * @brief Replaces the stored weather condition.
         * @param condition Condition value to store.
         */
        void SetWeatherCondition(const SCigiWeatherCondition& condition);
        /**
         * @brief Replaces the stored spatial weather condition.
         * @param spatialWeatherCondition Spatial envelope to store.
         */
        void SetSpatialWeatherCondition(const SCigiSpatialWeatherCondition& spatialWeatherCondition);
        /**
         * @brief Returns the stored weather condition.
         * @return Reference to the stored weather condition.
         */
        SCigiWeatherCondition& GetWeatherCondition();

        /**
         * @brief Computes the contribution of this layer at one altitude.
         * @param altitude Query altitude.
         * @return A contribution factor derived from the stored spatial envelope.
         */
        double IntersectionTest(sbio::math::HeightRelativeToWGS84Ellipsoid altitude);

      private:
        SCigiWeatherCondition m_Condition;///< Stored weather condition.
        SCigiSpatialWeatherCondition m_SpatialCondition;///< Stored spatial envelope for the weather layer.
        bool m_bActive = false;///< Indicates whether the layer contributes to queries.
        bool m_bScud = false;///< Indicates whether scud is enabled for this layer.
        bool m_bRandomWinds = false;///< Indicates whether random winds are enabled for this layer.
        bool m_bRandomLightning = false;///< Indicates whether random lightning is enabled for this layer.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
