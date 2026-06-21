//Copyright SimBlocks LLC 2016-2026
#include "HostCigiEvent.h"
#include "GlobalHeaders/Globals.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/EventHandler.h"
#include "HostCigiLibTypes.h"

using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi::host;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

void HostCigiEvent::Visit(const EventHandler& handler, const EventArgs& args)
{
  handler.Visit(*this, HostCigiEvent::GetStaticName(), args);
}

void HostCigiEventHandler::Visit(const Event& e, const std::string& sEvent, const EventArgs& args) const
{
  // Get the list of callbacks registered for this event type from the global event dispatcher.
  const TListeners& callbacks = g_HostCigiLibGlobals.pEventDispatcher->GetCallbacks(HostCigiEvent::GetStaticName());
  for (TListeners::const_iterator it = callbacks.begin(); it != callbacks.end(); ++it)
  {
    IHostCigiEventListener* pListener = dynamic_cast<IHostCigiEventListener*>(*it);
    if (pListener == nullptr)
    {
      continue;
    }

    const HostCigiEventArgs& hostEmulatorEventArgs = static_cast<const HostCigiEventArgs&>(args);

    // Dispatch the event to the appropriate handler method on the listener based on the event type.
    switch (hostEmulatorEventArgs.eEvent)
    {
    default:
    case EHostCigiEvent::UNKNOWN:
      break;
    case EHostCigiEvent::DATABASE_LOADED:
    {
      const HostCigiDatabaseLoadedEventArgs& hostCigiDatabaseEventArgs = static_cast<const HostCigiDatabaseLoadedEventArgs&>(args);
      pListener->OnDatabaseLoaded(hostCigiDatabaseEventArgs);
      break;
    }
    case EHostCigiEvent::ERROR_DETECTED:
    {
      const HostCigiErrorEventArgs& hostCigiErrorEventArgs = static_cast<const HostCigiErrorEventArgs&>(args);
      pListener->OnHostCigiErrorEvent(hostCigiErrorEventArgs);
      break;
    }
    case EHostCigiEvent::MESSAGE:
    {
      const HostCigiMessageEventArgs& hostCigiMessageEventArgs = static_cast<const HostCigiMessageEventArgs&>(args);
      pListener->OnHostCigiMessageEvent(hostCigiMessageEventArgs);
      break;
    }
    case EHostCigiEvent::DATA_MESSAGE:
    {
      const HostCigiDataMessageEventArgs& hostCigiDataMessageEventArgs = static_cast<const HostCigiDataMessageEventArgs&>(args);
      pListener->OnHostCigiDataMessageEvent(hostCigiDataMessageEventArgs);
      break;
    }
    case EHostCigiEvent::CLEAR_MESSAGE:
    {
      const HostCigiClearMessageEventArgs& hostCigiClearMessageEventArgs = static_cast<const HostCigiClearMessageEventArgs&>(args);
      pListener->OnHostCigiClearMessageEvent(hostCigiClearMessageEventArgs);
      break;
    }
    case EHostCigiEvent::WEATHER_CONTROL_MESSAGE:
    {
      const HostCigiWeatherControlMessageEventArgs& hostCigiWeatherControlMessageEventArgs = static_cast<const HostCigiWeatherControlMessageEventArgs&>(args);
      pListener->OnHostCigiWeatherControlMessageEvent(hostCigiWeatherControlMessageEventArgs);
      break;
    }
    case EHostCigiEvent::START_OF_FRAME:
    {
      const HostCigiStartOfFrameEventArgs& hostCigiStartOfFrameEventArgs = static_cast<const HostCigiStartOfFrameEventArgs&>(args);
      pListener->OnHostCigiStartOfFrameEvent(hostCigiStartOfFrameEventArgs);
      break;
    }
    case EHostCigiEvent::HAT_RESPONSE:
    {
      const HostCigiHatResponseEventArgs& hostCigiHatResponseEventArgs = static_cast<const HostCigiHatResponseEventArgs&>(args);
      pListener->OnHostCigiHatResponseEvent(hostCigiHatResponseEventArgs);
      break;
    }
    case EHostCigiEvent::HOT_RESPONSE:
    {
      const HostCigiHotResponseEventArgs& hostCigiHotResponseEventArgs = static_cast<const HostCigiHotResponseEventArgs&>(args);
      pListener->OnHostCigiHotResponseEvent(hostCigiHotResponseEventArgs);
      break;
    }
    case EHostCigiEvent::HAT_HOT_EXTENDED_RESPONSE:
    {
      const HostCigiHatHotExtendedResponseEventArgs& hostCigiHatHotExtendedResponseEventArgs = static_cast<const HostCigiHatHotExtendedResponseEventArgs&>(args);
      pListener->OnHostCigiHatHotExtendedResponseEvent(hostCigiHatHotExtendedResponseEventArgs);
      break;
    }
    case EHostCigiEvent::LINE_OF_SIGHT_NON_ENTITY_RESPONSE:
    {
      const HostCigiLineOfSightResponseEventArgs& hostCigiLineOfSightResponseEventArgs = static_cast<const HostCigiLineOfSightResponseEventArgs&>(args);
      pListener->OnHostCigiLineOfSightResponseEvent(hostCigiLineOfSightResponseEventArgs);
      break;
    }
    case EHostCigiEvent::LINE_OF_SIGHT_ENTITY_RESPONSE:
    {
      const HostCigiLineOfSightEntityResponseEventArgs& hostCigiLineOfSightEntityResponseEventArgs = static_cast<const HostCigiLineOfSightEntityResponseEventArgs&>(args);
      pListener->OnHostCigiLineOfSightEntityResponseEvent(hostCigiLineOfSightEntityResponseEventArgs);
      break;
    }
    case EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE:
    {
      const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& hostCigiLineOfSightExtendedGeodeticResponseEventArgs = static_cast<const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs&>(args);
      pListener->OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(hostCigiLineOfSightExtendedGeodeticResponseEventArgs);
      break;
    }
    case EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE:
    {
      const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& hostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs = static_cast<const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs&>(args);
      pListener->OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(hostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs);
      break;
    }
    case EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE:
    {
      const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& hostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs = static_cast<const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs&>(args);
      pListener->OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent(hostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs);
      break;
    }
    case EHostCigiEvent::SENSOR_RESPONSE:
    {
      const HostCigiSensorResponseEventArgs& hostCigiSensorEventArgs = static_cast<const HostCigiSensorResponseEventArgs&>(args);
      pListener->OnHostCigiSensorResponseEvent(hostCigiSensorEventArgs);
      break;
    }
    case EHostCigiEvent::SENSOR_EXTENDED_RESPONSE:
    {
      const HostCigiSensorExtendedResponseEventArgs& hostCigiSensorExtendedResponseEventArgs = static_cast<const HostCigiSensorExtendedResponseEventArgs&>(args);
      pListener->OnHostCigiSensorExtendedResponseEvent(hostCigiSensorExtendedResponseEventArgs);
      break;
    }
    case EHostCigiEvent::SENSOR_EXTENDED_ENTITY_RESPONSE:
    {
      const HostCigiSensorExtendedEntityResponseEventArgs& hostCigiSensorExtendedEntityResponseEventArgs = static_cast<const HostCigiSensorExtendedEntityResponseEventArgs&>(args);
      pListener->OnHostCigiSensorExtendedEntityResponseEvent(hostCigiSensorExtendedEntityResponseEventArgs);
      break;
    }
    case EHostCigiEvent::POSITION_RESPONSE:
    {
      const HostCigiPositionResponseEventArgs& hostCigiPositionResponseEventArgs = static_cast<const HostCigiPositionResponseEventArgs&>(args);
      pListener->OnHostCigiPositionResponseEvent(hostCigiPositionResponseEventArgs);
      break;
    }
    case EHostCigiEvent::WEATHER_CONDITIONS_RESPONSE:
    {
      const HostCigiWeatherConditionsResponseEventArgs& hostCigiWeatherConditionsResponseEventArgs = static_cast<const HostCigiWeatherConditionsResponseEventArgs&>(args);
      pListener->OnHostCigiWeatherConditionsResponseEvent(hostCigiWeatherConditionsResponseEventArgs);
      break;
    }
    case EHostCigiEvent::AEROSOL_CONCENTRATION_RESPONSE:
    {
      const HostCigiAerosolConcentrationResponseEventArgs& hostCigiAerosolConcentrationResponseEventArgs = static_cast<const HostCigiAerosolConcentrationResponseEventArgs&>(args);
      pListener->OnHostCigiAerosolConcentrationResponseEvent(hostCigiAerosolConcentrationResponseEventArgs);
      break;
    }
    case EHostCigiEvent::MARITIME_SURFACE_CONDITIONS_RESPONSE:
    {
      const HostCigiMaritimeSurfaceConditionsResponseEventArgs& hostCigiMaritimeSurfaceConditionsResponseEventArgs = static_cast<const HostCigiMaritimeSurfaceConditionsResponseEventArgs&>(args);
      pListener->OnHostCigiMaritimeSurfaceConditionsResponseEvent(hostCigiMaritimeSurfaceConditionsResponseEventArgs);
      break;
    }
    case EHostCigiEvent::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE:
    {
      const HostCigiTerrestrialSurfaceConditionsResponseEventArgs& hostCigiTerrestrialSurfaceConditionsResponseEventArgs = static_cast<const HostCigiTerrestrialSurfaceConditionsResponseEventArgs&>(args);
      pListener->OnHostCigiTerrestrialSurfaceConditionsResponseEvent(hostCigiTerrestrialSurfaceConditionsResponseEventArgs);
      break;
    }
    case EHostCigiEvent::COLLISION_DETECTION_SEGMENT_NOTIFICATION:
    {
      const HostCigiCollisionDetectionSegmentNotificationEventArgs& hostCigiCollisionDetectionSegmentNotificationEventArgs = static_cast<const HostCigiCollisionDetectionSegmentNotificationEventArgs&>(args);
      pListener->OnHostCigiCollisionDetectionSegmentNotificationEvent(hostCigiCollisionDetectionSegmentNotificationEventArgs);
      break;
    }
    case EHostCigiEvent::COLLISION_DETECTION_SEGMENT_ENTITY_NOTIFICATION:
    {
      const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs& hostCigiCollisionDetectionSegmentEntityNotificationEventArgs = static_cast<const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs&>(args);
      pListener->OnHostCigiCollisionDetectionSegmentEntityNotificationEvent(hostCigiCollisionDetectionSegmentEntityNotificationEventArgs);
      break;
    }
    case EHostCigiEvent::COLLISION_DETECTION_VOLUME_NOTIFICATION:
    {
      const HostCigiCollisionDetectionVolumeNotificationEventArgs& hostCigiCollisionDetectionVolumeNotificationEventArgs = static_cast<const HostCigiCollisionDetectionVolumeNotificationEventArgs&>(args);
      pListener->OnHostCigiCollisionDetectionVolumeNotificationEvent(hostCigiCollisionDetectionVolumeNotificationEventArgs);
      break;
    }
    case EHostCigiEvent::COLLISION_DETECTION_VOLUME_ENTITY_NOTIFICATION:
    {
      const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs& hostCigiCollisionDetectionVolumeEntityNotificationEventArgs = static_cast<const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs&>(args);
      pListener->OnHostCigiCollisionDetectionVolumeEntityNotificationEvent(hostCigiCollisionDetectionVolumeEntityNotificationEventArgs);
      break;
    }
    case EHostCigiEvent::ANIMATION_STOP_NOTIFICATION:
    {
      const HostCigiAnimationStopNotificationEventArgs& hostCigiAnimationStopNotificationEventArgs = static_cast<const HostCigiAnimationStopNotificationEventArgs&>(args);
      pListener->OnHostCigiAnimationStopNotificationEvent(hostCigiAnimationStopNotificationEventArgs);
      break;
    }
    case EHostCigiEvent::EVENT_NOTIFICATION:
    {
      const HostCigiEventNotificationEventArgs& hostCigiEventNotificationEventArgs = static_cast<const HostCigiEventNotificationEventArgs&>(args);
      pListener->OnHostCigiEventNotificationEvent(hostCigiEventNotificationEventArgs);
      break;
    }
    case EHostCigiEvent::IMAGE_GENERATOR_MESSAGE:
    {
      const HostCigiImageGeneratorMessageEventArgs& hostCigiImageGeneratorMessageEventArgs = static_cast<const HostCigiImageGeneratorMessageEventArgs&>(args);
      pListener->OnHostCigiImageGeneratorMessageEvent(hostCigiImageGeneratorMessageEventArgs);
      break;
    }
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
