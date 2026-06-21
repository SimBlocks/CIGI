//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketSenderV4.h
 * @brief Declares the CCigiPacketSenderV4 class for SimBlocks CIGI IG packet sending (CIGI version 4).
 *
 * Provides the CCigiPacketSenderV4 class for sending simulation packets over UDP to a host using CIGI version 4 in the SimBlocks CIGI IG library.
 * Inherits from CCigiPacketSender and integrates with SimBlocks CIGI, image generator, and utility types for simulation messaging and network communication.
 * Note: A traditional CIGI networking topology only has an IG communicate with a single host. This class supports IG-to-host communication for CIGI version 4.
 *
 * @see sbio::cigi::ig::CCigiPacketSenderV4
 * @see sbio::cigi::ig::CCigiPacketSender
 * @see sbio::cigi::ig::CCigiImageGenerator
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LIB_PACKET_SENDER_V4_H
#define SIMBLOCKS_CIGI_LIB_PACKET_SENDER_V4_H

#include "IGCigiLib/PacketSender.h"
#include <memory>
#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Serializes and sends IG-to-host packets using the CIGI 4.0 packet layout.
       */
      class CCigiPacketSenderV4 : public CCigiPacketSender
      {
      public:
        /**
         * @brief Constructs a packet sender for CIGI version 4.
         * @param imageGenerator Reference to the image generator.
         * @param hostIPAddress Host IP address string.
         * @param igToHostPort IG-to-host port number.
         */
        CCigiPacketSenderV4(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort);

        /// @name `IPacketSender` overrides
        /// These methods serialize logical IG responses using the CIGI 4.0 packet layout.
        /// @{
        /**
         * @brief Sends a start-of-frame packet.
         */
        virtual void SendStartOfFramePacket() override;
        /**
         * @brief Sends an animation stop notification.
         * @param response Animation stop notification data.
         */
        virtual void SendAnimationFinishedNotification(const sbio::cigi::SAnimationStopNotification& response) override;
        /**
         * @brief Sends a height-above-terrain response.
         * @param response Height-above-terrain response data.
         */
        virtual void SendHatResponse(const sbio::cigi::SHeightAboveTerrainResponse& response) override;
        /**
         * @brief Sends a height-of-terrain response.
         * @param response Height-of-terrain response data.
         */
        virtual void SendHotResponse(const sbio::cigi::SHeightOfTerrainResponse& response) override;
        /**
         * @brief Sends an extended HAT/HOT response.
         * @param response Extended HAT/HOT response data.
         */
        virtual void SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& response) override;
        /**
         * @brief Sends a collision-detection segment notification.
         * @param response Collision-detection segment notification data.
         */
        virtual void SendCollisionDetectionSegmentNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentNotification& response) override;

        /**
         * @brief Sends a collision-detection segment entity notification.
         * @param response Collision-detection segment entity notification data.
         */
        virtual void SendCollisionDetectionSegmentEntityNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& response) override;

        /**
         * @brief Sends a collision-detection volume notification.
         * @param response Collision-detection volume notification data.
         */
        virtual void SendCollisionDetectionVolumeNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeNotification& response) override;

        /**
         * @brief Sends a collision-detection volume entity notification.
         * @param response Collision-detection volume entity notification data.
         */
        virtual void SendCollisionDetectionVolumeEntityNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& response) override;

        /**
         * @brief Sends an event notification.
         * @param response Event notification data.
         */
        virtual void SendEventNotification(const sbio::cigi::SEventNotification& response) override;
        /**
         * @brief Sends an image-generator message notification.
         * @param response Image-generator notification data.
         */
        virtual void SendImageGeneratorMessageNotification(const sbio::cigi::SImageGeneratorNotification& response) override;

        /**
         * @brief Sends a line-of-sight response.
         * @param response Line-of-sight response data.
         */
        virtual void SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& response) override;

        /**
         * @brief Sends a line-of-sight entity response.
         * @param response Line-of-sight entity response data.
         */
        virtual void SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& response) override;

        /**
         * @brief Sends an extended line-of-sight response expressed in geodetic coordinates.
         * @param response Extended line-of-sight geodetic response data.
         */
        virtual void SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& response) override;

        /**
         * @brief Sends an extended line-of-sight response expressed in entity coordinates.
         * @param response Extended line-of-sight entity-coordinate response data.
         */
        virtual void SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& response) override;

        /**
         * @brief Sends an extended line-of-sight entity response expressed in geodetic coordinates.
         * @param response Extended line-of-sight entity geodetic response data.
         */
        virtual void SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& response) override;

        /**
         * @brief Sends a position response.
         * @param response Pointer to base position response data.
         * @param type Position response type.
         */
        virtual void SendPositionResponse(const SBasePositionResponse* response, sbio::cigi::EPositionResponseType type) override;
        /**
         * @brief Sends a weather conditions response.
         * @param response Weather conditions response data.
         */
        virtual void SendWeatherConditionsResponse(const sbio::cigi::SWeatherConditionsResponse& response) override;
        /**
         * @brief Sends an aerosol concentration response.
         * @param response Aerosol concentration response data.
         */
        virtual void SendAerosolConcentrationResponse(const sbio::cigi::SAerosolConcentrationResponse& response) override;
        /**
         * @brief Sends a maritime surface conditions response.
         * @param response Maritime surface conditions response data.
         */
        virtual void SendMaritimeSurfaceConditionsResponse(const sbio::cigi::SMaritimeSurfaceConditionsResponse& response) override;
        /**
         * @brief Sends a terrestrial surface conditions response.
         * @param response Terrestrial surface conditions response data.
         */
        virtual void SendTerrestrialSurfaceConditionsResponse(const sbio::cigi::STerrestrialSurfaceConditionsResponse& response) override;
        /**
         * @brief Sends a sensor response.
         * @param response Sensor response data.
         */
        virtual void SendSensorResponse(const SSensorResponse& response) override;
        /**
         * @brief Sends an extended sensor response.
         * @param response Extended sensor response data.
         */
        virtual void SendSensorExtendedResponse(const SSensorExtendedResponse& response) override;

        /**
         * @brief Sends an extended sensor entity response.
         * @param response Extended sensor entity response data.
         */
        virtual void SendSensorExtendedEntityResponse(const SSensorExtendedEntityResponse& response) override;
        /// @}

      protected:
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
