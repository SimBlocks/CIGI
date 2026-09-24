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
      CigiDatabaseNumber databaseNumber = UnknownCigiDatabaseNumber;///< Requested database number.
      bool bEntityTypeSubstitutionEnabled = false;///< Whether entity type substitution is permitted.
      EIGMode eIgMode = EIGMode::UNKNOWN;///< Requested IG operating mode.
      bool bTimestampValid = false;///< Whether `timestamp` is valid.
      bool bSmoothingEnabled = false;///< Requested global smoothing enable state.
      FrameNumber hostFrameNumber = UnknownFrameNumber;///< Host frame associated with this control update.
      FrameNumber lastIgFrameNumber = UnknownFrameNumber;///< Last IG frame acknowledged by the host.
      uint32_t timestamp = 0;///< Host timestamp; meaningful when `bTimestampValid` is true.
    };

    /**
     * @brief Entity identity and attachment state shared by entity position updates.
     *
     * @note `entityID` identifies the entity being updated.
     * @note `bAttached` reflects only the requested attach state; no parent linkage is implied by this base type.
     */
    struct SEntityPosition
    {
      EntityID entityID = UnknownEntityID;///< Entity whose position is being updated.
      bool bAttached = false;///< Whether the update requests attachment to a parent.
    };

    /**
     * @brief Represents a top-level entity position update in geodetic space.
     *
     * @note `geodeticCoordinates` and `rotation` describe the requested world-space pose.
     * @note `eClamp` records the requested clamp mode exactly as parsed.
     */
    struct STopLevelEntityPosition : SEntityPosition
    {
      sbio::EClamp eClamp = EClamp::UNKNOWN;///< Requested terrain or ocean clamping mode.
      sbio::math::SGeodeticCoordinates geodeticCoordinates;///< World position: latitude, longitude, and ellipsoid height.
      sbio::cigi::TCigiNEDEulerRotation rotation;///< Orientation in the local north-east-down frame, in degrees.
    };

    /**
     * @brief Represents a child-entity position update relative to a parent entity.
     *
     * @note `parentID` identifies the requested parent entity.
     * @note `offset` and `rotation` are expressed in the CIGI body frame expected for child placement.
     */
    struct SChildEntityPosition : SEntityPosition
    {
      EntityID parentID = UnknownEntityID;///< Parent entity to which the child is attached.
      sbio::math::Vec3 offset;///< Position relative to the parent in CIGI forward-right-down axes.
      sbio::cigi::TCigiBodyEulerRotation rotation;///< Orientation relative to the parent, in degrees.
    };

    /**
     * @brief Stores a conformal clamped entity position request.
     *
     * @note The type carries only latitude, longitude, and yaw because altitude is derived by the conformal clamp operation.
     */
    struct SCigiConformalClampedEntityPosition
    {
      EntityID entityID = UnknownEntityID;///< Entity to position and conformally clamp.
      sbio::math::Degrees fYaw = UnknownDegrees;///< Requested yaw in degrees.
      sbio::math::Latitude latitude = sbio::math::UnknownLatitude;///< Requested latitude in degrees.
      sbio::math::Longitude longitude = sbio::math::UnknownLongitude;///< Requested longitude in degrees.
    };

    /**
     * @brief Identifies a CIGI component by component ID, component class, and instance ID.
     */
    struct SCigiComponentKey
    {
      CigiComponentID componentID = UnknownCigiComponentID;///< Component identifier.
      CigiComponentClassID componentClassID = UnknownCigiComponentClassID;///< Class of object containing the component.
      uint16_t nInstanceID = 0;///< Instance identifier within the component class.

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
       * @return `true` when this key precedes `key` lexicographically by `componentID`,
       * then `componentClassID`, then `nInstanceID`; otherwise `false`.
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
      uint8_t nComponentState = 0;///< Component-specific state code.
      uint32_t componentData[6] = {0, 0, 0, 0, 0, 0};///< Six component-specific payload words.

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
      SCigiComponentKey key;///< Component instance to update.
      SCigiComponentControlState state;///< Requested component state and payload.
    };

    /**
     * @brief Stores the compact short-component control form.
     *
     */
    struct SCigiShortComponentControl
    {
      CigiComponentID componentID = UnknownCigiComponentID;///< Component identifier.
      CigiComponentClassID componentClassID = UnknownCigiComponentClassID;///< Class of object containing the component.
      uint8_t nComponentState = 0;///< Component-specific state code.
      uint16_t nInstanceID = 0;///< Instance identifier within the component class.
      uint32_t componentData1 = 0;///< First component-specific payload word.
      uint32_t componentData2 = 0;///< Second component-specific payload word.
    };

    /**
     * @brief Stores a full articulated-part control update.
     *
     */
    struct SCigiArticulatedPart
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the articulated part.
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;///< Part to update.
      bool bEnabled = false;///< Requested part enable state.
      bool bOffsetEnabled[3] = {false, false, false};///< Enable flags for the three offset components, in axis order.
      bool bRollEnabled = false;///< Whether to apply the roll component.
      bool bPitchEnabled = false;///< Whether to apply the pitch component.
      bool bYawEnabled = false;///< Whether to apply the yaw component.
      sbio::cigi::CigiBodyCoordinates offset;///< Part offset in CIGI forward-right-down axes.
      sbio::cigi::TCigiBodyEulerRotation rotation;///< Part Euler rotation in degrees.
    };

    /**
     * @brief Stores the compact two-DOF short articulated-part control form.
     *
     */
    struct SCigiShortArticulatedPart
    {
      EntityID entityID = UnknownEntityID;///< Entity containing both referenced parts.
      ArticulatedPartID articulatedPartID1 = UnknownArticulatedPartID;///< Part targeted by the first update.
      ArticulatedPartID articulatedPartID2 = UnknownArticulatedPartID;///< Part targeted by the second update.
      EDegreeOfFreedom eDOF1 = EDegreeOfFreedom::UNKNOWN;///< Degree of freedom selected for the first update.
      EDegreeOfFreedom eDOF2 = EDegreeOfFreedom::UNKNOWN;///< Degree of freedom selected for the second update.
      bool bArticulatedPart1Enabled = false;///< Requested enable state for the first part.
      bool bArticulatedPart2Enabled = false;///< Requested enable state for the second part.
      float fDOF1 = 0;///< Value for `eDOF1`; interpretation depends on the selector.
      float fDOF2 = 0;///< Value for `eDOF2`; interpretation depends on the selector.
    };

    /**
     * @brief Stores linear and angular velocity for an entity.
     *
     */
    struct SCigiEntityVelocityControl
    {
      EntityID entityID = UnknownEntityID;///< Entity whose velocity is controlled.
      EObjectCoordinateSystem coordinateSystem = EObjectCoordinateSystem::UNKNOWN;///< Reference frame for the requested rates.
      sbio::cigi::CigiBodyCoordinates linearVelocity;///< Linear velocity components in the selected frame.
      sbio::cigi::TCigiBodyEulerVelocity angularVelocity;///< Signed Euler rates in degrees per second.
    };

    /**
     * @brief Stores linear and angular velocity for an articulated part.
     *
     */
    struct SCigiArticulatedPartVelocityControl
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the articulated part.
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;///< Part whose velocity is controlled.
      sbio::cigi::CigiBodyCoordinates linearVelocity;///< Part linear velocity in CIGI axes.
      sbio::cigi::TCigiBodyEulerVelocity angularVelocity;///< Signed part Euler rates in degrees per second.
    };

    /**
     * @brief Stores host-requested celestial sphere settings.
     *
     */
    struct SCigiCelestialSphereControl
    {
      bool bContinuousTimeOfDayEnable = false;///< Whether time of day should advance continuously.
      bool bSunEnable = false;///< Requested sun enable state.
      bool bMoonEnable = false;///< Requested moon enable state.
      bool bStarFieldEnable = false;///< Requested star-field enable state.
      bool bDateTimeValid = false;///< Whether the supplied date and time are valid.
      Hour hour = UnknownHour;///< Hour of the supplied time.
      Minute minute = UnknownMinute;///< Minute of the supplied time.
      Second second = UnknownSecond;///< Second of the supplied time.
      Year year = UnknownYear;///< Year of the supplied date.
      Month month = UnknownMonth;///< Month of the supplied date.
      Day day = UnknownDay;///< Day of the supplied date.
      Percentage starFieldIntensity = UnknownPercentage;///< Requested star-field intensity.
    };

    /**
     * @brief Stores host-requested global atmosphere settings.
     *
     */
    struct SCigiAtmosphereControl
    {
      bool bAtmosphereModelEnable = false;///< Requested atmosphere-model enable state.
      Percentage globalHumidity = UnknownPercentage;///< Global humidity.
      float fGlobalAirTemp = 0;///< Global air temperature.
      float fGlobalVisibility = 0;///< Global visibility range.
      float fGlobalHorizontalWindSpeed = 0;///< Global horizontal wind speed.
      float fGlobalVerticalWindSpeed = 0;///< Global vertical wind speed.
      Degrees globalWindDirection = UnknownDegrees;///< Global wind direction in degrees.
      float fGlobalBarometricPressure = 0;///< Global barometric pressure.
    };

    /**
     * @brief Defines one environmental region and its merge behavior.
     *
     */
    struct SCigiEnvironmentalRegion
    {
      EActiveState eRegionState = EActiveState::UNKNOWN;///< Requested region lifecycle state.
      EMergeState eMergeWeatherProperties = EMergeState::UNKNOWN;///< Weather-property merge policy.
      EMergeState eMergeAerosolConcentrations = EMergeState::UNKNOWN;///< Aerosol-concentration merge policy.
      EMergeState eMergeMaritimeSurfaceConditions = EMergeState::UNKNOWN;///< Maritime-condition merge policy.
      EMergeState eMergeTerrestrialSurfaceConditions = EMergeState::UNKNOWN;///< Terrestrial-condition merge policy.
      RegionID regionID = UnknownRegionID;///< Region to define or update.
      sbio::math::Latitude latitude = sbio::math::UnknownLatitude;///< Region latitude in degrees.
      sbio::math::Longitude longitude = sbio::math::UnknownLongitude;///< Region longitude in degrees.
      sbio::math::Vec2f size;///< Region dimensions.
      float fCornerRadius = 0;///< Radius of the region's rounded corners.
      Degrees180 fRotation = UnknownDegrees180;///< Region rotation in degrees.
      float fTransition = 0;///< Width of the region's transition band.
    };

    /**
     * @brief Stores one set of weather properties for composition or application.
     *
     *
     * Side effects: `Sum()` returns a composed value and `Scale()` returns a scaled copy; neither mutates the source operands.
     */
    struct SCigiWeatherCondition
    {
      Percentage humidity = UnknownPercentage;///< Humidity of the weather condition.
      bool bWeatherEnabled = false;///< Weather enable state.
      bool bBottomScudEnabled = false;///< Bottom-scud enable state.
      bool bRandomWindsEnabled = false;///< Random-wind enable state.
      bool bRandomLightningEnabled = false;///< Random-lightning enable state.
      CloudType cloudType = UnknownCloudType;///< Cloud type identifier.
      sbio::WeatherSeverity severity = sbio::UnknownWeatherSeverity;///< Weather severity.
      bool bTopScudEnabled = false;///< Top-scud enable state.
      sbio::TemperatureCelsius fAirTemperature = sbio::UnknownTemperatureCelsius;///< Air temperature in degrees Celsius.
      float fVisibilityRange = 0;///< Visibility range.
      Percentage bottomScudFrequency = UnknownPercentage;///< Bottom-scud frequency.
      Percentage coverage = UnknownPercentage;///< Cloud coverage.
      float HorizontalWindSpeed = 0;///< Horizontal wind speed.
      float VerticalWindSpeed = 0;///< Vertical wind speed.
      sbio::math::Degrees360 WindDirection = UnknownDegrees360;///< Wind direction in degrees.
      float fBarometricPressure = 0;///< Barometric pressure.
      float fAerosolConcentration = 0;///< Aerosol concentration.
      Percentage topScudFrequency = UnknownPercentage;///< Top-scud frequency.

      /**
       * @brief Combines weather flags, categorical values, and numeric contributions.
       * @param a First condition; supplies cloud type and severity when the corresponding value in `b` is unknown.
       * @param b Second condition; known cloud type and severity take precedence over those in `a`.
       * @return A new condition with enable flags combined by logical OR and numeric fields added.
       * Valid wind directions are added modulo 360 degrees. If only one direction is valid, that
       * direction is preserved; if neither is valid, the result is `UnknownDegrees360`.
       * Cloud type and severity are selected, not added. Arithmetic on strong types follows their
       * operators; unknown numeric values are not skipped.
       *
       * Side effects: None.
       */
      SCigiWeatherCondition static Sum(const SCigiWeatherCondition& a, const SCigiWeatherCondition& b);

      /**
       * @brief Scales numeric weather contributions while preserving flags and categorical values.
       * @param scale Multiplier for humidity, temperature, visibility, scud frequencies, coverage,
       * pressure, aerosol concentration, and both wind-speed components.
       * @return A new condition with those fields multiplied by `scale`; enable flags, cloud type,
       * severity, and wind direction are copied unchanged. Unknown numeric values are not skipped.
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
      float fBaseElevation = 0;///< Elevation of the layer base.
      float fThickness = 0;///< Vertical layer thickness.
      float fBottomTransitionBandThickness = 0;///< Thickness of the lower transition band.
      float fTopTransitionBandThickness = 0;///< Thickness of the upper transition band.
    };

    /**
     * @brief Stores maritime surface properties for one scope.
     *
     *
     * Side effects: `Sum()` and `Scale()` return derived values without mutating the source operands.
     */
    struct SCigiMaritimeSurfaceCondition
    {
      bool bActive = false;///< Whether the maritime condition is active.
      bool bWhitecapEnabled = false;///< Whitecap enable state.
      sbio::math::HeightRelativeToWGS84Ellipsoid fSeaSurfaceHeight = sbio::math::UnknownHeightRelativeToWGS84Ellipsoid;///< Sea height above the WGS84 ellipsoid, in meters.
      sbio::TemperatureCelsius fSurfaceWaterTemperature = sbio::UnknownTemperatureCelsius;///< Surface-water temperature in degrees Celsius.
      Percentage surfaceClarity = UnknownPercentage;///< Water clarity.

      /**
       * @brief Combines maritime enable flags and numeric contributions without changing the inputs.
       * @param a First maritime condition.
       * @param b Second maritime condition.
       * @return A new condition with `bActive` and `bWhitecapEnabled` combined by logical OR,
       * and sea height, water temperature, and clarity added using their strong-type operators.
       * Unknown numeric values are not skipped.
       */
      SCigiMaritimeSurfaceCondition static Sum(const SCigiMaritimeSurfaceCondition& a, const SCigiMaritimeSurfaceCondition& b);

      /**
       * @brief Scales maritime numeric contributions while preserving enable flags.
       * @param scale Multiplier for sea height, water temperature, and clarity.
       * @return A new condition with those fields multiplied by `scale` and both flags copied unchanged.
       * Unknown numeric values are not skipped.
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
      uint8_t waveID = 0;///< Wave identifier within its scope.
      bool bWaveEnabled = false;///< Wave enable state.
      EWaveBreakerType eBreakerType = EWaveBreakerType::UNKNOWN;///< Breaker shape classification.
      float fWaveHeight = 0;///< Wave height.
      float fWavelength = 0;///< Wavelength.
      float fPeriod = 0;///< Wave period.
      Degrees360 direction = UnknownDegrees360;///< Wave direction in degrees.
      Degrees360 phaseOffset = UnknownDegrees360;///< Wave phase offset in degrees.
      Degrees180 leading = UnknownDegrees180;///< Wave leading angle in degrees.
    };

    /** @brief Encoded terrestrial-surface severity in the inclusive range 0 through 31, with an unknown sentinel. */
    RANGED_STRONG_INT_WITH_UNKNOWN(CigiTerrestrialSurfaceSeverity, uint8_t, 0, 31);

    /**
     * @brief Stores one terrestrial surface condition request or state description.
     *
     *
     * Side effects: `IsDry()` reads the current instance only.
     */
    struct SCigiTerrestrialSurfaceCondition
    {
      bool bEnabled = false;///< Surface-condition enable state.
      Percentage severity = UnknownPercentage;///< Condition severity.
      Percentage coverage = UnknownPercentage;///< Surface coverage.
      SurfaceConditionID surfaceConditionID = UnknownSurfaceConditionID;///< Condition identifier; zero denotes dry.

      /**
       * @brief Checks whether this condition represents the dry surface condition.
       * @return `true` exactly when `surfaceConditionID == SurfaceConditionID(0)`, regardless of
       * enable state, severity, or coverage; otherwise `false`.
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
      uint8_t groupID = 0;///< Raw group identifier retained in the control payload.
      ViewGroupID viewGroupID = UnknownViewGroupID;///< Target view group identifier.
      bool offsetEnabled[3] = {false, false, false};///< Enable flags for the three offset components, in axis order.
      bool bYawEnabled = false;///< Whether to apply yaw.
      bool bPitchEnabled = false;///< Whether to apply pitch.
      bool bRollEnabled = false;///< Whether to apply roll.
      ViewID viewID = UnknownViewID;///< Target view identifier.
      EntityID entityID = UnknownEntityID;///< Entity to which the view is attached.
      sbio::cigi::CigiBodyCoordinates offset;///< View offset in CIGI forward-right-down axes.
      sbio::cigi::TCigiBodyEulerRotation rotation;///< View Euler rotation in degrees.
    };

    /**
     * @brief Stores sensor control values for one sensor.
     *
     */
    struct SCigiSensorControl
    {
      SensorID sensorID = UnknownSensorID;///< Sensor to control.
      ETrackMode eTrackMode = ETrackMode::UNKNOWN;///< Requested tracking mode.
      bool bSensorOn = false;///< Requested sensor power state.
      EPolarity ePolarity = EPolarity::UNKNOWN;///< Image polarity.
      bool bLineByLineDropoutEnabled = false;///< Line-by-line dropout enable state.
      bool bAutomaticGain = false;///< Automatic gain control enable state.
      ESensorTrack eSensorTrack = ESensorTrack::UNKNOWN;///< Track-gate color selection.
      bool bExtendedResponse = false;///< Whether an extended sensor response is requested.
      ViewID viewID = UnknownViewID;///< View associated with the sensor.
      SensorGain gain = SensorGain(0);///< Sensor gain in [0, 1].
      SensorLevel level = SensorLevel(0);///< Sensor level in [0, 1].
      float fACCoupling = 0;///< AC coupling setting.
      SensorNoise noise = SensorNoise(0);///< Sensor noise in [0, 1].
    };

    /**
     * @brief Stores the enabled degrees of freedom for one motion tracker.
     *
     */
    struct SMotionTrackerControl
    {
      MotionTrackerID motionTrackerID = UnknownMotionTrackerID;///< Tracker to control.
      bool bEnable = false;///< Tracker enable state.
      bool bBoresightEnable = false;///< Boresight enable state.
      bool bXEnable = false;///< Whether to use the tracker X component.
      bool bYEnable = false;///< Whether to use the tracker Y component.
      bool bZEnable = false;///< Whether to use the tracker Z component.
      bool bRollEnable = false;///< Whether to use tracker roll.
      bool bPitchEnable = false;///< Whether to use tracker pitch.
      bool bYawEnable = false;///< Whether to use tracker yaw.
    };

    /**
     * @brief Associates a motion tracker control request with a specific view.
     *
     */
    struct SMotionTrackerViewControl : SMotionTrackerControl
    {
      ViewID viewID = UnknownViewID;///< View to receive tracker updates.
    };

    /**
     * @brief Associates a motion tracker control request with a specific view group.
     *
     */
    struct SMotionTrackerViewGroupControl : SMotionTrackerControl
    {
      ViewGroupID viewGroupID = UnknownViewGroupID;///< View group to receive tracker updates.
    };

    /**
     * @brief Stores an earth-reference-model definition supplied by the host.
     *
     */
    struct SCigiEarthReferenceModel
    {
      EEarthReferenceModel eEarthReferenceModel = EEarthReferenceModel::UNKNOWN;///< WGS84 or host-defined model selection.
      double fEquatorialRadius = 0;///< Equatorial radius supplied for the reference ellipsoid.
      double fFlattening = 0;///< Flattening supplied for the reference ellipsoid.
    };

    /**
     * @brief Stores linear and angular acceleration for an entity.
     *
     */
    struct SCigiEntityAcceleration
    {
      EntityID entityID = UnknownEntityID;///< Entity whose acceleration is controlled.
      sbio::cigi::CigiBodyCoordinates linearAcceleration;///< Linear acceleration components in the selected frame.
      sbio::EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;///< Reference frame for the requested accelerations.
      sbio::cigi::TCigiBodyEulerAcceleration angularAcceleration;///< Signed Euler accelerations in degrees per second squared.
    };

    /**
     * @brief Stores linear and angular acceleration for an articulated part.
     *
     */
    struct SCigiArticulatedPartAcceleration
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the articulated part.
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;///< Part whose acceleration is controlled.
      sbio::EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;///< Reference frame for the requested accelerations.
      sbio::cigi::CigiBodyCoordinates linearAcceleration;///< Part linear acceleration components in the selected frame.
      sbio::cigi::TCigiBodyEulerAcceleration angularAcceleration;///< Signed part Euler accelerations in degrees per second squared.
    };

    /** @brief Encoded pixel-replication mode for a CIGI view definition. */
    STRONG_TYPE(PixelReplicationMode, uint8_t)

    /**
     * @brief Stores the static definition for one view.
     *
     */
    struct SCigiViewDefinition
    {
      ViewID viewID = UnknownViewID;///< View to define or update.
      ViewGroupID viewGroupID = UnknownViewGroupID;///< Group associated with the view.
      bool bNearEnabled = false;///< Whether to apply `fNear`.
      bool bFarEnabled = false;///< Whether to apply `fFar`.
      bool bLeftEnabled = false;///< Whether to apply `fLeft`.
      bool bRightEnabled = false;///< Whether to apply `fRight`.
      bool bTopEnabled = false;///< Whether to apply `fTop`.
      bool bBottomEnabled = false;///< Whether to apply `fBottom`.
      EMirrorMode eMirrorMode = EMirrorMode::UNKNOWN;///< Requested image mirroring mode.
      PixelReplicationMode pixelReplicationMode = UnknownPixelReplicationMode;///< Encoded pixel-replication setting.
      EProjectionMode eProjectionMode = EProjectionMode::UNKNOWN;///< Requested projection mode.
      bool bReorder = false;///< Whether view reordering is requested.
      ViewType viewType = UnknownViewType;///< Requested view type.
      float fNear = 0;///< Near clipping-plane value.
      float fFar = 0;///< Far clipping-plane value.
      float fLeft = 0;///< Left projection-boundary value.
      float fRight = 0;///< Right projection-boundary value.
      float fTop = 0;///< Top projection-boundary value.
      float fBottom = 0;///< Bottom projection-boundary value.
    };

    /**
     * @brief Defines a collision-detection segment attached to an entity.
     *
     */
    struct SCollisionDetectionSegmentDefinition
    {
      SegmentID segmentID = UnknownSegmentID;///< Segment identifier on the entity.
      bool bSegmentEnabled = false;///< Segment collision-detection enable state.
      EntityID entityID = UnknownEntityID;///< Entity containing the segment.
      CigiBodyCoordinates beg;///< Segment start in entity-relative CIGI coordinates.
      CigiBodyCoordinates end;///< Segment end in entity-relative CIGI coordinates.
      uint32_t nMaterialMask = 0;///< Material mask for collision queries.
    };

    /**
     * @brief Base data shared by collision-detection volume definitions.
     *
     */
    struct SCollisionDetectionVolumeDefinition
    {
      VolumeID volumeID = UnknownVolumeID;///< Volume identifier on the entity.
      bool bVolumeEnabled = false;///< Volume collision-detection enable state.
      EntityID entityID = UnknownEntityID;///< Entity containing the volume.
      sbio::cigi::CigiBodyCoordinates offset;///< Volume offset in entity-relative CIGI coordinates.
    };

    /**
     * @brief Defines a cuboid collision-detection volume.
     *
     */
    struct SCollisionDetectionCuboidDefinition : SCollisionDetectionVolumeDefinition
    {
      float fHeight = 0;///< Cuboid height.
      float fWidth = 0;///< Cuboid width.
      float fDepth = 0;///< Cuboid depth.
      TCigiBodyEulerRotation rotation;///< Cuboid orientation in CIGI Euler angles, in degrees.
    };

    /**
     * @brief Defines a spherical collision-detection volume.
     *
     */
    struct SCollisionDetectionSphereDefinition : SCollisionDetectionVolumeDefinition
    {
      float fRadius = 0;///< Collision sphere radius.
    };

    /** @brief Encoded update period carried by HAT/HOT and line-of-sight requests. */
    STRONG_TYPE(UpdatePeriod, uint8_t)

    /**
     * @brief Base data shared by all HAT/HOT request variants.
     *
     */
    struct SBaseHATHOTRequest
    {
      HATHOTID requestID = UnknownHATHOTID;///< Identifier used to correlate the response.
      UpdatePeriod updatePeriod = UnknownUpdatePeriod;///< Requested response update period.
      FrameNumber lastHostFrameNumber = UnknownFrameNumber;///< Host frame associated with the request.
      ERequestType eRequestType = ERequestType::UNKNOWN;///< Selects height above terrain, height of terrain, or an extended response.
    };

    /**
     * @brief Requests HAT, HOT, or extended terrain information at a geodetic point.
     *
     */
    struct SHATHOTGlobalRequest : SBaseHATHOTRequest
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;///< Geodetic test point.
    };

    /**
     * @brief Requests HAT, HOT, or extended terrain information at an entity-relative point.
     *
     */
    struct SHATHOTEntityRequest : SBaseHATHOTRequest
    {
      EntityID entityID = UnknownEntityID;///< Entity defining the test point's reference frame.
      sbio::math::Vec3 offset;///< Test-point offset from the entity.
    };

    /**
     * @brief Base data shared by all line-of-sight request variants.
     *
     */
    struct SLineOfSightRequest
    {
      LineOfSightRequestID requestID = UnknownLineOfSightRequestID;///< Identifier used to correlate the response.
      UpdatePeriod updatePeriod = UnknownUpdatePeriod;///< Requested response update period.
      uint8_t nAlphaThreshold = 0;///< Alpha threshold for intersection testing.
      uint32_t nMaterialMask = 0;///< Material mask for intersection testing.
      FrameNumber lastHostFrameNumber = UnknownFrameNumber;///< Host frame associated with the request.
    };

    /**
     * @brief Line-of-sight segment request from a geodetic source to a geodetic destination.
     */
    struct SLineOfSightSegmentRequestGeodeticToGeodeticBasic : SLineOfSightRequest
    {
      sbio::math::SGeodeticCoordinates sourceGeodeticCoordinates;///< Geodetic segment start.
      sbio::math::SGeodeticCoordinates destinationGeodeticCoordinates;///< Geodetic segment end.
    };

    /**
     * @brief Geodetic-to-geodetic segment query requesting extended hit details in a selected response frame.
     */
    struct SLineOfSightSegmentRequestGeodeticToGeodeticExtended : SLineOfSightSegmentRequestGeodeticToGeodeticBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;///< Requested coordinate system for the reported hit position.
    };

    /**
     * @brief Line-of-sight segment request from a geodetic source to an entity-relative destination.
     *
     */
    struct SLineOfSightSegmentRequestGeodeticToEntityBasic : SLineOfSightRequest
    {
      sbio::math::SGeodeticCoordinates sourceGeodeticCoordinates;///< Geodetic segment start.
      EntityID destinationEntityID = UnknownEntityID;///< Entity defining the destination frame.
      sbio::math::Vec3 destinationOffset;///< Segment end relative to the destination entity.
    };

    /**
     * @brief Geodetic-to-entity segment query requesting extended hit details in a selected response frame.
     */
    struct SLineOfSightSegmentRequestGeodeticToEntityExtended : SLineOfSightSegmentRequestGeodeticToEntityBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;///< Requested coordinate system for the reported hit position.
    };

    /**
     * @brief Line-of-sight segment request from an entity-relative source to a geodetic destination.
     */
    struct SLineOfSightSegmentRequestEntityToGeodeticBasic : SLineOfSightRequest
    {
      EntityID sourceEntityID = UnknownEntityID;///< Entity defining the source frame.
      sbio::cigi::CigiBodyCoordinates sourceOffset;///< Segment start relative to the source entity, in CIGI axes.
      sbio::math::SGeodeticCoordinates destinationGeodeticCoordinates;///< Geodetic segment end.
    };

    /**
     * @brief Entity-to-geodetic segment query requesting extended hit details in a selected response frame.
     */
    struct SLineOfSightSegmentRequestEntityToGeodeticExtended : SLineOfSightSegmentRequestEntityToGeodeticBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;///< Requested coordinate system for the reported hit position.
    };

    /**
     * @brief Line-of-sight segment request from one source entity-relative point to a point on the destination entity.
     * The destination entity may be the same as the source entity.
     *
     */
    struct SLineOfSightSegmentRequestEntityToEntityBasic : SLineOfSightRequest
    {
      EntityID sourceEntityID = UnknownEntityID;///< Entity defining the source frame.
      sbio::cigi::CigiBodyCoordinates sourceOffset;///< Segment start relative to the source entity, in CIGI axes.
      EntityID destinationEntityID = UnknownEntityID;///< Entity defining the destination frame.
      sbio::cigi::CigiBodyCoordinates destinationOffset;///< Segment end relative to the destination entity, in CIGI axes.
    };

    /**
     * @brief Entity-to-entity segment query requesting extended hit details in a selected response frame.
     */
    struct SLineOfSightSegmentRequestEntityToEntityExtended : SLineOfSightSegmentRequestEntityToEntityBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;///< Requested coordinate system for the reported hit position.
    };

    // Line of Sight Vector Request
    /**
     * @brief Base data for line-of-sight vector requests.
     *
     */
    struct SLineOfSightVectorRequest : SLineOfSightRequest
    {
      Degrees180 azimuth = UnknownDegrees180;///< Query direction azimuth in degrees.
      Degrees90 elevation = UnknownDegrees90;///< Query direction elevation in degrees.
      float fMinimumRange = 0;///< Minimum query range.
      float fMaximumRange = 0;///< Maximum query range.
    };

    /**
     * @brief Line-of-sight vector request originating from a geodetic point.
     */
    struct SLineOfSightVectorRequestGeodeticBasic : SLineOfSightVectorRequest
    {
      sbio::math::SGeodeticCoordinates sourceGeodeticCoordinates;///< Geodetic query origin.
    };

    /**
     * @brief Geodetic-origin vector query requesting extended hit details in a selected response frame.
     */
    struct SLineOfSightVectorRequestGeodeticExtended : SLineOfSightVectorRequestGeodeticBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;///< Requested coordinate system for the reported hit position.
    };

    /**
     * @brief Line-of-sight vector request originating from an entity-relative point.
     */
    struct SLineOfSightVectorRequestEntityBasic : SLineOfSightVectorRequest
    {
      EntityID sourceEntityID = UnknownEntityID;///< Entity defining the source frame.
      sbio::cigi::CigiBodyCoordinates sourceOffset;///< Query origin relative to the source entity, in CIGI axes.
    };

    /**
     * @brief Entity-relative vector query requesting extended hit details in a selected response frame.
     */
    struct SLineOfSightVectorRequestEntityExtended : SLineOfSightVectorRequestEntityBasic
    {
      ETopLevelCoordinateSystem eResponseCoordinateSystem = ETopLevelCoordinateSystem::UNKNOWN;///< Requested coordinate system for the reported hit position.
    };

    /**
     * @brief Stores one position request for an entity, articulated part, view, or tracker object.
     *
     */
    struct SPositionRequest
    {
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;///< Part identifier for an articulated-part query.
      bool bContinuous = false;///< Whether responses should be sent continuously.
      EObjectClass eObjectClass = EObjectClass::UNKNOWN;///< Class of object being queried.
      EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;///< Requested response frame.
      uint16_t nObjectID = 0;///< Object identifier interpreted according to `eObjectClass`.
    };

    /**
     * @brief Stores one environmental conditions query for a geodetic location.
     *
     */
    struct SEnvironmentalConditionsRequest
    {
      bool bMaritimeSurfaceConditionsRequest = false;///< Whether to report maritime surface conditions.
      bool bTerrestrialSurfaceConditionsRequest = false;///< Whether to report terrestrial surface conditions.
      bool bWeatherConditionsRequest = false;///< Whether to report weather conditions.
      bool bAerosolConcentrationsRequest = false;///< Whether to report aerosol concentrations.
      uint8_t nRequestID = 0;///< Identifier used to correlate responses.
      sbio::math::SGeodeticCoordinates geodeticCoordinates;///< Location at which to sample conditions.
    };

    /**
     * @brief Stores host control state for one entity.
     *
     */
    struct SEntityControl
    {
      EActiveState eState = EActiveState::UNKNOWN;///< Requested entity lifecycle state.
      bool bCollisionReportingEnabled = false;///< Requested collision-reporting enable state.
      bool bInheritAlpha = false;///< Whether to inherit alpha from the parent.
      bool bSmoothingEnabled = false;///< Requested entity smoothing enable state.
      EExtendedEntityType eExtendedEntityType = EExtendedEntityType::UNKNOWN;///< Selects short or extended entity-type representation.
      uint8_t alpha = 0;///< Encoded entity alpha byte.
      EntityID entityID = UnknownEntityID;///< Entity to control.
      ShortEntityTypeID shortEntityTypeID = UnknownShortEntityTypeID;///< Short entity type identifier.
      sbio::entity::SEntityType entityType;///< Extended SISO entity type enumeration.
      EntityID parentID = UnknownEntityID;///< Parent identifier when `bHasParent` is true.
      bool bHasParent = false;///< Whether the control state specifies a parent.
    };

    /**
     * @brief Stores animation playback control for a single entity animation.
     *
     */
    struct SCigiAnimationControl
    {
      EAnimationState eAnimationState = EAnimationState::UNKNOWN;///< Requested playback state.
      EAnimationFramePositionReset eAnimationFramePositionReset = EAnimationFramePositionReset::UNKNOWN;///< Whether to retain or reset the frame position.
      EAnimationLoopMode eAnimationLoopMode = EAnimationLoopMode::UNKNOWN;///< One-shot or continuous playback selection.
      bool bInheritAlpha = false;///< Whether the animation inherits alpha.
      Percentage alpha = UnknownPercentage;///< Requested animation alpha.
      EntityID entityID = UnknownEntityID;///< Entity containing the animation.
      AnimationID animationID = UnknownAnimationID;///< Animation to control.
      float fAnimationSpeed = 0;///< Requested animation speed.
    };
  }
}
#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
