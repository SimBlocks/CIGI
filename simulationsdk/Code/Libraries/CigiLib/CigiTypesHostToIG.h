//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiTypesHostToIG.h
 * @brief Declares host-to-IG control, definition, and request structures for the CIGI API.
 *
 * The types in this header are passive data carriers used to represent packet payloads received from a CIGI host.
 * They own only their contained values, perform no I/O on their own, and generally do not enforce semantic validation
 * beyond the strong types used by individual members.
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TYPES_HOST_TO_IG_H
#define SIMBLOCKS_CIGI_TYPES_HOST_TO_IG_H

#include "CigiLib/CigiTypes.h"
#include "EntityLib/EntityTypes.h"
#include "ViewLib/ViewTypes.h"

namespace sbio
{
  namespace cigi
  {
    /**
     * @brief Stores the current IG control values supplied by the host.
     *
     * Invariants:
     * - `databaseNumber` is stored exactly as received and may use the CIGI loading conventions for negative values.
     * - `hostFrameNumber` and `lastIgFrameNumber` are host-supplied frame identifiers.
     */
    struct SCigiIgControl
    {
      CigiDatabaseNumber databaseNumber = UnknownCigiDatabaseNumber;
      bool bEntityTypeSubstitutionEnabled = false;
      EIGMode eIgMode = EIGMode::UNKNOWN;
      bool bTimestampValid = false;
      bool bSmoothingEnabled = false;
      FrameNumber hostFrameNumber = UnknownFrameNumber;
      FrameNumber lastIgFrameNumber = UnknownFrameNumber;
      uint32_t timestamp = 0;
    };

    /**
     * @brief Base data shared by all entity position requests.
     *
     * Invariants:
     * - `entityID` identifies the entity being updated.
     * - `bAttached` reflects only the requested attach state; no parent linkage is implied by this base type.
     */
    struct SEntityPosition
    {
      EntityID entityID = UnknownEntityID;
      bool bAttached = false;
    };

    /**
     * @brief Represents a top-level entity position update in geodetic space.
     *
     * Invariants:
     * - `geodeticCoordinates` and `rotation` describe the requested world-space pose.
     * - `eClamp` records the requested clamp mode exactly as parsed.
     */
    struct STopLevelEntityPosition : SEntityPosition
    {
      sbio::EClamp eClamp = EClamp::UNKNOWN;
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
      sbio::cigi::TCigiNEDEulerRotation rotation;
    };

    /**
     * @brief Represents a child-entity position update relative to a parent entity.
     *
     * Invariants:
     * - `parentID` identifies the requested parent entity.
     * - `offset` and `rotation` are expressed in the CIGI body frame expected for child placement.
     */
    struct SChildEntityPosition : SEntityPosition
    {
      EntityID parentID = UnknownEntityID;
      sbio::math::Vec3 offset;
      sbio::cigi::TCigiBodyEulerRotation rotation;
    };

    /**
     * @brief Stores a conformal clamped entity position request.
     *
     * Invariants:
     * - The type carries only latitude, longitude, and yaw because altitude is derived by the conformal clamp operation.
     * - Members are stored exactly as assigned.
     */
    struct SCigiConformalClampedEntityPosition
    {
      EntityID entityID = UnknownEntityID;
      sbio::math::Degrees fYaw = UnknownDegrees;
      sbio::math::Latitude latitude = sbio::math::UnknownLatitude;
      sbio::math::Longitude longitude = sbio::math::UnknownLongitude;
    };

    /**
     * @brief Identifies one component instance within a CIGI entity.
     *
     * Invariants:
     * - The tuple `{ componentID, componentClassID, nInstanceID }` forms the complete comparison key.
     */
    struct SCigiComponentKey
    {
      CigiComponentID componentID = UnknownCigiComponentID;
      CigiComponentClassID componentClassID = UnknownCigiComponentClassID;
      uint16_t nInstanceID = 0;

      /**
       * @brief Tests whether two component keys identify the same component instance.
       * @param key Key to compare against.
       * @return `true` when all key fields are equal; otherwise `false`.
       *
       * Side effects: None.
       */
      bool operator==(const SCigiComponentKey& key) const;

