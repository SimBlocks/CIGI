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
     * @brief Process-wide view configuration containing a data path and borrowed service pointers.
     *
     * `InitViewLib()` populates these settings from shared resources but stores only raw pointers. External owners
     * must keep each referenced resource alive while it is used through these settings. Null pointers are permitted
     * by initialization; the structure does not provide fallback services.
     */
    struct SViewLibSettings
    {
      std::filesystem::path dataPath;///< globals.librariesDataPath / "ViewLib", copied during initialization and retained on teardown.
      sbio::view::CViewManager* pViewManager = nullptr;///< Borrowed manager; ViewLib does not retain shared ownership.
      sbio::utils::CEventDispatcher* pEventDispatcher = nullptr;///< Borrowed event dispatcher; not destroyed by ViewLib.
      sbio::utils::CLogger* pLogger = nullptr;///< Borrowed logger; not destroyed by ViewLib.
    };

    /**
     * @brief Supplies the view manager from which initialization borrows a raw pointer.
     *
     * This parameter object holds shared ownership, but `InitViewLib()` does not copy that ownership into the global
     * settings. Keep this or another owning reference alive for as long as the library uses the manager.
     */
    struct SViewLibParams
    {
      std::shared_ptr<CViewManager> pViewManager;///< Manager to expose through global settings; may be empty.
    };
  }
}

/**
 * @brief Converts an exact projection-mode name to its enum value.
 * @param sProjectionMode Case-sensitive name; no trimming or normalization is performed.
 * @return PERSPECTIVE for "PERSPECTIVE", ORTHOGRAPHIC for "ORTHOGRAPHIC", or UNKNOWN for any other string.
 */
sbio::EProjectionMode ToProjectionMode(const std::string& sProjectionMode);

/** @brief Process-wide settings populated by InitViewLib(); resource pointers are non-owning. */
extern sbio::view::SViewLibSettings g_ViewLibSettings;

/**
 * @brief Replaces the process-wide data path and borrowed service pointers.
 * @param globals Source of the library data path, event dispatcher, and logger.
 * @param params Source of the view manager pointer.
 *
 * Stores globals.librariesDataPath / "ViewLib" and the raw pointers obtained from the supplied shared pointers,
 * without extending resource lifetimes or validating null pointers. Does not create, reset, or destroy managers,
 * views, or groups. External owners must keep referenced resources alive while the library uses them.
 */
void InitViewLib(const sbio::SGlobals& globals, const sbio::view::SViewLibParams& params);

/**
 * @brief Clears the borrowed manager, event-dispatcher, and logger pointers.
 *
 * Does not destroy the referenced resources, reset views or groups, or clear the stored data path. Repeated calls
 * have the same effect as a single call.
 */
void UninitViewLib();

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
