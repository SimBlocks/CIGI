//Copyright SimBlocks LLC 2016-2026
/**
 * @file main.h
 * @brief Declares the STestImageGeneratorGlobals struct for CIGI test image generator global state.
 *
 * Provides the STestImageGeneratorGlobals struct for holding global pointers and managers used by the CIGI test image generator application.
 * Integrates with SimBlocks entity, view, and symbol libraries for simulation management.
 *
 * @see STestImageGeneratorGlobals
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::entity::CEntityManager
 * @see sbio::view::CViewManager
 * @see sbio::symbol::CSymbolSurfaceManager
 */
#pragma once

#include "GlobalHeaders/Globals.h"
#include "EntityLib/EntityDeclarations.h"
#include "ViewLib/ViewDeclarations.h"
#include "SymbolLib/SymbolDeclarations.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"

/**
 * @brief Application services shared by setup code and the test event handler.
 *
 * Inherits paths, logging, and event-dispatch services from SGlobals. Managers are shared-owned; pImageGenerator
 * borrows the instance owned by the application's g_pImageGenerator. Resources accessed by detached test-response
 * workers must remain valid while those workers run; this structure provides no worker synchronization or shutdown.
 */
struct STestImageGeneratorGlobals : sbio::SGlobals
{
  sbio::cigi::ig::CCigiImageGenerator* pImageGenerator = nullptr;///< Non-owning active IG pointer; initially null.
  std::shared_ptr<sbio::entity::CEntityManager> pEntityManager;///< Shared entity manager used for test-volume queries and entity updates.
  std::shared_ptr<sbio::view::CViewManager> pViewManager;///< Shared owner of the application's views and view groups.
  std::shared_ptr<sbio::symbol::CSymbolSurfaceManager> pSymbolSurfaceManager;///< Shared owner of managed symbols and surface-ID records.
};

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