      /**
       * @brief Orders component keys for associative containers.
       * @param key Key to compare against.
       * @return `true` when this key sorts before `key`; otherwise `false`.
       *
       * Side effects: None.
       */
      bool operator<(const SCigiComponentKey& key) const;
    };

    /**
     * @brief Stores the state data for a component control request.
     *
     * Invariants:
     * - `componentData` contains exactly six 32-bit words because that is the fixed component-data shape represented by this API.
     * - No interpretation of `nComponentState` or `componentData` is enforced by this type.
     */
    struct SCigiComponentControlState
    {
      uint8_t nComponentState = 0;
      uint32_t componentData[6] = {0, 0, 0, 0, 0, 0};

      /**
       * @brief Tests whether two component control states contain the same component data.
       * @param state State to compare against.
       * @return `true` when `nComponentState` and all six component data words match; otherwise `false`.
       *
       * Side effects: None.
       */
      bool operator==(const SCigiComponentControlState& state) const;
    };

    /**
     * @brief Stores a full component control request keyed by component identity.
     *
     * Invariants:
     * - `key` identifies the target component.
     * - `state` carries the requested component state data.
     */
    struct SCigiComponentControl
    {
      SCigiComponentKey key;
      SCigiComponentControlState state;
    };

    /**
     * @brief Stores the compact short-component control form.
     *
     * Invariants:
     * - This type exposes only the two component data words carried by the short packet variant.
     * - Members are stored exactly as received.
     */
    struct SCigiShortComponentControl
    {
      CigiComponentID componentID = UnknownCigiComponentID;
      CigiComponentClassID componentClassID = UnknownCigiComponentClassID;
      uint8_t nComponentState = 0;
      uint16_t nInstanceID = 0;
      uint32_t componentData1 = 0;
      uint32_t componentData2 = 0;
    };

    /**
     * @brief Stores a full articulated-part control update.
     *
     * Invariants:
     * - `offset` and `rotation` are interpreted only for the enabled degrees of freedom.
     * - `entityID` and `articulatedPartID` together identify the target articulated part.
     */
    struct SCigiArticulatedPart
    {
      EntityID entityID = UnknownEntityID;
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;
      bool bEnabled = false;
      bool bOffsetEnabled[3] = {false, false, false};
      bool bRollEnabled = false;
      bool bPitchEnabled = false;
      bool bYawEnabled = false;
      sbio::cigi::CigiBodyCoordinates offset;
      sbio::cigi::TCigiBodyEulerRotation rotation;
    };

    /**
     * @brief Stores the compact two-DOF short articulated-part control form.
     *
     * Invariants:
     * - Each `eDOF` member describes the semantic meaning of the paired `fDOF` value.
     * - The type carries at most two articulated part identifiers because that is the short packet format.
     */
    struct SCigiShortArticulatedPart
    {
      EntityID entityID = UnknownEntityID;
      ArticulatedPartID articulatedPartID1 = UnknownArticulatedPartID;
      ArticulatedPartID articulatedPartID2 = UnknownArticulatedPartID;
      EDegreeOfFreedom eDOF1 = EDegreeOfFreedom::UNKNOWN;
      EDegreeOfFreedom eDOF2 = EDegreeOfFreedom::UNKNOWN;
      bool bArticulatedPart1Enabled = false;
      bool bArticulatedPart2Enabled = false;
      float fDOF1 = 0;
      float fDOF2 = 0;
    };

    /**
     * @brief Stores linear and angular velocity for an entity.
     *
     * Invariants:
     * - `coordinateSystem` identifies the frame used to interpret the velocity vectors.
     * - Members are stored exactly as assigned.
     */
    struct SCigiEntityVelocityControl
    {
      EntityID entityID = UnknownEntityID;
      EObjectCoordinateSystem coordinateSystem = EObjectCoordinateSystem::UNKNOWN;
      sbio::cigi::CigiBodyCoordinates linearVelocity;
      sbio::cigi::TCigiBodyEulerRotation angularVelocity;
    };

