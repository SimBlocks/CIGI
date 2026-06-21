//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_IMAGE_GENERATOR_EVENT_MESSENGER_H
#define SIMBLOCKS_IMAGE_GENERATOR_EVENT_MESSENGER_H

#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "GlobalHeaders/CommonTypes.h"

namespace sbio
{
  namespace ig
  {
    /**
     * @brief Forwards strongly typed engine messages to an image-generator event handler.
     *
     * `CImageGeneratorEventMessenger` is a thin convenience wrapper around the generated
     * `IImageGeneratorEventHandler` interface. Each `Send...Message()` method forwards directly to
     * the corresponding `On...Message()` callback.
     *
     * Ownership:
     * - `m_pEventHandler` is non-owning and must outlive the messenger.
     */
    class CImageGeneratorEventMessenger
    {
    public:
      /**
       * @brief Constructs a messenger bound to an event handler implementation.
       * @param pEventHandler Non-owning target that receives forwarded messages.
       */
      CImageGeneratorEventMessenger(IImageGeneratorEventHandler* pEventHandler);

      virtual ~CImageGeneratorEventMessenger()
      {
      }

      /**
       * @brief Tests whether a point lies inside the collision or volume representation of an entity.
       * @return Result forwarded from the underlying event handler.
       */
      virtual bool IsPointInEntityVolume(const sbio::math::GeocentricCoordinates& point, sbio::EntityID entityID) const;

      /// @name Entity messages
      /// @{
      virtual void SendCreateEntityMessage(const sbio::ig::entity::SCreateEntityMessage& data);
      virtual void SendDestroyEntityMessage(const sbio::ig::entity::SDestroyEntityMessage& data);
      virtual void SendUpdateTopLevelEntityTransformMessage(const sbio::ig::entity::SUpdateTopLevelEntityTransformMessage& data);
      virtual void SendUpdateChildEntityTransformMessage(const sbio::ig::entity::SUpdateChildEntityTransformMessage& data);
      virtual void SendSetEntityAttachedMessage(const sbio::ig::entity::SSetEntityAttachedMessage& data);
      virtual void SendSetEntityActiveMessage(const sbio::ig::entity::SSetEntityActiveMessage& data);
      virtual void SendSetEntityUnattachedMessage(const sbio::ig::entity::SSetEntityUnattachedMessage& data);
      virtual void SendSetEntityComponentStateMessage(const sbio::ig::entity::SSetEntityComponentStateMessage& data);
      virtual void SendUpdateArticulatedPartTransformMessage(const sbio::ig::entity::SUpdateArticulatedPartTransformMessage& data);
      virtual void SendSetArticulatedPartVisibleMessage(const sbio::ig::entity::SSetArticulatedPartVisibleMessage& data);
      virtual void SendSetEntityAlphaMessage(const sbio::ig::entity::SSetEntityAlphaMessage& data);
      virtual void SendSetEntityCollisionDetectionEnabledMessage(const sbio::ig::entity::SSetEntityCollisionDetectionEnabledMessage& data);
      /// @}

