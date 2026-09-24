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
#include <optional>

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
     * Parent and child IDs are non-owning bookkeeping. Base setters do not resolve those IDs, maintain reciprocal
     * relationships, propagate visibility to descendants, or send rendering updates. Tree-level visibility updates
     * are provided by `CSymbolSurfaceManager`.
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
       * @brief Destroys the symbol and its owned geometry, without modifying other symbols' relationships.
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
       * @param pSymbol Non-null, borrowed source symbol to read from.
       * @param symbolID Identifier to assign to this symbol after the copy.
       *
       * Copies color, position, scale, visibility, rotation, and surface attachment. The copied symbol is detached
       * from any parent, its child set is cleared, and `m_bHiddenByAncestor` is reset to `false`. Geometry is not
       * copied by this function, and the declared symbol type is unchanged. No manager key or reciprocal parent/child
       * relationship is updated; callers copying into a managed symbol must keep those records consistent.
       *
       * @ownership No ownership of `pSymbol` is transferred.
       * @failurecases `pSymbol` must not be `nullptr`.
       */
      virtual void CopyFrom(CSymbol* pSymbol, sbio::symbol::SymbolID symbolID);

      /**
       * @brief Returns the symbol's effective visibility.
       * @return `true` when the symbol is locally visible and not hidden by an ancestor; otherwise `false`.
       *
       * Uses the stored ancestor-hidden flag; it does not traverse parents or check surface assignment.
       */
      bool GetEffectiveVisibility() const;

      /**
       * @brief Returns the symbol color.
       * @return Borrowed reference to the stored color, valid for this symbol's lifetime and reflecting later changes.
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
       * The child is not looked up, reparented, or owned by this symbol; cycles are not rejected.
       */
      void AddChild(sbio::symbol::SymbolID symbolID);

      /**
       * @brief Removes a recorded child relationship.
       * @param symbolID Child symbol ID to remove.
       *
       * Does not modify the child's stored parent ID or visibility.
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
       * @return Parent symbol ID, or `std::nullopt` when the symbol is top-level.
       */
      std::optional<sbio::symbol::SymbolID> GetParentSymbolID() const;

      /**
       * @brief Replaces the parent symbol ID.
       * @param parentSymbolID Parent symbol ID to store, or `std::nullopt` to detach the symbol.
       *
       * This function updates only the stored parent ID. It does not modify the parent's child collection.
       * The ID need not resolve to a managed symbol, and the ancestor-hidden flag is not recomputed.
       */
      void SetParentSymbolID(std::optional<sbio::symbol::SymbolID> parentSymbolID);

      /**
       * @brief Returns the symbol ID.
       * @return The symbol's identifier.
       */
      sbio::symbol::SymbolID GetSymbolID() const;

      /**
       * @brief Returns the associated geometry object.
       * @return Borrowed pointer to the geometry object, or `nullptr` when no geometry was created.
       * The pointer remains valid until the geometry is replaced by a derived class or the symbol is destroyed.
       *
       * @ownership The returned pointer remains owned by the symbol.
       */
      sbio::symbol::CSymbolGeometry* GetSymbolGeometry() const;

      /**
       * @brief Returns the attached symbol surface ID.
       * @return The stored symbol surface ID, or `UnknownSymbolSurfaceID` when no surface is assigned.
       *
       * Use `HasSymbolSurfaceID()` to distinguish an unassigned surface from surface ID 65535.
       */
      sbio::symbol::SymbolSurfaceID GetSymbolSurfaceID() const;

      /**
       * @brief Reports whether a symbol surface ID has been assigned.
       * @return `true` while a surface is assigned, including surface ID 65535; `false` initially or after clearing it.
       */
      bool HasSymbolSurfaceID() const;

      /**
       * @brief Reports whether the symbol has no parent.
       * @return `true` when `GetParentSymbolID()` has no value; otherwise `false`.
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
       *
       * Only stores the flag; it neither calls `SetVisible()` nor updates descendants.
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
       * Marks the surface as assigned. All 16-bit IDs, including 65535, are valid assignments.
       * The base implementation performs no validation that the surface exists.
       */
      virtual void SetSymbolSurfaceID(sbio::symbol::SymbolSurfaceID symbolSurfaceID);

      /**
       * @brief Clears the surface assignment without changing the symbol's parent or local visibility.
       *
       * The base implementation stores `UnknownSymbolSurfaceID` and clears the assignment flag. It does not remove
       * the surface from a manager or modify descendant symbols.
       */
      virtual void ClearSymbolSurfaceID();

    protected:
      sbio::symbol::SymbolID m_SymbolID;///< Unique symbol ID.
      sbio::symbol::ESymbolType m_eSymbolType = sbio::symbol::ESymbolType::UNKNOWN;///< Declared symbol type.
      sbio::SColor32 m_Color;///< Symbol color.
      sbio::math::Vec2f m_Position = sbio::math::Vec2f::Zero();///< Symbol position.
      sbio::math::Vec2f m_Scale = sbio::math::Vec2f::Ones();///< Symbol scale.
      bool m_bVisible = false;///< Local visibility flag.
      bool m_bHiddenByAncestor = false;///< `true` when an ancestor causes this symbol to be hidden.
      sbio::math::Degrees m_Rotation = UnknownDegrees;///< Symbol rotation.
      sbio::symbol::SymbolSurfaceID m_SymbolSurfaceID = UnknownSymbolSurfaceID;///< Attached symbol surface ID.
      bool m_bSymbolSurfaceAssigned = false;///< Distinguishes an unassigned surface from any valid 16-bit surface ID.
      std::optional<sbio::symbol::SymbolID> m_ParentSymbolID;///< Parent symbol ID, or no value for a top-level symbol.

      std::unordered_set<sbio::symbol::SymbolID, StrongTypeHash<sbio::symbol::SymbolID>> m_Children;///< Child symbol IDs.
      std::unique_ptr<CSymbolGeometry> m_pGeometry;///< Owned geometry created for this symbol, if available.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
