//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolDeclarations.h
 * @brief Forward declarations and type definitions for symbol-related types in the sbio::symbol namespace.
 *
 * Provides forward declarations for symbol state enums, core symbol classes, geometry, and surface managers.
 * This header reduces dependencies and improves compile times by avoiding unnecessary includes in files
 * that only require type information for symbols and related types.
 *
 * @see sbio::symbol namespace
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_DECLARATIONS_H
#define SIMBLOCKS_SYMBOL_DECLARATIONS_H

namespace sbio
{
  namespace symbol
  {
    enum class ESymbolState;

    class CSymbol;
    class CSymbolSurfaceManager;
    class CSymbolGeometry;
    class CSymbolGeometryFactory;
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
