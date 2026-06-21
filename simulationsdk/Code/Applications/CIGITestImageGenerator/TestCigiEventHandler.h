//Copyright SimBlocks LLC 2016-2026
/**
 * @file TestCigiEventHandler.h
 * @brief Declares the CTestCigiEventHandler class for handling CIGI event messages in the test image generator.
 *
 * Provides the CTestCigiEventHandler class for handling a wide range of CIGI event messages in the test image generator application.
 * Implements the IImageGeneratorEventHandler interface to process entity, terrain, physics, view, animation, symbol, celestial, sensor, atmosphere, ocean, earth, and system messages.
 *
 * @see sbio::cigi::CTestCigiEventHandler
 * @see sbio::ig::IImageGeneratorEventHandler
 */
#pragma once
#ifndef SIMBLOCKS_UNREAL_EVENT_HANDLER_H
#define SIMBLOCKS_UNREAL_EVENT_HANDLER_H

#include "EngineLib/IImageGeneratorEventMessenger.h"
#include <atomic>

namespace sbio
{
  namespace cigi
  {
    /**
     * @brief Event handler for CIGI test image generator event messages.
     *
     * Handles entity, terrain, physics, view, animation, symbol, celestial, sensor, atmosphere, ocean, earth, and system messages.
     */
    class CTestCigiEventHandler : public sbio::ig::IImageGeneratorEventHandler
    {
    public:
      CTestCigiEventHandler();

      virtual ~CTestCigiEventHandler() override;

