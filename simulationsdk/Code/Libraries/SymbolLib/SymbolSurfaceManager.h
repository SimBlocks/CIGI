//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolSurfaceManager.h
 * @brief Declares the `CSymbolSurfaceManager` class for managing symbol surfaces and symbol objects.
 *
 * Provides the `CSymbolSurfaceManager` class for adding, removing, and querying symbols and symbol surfaces.
 * The manager owns the symbol instances registered with it and provides the tree maintenance operations used to
 * propagate ancestor-driven visibility state.
 *
 * @see sbio::symbol::CSymbol
 * @see sbio::symbol::CSymbolSurfaceManager
 * @see sbio::SymbolID
 * @see sbio::SymbolSurfaceID
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_SURFACE_MANAGER_H
#define SIMBLOCKS_SYMBOL_SURFACE_MANAGER_H

#include "SymbolLib/SymbolDeclarations.h"
#include "SymbolLib/SymbolTypes.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Owns symbol instances and tracks known symbol surfaces.
     *
     * `CSymbolSurfaceManager` is the central container for `SymbolLib` runtime state. It stores symbols keyed by
     * `SymbolID`, records known `SymbolSurfaceID` values, and provides helper operations for detaching symbols and
     * recomputing inherited visibility across parent-child symbol trees.
     *
     * @invariant Each key in `m_Symbols` is unique and owns exactly one `CSymbol` instance.
     * @invariant `m_SymbolSurfaces` contains no duplicate surface IDs.
     * @ownership The manager exclusively owns every symbol stored in `m_Symbols`.
     */
    class CSymbolSurfaceManager
    {
    public:
      typedef std::unordered_set<sbio::symbol::SymbolSurfaceID, StrongTypeHash<sbio::symbol::SymbolSurfaceID>> TSymbolSurfaces;
      typedef std::unordered_map<sbio::symbol::SymbolID, std::unique_ptr<CSymbol>, StrongTypeHash<sbio::symbol::SymbolID>> TSymbols;

      /**
       * @brief Adds a symbol to the manager.
       * @param symbolID Key under which the symbol is stored.
       * @param pSymbol Owning pointer to the symbol instance.
       *
       * The symbol is inserted only when `symbolID` is not already present.
       *
       * @ownership Ownership of `pSymbol` is transferred to the manager on successful insertion.
       * @failurecases If `symbolID` already exists, the insertion is ignored and an error is logged when a global
       * logger is configured.
       * @failurecases When insertion is ignored, the passed `pSymbol` is destroyed when the function returns.
       */
      void AddSymbol(sbio::symbol::SymbolID symbolID, std::unique_ptr<CSymbol> pSymbol);

      /**
       * @brief Records a symbol surface ID.
       * @param symbolSurfaceID Surface ID to add.
       *
       * @failurecases Duplicate surface IDs are ignored.
       */
      void AddSymbolSurface(sbio::symbol::SymbolSurfaceID symbolSurfaceID);

      /**
       * @brief Clears all managed symbols and known symbol surface IDs.
       *
       * @sideeffects Destroys all symbols owned by the manager and removes all surface IDs from the known set.
       * @failurecases This function always succeeds and leaves the manager in an empty state.
       */
      void ClearSymbols();

      /**
       * @brief Clears all known symbol surface IDs.
       *
       * @sideeffects Removes all surface IDs from the known set. 
       * This function does not modify any symbols that may still reference the removed surface IDs.
       * @failurecases This function always succeeds and leaves the manager with an empty set of known surface IDs.
       */
      void ClearSymbolSurfaces();

      /**
       * @brief Returns a managed symbol by ID.
       * @param symbolID Symbol ID to locate.
       * @return Non-owning pointer to the managed symbol, or `nullptr` when the ID is not present.
       *
       * @ownership The returned pointer remains owned by the manager.
       */
      CSymbol* GetSymbol(sbio::symbol::SymbolID symbolID) const;

      /**
       * @brief Gets all managed symbols keyed by symbol ID.
       * @return Const reference to the symbol map.
       */
      const TSymbols& GetSymbols() const;

      /**
       * @brief Gets all recorded symbol surface IDs.
       * @return Const reference to the symbol surface ID set.
       */
      const TSymbolSurfaces& GetSymbolSurfaces() const;

      /**
       * @brief Reports whether a symbol is currently managed.
       * @param symbolID Symbol ID to test.
       * @return `true` when `symbolID` exists in the manager; otherwise `false`.
       */
      bool HasSymbol(sbio::symbol::SymbolID symbolID) const;

      /**
       * @brief Reports whether a symbol surface ID is currently recorded.
       * @param symbolSurfaceID Surface ID to test.
       * @return `true` when `symbolSurfaceID` exists in the manager; otherwise `false`.
       */
      bool HasSymbolSurface(sbio::symbol::SymbolSurfaceID symbolSurfaceID) const;

      /**
       * @brief Removes a recorded symbol surface ID.
       * @param symbolSurfaceID Surface ID to remove.
       *
       * This function does not update symbols that may still reference the removed surface ID.
       *
       * @failurecases Removing an unknown surface ID has no effect.
       */
      void RemoveSymbolSurface(sbio::symbol::SymbolSurfaceID symbolSurfaceID);

      /**
       * @brief Removes a managed symbol.
       * @param symbolID Symbol ID to remove.
       *
       * Before erasing the symbol, the manager removes the symbol from its parent's child set, detaches any direct
       * children whose stored parent matches `symbolID`, clears those children's ancestor-hidden state, and forces a
       * visibility refresh on the detached children.
       *
       * @sideeffects Destroys the removed symbol and may change the parent and visibility-related state of its direct children.
       * @failurecases If `symbolID` is not present, the function returns without modifying the manager.
       */
      void RemoveSymbol(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Recomputes ancestor-hidden state for a symbol subtree.
       * @param symbolID Root symbol ID of the subtree to update.
       *
       * The root symbol's hidden-by-ancestor flag is derived from its parent's effective visibility. Descendants are
       * then processed with a depth-first traversal, using a visited set to avoid infinite loops when circular
       * references exist in the stored relationships.
       *
       * @sideeffects May call `CSymbol::SetVisible(..., true)` on the root and descendants when effective visibility changes.
       * @failurecases If `symbolID` is not present, the function returns without performing any work.
       */
      void UpdateSymbolTreeHiddenByAncestor(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Sets the local visibility of a symbol and refreshes inherited visibility for its subtree.
       * @param symbolID Root symbol ID to update.
       * @param bVisible Local visibility value to apply to the root symbol.
       *
       * @sideeffects Updates the root symbol's local visibility and may update ancestor-hidden state for all reachable descendants.
       * @failurecases If `symbolID` is not present, the function returns without performing any work.
       */
      void SetSymbolTreeVisible(sbio::symbol::SymbolID symbolID, bool bVisible);

      /**
       * @brief Calls `Update()` on every managed symbol.
       *
       * @sideeffects Executes symbol-specific update logic in an unspecified container iteration order.
       */
      void Update();

    private:
      TSymbolSurfaces m_SymbolSurfaces;///< Set of known symbol surface IDs.
      TSymbols m_Symbols;///< Owned symbols keyed by `SymbolID`.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