      /// @name Terrain and collision messages
      /// @{
      virtual void SendLineOfSightSegmentRequestBasicMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestBasicMessage& data);
      virtual void SendLineOfSightSegmentRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestExtendedMessage& data);
      virtual void SendLineOfSightVectorRequestBasicMessage(const sbio::ig::terrain::SLineOfSightVectorRequestBasicMessage& data);
      virtual void SendLineOfSightVectorRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightVectorRequestExtendedMessage& data);
      virtual void SendHeightAboveTerrainRequestMessage(const sbio::ig::terrain::SHeightAboveTerrainRequestMessage& data);
      virtual void SendHeightOfTerrainRequestMessage(const sbio::ig::terrain::SHeightOfTerrainRequestMessage& data);
      virtual void SendTerrestrialSurfaceConditionsChangedMessage();
      virtual void SendSetRegionalTerrainSurfaceComponentStateMessage(const sbio::ig::terrain::SSetRegionalTerrainSurfaceComponentStateMessage& data);
      virtual void SendSetGlobalTerrainComponentStateMessage(const sbio::ig::terrain::SSetGlobalTerrainComponentStateMessage& data);
      virtual void SendSetCollisionDetectionSegmentEnabledMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentEnabledMessage& data);
      virtual void SendSetCollisionDetectionSegmentMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentMessage& data);
      virtual void SendCreateCollisionDetectionSegmentMessage(const sbio::ig::physics::SCreateCollisionDetectionSegmentMessage& data);
      virtual void SendCreateCollisionVolumeSphereMessage(const sbio::ig::physics::SCreateCollisionVolumeSphereMessage& data);
      virtual void SendCreateCollisionVolumeCuboidMessage(const sbio::ig::physics::SCreateCollisionVolumeCuboidMessage& data);
      virtual void SendSetCollisionVolumeEnabledMessage(const sbio::ig::physics::SSetCollisionVolumeEnabledMessage& data);
      virtual void SendSetCollisionVolumeMessage(const sbio::ig::physics::SSetCollisionVolumeMessage& data);
      virtual void SendDestroyCollisionVolumeMessage(const sbio::ig::physics::SDestroyCollisionVolumeMessage& data);
      /// @}

      /// @name View and database messages
      /// @{
      virtual void SendUpdateAttachedCameraTransformMessage(const sbio::ig::view::SUpdateAttachedCameraTransformMessage& data);
      virtual void SendSetCameraAttachedToEntityMessage(const sbio::ig::view::SSetCameraAttachedToEntityMessage& data);
      virtual void SendSetCameraUnattachedMessage(const sbio::ig::view::SSetCameraUnattachedMessage& data);
      virtual void SendSetCameraProjectionMessage(const sbio::ig::view::SSetCameraProjectionMessage& data);
      virtual void SendSetViewComponentStateMessage(const sbio::ig::view::SSetViewComponentStateMessage& data);
      virtual void SendSetViewGroupComponentStateMessage(const sbio::ig::view::SSetViewGroupComponentStateMessage& data);
      virtual void SendLoadDatabaseMessage(const sbio::ig::database::SLoadDatabaseMessage& data);
      virtual void SendUnloadDatabaseMessage();
      /// @}

      /// @name Animation and symbol messages
      /// @{
      virtual void SendSetAnimationDirectionMessage(const sbio::ig::animation::SSetAnimationDirectionMessage& data);
      virtual void SendSetAnimationLoopModeMessage(const sbio::ig::animation::SSetAnimationLoopModeMessage& data);
      virtual void SendSetAnimationSpeedMessage(const sbio::ig::animation::SSetAnimationSpeedMessage& data);
      virtual void SendStopEntityAnimationMessage(const sbio::ig::animation::SStopEntityAnimationMessage& data);
      virtual void SendStopAtCurrentFrameEntityAnimationMessage(const sbio::ig::animation::SStopAtCurrentFrameEntityAnimationMessage& data);
      virtual void SendPauseEntityAnimationMessage(const sbio::ig::animation::SPauseEntityAnimationMessage& data);
      virtual void SendPlayEntityAnimationMessage(const sbio::ig::animation::SPlayEntityAnimationMessage& data);
      virtual void SendRestartEntityAnimationMessage(const sbio::ig::animation::SRestartEntityAnimationMessage& data);
      virtual void SendCreateSymbolTextMessage(const sbio::ig::symbol::SCreateSymbolTextMessage& data);
      virtual void SendUpdateSymbolTextMessage(const sbio::ig::symbol::SUpdateSymbolTextMessage& data);
      virtual void SendSetSymbolColorMessage(const sbio::ig::symbol::SSetSymbolColorMessage& data);
      virtual void SendDestroySymbolMessage(const sbio::ig::symbol::SDestroySymbolMessage& data);
      virtual void SendCreateSymbolCircleMessage(const sbio::ig::symbol::SCreateSymbolCircleMessage& data);
      virtual void SendUpdateSymbolCircleMessage(const sbio::ig::symbol::SUpdateSymbolCircleMessage& data);
      virtual void SendUpdateSymbolCircleElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleElementMessage& data);
      virtual void SendUpdateSymbolCircleFilledMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledMessage& data);
      virtual void SendUpdateSymbolCircleFilledElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledElementMessage& data);
      virtual void SendCreateSymbolTexturedCircleMessage(const sbio::ig::symbol::SCreateSymbolTexturedCircleMessage& data);
      virtual void SendUpdateSymbolTexturedCircleMessage(const sbio::ig::symbol::SUpdateSymbolTexturedCircleMessage& data);
      virtual void SendUpdateTexturedCircleMessage(const sbio::ig::symbol::SUpdateTexturedCircleMessage& data);
      virtual void SendCreateSymbolPolygonMessage(const sbio::ig::symbol::SCreateSymbolPolygonMessage& data);
      virtual void SendUpdateSymbolPolygonMessage(const sbio::ig::symbol::SUpdateSymbolPolygonMessage& data);
      virtual void SendSetSymbolPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolPolygonVertexMessage& data);
      virtual void SendCreateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SCreateSymbolTexturedPolygonMessage& data);
      virtual void SendUpdateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SUpdateSymbolTexturedPolygonMessage& data);
      virtual void SendSetSymbolTexturedPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolTexturedPolygonVertexMessage& data);
      virtual void SendUpdateEntityBillboardSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateEntityBillboardSymbolSurfaceMessage& data);
      virtual void SendCreateSymbolSurfaceMessage(const sbio::ig::symbol::SCreateSymbolSurfaceMessage& data);
      virtual void SendDestroySymbolSurfaceMessage(const sbio::ig::symbol::SDestroySymbolSurfaceMessage& data);
      virtual void SendUpdateSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateSymbolSurfaceMessage& data);
      virtual void SendUpdateViewSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateViewSymbolSurfaceMessage& data);
      virtual void SendSetSymbolVisibleMessage(const sbio::ig::symbol::SSetSymbolVisibleMessage& data);
      virtual void SendSetSymbolAttachedMessage(const sbio::ig::symbol::SSetSymbolAttachedMessage& data);
      virtual void SendSetSymbolUnattachedMessage(const sbio::ig::symbol::SSetSymbolUnattachedMessage& data);
      virtual void SendSetSymbolSurfaceMessage(const sbio::ig::symbol::SSetSymbolSurfaceMessage& data);
      virtual void SendSetTopLevelSymbolTransformMessage(const sbio::ig::symbol::SSetTopLevelSymbolTransformMessage& data);
      virtual void SendSetChildSymbolTransformMessage(const sbio::ig::symbol::SSetChildSymbolTransformMessage& data);
      virtual void SendUpdateSymbolMessage(const sbio::ig::symbol::SUpdateSymbolMessage& data);
      virtual void SendSetSymbolComponentStateMessage(const sbio::ig::symbol::SSetSymbolComponentStateMessage& data);
      virtual void SendSetSymbolSurfaceComponentStateMessage(const sbio::ig::symbol::SSetSymbolSurfaceComponentStateMessage& data);
      virtual void SendCreateSymbolFromTemplateMessage(const sbio::ig::symbol::SCreateSymbolFromTemplateMessage& data);
      /// @}

      /// @name Environment, sensor, and system messages
      /// @{
      virtual void SendUpdateCelestialSphereMessage(const sbio::ig::celestial::SUpdateCelestialSphereMessage& data);
      virtual void SendUpdateDateTimeMessage(const sbio::ig::celestial::SUpdateDateTimeMessage& data);
      virtual void SendSetCelestialSphereComponentStateMessage(const sbio::ig::celestial::SSetCelestialSphereComponentStateMessage& data);
      virtual void SendUpdateSensorMessage(const sbio::ig::sensor::SUpdateSensorMessage& data);
      virtual void SendUpdateSensorComponentMessage(const sbio::ig::sensor::SUpdateSensorComponentMessage& data);
      virtual void SendCreateMotionTrackerViewMessage(const sbio::ig::sensor::SCreateMotionTrackerViewMessage& data);
      virtual void SendCreateMotionTrackerViewGroupMessage(const sbio::ig::sensor::SCreateMotionTrackerViewGroupMessage& data);
      virtual void SendSetMotionTrackerMessage(const sbio::ig::sensor::SSetMotionTrackerMessage& data);
      virtual void SendSetAtmosphereEnabledMessage(const sbio::ig::atmosphere::SSetAtmosphereEnabledMessage& data);
      virtual void SendSetAtmosphereMessage(const sbio::ig::atmosphere::SSetAtmosphereMessage& data);
      virtual void SendSetWeatherMessage(const sbio::ig::atmosphere::SSetWeatherMessage& data);
      virtual void SendSetRegionalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetRegionalLayeredWeatherComponentStateMessage& data);
      virtual void SendSetGlobalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetGlobalLayeredWeatherComponentStateMessage& data);
      virtual void SendSetAtmosphereComponentStateMessage(const sbio::ig::atmosphere::SSetAtmosphereComponentStateMessage& data);
      virtual void SendSetMaritimeSurfaceConditionsMessage(const sbio::ig::ocean::SSetMaritimeSurfaceConditionsMessage& data);
      virtual void SendSetRegionMaritimeComponentStateMessage(const sbio::ig::ocean::SSetRegionMaritimeComponentStateMessage& data);
      virtual void SendSetGlobalMaritimeComponentStateMessage(const sbio::ig::ocean::SSetGlobalMaritimeComponentStateMessage& data);
      virtual void SendSetEarthReferenceModelMessage(const sbio::ig::earth::SSetEarthReferenceModelMessage& data);
      virtual void SendSetEventComponentStateMessage(const sbio::ig::system::SSetEventComponentStateMessage& data);
      virtual void SendSetSystemComponentStateMessage(const sbio::ig::system::SSetSystemComponentStateMessage& data);
      virtual void SendHostConnectedMessage(const sbio::ig::network::SHostConnectedMessage& data);
      virtual void SendHostDisconnectedMessage(const sbio::ig::network::SHostDisconnectedMessage& data);
      /// @}

    private:
      IImageGeneratorEventHandler* m_pEventHandler = nullptr;
    };
  }
}

#endif//#ifndef SIMBLOCKS_IMAGE_GENERATOR_EVENT_MESSENGER_H

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
