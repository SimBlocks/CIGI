//Copyright SimBlocks LLC 2016-2026
/**
 * @file VelocityControlHandler.h
 * @brief Declares the CCigiVelocityControlHandler class for handling CIGI entity and articulated part velocity control messages.
 *
 * Provides the CCigiVelocityControlHandler class for processing velocity control messages for entities and articulated parts in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI types for simulation and velocity management.
 *
 * @see sbio::cigi::ig::CCigiVelocityControlHandler
 * @see SCigiEntityVelocityControl
 * @see SCigiArticulatedPartVelocityControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_VELOCITY_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_VELOCITY_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies host-supplied velocity controls to entities and articulated parts.
       */
      class CCigiVelocityControlHandler
      {
      public:
        /**
         * @brief Applies an entity velocity control update.
         * @param entityVelocityControl Parsed entity velocity state.
         */
        void HandleEntityVelocityControl(const SCigiEntityVelocityControl& entityVelocityControl);
        /**
         * @brief Applies an articulated-part velocity control update.
         * @param articulatedPartVelocityControl Parsed articulated-part velocity state.
         */
        void HandleArticulatedPartVelocityControl(const SCigiArticulatedPartVelocityControl& articulatedPartVelocityControl);
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
