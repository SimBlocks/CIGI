//Copyright SimBlocks LLC 2016-2026
/**
 * @file IGResponseEventDispatcher.h
 * @brief Declares the `CIGResponseEventDispatcher` class for forwarding IG responses to packet senders and engine events.
 *
 * Provides the `CIGResponseEventDispatcher` class used by `IGCigiLib` to route generated IG-to-host response data structures.
 * Some responses are sent directly through the active packet senders, while terrain and line-of-sight responses are raised as engine events.
 * The dispatcher is stateless and relies on the active global `IGCigiLib` image generator and event infrastructure.
 *
 * @see sbio::cigi::ig::CIGResponseEventDispatcher
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::ig::CCigiPacketSenders
 * @see sbio::cigi::SCollisionDetectionSegmentNotification
 * @see sbio::cigi::SLineOfSightResponse
 * @see sbio::cigi::SSensorResponse
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LIB_IG_RESPONSE_EVENT_DISPATCHER_H
#define SIMBLOCKS_CIGI_LIB_IG_RESPONSE_EVENT_DISPATCHER_H

#include "CigiLib/CigiTypesIGToHost.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Bridges IG response structs into the engine event and messaging layer.
       */
      class CIGResponseEventDispatcher
      {
      public:
        /// @name Forwarded response helpers
        /// Each method forwards an already-populated IG response or notification to the exported
        /// messaging path used by the surrounding SDK.
        /// @{
        /**
         * @brief Sends a collision detection segment notification message.
         * @param data The collision detection segment notification message data.
         */
        void SendCollisionDetectionSegmentNotification(const sbio::cigi::SCollisionDetectionSegmentNotification& data);
        /**
         * @brief Sends a collision detection segment entity notification message.
         * @param data The collision detection segment entity notification message data.
         */
        void SendCollisionDetectionSegmentEntityNotification(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& data);
        /**
         * @brief Sends a collision detection volume notification message.
         * @param data The collision detection volume notification message data.
         */
        void SendCollisionDetectionVolumeNotification(const sbio::cigi::SCollisionDetectionVolumeNotification& data);
        /**
         * @brief Sends a collision detection volume entity notification message.
         * @param data The collision detection volume entity notification message data.
         */
        void SendCollisionDetectionVolumeEntityNotification(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& data);
        /**
         * @brief Sends an entity animation stop notification message.
         * @param data The entity animation stop notification message data.
         */
        void SendEntityAnimationStopNotification(const sbio::cigi::SAnimationStopNotification& data);
        /**
         * @brief Sends an event notification message.
         * @param data The event notification message data.
         */
        void SendEventNotification(const sbio::cigi::SEventNotification& data);
        /**
         * @brief Sends a height of terrain response message.
         * @param data The height of terrain response message data.
         */
        void SendHeightAboveTerrainResponse(const sbio::cigi::SHeightAboveTerrainResponse& data);
        /**
         * @brief Sends a height of terrain response message.
         * @param data The height of terrain response message data.
         */
        void SendHeightOfTerrainResponse(const sbio::cigi::SHeightOfTerrainResponse& data);
        /**
         * @brief Sends a height of terrain extended response message.
         * @param data The height of terrain extended response message data.
         */
        void SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& data);

        /**
         * @brief Sends an image generator notification message.
         * @param data The image generator notification message data.
         */
        void SendImageGeneratorNotification(const sbio::cigi::SImageGeneratorNotification& data);
        /**
         * @brief Sends a line of sight response message.
         * @param data The line of sight response message data.
         */
        void SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& data);

        void SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& data);
        /**
         * @brief Sends a line of sight extended response message.
         * @param data The line of sight extended response message data.
         */
        void SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& data);

        /**
         * @brief Sends a line of sight extended entity-geodetic response message.
         * @param data The line of sight extended entity-geodetic response message data.
         */
        void SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& data);

        /**
         * @brief Sends a line of sight extended entity-coordinates response message.
         * @param data The line of sight extended entity-coordinates response message data.
         */
        void SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& data);

        /**
         * @brief Sends a sensor response message.
         * @param data The sensor response message data.
         */
        void SendSensorResponse(const sbio::cigi::SSensorResponse& data);
        /**
         * @brief Sends a sensor extended response message.
         * @param data The sensor extended response message data.
         */
        void SendSensorExtendedResponse(const sbio::cigi::SSensorExtendedResponse& data);
        /**
         * @brief Sends a sensor extended entity response message.
         * @param data The sensor extended entity response message data.
         */
        void SendSensorExtendedEntityResponse(const sbio::cigi::SSensorExtendedEntityResponse& data);
        /// @}
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
