//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolGeometryFactory.h
 * @brief Declares the `CSymbolGeometryFactory` class for creating symbol geometry objects.
 *
 * Provides the default factory used by `SymbolLib` to construct the concrete `CSymbolGeometry` instance associated
 * with a symbol type.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SymbolID
 * @see sbio::symbol::ESymbolType
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_GEOMETRY_FACTORY_H
#define SIMBLOCKS_SYMBOL_GEOMETRY_FACTORY_H

#include "SymbolLib/SymbolTypes.h"
#include "SymbolLib/SymbolDeclarations.h"
#include <memory>

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Factory for creating concrete symbol geometry objects.
     *
     * The default implementation returns the built-in `SymbolLib` geometry classes for the supported symbol types.
     * Applications can derive from this class to supply alternative geometry implementations.
     */
    class CSymbolGeometryFactory
    {
    public:
      /**
       * @brief Destroys the factory.
       */
      virtual ~CSymbolGeometryFactory() = default;

      /**
       * @brief Creates a geometry object for the supplied symbol type.
       * @param symbolID Symbol ID to write into the created geometry's stored properties.
       * @param eSymbolType Symbol type used to select the concrete geometry class.
       * @return Owning pointer to the created geometry object, or `nullptr` when the type is unsupported.
       *
       * @ownership Ownership of the returned geometry is transferred to the caller.
       * @failurecases Returns `nullptr` for unsupported or unknown symbol types.
       */
      virtual std::unique_ptr<CSymbolGeometry> Create(sbio::symbol::SymbolID symbolID, sbio::symbol::ESymbolType eSymbolType);
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
