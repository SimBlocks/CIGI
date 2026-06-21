//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiEvent.h
 * @brief Declares CIGI event types, event arguments, listeners, and handlers for SimBlocks CIGI IG event system.
 *
 * Provides enums, structs, and classes for handling CIGI IG events such as database loading, entity removal, and error reporting.
 * Integrates with SimBlocks utilities for event dispatching, handling, and listener interfaces.
 * Supports custom event arguments and event types for simulation event management.
 *
 * @see sbio::cigi::ig::EIGCIGIEvent
 * @see sbio::cigi::ig::IGCIGIEvent
 * @see sbio::cigi::ig::SIGCIGIEventArgs
 * @see sbio::cigi::ig::SDatabaseLoadedEventArgs
 * @see sbio::cigi::ig::SEntityRemovedEventArgs
 * @see sbio::cigi::ig::SImageGeneratorErrorEventArgs
 * @see sbio::cigi::ig::IIGCIGIEventListener
 * @see sbio::cigi::ig::IGCIGIEventHandler
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_EVENT_H
#define SIMBLOCKS_CIGI_EVENT_H

#include "CigiLib/CigiTypes.h"
#include "UtilitiesLib/Event.h"
#include "UtilitiesLib/EventHandler.h"
#include "UtilitiesLib/IEventListener.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Enumerates the high-level CIGI IG events published through the utility event system.
       */
      enum class EIGCIGIEvent
      {
        UNKNOWN,///< Event kind has not been identified.
        DATABASE_FINISHED_LOADING,///< Signals completion of a database load attempt.
        DATABASE_LOADING_FAILED,///< Signals that a database load failed.
        ENTITY_REMOVED,///< Signals that an entity was removed from the simulation.
      };

      /**
       * @brief Concrete event type used to dispatch `EIGCIGIEvent` notifications.
       */
      class IGCIGIEvent : public sbio::utils::Event
      {
      public:
        /**
         * @brief Returns the registration name for this event type.
         * @return Static event name used by the utility event dispatcher.
         */
        static std::string GetStaticName()
        {
          return "IGCIGIEvent";
        }

        /**
         * @brief Dispatches this event to a handler.
         * @param handler Handler selected by the event dispatcher.
         * @param sEvent Event channel name.
         * @param args Concrete event arguments.
         */
        virtual void Visit(const sbio::utils::EventHandler& handler, const std::string& sEvent, const sbio::utils::EventArgs& args) const override;
      };

      /**
       * @brief Base argument type for all `IGCIGIEvent` argument data.
       *
       */
      struct SIGCIGIEventArgs : public sbio::utils::EventArgs
      {
        /**
         * @brief Constructs base event arguments with a specific event kind.
         * @param e Event kind stored in `eEvent`.
         */
        SIGCIGIEventArgs(EIGCIGIEvent e) : eEvent(e) {};

        EIGCIGIEvent eEvent;///< Event kind carried by this argument object.
      };

      /**
       * @brief Event arguments describing the completion of a database load attempt.
       */
      struct SDatabaseLoadedEventArgs : public SIGCIGIEventArgs
      {
        SDatabaseLoadedEventArgs() : SIGCIGIEventArgs(EIGCIGIEvent::DATABASE_FINISHED_LOADING) {};

        bool bLoadSuccessful = false;///< Indicates whether the database load completed successfully.
      };

      /**
       * @brief Event arguments identifying a removed entity.
       */
      struct SEntityRemovedEventArgs : public SIGCIGIEventArgs
      {
        SEntityRemovedEventArgs() : SIGCIGIEventArgs(EIGCIGIEvent::ENTITY_REMOVED) {};

        sbio::EntityID entityID = UnknownEntityID;///< Identifier of the entity that was removed.
      };

      /**
       * @brief Event arguments describing an image-generator error.
       */
      struct SImageGeneratorErrorEventArgs : public SIGCIGIEventArgs
      {
        SImageGeneratorErrorEventArgs() : SIGCIGIEventArgs(EIGCIGIEvent::DATABASE_LOADING_FAILED) {};

        sbio::cigi::SImageGeneratorError error;///< Error data associated with the event.
      };

      /**
       * @brief Listener interface for high-level CIGI IG events.
       */
      struct IIGCIGIEventListener : public sbio::utils::IEventListener
      {
      public:
        virtual ~IIGCIGIEventListener() {};

        /// @name High-level IG lifecycle callbacks
        /// @{
        /**
         * @brief Called after a database load completes.
         */
        virtual void OnDatabaseLoadedEvent() = 0;
        /**
         * @brief Called when a database load fails.
         */
        virtual void OnDatabaseLoadingFailedEvent() = 0;
        /**
         * @brief Called when an entity is removed.
         * @param entityID Identifier of the removed entity.
         */
        virtual void OnEntityRemoved(sbio::EntityID entityID) = 0;
        /**
         * @brief Called when an image-generator error event is published.
         * @param args Error data describing the failure.
         */
        virtual void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) = 0;
        /// @}
      };

      /**
       * @brief Bridges `IGCIGIEvent` dispatches to `IIGCIGIEventListener` implementations.
       */
      class IGCIGIEventHandler : public sbio::utils::EventHandler
      {
      public:
        /**
         * @brief Dispatches one `IGCIGIEvent` instance to the appropriate listener callback.
         * @param event Event object supplied by the dispatcher.
         * @param sEvent Event channel name.
         * @param args Concrete event arguments.
         */
        virtual void Visit(const sbio::utils::Event& event, const std::string& sEvent, const sbio::utils::EventArgs& args) const override;

      private:
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
