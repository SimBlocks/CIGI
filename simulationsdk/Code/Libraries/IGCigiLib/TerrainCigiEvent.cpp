//Copyright SimBlocks LLC 2016-2026
#include "TerrainCigiEvent.h"
#include "GlobalHeaders/Globals.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/EventHandler.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi::ig;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void TerrainCigiEvent::Visit(const EventHandler& handler, const EventArgs& args) const
{
  handler.Visit(*this, TerrainCigiEvent::GetStaticName(), args);
}

void TerrainCigiEventHandler::Visit(const Event& e, const std::string& sEvent, const EventArgs& args) const
{
  const TListeners& callbacks = g_CigiLibGlobals.pEventDispatcher->GetCallbacks(TerrainCigiEvent::GetStaticName());
  for (TListeners::const_iterator it = callbacks.begin(); it != callbacks.end(); ++it)
  {
    ITerrainCigiEventListener* pListener = dynamic_cast<ITerrainCigiEventListener*>(*it);

    const TerrainCigiEventArgs& terrainEventArgs = static_cast<const TerrainCigiEventArgs&>(args);

    switch (terrainEventArgs.eEvent)
    {
    default:
    case ETerrainCigiEvent::UNKNOWN:
      break;
    case ETerrainCigiEvent::HAT_RESPONSE:
    {
      const CigiHatResponseEventArgs& hostCigiHatHotResponseEventArgs = static_cast<const CigiHatResponseEventArgs&>(args);
      pListener->OnCigiHatResponseEvent(hostCigiHatHotResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::HOT_RESPONSE:
    {
      const CigiHotResponseEventArgs& hostCigiHotResponseEventArgs = static_cast<const CigiHotResponseEventArgs&>(args);
      pListener->OnCigiHotResponseEvent(hostCigiHotResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::HAT_HOT_EXTENDED_RESPONSE:
    {
      const CigiHatHotExtendedResponseEventArgs& terrainCigiHatHotExtendedResponseEventArgs = static_cast<const CigiHatHotExtendedResponseEventArgs&>(args);
      pListener->OnCigiHatHotExtendedResponseEvent(terrainCigiHatHotExtendedResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::LINE_OF_SIGHT_NON_ENTITY_RESPONSE:
    {
      const CigiLineOfSightResponseEventArgs& terrainCigiLineOfSightResponseEventArgs = static_cast<const CigiLineOfSightResponseEventArgs&>(args);
      pListener->OnCigiLineOfSightResponseEvent(terrainCigiLineOfSightResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::LINE_OF_SIGHT_ENTITY_RESPONSE:
    {
      const CigiLineOfSightEntityResponseEventArgs& terrainCigiLineOfSightEntityResponseEventArgs = static_cast<const CigiLineOfSightEntityResponseEventArgs&>(args);
      pListener->OnCigiLineOfSightEntityResponseEvent(terrainCigiLineOfSightEntityResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE:
    {
      const CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& terrainCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs = static_cast<const CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs&>(args);
      pListener->OnCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(terrainCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE:
    {
      const CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& terrainCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs = static_cast<const CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs&>(args);
      pListener->OnCigiLineOfSightExtendedEntityCoordinatesResponseEvent(terrainCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs);
      break;
    }
    case ETerrainCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE:
    {
      const CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& terrainCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs = static_cast<const CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs&>(args);
      pListener->OnCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(terrainCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs);
      break;
    }
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
