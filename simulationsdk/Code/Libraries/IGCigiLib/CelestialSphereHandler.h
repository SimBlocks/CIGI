//Copyright SimBlocks LLC 2016-2026
/**
 * @file CelestialSphereHandler.h
 * @brief Declares the CCigiCelestialSphereHandler class for handling CIGI celestial sphere and date/time messages.
 *
 * Provides the CCigiCelestialSphereHandler class for processing celestial sphere and date/time messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI and common types for simulation, celestial sphere, and date/time management.
 *
 * @see sbio::cigi::ig::CCigiCelestialSphereHandler
 * @see sbio::cigi::SCelestialSphere
 * @see sbio::SDate
 * @see sbio::STime
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_CELESTIAL_SPHERE_HANDLER_H
#define SIMBLOCKS_CIGI_CELESTIAL_SPHERE_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies celestial-sphere control packets and explicit date/time updates.
       *
       * Responsibilities:
       * - Consumes parsed celestial-sphere state.
       * - Applies explicit date/time values used by the active celestial model.
       *
       * Ownership:
       * - This type does not own the celestial model or any shared services.
       *
       * Invariants:
       * - The handler is stateless between calls.
       *
       * Failure cases:
       * - This type does not expose direct error reporting; unsupported values are handled downstream.
       *
       * Side effects:
       * - Successful calls may change sun, moon, star-field, and time-of-day state in the IG.
       */
      class CCigiCelestialSphereHandler
      {
      public:
        /**
         * @brief Applies a celestial-sphere control update.
         * @param celestialSphere Parsed celestial-sphere state.
         */
        void Handle(const sbio::cigi::SCelestialSphere& celestialSphere);
        /**
         * @brief Applies an explicit date/time override.
         * @param date Date to publish to the celestial model.
         * @param time Time to publish to the celestial model.
         */
        void SetDateTime(const sbio::SDate& date, const sbio::STime& time);

      private:
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
