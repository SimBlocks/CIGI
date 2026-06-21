//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolPolygon.h
 * @brief Declares the `CSymbolPolygon` class for polygonal symbol geometry representation.
 *
 * Provides the CSymbolPolygon class, derived from CSymbolGeometry, for representing and managing polygonal symbol geometry.
 * Supports copying properties from other symbol geometries, accessing and setting polygon-specific properties, and integration
 * with symbol rendering systems.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::symbol::SSymbolPolygon
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_POLYGON_H
#define SIMBLOCKS_SYMBOL_POLYGON_H

#include "Symbol.h"
#include "SymbolLib/SymbolTypes.h"
#include "SymbolLib/SymbolGeometry.h"

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Geometry object for polygon symbols.
     *
     * `CSymbolPolygon` stores the current `SSymbolPolygon` definition and keeps the embedded `symbolID`
     * synchronized with the identifier supplied by the caller when constructed or updated.
     *
     * @invariant `m_SymbolPolygon.symbolID` matches the symbol ID last supplied to the constructor or `Set()`.
     */
    class CSymbolPolygon : public sbio::symbol::CSymbolGeometry
    {
    public:
      /**
       * @brief Constructs polygon geometry for a symbol.
       * @param symbolID Symbol ID to store in the embedded `SSymbolPolygon` properties.
       */
      CSymbolPolygon(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Copies polygon properties from another geometry object.
       * @param pSymbolGeometry Source geometry.
       *
       * Copies properties only when `pSymbolGeometry` is a `CSymbolPolygon`. The stored symbol ID is preserved.
       *
       * @ownership `pSymbolGeometry` remains owned by the caller.
       * @failurecases If `pSymbolGeometry` is `nullptr` or does not reference `CSymbolPolygon`, the function has no effect.
       */
      virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) override;

      /**
       * @brief Returns the stored polygon properties.
       * @return Const reference to the internal `SSymbolPolygon` properties.
       *
       * @ownership The returned reference remains owned by this object.
       */
      const sbio::symbol::SSymbolPolygon& GetProperties() const;

      /**
       * @brief Replaces the stored polygon properties.
       * @param symbolPolygonProperties Source property structure.
       * @param symbolID Symbol ID to write into the stored properties after copying.
       *
       * The `symbolID` embedded in `symbolPolygonProperties` is ignored and overwritten with `symbolID`.
       */
      void Set(const sbio::symbol::SSymbolPolygon& symbolPolygonProperties, sbio::symbol::SymbolID symbolID);

    protected:
      sbio::symbol::SSymbolPolygon m_SymbolPolygon;///< Stored polygon properties.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
