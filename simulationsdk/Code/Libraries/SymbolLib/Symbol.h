//Copyright SimBlocks LLC 2016-2026
/**
 * @file Symbol.h
 * @brief Declares the `CSymbol` base class and related types for symbol management and rendering.
 *
 * Provides the `CSymbol` base class for middleware symbol representation. A symbol stores shared state such as
 * color, transform, visibility, surface attachment, and parent-child relationships, while delegating type-specific
 * geometry data to a `CSymbolGeometry` instance created by the configured geometry factory.
 *
 * @see sbio::symbol::CSymbolGeometry
 * @see sbio::SColor32
 * @see sbio::math::Vec2f
 * @see sbio::SymbolID
 * @see sbio::SymbolSurfaceID
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_H
#define SIMBLOCKS_SYMBOL_H

#include "SymbolLib/SymbolTypes.h"
#include "SymbolLib/SymbolDeclarations.h"
#include <unordered_set>
#include <vector>
#include <memory>

namespace sbio
{
  namespace symbol
  {
    /**
     * @brief Abstract base type for all managed symbols.
     *
     * `CSymbol` owns the shared state required by the symbol system and exposes the common operations used by
     * `CSymbolSurfaceManager` and CIGI handlers. Concrete symbol types implement cloning behavior while the base
     * class manages visibility state, hierarchical relationships, and the optional geometry object produced from the
     * global `CSymbolGeometryFactory` during construction.
     *
     * @invariant `m_SymbolID` identifies this symbol instance.
     * @invariant `m_eSymbolType` remains the declared type supplied at construction.
     * @invariant `m_Children` contains at most one entry for a given child symbol ID.
     * @ownership `CSymbol` exclusively owns `m_pGeometry` when a geometry factory creates one.
     */
    class CSymbol
    {
    public:
      /**
       * @brief Constructs a symbol with the supplied identifier and declared type.
       * @param symbolID Identifier assigned to the symbol.
       * @param eSymbolType Declared symbol type for this instance.
       *
       * When `g_SymbolLibSettings.pSymbolGeometryFactory` is configured, the constructor requests a matching
       * geometry object and stores it in `m_pGeometry`.
       *
       * @sideeffects May allocate a geometry object through the configured global geometry factory.
       * @failurecases If no geometry factory is configured, `m_pGeometry` remains `nullptr`.
       * @failurecases Unsupported symbol types may cause the factory to return `nullptr`.
       */
      CSymbol(sbio::symbol::SymbolID symbolID, ESymbolType eSymbolType);

      /**
       * @brief Destroys the symbol.
       */
      virtual ~CSymbol() = 0;

      /**
       * @brief Creates a copy of the symbol with a new symbol ID.
       * @param symbolID Identifier to assign to the clone.
       * @return Owning pointer to the cloned symbol.
       *
       * Derived classes are responsible for copying any type-specific state.
       *
       * @ownership Ownership of the returned symbol is transferred to the caller.
       */
      virtual std::unique_ptr<CSymbol> Clone(sbio::symbol::SymbolID symbolID) = 0;

      /**
       * @brief Copies the base symbol state from another symbol.
       * @param pSymbol Source symbol to read from.
       * @param symbolID Identifier to assign to this symbol after the copy.
       *
       * Copies color, position, scale, visibility, rotation, and surface attachment. The copied symbol is detached
       * from any parent, its child set is cleared, and `m_bHiddenByAncestor` is reset to `false`. Geometry is not
       * copied by this function.
       *
       * @ownership `pSymbol` remains owned by the caller.
       * @failurecases `pSymbol` must not be `nullptr`.
       */
      virtual void CopyFrom(CSymbol* pSymbol, sbio::symbol::SymbolID symbolID);

      /**
       * @brief Returns the symbol's effective visibility.
       * @return `true` when the symbol is locally visible and not hidden by an ancestor; otherwise `false`.
       */
      bool GetEffectiveVisibility() const;

      /**
       * @brief Returns the symbol color.
       * @return Const reference to the stored color value.
       *
       * @ownership The returned reference remains owned by the symbol.
       */
      const sbio::SColor32& GetColor() const;

      /**
       * @brief Returns the declared symbol type.
       * @return The symbol type supplied at construction.
       */
      sbio::symbol::ESymbolType GetSymbolType() const;

      /**
       * @brief Replaces the symbol color.
       * @param color New color value.
       */
      virtual void SetColor(const sbio::SColor32& color);

      /**
       * @brief Replaces the symbol position.
       * @param position New symbol position.
       */
      void SetPosition(const sbio::math::Vec2f& position);

      /**
       * @brief Replaces the symbol scale.
       * @param scale New symbol scale.
       */
      void SetScale(const sbio::math::Vec2f& scale);

      /**
       * @brief Returns the current symbol scale.
       * @return Copy of the stored scale vector.
       */
      sbio::math::Vec2f GetScale();

      /**
       * @brief Returns the current symbol position.
       * @return Copy of the stored position vector.
       */
      sbio::math::Vec2f GetPosition();

      /**
       * @brief Sets the symbol's local visibility flag.
       * @param bVisible `true` to mark the symbol visible; `false` to hide it.
       * @param bForceChange Hint used by derived implementations when the caller needs visibility-dependent state to
       * be refreshed even if the stored visibility value is unchanged.
       *
       * The base implementation only updates `m_bVisible` and ignores `bForceChange`.
       */
      virtual void SetVisible(bool bVisible, bool bForceChange = false);

      /**
       * @brief Updates symbol state.
       *
       * The base implementation performs no work.
       */
      virtual void Update();

      /**
       * @brief Records a child relationship.
       * @param symbolID Child symbol ID to add.
       *
       * Duplicate child IDs are ignored by the underlying set.
       */
      void AddChild(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Removes a recorded child relationship.
       * @param symbolID Child symbol ID to remove.
       *
       * @failurecases Removing an ID that is not present has no effect.
       */
      void RemoveChild(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Returns the current child symbol IDs.
       * @return Snapshot of the child IDs stored by the symbol.
       *
       * The returned order is unspecified.
       */
      std::vector<sbio::symbol::SymbolID> GetChildren() const;

      /**
       * @brief Returns the parent symbol ID.
       * @return Parent symbol ID, or `UnknownSymbolID` when the symbol is top-level.
       */
      sbio::symbol::SymbolID GetParentSymbolID() const;

      /**
       * @brief Replaces the parent symbol ID.
       * @param parentSymbolID Parent symbol ID to store, or `UnknownSymbolID` to detach the symbol.
       *
       * This function updates only the stored parent ID. It does not modify the parent's child collection.
       */
      void SetParentSymbolID(sbio::symbol::SymbolID parentSymbolID);

      /**
       * @brief Returns the symbol ID.
       * @return The symbol's identifier.
       */
      sbio::symbol::SymbolID GetSymbolID() const;

      /**
       * @brief Returns the associated geometry object.
       * @return Non-owning pointer to the geometry object, or `nullptr` when no geometry was created.
       *
       * @ownership The returned pointer remains owned by the symbol.
       */
      sbio::symbol::CSymbolGeometry* GetSymbolGeometry() const;

      /**
       * @brief Returns the attached symbol surface ID.
       * @return The stored symbol surface ID, or `UnknownSymbolSurfaceID` when no surface is assigned.
       */
      sbio::symbol::SymbolSurfaceID GetSymbolSurfaceID() const;

      /**
       * @brief Reports whether the symbol has no parent.
       * @return `true` when `GetParentSymbolID()` is `UnknownSymbolID`; otherwise `false`.
       */
      bool IsTopLevel() const;

      /**
       * @brief Returns the symbol's local visibility flag.
       * @return `true` when the symbol is locally marked visible; otherwise `false`.
       */
      bool IsVisible() const;

      /**
       * @brief Returns whether an ancestor currently suppresses this symbol.
       * @return `true` when an ancestor-hidden state is stored; otherwise `false`.
       */
      bool IsHiddenByAncestor() const;

      /**
       * @brief Stores whether an ancestor hides this symbol.
       * @param bHiddenByAncestor `true` when an ancestor should suppress effective visibility.
       */
      void SetHiddenByAncestor(bool bHiddenByAncestor);

      /**
       * @brief Returns the stored rotation.
       * @return Rotation in degrees.
       */
      sbio::math::Degrees GetRotation() const;

      /**
       * @brief Replaces the stored rotation.
       * @param rotation Rotation value to store.
       */
      virtual void SetRotation(sbio::math::Degrees rotation);

      /**
       * @brief Replaces the attached symbol surface ID.
       * @param symbolSurfaceID Surface ID to store.
       *
       * The base implementation performs no validation that the surface exists.
       */
      virtual void SetSymbolSurfaceID(sbio::symbol::SymbolSurfaceID symbolSurfaceID);

    protected:
      sbio::symbol::SymbolID m_SymbolID;///< Unique symbol ID.
      sbio::symbol::ESymbolType m_eSymbolType = sbio::symbol::ESymbolType::UNKNOWN;///< Declared symbol type.
      sbio::SColor32 m_Color;///< Symbol color.
      sbio::math::Vec2f m_Position;///< Symbol position.
      sbio::math::Vec2f m_Scale;///< Symbol scale.
      bool m_bVisible = false;///< Local visibility flag.
      bool m_bHiddenByAncestor = false;///< `true` when an ancestor causes this symbol to be hidden.
      sbio::math::Degrees m_Rotation = UnknownDegrees;///< Symbol rotation.
      sbio::symbol::SymbolSurfaceID m_SymbolSurfaceID = UnknownSymbolSurfaceID;///< Attached symbol surface ID.
      sbio::symbol::SymbolID m_ParentSymbolID = UnknownSymbolID;///< Parent symbol ID.

      std::unordered_set<sbio::symbol::SymbolID, StrongTypeHash<sbio::symbol::SymbolID>> m_Children;///< Child symbol IDs.
      std::unique_ptr<CSymbolGeometry> m_pGeometry;///< Owned geometry created for this symbol, if available.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
