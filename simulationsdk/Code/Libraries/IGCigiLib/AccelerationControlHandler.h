//Copyright SimBlocks LLC 2016-2026
/**
 * @file AccelerationControlHandler.h
 * @brief Declares the CCigiAccelerationControlHandler class for handling CIGI entity and articulated part acceleration control messages.
 *
 * Provides the CCigiAccelerationControlHandler class for processing acceleration control messages for entities and articulated parts in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI types for simulation and acceleration management.
 *
 * @see sbio::cigi::ig::CCigiAccelerationControlHandler
 * @see SCigiEntityAcceleration
 * @see SCigiArticulatedPartAcceleration
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ACCELERATION_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_ACCELERATION_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies host-supplied acceleration controls to entities and articulated parts.
       *
       * Responsibilities:
       * - Translates parsed CIGI acceleration data into updates on simulation objects managed elsewhere.
       * - Provides separate entry points for entity and articulated-part acceleration packets.
       *
       * Ownership:
       * - This type does not own simulation objects or shared library services.
       *
       * Invariants:
       * - The handler is stateless between calls.
       *
       * Failure cases:
       * - Invalid identifiers or unavailable targets are handled by downstream systems; this type exposes no direct error reporting.
       *
       * Side effects:
       * - Successful calls update acceleration state on the addressed simulation object.
       */
      class CCigiAccelerationControlHandler
      {
      public:
        /**
         * @brief Applies an entity acceleration control update.
         * @param acceleration Parsed entity acceleration payload.
         */
        void HandleEntityAccelerationControl(const SCigiEntityAcceleration& acceleration);
        /**
         * @brief Applies an articulated-part acceleration control update.
         * @param acceleration Parsed articulated-part acceleration data.
         */
        void HandleArticulatedPartAccelerationControl(const SCigiArticulatedPartAcceleration& acceleration);
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
