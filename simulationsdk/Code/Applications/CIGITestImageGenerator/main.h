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
 * @brief Global state struct for the CIGI test image generator application.
 *
 * Holds pointers to the image generator, entity manager, view manager, and symbol surface manager.
 */
struct STestImageGeneratorGlobals : sbio::SGlobals
{
  sbio::cigi::ig::CCigiImageGenerator* pImageGenerator = nullptr;///< Pointer to the image generator
  std::shared_ptr<sbio::entity::CEntityManager> pEntityManager;///< Shared pointer to the entity manager
  std::shared_ptr<sbio::view::CViewManager> pViewManager;///< Shared pointer to the view manager
  std::shared_ptr<sbio::symbol::CSymbolSurfaceManager> pSymbolSurfaceManager;///< Shared pointer to the symbol surface manager
};

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
