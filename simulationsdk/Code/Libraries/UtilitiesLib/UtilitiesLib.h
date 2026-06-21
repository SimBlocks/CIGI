//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_UTILITIES_LIB_H
#define SIMBLOCKS_UTILITIES_LIB_H

#include "GlobalHeaders/CommonDeclarations.h"
#include "UtilitiesDeclarations.h"
#include "FileSystemUtilities.h"
#include "StringUtilities.h"
#include <memory>
#include <string>
#include <list>
#include <vector>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Shared utility services exposed globally within `UtilitiesLib`.
     *
     * This structure stores shared pointers copied from `sbio::SGlobals` so utility code can access
     * the process-wide event dispatcher and logger without receiving them explicitly.
     *
     * @ownership Each member shares ownership with the corresponding object supplied to `InitUtilitiesLib()`.
     * @invariant Members may be `nullptr` before initialization and after `UninitUtilitiesLib()`.
     */
    struct SUtilitiesGlobals
    {
      /**
       * @brief Shared event dispatcher used by `UtilitiesLib` event helpers.
       *
       * May be `nullptr` when event dispatch has not been initialized.
       */
      std::shared_ptr<sbio::utils::CEventDispatcher> pEventDispatcher;

      /**
       * @brief Shared logger used by `UtilitiesLib` components for diagnostic output.
       *
       * May be `nullptr` when logging has not been initialized.
       */
      std::shared_ptr<sbio::utils::CLogger> pLogger;
    };

    extern SUtilitiesGlobals g_UtilitiesGlobals;

    /**
     * @brief Copies the process-global utility services into `UtilitiesLib`.
     * @param globals Source global state.
     *
     * `InitUtilitiesLib()` copies the shared event dispatcher and logger pointers from `globals` into
     * `g_UtilitiesGlobals`.
     *
     * @sideeffects Replaces the current global utility pointers used by `UtilitiesLib`.
     */
    void InitUtilitiesLib(const sbio::SGlobals& globals);

    /**
     * @brief Clears the globally stored utility services.
     *
     * Sets `g_UtilitiesGlobals.pEventDispatcher` and `g_UtilitiesGlobals.pLogger` to `nullptr`.
     *
     * @sideeffects Subsequent utility code that depends on the global dispatcher or logger will observe them as unavailable.
     */
    void UninitUtilitiesLib();
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
