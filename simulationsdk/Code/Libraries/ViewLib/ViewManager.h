//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewManager.h
 * @brief Declares the CViewManager class for managing views and view groups in the view system.
 *
 * Provides the CViewManager class for adding, retrieving, and managing views and view groups. Supports view lookup,
 * group management, and update/reset operations. Enables extensibility for custom view and group logic and integration
 * with the view management system.
 *
 * @see sbio::view::CViewManager
 * @see sbio::view::CView
 * @see sbio::view::CViewGroup
 * @see sbio::ViewID
 * @see sbio::ViewGroupID
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_LIB_VIEW_MANAGER_H
#define SIMBLOCKS_VIEW_LIB_VIEW_MANAGER_H

#include "ViewLib/ViewDeclarations.h"
#include "ViewLib/ViewTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include <memory>
#include <set>
#include <unordered_map>

namespace sbio
{
  namespace view
  {
    /**
     * @brief Owns views and view groups indexed by their respective identifiers.
     *
     * Insertion does not replace an existing object with the same ID. View removal updates all registered groups
     * before destroying the view. Reset and update delegate to groups first, then views, without clearing either map.
     * Destroying the manager destroys all objects it owns.
     */
    class CViewManager
    {
    public:
      /**
       * @brief Adds a view to the manager.
       * @param pView Owning pointer consumed by this call; null is ignored.
       *
       * Stores the view under its `GetViewID()` if the ID is neither unknown nor already registered. An unknown ID
       * or duplicate leaves the manager unchanged and destroys the supplied object. Successful insertion transfers
       * ownership to the manager; it does not add the view to any group.
       */
      void AddView(std::unique_ptr<sbio::view::CView> pView);

      /**
       * @brief Adds a view group to the manager.
       * @param pViewGroup Owning pointer consumed by this call; null is ignored.
       *
       * Stores the group under its `GetViewGroupID()` if the ID is neither unknown nor already registered. An
       * unknown ID or duplicate leaves the manager unchanged and destroys the supplied object. Member IDs and
       * the center are retained as supplied, without checking whether they resolve to managed views.
       */
      void AddViewGroup(std::unique_ptr<sbio::view::CViewGroup> pViewGroup);

      /**
       * @brief Gets a view by its ID.
       * @param viewID Identifier to look up.
       * @return Borrowed pointer to the managed view, or nullptr if absent. A returned view remains valid until
       * removed or the manager is destroyed; the const manager accessor does not make the view read-only.
       */
      CView* GetView(sbio::ViewID viewID) const;

      /**
       * @brief Gets a view group by its ID.
       * @param viewGroupID Identifier to look up.
       * @return Borrowed pointer to the managed group, or nullptr if absent. A returned group remains valid until
       * the manager is destroyed; the const manager accessor does not make the group read-only.
       */
      CViewGroup* GetViewGroup(sbio::ViewGroupID viewGroupID) const;

      /**
       * @brief Checks if a view exists in the manager.
       * @param viewID The view ID.
       * @return True if the view exists, false otherwise.
       */
      bool HasView(sbio::ViewID viewID) const;

      /**
       * @brief Removes a view ID from every registered group, then destroys the managed view.
       * @param viewID Identifier of the view to remove.
       * @return True after removal; false for `UnknownViewID` or an unregistered ID, without modifying groups.
       *
       * Calls each group's virtual `RemoveViewID()` before erasing the view. Base groups update their center if
       * needed. Pointers previously returned for the removed view become invalid; the group objects are retained.
       */
      bool RemoveView(sbio::ViewID viewID);

      /**
       * @brief Calls `Reset()` on every group, then every view, without removing any objects.
       *
       * Order within each map is unspecified. Reset behavior is determined by the registered objects' virtual methods.
       */
      void Reset();

      /**
       * @brief Calls `Update()` on every group, then every view.
       *
       * Order within each map is unspecified. Update behavior is determined by the registered objects' virtual methods.
       */
      void Update();

    private:
      std::unordered_map<sbio::ViewID, std::unique_ptr<sbio::view::CView>, StrongTypeHash<sbio::ViewID>> m_Views;///< Owned views keyed by their identifiers at insertion.
      std::unordered_map<sbio::ViewGroupID, std::unique_ptr<CViewGroup>, StrongTypeHash<sbio::ViewGroupID>> m_ViewGroups;///< Owned groups keyed by their identifiers at insertion.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
