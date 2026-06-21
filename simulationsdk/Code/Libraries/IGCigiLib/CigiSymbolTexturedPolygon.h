//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiSymbolTexturedPolygon.h
 * @brief Declares the CIGCigiSymbolTexturedPolygon class for SimBlocks CIGI IG symbol textured polygon management and geometry.
 *
 * Provides the CIGCigiSymbolTexturedPolygon class for representing and managing textured polygon symbols with CIGI-specific logic in the SimBlocks CIGI IG library.
 * Inherits from sbio::symbol::CSymbolTexturedPolygon and integrates with SimBlocks symbol geometry types for symbol rendering and manipulation.
 * Supports symbol geometry copying, symbol creation, update, and ID management for simulation symbols.
 *
 * @see sbio::cigi::ig::CIGCigiSymbolTexturedPolygon
 * @see sbio::symbol::CSymbolTexturedPolygon
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SymbolID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_TEXTURED_POLYGON_H
#define SIMBLOCKS_CIGI_SYMBOL_TEXTURED_POLYGON_H

#include "SymbolLib/SymbolGeometry.h"
#include "SymbolLib/SymbolTexturedPolygon.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Textured-polygon specialization used for CIGI symbol packets.
       */
      class CIGCigiSymbolTexturedPolygon : public sbio::symbol::CSymbolTexturedPolygon
      {
      public:
        /**
         * @brief Constructs a CIGI symbol textured polygon.
         * @param symbolID Unique symbol identifier.
         */
        CIGCigiSymbolTexturedPolygon(sbio::symbol::SymbolID symbolID);

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
