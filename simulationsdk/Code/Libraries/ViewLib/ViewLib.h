//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewLib.h
 * @brief Core declarations, settings, and initialization for the ViewLib module.
 *
 * Provides global settings, initialization, and utility functions for the view system, including
 * view manager, event dispatcher, and logger integration. Declares SViewLibSettings and SViewLibParams
 * for configuring and initializing the view library, and exposes global functions for library setup and teardown.
 *
 * @see sbio::view::CViewManager
 * @see sbio::view::CViewGroup
 * @see sbio::view::SViewLibSettings
 * @see sbio::view::SViewLibParams
 * @see InitViewLib
 * @see UninitViewLib
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_LIB_H
#define SIMBLOCKS_VIEW_LIB_H

#include "ViewLib/ViewDeclarations.h"
#include "GlobalHeaders/CommonDeclarations.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>
#include <string>
#include <filesystem>

namespace sbio
{
  namespace view
  {
    /**
     * @brief Global settings for the view library, including resource pointers and configuration paths.
     */
    struct SViewLibSettings
    {
      std::filesystem::path dataPath;///< Path to view data
      sbio::view::CViewManager* pViewManager = nullptr;///< Pointer to view manager
      sbio::utils::CEventDispatcher* pEventDispatcher = nullptr;///< Pointer to event dispatcher
      sbio::utils::CLogger* pLogger = nullptr;///< Pointer to logger
    };

    /**
     * @brief Parameters for initializing the view library, including shared pointers to managers.
     */
    struct SViewLibParams
    {
      std::shared_ptr<CViewManager> pViewManager;///< Shared pointer to view manager
    };
  }
}

/**
 * @brief Converts a string to a projection mode enum value.
 * @param sProjectionMode The string representation of the projection mode.
 * @return The corresponding EProjectionMode value.
 */
sbio::EProjectionMode ToProjectionMode(const std::string& sProjectionMode);

extern sbio::view::SViewLibSettings g_ViewLibSettings;

/**
 * @brief Initializes the view library with global settings and parameters.
 * @param globals Global settings and shared resource pointers.
 * @param params Parameters for view library initialization.
 */
void InitViewLib(const sbio::SGlobals& globals, const sbio::view::SViewLibParams& params);

/**
 * @brief Uninitializes the view library and releases resources.
 */
void UninitViewLib();

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