    /**
     * @brief Stores linear and angular velocity for an articulated part.
     *
     * Invariants:
     * - `entityID` and `articulatedPartID` identify the target articulated part.
     * - Members are stored exactly as assigned.
     */
    struct SCigiArticulatedPartVelocityControl
    {
      EntityID entityID = UnknownEntityID;
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;
      sbio::cigi::CigiBodyCoordinates linearVelocity;
      sbio::cigi::TCigiBodyEulerRotation angularVelocity;
    };

    /**
     * @brief Stores host-requested celestial sphere settings.
     *
     * Invariants:
     * - Date and time fields are meaningful only when `bDateTimeValid` is `true`.
     * - `starFieldIntensity` uses the SDK percentage type and is stored exactly as assigned.
     */
    struct SCigiCelestialSphereControl
    {
      bool bContinuousTimeOfDayEnable = false;
      bool bSunEnable = false;
      bool bMoonEnable = false;
      bool bStarFieldEnable = false;
      bool bDateTimeValid = false;
      Hour hour = UnknownHour;
      Minute minute = UnknownMinute;
      Second second = UnknownSecond;
      Year year = UnknownYear;
      Month month = UnknownMonth;
      Day day = UnknownDay;
      Percentage starFieldIntensity = UnknownPercentage;
    };

    /**
     * @brief Stores host-requested global atmosphere settings.
     *
     * Invariants:
     * - Members are stored exactly as assigned.
     * - This type does not compute derived weather state.
     */
    struct SCigiAtmosphereControl
    {
      bool bAtmosphereModelEnable = false;
      Percentage globalHumidity = UnknownPercentage;
      float fGlobalAirTemp = 0;
      float fGlobalVisibility = 0;
      float fGlobalHorizontalWindSpeed = 0;
      float fGlobalVerticalWindSpeed = 0;
      Degrees globalWindDirection = UnknownDegrees;
      float fGlobalBarometricPressure = 0;
    };

    /**
     * @brief Defines one environmental region and its merge behavior.
     *
     * Invariants:
     * - `regionID` identifies the region.
     * - Merge fields record the requested per-domain merge behavior exactly as parsed.
     * - Geometry members describe the region footprint and transition without additional validation.
     */
    struct SCigiEnvironmentalRegion
    {
      EActiveState eRegionState = EActiveState::UNKNOWN;
      EMergeState eMergeWeatherProperties = EMergeState::UNKNOWN;
      EMergeState eMergeAerosolConcentrations = EMergeState::UNKNOWN;
      EMergeState eMergeMaritimeSurfaceConditions = EMergeState::UNKNOWN;
      EMergeState eMergeTerrestrialSurfaceConditions = EMergeState::UNKNOWN;
      RegionID regionID = UnknownRegionID;
      sbio::math::Latitude latitude = sbio::math::UnknownLatitude;
      sbio::math::Longitude longitude = sbio::math::UnknownLongitude;
      sbio::math::Vec2f size;
      float fCornerRadius = 0;
      Degrees180 fRotation = UnknownDegrees180;
      float fTransition = 0;
    };

    /**
     * @brief Stores one set of weather properties for composition or application.
     *
     * Invariants:
     * - Members are stored exactly as assigned.
     * - No normalization is performed across enable flags and numeric fields.
     *
     * Side effects: `Sum()` returns a composed value and `Scale()` returns a scaled copy; neither mutates the source operands.
     */
    struct SCigiWeatherCondition
    {
      Percentage humidity = UnknownPercentage;
      bool bWeatherEnabled = false;
      bool bBottomScudEnabled = false;
      bool bRandomWindsEnabled = false;
      bool bRandomLightningEnabled = false;
      CloudType cloudType = UnknownCloudType;
      sbio::WeatherSeverity severity = sbio::UnknownWeatherSeverity;
      bool bTopScudEnabled = false;
      sbio::TemperatureCelsius fAirTemperature = sbio::UnknownTemperatureCelsius;
      float fVisibilityRange = 0;
      Percentage bottomScudFrequency = UnknownPercentage;
      Percentage coverage = UnknownPercentage;
      float HorizontalWindSpeed = 0;
      float VerticalWindSpeed = 0;
      float WindDirection = 0;
      float fBarometricPressure = 0;
      float fAerosolConcentration = 0;
      Percentage topScudFrequency = UnknownPercentage;

