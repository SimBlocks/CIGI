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
         * @brief Initializes the host view manager with a config file and view creator.
         * @param viewsConfigFilePath Path to the views configuration file.
         * @param pViewCreator Unique pointer to a custom view creator consumed by the manager.
         *
         * Ownership:
         * - Ownership of `pViewCreator` is transferred to this call.
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
