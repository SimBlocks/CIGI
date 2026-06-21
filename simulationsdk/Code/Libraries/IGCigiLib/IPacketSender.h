//Copyright SimBlocks LLC 2016-2026
/**
 * @file IPacketSender.h
 * @brief Declares the IPacketSender interface for sending CIGI IG packets and notifications.
 *
 * Provides the IPacketSender interface for sending various simulation packets and notifications in the SimBlocks CIGI IG library, including frame, animation, collision, event, sensor, and position responses.
 * Integrates with SimBlocks CIGI, entity, and global types for simulation messaging and interoperability.
 *
 * @see sbio::cigi::ig::IPacketSender
 * @see SCreateEntityMessage
 * @see SDestroyEntityMessage
 * @see SUpdateTopLevelEntityTransformMessage
 * @see SUpdateChildEntityTransformMessage
 * @see SSetEntityAttachedMessage
 * @see SSetEntityActiveMessage
 * @see SSetEntityUnattachedMessage
 * @see SSetEntityComponentStateMessage
 * @see SUpdateArticulatedPartTransformMessage
 * @see SSetArticulatedPartVisibleMessage
 * @see SSetEntityAlphaMessage
 * @see SSetEntityCollisionDetectionEnabledMessage
 * @see SLineOfSightRequestMessage
 * @see SLineOfSightResponseMessage
 * @see SLineOfSightExtendedResponseMessage
 * @see SHeightOfTerrainRequestMessage
 * @see SHeightOfTerrainResponseMessage
 * @see SHeightOfTerrainExtendedResponseMessage
 * @see SUpdateTerrestrialSurfaceConditionsMessage
 * @see SSetRegionalTerrainComponentStateMessage
 * @see SSetGlobalTerrainComponentStateMessage
 * @see SSetCollisionDetectionSegmentEnabledMessage
 * @see SCreateCollisionDetectionSegmentMessage
 * @see SCreateCollisionVolumeSphereMessage
 * @see SCreateCollisionVolumeCuboidMessage
 * @see SSetCollisionVolumeEnabledMessage
 * @see SSetCollisionVolumeSphereRadiusMessage
 * @see SSetCollisionVolumeOffsetMessage
 */
#pragma once

