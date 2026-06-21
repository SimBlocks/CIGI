//Copyright SimBlocks LLC 2016-2026
/**
 * @file EngineLib.h
 * @brief Main include file for the EngineLib module, providing an abstraction layer on top of the underlying engine.
 *
 * This header defines global structures and initialization functions for the engine library, which acts as an abstraction
 * layer over the core engine. It provides access to shared loggers, event messengers, and configuration parameters, and
 * serves as the entry point for initializing and uninitializing the engine library, as well as for accessing engine-wide resources.
 */
#ifndef SIMBLOCKS_ENGINE_LIB_H
#define SIMBLOCKS_ENGINE_LIB_H

#include "EngineLib/EngineTypeDeclarations.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EntityLib/EntityDeclarations.h"
#include "GlobalHeaders/CommonDeclarations.h"
#include "MathLib/MathTypes.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>

namespace sbio
{
  namespace engine
  {
    /**
     * @brief Parameters for initializing the engine library.
     *
     * `SEngineLibParams` supplies the externally owned dependencies that `EngineLib` caches in its
     * process-wide global state.
     *
     * Ownership:
     * - `pEventMessenger` is non-owning and must remain valid while `EngineLib` uses it.
     */
    struct SEngineLibParams
    {
      sbio::ig::CImageGeneratorEventMessenger* pEventMessenger = nullptr;///< Pointer to the image generator event messenger.
    };

    /**
     * @brief Global engine resources and shared pointers.
     *
     * `SEngineLibGlobals` exposes the shared services that manual `EngineLib` components consult at
     * runtime.
     *
     * Ownership:
     * - `pLogger` shares ownership with the application.
     * - `pImageGeneratorEventMessenger` is non-owning.
     *
     * Invariants:
     * - Members are `nullptr` until `InitEngineLib()` assigns them or `UninitEngineLib()` clears them.
     */
    struct SEngineLibGlobals
    {
      std::shared_ptr<sbio::utils::CLogger> pLogger = nullptr;///< Shared pointer to the logger.
      sbio::ig::CImageGeneratorEventMessenger* pImageGeneratorEventMessenger = nullptr;///< Pointer to the image generator event messenger.
    };
  }
}

/**
 * @brief Initializes the engine library with global settings and parameters.
 * @param globals Global settings and shared resource pointers.
 * @param params Engine library initialization parameters.
 *
 * Side effects:
 * - Copies the shared logger from `globals` into `g_EngineLibGlobals`.
 * - Stores the non-owning image-generator event messenger from `params`.
 */
void InitEngineLib(const sbio::SGlobals& globals, const sbio::engine::SEngineLibParams& params);

/**
 * @brief Uninitializes the engine library and releases global resources.
 *
 * Side effects:
 * - Clears the cached logger and image-generator event messenger from `g_EngineLibGlobals`.
 */
void UninitEngineLib();

/**
 * @brief Global instance of engine library globals, providing access to shared resources.
 *
 * This object is populated by `InitEngineLib()` and consulted by `EngineLib` components such as
 * `CDatabaseLoader`.
 */
extern sbio::engine::SEngineLibGlobals g_EngineLibGlobals;

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
