//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiSymbolPolygon.h
 * @brief Declares the CIGCigiSymbolPolygon class for SimBlocks CIGI IG symbol polygon management and geometry.
 *
 * Provides the CIGCigiSymbolPolygon class for representing and managing polygon symbols with CIGI-specific logic in the SimBlocks CIGI IG library.
 * Inherits from sbio::symbol::CSymbolPolygon and integrates with SimBlocks symbol geometry types for symbol rendering and manipulation.
 * Supports symbol geometry copying, symbol creation, update, and ID management for simulation symbols.
 *
 * @see sbio::cigi::ig::CIGCigiSymbolPolygon
 * @see sbio::symbol::CSymbolPolygon
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SymbolID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_POLYGON_H
#define SIMBLOCKS_CIGI_SYMBOL_POLYGON_H

#include "SymbolLib/SymbolGeometry.h"
#include "SymbolLib/SymbolPolygon.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Polygon-geometry specialization used for CIGI symbol packets.
       */
      class CIGCigiSymbolPolygon : public sbio::symbol::CSymbolPolygon
      {
      public:
        /**
         * @brief Constructs a CIGI symbol polygon.
         * @param symbolID Unique symbol identifier.
         */
        CIGCigiSymbolPolygon(sbio::symbol::SymbolID symbolID);

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