      /**
       * @brief Produces the additive combination of two weather condition values.
       * @param a First operand.
       * @param b Second operand.
       * @return A new value containing the implementation-defined sum of `a` and `b`.
       *
       * Side effects: None.
       */
      SCigiWeatherCondition static Sum(const SCigiWeatherCondition& a, const SCigiWeatherCondition& b);

      /**
       * @brief Returns a copy of this weather condition scaled by `scale`.
       * @param scale Scalar applied to the floating-point and percentage members handled by the implementation.
       * @return A scaled copy of this instance.
       *
       * Side effects: None on the source instance.
       */
      SCigiWeatherCondition Scale(float scale);
    };

    /**
     * @brief Stores vertical bounds for non-entity spatial weather layers.
     *
     * Invariants:
     * - This type applies only to regional or global weather representations, as noted by the surrounding API.
     * - Members are stored exactly as assigned.
     */
    struct SCigiSpatialWeatherCondition
    {
      float fBaseElevation = 0;
      float fThickness = 0;
      float fBottomTransitionBandThickness = 0;
      float fTopTransitionBandThickness = 0;
    };

    /**
     * @brief Stores maritime surface properties for one scope.
     *
     * Invariants:
     * - `scope` identifies whether the condition applies globally, regionally, or to an entity.
     * - Members are stored exactly as assigned.
     *
     * Side effects: `Sum()` and `Scale()` return derived values without mutating the source operands.
     */
    struct SCigiMaritimeSurfaceCondition
    {
      bool bActive = false;
      bool bWhitecapEnabled = false;
      sbio::math::HeightRelativeToWGS84Ellipsoid fSeaSurfaceHeight = sbio::math::UnknownHeightRelativeToWGS84Ellipsoid;
      sbio::TemperatureCelsius fSurfaceWaterTemperature = sbio::UnknownTemperatureCelsius;
      Percentage surfaceClarity = UnknownPercentage;

      /**
       * @brief Produces the additive combination of two maritime surface condition values.
       * @param a First operand.
       * @param b Second operand.
       * @return A new value containing the implementation-defined sum of `a` and `b`.
       */
      SCigiMaritimeSurfaceCondition static Sum(const SCigiMaritimeSurfaceCondition& a, const SCigiMaritimeSurfaceCondition& b);

      /**
       * @brief Returns a copy of this maritime surface condition scaled by `scale`.
       * @param scale Scalar applied to the members handled by the implementation.
       * @return A scaled copy of this instance.
       *
       * Side effects: None on the source instance.
       */
      SCigiMaritimeSurfaceCondition Scale(float scale);
    };

    /**
     * @brief Stores wave properties for a single wave condition.
     *
     * Invariants:
     * - `waveID` is a unique identifier for this wave condition instance.
     * - Members are stored exactly as received.
     */
    struct SCigiWaveCondition
    {
      uint8_t waveID = 0;
      bool bWaveEnabled = false;
      EWaveBreakerType eBreakerType = EWaveBreakerType::UNKNOWN;
      float fWaveHeight = 0;
      float fWavelength = 0;
      float fPeriod = 0;
      Degrees360 direction = UnknownDegrees360;
      Degrees360 phaseOffset = UnknownDegrees360;
      Degrees180 leading = UnknownDegrees180;
    };

    // Terrestrial Surface Conditions Control
    RANGED_STRONG_INT(CigiTerrestrialSurfaceSeverity, uint8_t, 0, 31);

    /**
     * @brief Stores one terrestrial surface condition request or state description.
     *
     * Invariants:
     * - `surfaceConditionID` identifies the requested or active terrain-surface condition.
     * - `scope` determines how the condition is applied.
     *
     * Side effects: `IsDry()` reads the current instance only.
     */
    struct SCigiTerrestrialSurfaceCondition
    {
      bool bEnabled = false;
      Percentage severity = UnknownPercentage;
      Percentage coverage = UnknownPercentage;
      SurfaceConditionID surfaceConditionID = UnknownSurfaceConditionID;

