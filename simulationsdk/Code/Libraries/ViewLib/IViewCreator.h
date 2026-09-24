//Copyright SimBlocks LLC 2016-2026
/**
 * @file IViewCreator.h
 * @brief Declares the IViewCreator interface for creating CView objects in the view system.
 *
 * Provides the IViewCreator interface for creating CView objects, supporting custom view instantiation
 * and integration with the view management system. Enables extensibility for different view types and creation logic.
 *
 * @see sbio::view::CView
 * @see sbio::ViewID
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_CREATOR_H
#define SIMBLOCKS_VIEW_CREATOR_H

#include "ViewLib/ViewDeclarations.h"
#include <memory>

namespace sbio
{
  namespace view
  {
    /**
     * @brief Interface for creating CView objects in the view system.
     *
     * Implementations select the concrete view type and creation policy. The interface provides no default creation,
     * manager registration, or failure handling.
     */
    struct IViewCreator
    {
      /** @brief Supports destruction of concrete creators through the interface. */
      virtual ~IViewCreator() {};

      /**
       * @brief Creates a new CView object with the specified view ID.
       * @param viewID Identifier requested for the new view; acceptance is defined by the implementation.
       * @return Owning pointer to the created view, transferring ownership to the caller. Whether failure returns
       * nullptr or throws is defined by the concrete creator.
       */
      virtual std::unique_ptr<CView> CreateView(sbio::ViewID viewID) = 0;
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
