//Copyright SimBlocks LLC 2016-2026
/**
 * @file IEventListener.h
 * @brief Declares the event listener interface for event handling.
 *
 * Defines the IEventListener interface, which provides a virtual OnEvent method
 * for handling events with associated arguments. Intended to be implemented by
 * classes that need to respond to events dispatched by the event system.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_I_EVENT_LISTENER_H
#define SIMBLOCKS_UTILITIES_I_EVENT_LISTENER_H

#include "UtilitiesDeclarations.h"

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Interface implemented by objects that consume dispatched events.
     *
     * `CEventDispatcher` stores raw listener pointers and never takes ownership of them.
     * Implementations are responsible for remaining valid while registered and for unregistering
     * themselves before destruction.
     */
    struct IEventListener
    {
    public:
      /**
       * @brief Destroys the listener.
       */
      virtual ~IEventListener()
      {
      }

      /**
       * @brief Receives event arguments from a dispatcher callback.
       * @param args Event-specific argument object.
       *
       * The default implementation ignores the event.
       *
       * @ownership `args` remains owned by the caller.
       * @failurecases Exceptions from overrides are not intercepted by the dispatcher path implemented in `UtilitiesLib`.
       */
      virtual void OnEvent(const EventArgs& args)
      {
        // Default implementation does nothing.
        // Override this method in derived classes to handle specific events.
      }
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
