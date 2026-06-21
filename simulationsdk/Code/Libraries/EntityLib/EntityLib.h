//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityLib.h
 * @brief Core declarations and global settings for the EntityLib module.
 *
 * Provides library-level settings, initialization helpers, and utility functions used to bootstrap EntityLib.
 * The declarations in this header expose the global resources that EntityLib consults at runtime and the
 * entry points used to initialize and tear down those bindings.
 *
 * @see sbio::entity::CEntityManager
 * @see sbio::entity::SEntityLibSettings
 * @see sbio::entity::SEntityLibParams
 * @see InitEntityLib
 * @see UninitEntityLib
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_LIB_H
#define SIMBLOCKS_ENTITY_LIB_H

#include "EntityLib/EntityDeclarations.h"
#include "GlobalHeaders/CommonDeclarations.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>
#include <string>
#include <filesystem>

namespace sbio
{
  namespace entity
  {
    /**
     * @brief Global runtime settings consulted by EntityLib.
     *
     * Ownership:
     * - All pointers are non-owning.
     * - The pointed-to objects must outlive any EntityLib code that uses them.
     *
     * Invariants:
     * - `dataPath` identifies the directory from which EntityLib loads its data files.
     */
    struct SEntityLibSettings
    {
      std::filesystem::path dataPath;///< Path to entity data
      sbio::utils::CEventDispatcher* pEventDispatcher = nullptr;///< Pointer to event dispatcher
      CEntityManager* pEntityManager = nullptr;///< Pointer to entity manager
      sbio::utils::CLogger* pLogger = nullptr;///< Pointer to logger
    };

    /**
     * @brief Parameters supplied to `InitEntityLib()`.
     *
     * Ownership:
     * - `pEntityManager` shares ownership with the caller.
     * - `InitEntityLib()` stores only the raw pointer obtained from this shared pointer.
     */
    struct SEntityLibParams
    {
      std::shared_ptr<sbio::entity::CEntityManager> pEntityManager;///< Shared pointer to entity manager
    };
  }
}

/**
 * @brief Converts a textual clamp name to an `EClamp` value.
 *
 * @param sClamp Clamp name to convert.
 * @return `EClamp::NONE`, `EClamp::CONFORMAL`, or `EClamp::NON_CONFORMAL` for the recognized spellings used by
 *         the implementation; otherwise `EClamp::UNKNOWN`.
 *
 * Failure cases:
 * - Unrecognized strings are not rejected with an error; they map to `EClamp::UNKNOWN`.
 */
sbio::EClamp ToClamp(const std::string& sClamp);

extern sbio::entity::SEntityLibSettings g_EntityLibSettings;

/**
 * @brief Initializes EntityLib global state from application globals and module parameters.
 *
 * @param globals Global application services and paths.
 * @param params EntityLib initialization parameters.
 *
 * Side effects:
 * - Populates `g_EntityLibSettings` with non-owning pointers derived from `globals` and `params`.
 * - Sets `g_EntityLibSettings.dataPath` to `globals.librariesDataPath / "EntityLib"`.
 * - Calls `CEntityManager::Init()` when a manager is supplied.
 */
void InitEntityLib(const sbio::SGlobals& globals, const sbio::entity::SEntityLibParams& params);

/**
 * @brief Clears EntityLib global service pointers.
 *
 * Side effects:
 * - Sets the stored manager, event dispatcher, and logger pointers in `g_EntityLibSettings` to `nullptr`.
 *
 * Ownership:
 * - Does not destroy caller-owned objects.
 */
void UninitEntityLib();

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
