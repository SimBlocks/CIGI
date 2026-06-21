//Copyright SimBlocks LLC 2016-2026
/**
 * @file EventHandler.h
 * @brief Declares the event handler interface for event processing.
 *
 * Defines the EventHandler abstract class, which provides an interface for handling
 * events with associated arguments and event names.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_EVENT_HANDLER_H
#define SIMBLOCKS_UTILITIES_EVENT_HANDLER_H

#include "UtilitiesDeclarations.h"
#include <string>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Abstract visitor used by `Event` dispatch.
     *
     * `EventHandler` provides the named visit operation executed by `Event::Visit(...)`.
     * The pure virtual function has a default implementation in the `.cpp` file that forwards
     * the event arguments to all listeners currently registered for the supplied event name.
     *
     * @note The dispatcher stores handlers through `std::unique_ptr`, so a registered handler has unique ownership within `CEventDispatcher`.
     */
    class EventHandler
    {
    public:
      /**
       * @brief Destroys the handler.
       */
      virtual ~EventHandler()
      {
      }

      /**
       * @brief Processes an event for a named registration key.
       * @param event Event descriptor being dispatched.
       * @param sEvent Registration key used to retrieve listeners.
       * @param args Event argument object passed to listeners.
       *
       * @ownership `event` and `args` remain owned by the caller.
       * @sideeffects Implementations may invoke arbitrary listener or handler code.
       * @failurecases The default implementation returns immediately when no global dispatcher is configured.
       * @failurecases Exceptions from derived implementations or listener callbacks are not intercepted here.
       */
      virtual void Visit(const Event& event, const std::string& sEvent, const EventArgs& args) const = 0;

    protected:
      // No protected members
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
