//Copyright SimBlocks LLC 2016-2026
/**
 * @file Event.cpp
 * @brief Implements event handling and notification utilities.
 *
 * This file contains the implementation of the Event and EventArgs classes, as well as
 * the notifyEvent function for dispatching events to handlers. It also provides default
 * implementations for event visitation and writing.
 *
 * @see sbio::utils::Event
 * @see sbio::utils::EventArgs
 * @see sbio::utils::notifyEvent
 */
#include "Event.h"
#include "EventDispatcher.h"
#include "EventHandler.h"
#include "GlobalHeaders/Globals.h"
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

// Constructs an Event instance.
Event::Event()
{
}

namespace sbio
{
  namespace utils
  {
    // Notifies listeners of an event with the given name and arguments.
    void notifyEvent(Event* pEvent, const std::string& sEvent, const EventArgs& args)
    {
      if (g_UtilitiesGlobals.pEventDispatcher == nullptr)
      {
        return;
      }

      g_UtilitiesGlobals.pEventDispatcher->Notify(pEvent, sEvent, args);
    }
  }
}

// Visits the event handler with the given arguments (default: no-op).
void Event::Visit(const EventHandler& handler, const EventArgs& args) const
{
}

// Visits the event handler with the event name and arguments.
void Event::Visit(const EventHandler& handler, const std::string& sEvent, const EventArgs& args) const
{
  handler.Visit(*this, sEvent, args);
}

// Writes event data to the given output stream (default: no-op).
void Event::Write(std::ofstream& stream) const
{
  boost::ignore_unused(stream);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