      /**
       * @brief Checks whether this condition represents the dry surface condition.
       * @return `true` when the implementation considers `surfaceConditionID` dry; otherwise `false`.
       *
       * Side effects: None.
       */
      bool IsDry() const;// Return true if SurfaceCondition is 0
    };

    /**
     * @brief Stores view-attachment control values for one view or view group.
     *
     * Invariants:
     * - `offset` and `rotation` are interpreted only for the enabled components.
     * - `viewID`, `viewGroupID`, and `groupID` are preserved exactly as supplied.
     */
    struct SCigiViewControl
    {
      uint8_t groupID = 0;
      ViewGroupID viewGroupID = UnknownViewGroupID;
      bool offsetEnabled[3] = {false, false, false};
      bool bYawEnabled = false;
      bool bPitchEnabled = false;
      bool bRollEnabled = false;
      ViewID viewID = UnknownViewID;
      EntityID entityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates offset;
      sbio::cigi::TCigiBodyEulerRotation rotation;
    };

    /**
     * @brief Stores sensor control values for one sensor.
     *
     * Invariants:
     * - `sensorID` identifies the controlled sensor.
     * - Gain, level, and noise are constrained by their strong types; all other fields are stored exactly as assigned.
     */
    struct SCigiSensorControl
    {
      SensorID sensorID = UnknownSensorID;
      ETrackMode eTrackMode = ETrackMode::UNKNOWN;
      bool bSensorOn = false;
      EPolarity ePolarity = EPolarity::UNKNOWN;
      bool bLineByLineDropoutEnabled = false;
      bool bAutomaticGain = false;
      ESensorTrack eSensorTrack = ESensorTrack::UNKNOWN;
      bool bExtendedResponse = false;
      ViewID viewID = UnknownViewID;
      SensorGain gain = SensorGain(0);
      SensorLevel level = SensorLevel(0);
      float fACCoupling = 0;
      SensorNoise noise = SensorNoise(0);
    };

    /**
     * @brief Stores the enabled degrees of freedom for one motion tracker.
     *
     * Invariants:
     * - Boolean members indicate which tracker axes are enabled.
     * - `motionTrackerID` identifies the target tracker.
     */
    struct SMotionTrackerControl
    {
      MotionTrackerID motionTrackerID = UnknownMotionTrackerID;
      bool bEnable = false;
      bool bBoresightEnable = false;
      bool bXEnable = false;
      bool bYEnable = false;
      bool bZEnable = false;
      bool bRollEnable = false;
      bool bPitchEnable = false;
      bool bYawEnable = false;
    };

    /**
     * @brief Associates a motion tracker control request with a specific view.
     *
     * Invariants:
     * - Inherits the tracker-axis enable flags from `SMotionTrackerControl`.
     * - `viewID` identifies the target view for the tracker association.
     */
    struct SMotionTrackerViewControl : SMotionTrackerControl
    {
      ViewID viewID = UnknownViewID;
    };

    /**
     * @brief Associates a motion tracker control request with a specific view group.
     *
     * Invariants:
     * - Inherits the tracker-axis enable flags from `SMotionTrackerControl`.
     * - `viewGroupID` identifies the target view group for the tracker association.
     */
    struct SMotionTrackerViewGroupControl : SMotionTrackerControl
    {
      ViewGroupID viewGroupID = UnknownViewGroupID;
    };

    /**
     * @brief Stores an earth-reference-model definition supplied by the host.
     *
     * Invariants:
     * - `eEarthReferenceModel` selects the model identity.
     * - Radius and flattening are stored exactly as assigned and are meaningful only for host-defined models.
     */
    struct SCigiEarthReferenceModel
    {
      EEarthReferenceModel eEarthReferenceModel = EEarthReferenceModel::UNKNOWN;
      double fEquatorialRadius = 0;
      double fFlattening = 0;
    };

    /**
     * @brief Stores linear and angular acceleration for an entity.
     *
     * Invariants:
     * - `eCoordinateSystem` identifies the frame used to interpret the accelerations.
     * - Members are stored exactly as assigned.
     */
    struct SCigiEntityAcceleration
    {
      EntityID entityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates linearAcceleration;
      sbio::EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;
      sbio::cigi::TCigiBodyEulerRotation angularAcceleration;
    };

