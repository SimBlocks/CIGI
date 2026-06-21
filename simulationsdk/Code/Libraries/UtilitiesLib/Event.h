//Copyright SimBlocks LLC 2016-2026
/**
 * @file Event.h
 * @brief Declares event and event argument types and event notification utilities.
 *
 * Defines the Event and EventArgs classes for representing and handling events,
 * as well as the notifyEvent utility for dispatching events with arguments.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_EVENT_H
#define SIMBLOCKS_UTILITIES_EVENT_H

#include "UtilitiesDeclarations.h"
#include <fstream>
#include <string>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Dispatches an event through the globally configured event dispatcher.
     * @param pEvent Pointer to the event instance describing the dispatch operation.
     * @param sEvent Logical event name used as the registration key.
     * @param args Event-specific argument object passed to listeners.
     *
     * @ownership `pEvent` and `args` remain owned by the caller.
     * @sideeffects Invokes the registered handler for `sEvent` when `g_UtilitiesGlobals.pEventDispatcher` is configured.
     * @failurecases If no global dispatcher is configured, the function returns without dispatching.
     */
    void notifyEvent(Event* pEvent, const std::string& sEvent, const EventArgs& args);

    /**
     * @brief Polymorphic base type for event argument data.
     *
     * Derived types carry the data delivered to listeners through `Event`, `EventHandler`, and `IEventListener`.
     * This base class owns no resources and imposes no behavior beyond providing a virtual destructor.
     */
    class EventArgs
    {
    public:
      /**
       * @brief Destroys the argument object.
       */
      virtual ~EventArgs() {};
    };

    /**
     * @brief Base type for event descriptors dispatched through `UtilitiesLib`.
     *
     * An `Event` instance identifies the dispatch path and optionally customizes how a handler is visited
     * and how event data is written to a stream. The default implementation forwards the named visit to
     * the handler and performs no work for the unnamed visit or for stream output.
     *
     * @note `Event` instances are passed by pointer during dispatch but are not owned by the dispatcher.
     */
    class Event
    {
    public:
      /**
       * @brief Constructs an event descriptor.
       */
      Event();

      /**
       * @brief Raises an event of type `TEvent` through the global dispatcher.
       * @tparam TEvent Event type to instantiate and dispatch.
       * @param args Event argument object forwarded to the handler and listeners.
       *
       * This function default-constructs a temporary `TEvent` and dispatches it using `TEvent::GetStaticName()`.
       *
       * @ownership `args` remains owned by the caller.
       * @sideeffects May invoke the registered event handler and any listeners reached by that handler.
       * @failurecases If no global dispatcher is configured, dispatch is skipped.
       * @failurecases `TEvent` must be default-constructible and provide `static GetStaticName()`.
       */
      template <typename TEvent>
      static void Raise(const EventArgs& args)
      {
        TEvent t;
        notifyEvent(&t, TEvent::GetStaticName(), args);
      }

      /**
       * @brief Writes event-specific data to a stream.
       * @param stream Output stream to receive serialized event data.
       *
       * The base implementation is a no-op.
       *
       * @ownership The stream remains owned by the caller.
       * @failurecases The base implementation performs no error reporting.
       */
      virtual void Write(std::ofstream& stream) const;

      /**
       * @brief Visits a handler without an explicit event name.
       * @param handler Handler to visit.
       * @param args Event argument object.
       *
       * The base implementation is a no-op.
       *
       * @ownership `handler` and `args` remain owned by the caller.
       */
      virtual void Visit(const EventHandler& handler, const EventArgs& args) const;

      /**
       * @brief Visits a handler using an explicit event name.
       * @param handler Handler to visit.
       * @param sEvent Logical event name used for dispatch.
       * @param args Event argument object.
       *
       * The base implementation forwards directly to `handler.Visit(*this, sEvent, args)`.
       *
       * @ownership `handler` and `args` remain owned by the caller.
       * @sideeffects Executes handler code.
       * @failurecases `handler` exceptions are not intercepted by this function.
       */
      virtual void Visit(const EventHandler& handler, const std::string& sEvent, const EventArgs& args) const;
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
