//Copyright SimBlocks LLC 2016-2026
/**
 * @file MotionTrackerControlHandler.h
 * @brief Declares the CCigiMotionTrackerControlHandler class for handling CIGI motion tracker control messages in SimBlocks CIGI IG library.
 *
 * Provides the CCigiMotionTrackerControlHandler class for processing motion tracker view and view group control messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI and math types for simulation and motion tracker management.
 *
 * @see sbio::cigi::ig::CCigiMotionTrackerControlHandler
 * @see sbio::cigi::SMotionTrackerViewControl
 * @see sbio::cigi::SMotionTrackerViewGroupControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_MOTION_TRACKER_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_MOTION_TRACKER_CONTROL_HANDLER_H

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
       * @brief Applies motion-tracker enablement and routing updates for views and view groups.
       */
      class CCigiMotionTrackerControlHandler
      {
      public:
        /**
         * @brief Applies a motion-tracker control update for a view.
         * @param motionTrackerViewControl Parsed view-scoped tracker state.
         */
        void Handle(const sbio::cigi::SMotionTrackerViewControl& motionTrackerViewControl);
        /**
         * @brief Applies a motion-tracker control update for a view group.
         * @param motionTrackerViewGroupControl Parsed view-group-scoped tracker state.
         */
        void Handle(const sbio::cigi::SMotionTrackerViewGroupControl& motionTrackerViewGroupControl);

      private:
        std::unordered_set<sbio::MotionTrackerID, StrongTypeHash<sbio::MotionTrackerID>> m_Trackers;///< Tracker identifiers observed by this handler.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
