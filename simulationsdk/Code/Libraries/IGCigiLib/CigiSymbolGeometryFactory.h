//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiSymbolGeometryFactory.h
 * @brief Declares the CCigiSymbolGeometryFactory class for SimBlocks CIGI IG symbol geometry creation.
 *
 * Provides the CCigiSymbolGeometryFactory class for creating symbol geometry objects with CIGI-specific logic in the SimBlocks CIGI IG library.
 * Inherits from sbio::symbol::CSymbolGeometryFactory and integrates with SimBlocks symbol types for symbol geometry instantiation and management.
 * Supports creation of symbol geometry objects based on symbol ID and type for simulation symbol rendering.
 *
 * @see sbio::cigi::ig::CCigiSymbolGeometryFactory
 * @see sbio::symbol::CSymbolGeometryFactory
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SymbolID
 * @see sbio::symbol::ESymbolType
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_GEOMETRY_FACTORY_H
#define SIMBLOCKS_CIGI_SYMBOL_GEOMETRY_FACTORY_H

#include "SymbolLib/SymbolGeometryFactory.h"
#include <memory>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Factory that instantiates the CIGI-specific symbol-geometry subclasses.
       */
      class CCigiSymbolGeometryFactory : public sbio::symbol::CSymbolGeometryFactory
      {
      public:
        /**
         * @brief Creates geometry for one symbol.
         * @param symbolID Identifier to assign to the created geometry object.
         * @param eSymbolType Symbol geometry category to instantiate.
         * @return Owning pointer to the created geometry, or null when the type cannot be created.
         */
        virtual std::unique_ptr<sbio::symbol::CSymbolGeometry> Create(sbio::symbol::SymbolID symbolID, sbio::symbol::ESymbolType eSymbolType);
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