    /**
     * @brief Stores linear and angular acceleration for an articulated part.
     *
     * Invariants:
     * - `entityID` and `articulatedPartID` identify the target articulated part.
     * - Members are stored exactly as assigned.
     */
    struct SCigiArticulatedPartAcceleration
    {
      EntityID entityID = UnknownEntityID;
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;
      sbio::EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;
      sbio::cigi::CigiBodyCoordinates linearAcceleration;
      sbio::cigi::TCigiBodyEulerRotation angularAcceleration;
    };

    STRONG_TYPE(PixelReplicationMode, uint8_t)

    /**
     * @brief Stores the static definition for one view.
     *
     * Invariants:
     * - Frustum members are meaningful only for the corresponding enabled flags.
     * - `viewID` identifies the defined view and `viewGroupID` records its group membership.
     */
    struct SCigiViewDefinition
    {
      ViewID viewID = UnknownViewID;
      ViewGroupID viewGroupID = UnknownViewGroupID;
      bool bNearEnabled = false;
      bool bFarEnabled = false;
      bool bLeftEnabled = false;
      bool bRightEnabled = false;
      bool bTopEnabled = false;
      bool bBottomEnabled = false;
      EMirrorMode eMirrorMode = EMirrorMode::UNKNOWN;
      PixelReplicationMode pixelReplicationMode = UnknownPixelReplicationMode;
      EProjectionMode eProjectionMode = EProjectionMode::UNKNOWN;
      bool bReorder = false;
      ViewType viewType = UnknownViewType;
      float fNear = 0;
      float fFar = 0;
      float fLeft = 0;
      float fRight = 0;
      float fTop = 0;
      float fBottom = 0;
    };

    /**
     * @brief Defines a collision-detection segment attached to an entity.
     *
     * Invariants:
     * - `segmentID` identifies the segment for `entityID`.
     * - `beg` and `end` are expressed in CIGI body coordinates.
     */
    struct SCollisionDetectionSegmentDefinition
    {
      SegmentID segmentID = UnknownSegmentID;
      bool bSegmentEnabled = false;
      EntityID entityID = UnknownEntityID;
      CigiBodyCoordinates beg;
      CigiBodyCoordinates end;
      uint32_t nMaterialMask = 0;
    };

    /**
     * @brief Base data shared by collision-detection volume definitions.
     *
     * Invariants:
     * - `volumeID` identifies the volume for `entityID`.
     * - `offset` is expressed in CIGI body coordinates.
     */
    struct SCollisionDetectionVolumeDefinition
    {
      VolumeID volumeID = UnknownVolumeID;
      bool bVolumeEnabled = false;
      EntityID entityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates offset;
    };

    /**
     * @brief Defines a cuboid collision-detection volume.
     *
     * Invariants:
     * - Dimensions and orientation are stored exactly as assigned.
     * - This type adds cuboid-specific geometry to `SCollisionDetectionVolumeDefinition`.
     */
    struct SCollisionDetectionCuboidDefinition : SCollisionDetectionVolumeDefinition
    {
      float fHeight = 0;
      float fWidth = 0;
      float fDepth = 0;
      TCigiBodyEulerRotation rotation;
    };

    /**
     * @brief Defines a spherical collision-detection volume.
     *
     * Invariants:
     * - `fRadius` is stored exactly as assigned.
     * - This type adds sphere-specific geometry to `SCollisionDetectionVolumeDefinition`.
     */
    struct SCollisionDetectionSphereDefinition : SCollisionDetectionVolumeDefinition
    {
      float fRadius = 0;
    };

    STRONG_TYPE(UpdatePeriod, uint8_t)

    /**
     * @brief Base data shared by all HAT/HOT request variants.
     *
     * Invariants:
     * - `requestID` identifies the request instance.
     * - `lastHostFrameNumber` preserves the host frame associated with the request.
     */
    struct SBaseHATHOTRequest
    {
      HATHOTID requestID = UnknownHATHOTID;
      UpdatePeriod updatePeriod = UnknownUpdatePeriod;
      FrameNumber lastHostFrameNumber = UnknownFrameNumber;
      ERequestType eRequestType = ERequestType::UNKNOWN;
    };

