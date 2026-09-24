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
        /**
         * @brief Gets the event-dispatch registration key.
         * @return `"HostEmulatorEvent"`, shared by all host CIGI event variants.
         */
        static std::string GetStaticName()
        {
          return "HostEmulatorEvent";
        }

        /**
         * @brief Dispatches the event to a host CIGI event handler.
         * @param handler Handler invoked with this event and `GetStaticName()`.
         * @param args Borrowed event data forwarded unchanged to the handler.
         */
        virtual void Visit(const sbio::utils::EventHandler& handler, const sbio::utils::EventArgs& args);
      };

      /**
       * @brief Base event-argument data for all HostCigiLib events.
       *
       * `eEvent` must agree with the concrete payload type: dispatch uses unchecked downcasts.
       * Session events are tagged by `CHostSession::RaiseSessionEvent()`; host-wide events may retain
       * the unknown session identifier.
       */
      struct HostCigiEventArgs : public sbio::utils::EventArgs
      {
        /**
         * @brief Constructs the common event discriminator with an unknown session identifier.
         * @param e Event kind corresponding to the concrete payload type.
         */
        HostCigiEventArgs(EHostCigiEvent e) : eEvent(e) {};

        EHostCigiEvent eEvent = {EHostCigiEvent::UNKNOWN};///< Payload discriminator used by the event handler.
        sbio::SessionID sessionID = sbio::UnknownSessionID;///< Originating session, or unknown for untagged events.
      };

      /**
       * @brief Carries data for the error host CIGI event.
       */
      struct HostCigiErrorEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an `ERROR_DETECTED` event with empty error text. */
        HostCigiErrorEventArgs() : HostCigiEventArgs(EHostCigiEvent::ERROR_DETECTED) {};
        std::string sError;///< Diagnostic error text.
      };

      /**
       * @brief Carries data for the message host CIGI event.
       */
      struct HostCigiMessageEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `MESSAGE` event with empty message text. */
        HostCigiMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::MESSAGE) {};
        std::string sMessage;///< Informational message text.
      };

      /**
       * @brief Carries data for the data message host CIGI event.
       */
      struct HostCigiDataMessageEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `DATA_MESSAGE` event with an empty diagnostic stream. */
        HostCigiDataMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::DATA_MESSAGE) {};
        std::stringstream sDataMessage;///< Formatted packet diagnostics.
      };

      /**
       * @brief Carries data for the clear message host CIGI event.
       */
      struct HostCigiClearMessageEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `CLEAR_MESSAGE` notification with no additional payload. */
        HostCigiClearMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::CLEAR_MESSAGE) {};
      };

      /**
       * @brief Carries data for the weather control message host CIGI event.
       */
      struct HostCigiWeatherControlMessageEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `WEATHER_CONTROL_MESSAGE` event with an empty diagnostic stream. */
        HostCigiWeatherControlMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::WEATHER_CONTROL_MESSAGE) {};
        std::stringstream sDataMessage;///< Formatted weather-control diagnostics.
      };

      /**
       * @brief Carries data for the database loaded host CIGI event.
       */
      struct HostCigiDatabaseLoadedEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `DATABASE_LOADED` event with an unknown database identifier. */
        HostCigiDatabaseLoadedEventArgs() : HostCigiEventArgs(EHostCigiEvent::DATABASE_LOADED) {};
        sbio::DatabaseID eDatabaseID = sbio::UnknownDatabaseID;///< Database associated with the notification.
      };

      /**
       * @brief Carries data for the hat response host CIGI event.
       */
      struct HostCigiHatResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `HAT_RESPONSE` event; the producer fills the response payload. */
        HostCigiHatResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::HAT_RESPONSE) {};

        sbio::cigi::SHeightAboveTerrainResponse hatResponse;
      };

      /**
       * @brief Carries data for the hot response host CIGI event.
       */
      struct HostCigiHotResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `HOT_RESPONSE` event; the producer fills the response payload. */
        HostCigiHotResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::HOT_RESPONSE) {};

        sbio::cigi::SHeightOfTerrainResponse hotResponse;
      };

      // HAT/HOT Extended Responses
      /**
       * @brief Carries data for the hat hot extended response host CIGI event.
       */
      struct HostCigiHatHotExtendedResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `HAT_HOT_EXTENDED_RESPONSE` event for terrain heights and surface data. */
        HostCigiHatHotExtendedResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::HAT_HOT_EXTENDED_RESPONSE) {};

        sbio::cigi::SHATHOTExtendedResponse hatHotExtendedResponse;
      };

      // Line Of Sight Response. for non-entities
      /**
       * @brief Carries data for the line of sight response host CIGI event.
       */
      struct HostCigiLineOfSightResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `LINE_OF_SIGHT_NON_ENTITY_RESPONSE` event. */
        HostCigiLineOfSightResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_NON_ENTITY_RESPONSE) {};

        SLineOfSightResponse lineOfSightResponse;
      };

      // Line Of Sight Entity Response
      /**
       * @brief Carries data for the line of sight entity response host CIGI event.
       */
      struct HostCigiLineOfSightEntityResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `LINE_OF_SIGHT_ENTITY_RESPONSE` event. */
        HostCigiLineOfSightEntityResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_ENTITY_RESPONSE) {};

        SLineOfSightEntityResponse lineOfSightEntityResponse;
      };

      // Line Of Sight Extended Geodetic Coordinates Response
      /**
       * @brief Carries data for the line of sight extended geodetic coordinates response host CIGI event.
       */
      struct HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an extended non-entity line-of-sight event with geodetic coordinates. */
        HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE) {};

        SLineOfSightExtendedGeodeticCoordinatesResponse lineOfSightExtendedGeodeticCoordinatesResponse;
      };

      // Line Of Sight Extended Entity with Geodetic Coordinates Response
      /**
       * @brief Carries data for the line of sight extended entity geodetic coordinates response host CIGI event.
       */
      struct HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an extended entity line-of-sight event with geodetic coordinates. */
        HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE) {};

        SLineOfSightExtendedEntityGeodeticCoordinatesResponse lineOfSightExtendedEntityGeodeticCoordinatesResponse;
      };

      // Line Of Sight Extended Entity Coordinates Response
      /**
       * @brief Carries data for the line of sight extended entity coordinates response host CIGI event.
       */
      struct HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an extended entity line-of-sight event with entity-relative coordinates. */
        HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE) {};

        SLineOfSightExtendedEntityCoordinatesResponse lineOfSightExtendedEntityCoordinatesResponse;
      };

      // Sensor Response
      /**
       * @brief Carries data for the sensor response host CIGI event.
       */
      struct HostCigiSensorResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `SENSOR_RESPONSE` event. */
        HostCigiSensorResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::SENSOR_RESPONSE) {};

        SSensorResponse sensorResponse;
      };

      // Sensor Extended Response
      /**
       * @brief Carries data for the sensor extended response host CIGI event.
       */
      struct HostCigiSensorExtendedResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `SENSOR_EXTENDED_RESPONSE` event without an identified entity. */
        HostCigiSensorExtendedResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::SENSOR_EXTENDED_RESPONSE) {};

        SSensorExtendedResponse sensorExtendedResponse;
      };

      // Sensor Extended Entity Response
      /**
       * @brief Carries data for the sensor extended entity response host CIGI event.
       */
      struct HostCigiSensorExtendedEntityResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `SENSOR_EXTENDED_ENTITY_RESPONSE` event. */
        HostCigiSensorExtendedEntityResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::SENSOR_EXTENDED_ENTITY_RESPONSE) {};

        SSensorExtendedEntityResponse sensorExtendedEntityResponse;
      };

      /**
       * @brief Borrows a position response for synchronous event dispatch.
       *
       * Unlike the value-based response payloads, `positionResponse` is not owned. The producer must
       * keep it alive throughout dispatch and set `ePositionResponseType` to match its concrete type.
       */
      struct HostCigiPositionResponseEventArgs : public HostCigiEventArgs
      {
        /**
         * @brief Constructs a `POSITION_RESPONSE` event borrowing the supplied response.
         * @param _positionResponse Response that must outlive this event and any access by its listeners.
         */
        HostCigiPositionResponseEventArgs(sbio::cigi::SBasePositionResponse& _positionResponse) :
          HostCigiEventArgs(EHostCigiEvent::POSITION_RESPONSE), positionResponse(_positionResponse) {};
        sbio::cigi::EPositionResponseType ePositionResponseType = sbio::cigi::EPositionResponseType::UNKNOWN;///< Concrete response discriminator; set by the producer.
        sbio::cigi::SBasePositionResponse& positionResponse;///< Borrowed response; not copied or deleted by the event.
      };

      // Weather Conditions Response
      /**
       * @brief Carries data for the weather conditions response host CIGI event.
       */
      struct HostCigiWeatherConditionsResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `WEATHER_CONDITIONS_RESPONSE` event. */
        HostCigiWeatherConditionsResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::WEATHER_CONDITIONS_RESPONSE) {};

        SWeatherConditionsResponse weatherConditionsResponse;
      };

      // Aerosol Concentration Response
      /**
       * @brief Carries data for the aerosol concentration response host CIGI event.
       */
      struct HostCigiAerosolConcentrationResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an `AEROSOL_CONCENTRATION_RESPONSE` event. */
        HostCigiAerosolConcentrationResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::AEROSOL_CONCENTRATION_RESPONSE) {};

        SAerosolConcentrationResponse aerosolConcentrationResponse;
      };

      // Maritime Surface Conditions Response
      /**
       * @brief Carries data for the maritime surface conditions response host CIGI event.
       */
      struct HostCigiMaritimeSurfaceConditionsResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `MARITIME_SURFACE_CONDITIONS_RESPONSE` event. */
        HostCigiMaritimeSurfaceConditionsResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::MARITIME_SURFACE_CONDITIONS_RESPONSE) {};

        SMaritimeSurfaceConditionsResponse maritimeSurfaceConditionsResponse;
      };

      // Terrestrial Surface Conditions Response
      /**
       * @brief Carries data for the terrestrial surface conditions response host CIGI event.
       */
      struct HostCigiTerrestrialSurfaceConditionsResponseEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE` event. */
        HostCigiTerrestrialSurfaceConditionsResponseEventArgs() : HostCigiEventArgs(EHostCigiEvent::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE) {};

        STerrestrialSurfaceConditionsResponse terrestrialSurfaceConditionsResponse;
      };

      // Collision Detection Segment Notification
      /**
       * @brief Carries data for the collision detection segment notification host CIGI event.
       */
      struct HostCigiCollisionDetectionSegmentNotificationEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a collision-segment notification without an identified contacted entity. */
        HostCigiCollisionDetectionSegmentNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_SEGMENT_NOTIFICATION) {};

        SCollisionDetectionSegmentNotification collisionDetectionSegmentNotification;
      };

      // Collision Detection Segment Entity Notification
      /**
       * @brief Carries data for the collision detection segment entity notification host CIGI event.
       */
      struct HostCigiCollisionDetectionSegmentEntityNotificationEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a collision-segment notification identifying the contacted entity. */
        HostCigiCollisionDetectionSegmentEntityNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_SEGMENT_ENTITY_NOTIFICATION) {};

        SCollisionDetectionSegmentEntityNotification collisionDetectionSegmentEntityNotification;
      };

      // Collision Detection Volume Notification
      /**
       * @brief Carries data for the collision detection volume notification host CIGI event.
       */
      struct HostCigiCollisionDetectionVolumeNotificationEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a collision-volume notification without an identified contacted entity. */
        HostCigiCollisionDetectionVolumeNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_VOLUME_NOTIFICATION) {};

        SCollisionDetectionVolumeNotification collisionDetectionVolumeNotification;
      };

      // Collision Detection Volume Entity Notification
      /**
       * @brief Carries data for the collision detection volume entity notification host CIGI event.
       */
      struct HostCigiCollisionDetectionVolumeEntityNotificationEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a collision-volume notification identifying the contacted entity. */
        HostCigiCollisionDetectionVolumeEntityNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::COLLISION_DETECTION_VOLUME_ENTITY_NOTIFICATION) {};

        SCollisionDetectionVolumeEntityNotification collisionDetectionVolumeEntityNotification;
      };

      // Animation Stop Notification
      /**
       * @brief Carries data for the animation stop notification host CIGI event.
       */
      struct HostCigiAnimationStopNotificationEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an `ANIMATION_STOP_NOTIFICATION` event. */
        HostCigiAnimationStopNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::ANIMATION_STOP_NOTIFICATION) {};

        SAnimationStopNotification animationStopNotification;
      };

      /**
       * @brief Carries data for the start of frame host CIGI event.
       */
      struct HostCigiStartOfFrameEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs a `START_OF_FRAME` event. */
        HostCigiStartOfFrameEventArgs() : HostCigiEventArgs(EHostCigiEvent::START_OF_FRAME) {};

        SCigiStartOfFrame startOfFrame;
      };

      /**
       * @brief Carries data for the event notification host CIGI event.
       */
      struct HostCigiEventNotificationEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an `EVENT_NOTIFICATION` event with zero identifier and data words. */
        HostCigiEventNotificationEventArgs() : HostCigiEventArgs(EHostCigiEvent::EVENT_NOTIFICATION) {};

        uint16_t eventID = 0;///< Identifier from the IG event notification.
        uint32_t eventData[3] = {0, 0, 0};///< Three event-specific data words in packet order.
      };

      /**
       * @brief Carries data for the image generator message host CIGI event.
       */
      struct HostCigiImageGeneratorMessageEventArgs : public HostCigiEventArgs
      {
        /** @brief Constructs an `IMAGE_GENERATOR_MESSAGE` event with zero identifier and empty text. */
        HostCigiImageGeneratorMessageEventArgs() : HostCigiEventArgs(EHostCigiEvent::IMAGE_GENERATOR_MESSAGE) {};

        uint16_t messageID = 0;///< Identifier from the IG message packet.
        std::string sMessage;///< Message payload copied from the packet; may include padding or null bytes.
      };

      /**
       * @brief Listener interface for host-side CIGI events.
       *
       * Implementers override only the callbacks relevant to the events they consume; every default
       * callback does nothing. Dispatch is synchronous. Arguments and referenced response objects are
       * borrowed for the call and must not be retained without copying the required data.
       */
      struct IHostCigiEventListener : public sbio::utils::IEventListener
      {
      public:
        /** @brief Destroys a listener; does not unregister it from an event dispatcher. */
        virtual ~IHostCigiEventListener() {};

        /**
         * @brief Receives a database-loaded notification.
         * @param args Database identifier and originating session.
         */
        virtual void OnDatabaseLoaded(const HostCigiDatabaseLoadedEventArgs& args) {};
        /**
         * @brief Receives a host CIGI error diagnostic.
         * @param args Error text and session tag.
         */
        virtual void OnHostCigiErrorEvent(const HostCigiErrorEventArgs& args) {};
        /**
         * @brief Receives an informational host CIGI message.
         * @param args Message text and session tag.
         */
        virtual void OnHostCigiMessageEvent(const HostCigiMessageEventArgs& args) {};
        /**
         * @brief Receives formatted packet diagnostics.
         * @param args Diagnostic stream and session tag.
         */
        virtual void OnHostCigiDataMessageEvent(const HostCigiDataMessageEventArgs& args) {};
        /**
         * @brief Receives a request to clear displayed messages.
         * @param args Clear-message event and session tag.
         */
        virtual void OnHostCigiClearMessageEvent(const HostCigiClearMessageEventArgs& args) {};
        /**
         * @brief Receives formatted weather-control diagnostics.
         * @param args Diagnostic stream and session tag.
         */
        virtual void OnHostCigiWeatherControlMessageEvent(const HostCigiWeatherControlMessageEventArgs& args) {};
        /**
         * @brief Receives a height-above-terrain response.
         * @param args Decoded HAT response and originating session.
         */
        virtual void OnHostCigiHatResponseEvent(const HostCigiHatResponseEventArgs& args) {};
        /**
         * @brief Receives a height-of-terrain response.
         * @param args Decoded HOT response and originating session.
         */
        virtual void OnHostCigiHotResponseEvent(const HostCigiHotResponseEventArgs& args) {};
        /**
         * @brief Receives an extended terrain-height response.
         * @param args Decoded HAT/HOT response with surface data and session tag.
         */
        virtual void OnHostCigiHatHotExtendedResponseEvent(const HostCigiHatHotExtendedResponseEventArgs& args) {};
        /**
         * @brief Receives a line-of-sight response without an identified entity.
         * @param args Decoded intersection response and session tag.
         */
        virtual void OnHostCigiLineOfSightResponseEvent(const HostCigiLineOfSightResponseEventArgs& args) {};
        /**
         * @brief Receives a line-of-sight response identifying an entity.
         * @param args Decoded entity intersection response and session tag.
         */
        virtual void OnHostCigiLineOfSightEntityResponseEvent(const HostCigiLineOfSightEntityResponseEventArgs& args) {};
        /**
         * @brief Receives an extended non-entity line-of-sight response in geodetic coordinates.
         * @param args Decoded geodetic intersection and surface data.
         */
        virtual void OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args) {};
        /**
         * @brief Receives an extended entity line-of-sight response in geodetic coordinates.
         * @param args Decoded entity intersection and surface data.
         */
        virtual void OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args) {};
        /**
         * @brief Receives an extended entity-relative line-of-sight response.
         * @param args Decoded entity-relative intersection and surface data.
         */
        virtual void OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args) {};
        /**
         * @brief Receives a sensor tracking response.
         * @param args Decoded sensor response and session tag.
         */
        virtual void OnHostCigiSensorResponseEvent(const HostCigiSensorResponseEventArgs& args) {};
        /**
         * @brief Receives an extended sensor response without an identified entity.
         * @param args Decoded sensor and track-point data.
         */
        virtual void OnHostCigiSensorExtendedResponseEvent(const HostCigiSensorExtendedResponseEventArgs& args) {};
        /**
         * @brief Receives an extended sensor response identifying an entity.
         * @param args Decoded sensor, entity, and track-point data.
         */
        virtual void OnHostCigiSensorExtendedEntityResponseEvent(const HostCigiSensorExtendedEntityResponseEventArgs& args) {};
        /**
         * @brief Receives an object position response.
         * @param args Borrowed response and concrete coordinate-type discriminator.
         */
        virtual void OnHostCigiPositionResponseEvent(const HostCigiPositionResponseEventArgs& args) {};
        /**
         * @brief Receives queried weather conditions.
         * @param args Decoded weather response and session tag.
         */
        virtual void OnHostCigiWeatherConditionsResponseEvent(const HostCigiWeatherConditionsResponseEventArgs& args) {};
        /**
         * @brief Receives queried aerosol concentration.
         * @param args Decoded aerosol response and session tag.
         */
        virtual void OnHostCigiAerosolConcentrationResponseEvent(const HostCigiAerosolConcentrationResponseEventArgs& args) {};
        /**
         * @brief Receives queried maritime surface conditions.
         * @param args Decoded maritime response and session tag.
         */
        virtual void OnHostCigiMaritimeSurfaceConditionsResponseEvent(const HostCigiMaritimeSurfaceConditionsResponseEventArgs& args) {};
        /**
         * @brief Receives queried terrestrial surface conditions.
         * @param args Decoded terrestrial response and session tag.
         */
        virtual void OnHostCigiTerrestrialSurfaceConditionsResponseEvent(const HostCigiTerrestrialSurfaceConditionsResponseEventArgs& args) {};
        /**
         * @brief Receives a collision-segment notification without a contacted entity identifier.
         * @param args Segment contact data and session tag.
         */
        virtual void OnHostCigiCollisionDetectionSegmentNotificationEvent(const HostCigiCollisionDetectionSegmentNotificationEventArgs& args) {};
        /**
         * @brief Receives a collision-segment notification identifying the contacted entity.
         * @param args Segment contact and entity data.
         */
        virtual void OnHostCigiCollisionDetectionSegmentEntityNotificationEvent(const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs& args) {};
        /**
         * @brief Receives a collision-volume notification without a contacted entity identifier.
         * @param args Volume contact data and session tag.
         */
        virtual void OnHostCigiCollisionDetectionVolumeNotificationEvent(const HostCigiCollisionDetectionVolumeNotificationEventArgs& args) {};
        /**
         * @brief Receives a collision-volume notification identifying the contacted entity.
         * @param args Volume contact and entity data.
         */
        virtual void OnHostCigiCollisionDetectionVolumeEntityNotificationEvent(const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs& args) {};
        /**
         * @brief Receives an entity animation-stop notification.
         * @param args Decoded notification and session tag.
         */
        virtual void OnHostCigiAnimationStopNotificationEvent(const HostCigiAnimationStopNotificationEventArgs& args) {};
        /**
         * @brief Receives IG start-of-frame state.
         * @param args Decoded frame counters, mode, timing, and database report.
         */
        virtual void OnHostCigiStartOfFrameEvent(const HostCigiStartOfFrameEventArgs& args) {};
        /**
         * @brief Receives an IG event notification.
         * @param args Event identifier, three data words, and session tag.
         */
        virtual void OnHostCigiEventNotificationEvent(const HostCigiEventNotificationEventArgs& args) {};
        /**
         * @brief Receives an image-generator message.
         * @param args Message identifier, payload text, and session tag.
         */
        virtual void OnHostCigiImageGeneratorMessageEvent(const HostCigiImageGeneratorMessageEventArgs& args) {};
      };

      /**
       * @brief Dispatches host CIGI payloads to registered `IHostCigiEventListener` objects.
       */
      class HostCigiEventHandler : public sbio::utils::EventHandler
      {
      public:
        /**
         * @brief Synchronously invokes the callback selected by the event discriminator on each listener.
         * @param e Event instance; unused by this implementation.
         * @param sEvent Supplied event name; ignored in favor of `HostCigiEvent::GetStaticName()`.
         * @param args `HostCigiEventArgs`-derived payload whose concrete type must match `eEvent`.
         *
         * Returns without dispatch when the global dispatcher is absent. Traverses a listener snapshot,
         * skipping unregistered entries and listeners of other types. Unknown event kinds are ignored.
         * Listener exceptions propagate to the caller.
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
