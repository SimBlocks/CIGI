//Copyright SimBlocks LLC 2016-2026
/**
 * @file EventDispatcher.cpp
 * @brief Implements event dispatching and listener management.
 *
 * This file contains the implementation of the CEventDispatcher class, which manages
 * event registration, listener management, and event notification. It provides methods
 * for retrieving registered listeners and notifying them when events occur.
 *
 * @see sbio::utils::CEventDispatcher
 */
#include "EventDispatcher.h"
#include "Event.h"

using namespace sbio::utils;

// Constructs an event dispatcher.
CEventDispatcher::CEventDispatcher()
{
}

// Destructor. Cleans up event dispatcher resources.
CEventDispatcher::~CEventDispatcher()
{
}

namespace
{
  const TListeners emptyListeners;///< Empty list of listeners for cases where no listeners are registered.
}

// Gets the list of listeners registered for a specific event name.
const TListeners& CEventDispatcher::GetCallbacks(const std::string& sEvent)
{
  TEvents::iterator it = m_Events.find(sEvent);
  if (it == m_Events.end())
  {
    return emptyListeners;
  }

  return it->second.Listeners;
}

// Notifies all listeners of an event occurrence.
void CEventDispatcher::Notify(Event* pEvent, const std::string& sEvent, const EventArgs& args)
{
  TEvents::iterator it = m_Events.find(sEvent);
  if (it == m_Events.end())
  {
    return;
  }

  EventHandler* pHandler = it->second.pHandler.get();
  if (pHandler == nullptr)
  {
    return;
  }

  pEvent->Visit(*pHandler, sEvent, args);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
