//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolTexturedPolygon.h
 * @brief Declares the `CSymbolTexturedPolygon` class for textured polygonal symbol geometry representation.
 *
 * Provides the CSymbolTexturedPolygon class, derived from CSymbolGeometry, for representing and managing textured polygonal symbol geometry.
 * Supports copying properties from other symbol geometries, accessing and setting textured polygon-specific properties, and integration
 * with symbol rendering systems.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::symbol::SSymbolTexturedPolygon
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_TEXTURED_POLYGON_H
#define SIMBLOCKS_SYMBOL_TEXTURED_POLYGON_H

#include "Symbol.h"
#include "SymbolLib/SymbolTypes.h"
#include "SymbolLib/SymbolGeometry.h"

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Geometry object for textured polygon symbols.
     *
     * `CSymbolTexturedPolygon` stores the current `SSymbolTexturedPolygon` definition and keeps the embedded
     * `symbolID` synchronized with the identifier supplied by the caller when constructed or updated.
     *
     * @invariant `m_SymbolTexturedPolygon.symbolID` matches the symbol ID last supplied to the constructor or `Set()`.
     */
    class CSymbolTexturedPolygon : public sbio::symbol::CSymbolGeometry
    {
    public:
      /**
       * @brief Constructs textured polygon geometry for a symbol.
       * @param symbolID Symbol ID to store in the embedded `SSymbolTexturedPolygon` properties.
       */
      CSymbolTexturedPolygon(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Copies textured polygon properties from another geometry object.
       * @param pSymbolGeometry Source geometry.
       *
       * Copies properties only when `pSymbolGeometry` is a `CSymbolTexturedPolygon`. The stored symbol ID is preserved.
       *
       * @ownership `pSymbolGeometry` remains owned by the caller.
       * @failurecases If `pSymbolGeometry` is `nullptr` or does not reference `CSymbolTexturedPolygon`, the function has no effect.
       */
      virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) override;

      /**
       * @brief Returns the stored textured polygon properties.
       * @return Const reference to the internal `SSymbolTexturedPolygon` properties.
       *
       * @ownership The returned reference remains owned by this object.
       */
      const sbio::symbol::SSymbolTexturedPolygon& GetProperties() const;

      /**
       * @brief Replaces the stored textured polygon properties.
       * @param properties Source property structure.
       * @param symbolID Symbol ID to write into the stored properties after copying.
       *
       * The `symbolID` embedded in `properties` is ignored and overwritten with `symbolID`.
       */
      void Set(const sbio::symbol::SSymbolTexturedPolygon& properties, sbio::symbol::SymbolID symbolID);

    protected:
      sbio::symbol::SSymbolTexturedPolygon m_SymbolTexturedPolygon;///< Stored textured polygon properties.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
