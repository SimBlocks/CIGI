//Copyright SimBlocks LLC 2016-2026
/**
 * @file EventHandler.cpp
 * @brief Implements the event handler interface for event processing.
 *
 * This file contains the implementation of the EventHandler class's Visit method,
 * which dispatches events to all registered listeners for a given event name.
 *
 * @see sbio::utils::EventHandler
 */
#include "EventHandler.h"
#include "EventDispatcher.h"
#include "GlobalHeaders/Globals.h"
#include "IEventListener.h"
#include "UtilitiesLib.h"
#include <boost/core/ignore_unused.hpp>

using namespace sbio::utils;

namespace sbio
{
  namespace utils
  {
    // Global instance for utility pointers (event dispatcher, logger, etc.)
    extern SUtilitiesGlobals g_UtilitiesGlobals;
  }
}

/**
 * @brief Dispatches the event to all registered listeners for the given event name.
 *
 * Iterates through the list of listeners registered for the event and calls their OnEvent method
 * with the provided arguments.
 *
 * @param event The event instance being handled.
 * @param sEvent Name of the event.
 * @param args Arguments associated with the event.
 */
void EventHandler::Visit(const Event& event, const std::string& sEvent, const EventArgs& args) const
{
  boost::ignore_unused(event);

  if (g_UtilitiesGlobals.pEventDispatcher == nullptr)
  {
    return;
  }

  TListeners callbacks = g_UtilitiesGlobals.pEventDispatcher->GetCallbacks(sEvent);
  for (TListeners::iterator it = callbacks.begin(); it != callbacks.end(); ++it)
  {
    IEventListener* pCallback = *it;
    pCallback->OnEvent(args);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
