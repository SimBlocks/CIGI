//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolGeometry.h
 * @brief Declares the `CSymbolGeometry` base class for symbol geometry representation.
 *
 * Provides the abstract `CSymbolGeometry` base class used to hold the type-specific properties for a symbol. Concrete
 * subclasses store the property structures for circles, polygons, text, and textured variants while the base class
 * manages the interface for 2D/3D symbol rendering systems.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::symbol::ESymbolType
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_GEOMETRY_H
#define SIMBLOCKS_SYMBOL_GEOMETRY_H

#include "SymbolLib/SymbolTypes.h"
#include <unordered_set>
#include <vector>
#include <memory>

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Abstract base type for symbol geometry objects.
     *
     * `CSymbolGeometry` exposes the common interface used by symbols and geometry factories while leaving storage
     * and copy semantics to concrete subclasses.
     *
     * @invariant `m_eSymbolType` remains the geometry type supplied at construction.
     */
    class CSymbolGeometry
    {
    public:
      /**
       * @brief Constructs a geometry object for the supplied symbol type.
       * @param eSymbolType Declared geometry type.
       */
      CSymbolGeometry(ESymbolType eSymbolType);

      /**
       * @brief Destroys the geometry object.
       */
      virtual ~CSymbolGeometry();

      /**
       * @brief Copies geometry-specific state from another geometry object.
       * @param pSymbolGeometry Source geometry to copy from.
       *
       * Concrete implementations decide how incompatible source types are handled.
       *
       * @ownership `pSymbolGeometry` remains owned by the caller.
       */
      virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) = 0;

      /**
       * @brief Returns the declared geometry type.
       * @return Geometry type supplied at construction.
       */
      ESymbolType GetSymbolType() const;

    private:
      ESymbolType m_eSymbolType = ESymbolType::UNKNOWN;///< Declared geometry type.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
