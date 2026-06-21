//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketSenders.h
 * @brief Declares the CCigiPacketSenders class for SimBlocks CIGI IG multi-host packet sending and notification management.
 *
 * Provides the CCigiPacketSenders class for managing and sending simulation packets to multiple hosts in the SimBlocks CIGI IG library.
 * Inherits from IPacketSender and integrates with SimBlocks CIGI, image generator, and packet sender types for simulation messaging and interoperability.
 *
 * Note: A traditional CIGI networking topology only has an IG communicate with a single host. This class was designed to allow an IG to communicate with multiple hosts for exceptional use cases.
 *
 * @see sbio::cigi::ig::CCigiPacketSenders
 * @see sbio::cigi::ig::IPacketSender
 * @see sbio::cigi::ig::CCigiPacketSender
 * @see sbio::cigi::ig::CCigiImageGenerator
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LIB_PACKET_SENDERS_H
#define SIMBLOCKS_CIGI_LIB_PACKET_SENDERS_H

#include "PacketSender.h"
#include <list>
#include <memory>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Fan-out sender that forwards IG-to-host packets to each configured host.
       */
      class CCigiPacketSenders : public IPacketSender
      {
      public:
        /**
         * @brief Adds a host for packet sending.
         * @param imageGenerator Reference to the image generator.
         * @param hostIPAddress Host IP address string.
         * @param igToHostPort IG-to-host port number.
         * @param eCigiVersion CIGI protocol version.
         */
        void AddHost(CCigiImageGenerator& imageGenerator, std::string hostIPAddress, int igToHostPort, ECigiVersion eCigiVersion);

        /// @name `IPacketSender` overrides
        /// These methods forward each packet or notification to every configured host sender.
        /// @{
        /**
         * @brief Sends a start-of-frame packet to all hosts.
         */
        virtual void SendStartOfFramePacket() override;
        /**
         * @brief Sends an animation stop notification to all hosts.
         * @param response Animation stop notification data.
         */
        virtual void SendAnimationFinishedNotification(const sbio::cigi::SAnimationStopNotification& response) override;
        /**
         * @brief Sends a height-above-terrain response to all hosts.
         * @param response Height-above-terrain response data.
         */
        virtual void SendHatResponse(const sbio::cigi::SHeightAboveTerrainResponse& response) override;

        /**
         * @brief Sends a height-of-terrain response to all hosts.
         * @param response Height-of-terrain response data.
         */
        virtual void SendHotResponse(const sbio::cigi::SHeightOfTerrainResponse& response) override;
        /**
         * @brief Sends an extended HAT/HOT response to all hosts.
         * @param response Extended HAT/HOT response data.
         */
        virtual void SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& response) override;
        /**
         * @brief Sends a collision-detection segment notification to all hosts.
         * @param response Collision-detection segment notification data.
         */
        virtual void SendCollisionDetectionSegmentNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentNotification& response) override;

        /**
         * @brief Sends a collision-detection segment entity notification to all hosts.
         * @param response Collision-detection segment entity notification data.
         */
        virtual void SendCollisionDetectionSegmentEntityNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& response) override;

        /**
         * @brief Sends a collision-detection volume notification to all hosts.
         * @param response Collision-detection volume notification data.
         */
        virtual void SendCollisionDetectionVolumeNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeNotification& response) override;

        /**
         * @brief Sends a collision-detection volume entity notification to all hosts.
         * @param response Collision-detection volume entity notification data.
         */
        virtual void SendCollisionDetectionVolumeEntityNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& response) override;
        /**
         * @brief Sends event notification to all hosts.
         * @param response Event notification data.
         */
        virtual void SendEventNotification(const sbio::cigi::SEventNotification& response) override;
        /**
         * @brief Sends an image-generator message notification to all hosts.
         * @param response Image-generator notification data.
         */
        virtual void SendImageGeneratorMessageNotification(const sbio::cigi::SImageGeneratorNotification& response) override;

        /**
         * @brief Sends a line-of-sight response to all hosts.
         * @param response Line-of-sight response data.
         */
        virtual void SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& response) override;

        /**
         * @brief Sends a line-of-sight entity response to all hosts.
         * @param response Line-of-sight entity response data.
         */
        virtual void SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& response) override;

        /**
         * @brief Sends an extended line-of-sight response expressed in geodetic coordinates to all hosts.
         * @param response Extended line-of-sight geodetic response data.
         */
        virtual void SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& response) override;

        /**
         * @brief Sends an extended line-of-sight response expressed in entity coordinates to all hosts.
         * @param response Extended line-of-sight entity-coordinate response data.
         */
        virtual void SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& response) override;

        /**
         * @brief Sends an extended line-of-sight entity response expressed in geodetic coordinates to all hosts.
         * @param response Extended line-of-sight entity geodetic response data.
         */
        virtual void SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& response) override;

        /**
         * @brief Sends a position response to all hosts.
         * @param response Pointer to base position response data.
         * @param type Position response type.
         */
        virtual void SendPositionResponse(const SBasePositionResponse* response, sbio::cigi::EPositionResponseType type) override;
        /**
         * @brief Sends a weather conditions response to all hosts.
         * @param response Weather conditions response data.
         */
        virtual void SendWeatherConditionsResponse(const sbio::cigi::SWeatherConditionsResponse& response) override;
        /**
         * @brief Sends an aerosol concentration response to all hosts.
         * @param response Aerosol concentration response data.
         */
        virtual void SendAerosolConcentrationResponse(const sbio::cigi::SAerosolConcentrationResponse& response) override;
        /**
         * @brief Sends a maritime surface conditions response to all hosts.
         * @param response Maritime surface conditions response data.
         */
        virtual void SendMaritimeSurfaceConditionsResponse(const sbio::cigi::SMaritimeSurfaceConditionsResponse& response) override;
        /**
         * @brief Sends a terrestrial surface conditions response to all hosts.
         * @param response Terrestrial surface conditions response data.
         */
        virtual void SendTerrestrialSurfaceConditionsResponse(const sbio::cigi::STerrestrialSurfaceConditionsResponse& response) override;
        /**
         * @brief Sends a sensor response to all hosts.
         * @param response Sensor response data.
         */
        virtual void SendSensorResponse(const sbio::cigi::SSensorResponse& response) override;
        /**
         * @brief Sends an extended sensor response to all hosts.
         * @param response Extended sensor response data.
         */
        virtual void SendSensorExtendedResponse(const sbio::cigi::SSensorExtendedResponse& response) override;

        /**
         * @brief Sends an extended sensor entity response to all hosts.
         * @param response Extended sensor entity response data.
         */
        virtual void SendSensorExtendedEntityResponse(const sbio::cigi::SSensorExtendedEntityResponse& response) override;
        /// @}

        /**
         * @brief Sends simulation packets to all hosts.
         */
        void SendPackets();

      private:
        std::list<std::unique_ptr<sbio::cigi::ig::CCigiPacketSender>> m_pPacketSenders;///< Owned per-host packet senders.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
