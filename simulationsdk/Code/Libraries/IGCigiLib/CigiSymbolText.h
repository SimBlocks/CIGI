//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiSymbolText.h
 * @brief Declares the CIGCigiSymbolText class for SimBlocks CIGI IG symbol text management and geometry.
 *
 * Provides the CIGCigiSymbolText class for representing and managing text symbols with CIGI-specific logic in the SimBlocks CIGI IG library.
 * Inherits from sbio::symbol::CSymbolText and integrates with SimBlocks symbol geometry types for symbol rendering and manipulation.
 * Supports symbol geometry copying, symbol creation, update, and ID management for simulation symbols.
 *
 * @see sbio::cigi::ig::CIGCigiSymbolText
 * @see sbio::symbol::CSymbolText
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SymbolID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_TEXT_H
#define SIMBLOCKS_CIGI_SYMBOL_TEXT_H

#include "SymbolLib/SymbolGeometry.h"
#include "SymbolLib/SymbolText.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Text-geometry specialization used for CIGI symbol packets.
       */
      class CIGCigiSymbolText : public sbio::symbol::CSymbolText
      {
      public:
        /**
         * @brief Constructs a CIGI symbol text.
         * @param symbolID Unique symbol identifier.
         */
        CIGCigiSymbolText(sbio::symbol::SymbolID symbolID);

        /**
         * @brief Copies properties from another symbol geometry.
         * @param pSymbolGeometry The symbol geometry to copy from.
         */
        virtual void CopyFrom(CSymbolGeometry* pSymbolGeometry) override;
        /**
         * @brief Sends a create message for the symbol.
         */
        void SendCreate();
        /**
         * @brief Sends an update message for the symbol.
         */
        void SendUpdate();
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
