//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostCigiEvent.h
 * @brief Declares event types and event argument structures for CIGI Host event handling.
 *
 * Provides enums, event classes, and event argument structures for handling CIGI protocol events in the host emulator.
 * Supports event notification, error and message handling, and IG-to-Host response processing for simulation interoperability.
 * Integrates with the utilities event system for extensible event-driven architecture.
 *
 * @see sbio::cigi::host::HostCigiEvent
 * @see sbio::cigi::host::IHostCigiEventListener
 * @see sbio::cigi::host::HostCigiEventArgs
 * @see sbio::cigi::host::HostCigiEventHandler
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_EVENT_H
#define SIMBLOCKS_HOST_EMULATOR_EVENT_H

#include "CigiLib/CigiTypesIGToHost.h"
#include "GlobalHeaders/CommonTypes.h"
#include "UtilitiesLib/Event.h"
#include "UtilitiesLib/EventHandler.h"
#include "UtilitiesLib/IEventListener.h"

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Identifies the host-side events emitted while processing CIGI traffic.
       *
       * These values drive dispatch through `HostCigiEvent`, `HostCigiEventHandler`, and the
       * `IHostCigiEventListener` callback interface.
       */
      enum class EHostCigiEvent
      {
        UNKNOWN = 1,
        DATABASE_LOADED,
        ERROR_DETECTED,
        MESSAGE,
        DATA_MESSAGE,
        CLEAR_MESSAGE,
        WEATHER_CONTROL_MESSAGE,
        START_OF_FRAME,
        HAT_RESPONSE,
        HOT_RESPONSE,
        HAT_HOT_EXTENDED_RESPONSE,
        LINE_OF_SIGHT_RESPONSE,
        LINE_OF_SIGHT_NON_ENTITY_RESPONSE,
        LINE_OF_SIGHT_ENTITY_RESPONSE,
        LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE,
        LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE,
        LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE,
        SENSOR_RESPONSE,
        SENSOR_EXTENDED_RESPONSE,
        SENSOR_EXTENDED_ENTITY_RESPONSE,
        POSITION_RESPONSE,
        WEATHER_CONDITIONS_RESPONSE,
        AEROSOL_CONCENTRATION_RESPONSE,
        MARITIME_SURFACE_CONDITIONS_RESPONSE,
        TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE,
        COLLISION_DETECTION_SEGMENT_NOTIFICATION,
        COLLISION_DETECTION_SEGMENT_ENTITY_NOTIFICATION,
        COLLISION_DETECTION_VOLUME_NOTIFICATION,
        COLLISION_DETECTION_VOLUME_ENTITY_NOTIFICATION,
        ANIMATION_STOP_NOTIFICATION,
        EVENT_NOTIFICATION,
        IMAGE_GENERATOR_MESSAGE
      };

      /**
       * @brief Concrete event type used for HostCigiLib event dispatch.
       */
      class HostCigiEvent : public sbio::utils::Event
      {
      public:
        static std::string GetStaticName()
        {
          return "HostEmulatorEvent";
        }

        /**
       * @brief Dispatches the event to a host CIGI event handler.
         * @param handler Event handler that receives the dispatched event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void Visit(const sbio::utils::EventHandler& handler, const sbio::utils::EventArgs& args);
      };

      /**
       * @brief Base event-argument data for all HostCigiLib events.
       */
      struct HostCigiEventArgs : public sbio::utils::EventArgs
      {
        HostCigiEventArgs(EHostCigiEvent e) : eEvent(e) {};

        EHostCigiEvent eEvent = {EHostCigiEvent::UNKNOWN};
      };

      /**
       * @brief Carries data for the error host CIGI event.
       */
      struct HostCigiErrorEventArgs : public HostCigiEventArgs
      {
        HostCigiErrorEventArgs() : HostCigiEventArgs(EHostCigiEvent::ERROR_DETECTED) {};
        std::string sError;
      };

      /**
       * @brief Carries data for the message host CIGI event.
       */
      struct HostCigiMessageEventArgs : public HostCigiEventArgs
      {
        HostCigiMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::MESSAGE) {};
        std::string sMessage;
      };

      /**
       * @brief Carries data for the data message host CIGI event.
       */
      struct HostCigiDataMessageEventArgs : public HostCigiEventArgs
      {
        HostCigiDataMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::DATA_MESSAGE) {};
        std::stringstream sDataMessage;
      };

      /**
       * @brief Carries data for the clear message host CIGI event.
       */
      struct HostCigiClearMessageEventArgs : public HostCigiEventArgs
      {
        HostCigiClearMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::CLEAR_MESSAGE) {};
      };

      /**
       * @brief Carries data for the weather control message host CIGI event.
       */
      struct HostCigiWeatherControlMessageEventArgs : public HostCigiEventArgs
      {
        HostCigiWeatherControlMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::WEATHER_CONTROL_MESSAGE) {};
        std::stringstream sDataMessage;
      };

      /**
       * @brief Carries data for the database loaded host CIGI event.
       */
      struct HostCigiDatabaseLoadedEventArgs : public HostCigiEventArgs
      {
        HostCigiDatabaseLoadedEventArgs() : HostCigiEventArgs(EHostCigiEvent::DATABASE_LOADED) {};
        sbio::DatabaseID eDatabaseID = sbio::UnknownDatabaseID;
      };

      /**
       * @brief Carries data for the hat response host CIGI event.
       */
      struct HostCigiHatResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiHatResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::HAT_RESPONSE) {};

        sbio::cigi::SHeightAboveTerrainResponse hatResponse;
      };

      /**
       * @brief Carries data for the hot response host CIGI event.
       */
      struct HostCigiHotResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiHotResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::HOT_RESPONSE) {};

        sbio::cigi::SHeightOfTerrainResponse hotResponse;
      };

      // HAT/HOT Extended Responses
      /**
       * @brief Carries data for the hat hot extended response host CIGI event.
       */
      struct HostCigiHatHotExtendedResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiHatHotExtendedResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::HAT_HOT_EXTENDED_RESPONSE) {};

        sbio::cigi::SHATHOTExtendedResponse hatHotExtendedResponse;
      };

      // Line Of Sight Response. for non-entities
      /**
       * @brief Carries data for the line of sight response host CIGI event.
       */
      struct HostCigiLineOfSightResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiLineOfSightResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_NON_ENTITY_RESPONSE) {};

        SLineOfSightResponse lineOfSightResponse;
      };

      // Line Of Sight Entity Response
      /**
       * @brief Carries data for the line of sight entity response host CIGI event.
       */
      struct HostCigiLineOfSightEntityResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiLineOfSightEntityResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_ENTITY_RESPONSE) {};

        SLineOfSightEntityResponse lineOfSightEntityResponse;
      };

      // Line Of Sight Extended Geodetic Coordinates Response
      /**
       * @brief Carries data for the line of sight extended geodetic coordinates response host CIGI event.
       */
      struct HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE) {};

        SLineOfSightExtendedGeodeticCoordinatesResponse lineOfSightExtendedGeodeticCoordinatesResponse;
      };

      // Line Of Sight Extended Entity with Geodetic Coordinates Response
      /**
       * @brief Carries data for the line of sight extended entity geodetic coordinates response host CIGI event.
       */
      struct HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE) {};

        SLineOfSightExtendedEntityGeodeticCoordinatesResponse lineOfSightExtendedEntityGeodeticCoordinatesResponse;
      };

      // Line Of Sight Extended Entity Coordinates Response
      /**
       * @brief Carries data for the line of sight extended entity coordinates response host CIGI event.
       */
      struct HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE) {};

        SLineOfSightExtendedEntityCoordinatesResponse lineOfSightExtendedEntityCoordinatesResponse;
      };

      // Sensor Response
      /**
       * @brief Carries data for the sensor response host CIGI event.
       */
      struct HostCigiSensorResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiSensorResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::SENSOR_RESPONSE) {};

        SSensorResponse sensorResponse;
      };

      // Sensor Extended Response
      /**
       * @brief Carries data for the sensor extended response host CIGI event.
       */
      struct HostCigiSensorExtendedResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiSensorExtendedResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::SENSOR_EXTENDED_RESPONSE) {};

        SSensorExtendedResponse sensorExtendedResponse;
      };

      // Sensor Extended Entity Response
      /**
       * @brief Carries data for the sensor extended entity response host CIGI event.
       */
      struct HostCigiSensorExtendedEntityResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiSensorExtendedEntityResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::SENSOR_EXTENDED_ENTITY_RESPONSE) {};

        SSensorExtendedEntityResponse sensorExtendedEntityResponse;
      };

      /**
       * @brief Carries data for the position response host CIGI event.
       */
      struct HostCigiPositionResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiPositionResponseEventArgs(sbio::cigi::SBasePositionResponse& _positionResponse) : HostCigiEventArgs(EHostCigiEvent::POSITION_RESPONSE), positionResponse(_positionResponse) {};
        sbio::cigi::EPositionResponseType ePositionResponseType = sbio::cigi::EPositionResponseType::UNKNOWN;
        sbio::cigi::SBasePositionResponse& positionResponse;
      };

      // Weather Conditions Response
      /**
       * @brief Carries data for the weather conditions response host CIGI event.
       */
      struct HostCigiWeatherConditionsResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiWeatherConditionsResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::WEATHER_CONDITIONS_RESPONSE) {};

        SWeatherConditionsResponse weatherConditionsResponse;
      };

      // Aerosol Concentration Response
      /**
       * @brief Carries data for the aerosol concentration response host CIGI event.
       */
      struct HostCigiAerosolConcentrationResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiAerosolConcentrationResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::AEROSOL_CONCENTRATION_RESPONSE) {};

        SAerosolConcentrationResponse aerosolConcentrationResponse;
      };

      // Maritime Surface Conditions Response
      /**
       * @brief Carries data for the maritime surface conditions response host CIGI event.
       */
      struct HostCigiMaritimeSurfaceConditionsResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiMaritimeSurfaceConditionsResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::MARITIME_SURFACE_CONDITIONS_RESPONSE) {};

        SMaritimeSurfaceConditionsResponse maritimeSurfaceConditionsResponse;
      };

      // Terrestrial Surface Conditions Response
      /**
       * @brief Carries data for the terrestrial surface conditions response host CIGI event.
       */
      struct HostCigiTerrestrialSurfaceConditionsResponseEventArgs : public HostCigiEventArgs
      {
        HostCigiTerrestrialSurfaceConditionsResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE) {};

        STerrestrialSurfaceConditionsResponse terrestrialSurfaceConditionsResponse;
      };

      // Collision Detection Segment Notification
      /**
       * @brief Carries data for the collision detection segment notification host CIGI event.
       */
      struct HostCigiCollisionDetectionSegmentNotificationEventArgs : public HostCigiEventArgs
      {
        HostCigiCollisionDetectionSegmentNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_SEGMENT_NOTIFICATION) {};

        SCollisionDetectionSegmentNotification collisionDetectionSegmentNotification;
      };

      // Collision Detection Segment Entity Notification
      /**
       * @brief Carries data for the collision detection segment entity notification host CIGI event.
       */
      struct HostCigiCollisionDetectionSegmentEntityNotificationEventArgs : public HostCigiEventArgs
      {
        HostCigiCollisionDetectionSegmentEntityNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_SEGMENT_ENTITY_NOTIFICATION) {};

        SCollisionDetectionSegmentEntityNotification collisionDetectionSegmentEntityNotification;
      };

      // Collision Detection Volume Notification
      /**
       * @brief Carries data for the collision detection volume notification host CIGI event.
       */
      struct HostCigiCollisionDetectionVolumeNotificationEventArgs : public HostCigiEventArgs
      {
        HostCigiCollisionDetectionVolumeNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_VOLUME_NOTIFICATION) {};

        SCollisionDetectionVolumeNotification collisionDetectionVolumeNotification;
      };

      // Collision Detection Volume Entity Notification
      /**
       * @brief Carries data for the collision detection volume entity notification host CIGI event.
       */
      struct HostCigiCollisionDetectionVolumeEntityNotificationEventArgs : public HostCigiEventArgs
      {
        HostCigiCollisionDetectionVolumeEntityNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_VOLUME_ENTITY_NOTIFICATION) {};

        SCollisionDetectionVolumeEntityNotification collisionDetectionVolumeEntityNotification;
      };

      // Animation Stop Notification
      /**
       * @brief Carries data for the animation stop notification host CIGI event.
       */
      struct HostCigiAnimationStopNotificationEventArgs : public HostCigiEventArgs
      {
        HostCigiAnimationStopNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::ANIMATION_STOP_NOTIFICATION) {};

        SAnimationStopNotification animationStopNotification;
      };

      /**
       * @brief Carries data for the start of frame host CIGI event.
       */
      struct HostCigiStartOfFrameEventArgs : public HostCigiEventArgs
      {
        HostCigiStartOfFrameEventArgs() : HostCigiEventArgs(EHostCigiEvent::START_OF_FRAME) {};

        SCigiStartOfFrame startOfFrame;
      };

      /**
       * @brief Carries data for the event notification host CIGI event.
       */
      struct HostCigiEventNotificationEventArgs : public HostCigiEventArgs
      {
        HostCigiEventNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::EVENT_NOTIFICATION) {};

        uint16_t eventID = 0;
        uint32_t eventData[3] = {0, 0, 0};
      };

      /**
       * @brief Carries data for the image generator message host CIGI event.
       */
      struct HostCigiImageGeneratorMessageEventArgs : public HostCigiEventArgs
      {
        HostCigiImageGeneratorMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::IMAGE_GENERATOR_MESSAGE) {};

        uint16_t messageID = 0;
        std::string sMessage;
      };

      /**
       * @brief Listener interface for host-side CIGI events.
       *
       * Implementers override only the callbacks relevant to the events they consume.
       */
      struct IHostCigiEventListener : public sbio::utils::IEventListener
      {
      public:
        virtual ~IHostCigiEventListener() {};

        virtual void OnDatabaseLoaded(const HostCigiDatabaseLoadedEventArgs& args) {};
        virtual void OnHostCigiErrorEvent(const HostCigiErrorEventArgs& args) {};
        virtual void OnHostCigiMessageEvent(const HostCigiMessageEventArgs& args) {};
        virtual void OnHostCigiDataMessageEvent(const HostCigiDataMessageEventArgs& args) {};
        virtual void OnHostCigiClearMessageEvent(const HostCigiClearMessageEventArgs& args) {};
        virtual void OnHostCigiWeatherControlMessageEvent(const HostCigiWeatherControlMessageEventArgs& args) {};
        virtual void OnHostCigiHatResponseEvent(const HostCigiHatResponseEventArgs& args) {};
        virtual void OnHostCigiHotResponseEvent(const HostCigiHotResponseEventArgs& args) {};
        virtual void OnHostCigiHatHotExtendedResponseEvent(const HostCigiHatHotExtendedResponseEventArgs& args) {};
        virtual void OnHostCigiLineOfSightResponseEvent(const HostCigiLineOfSightResponseEventArgs& args) {};
        virtual void OnHostCigiLineOfSightEntityResponseEvent(const HostCigiLineOfSightEntityResponseEventArgs& args) {};
        virtual void OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args) {};
        virtual void OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args) {};
        virtual void OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args) {};
        virtual void OnHostCigiSensorResponseEvent(const HostCigiSensorResponseEventArgs& args) {};
        virtual void OnHostCigiSensorExtendedResponseEvent(const HostCigiSensorExtendedResponseEventArgs& args) {};
        virtual void OnHostCigiSensorExtendedEntityResponseEvent(const HostCigiSensorExtendedEntityResponseEventArgs& args) {};
        virtual void OnHostCigiPositionResponseEvent(const HostCigiPositionResponseEventArgs& args) {};
        virtual void OnHostCigiWeatherConditionsResponseEvent(const HostCigiWeatherConditionsResponseEventArgs& args) {};
        virtual void OnHostCigiAerosolConcentrationResponseEvent(const HostCigiAerosolConcentrationResponseEventArgs& args) {};
        virtual void OnHostCigiMaritimeSurfaceConditionsResponseEvent(const HostCigiMaritimeSurfaceConditionsResponseEventArgs& args) {};
        virtual void OnHostCigiTerrestrialSurfaceConditionsResponseEvent(const HostCigiTerrestrialSurfaceConditionsResponseEventArgs& args) {};
        virtual void OnHostCigiCollisionDetectionSegmentNotificationEvent(const HostCigiCollisionDetectionSegmentNotificationEventArgs& args) {};
        virtual void OnHostCigiCollisionDetectionSegmentEntityNotificationEvent(const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs& args) {};
        virtual void OnHostCigiCollisionDetectionVolumeNotificationEvent(const HostCigiCollisionDetectionVolumeNotificationEventArgs& args) {};
        virtual void OnHostCigiCollisionDetectionVolumeEntityNotificationEvent(const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs& args) {};
        virtual void OnHostCigiAnimationStopNotificationEvent(const HostCigiAnimationStopNotificationEventArgs& args) {};
        virtual void OnHostCigiStartOfFrameEvent(const HostCigiStartOfFrameEventArgs& args) {};
        virtual void OnHostCigiEventNotificationEvent(const HostCigiEventNotificationEventArgs& args) {};
        virtual void OnHostCigiImageGeneratorMessageEvent(const HostCigiImageGeneratorMessageEventArgs& args) {};
      };

      /**
       * @brief Dispatches `HostCigiEvent` datas to an `IHostCigiEventListener` implementation.
       */
      class HostCigiEventHandler : public sbio::utils::EventHandler
      {
      public:
        /**
         * @brief Dispatches a Host CIGI event to the registered listener.
         * @param e Event instance being dispatched.
         * @param sEvent Event name used by the dispatcher.
         * @param args Event data supplied with the dispatch.
         */
        virtual void Visit(const sbio::utils::Event& e, const std::string& sEvent, const sbio::utils::EventArgs& args) const override;

      private:
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
