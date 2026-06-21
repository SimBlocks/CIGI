//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolText.h
 * @brief Declares the `CSymbolText` class for text-based symbol geometry representation.
 *
 * Provides the concrete geometry class that stores `SSymbolTextDefinition` properties for `ESymbolType::TEXT` symbols.
 * Supports copying properties from other symbol geometries, accessing and setting text-specific properties, and integration
 * with symbol rendering systems.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::symbol::SSymbolTextDefinition
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_TEXT_H
#define SIMBLOCKS_SYMBOL_TEXT_H

#include "Symbol.h"
#include "SymbolLib/SymbolTypes.h"
#include "SymbolLib/SymbolGeometry.h"

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Geometry object for text symbols.
     *
     * `CSymbolText` stores the current `SSymbolTextDefinition` and keeps the embedded `symbolID` synchronized with
     * the identifier supplied by the caller when constructed or updated.
     *
     * @invariant `m_SymbolText.symbolID` matches the symbol ID last supplied to the constructor or `Set()`.
     */
    class CSymbolText : public sbio::symbol::CSymbolGeometry
    {
    public:
      /**
       * @brief Constructs text geometry for a symbol.
       * @param symbolID Symbol ID to store in the embedded `SSymbolTextDefinition` properties.
       */
      CSymbolText(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Copies text properties from another geometry object.
       * @param pSymbolGeometry Source geometry.
       *
       * Copies properties only when `pSymbolGeometry` is a `CSymbolText`. The stored symbol ID is preserved.
       *
       * @ownership `pSymbolGeometry` remains owned by the caller.
       * @failurecases If `pSymbolGeometry` is `nullptr` or does not reference `CSymbolText`, the function has no effect.
       */
      virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) override;

      /**
       * @brief Returns the stored text properties.
       * @return Const reference to the internal `SSymbolTextDefinition` properties.
       *
       * @ownership The returned reference remains owned by this object.
       */
      const sbio::symbol::SSymbolTextDefinition& GetProperties() const;

      /**
       * @brief Replaces the stored text properties.
       * @param properties Source property structure.
       * @param symbolID Symbol ID to write into the stored properties after copying.
       *
       * The `symbolID` embedded in `properties` is ignored and overwritten with `symbolID`.
       */
      void Set(sbio::symbol::SSymbolTextDefinition properties, sbio::symbol::SymbolID symbolID);

    protected:
      sbio::symbol::SSymbolTextDefinition m_SymbolText;///< Stored text properties.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
