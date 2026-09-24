//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostViewManager.h
 * @brief Declares the CHostViewManager class for host-side view management in CIGI integration.
 *
 * Provides the CHostViewManager class for managing and initializing views on the host side of a CIGI-based simulation.
 * Supports configuration from a views config file and integration with custom view creators. Inherits from CViewManager
 * for extensible view management and configuration.
 *
 * @see sbio::cigi::host::CHostViewManager
 * @see sbio::view::CViewManager
 * @see sbio::view::IViewCreator
 */
#pragma once
#ifndef SIMBLOCKS_HOST_VIEW_MANAGER_H
#define SIMBLOCKS_HOST_VIEW_MANAGER_H

#include "ViewLib/ViewManager.h"
#include <memory>
#include <filesystem>

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Manages and initializes views for the host in a CIGI-based simulation.
       *
       * Supports configuration from a views config file and integration with custom view creators.
       * The class adapts the generic view manager to the configuration workflow used by the
       * host-side CIGI applications.
       */
      class CHostViewManager : public sbio::view::CViewManager
      {
      public:
        /**
         * @brief Creates and adds views described by a JSON configuration file.
         * @param viewsConfigFilePath Path to a JSON object containing a `Views` array of objects.
         * @param pViewCreator Creator used during this call; ownership is consumed and it is not retained.
         *
         * Entries without `viewID` are skipped. Optional `projectionMode`, `near`, `far`, `left`, `right`,
         * `top`, and `bottom` values are forwarded to the view's setters. Existing views are not cleared.
         * Null creators, file/parse errors, invalid entries, and creation failures are logged when a logger
         * is available. Processing stops on failure; views already added are not rolled back.
         */
        void Init(const std::filesystem::path& viewsConfigFilePath, std::unique_ptr<sbio::view::IViewCreator> pViewCreator);

      private:
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