#include "CigiLib/CigiTypeDeclarations.h"
#include "EntityLib/EntityType.h"
#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "CigiLib/CigiTypesIGToHost.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Interface for sending IG-to-host CIGI packets and notifications.
       */
      struct IPacketSender
      {
      public:
        /**
         * @brief Destroys the packet sender interface.
         */
        virtual ~IPacketSender() {};

        /// @name Frame and status notifications
        /// @{
        /**
         * @brief Sends a start-of-frame packet.
         */
        virtual void SendStartOfFramePacket() = 0;
        /**
         * @brief Sends an animation stop notification.
         * @param response Animation stop notification data.
         */
        virtual void SendAnimationFinishedNotification(const sbio::cigi::SAnimationStopNotification& response) = 0;
        /// @}

        /// @name Terrain and line-of-sight responses
        /// @{
        /**
         * @brief Sends a height-above-terrain response.
         * @param response Height-above-terrain response data.
         */
        virtual void SendHatResponse(const sbio::cigi::SHeightAboveTerrainResponse& response) = 0;
        /**
         * @brief Sends a height-of-terrain response.
         * @param response Height-of-terrain response data.
         */
        virtual void SendHotResponse(const sbio::cigi::SHeightOfTerrainResponse& response) = 0;
        /**
         * @brief Sends an extended HAT/HOT response.
         * @param response Extended HAT/HOT response data.
         */
        virtual void SendHatHotExtendedResponse(const sbio::cigi::SHATHOTExtendedResponse& response) = 0;
        /**
         * @brief Sends a collision-detection segment notification.
         * @param response Collision-detection segment notification data.
         */
        virtual void SendCollisionDetectionSegmentNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentNotification& response) = 0;

        /**
         * @brief Sends a collision-detection segment entity notification.
         * @param response Collision-detection segment entity notification data.
         */
        virtual void SendCollisionDetectionSegmentEntityNotificationResponse(const sbio::cigi::SCollisionDetectionSegmentEntityNotification& response) = 0;
        /**
         * @brief Sends a collision-detection volume notification.
         * @param response Collision-detection volume notification data.
         */
        virtual void SendCollisionDetectionVolumeNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeNotification& response) = 0;

        /**
         * @brief Sends a collision-detection volume entity notification.
         * @param response Collision-detection volume entity notification data.
         */
        virtual void SendCollisionDetectionVolumeEntityNotificationResponse(const sbio::cigi::SCollisionDetectionVolumeEntityNotification& response) = 0;

        /**
         * @brief Sends an event notification.
         * @param response Event notification data.
         */
        virtual void SendEventNotification(const sbio::cigi::SEventNotification& response) = 0;
        /**
         * @brief Sends an image-generator message notification.
         * @param response Image-generator notification data.
         */
        virtual void SendImageGeneratorMessageNotification(const sbio::cigi::SImageGeneratorNotification& response) = 0;

        /**
         * @brief Sends a line-of-sight response.
         * @param response Line-of-sight response data.
         */
        virtual void SendLineOfSightResponse(const sbio::cigi::SLineOfSightResponse& response) = 0;

        /**
         * @brief Sends a line-of-sight entity response.
         * @param response Line-of-sight entity response data.
         */
        virtual void SendLineOfSightEntityResponse(const sbio::cigi::SLineOfSightEntityResponse& response) = 0;

        /**
         * @brief Sends an extended line-of-sight response expressed in geodetic coordinates.
         * @param response Extended line-of-sight geodetic response data.
         */
        virtual void SendLineOfSightExtendedGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& response) = 0;

        /**
         * @brief Sends an extended line-of-sight response expressed in entity coordinates.
         * @param response Extended line-of-sight entity-coordinate response data.
         */
        virtual void SendLineOfSightExtendedEntityCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityCoordinatesResponse& response) = 0;

        /**
         * @brief Sends an extended line-of-sight entity response expressed in geodetic coordinates.
         * @param response Extended line-of-sight entity geodetic response data.
         */
        virtual void SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(const sbio::cigi::SLineOfSightExtendedEntityGeodeticCoordinatesResponse& response) = 0;
        /// @}

        /// @name Position, weather, and surface responses
        /// @{
        /**
         * @brief Sends a position response.
         * @param response Pointer to base position response data.
         * @param type Position response type.
         */
        virtual void SendPositionResponse(const SBasePositionResponse* response, sbio::cigi::EPositionResponseType type) = 0;
        /**
         * @brief Sends a weather conditions response.
         * @param response Weather conditions response data.
         */
        virtual void SendWeatherConditionsResponse(const sbio::cigi::SWeatherConditionsResponse& response) = 0;
        /**
         * @brief Sends an aerosol concentration response.
         * @param response Aerosol concentration response data.
         */
        virtual void SendAerosolConcentrationResponse(const sbio::cigi::SAerosolConcentrationResponse& response) = 0;
        /**
         * @brief Sends a maritime surface conditions response.
         * @param response Maritime surface conditions response data.
         */
        virtual void SendMaritimeSurfaceConditionsResponse(const sbio::cigi::SMaritimeSurfaceConditionsResponse& response) = 0;
        /**
         * @brief Sends a terrestrial surface conditions response.
         * @param response Terrestrial surface conditions response data.
         */
        virtual void SendTerrestrialSurfaceConditionsResponse(const sbio::cigi::STerrestrialSurfaceConditionsResponse& response) = 0;
        /// @}

        /// @name Sensor, collision, and generic notification responses
        /// @{
        /**
         * @brief Sends a sensor response.
         * @param response Sensor response data.
         */
        virtual void SendSensorResponse(const sbio::cigi::SSensorResponse& response) = 0;
        /**
         * @brief Sends an extended sensor response.
         * @param response Extended sensor response data.
         */
        virtual void SendSensorExtendedResponse(const sbio::cigi::SSensorExtendedResponse& response) = 0;

        /**
         * @brief Sends an extended sensor entity response.
         * @param response Extended sensor entity response data.
         */
        virtual void SendSensorExtendedEntityResponse(const sbio::cigi::SSensorExtendedEntityResponse& response) = 0;
        /// @}
      };
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
