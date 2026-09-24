//Copyright SimBlocks LLC 2016-2026
/**
 * @file TestCigiEventHandler.h
 * @brief Declares the CTestCigiEventHandler class for handling CIGI event messages in the test image generator.
 *
 * Provides the CTestCigiEventHandler class for handling a wide range of CIGI event messages in the test image generator application.
 * Implements the IImageGeneratorEventHandler interface to process entity, terrain, physics, view, animation, symbol, celestial, sensor, atmosphere, ocean, earth, and system
 * messages.
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
     * @brief Diagnostic IG event handler with synthetic terrain, collision, and database responses.
     *
     * Callbacks described as logging only write debug diagnostics; they do not create rendering resources or apply
     * scene changes. Message arguments are borrowed for the call. LOS and collision workers copy their request data.
     * Most callbacks require globals.pLogger; volume queries also require globals.pEntityManager.
     *
     * LOS handlers capture a shared response dispatcher on the calling thread and start detached workers that wait
     * 100 milliseconds before queuing a response. A process-wide atomic counter shared by all four LOS handlers
     * alternates non-entity and entity hits in worker execution order. Replies use range 100, response count 1, and
     * entity ID 2 for entity hits, without testing scene geometry. Extended replies use material 1 and zero intersection
     * coordinates. Request IDs, generation values, and host-frame nibbles are preserved. Missing dispatchers produce
     * no response, and closed dispatchers discard queued submissions.
     *
     * Collision workers instead consult the current global IG when they send. Database workers capture this object.
     * Neither destruction nor unload joins or cancels workers. The handler must outlive its database workers, and
     * global services used by collision workers must remain valid for their execution. The completion flag does not
     * provide per-load correlation or cancellation.
     */
    class CTestCigiEventHandler : public sbio::ig::IImageGeneratorEventHandler
    {
    public:
      /** @brief Constructs a handler with no pending database-completion notification; does not register it. */
      CTestCigiEventHandler();

      /** @brief Destroys the handler without waiting for or cancelling detached workers. */
      virtual ~CTestCigiEventHandler() override;

      /**
       * @brief Tests a synthetic sphere of radius 100 meters (for testing purposes) around a managed entity's world position.
       * @param point World-space query point in the same coordinate frame as the entity's position.
       * @param entityID Entity to look up through globals.pEntityManager.
       * @return True when the distance is at most 100; false otherwise, including a missing entity, which is logged.
       */
      virtual bool IsPointInEntityVolume(const sbio::math::GeocentricCoordinates& point, sbio::EntityID entityID) const override;
      /**
       * @brief Reports that motion-tracker position sampling is not implemented.
       * @param trackerID Unused tracker identifier.
       * @param offset Output offset, left unchanged.
       * @param rotation Output rotation, left unchanged.
       * @return Always false; no tracker data is supplied.
       */
      virtual bool GetMotionTrackerPosition(sbio::MotionTrackerID trackerID, sbio::math::Vec3& offset, sbio::math::TBodyEulerRotation& rotation) const override;
      /** @brief Logs an entity-creation request without creating an engine entity.
       * @param data Borrowed creation message to log.
       */
      virtual void OnCreateEntityMessage(const sbio::ig::entity::SCreateEntityMessage& data) override;
      /** @brief Logs an entity-destruction request without destroying an engine entity.
       * @param data Borrowed destruction message to log.
       */
      virtual void OnDestroyEntityMessage(const sbio::ig::entity::SDestroyEntityMessage& data) override;
      /** @brief Logs a top-level entity transform without applying it.
       * @param data Borrowed transform message to log.
       */
      virtual void OnUpdateTopLevelEntityTransformMessage(const sbio::ig::entity::SUpdateTopLevelEntityTransformMessage& data) override;
      /** @brief Logs the entity ID of a child-transform update without applying the transform.
       * @param data Borrowed message supplying the entity ID.
       */
      virtual void OnUpdateChildEntityTransformMessage(const sbio::ig::entity::SUpdateChildEntityTransformMessage& data) override;
      /** @brief Logs an entity attachment request without modifying attachment state.
       * @param data Borrowed attachment message to log.
       */
      virtual void OnSetEntityAttachedMessage(const sbio::ig::entity::SSetEntityAttachedMessage& data) override;
      /** @brief Logs an entity active-state request without applying it.
       * @param data Borrowed active-state message to log.
       */
      virtual void OnSetEntityActiveMessage(const sbio::ig::entity::SSetEntityActiveMessage& data) override;
      /** @brief Logs an entity detachment request without modifying attachment state.
       * @param data Borrowed detachment message to log.
       */
      virtual void OnSetEntityUnattachedMessage(const sbio::ig::entity::SSetEntityUnattachedMessage& data) override;
      /** @brief Logs entity component state without applying it.
       * @param data Borrowed component-state message to log.
       */
      virtual void OnSetEntityComponentStateMessage(const sbio::ig::entity::SSetEntityComponentStateMessage& data) override;
      /** @brief Logs an articulated-part transform without applying it.
       * @param data Borrowed part-transform message to log.
       */
      virtual void OnUpdateArticulatedPartTransformMessage(const sbio::ig::entity::SUpdateArticulatedPartTransformMessage& data) override;
      /** @brief Logs articulated-part visibility without applying it.
       * @param data Borrowed part-visibility message to log.
       */
      virtual void OnSetArticulatedPartVisibleMessage(const sbio::ig::entity::SSetArticulatedPartVisibleMessage& data) override;
      /** @brief Logs entity alpha without applying it.
       * @param data Borrowed alpha message to log.
       */
      virtual void OnSetEntityAlphaMessage(const sbio::ig::entity::SSetEntityAlphaMessage& data) override;
      /** @brief Logs entity collision enable state without configuring collision detection.
       * @param data Borrowed collision enable-state message to log.
       */
      virtual void OnSetEntityCollisionDetectionEnabledMessage(const sbio::ig::entity::SSetEntityCollisionDetectionEnabledMessage& data) override;
      /** @brief Logs a basic segment query and schedules a synthetic basic LOS response after a worker delay.
       * @param data Request copied into the detached worker; endpoints do not affect the synthetic result.
       * @see CTestCigiEventHandler for shared response values and dispatcher lifetime behavior.
       */
      virtual void OnLineOfSightSegmentRequestBasicMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestBasicMessage& data) override;
      /**
       * @brief Logs an extended segment query and schedules a synthetic extended LOS response.
       * @param data Copied request; eResponseCoordinateSystem selects entity-relative or geodetic coordinates for entity hits.
       * Non-entity hits always use a geodetic response, even when entity coordinates were requested.
       * @see CTestCigiEventHandler for worker delay, shared response values, and dispatcher lifetime behavior.
       */
      virtual void OnLineOfSightSegmentRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightSegmentRequestExtendedMessage& data) override;
      /** @brief Logs a basic vector query and schedules a synthetic basic LOS response after a worker delay.
       * @param data Request copied into the detached worker; origin, direction, and limits do not affect the synthetic result.
       * @see CTestCigiEventHandler for shared response values and dispatcher lifetime behavior.
       */
      virtual void OnLineOfSightVectorRequestBasicMessage(const sbio::ig::terrain::SLineOfSightVectorRequestBasicMessage& data) override;
      /**
       * @brief Logs an extended vector query and schedules a synthetic extended LOS response.
       * @param data Copied request; eResponseCoordinateSystem selects entity-relative or geodetic coordinates for entity hits.
       * Non-entity hits always use a geodetic response, even when entity coordinates were requested.
       * @see CTestCigiEventHandler for worker delay, shared response values, and dispatcher lifetime behavior.
       */
      virtual void OnLineOfSightVectorRequestExtendedMessage(const sbio::ig::terrain::SLineOfSightVectorRequestExtendedMessage& data) override;
      /**
       * @brief Logs and queues a HAT or extended HAT/HOT response using terrain at zero WGS84-ellipsoid altitude.
       * @param data Borrowed request; point altitude supplies HAT and isExtendedRequest selects the response format.
       *
       * Validity is true only for altitudes at or above zero. Extended replies use material 1 and normal angles
       * of 0 degrees azimuth and 90 degrees elevation. Correlation metadata is copied. No worker is started;
       * nothing is queued if the IG or dispatcher is unavailable.
       */
      virtual void OnHeightAboveTerrainRequestMessage(const sbio::ig::terrain::SHeightAboveTerrainRequestMessage& data) override;
      /**
       * @brief Logs and queues a synthetic terrain-height response using terrain at zero WGS84-ellipsoid altitude.
       * @param data Borrowed request supplying altitude, response-format selection, and correlation metadata.
       *
       * Extended requests always produce a valid HAT/HOT reply with material 1 and normal angles of 0/90 degrees.
       * Basic requests above zero produce a HAT reply; at or below zero they produce a HOT reply. Replies are marked
       * valid. No worker is started; nothing is queued if the IG or dispatcher is unavailable.
       */
      virtual void OnHeightOfTerrainRequestMessage(const sbio::ig::terrain::SHeightOfTerrainRequestMessage& data) override;
      /** @brief Queries conditions at latitude, longitude, and altitude zero and logs each returned surface-condition ID.
       * @pre globals.pImageGenerator and globals.pLogger are available.
       */
      virtual void OnTerrestrialSurfaceConditionsChangedMessage() override;
      /** @brief Logs regional terrain component state without applying it.
       * @param data Borrowed regional component-state message to log.
       */
      virtual void OnSetRegionalTerrainSurfaceComponentStateMessage(const sbio::ig::terrain::SSetRegionalTerrainSurfaceComponentStateMessage& data) override;
      /** @brief Logs global terrain component state without applying it.
       * @param data Borrowed global component-state message to log.
       */
      virtual void OnSetGlobalTerrainComponentStateMessage(const sbio::ig::terrain::SSetGlobalTerrainComponentStateMessage& data) override;
      /**
       * @brief Logs segment enable state and, when enabled, starts a detached synthetic collision worker.
       * @param data Message copied by the worker; entity and segment IDs identify both notifications.
       *
       * After a 100-millisecond delay, attempts to queue a segment notification and a segment-entity notification
       * using the current global dispatcher. Both use material 1 and distance zero; the latter contacts entity 10.
       * Disabled messages only log and do not cancel workers already started.
       */
      virtual void OnSetCollisionDetectionSegmentEnabledMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentEnabledMessage& data) override;
      /** @brief Logs a collision-segment update without applying it.
       * @param data Borrowed segment-update message to log.
       */
      virtual void OnSetCollisionDetectionSegmentMessage(const sbio::ig::physics::SSetCollisionDetectionSegmentMessage& data) override;
      /** @brief Logs a collision-segment creation request without creating a segment.
       * @param data Borrowed segment-creation message to log.
       */
      virtual void OnCreateCollisionDetectionSegmentMessage(const sbio::ig::physics::SCreateCollisionDetectionSegmentMessage& data) override;
      /** @brief Logs a spherical collision-volume creation request without creating a volume.
       * @param data Borrowed sphere-creation message to log.
       */
      virtual void OnCreateCollisionVolumeSphereMessage(const sbio::ig::physics::SCreateCollisionVolumeSphereMessage& data) override;
      /** @brief Logs a cuboid collision-volume creation request without creating a volume.
       * @param data Borrowed cuboid-creation message to log.
       */
      virtual void OnCreateCollisionVolumeCuboidMessage(const sbio::ig::physics::SCreateCollisionVolumeCuboidMessage& data) override;
      /**
       * @brief Logs volume enable state and, when enabled, starts a detached synthetic collision worker.
       * @param data Message copied by the worker; entity and volume IDs identify both notifications.
       *
       * After a 100-millisecond delay, attempts to queue volume and volume-entity notifications through the current
       * global dispatcher. Both use contacted volume 2; no contacted entity is assigned explicitly. Disabled messages
       * only log and do not cancel workers already started.
       */
      virtual void OnSetCollisionVolumeEnabledMessage(const sbio::ig::physics::SSetCollisionVolumeEnabledMessage& data) override;
      /** @brief Logs a collision-volume update without applying it.
       * @param data Borrowed volume-update message to log.
       */
      virtual void OnSetCollisionVolumeMessage(const sbio::ig::physics::SSetCollisionVolumeMessage& data) override;
      /** @brief Logs a collision-volume destruction request without destroying a volume.
       * @param data Borrowed volume-destruction message to log.
       */
      virtual void OnDestroyCollisionVolumeMessage(const sbio::ig::physics::SDestroyCollisionVolumeMessage& data) override;
      /** @brief Logs an attached-camera transform without applying it.
       * @param data Borrowed camera-transform message to log.
       */
      virtual void OnUpdateAttachedCameraTransformMessage(const sbio::ig::view::SUpdateAttachedCameraTransformMessage& data) override;
      /** @brief Logs camera attachment without attaching a camera.
       * @param data Borrowed camera-attachment message to log.
       */
      virtual void OnSetCameraAttachedToEntityMessage(const sbio::ig::view::SSetCameraAttachedToEntityMessage& data) override;
      /** @brief Logs camera detachment without detaching a camera.
       * @param data Borrowed camera-detachment message to log.
       */
      virtual void OnSetCameraUnattachedMessage(const sbio::ig::view::SSetCameraUnattachedMessage& data) override;
      /** @brief Logs a camera stacking request without changing camera order.
       * @param data Borrowed stacking message to log.
       */
      virtual void OnBringCameraToTopMessage(const sbio::ig::view::SBringCameraToTopMessage& data) override;
      /** @brief Logs camera projection settings without applying them.
       * @param data Borrowed projection message to log.
       */
      virtual void OnSetCameraProjectionMessage(const sbio::ig::view::SSetCameraProjectionMessage& data) override;
      /** @brief Logs view component state without applying it.
       * @param data Borrowed view component-state message to log.
       */
      virtual void OnSetViewComponentStateMessage(const sbio::ig::view::SSetViewComponentStateMessage& data) override;
      /** @brief Logs view-group component state without applying it.
       * @param data Borrowed group component-state message to log.
       */
      virtual void OnSetViewGroupComponentStateMessage(const sbio::ig::view::SSetViewGroupComponentStateMessage& data) override;
      /** @brief Logs an unload request without unloading data, clearing completion state, or cancelling workers. */
      virtual void OnUnloadDatabaseMessage() override;
      /**
       * @brief Logs a load request and simulates completion with a detached two-second worker.
       * @param data Borrowed load message, used only for logging; no database content is loaded.
       *
       * Clears the completion flag before starting a worker that sets it to true after the delay. Update() raises
       * the eventual success event. Multiple requests share one flag and may coalesce; there is no request-generation
       * check. The handler must remain alive until all database workers have finished.
       */
      virtual void OnLoadDatabaseMessage(const sbio::ig::database::SLoadDatabaseMessage& data) override;
      /** @brief Logs animation direction without applying it.
       * @param data Borrowed direction message to log.
       */
      virtual void OnSetAnimationDirectionMessage(const sbio::ig::animation::SSetAnimationDirectionMessage& data) override;
      /** @brief Logs animation loop mode without applying it.
       * @param data Borrowed loop-mode message to log.
       */
      virtual void OnSetAnimationLoopModeMessage(const sbio::ig::animation::SSetAnimationLoopModeMessage& data) override;
      /** @brief Logs animation speed without applying it.
       * @param data Borrowed speed message to log.
       */
      virtual void OnSetAnimationSpeedMessage(const sbio::ig::animation::SSetAnimationSpeedMessage& data) override;
      /** @brief Logs animation alpha without applying it.
       * @param data Borrowed alpha message to log.
       */
      virtual void OnSetAnimationAlphaMessage(const sbio::ig::animation::SSetAnimationAlphaMessage& data) override;
      /** @brief Logs an animation stop request without stopping animation.
       * @param data Borrowed stop message to log.
       */
      virtual void OnStopEntityAnimationMessage(const sbio::ig::animation::SStopEntityAnimationMessage& data) override;
      /** @brief Logs a stop-at-current-frame request without changing animation state.
       * @param data Borrowed stop-at-current-frame message to log.
       */
      virtual void OnStopAtCurrentFrameEntityAnimationMessage(const sbio::ig::animation::SStopAtCurrentFrameEntityAnimationMessage& data) override;
      /** @brief Logs an animation pause request without pausing animation.
       * @param data Borrowed pause message to log.
       */
      virtual void OnPauseEntityAnimationMessage(const sbio::ig::animation::SPauseEntityAnimationMessage& data) override;
      /** @brief Logs an animation play request without starting animation.
       * @param data Borrowed play message to log.
       */
      virtual void OnPlayEntityAnimationMessage(const sbio::ig::animation::SPlayEntityAnimationMessage& data) override;
      /** @brief Logs an animation restart request without restarting animation.
       * @param data Borrowed restart message to log.
       */
      virtual void OnRestartEntityAnimationMessage(const sbio::ig::animation::SRestartEntityAnimationMessage& data) override;
      /** @brief Logs text-symbol creation without creating a symbol.
       * @param data Borrowed text-creation message to log.
       */
      virtual void OnCreateSymbolTextMessage(const sbio::ig::symbol::SCreateSymbolTextMessage& data) override;
      /** @brief Logs a text-symbol update without applying it.
       * @param data Borrowed text-update message to log.
       */
      virtual void OnUpdateSymbolTextMessage(const sbio::ig::symbol::SUpdateSymbolTextMessage& data) override;
      /** @brief Logs symbol color without applying it.
       * @param data Borrowed color message to log.
       */
      virtual void OnSetSymbolColorMessage(const sbio::ig::symbol::SSetSymbolColorMessage& data) override;
      /** @brief Logs symbol destruction without destroying a symbol.
       * @param data Borrowed destruction message to log.
       */
      virtual void OnDestroySymbolMessage(const sbio::ig::symbol::SDestroySymbolMessage& data) override;
      /** @brief Logs circle-symbol creation without creating a symbol.
       * @param data Borrowed circle-creation message to log.
       */
      virtual void OnCreateSymbolCircleMessage(const sbio::ig::symbol::SCreateSymbolCircleMessage& data) override;
      /** @brief Logs a circle-symbol update without applying it.
       * @param data Borrowed circle-update message to log.
       */
      virtual void OnUpdateSymbolCircleMessage(const sbio::ig::symbol::SUpdateSymbolCircleMessage& data) override;
      /** @brief Logs a circle-element update without applying it.
       * @param data Borrowed circle-element message to log.
       */
      virtual void OnUpdateSymbolCircleElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleElementMessage& data) override;
      /** @brief Logs a filled-circle update without applying it.
       * @param data Borrowed filled-circle message to log.
       */
      virtual void OnUpdateSymbolCircleFilledMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledMessage& data) override;
      /** @brief Logs a filled-circle element update without applying it.
       * @param data Borrowed filled-circle element message to log.
       */
      virtual void OnUpdateSymbolCircleFilledElementMessage(const sbio::ig::symbol::SUpdateSymbolCircleFilledElementMessage& data) override;
      /** @brief Logs textured-circle creation without creating a symbol.
       * @param data Borrowed textured-circle creation message to log.
       */
      virtual void OnCreateSymbolTexturedCircleMessage(const sbio::ig::symbol::SCreateSymbolTexturedCircleMessage& data) override;
      /** @brief Logs a textured-circle symbol update without applying it.
       * @param data Borrowed textured-circle symbol message to log.
       */
      virtual void OnUpdateSymbolTexturedCircleMessage(const sbio::ig::symbol::SUpdateSymbolTexturedCircleMessage& data) override;
      /** @brief Logs textured-circle geometry without applying it.
       * @param data Borrowed textured-circle geometry message to log.
       */
      virtual void OnUpdateTexturedCircleMessage(const sbio::ig::symbol::SUpdateTexturedCircleMessage& data) override;
      /** @brief Logs polygon-symbol creation without creating a symbol.
       * @param data Borrowed polygon-creation message to log.
       */
      virtual void OnCreateSymbolPolygonMessage(const sbio::ig::symbol::SCreateSymbolPolygonMessage& data) override;
      /** @brief Logs a polygon-symbol update without applying it.
       * @param data Borrowed polygon-update message to log.
       */
      virtual void OnUpdateSymbolPolygonMessage(const sbio::ig::symbol::SUpdateSymbolPolygonMessage& data) override;
      /** @brief Logs a polygon vertex update without applying it.
       * @param data Borrowed vertex-update message to log.
       */
      virtual void OnSetSymbolPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolPolygonVertexMessage& data) override;
      /** @brief Logs textured-polygon creation without creating a symbol.
       * @param data Borrowed textured-polygon creation message to log.
       */
      virtual void OnCreateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SCreateSymbolTexturedPolygonMessage& data) override;
      /** @brief Logs a textured-polygon update without applying it.
       * @param data Borrowed textured-polygon update message to log.
       */
      virtual void OnUpdateSymbolTexturedPolygonMessage(const sbio::ig::symbol::SUpdateSymbolTexturedPolygonMessage& data) override;
      /** @brief Logs a textured-polygon vertex update without applying it.
       * @param data Borrowed textured-vertex message to log.
       */
      virtual void OnSetSymbolTexturedPolygonVertexMessage(const sbio::ig::symbol::SSetSymbolTexturedPolygonVertexMessage& data) override;
      /** @brief Logs an entity billboard symbol-surface update without applying it.
       * @param data Borrowed billboard-update message to log.
       */
      virtual void OnUpdateEntityBillboardSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateEntityBillboardSymbolSurfaceMessage& data) override;
      /** @brief Logs symbol-surface creation without creating a surface.
       * @param data Borrowed surface-creation message to log.
       */
      virtual void OnCreateSymbolSurfaceMessage(const sbio::ig::symbol::SCreateSymbolSurfaceMessage& data) override;
      /** @brief Logs symbol-surface destruction without destroying a surface.
       * @param data Borrowed surface-destruction message to log.
       */
      virtual void OnDestroySymbolSurfaceMessage(const sbio::ig::symbol::SDestroySymbolSurfaceMessage& data) override;
      /** @brief Logs a symbol-surface update without applying it.
       * @param data Borrowed surface-update message to log.
       */
      virtual void OnUpdateSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateSymbolSurfaceMessage& data) override;
      /** @brief Logs a view-relative symbol-surface update without applying it.
       * @param data Borrowed view-surface message to log.
       */
      virtual void OnUpdateViewSymbolSurfaceMessage(const sbio::ig::symbol::SUpdateViewSymbolSurfaceMessage& data) override;
      /** @brief Logs symbol visibility without applying it.
       * @param data Borrowed visibility message to log.
       */
      virtual void OnSetSymbolVisibleMessage(const sbio::ig::symbol::SSetSymbolVisibleMessage& data) override;
      /** @brief Logs symbol attachment without modifying attachments.
       * @param data Borrowed attachment message to log.
       */
      virtual void OnSetSymbolAttachedMessage(const sbio::ig::symbol::SSetSymbolAttachedMessage& data) override;
      /** @brief Logs symbol detachment without modifying attachments.
       * @param data Borrowed detachment message to log.
       */
      virtual void OnSetSymbolUnattachedMessage(const sbio::ig::symbol::SSetSymbolUnattachedMessage& data) override;
      /** @brief Logs symbol surface assignment without applying it.
       * @param data Borrowed surface-assignment message to log.
       */
      virtual void OnSetSymbolSurfaceMessage(const sbio::ig::symbol::SSetSymbolSurfaceMessage& data) override;
      /** @brief Logs a request to clear a symbol's surface without applying it.
       * @param data Borrowed surface-clearing message to log.
       */
      virtual void OnClearSymbolSurfaceMessage(const sbio::ig::symbol::SClearSymbolSurfaceMessage& data) override;
      /** @brief Logs a top-level symbol transform without applying it.
       * @param data Borrowed top-level transform message to log.
       */
      virtual void OnSetTopLevelSymbolTransformMessage(const sbio::ig::symbol::SSetTopLevelSymbolTransformMessage& data) override;
      /** @brief Logs a child-symbol transform without applying it.
       * @param data Borrowed child-transform message to log.
       */
      virtual void OnSetChildSymbolTransformMessage(const sbio::ig::symbol::SSetChildSymbolTransformMessage& data) override;
      /** @brief Logs a symbol update without applying it.
       * @param data Borrowed symbol-update message to log.
       */
      virtual void OnUpdateSymbolMessage(const sbio::ig::symbol::SUpdateSymbolMessage& data) override;
      /** @brief Logs symbol component state without applying it.
       * @param data Borrowed symbol component-state message to log.
       */
      virtual void OnSetSymbolComponentStateMessage(const sbio::ig::symbol::SSetSymbolComponentStateMessage& data) override;
      /** @brief Logs symbol-surface component state without applying it.
       * @param data Borrowed surface component-state message to log.
       */
      virtual void OnSetSymbolSurfaceComponentStateMessage(const sbio::ig::symbol::SSetSymbolSurfaceComponentStateMessage& data) override;
      /** @brief Logs template-based symbol creation without resolving a template or creating a symbol.
       * @param data Borrowed template-creation message to log.
       */
      virtual void OnCreateSymbolFromTemplateMessage(const sbio::ig::symbol::SCreateSymbolFromTemplateMessage& data) override;

      /** @brief Logs celestial-sphere settings without applying them.
       * @param data Borrowed celestial-sphere message to log.
       */
      virtual void OnUpdateCelestialSphereMessage(const sbio::ig::celestial::SUpdateCelestialSphereMessage& data) override;

      /** @brief Logs date/time settings without changing simulation time.
       * @param data Borrowed date/time message to log.
       */
      virtual void OnUpdateDateTimeMessage(const sbio::ig::celestial::SUpdateDateTimeMessage& data) override;

      /** @brief Logs celestial-sphere component state without applying it.
       * @param data Borrowed celestial component-state message to log.
       */
      virtual void OnSetCelestialSphereComponentStateMessage(const sbio::ig::celestial::SSetCelestialSphereComponentStateMessage& data) override;

      /** @brief Logs sensor settings without applying them.
       * @param data Borrowed sensor-update message to log.
       */
      virtual void OnUpdateSensorMessage(const sbio::ig::sensor::SUpdateSensorMessage& data) override;

      /** @brief Logs sensor component state without applying it.
       * @param data Borrowed sensor component-state message to log.
       */
      virtual void OnUpdateSensorComponentMessage(const sbio::ig::sensor::SUpdateSensorComponentMessage& data) override;

      /** @brief Logs a view motion-tracker creation request without creating a tracker.
       * @param data Borrowed view tracker-creation message to log.
       */
      virtual void OnCreateMotionTrackerViewMessage(const sbio::ig::sensor::SCreateMotionTrackerViewMessage& data) override;

      /** @brief Logs a view-group motion-tracker creation request without creating a tracker.
       * @param data Borrowed group tracker-creation message to log.
       */
      virtual void OnCreateMotionTrackerViewGroupMessage(const sbio::ig::sensor::SCreateMotionTrackerViewGroupMessage& data) override;

      /** @brief Logs motion-tracker settings without applying them.
       * @param data Borrowed tracker-settings message to log.
       */
      virtual void OnSetMotionTrackerMessage(const sbio::ig::sensor::SSetMotionTrackerMessage& data) override;

      /** @brief Logs atmosphere enable state without applying it.
       * @param data Borrowed atmosphere enable-state message to log.
       */
      virtual void OnSetAtmosphereEnabledMessage(const sbio::ig::atmosphere::SSetAtmosphereEnabledMessage& data) override;

      /** @brief Logs atmosphere settings without applying them.
       * @param data Borrowed atmosphere-settings message to log.
       */
      virtual void OnSetAtmosphereMessage(const sbio::ig::atmosphere::SSetAtmosphereMessage& data) override;

      /** @brief Logs weather settings without applying them.
       * @param data Borrowed weather message to log.
       */
      virtual void OnSetWeatherMessage(const sbio::ig::atmosphere::SSetWeatherMessage& data) override;

      /** @brief Logs regional layered-weather component state without applying it.
       * @param data Borrowed regional weather component-state message to log.
       */
      virtual void OnSetRegionalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetRegionalLayeredWeatherComponentStateMessage& data) override;

      /** @brief Logs global layered-weather component state without applying it.
       * @param data Borrowed global weather component-state message to log.
       */
      virtual void OnSetGlobalLayeredWeatherComponentStateMessage(const sbio::ig::atmosphere::SSetGlobalLayeredWeatherComponentStateMessage& data) override;

      /** @brief Logs atmosphere component state without applying it.
       * @param data Borrowed atmosphere component-state message to log.
       */
      virtual void OnSetAtmosphereComponentStateMessage(const sbio::ig::atmosphere::SSetAtmosphereComponentStateMessage& data) override;

      /** @brief Logs maritime surface conditions without applying them.
       * @param data Borrowed maritime-conditions message to log.
       */
      virtual void OnSetMaritimeSurfaceConditionsMessage(const sbio::ig::ocean::SSetMaritimeSurfaceConditionsMessage& data) override;

      /** @brief Logs regional maritime component state without applying it.
       * @param data Borrowed regional maritime component-state message to log.
       */
      virtual void OnSetRegionMaritimeComponentStateMessage(const sbio::ig::ocean::SSetRegionMaritimeComponentStateMessage& data) override;

      /** @brief Logs global maritime component state without applying it.
       * @param data Borrowed global maritime component-state message to log.
       */
      virtual void OnSetGlobalMaritimeComponentStateMessage(const sbio::ig::ocean::SSetGlobalMaritimeComponentStateMessage& data) override;

      /** @brief Logs the earth-reference-model selection without applying it.
       * @param data Borrowed reference-model message to log.
       */
      virtual void OnSetEarthReferenceModelMessage(const sbio::ig::earth::SSetEarthReferenceModelMessage& data) override;

      /** @brief Logs event component state without applying it.
       * @param data Borrowed event component-state message to log.
       */
      virtual void OnSetEventComponentStateMessage(const sbio::ig::system::SSetEventComponentStateMessage& data) override;

      /** @brief Logs system component state without applying it.
       * @param data Borrowed system component-state message to log.
       */
      virtual void OnSetSystemComponentStateMessage(const sbio::ig::system::SSetSystemComponentStateMessage& data) override;

      /** @brief Logs the address of a connected host without managing the connection.
       * @param data Borrowed connection notification supplying sHostIP.
       */
      virtual void OnSetHostConnectedMessage(const sbio::ig::network::SHostConnectedMessage& data) override;

      /** @brief Logs the address of a disconnected host without managing the connection.
       * @param data Borrowed disconnection notification supplying sHostIP.
       */
      virtual void OnSetHostDisconnectedMessage(const sbio::ig::network::SHostDisconnectedMessage& data) override;

      /**
       * @brief Polls simulated database completion and raises a successful IGCIGIEvent when the flag is set.
       *
       * Clears the flag before raising SDatabaseLoadedEventArgs with bLoadSuccessful true. Does nothing if no
       * completion is pending. Call on the IG update thread; this method does not wait for workers to finish.
       */
      void Update();

    private:
      std::atomic<bool> m_bLoadDatabaseComplete = false;///< Shared completion signal set by simulated load workers and consumed by Update().
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
