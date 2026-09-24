//Copyright SimBlocks LLC 2016-2026
/**
 * @file Globals.h
 * @brief Declares global settings and shared resource pointers for the `sbio` namespace.
 *
 * Defines the SGlobals structure, which holds paths and shared pointers to global resources
 * such as event dispatchers and loggers. Used for application-wide configuration and access
 * to shared utilities.
 */
#pragma once
#ifndef SIMBLOCKS_COMMON_GLOBALS_H
#define SIMBLOCKS_COMMON_GLOBALS_H

#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>
#include <string>
#include <filesystem>

/**
 * @namespace sbio
 * @brief The highest level namespace for all SimBlocks.io SDK code.
 *
 * All core types, utilities, and modules in the SimBlocks SDK are defined within this namespace or its sub-namespaces.
 */
namespace sbio
{
  /**
   * @brief Bundles global paths and shared services used across an application.
   *
   */
  struct SGlobals
  {
    std::filesystem::path executablePath;///< Path to executable
    std::filesystem::path thirdPartyPath;///< Path to third-party libraries
    std::filesystem::path applicationsDataPath;///< Path containing application data directories.
    std::filesystem::path currentApplicationDataPath;///< Path to the data directory for the active application.
    std::filesystem::path librariesDataPath;///< Path containing shared library data.
    std::shared_ptr<sbio::utils::CEventDispatcher> pEventDispatcher;///< Shared event dispatcher; null until supplied by the caller.
    std::shared_ptr<sbio::utils::CLogger> pLogger;///< Shared logger; null until supplied by the caller.
  };
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
