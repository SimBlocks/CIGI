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
     * `InitEntityLib()` populates these settings; default-constructed settings have an empty path
     * and null service pointers.
     */
    struct SEntityLibSettings
    {
      std::filesystem::path dataPath;///< Directory containing `SISO-REF-010.xml` for manager initialization.
      sbio::utils::CEventDispatcher* pEventDispatcher = nullptr;///< Non-owning application event dispatcher.
      CEntityManager* pEntityManager = nullptr;///< Non-owning entity manager; may be null.
      sbio::utils::CLogger* pLogger = nullptr;///< Non-owning logger; diagnostics are skipped when null.
    };

    /**
     * @brief Parameters supplied to `InitEntityLib()`.
     *
     * Ownership:
     * - `pEntityManager` shares ownership with the caller.
     * - `InitEntityLib()` stores only the raw pointer obtained from this shared pointer and does not
     *   extend the manager's lifetime. An owner must keep the manager alive while the global binding is used.
     */
    struct SEntityLibParams
    {
      std::shared_ptr<sbio::entity::CEntityManager> pEntityManager;///< Manager to bind and initialize; may be empty.
    };
  }
}

/**
 * @brief Converts a textual clamp name to an `EClamp` value.
 *
 * @param sClamp Case-sensitive clamp name; no whitespace is trimmed.
 * @return `EClamp::NONE` for `"NONE"` or `"No Clamp"`, `EClamp::CONFORMAL` for `"CONFORMAL"` or
 *         `"Conformal"`, `EClamp::NON_CONFORMAL` for `"NON_CONFORMAL"` or `"Non-Conformal"`,
 *         and `EClamp::UNKNOWN` for any other string.
 */
sbio::EClamp ToClamp(const std::string& sClamp);

/** @brief Global non-owning service bindings and data directory used by EntityLib. */
extern sbio::entity::SEntityLibSettings g_EntityLibSettings;

/**
 * @brief Initializes EntityLib global state from application globals and module parameters.
 *
 * @param globals Application services and paths; bound services must remain alive while EntityLib uses them.
 * @param params Manager binding; a non-null manager must remain alive while EntityLib uses it.
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
 * - Leaves the stored data directory unchanged and does not call `CEntityManager::Reset()`.
 *
 * Ownership:
 * - Does not destroy caller-owned objects.
 */
void UninitEntityLib();

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
