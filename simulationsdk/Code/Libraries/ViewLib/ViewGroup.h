//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewGroup.h
 * @brief Declares the CViewGroup class for managing groups of views in the view system.
 *
 * Provides the CViewGroup class for grouping, managing, and configuring multiple views. Supports adding/removing views,
 * setting a center view, and updating or resetting the group. Enables extensibility for custom group logic and integration
 * with the view management system.
 *
 * @see sbio::view::CViewGroup
 * @see sbio::ViewID
 * @see sbio::ViewGroupID
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_GROUP_H
#define SIMBLOCKS_VIEW_GROUP_H

#include "ViewLib/View.h"
#include "MathLib/MathTypes.h"
#include <set>
#include <unordered_map>

namespace sbio
{
  namespace view
  {
    /**
     * @brief Stores a group identifier, a set of view IDs, and a separately selected center view ID.
     *
     * The base class neither owns nor resolves view objects. Membership is deduplicated and ordered by the set's
     * comparator. The center may reference a view outside the set because `SetCenterViewID()` does not enforce
     * membership. Derived classes provide group-specific update and reset behavior.
     */
    class CViewGroup
    {
    public:
      /**
       * @brief Constructs a view group with the specified group ID.
       * @param viewGroupID Identifier to store, without validation or registration.
       *
       * The group initially has no members and its center is `UnknownViewID`.
       */
      CViewGroup(sbio::ViewGroupID viewGroupID);

      /**
       * @brief Destroys the group and its ID set without destroying any referenced views.
       */
      virtual ~CViewGroup();

      /**
       * @brief Adds a view ID to the group.
       * @param viewID Identifier to record; `UnknownViewID` is ignored.
       *
       * Duplicate IDs do not add another member. If the center is unknown, it is set to this ID even when the ID
       * was already present. The base implementation does not check whether the view exists in a manager.
       */
      virtual void AddViewID(sbio::ViewID viewID);

      /**
       * @brief Removes a view ID from the group.
       * @param viewID Identifier to erase; `UnknownViewID` is ignored.
       *
       * If the ID matches the center, the first remaining ID in set order becomes the center, or `UnknownViewID`
       * if the set is empty. This center adjustment also applies when the ID was not a member. Other absent IDs
       * have no effect. No view object is destroyed.
       */
      virtual void RemoveViewID(sbio::ViewID viewID);

      /**
       * @brief Gets the center view ID of the group.
       * @return Stored center identifier, or `UnknownViewID` when unset; membership and view existence are not guaranteed.
       */
      sbio::ViewID GetCenterViewID() const;

      /**
       * @brief Gets the unique group ID.
       * @return Copy of the identifier supplied at construction.
       */
      sbio::ViewGroupID GetViewGroupID() const;

      /**
       * @brief Gets the set of view IDs in the group.
       * @return Borrowed const reference to the live, ordered membership set, valid for the group's lifetime.
       * Later additions and removals are reflected in this set; it is not a snapshot.
       */
      const std::set<sbio::ViewID>& GetViewIDs() const;

      /**
       * @brief Extension point for resetting derived group state.
       *
       * The base implementation does nothing; membership, center, and group ID are preserved.
       */
      virtual void Reset();

      /**
       * @brief Sets the center view ID for the group.
       * @param centerViewID Identifier to store; `UnknownViewID` is ignored and cannot clear the center.
       *
       * The ID need not be a member or resolve to an existing view. Setting it does not add a member.
       */
      void SetCenterViewID(sbio::ViewID centerViewID);

      /**
       * @brief Extension point for updating derived group state.
       *
       * The base implementation does nothing and does not update referenced views.
       */
      virtual void Update();

    protected:
      sbio::ViewGroupID m_ViewGroupID = UnknownViewGroupID;///< Identifier supplied at construction.
      std::set<sbio::ViewID> m_ViewIDs;///< Ordered, unique member IDs; referenced views are not owned.
      sbio::ViewID m_CenterViewID = UnknownViewID;///< Center selection, independent of membership, or UnknownViewID when unset.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
