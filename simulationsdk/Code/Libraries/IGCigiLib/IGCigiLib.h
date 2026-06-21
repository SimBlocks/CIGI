//Copyright SimBlocks LLC 2016-2026
/**
 * @file IGCigiLib.h
 * @brief Declares core types, parameters, and initialization functions for SimBlocks CIGI IG library integration.
 *
 * Provides the SCigiLibParams and SCigiLibGlobals structs for configuring and sharing resources between SimBlocks IG, entity, view, and symbol libraries.
 * Declares functions for initializing, setting, and uninitializing the CIGI IG library.
 * Integrates with SimBlocks simulation SDK for entity type conversion, event messaging, and resource management.
 *
 * @see sbio::cigi::ig::SCigiLibParams
 * @see sbio::cigi::ig::SCigiLibGlobals
 * @see CCigiEntityTypes
 * @see SGlobals
 */
#pragma once
#ifndef SIMBLOCKS_IG_CIGI_LIB_H
#define SIMBLOCKS_IG_CIGI_LIB_H

#include "SymbolLib/SymbolDeclarations.h"
#include "EngineLib/EngineTypeDeclarations.h"
#include "EntityLib/EntityDeclarations.h"
#include "ViewLib/ViewDeclarations.h"
#include "GlobalHeaders/CommonDeclarations.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "CigiLib/CigiEntityTypes.h"
#include "MathLib/Math.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <string>
#include <memory>
#include <filesystem>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Supplies the externally owned parameters required to initialize `IGCigiLib`.
       */
      struct SIGCigiLibParams
      {
        sbio::ig::CImageGeneratorEventMessenger* pEventMessenger = nullptr;///< Non-owning messenger used to publish IG events.
        std::shared_ptr<sbio::entity::CEntityManager> pEntityManager;///< Shared entity manager used by the CIGI IG library.
        std::shared_ptr<sbio::view::CViewManager> pViewManager;///< Shared view manager used by the CIGI IG library.
        std::shared_ptr<sbio::symbol::CSymbolSurfaceManager> pSymbolSurfaceManager;///< Shared symbol-surface manager used by the CIGI IG library.
      };

      /**
       * @brief Stores process-wide state shared by the CIGI IG library after initialization.
       */
      struct SIGCigiLibGlobals
      {
        std::filesystem::path dataPath;///< Library data root for `IGCigiLib` resources.
        std::filesystem::path applicationDataPath;///< Application-specific data root used by the library.
        sbio::utils::CEventDispatcher* pEventDispatcher = nullptr;///< Non-owning event dispatcher used for CIGI event registration.
        sbio::entity::CEntityManager* pEntityManager = nullptr;///< Non-owning entity manager.
        sbio::view::CViewManager* pViewManager = nullptr;///< Non-owning view manager.
        sbio::symbol::CSymbolSurfaceManager* pSymbolSurfaceManager = nullptr;///< Non-owning symbol-surface manager.
        sbio::utils::CLogger* pLogger = nullptr;///< Non-owning logger.
        sbio::ig::CImageGeneratorEventMessenger* pEventMessenger = nullptr;///< Non-owning event messenger.
        std::unique_ptr<sbio::math::CProjection> pProjection;///< Owned projection instance for the active database, if any.
        sbio::cigi::ig::CCigiImageGenerator* pImageGenerator = nullptr;///< Non-owning pointer to the active CIGI image generator.
        std::unique_ptr<sbio::cigi::ig::CCigiMessageLogger> pCigiMessageLogger;///< Owned human-readable CIGI message logger when enabled.
        std::unique_ptr<CCigiEntityTypes> pCigiEntityTypes;///< Owned entity-type conversion helper.
      };
    }
  }
}

/**
 * @brief Initializes the global `IGCigiLib` state.
 * @param globals Shared application globals supplied by the surrounding SDK.
 * @param params Library-specific dependencies and managers.
 * @return Pointer to the process-wide `SCigiLibGlobals` instance.
 */
sbio::cigi::ig::SIGCigiLibGlobals* InitIGCigiLib(const sbio::SGlobals& globals, const sbio::cigi::ig::SIGCigiLibParams& params);
/**
 * @brief Records the active image generator in the global library state.
 * @param pImageGenerator Non-owning pointer to the active image generator.
 */
void SetIGCigiLibImageGenerator(sbio::cigi::ig::CCigiImageGenerator* pImageGenerator);
/**
 * @brief Releases library-owned global resources and clears cached non-owning pointers.
 */
void UninitIGCigiLib();

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
