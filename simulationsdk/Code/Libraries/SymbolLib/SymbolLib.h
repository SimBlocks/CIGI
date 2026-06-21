//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolLib.h
 * @brief Core declarations and global settings for the `SymbolLib` library.
 *
 * Provides global settings, initialization, and utility functions for the symbol system, including
 * symbol geometry factory integration and data path configuration. Declares SSymbolLibSettings for
 * configuring and initializing the symbol library, and exposes global functions for library setup and teardown.
 *
 * @see sbio::symbol::CSymbolGeometryFactory
 * @see sbio::symbol::SSymbolLibSettings
 * @see InitSymbolLib
 * @see UninitSymbolLib
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_LIB_H
#define SIMBLOCKS_SYMBOL_LIB_H

#include "GlobalHeaders/CommonDeclarations.h"
#include "SymbolLib/SymbolDeclarations.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <string>
#include <memory>
#include <filesystem>

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Global configuration and shared resources used by `SymbolLib`.
     *
     * This structure mirrors the process-wide state stored in `g_SymbolLibSettings`.
     *
     * @ownership `pLogger` is shared with the rest of the process through `std::shared_ptr`.
     * @ownership `pSymbolGeometryFactory` is exclusively owned by `SymbolLib` once moved into the global settings.
     */
    struct SSymbolLibSettings
    {
      std::filesystem::path dataPath;///< Root data path used by `SymbolLib`.
      std::shared_ptr<sbio::utils::CLogger> pLogger;///< Shared logger used for `SymbolLib` diagnostics.
      std::unique_ptr<sbio::symbol::CSymbolGeometryFactory> pSymbolGeometryFactory;///< Owned geometry factory used when symbols create geometry objects.
    };
  }
}

/**
 * @brief Initializes the process-wide `SymbolLib` settings.
 * @param globals Global application services and paths.
 * @param pSymbolGeometryFactory Geometry factory to install for future symbol construction.
 *
 * Copies `globals.librariesDataPath`, shares `globals.pLogger`, and stores the supplied geometry factory in the
 * global `g_SymbolLibSettings` object.
 *
 * @ownership Ownership of `pSymbolGeometryFactory` is transferred to `SymbolLib`.
 * @sideeffects Replaces any previously installed geometry factory.
 */
void InitSymbolLib(const sbio::SGlobals& globals, std::unique_ptr<sbio::symbol::CSymbolGeometryFactory> pSymbolGeometryFactory);

/**
 * @brief Clears the process-wide `SymbolLib` settings.
 *
 * Resets the stored data path, releases the shared logger reference, and destroys the installed geometry factory.
 *
 * @sideeffects Releases global `SymbolLib` resources.
 */
void UninitSymbolLib();

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
