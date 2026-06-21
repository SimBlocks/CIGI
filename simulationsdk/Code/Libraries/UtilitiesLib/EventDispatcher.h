//Copyright SimBlocks LLC 2016-2026
/**
 * @file EventDispatcher.h
 * @brief Declares the event dispatcher and event registration/listener management.
 *
 * Defines the CEventDispatcher class for registering, unregistering, and notifying event listeners.
 * Also provides supporting types for event handling and listener management.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_EVENT_DISPATCHER_H
#define SIMBLOCKS_UTILITIES_EVENT_DISPATCHER_H

#include "Event.h"
#include "EventHandler.h"
#include "UtilitiesDeclarations.h"
#include <list>
#include <map>
#include <memory>
#include <sstream>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Ordered collection of listener pointers registered for a specific event name.
     *
     * The dispatcher does not own the listener objects stored in this list.
     */
    typedef std::list<IEventListener*> TListeners;

    /**
     * @brief Registration state for one event name.
     *
     * Stores the listener list and the event handler used to dispatch to those listeners.
     *
     * @ownership `Listeners` contains non-owning raw pointers.
     * @ownership `pHandler` owns the registered handler for the event name when non-null.
     */
    struct EventHelper
    {
      TListeners Listeners;///< Registered listeners in registration order. Duplicate entries are permitted by the dispatcher.
      std::unique_ptr<EventHandler> pHandler;///< Handler used when the event is notified.
    };

    /**
     * @brief Registry and dispatcher for named events.
     *
     * `CEventDispatcher` maps event names to an `EventHelper` containing a non-owning listener list
     * and an optional owning handler. Notifications are delivered only when a matching handler has been
     * registered for the event name.
     *
     * @invariant Listener pointers are never owned or deleted by the dispatcher.
     * @invariant At most one handler is stored per event name; registering a new handler replaces the previous handler.
     */
    class CEventDispatcher
    {
    public:
      /**
       * @brief Constructs an empty dispatcher.
       */
      CEventDispatcher();

      /**
       * @brief Destroys the dispatcher.
       *
       * Destroying the dispatcher releases all registered handlers and forgets all listener pointers.
       * Listener objects themselves are not destroyed.
       */
      ~CEventDispatcher();

      /**
       * @brief Returns the listeners currently registered for an event name.
       * @param sEvent Event registration key.
       * @return Reference to the registered listener list, or a shared empty list when the event name is unknown.
       *
       * @ownership The returned list remains owned by the dispatcher.
       * @failurecases Unknown event names return an empty list.
       */
      const TListeners& GetCallbacks(const std::string& sEvent);

      /**
       * @brief Registers a listener for an event type.
       * @tparam TEvent Event type that supplies `GetStaticName()`.
       * @param pCallback Non-owning pointer to the listener.
       *
       * The pointer is appended to the listener list for the event name.
       *
       * @ownership The caller retains ownership of `pCallback`.
       * @sideeffects Creates the event entry if it does not already exist.
       * @sideeffects Re-registering the same pointer adds another list entry and results in repeated callbacks.
       */
      template <typename TEvent>
      void RegisterListener(IEventListener* pCallback)
      {
        m_Events[TEvent::GetStaticName()].Listeners.push_back(pCallback);
      }

      /**
       * @brief Unregisters a listener for an event type.
       * @tparam TEvent Event type that supplies `GetStaticName()`.
       * @param pCallback Listener pointer to remove.
       *
       * All matching entries are removed from the listener list for the event name.
       *
       * @ownership The caller retains ownership of `pCallback`.
       * @failurecases If the event type has not been registered, the function returns without modification.
       */
      template <typename TEvent>
      void UnregisterListener(IEventListener* pCallback)
      {
        TEvents::iterator it = m_Events.find(TEvent::GetStaticName());
        if (it == m_Events.end())
        {
          return;
        }

        it->second.Listeners.remove(pCallback);
      }

      /**
       * @brief Registers or replaces the handler for an event type.
       * @tparam TEvent Event type that supplies `GetStaticName()`.
       * @param pHandler Owning pointer to the handler implementation.
       *
       * @ownership Ownership of `pHandler` is transferred to the dispatcher.
       * @sideeffects Replaces any previously registered handler for the same event name.
       */
      template <typename TEvent>
      void RegisterEvent(std::unique_ptr<EventHandler> pHandler)
      {
        m_Events[TEvent::GetStaticName()].pHandler = std::move(pHandler);
      }

      /**
       * @brief Dispatches a notified event to its registered handler.
       * @param pEvent Event descriptor to visit.
       * @param sEvent Event registration key.
       * @param args Event argument object.
       *
       * `Notify()` looks up the event name and, when a handler is present, invokes `pEvent->Visit(...)`.
       * Listener callbacks occur only if the handler implementation forwards to them.
       *
       * @ownership `pEvent` and `args` remain owned by the caller.
       * @sideeffects Executes handler code and any downstream listener callbacks.
       * @failurecases If the event name is unknown or no handler is registered, the function returns without dispatching.
       * @failurecases `pEvent` must not be `nullptr` when a handler is present.
       */
      void Notify(Event* pEvent, const std::string& sEvent, const EventArgs& args);

    private:
      /**
       * @brief Map of event names to their associated listeners and handler.
       */
      typedef std::map<std::string, EventHelper> TEvents;
      TEvents m_Events;///< Registry of event names to listeners and handler state.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
