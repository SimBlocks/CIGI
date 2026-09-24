//Copyright SimBlocks LLC 2016-2026
/**
 * @file TerrainHandler.h
 * @brief Declares the CCigiTerrainHandler class for SimBlocks CIGI IG terrain and line of sight request management.
 *
 * Provides the CCigiTerrainHandler class for handling terrain-related CIGI events, line of sight requests, HAT/HOT requests, and top-level entity clamping in the SimBlocks CIGI IG
 * library. Integrates with SimBlocks CIGI, math, event, and image generator types for terrain simulation, event dispatching, and request management. Supports event listener
 * interface for terrain CIGI events, request handling for various line of sight and HAT/HOT types, entity position updates, and terrain simulation state management.
 *
 * @see sbio::cigi::ig::CCigiTerrainHandler
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::ig::ITerrainCigiEventListener
 * @see sbio::cigi::ig::CigiHatHotResponseEventArgs
 * @see sbio::cigi::ig::CigiHatHotExtendedResponseEventArgs
 * @see sbio::cigi::ig::CigiLineOfSightResponseEventArgs
 * @see sbio::cigi::ig::CigiLineOfSightExtendedResponseEventArgs
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TERRAIN_HANDLER_H
#define SIMBLOCKS_CIGI_TERRAIN_HANDLER_H

#include "CigiLib/CigiTypesHostToIG.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/ImageGenerator.h"
#include "TerrainCigiEvent.h"
#include "CigiEvent.h"

class CHATHOTRequestHandler;
class CLineOfSightRequestHandler;
class CLineOfSightSegmentRequestHandler;

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Handles terrain-related CIGI events, line of sight requests, and entity clamping for SimBlocks CIGI IG integration.
       *
       * Implements ITerrainCigiEventListener to process terrain CIGI events and manages line of sight and HAT/HOT requests.
       */
      class CCigiTerrainHandler : ITerrainCigiEventListener, public IIGCIGIEventListener
      {
      public:
        /**
         * @brief Constructs a terrain handler.
         * @param ImageGenerator Reference to the image generator.
         */
        CCigiTerrainHandler(CCigiImageGenerator& ImageGenerator);

        /** @brief Unregisters event listeners and releases retained request handlers. */
        ~CCigiTerrainHandler();

        /** @brief Ignores database-loaded notifications. */
        virtual void OnDatabaseLoadedEvent() override {};
        /** @brief Ignores database-load failures. */
        virtual void OnDatabaseLoadingFailedEvent() override {};
        /**
         * @brief Removes retained HAT/HOT and line-of-sight requests referencing the entity, including scheduled updates.
         * @param entityID Identifier of the removed entity.
         */
        virtual void OnEntityRemoved(sbio::EntityID entityID) override;
        /**
         * @brief Ignores image-generator error notifications.
         * @param args Unused error details.
         */
        virtual void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) override {};

        // ITerrainCigiEventListener overrides
        /**
         * @brief Handles HAT/HOT response event.
         * @param args Event arguments.
         */
        virtual void OnCigiHatResponseEvent(const CigiHatResponseEventArgs& args) override;
        /**
         * @brief Handles HOT response event.
         * @param args Event arguments.
         */
        virtual void OnCigiHotResponseEvent(const CigiHotResponseEventArgs& args) override;
        /**
         * @brief Handles extended HAT response event.
         * @param args Event arguments.
         */
        virtual void OnCigiHatHotExtendedResponseEvent(const CigiHatHotExtendedResponseEventArgs& args) override;
        /**
         * @brief Handles line of sight response event.
         * @param args Event arguments.
         */
        virtual void OnCigiLineOfSightResponseEvent(const CigiLineOfSightResponseEventArgs& args) override;
        /**
         * @brief Forwards a matching entity response, adjusting valid ranges and retiring completed one-shot requests.
         * @param args Response and request-generation metadata; unmatched or stale responses are ignored.
         */
        virtual void OnCigiLineOfSightEntityResponseEvent(const CigiLineOfSightEntityResponseEventArgs& args) override;
        /**
         * @brief Handles extended line of sight response event.
         * @param args Event arguments.
         */
        virtual void OnCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Called on extended line of sight response event with entity geodetic coordinates.
         * @param args Event arguments.
         */
        virtual void OnCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(const CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Called on extended line of sight response event with entity coordinates.
         * @param args Event arguments.
         */
        virtual void OnCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args) override;

        // Request handling
        /**
         * @brief Handles line of sight segment request (geodetic to geodetic).
         * @param lineOfSightRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Submits an extended geodetic-to-geodetic segment query.
         * @param lineOfSightRequest Query endpoints and response options.
         * @param isFromHost Whether to retain the request for response correlation and periodic updates.
         * @return True if submitted; false if the request handler cannot submit the query.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Handles line of sight segment request (geodetic to entity).
         * @param lineOfSightRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Submits an extended geodetic-to-entity segment query.
         * @param lineOfSightRequest Query endpoints and response options.
         * @param isFromHost Whether to retain the request for response correlation and periodic updates.
         * @return True if submitted; false if the request handler cannot submit the query.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Handles line of sight segment request (entity to geodetic).
         * @param lineOfSightRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Submits an extended entity-to-geodetic segment query.
         * @param lineOfSightRequest Query endpoints and response options.
         * @param isFromHost Whether to retain the request for response correlation and periodic updates.
         * @return True if submitted; false if the request handler cannot submit the query.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticExtended& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Handles line of sight segment request (entity to entity).
         * @param lineOfSightRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Submits an extended entity-to-entity segment query.
         * @param lineOfSightRequest Query endpoints and response options.
         * @param isFromHost Whether to retain the request for response correlation and periodic updates.
         * @return True if submitted; false if the request handler cannot submit the query.
         */
        bool Handle(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Handles line of sight vector request (geodetic).
         * @param lineOfSightRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Submits an extended vector query from a geodetic origin.
         * @param lineOfSightRequest Query origin, direction, range, and response options.
         * @param isFromHost Whether to retain the request for response correlation and periodic updates.
         * @return True if submitted; false if the request handler cannot submit the query.
         */
        bool Handle(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Handles line of sight vector request (entity).
         * @param lineOfSightRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Submits an extended vector query from an entity-relative origin.
         * @param lineOfSightRequest Query origin, direction, range, and response options.
         * @param isFromHost Whether to retain the request for response correlation and periodic updates.
         * @return True if submitted; false if the request handler cannot submit the query.
         */
        bool Handle(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& lineOfSightRequest, bool isFromHost = true);
        /**
         * @brief Handles HAT/HOT request.
         * @param hatHotRequest Request data.
         * @param isFromHost True if request is from host.
         * @return True if handled successfully.
         */
        bool Handle(const sbio::cigi::SHATHOTGlobalRequest& hatHotRequest, bool isFromHost = true);
        /**
         * @brief Submits an entity-relative HAT/HOT query.
         * @param hatHotRequest Query location and response options.
         * @param isFromHost Whether to cache the request and update its periodic scheduling state.
         * @return True if submitted; false for a non-host one-shot request or failed submission.
         */
        bool Handle(const sbio::cigi::SHATHOTEntityRequest& hatHotRequest, bool isFromHost = true);
        /**
         * @brief Handles top-level entity position update.
         * @param topLevelEntityPosition Entity position data.
         */
        void Handle(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition);
        /**
         * @brief Updates terrain handler state.
         * @param lastHostFrameNumber Last host frame number.
         */
        void Update(FrameNumber lastHostFrameNumber);
        /**
         * @brief Resets terrain handler state.
         */
        void Reset();

      private:
        /**
         * @brief Resolves a geodetic point with offset and coordinate system.
         * @param coordinates Geodetic coordinates.
         * @param offset Offset vector.
         * @param coordinateSystem Coordinate system type.
         * @param entityID Entity identifier.
         * @param point Output parameter for resolved point.
         * @return True if resolved successfully.
         */
        bool ResolvePoint(const sbio::math::SGeodeticCoordinates& coordinates, const sbio::math::Vec3& offset, sbio::ETopLevelCoordinateSystem coordinateSystem,
                          sbio::EntityID entityID, GeocentricCoordinates& point);
        /**
         * @brief Gets the host frame number least significant nibble.
         * @param frameNumber Frame number value.
         * @return Least significant nibble.
         */
        uint8_t GetHostFrameNumberLSN(FrameNumber frameNumber);
        /**
         * @brief Gets azimuth and elevation from normal and coordinates.
         * @param normal Surface normal vector.
         * @param coord Geodetic coordinates.
         * @param azimuth Output azimuth value.
         * @param elevation Output elevation value.
         */
        void GetAzimuthAndElevation(const sbio::math::GeocentricCoordinates& normal, const sbio::math::SGeodeticCoordinates& coord, sbio::math::Degrees180& azimuth,
                                    sbio::math::Degrees90& elevation);

        std::unordered_set<LineOfSightRequestID, StrongTypeHash<LineOfSightRequestID>> LineOfSightUpdateRequests;///< Line of sight update request IDs
        std::unordered_set<HATHOTID, StrongTypeHash<HATHOTID>> HATHOTUpdateRequests;///< HAT/HOT update request IDs
        std::unordered_map<LineOfSightRequestID, std::unique_ptr<CLineOfSightRequestHandler>, StrongTypeHash<LineOfSightRequestID>>
          m_LineOfSightRequests;///< Line of sight request handlers
        std::unordered_map<HATHOTID, std::unique_ptr<CHATHOTRequestHandler>, StrongTypeHash<HATHOTID>> HATHOTRequests;///< HAT/HOT requests

        /**
         * @brief Submits due HAT/HOT updates and removes unschedulable or failed requests.
         * @tparam RequestType Request identifier type.
         * @tparam Hash Hash function for request identifiers.
         * @param updateRequests Periodic request identifiers, pruned in place.
         * @param requests Retained handlers, pruned when submission fails.
         * @param lastHostFrameNumber Host frame number associated with submitted updates.
         */
        template <class RequestType, class Hash>
        void UpdateHATHOTRequests(std::unordered_set<RequestType, Hash>& updateRequests, std::unordered_map<RequestType, std::unique_ptr<CHATHOTRequestHandler>, Hash>& requests,
                                  int lastHostFrameNumber);
        /**
         * @brief Submits due line-of-sight updates and removes requests whose submission fails.
         * @param lastHostFrameNumber Host frame number associated with submitted updates.
         */
        void UpdateLineOfSightRequests(int lastHostFrameNumber);
        /**
         * @brief Caches a host global request and updates periodic scheduling membership.
         * @tparam RequestType Request identifier type.
         * @tparam Hash Hash function for request identifiers.
         * @param req Request to cache when isFromHost is true.
         * @param updateRequests Periodic request identifiers to update.
         * @param requests Retained handlers; a host request replaces the matching entry.
         * @param isFromHost Whether the request came from the host.
         * @return False only for a non-host request with a zero update period.
         */
        template <class RequestType, class Hash>
        bool CheckHost(const sbio::cigi::SHATHOTGlobalRequest& req, std::unordered_set<RequestType, Hash>& updateRequests,
                       std::unordered_map<RequestType, std::unique_ptr<CHATHOTRequestHandler>, Hash>& requests, bool isFromHost);
        /**
         * @brief Caches a host entity request and updates periodic scheduling membership.
         * @tparam RequestType Request identifier type.
         * @tparam Hash Hash function for request identifiers.
         * @param req Request to cache when isFromHost is true.
         * @param updateRequests Periodic request identifiers to update.
         * @param requests Retained handlers; a host request replaces the matching entry.
         * @param isFromHost Whether the request came from the host.
         * @return False only for a non-host request with a zero update period.
         */
        template <class RequestType, class Hash>
        bool CheckHost(const sbio::cigi::SHATHOTEntityRequest& req, std::unordered_set<RequestType, Hash>& updateRequests,
                       std::unordered_map<RequestType, std::unique_ptr<CHATHOTRequestHandler>, Hash>& requests, bool isFromHost);
        CCigiImageGenerator& m_ImageGenerator;///< Reference to the image generator
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
