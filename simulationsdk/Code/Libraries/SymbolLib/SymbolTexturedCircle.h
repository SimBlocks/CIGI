//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolTexturedCircle.h
 * @brief Declares the `CSymbolTexturedCircle` class for textured circular symbol geometry representation.
 *
 * Provides the CSymbolTexturedCircle class, derived from CSymbolGeometry, for representing and managing textured circular symbol geometry.
 * Supports copying properties from other symbol geometries, accessing and setting textured circle-specific properties, and integration
 * with symbol rendering systems.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::symbol::SSymbolTexturedCircle
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_TEXTURED_CIRCLE_H
#define SIMBLOCKS_SYMBOL_TEXTURED_CIRCLE_H

#include "Symbol.h"
#include "SymbolLib/SymbolGeometry.h"

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Geometry object for textured circle symbols.
     *
     * `CSymbolTexturedCircle` stores the current `SSymbolTexturedCircle` definition and keeps the embedded
     * `symbolID` synchronized with the identifier supplied by the caller when constructed or updated.
     *
     * @invariant `m_SymbolTexturedCircle.symbolID` matches the symbol ID last supplied to the constructor or `Set()`.
     */
    class CSymbolTexturedCircle : public sbio::symbol::CSymbolGeometry
    {
    public:
      /**
       * @brief Constructs textured circle geometry for a symbol.
       * @param symbolID Symbol ID to store in the embedded `SSymbolTexturedCircle` properties.
       */
      CSymbolTexturedCircle(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Copies textured circle properties from another geometry object.
       * @param pSymbolGeometry Source geometry.
       *
       * Copies properties only when `pSymbolGeometry` is a `CSymbolTexturedCircle`. The stored symbol ID is preserved.
       *
       * @ownership `pSymbolGeometry` remains owned by the caller.
       * @failurecases If `pSymbolGeometry` is `nullptr` or does not reference `CSymbolTexturedCircle`, the function has no effect.
       */
      virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) override;

      /**
       * @brief Returns the stored textured circle properties.
       * @return Const reference to the internal `SSymbolTexturedCircle` properties.
       *
       * @ownership The returned reference remains owned by this object.
       */
      const sbio::symbol::SSymbolTexturedCircle& GetProperties() const;

      /**
       * @brief Replaces the stored textured circle properties.
       * @param properties Source property structure.
       * @param symbolID Symbol ID to write into the stored properties after copying.
       *
       * The `symbolID` embedded in `properties` is ignored and overwritten with `symbolID`.
       */
      void Set(const sbio::symbol::SSymbolTexturedCircle& properties, sbio::symbol::SymbolID symbolID);

    protected:
      sbio::symbol::SSymbolTexturedCircle m_SymbolTexturedCircle;///< Stored textured circle properties.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