    /**
     * @brief Requests height above terrain for a global geodetic point.
     *
     * Invariants:
     * - `geodeticCoordinates` identifies the world-space point to evaluate.
     */
    struct SHATHOTGlobalRequest : SBaseHATHOTRequest
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
    };

    /**
     * @brief Requests height above terrain for an entity-relative point.
     *
     * Invariants:
     * - `entityID` identifies the entity whose frame defines `offset`.
     */
    struct SHATHOTEntityRequest : SBaseHATHOTRequest
    {
      EntityID entityID = UnknownEntityID;
      sbio::math::Vec3 offset;
    };

    /**
     * @brief Base data shared by all line-of-sight request variants.
     *
     * Invariants:
     * - `requestID` identifies the request instance.
     * - `eType` records whether the request is a segment or vector query.
     * - `lastHostFrameNumber` preserves the host frame associated with the request.
     */
    struct SLineOfSightRequest
    {
      LineOfSightRequestID requestID = UnknownLineOfSightRequestID;
      UpdatePeriod updatePeriod = UnknownUpdatePeriod;
      uint8_t nAlphaThreshold = 0;
      uint32_t nMaterialMask = 0;
      FrameNumber lastHostFrameNumber = UnknownFrameNumber;
    };

    /**
     * @brief Line-of-sight segment request from a geodetic source to a geodetic destination.
     */
    struct SLineOfSightSegmentRequestGeodeticToGeodeticBasic : SLineOfSightRequest
    {
      sbio::math::SGeodeticCoordinates sourceGeodeticCoordinates;
      sbio::math::SGeodeticCoordinates destinationGeodeticCoordinates;
    };

