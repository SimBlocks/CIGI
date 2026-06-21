//Copyright SimBlocks LLC 2016-2026
/**
 * @file TerrainCigiEvent.h
 * @brief Declares event types and handlers for SimBlocks CIGI IG terrain-related CIGI events.
 *
 * Provides enums, event argument structures, event listener interfaces, and event handler classes for terrain-related CIGI events in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, math, and utility types for event-driven terrain simulation and CIGI IG communication.
 * Supports HAT/HOT response, line of sight response, extended response events, and event dispatching for terrain simulation.
 *
 * @see sbio::cigi::ig::ETerrainCigiEvent
 * @see sbio::cigi::ig::TerrainCigiEvent
 * @see sbio::cigi::ig::TerrainCigiEventArgs
 * @see sbio::cigi::ig::CigiHatHotResponseEventArgs
 * @see sbio::cigi::ig::CigiHatHotExtendedResponseEventArgs
 * @see sbio::cigi::ig::CigiLineOfSightResponseEventArgs
 * @see sbio::cigi::ig::CigiLineOfSightExtendedResponseEventArgs
 * @see sbio::cigi::ig::ITerrainCigiEventListener
 * @see sbio::cigi::ig::TerrainCigiEventHandler
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_EVENT_H
#define SIMBLOCKS_HOST_EMULATOR_EVENT_H

#include "CigiLib/CigiTypesIGToHost.h"
#include "GlobalHeaders/CommonTypes.h"
#include "UtilitiesLib/Event.h"
#include "UtilitiesLib/EventHandler.h"
#include "UtilitiesLib/IEventListener.h"
#include "EngineLib/ImageGeneratorMessages.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Enumerates the terrain-related response events emitted by the CIGI IG library.
       */
      enum class ETerrainCigiEvent
      {
        UNKNOWN = 1,
        HAT_RESPONSE,
        HOT_RESPONSE,
        HAT_HOT_EXTENDED_RESPONSE,
        LINE_OF_SIGHT_RESPONSE,
        LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE,
        LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE,
        LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE,
        LINE_OF_SIGHT_NON_ENTITY_RESPONSE,
        LINE_OF_SIGHT_ENTITY_RESPONSE,
      };

      /**
       * @brief Concrete event type used to dispatch terrain-related CIGI responses.
       */
      class TerrainCigiEvent : public sbio::utils::Event
      {
      public:
        /**
         * @brief Returns the registration name for terrain events.
         * @return Static terrain event channel name.
         */
        static std::string GetStaticName()
        {
          return "TerrainEvent";
        }

        /**
         * @brief Dispatches a terrain event to a handler.
         * @param handler Handler selected by the event dispatcher.
         * @param args Concrete terrain event arguments.
         */
        virtual void Visit(const sbio::utils::EventHandler& handler, const sbio::utils::EventArgs& args) const override;
      };

      /**
       * @brief Base argument type for all terrain-response events.
       */
      struct TerrainCigiEventArgs : public sbio::utils::EventArgs
      {
        /**
         * @brief Constructs base terrain event arguments.
         * @param e Terrain event kind stored in `eEvent`.
         */
        TerrainCigiEventArgs(ETerrainCigiEvent e) : eEvent(e)
        {
        }

        ETerrainCigiEvent eEvent = {ETerrainCigiEvent::UNKNOWN};
      };

      /**
       * @brief Event arguments carrying a HAT response.
       */
      struct CigiHatResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiHatResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::HAT_RESPONSE)
        {
        }

        SHeightAboveTerrainResponse hatResponse;///< HAT response data.
      };

      /**
       * @brief Event arguments carrying a HOT response.
       */
      struct CigiHotResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiHotResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::HOT_RESPONSE)
        {
        }

        SHeightOfTerrainResponse hotResponse;///< HOT response data.
      };

      /**
       * @brief Event arguments carrying an extended HAT/HOT response.
       */
      struct CigiHatHotExtendedResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiHatHotExtendedResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::HAT_HOT_EXTENDED_RESPONSE)
        {
        }

        sbio::cigi::SHATHOTExtendedResponse hatHotExtendedResponse;///< Extended HAT/HOT response data.
        sbio::math::GeocentricCoordinates IntersectionPoint;///< Reported intersection point in geocentric coordinates.
        sbio::math::GeocentricCoordinates Normal;///< Reported surface normal in geocentric coordinates.
      };

      /**
       * @brief Event arguments carrying a line-of-sight response.
       */
      struct CigiLineOfSightResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiLineOfSightResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::LINE_OF_SIGHT_NON_ENTITY_RESPONSE)
        {
        }

        SLineOfSightResponse lineOfSightResponse;
      };

      /**
       * @brief Event arguments carrying a line-of-sight entity response.
       */
      struct CigiLineOfSightEntityResponseEventArgs : public TerrainCigiEventArgs
      {
        /**
         * @brief Constructs event arguments for a line-of-sight entity response.
         */
        CigiLineOfSightEntityResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::LINE_OF_SIGHT_ENTITY_RESPONSE)
        {
        }

        SLineOfSightEntityResponse lineOfSightEntityResponse;///< LOS entity response data.
      };

      /**
       * @brief Event arguments carrying an extended geodetic LOS response.
       */
      struct CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::LINE_OF_SIGHT_EXTENDED_GEODETIC_COORDINATES_RESPONSE)
        {
        }

        SLineOfSightExtendedGeodeticCoordinatesResponse lineOfSightExtendedGeodeticCoordinatesResponse;///< Extended LOS response data expressed in geodetic coordinates.
      };

      /**
       * @brief Event arguments carrying an extended entity-coordinate LOS response.
       */
      struct CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_COORDINATES_RESPONSE)
        {
        }

        SLineOfSightExtendedEntityCoordinatesResponse lineOfSightExtendedEntityCoordinatesResponse;///< Extended LOS response data expressed in entity coordinates.
      };

      /**
       * @brief Event arguments carrying an extended entity-geodetic LOS response.
       */
      struct CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs : public TerrainCigiEventArgs
      {
        CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs() : TerrainCigiEventArgs(ETerrainCigiEvent::LINE_OF_SIGHT_EXTENDED_ENTITY_GEODETIC_COORDINATES_RESPONSE)
        {
        }

        SLineOfSightExtendedEntityGeodeticCoordinatesResponse lineOfSightExtendedEntityGeodeticCoordinatesResponse;
      };

      /**
       * @brief Listener interface for terrain-related CIGI responses.
       */
      struct ITerrainCigiEventListener : public sbio::utils::IEventListener
      {
      public:
        virtual ~ITerrainCigiEventListener() {};

        /// @name Terrain and line-of-sight response callbacks
        /// @{
        /**
         * @brief Called when a HAT response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiHatResponseEvent(const CigiHatResponseEventArgs& args) = 0;
        /**
         * @brief Called when a HOT response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiHotResponseEvent(const CigiHotResponseEventArgs& args) = 0;
        /**
         * @brief Called when an extended HAT/HOT response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiHatHotExtendedResponseEvent(const CigiHatHotExtendedResponseEventArgs& args) = 0;
        /**
         * @brief Called when a LOS response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiLineOfSightResponseEvent(const CigiLineOfSightResponseEventArgs& args) = 0;
        virtual void OnCigiLineOfSightEntityResponseEvent(const CigiLineOfSightEntityResponseEventArgs& args) = 0;
        /**
         * @brief Called when an extended geodetic LOS response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args) = 0;
        /**
         * @brief Called when an extended entity-coordinate LOS response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args) = 0;
        /**
         * @brief Called when an extended entity-geodetic LOS response is dispatched.
         * @param args Event argument data.
         */
        virtual void OnCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(const CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args) = 0;
        /// @}
      };

      /**
       * @brief Bridges terrain event dispatches to `ITerrainCigiEventListener` callbacks.
       */
      class TerrainCigiEventHandler : public sbio::utils::EventHandler
      {
      public:
        /**
         * @brief Dispatches one terrain event to the appropriate listener callback.
         * @param e Event object supplied by the dispatcher.
         * @param sEvent Event channel name.
         * @param args Concrete event arguments.
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
