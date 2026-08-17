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
     * @note `databaseNumber` is stored exactly as received and may use the CIGI loading conventions for negative values.
     * @note `hostFrameNumber` and `lastIgFrameNumber` are host-supplied frame identifiers.
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
     * @note `entityID` identifies the entity being updated.
     * @note `bAttached` reflects only the requested attach state; no parent linkage is implied by this base type.
     */
    struct SEntityPosition
    {
      EntityID entityID = UnknownEntityID;
      bool bAttached = false;
    };

    /**
     * @brief Represents a top-level entity position update in geodetic space.
     *
     * @note `geodeticCoordinates` and `rotation` describe the requested world-space pose.
     * @note `eClamp` records the requested clamp mode exactly as parsed.
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
     * @note `parentID` identifies the requested parent entity.
     * @note `offset` and `rotation` are expressed in the CIGI body frame expected for child placement.
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
     * @note The type carries only latitude, longitude, and yaw because altitude is derived by the conformal clamp operation.
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
     * @note `componentData` contains exactly six 32-bit words because that is the fixed component-data shape represented by this API.
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
     */
    struct SCigiComponentControl
    {
      SCigiComponentKey key;
      SCigiComponentControlState state;
    };

    /**
     * @brief Stores the compact short-component control form.
     *
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
      sbio::math::Degrees360 WindDirection = UnknownDegrees360;
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
     */
    struct SMotionTrackerViewControl : SMotionTrackerControl
    {
      ViewID viewID = UnknownViewID;
    };

    /**
     * @brief Associates a motion tracker control request with a specific view group.
     *
     */
    struct SMotionTrackerViewGroupControl : SMotionTrackerControl
    {
      ViewGroupID viewGroupID = UnknownViewGroupID;
    };

    /**
     * @brief Stores an earth-reference-model definition supplied by the host.
     *
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
     */
    struct SCollisionDetectionSphereDefinition : SCollisionDetectionVolumeDefinition
    {
      float fRadius = 0;
    };

    STRONG_TYPE(UpdatePeriod, uint8_t)

    /**
     * @brief Base data shared by all HAT/HOT request variants.
     *
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
     */
    struct SHATHOTGlobalRequest : SBaseHATHOTRequest
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
    };

    /**
     * @brief Requests height above terrain for an entity-relative point.
     *
     */
    struct SHATHOTEntityRequest : SBaseHATHOTRequest
    {
      EntityID entityID = UnknownEntityID;
      sbio::math::Vec3 offset;
    };

    /**
     * @brief Base data shared by all line-of-sight request variants.
     *
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
