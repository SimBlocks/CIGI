//Copyright SimBlocks LLC 2016-2026
/**
 * @file SensorControlHandler.h
 * @brief Declares the CCigiSensorControlHandler class for handling CIGI sensor control messages in SimBlocks CIGI IG library.
 *
 * Provides the CCigiSensorControlHandler class for processing sensor control messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI and math types for simulation and sensor management.
 *
 * @see sbio::cigi::ig::CCigiSensorControlHandler
 * @see sbio::cigi::SCigiSensorControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SENSOR_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_SENSOR_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "MathLib/MathTypes.h"
#include <unordered_set>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies host-supplied sensor control updates.
       */
      class CCigiSensorControlHandler
      {
      public:
        /**
         * @brief Applies a sensor-control update.
         * @param sensorControl Parsed sensor-control state.
         */
        void Handle(const sbio::cigi::SCigiSensorControl& sensorControl);

      private:
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
