//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolHandler.h
 * @brief Declares the CCigiSymbolHandler class for SimBlocks CIGI IG symbol management and message handling.
 *
 * Provides the CCigiSymbolHandler class for managing symbols and handling symbol-related messages in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, symbol, and utility types for symbol creation, update, destruction, and message processing.
 * Supports symbol tree management, surface definition handling, control, text, circle, polygon, clone, and reset operations for simulation symbols.
 *
 * @see sbio::cigi::ig::CCigiSymbolHandler
 * @see sbio::symbol::SEntitySymbolSurfaceDefinition
 * @see sbio::symbol::SEntityBillboardSymbolSurfaceDefinition
 * @see sbio::symbol::SViewSymbolSurfaceDefinition
 * @see sbio::symbol::SSymbolControl
 * @see sbio::symbol::SSymbolTextDefinition
 * @see sbio::symbol::SSymbolTexturedCircle
 * @see sbio::symbol::SSymbolCircle
 * @see sbio::symbol::SSymbolPolygon
 * @see sbio::symbol::SSymbolTexturedPolygon
 * @see sbio::symbol::SSymbolClone
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_HANDLER_H
#define SIMBLOCKS_CIGI_SYMBOL_HANDLER_H

#include "CigiLib/CigiLib.h"
#include "SymbolLib/SymbolTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Owns symbol creation, lookup, cloning, and reset behavior for CIGI symbol traffic.
       */
      class CCigiSymbolHandler
      {
      public:
        /**
         * @brief Constructs a symbol handler.
         */
        CCigiSymbolHandler();
        /**
         * @brief Destroys the symbol handler.
         */
        ~CCigiSymbolHandler();

        /**
         * @brief Destroys the symbol tree for a given symbol ID.
         * @param symbolID Symbol identifier.
         */
        void DestroySymbolTree(sbio::symbol::SymbolID symbolID);
        /**
         * @brief Handles entity symbol surface definition message.
         * @param entitySymbolSurfaceDefinition Entity symbol surface definition data.
         */
        void Handle(const sbio::symbol::SEntitySymbolSurfaceDefinition& entitySymbolSurfaceDefinition);
        /**
         * @brief Handles entity billboard symbol surface definition message.
         * @param entityBillboardSurfaceDefinition Entity billboard surface definition data.
         */
        void Handle(const sbio::symbol::SEntityBillboardSymbolSurfaceDefinition& entityBillboardSurfaceDefinition);
        /**
         * @brief Handles view symbol surface definition message.
         * @param viewSymbolSurfaceDefinition View symbol surface definition data.
         */
        void Handle(const sbio::symbol::SViewSymbolSurfaceDefinition& viewSymbolSurfaceDefinition);
        /**
         * @brief Handles symbol control message.
         * @param symbolControl Symbol control data.
         */
        void Handle(const sbio::symbol::SSymbolControl& symbolControl);
        /**
         * @brief Handles symbol text definition message.
         * @param symbolTextDefinition Symbol text definition data.
         */
        void Handle(const sbio::symbol::SSymbolTextDefinition& symbolTextDefinition);
        /**
         * @brief Handles symbol textured circle message.
         * @param symbolTexturedCricle Symbol textured circle data.
         */
        void Handle(const sbio::symbol::SSymbolTexturedCircle& symbolTexturedCricle);
        /**
         * @brief Handles symbol circle message.
         * @param symbolCircle Symbol circle data.
         */
        void Handle(const sbio::symbol::SSymbolCircle& symbolCircle);
        /**
         * @brief Handles symbol polygon message.
         * @param symbolPolygon Symbol polygon data.
         */
        void Handle(const sbio::symbol::SSymbolPolygon& symbolPolygon);
        /**
         * @brief Handles symbol textured polygon message.
         * @param symbolTexturedPolygon Symbol textured polygon data.
         */
        void Handle(const sbio::symbol::SSymbolTexturedPolygon& symbolTexturedPolygon);
        /**
         * @brief Handles symbol clone message.
         * @param symbolClone Symbol clone data.
         */
        void Handle(const sbio::symbol::SSymbolClone& symbolClone);
        /**
         * @brief Resets the symbol handler state.
         */
        void Reset();

      private:
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
