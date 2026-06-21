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
     * @brief Manages views and view groups for the view system.
     *
     * Supports adding, retrieving, and managing views and view groups, as well as update and reset operations.
     */
    class CViewManager
    {
    public:
      /**
       * @brief Adds a view to the manager.
       * @param pView The unique pointer to the view object.
       */
      void AddView(std::unique_ptr<sbio::view::CView> pView);

      /**
       * @brief Adds a view group to the manager.
       * @param pViewGroup The unique pointer to the view group object.
       */
      void AddViewGroup(std::unique_ptr<sbio::view::CViewGroup> pViewGroup);

      /**
       * @brief Gets a view by its ID.
       * @param viewID The view ID.
       * @return Pointer to the view, or nullptr if not found.
       */
      CView* GetView(sbio::ViewID viewID) const;

      /**
       * @brief Gets a view group by its ID.
       * @param viewGroupID The view group ID.
       * @return Pointer to the view group, or nullptr if not found.
       */
      CViewGroup* GetViewGroup(sbio::ViewGroupID viewGroupID) const;

      /**
       * @brief Checks if a view exists in the manager.
       * @param viewID The view ID.
       * @return True if the view exists, false otherwise.
       */
      bool HasView(sbio::ViewID viewID) const;

      /**
       * @brief Removes a view from the manager.
       * @param viewID The view ID.
       * @return True if the view was removed, false otherwise.
       */
      bool RemoveView(sbio::ViewID viewID);

      /**
       * @brief Resets all managed views and view groups.
       */
      void Reset();

      /**
       * @brief Updates all managed views and view groups.
       */
      void Update();

    private:
      std::unordered_map<sbio::ViewID, std::unique_ptr<sbio::view::CView>, StrongTypeHash<sbio::ViewID>> m_Views;///< Map of views
      std::unordered_map<sbio::ViewGroupID, std::unique_ptr<CViewGroup>, StrongTypeHash<sbio::ViewGroupID>> m_ViewGroups;///< Map of view groups
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