    /**
     * @brief Stores line of sight segment request geodetic to geodetic extended data.
     */
    struct SLineOfSightSegmentRequestGeodeticToGeodeticExtended : SLineOfSightSegmentRequestGeodeticToGeodeticBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;
    };

    /**
     * @brief Line-of-sight segment request from a geodetic source to an entity-relative destination.
     *
     */
    struct SLineOfSightSegmentRequestGeodeticToEntityBasic : SLineOfSightRequest
    {
      sbio::math::SGeodeticCoordinates sourceGeodeticCoordinates;
      EntityID destinationEntityID = UnknownEntityID;
      sbio::math::Vec3 destinationOffset;
    };

    /**
     * @brief Stores line of sight segment request geodetic to entity extended data.
     */
    struct SLineOfSightSegmentRequestGeodeticToEntityExtended : SLineOfSightSegmentRequestGeodeticToEntityBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;
    };

    /**
     * @brief Line-of-sight segment request from an entity-relative source to a geodetic destination.
     */
    struct SLineOfSightSegmentRequestEntityToGeodeticBasic : SLineOfSightRequest
    {
      EntityID sourceEntityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates sourceOffset;
      sbio::math::SGeodeticCoordinates destinationGeodeticCoordinates;
    };

    /**
     * @brief Stores line of sight segment request entity to geodetic extended data.
     */
    struct SLineOfSightSegmentRequestEntityToGeodeticExtended : SLineOfSightSegmentRequestEntityToGeodeticBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;
    };

    /**
     * @brief Line-of-sight segment request from one source entity-relative point to a point on the destination entity.
     * The destination entity may be the same as the source entity.
     *
     */
    struct SLineOfSightSegmentRequestEntityToEntityBasic : SLineOfSightRequest
    {
      EntityID sourceEntityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates sourceOffset;
      EntityID destinationEntityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates destinationOffset;
    };

    /**
     * @brief Stores line of sight segment request entity to entity extended data.
     */
    struct SLineOfSightSegmentRequestEntityToEntityExtended : SLineOfSightSegmentRequestEntityToEntityBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;
    };

    // Line of Sight Vector Request
    /**
     * @brief Base data for line-of-sight vector requests.
     *
     * Invariants:
     * - `eType` is initialized to `ELineOfSightRequestType::LINE_OF_SIGHT_VECTOR_RESPONSE`.
     * - `azimuth`, `elevation`, and range values describe the vector query exactly as supplied.
     */
    struct SLineOfSightVectorRequest : SLineOfSightRequest
    {
      Degrees180 azimuth = UnknownDegrees180;
      Degrees90 elevation = UnknownDegrees90;
      float fMinimumRange = 0;
      float fMaximumRange = 0;
    };

    /**
     * @brief Line-of-sight vector request originating from a geodetic point.
     */
    struct SLineOfSightVectorRequestGeodeticBasic : SLineOfSightVectorRequest
    {
      sbio::math::SGeodeticCoordinates sourceGeodeticCoordinates;
    };

    /**
     * @brief Stores line of sight vector request geodetic extended data.
     */
    struct SLineOfSightVectorRequestGeodeticExtended : SLineOfSightVectorRequestGeodeticBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;
    };

    /**
     * @brief Line-of-sight vector request originating from an entity-relative point.
     */
    struct SLineOfSightVectorRequestEntityBasic : SLineOfSightVectorRequest
    {
      EntityID sourceEntityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates sourceOffset;
    };

    /**
     * @brief Stores line of sight vector request entity extended data.
     */
    struct SLineOfSightVectorRequestEntityExtended : SLineOfSightVectorRequestEntityBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;
    };

    /**
     * @brief Stores one position request for an entity, articulated part, view, or tracker object.
     *
     * Invariants:
     * - `nObjectID` and `eObjectClass` together identify the requested object.
     * - `articulatedPartID` is meaningful when the object class requires it.
     */
    struct SPositionRequest
    {
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;
      bool bContinuous = false;
      EObjectClass eObjectClass = EObjectClass::UNKNOWN;
      EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;
      uint16_t nObjectID = 0;
    };

    /**
     * @brief Stores one environmental conditions query for a geodetic location.
     *
     * Invariants:
     * - Request booleans select which response packet types are expected.
     * - `nRequestID` identifies the request instance.
     */
    struct SEnvironmentalConditionsRequest
    {
      bool bMaritimeSurfaceConditionsRequest = false;
      bool bTerrestrialSurfaceConditionsRequest = false;
      bool bWeatherConditionsRequest = false;
      bool bAerosolConcentrationsRequest = false;
      uint8_t nRequestID = 0;
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
    };

    /**
     * @brief Stores host control state for one entity.
     *
     * Invariants:
     * - `entityID` identifies the controlled entity.
     * - `shortEntityTypeID` and `entityType` are interpreted according to `eExtendedEntityType`.
     * - `parentID` is meaningful only when `bHasParent` is `true`.
     */
    struct SEntityControl
    {
      EActiveState eState = EActiveState::UNKNOWN;
      bool bCollisionReportingEnabled = false;
      bool bInheritAlpha = false;
      bool bSmoothingEnabled = false;
      EExtendedEntityType eExtendedEntityType = EExtendedEntityType::UNKNOWN;
      uint8_t alpha = 0;
      EntityID entityID = UnknownEntityID;
      ShortEntityTypeID shortEntityTypeID = UnknownShortEntityTypeID;
      sbio::entity::SEntityType entityType;
      EntityID parentID = UnknownEntityID;
      bool bHasParent = false;
    };

    /**
     * @brief Stores animation playback control for a single entity animation.
     *
     * Invariants:
     * - `entityID` and `animationID` identify the controlled animation instance.
     * - Loop, state, and frame-reset values are stored exactly as assigned.
     */
    struct SCigiAnimationControl
    {
      EAnimationState eAnimationState = EAnimationState::UNKNOWN;
      EAnimationFramePositionReset eAnimationFramePositionReset = EAnimationFramePositionReset::UNKNOWN;
      EAnimationLoopMode eAnimationLoopMode = EAnimationLoopMode::UNKNOWN;
      bool bInheritAlpha = false;
      Percentage alpha = UnknownPercentage;
      EntityID entityID = UnknownEntityID;
      AnimationID animationID = UnknownAnimationID;
      float fAnimationSpeed = 0;
    };
  }
}
#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