      virtual bool IsPointInEntityVolume(const sbio::math::GeocentricCoordinates& point, sbio::EntityID entityID) const override;
      virtual void OnCreateEntityMessage(const sbio::ig::entity::SCreateEntityMessage& data) override;
      virtual void OnDestroyEntityMessage(const sbio::ig::entity::SDestroyEntityMessage& data) override;
      virtual void OnUpdateTopLevelEntityTransformMessage(const sbio::ig::entity::SUpdateTopLevelEntityTransformMessage& data) override;
      virtual void OnUpdateChildEntityTransformMessage(const sbio::ig::entity::SUpdateChildEntityTransformMessage& data) override;
      virtual void OnSetEntityAttachedMessage(const sbio::ig::entity::SSetEntityAttachedMessage& data) override;
      virtual void OnSetEntityActiveMessage(const sbio::ig::entity::SSetEntityActiveMessage& data) override;
      virtual void OnSetEntityUnattachedMessage(const sbio::ig::entity::SSetEntityUnattachedMessage& data) override;
      virtual void OnSetEntityComponentStateMessage(const sbio::ig::entity::SSetEntityComponentStateMessage& data) override;
      virtual void OnUpdateArticulatedPartTransformMessage(const sbio::ig::entity::SUpdateArticulatedPartTransformMessage& data) override;
      virtual void OnSetArticulatedPartVisibleMessage(const sbio::ig::entity::SSetArticulatedPartVisibleMessage& data) override;
      virtual void OnSetEntityAlphaMessage(const sbio::ig::entity::SSetEntityAlphaMessage& data) override;
      virtual void OnSetEntityCollisionDetectionEnabledMessage(const sbio::ig::entity::SSetEntityCollisionDetectionEnabledMessage& data) override;
      virtual void OnLineOfSightSegmentRequestBasicMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestBasicMessage& data) override;
      virtual void OnLineOfSightSegmentRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestExtendedMessage& data) override;
      virtual void OnLineOfSightVectorRequestBasicMessage(const sbio::ig::terrain::SLineOfSightVectorRequestBasicMessage& data) override;
      virtual void OnLineOfSightVectorRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightVectorRequestExtendedMessage& data) override;
      virtual void OnHeightAboveTerrainRequestMessage(const sbio::ig::terrain::SHeightAboveTerrainRequestMessage& data) override;
      virtual void OnHeightOfTerrainRequestMessage(const sbio::ig::terrain::SHeightOfTerrainRequestMessage& data) override;
      virtual void OnTerrestrialSurfaceConditionsChangedMessage() override;
      virtual void OnSetRegionalTerrainSurfaceComponentStateMessage(const sbio::ig::terrain::SSetRegionalTerrainSurfaceComponentStateMessage& data) override;
      virtual void OnSetGlobalTerrainComponentStateMessage(const sbio::ig::terrain::SSetGlobalTerrainComponentStateMessage& data) override;
      virtual void OnSetCollisionDetectionSegmentEnabledMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentEnabledMessage& data) override;
      virtual void OnSetCollisionDetectionSegmentMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentMessage& data) override;
      virtual void OnCreateCollisionDetectionSegmentMessage(const sbio::ig::physics::SCreateCollisionDetectionSegmentMessage& data) override;
      virtual void OnCreateCollisionVolumeSphereMessage(const sbio::ig::physics::SCreateCollisionVolumeSphereMessage& data) override;
      virtual void OnCreateCollisionVolumeCuboidMessage(const sbio::ig::physics::SCreateCollisionVolumeCuboidMessage& data) override;
      virtual void OnSetCollisionVolumeEnabledMessage(const sbio::ig::physics::SSetCollisionVolumeEnabledMessage& data) override;
      virtual void OnSetCollisionVolumeMessage(const sbio::ig::physics::SSetCollisionVolumeMessage& data) override;
      virtual void OnDestroyCollisionVolumeMessage(const sbio::ig::physics::SDestroyCollisionVolumeMessage& data) override;
      virtual void OnUpdateAttachedCameraTransformMessage(const sbio::ig::view::SUpdateAttachedCameraTransformMessage& data) override;
      virtual void OnSetCameraAttachedToEntityMessage(const sbio::ig::view::SSetCameraAttachedToEntityMessage& data) override;
      virtual void OnSetCameraUnattachedMessage(const sbio::ig::view::SSetCameraUnattachedMessage& data) override;
      virtual void OnSetCameraProjectionMessage(const sbio::ig::view::SSetCameraProjectionMessage& data) override;
      virtual void OnSetViewComponentStateMessage(const sbio::ig::view::SSetViewComponentStateMessage& data) override;
      virtual void OnSetViewGroupComponentStateMessage(const sbio::ig::view::SSetViewGroupComponentStateMessage& data) override;
      virtual void OnUnloadDatabaseMessage() override;
      virtual void OnLoadDatabaseMessage(const sbio::ig::database::SLoadDatabaseMessage& data) override;
      virtual void OnSetAnimationDirectionMessage(const sbio::ig::animation::SSetAnimationDirectionMessage& data) override;
      virtual void OnSetAnimationLoopModeMessage(const sbio::ig::animation::SSetAnimationLoopModeMessage& data) override;
      virtual void OnSetAnimationSpeedMessage(const sbio::ig::animation::SSetAnimationSpeedMessage& data) override;
      virtual void OnStopEntityAnimationMessage(const sbio::ig::animation::SStopEntityAnimationMessage& data) override;
      virtual void OnStopAtCurrentFrameEntityAnimationMessage(const sbio::ig::animation::SStopAtCurrentFrameEntityAnimationMessage& data) override;
      virtual void OnPauseEntityAnimationMessage(const sbio::ig::animation::SPauseEntityAnimationMessage& data) override;
      virtual void OnPlayEntityAnimationMessage(const sbio::ig::animation::SPlayEntityAnimationMessage& data) override;
      virtual void OnRestartEntityAnimationMessage(const sbio::ig::animation::SRestartEntityAnimationMessage& data) override;
      virtual void OnCreateSymbolTextMessage(const sbio::ig::symbol::SCreateSymbolTextMessage& data) override;
      virtual void OnUpdateSymbolTextMessage(const sbio::ig::symbol::SUpdateSymbolTextMessage& data) override;
      virtual void OnSetSymbolColorMessage(const sbio::ig::symbol::SSetSymbolColorMessage& data) override;
      virtual void OnDestroySymbolMessage(const sbio::ig::symbol::SDestroySymbolMessage& data) override;
      virtual void OnCreateSymbolCircleMessage(const sbio::ig::symbol::SCreateSymbolCircleMessage& data) override;
      virtual void OnUpdateSymbolCircleMessage(const sbio::ig::symbol::SUpdateSymbolCircleMessage& data) override;
      virtual void OnUpdateSymbolCircleElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleElementMessage& data) override;
      virtual void OnUpdateSymbolCircleFilledMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledMessage& data) override;
      virtual void OnUpdateSymbolCircleFilledElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledElementMessage& data) override;
      virtual void OnCreateSymbolTexturedCircleMessage(const sbio::ig::symbol::SCreateSymbolTexturedCircleMessage& data) override;
      virtual void OnUpdateSymbolTexturedCircleMessage(const sbio::ig::symbol::SUpdateSymbolTexturedCircleMessage& data) override;
      virtual void OnUpdateTexturedCircleMessage(const sbio::ig::symbol::SUpdateTexturedCircleMessage& data) override;
      virtual void OnCreateSymbolPolygonMessage(const sbio::ig::symbol::SCreateSymbolPolygonMessage& data) override;
      virtual void OnUpdateSymbolPolygonMessage(const sbio::ig::symbol::SUpdateSymbolPolygonMessage& data) override;
      virtual void OnSetSymbolPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolPolygonVertexMessage& data) override;
      virtual void OnCreateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SCreateSymbolTexturedPolygonMessage& data) override;
      virtual void OnUpdateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SUpdateSymbolTexturedPolygonMessage& data) override;
      virtual void OnSetSymbolTexturedPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolTexturedPolygonVertexMessage& data) override;
      virtual void OnUpdateEntityBillboardSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateEntityBillboardSymbolSurfaceMessage& data) override;
      virtual void OnCreateSymbolSurfaceMessage(const sbio::ig::symbol::SCreateSymbolSurfaceMessage& data) override;
      virtual void OnDestroySymbolSurfaceMessage(const sbio::ig::symbol::SDestroySymbolSurfaceMessage& data) override;
      virtual void OnUpdateSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateSymbolSurfaceMessage& data) override;
      virtual void OnUpdateViewSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateViewSymbolSurfaceMessage& data) override;
      virtual void OnSetSymbolVisibleMessage(const sbio::ig::symbol::SSetSymbolVisibleMessage& data) override;
      virtual void OnSetSymbolAttachedMessage(const sbio::ig::symbol::SSetSymbolAttachedMessage& data) override;
      virtual void OnSetSymbolUnattachedMessage(const sbio::ig::symbol::SSetSymbolUnattachedMessage& data) override;
      virtual void OnSetSymbolSurfaceMessage(const sbio::ig::symbol::SSetSymbolSurfaceMessage& data) override;
      virtual void OnSetTopLevelSymbolTransformMessage(const sbio::ig::symbol::SSetTopLevelSymbolTransformMessage& data) override;
      virtual void OnSetChildSymbolTransformMessage(const sbio::ig::symbol::SSetChildSymbolTransformMessage& data) override;
      virtual void OnUpdateSymbolMessage(const sbio::ig::symbol::SUpdateSymbolMessage& data) override;
      virtual void OnSetSymbolComponentStateMessage(const sbio::ig::symbol::SSetSymbolComponentStateMessage& data) override;
      virtual void OnSetSymbolSurfaceComponentStateMessage(const sbio::ig::symbol::SSetSymbolSurfaceComponentStateMessage& data) override;
      virtual void OnCreateSymbolFromTemplateMessage(const sbio::ig::symbol::SCreateSymbolFromTemplateMessage& data) override;
      virtual void OnUpdateCelestialSphereMessage(const sbio::ig::celestial::SUpdateCelestialSphereMessage& data) override;
      virtual void OnUpdateDateTimeMessage(const sbio::ig::celestial::SUpdateDateTimeMessage& data) override;
      virtual void OnSetCelestialSphereComponentStateMessage(const sbio::ig::celestial::SSetCelestialSphereComponentStateMessage& data) override;
      virtual void OnUpdateSensorMessage(const sbio::ig::sensor::SUpdateSensorMessage& data) override;
      virtual void OnUpdateSensorComponentMessage(const sbio::ig::sensor::SUpdateSensorComponentMessage& data) override;
      virtual void OnCreateMotionTrackerViewMessage(const sbio::ig::sensor::SCreateMotionTrackerViewMessage& data) override;
      virtual void OnCreateMotionTrackerViewGroupMessage(const sbio::ig::sensor::SCreateMotionTrackerViewGroupMessage& data) override;
      virtual void OnSetMotionTrackerMessage(const sbio::ig::sensor::SSetMotionTrackerMessage& data) override;
      virtual void OnSetAtmosphereEnabledMessage(const sbio::ig::atmosphere::SSetAtmosphereEnabledMessage& data) override;
      virtual void OnSetAtmosphereMessage(const sbio::ig::atmosphere::SSetAtmosphereMessage& data) override;
      virtual void OnSetWeatherMessage(const sbio::ig::atmosphere::SSetWeatherMessage& data) override;
      virtual void OnSetRegionalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetRegionalLayeredWeatherComponentStateMessage& data) override;
      virtual void OnSetGlobalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetGlobalLayeredWeatherComponentStateMessage& data) override;
      virtual void OnSetAtmosphereComponentStateMessage(const sbio::ig::atmosphere::SSetAtmosphereComponentStateMessage& data) override;
      virtual void OnSetMaritimeSurfaceConditionsMessage(const sbio::ig::ocean::SSetMaritimeSurfaceConditionsMessage& data) override;
      virtual void OnSetRegionMaritimeComponentStateMessage(const sbio::ig::ocean::SSetRegionMaritimeComponentStateMessage& data) override;
      virtual void OnSetGlobalMaritimeComponentStateMessage(const sbio::ig::ocean::SSetGlobalMaritimeComponentStateMessage& data) override;
      virtual void OnSetEarthReferenceModelMessage(const sbio::ig::earth::SSetEarthReferenceModelMessage& data) override;
      virtual void OnSetEventComponentStateMessage(const sbio::ig::system::SSetEventComponentStateMessage& data) override;
      virtual void OnSetSystemComponentStateMessage(const sbio::ig::system::SSetSystemComponentStateMessage& data) override;
      virtual void OnSetHostConnectedMessage(const sbio::ig::network::SHostConnectedMessage& data) override;
      virtual void OnSetHostDisconnectedMessage(const sbio::ig::network::SHostDisconnectedMessage& data) override;

      /**
       * @brief Updates the event handler state.
       */
      void Update();

    private:
      std::atomic<bool> m_bLoadDatabaseComplete = false;///< Indicates if database load is complete
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
