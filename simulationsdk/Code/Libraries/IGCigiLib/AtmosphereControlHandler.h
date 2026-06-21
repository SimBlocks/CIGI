//Copyright SimBlocks LLC 2016-2026
/**
 * @file AtmosphereControlHandler.h
 * @brief Declares the CCigiAtmosphereControlHandler class for handling CIGI atmosphere control messages.
 *
 * Provides the CCigiAtmosphereControlHandler class for processing atmosphere control messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI types for simulation and atmosphere management.
 *
 * @see sbio::cigi::ig::CCigiAtmosphereControlHandler
 * @see sbio::cigi::SAtmosphere
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ATMOSPHERE_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_ATMOSPHERE_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies atmosphere enablement and parameter updates.
       *
       * Responsibilities:
       * - Tracks the last requested atmosphere enable state.
       * - Forwards atmosphere parameter changes to downstream IG systems.
       *
       * Ownership:
       * - The handler owns only its cached enable flag.
       *
       * Invariants:
       * - `m_bAtmosphereEnabled` mirrors the last enable state passed to `Handle() `.
       *
       * Failure cases:
       * - This type does not report failures directly; downstream systems determine whether an atmosphere update can be applied.
       *
       * Side effects:
       * - Successful calls may change global atmospheric state in the active IG.
       */
      class CCigiAtmosphereControlHandler
      {
      public:
        /**
         * @brief Applies a global atmosphere control update.
         * @param bAtmosphereEnabled Requested atmosphere enable state.
         * @param atmosphere Parsed atmosphere state.
         */
        void Handle(bool bAtmosphereEnabled, const sbio::cigi::SAtmosphere& atmosphere);

      private:
        bool m_bAtmosphereEnabled = false;///< Last requested atmosphere enable state.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
