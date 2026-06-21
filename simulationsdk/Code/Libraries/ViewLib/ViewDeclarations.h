//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewDeclarations.h
 * @brief Forward declarations and type definitions for view-related types in the sbio::view namespace.
 *
 * Provides forward declarations for projection modes, core view classes, view groups, view managers, and view creators.
 * This header reduces dependencies and improves compile times by avoiding unnecessary includes in files
 * that only require type information for views and related types.
 *
 * @see sbio::view namespace
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_LIB_DECLARATIONS_H
#define SIMBLOCKS_VIEW_LIB_DECLARATIONS_H

namespace sbio
{
  enum class EProjectionMode;

  namespace view
  {
    class CView;
    class CViewGroup;
    class CViewManager;

    struct IViewCreator;
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
