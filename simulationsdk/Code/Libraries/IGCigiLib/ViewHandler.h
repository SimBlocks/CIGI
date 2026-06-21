//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewHandler.h
 * @brief Declares the CCigiViewHandler class for SimBlocks CIGI IG view control and definition handling.
 *
 * Provides the CCigiViewHandler class for handling CIGI view control and definition messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI image generator and view control/definition types for simulation view management.
 * Supports processing of view control and definition messages, and manages image generator reference for view operations.
 *
 * @see sbio::cigi::ig::CCigiViewHandler
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::SCigiViewControl
 * @see sbio::cigi::SCigiViewDefinition
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_VIEW_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_VIEW_CONTROL_HANDLER_H

#include "IGCigiLib/IGCigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Applies view-control and view-definition packets to image-generator view objects.
       */
      class CCigiViewHandler
      {
      public:
        /**
         * @brief Constructs a handler bound to one image generator.
         * @param pImageGenerator Non-owning image-generator pointer used to resolve view objects.
         */
        CCigiViewHandler(CCigiImageGenerator* pImageGenerator);

        /**
         * @brief Applies a view-control packet.
         * @param viewControl Parsed view-control state.
         */
        void Handle(const sbio::cigi::SCigiViewControl& viewControl);
        /**
         * @brief Applies a view-definition packet.
         * @param viewDefinition Parsed view-definition state.
         */
        void Handle(const sbio::cigi::SCigiViewDefinition& viewDefinition);

      private:
        CCigiImageGenerator* m_pImageGenerator = nullptr;///< Non-owning image generator used to resolve views.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
