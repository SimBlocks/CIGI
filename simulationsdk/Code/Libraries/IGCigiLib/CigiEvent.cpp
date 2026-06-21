//Copyright SimBlocks LLC 2016-2026
#include "CigiEvent.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/IGCigiLib.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/EventHandler.h"

using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void IGCIGIEvent::Visit(const EventHandler& handler, const std::string& sEvent, const EventArgs& args) const
{
  handler.Visit(*this, sEvent, args);
}

void IGCIGIEventHandler::Visit(const Event& e, const std::string& sEvent, const EventArgs& args) const
{
  const TListeners& callbacks = g_CigiLibGlobals.pEventDispatcher->GetCallbacks(IGCIGIEvent::GetStaticName());
  for (TListeners::const_iterator it = callbacks.begin(); it != callbacks.end(); ++it)
  {
    IIGCIGIEventListener* pListener = dynamic_cast<IIGCIGIEventListener*>(*it);

    const SIGCIGIEventArgs& cigiEventArgs = static_cast<const SIGCIGIEventArgs&>(args);

    switch (cigiEventArgs.eEvent)
    {
    default:
    case EIGCIGIEvent::UNKNOWN:
      break;
    case EIGCIGIEvent::DATABASE_FINISHED_LOADING:
      pListener->OnDatabaseLoadedEvent();
      break;
    case EIGCIGIEvent::DATABASE_LOADING_FAILED:
      pListener->OnDatabaseLoadingFailedEvent();
      break;
    case EIGCIGIEvent::ENTITY_REMOVED:
    {
      const SEntityRemovedEventArgs& entityRemovedEventArgs = static_cast<const SEntityRemovedEventArgs&>(args);

      pListener->OnEntityRemoved(entityRemovedEventArgs.entityID);
      break;
    }
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
