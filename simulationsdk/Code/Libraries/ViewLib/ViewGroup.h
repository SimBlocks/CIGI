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
     * @brief Manages a group of views for collective configuration and control.
     *
     * Supports adding/removing views, setting a center view, and updating or resetting the group.
     */
    class CViewGroup
    {
    public:
      /**
       * @brief Constructs a view group with the specified group ID.
       * @param viewGroupID The unique identifier for the view group.
       */
      CViewGroup(sbio::ViewGroupID viewGroupID);

      /**
       * @brief Virtual destructor for CViewGroup.
       */
      virtual ~CViewGroup();

      /**
       * @brief Adds a view ID to the group.
       * @param viewID The view ID to add.
       */
      virtual void AddViewID(sbio::ViewID viewID);

      /**
       * @brief Removes a view ID from the group.
       * @param viewID The view ID to remove.
       */
      virtual void RemoveViewID(sbio::ViewID viewID);

      /**
       * @brief Gets the center view ID of the group.
       * @return The center view ID.
       */
      sbio::ViewID GetCenterViewID() const;

      /**
       * @brief Gets the unique group ID.
       * @return The view group ID.
       */
      sbio::ViewGroupID GetViewGroupID() const;

      /**
       * @brief Gets the set of view IDs in the group.
       * @return Reference to the set of view IDs.
       */
      const std::set<sbio::ViewID>& GetViewIDs() const;

      /**
       * @brief Resets the view group to its default state.
       */
      virtual void Reset();

      /**
       * @brief Sets the center view ID for the group.
       * @param centerViewID The center view ID to set.
       */
      void SetCenterViewID(sbio::ViewID centerViewID);

      /**
       * @brief Updates the view group state.
       */
      virtual void Update();

    protected:
      sbio::ViewGroupID m_ViewGroupID = UnknownViewGroupID;///< Unique view group ID
      std::set<sbio::ViewID> m_ViewIDs;///< Set of view IDs in the group
      sbio::ViewID m_CenterViewID = UnknownViewID;///< Center view ID
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
