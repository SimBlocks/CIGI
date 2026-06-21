//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiSymbolTexturedCircle.h
 * @brief Declares the CIGCigiSymbolTexturedCircle class for SimBlocks CIGI IG symbol textured circle management and geometry.
 *
 * Provides the CIGCigiSymbolTexturedCircle class for representing and managing textured circle symbols with CIGI-specific logic in the SimBlocks CIGI IG library.
 * Inherits from sbio::symbol::CSymbolTexturedCircle and integrates with SimBlocks symbol geometry types for symbol rendering and manipulation.
 * Supports symbol geometry copying, symbol creation, update, and ID management for simulation symbols.
 *
 * @see sbio::cigi::ig::CIGCigiSymbolTexturedCircle
 * @see sbio::symbol::CSymbolTexturedCircle
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SymbolID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_TEXTURED_CIRCLE_H
#define SIMBLOCKS_CIGI_SYMBOL_TEXTURED_CIRCLE_H

#include "SymbolLib/SymbolGeometry.h"
#include "SymbolLib/SymbolTexturedCircle.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Textured-circle specialization used for CIGI symbol packets.
       */
      class CIGCigiSymbolTexturedCircle : public sbio::symbol::CSymbolTexturedCircle
      {
      public:
        /**
         * @brief Constructs a CIGI symbol textured circle.
         * @param symbolID Unique symbol identifier.
         */
        CIGCigiSymbolTexturedCircle(sbio::symbol::SymbolID symbolID);

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
