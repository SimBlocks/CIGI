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
   * `SGlobals` is a passive data carrier. It does not allocate resources, validate paths, or manage
   * service lifetime beyond storing `std::shared_ptr` instances supplied by the caller.
   *
   * Invariants:
   * - Path members may be empty; this type does not enforce path existence or normalization.
   * - `pEventDispatcher` and `pLogger` either hold shared ownership of service instances or are null.
   *
   * Ownership:
   * - Filesystem paths are owned by value.
   * - `pEventDispatcher` and `pLogger` participate in shared ownership with any other holders of the
   *   same `std::shared_ptr` instances.
   *
   * Failure cases:
   * - Consumers must handle missing paths and null shared service pointers.
   */
  struct SGlobals
  {
    std::filesystem::path executablePath;///< Path to the current executable or application root used by the caller.
    std::filesystem::path thirdPartyPath;///< Path to third-party runtime content expected by the application.
    std::filesystem::path applicationsDataPath;///< Path containing application data directories.
    std::filesystem::path currentApplicationDataPath;///< Path to the data directory for the active application.
    std::filesystem::path librariesDataPath;///< Path containing shared library data.
    std::shared_ptr<sbio::utils::CEventDispatcher> pEventDispatcher;///< Shared owner of the process-wide event dispatcher, or `nullptr` when event dispatching is unavailable.
    std::shared_ptr<sbio::utils::CLogger> pLogger;///< Shared owner of the process-wide logger, or `nullptr` when logging is unavailable.
  };
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
