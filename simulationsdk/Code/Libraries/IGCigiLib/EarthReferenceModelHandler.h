//Copyright SimBlocks LLC 2016-2026
/**
 * @file EarthReferenceModelHandler.h
 * @brief Declares the CCigiEarthReferenceModelHandler class for handling CIGI earth reference model messages.
 *
 * Provides the CCigiEarthReferenceModelHandler class for processing earth reference model messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI types and math types for simulation and earth model management.
 *
 * @see sbio::cigi::ig::CCigiEarthReferenceModelHandler
 * @see sbio::cigi::SCigiEarthReferenceModel
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_EARTH_REFERENCE_MODEL_HANDLER_H
#define SIMBLOCKS_CIGI_EARTH_REFERENCE_MODEL_HANDLER_H

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
       * @brief Applies host-selected earth-reference-model definitions.
       */
      class CCigiEarthReferenceModelHandler
      {
      public:
        /**
         * @brief Applies an earth-reference-model definition.
         * @param earthReferenceModel Parsed earth-reference-model state.
         */
        void Handle(const sbio::cigi::SCigiEarthReferenceModel& earthReferenceModel);

      private:
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
