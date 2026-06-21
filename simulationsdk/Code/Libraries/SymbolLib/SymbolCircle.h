//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolCircle.h
 * @brief Declares the `CSymbolCircle` class for circular symbol geometry representation.
 *
 * Provides the concrete geometry class that stores `SSymbolCircle` properties for `ESymbolType::CIRCLE` symbols.
 * Supports copying properties from other symbol geometries, accessing and setting circle-specific properties, and integration
 * with symbol rendering systems.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::symbol::SSymbolCircle
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_CIRCLE_H
#define SIMBLOCKS_SYMBOL_CIRCLE_H

#include "Symbol.h"
#include "SymbolLib/SymbolTypes.h"
#include "SymbolLib/SymbolGeometry.h"

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Geometry object for circle symbols.
     *
     * `CSymbolCircle` stores the current `SSymbolCircle` definition and keeps the embedded `symbolID` synchronized
     * with the identifier supplied by the caller when constructed or updated.
     *
     * @invariant `m_SymbolCircle.symbolID` matches the symbol ID last supplied to the constructor or `Set()`.
     */
    class CSymbolCircle : public sbio::symbol::CSymbolGeometry
    {
    public:
      /**
       * @brief Constructs circle geometry for a symbol.
       * @param symbolID Symbol ID to store in the embedded `SSymbolCircle` properties.
       */
      CSymbolCircle(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Copies circle properties from another geometry object.
       * @param pSymbolGeometry Source geometry.
       *
       * Copies properties only when `pSymbolGeometry` is a `CSymbolCircle`. The stored symbol ID is preserved.
       *
       * @ownership `pSymbolGeometry` remains owned by the caller.
       * @failurecases If `pSymbolGeometry` is `nullptr` or does not reference `CSymbolCircle`, the function has no effect.
       */
      virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) override;

      /**
       * @brief Returns the stored circle properties.
       * @return Const reference to the internal `SSymbolCircle` properties.
       *
       * @ownership The returned reference remains owned by this object.
       */
      const sbio::symbol::SSymbolCircle& GetProperties() const;

      /**
       * @brief Replaces the stored circle properties.
       * @param properties Source property structure.
       * @param symbolID Symbol ID to write into the stored properties after copying.
       *
       * The `symbolID` embedded in `properties` is ignored and overwritten with `symbolID`.
       */
      void Set(const sbio::symbol::SSymbolCircle& properties, sbio::symbol::SymbolID symbolID);

    protected:
      sbio::symbol::SSymbolCircle m_SymbolCircle;///< Stored circle properties.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
