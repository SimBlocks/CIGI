//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiTypesIGToHost.h
 * @brief Declares response structures for CIGI IG-to-Host messages.
 *
 * Provides structures for responses and notifications sent from the Image Generator (IG) to the Host in the CIGI protocol.
 * Includes start of frame, HAT/HOT, line of sight, sensor, position, weather, surface condition, collision, and animation
 * response types. Supports simulation interoperability and protocol integration for IG-to-Host communication.
 *
 * @see sbio::cigi namespace
 * @see CIGI::V40
 * @see CIGI::V33
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TYPES_IG_TO_HOST_H
#define SIMBLOCKS_CIGI_TYPES_IG_TO_HOST_H

#include "CigiLib/CigiTypes.h"

namespace sbio
{
  namespace cigi
  {
    /**
     * @brief Strong type for image-generator-defined message identifiers.
     *
     * The numeric assignments are IG-specific and are typically resolved to user-facing text by
     * higher-level application code.
      * @param ImageGeneratorMessageID Message text or payload.
      * @param uint16_t Uint16 t value.
     */
    STRONG_TYPE(ImageGeneratorMessageID, uint16_t);///< Strong type for Image Generator Message ID. Event ID assignments are IG-specific.

    /**
     * @brief Strong type for image-generator-defined event identifiers.
     *
     * The numeric assignments are IG-specific and are preserved exactly as reported by the IG.
      * @param ImageGeneratorEventID Image generator event id value.
      * @param uint16_t Uint16 t value.
     */
    STRONG_TYPE(ImageGeneratorEventID, uint16_t);///< Strong type for Event ID. Event ID assignments are IG-specific.

    // Start Of Frame
    /**
     * @brief Stores the IG state reported in a start-of-frame packet.
     *
     * Invariants:
     * - `eVersion` identifies the CIGI version used to interpret this packet.
     * - `databaseID`, `eIGMode`, and frame fields are stored exactly as supplied by the IG.
     * - Status flags mirror the packet state without additional validation.
     */
    struct SCigiStartOfFrame
    {
      ECigiVersion eVersion = ECigiVersion::UNKNOWN_VERSION;
      CigiDatabaseNumber databaseID = UnknownCigiDatabaseNumber;
      EIGMode eIGMode = EIGMode::UNKNOWN;
      bool bTimestampValid = false;
      bool bEarthReferenceModel = false;
      FrameNumber igFrameNumber = UnknownFrameNumber;
      Microsecond microseconds = UnknownMicrosecond;
      FrameNumber lastHostFrameNumber = UnknownFrameNumber;
      bool bOverframing = false;
      bool bPagingTerrain = false;
      bool bExcessiveVariableLengthData = false;
    };

    /**
     * @brief Base data shared by HAT, HOT, and extended HAT/HOT responses.
     *
     * Invariants:
     * - `HATHOTID` identifies the originating request.
     * - `hostFrameLSN` preserves the low-significance host frame number carried by the protocol.
     */
    struct SBaseHATHOTResponse
    {
      HATHOTID HATHOTID = UnknownHATHOTID;
      bool bValid = false;
      uint8_t hostFrameLSN = 0;
    };

    /**
     * @brief Reports height above terrain for a HAT request.
     *
     * Invariants:
     * - `heightAboveTerrain` is stored exactly as supplied by the IG and may be negative when the test point is below terrain.
     */
    struct SHeightAboveTerrainResponse : SBaseHATHOTResponse
    {
      double heightAboveTerrain = 0;// This parameter indicates the height of the test point above the terrain. A negative value indicates that the test point is below the terrain.
    };

    /**
     * @brief Reports terrain height relative to the test point for a HOT request.
     *
     * Invariants:
     * - `heightOfTerrain` is stored exactly as supplied by the IG.
     */
    struct SHeightOfTerrainResponse : SBaseHATHOTResponse
    {
      HeightRelativeToWGS84Ellipsoid heightOfTerrain = UnknownHeightRelativeToWGS84Ellipsoid;// This parameter indicates the height of terrain above or below the test point. This value is relative to the ellipsoid height.
    };

    /**
     * @brief Reports the extended HAT/HOT response that carries both heights and surface details.
     *
     * Invariants:
     * - Height and surface members are stored exactly as supplied by the IG.
     * - Surface fields are meaningful only when `bValid` is `true`.
     */
    struct SHATHOTExtendedResponse : SBaseHATHOTResponse
    {
      double heightAboveTerrain = 0;// This parameter indicates the height of the test point above the terrain. A negative value indicates that the test point is below the terrain.
      sbio::math::HeightRelativeToWGS84Ellipsoid heightOfTerrain = UnknownHeightRelativeToWGS84Ellipsoid;// This parameter indicates the height of terrain at the relative to the ellipsoid height.
      sbio::MaterialID materialCode = UnknownMaterialID;
      sbio::math::Degrees180 normalVectorAzimuth = UnknownDegrees180;// This parameter indicates the azimuth of the normal vector to the terrain surface at the test point. The value is measured in degrees clockwise from true north. The value is between 0 and 360 degrees.
      sbio::math::Degrees90 normalVectorElevation = UnknownDegrees90;// This parameter indicates the elevation of the normal vector to the terrain surface at the test point. The value is measured in degrees from the horizontal plane. The value is between -90 and 90 degrees.
    };

    /**
     * @brief Stores the basic line-of-sight response payload.
     *
     * Invariants:
     * - `lineOfSightRequestID` identifies the originating request.
     * - `entityID` is meaningful only when `bEntityIDValid` is `true`.
     * - `responseCount` preserves the count reported by the IG for repeated responses.
     */
    struct SLineOfSightResponse
    {
      LineOfSightRequestID lineOfSightRequestID = UnknownLineOfSightRequestID;
      bool bValid = false;
      uint8_t hostFrameLSN = 0;
      uint8_t responseCount = 0;
      double dRange = 0;
    };

    /**
     * @brief Stores the basic line-of-sight response when the hit result identifies an entity.
     *
     * Invariants:
     * - `entityID` identifies the hit entity when `bValid` is `true`.
     * - `bVisible` mirrors the visibility flag returned by the IG.
     */
    struct SLineOfSightEntityResponse
    {
      LineOfSightRequestID lineOfSightRequestID = UnknownLineOfSightRequestID;
      bool bValid = false;
      uint8_t hostFrameLSN = 0;
      uint8_t responseCount = 0;
      double dRange = 0;
      EntityID entityID = UnknownEntityID;

      //The IG should set the visible parameter to false if the segment intersected one or more polygons before reaching the destination point.
      // If the LOS segment destination point is within the body of a target entity model,
      // then the IG should set this parameter to false and the Entity ID parameter to the ID of that entity.
      //The IG should set the visible parameter to true if the segment did not intersect with any polygons before reaching the destination point.
      bool bVisible = false;
    };

    /**
     * @brief Base data shared by extended line-of-sight responses.
     *
     * Invariants:
     * - `dRange` is meaningful only when `bRangeValid` is `true`.
     * - Surface and normal-vector members are stored exactly as supplied by the IG.
     */
    struct SBaseLineOfSightExtendedResponse
    {
      LineOfSightRequestID lineOfSightRequestID = UnknownLineOfSightRequestID;
      bool bValid = false;
      bool bRangeValid = false;
      bool bVisible = false;
      uint8_t hostFrameLSN = 0;
      uint8_t responseCount = 0;
      double dRange = 0;
      SColor32 surfaceColor;
      sbio::MaterialID materialCode = UnknownMaterialID;
      float fNormalVectorAzimuth = {0};
      float fNormalVectorElevation = {0};
    };

    /**
     * @brief Extended line-of-sight response whose hit location is expressed in geodetic coordinates.
     * SLineOfSightExtendedGeodeticCoordinatesResponse will be the response type for an extended line-of-sight request
     if no entity entity is hit, regardless of the requested response coordinate system. 
     */
    struct SLineOfSightExtendedGeodeticCoordinatesResponse : SBaseLineOfSightExtendedResponse
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
    };

    /**
     * @brief Extended line-of-sight response whose hit location is expressed in geodetic coordinates.
     * SLineOfSightExtendedEntityGeodeticCoordinatesResponse will be the response type for an extended line-of-sight request
     if an entity is hit and the requested response coordinate system is geodetic.
     */
    struct SLineOfSightExtendedEntityGeodeticCoordinatesResponse : SBaseLineOfSightExtendedResponse
    {
      EntityID entityID = UnknownEntityID;
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
    };

    /**
     * @brief Extended line-of-sight response whose hit location is expressed relative to an entity.
     * SLineOfSightExtendedEntityCoordinatesResponse will be the response type for an extended line-of-sight request
     if an entity is hit and the requested response coordinate system is relative to an entity.
     *
     * Invariants:
     * - `entityID` identifies the reference entity for `offset`.
     */
    struct SLineOfSightExtendedEntityCoordinatesResponse : SBaseLineOfSightExtendedResponse
    {
      EntityID entityID = UnknownEntityID;
      sbio::cigi::CigiBodyCoordinates offset;
    };

    /**
     * @brief Reports the current state of a basic sensor response.
     *
     * Invariants:
     * - `sensorID` identifies the reporting sensor.
     * - `gateSize` and `gatePosition` are stored exactly as supplied by the IG.
     */
    struct SSensorResponse
    {
      SensorID sensorID = UnknownSensorID;
      ESensorStatus eSensorStatus = ESensorStatus::UKNOWN;
      ViewID viewID = UnknownViewID;
      sbio::math::Vec2f gateSize;
      sbio::math::Vec2f gatePosition;
      FrameNumber hostFrameNumber = UnknownFrameNumber;
    };

    /**
     * @brief Reports the current state of an extended sensor response using a geodetic track point.
     */
    struct SSensorExtendedResponse
    {
      ViewID viewID = UnknownViewID;
      SensorID sensorID = UnknownSensorID;
      ESensorStatus eSensorStatus = ESensorStatus::UKNOWN;
      sbio::math::Vec2f gateSize;
      sbio::math::Vec2f gatePosition;
      FrameNumber hostFrameNumber = UnknownFrameNumber;
      sbio::math::SGeodeticCoordinates trackPoint;
    };

    /**
     * @brief Reports the current state of an extended sensor response associated with an entity.
     *
     * Invariants:
     * - `entityID` identifies the tracked entity reference.
     */
    struct SSensorExtendedEntityResponse
    {
      ViewID viewID = UnknownViewID;
      EntityID entityID = UnknownEntityID;
      SensorID sensorID = UnknownSensorID;
      ESensorStatus eSensorStatus = ESensorStatus::UKNOWN;
      sbio::math::Vec2f gateSize;
      sbio::math::Vec2f gatePosition;
      FrameNumber hostFrameNumber = UnknownFrameNumber;
      sbio::math::SGeodeticCoordinates trackPoint;
    };

    /**
     * @brief Classifies the concrete position response payload returned by the IG.
     */
    enum class EPositionResponseType
    {
      UNKNOWN = -1,
      GEODETIC,
      PARENT,
      ARTICULATED
    };

    /**
     * @brief Base data shared by all position response variants.
     *
     * Invariants:
     * - `eObjectClass` and `objectID` identify the responding object.
     * - `rotation` stores the reported orientation exactly as supplied by the IG.
     */
    struct SBasePositionResponse
    {
      EObjectClass eObjectClass = EObjectClass::UNKNOWN;
      uint16_t objectID = 0;
      sbio::cigi::TCigiBodyEulerRotation rotation;
    };

    /**
     * @brief Position response expressed in geodetic coordinates.
     */
    struct SPositionResponseGeodeticCoordinates : SBasePositionResponse
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;
    };

    /**
     * @brief Position response expressed as an offset from a parent entity.
     */
    struct SPositionResponseParentEntityCoordinates : SBasePositionResponse
    {
      CigiBodyCoordinates offset;
    };

    /**
     * @brief Position response expressed as an offset from an articulated part.
     *
     * Invariants:
     * - `articulatedPartID` identifies the articulated part that provides the reference frame for `offset`.
     */
    struct SPositionResponseArticulatedPartCoordinates : SBasePositionResponse
    {
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;
      sbio::cigi::CigiBodyCoordinates offset;
    };

    /**
     * @brief Reports weather conditions at the location requested by the host.
     *
     * Invariants:
     * - `requestID` correlates this response with the originating request.
     * - Atmospheric members are stored exactly as supplied by the IG.
     */
    struct SWeatherConditionsResponse
    {
      uint8_t requestID = 0;
      Percentage humidity = UnknownPercentage;
      float fAirTemperature = 0;
      float fVisibilityRange = 0;
      SWindSpeed windSpeedHorVer = {0, 0};
      float fWindDirection = 0;
      float fBarometricPressure = 0;
    };

    /**
     * @brief Reports aerosol concentration for one weather layer.
     *
     * Invariants:
     * - `layerID` identifies the returned layer within the IG-specific weather model.
     */
    struct SAerosolConcentrationResponse
    {
      uint8_t requestID = 0;
      uint8_t layerID = 0;
      float fAerosolConcentration = 0;
    };

    /**
     * @brief Reports maritime surface conditions at the location requested by the host.
     *
     * Invariants:
     * - `requestID` correlates this response with the originating request.
     */
    struct SMaritimeSurfaceConditionsResponse
    {
      uint8_t requestID = 0;
      sbio::math::HeightRelativeToWGS84Ellipsoid fSeaSurfaceHeight = sbio::math::UnknownHeightRelativeToWGS84Ellipsoid;
      sbio::TemperatureCelsius fSurfaceWaterTemperature = sbio::UnknownTemperatureCelsius;
      Percentage surfaceClarity = UnknownPercentage;//0-1. A value of 100% indicates pristine water. A value of 0% indicates extremely turbid water.
    };

    /**
     * @brief Reports the terrestrial surface condition at the requested location.
     *
     * Invariants:
     * - `surfaceConditionID` is the IG-reported surface condition identifier for the sample point.
     */
    struct STerrestrialSurfaceConditionsResponse
    {
      uint8_t requestID = 0;
      uint32_t surfaceConditionID = 0;
    };

    /**
     * @brief Reports a non-entity collision detected by a collision segment.
     *
     * Invariants:
     * - `entityID` and `segmentID` identify the segment that generated the notification.
     */
    struct SCollisionDetectionSegmentNotification
    {
      EntityID entityID = UnknownEntityID;
      sbio::SegmentID segmentID = UnknownSegmentID;
      MaterialID materialCode = UnknownMaterialID;
      float fIntersectionDistance = 0;
    };

    /**
     * @brief Reports an entity collision detected by a collision segment.
     *
     * Invariants:
     * - `contactedEntityID` identifies the contacted entity.
     */
    struct SCollisionDetectionSegmentEntityNotification
    {
      EntityID entityID = UnknownEntityID;
      EntityID contactedEntityID = UnknownEntityID;
      sbio::SegmentID segmentID = UnknownSegmentID;
      MaterialID materialCode = UnknownMaterialID;
      float fIntersectionDistance = 0;
    };

    /**
     * @brief Reports a non-entity collision detected by a collision volume.
     *
     * Invariants:
     * - `entityID` and `volumeID` identify the volume that generated the notification.
     */
    struct SCollisionDetectionVolumeNotification
    {
      EntityID entityID = UnknownEntityID;
      sbio::VolumeID volumeID = UnknownVolumeID;
      sbio::VolumeID contactedVolumeID = UnknownVolumeID;
    };

    /**
     * @brief Reports an entity collision detected by a collision volume.
     *
     * Invariants:
     * - `contactedEntityID` identifies the contacted entity.
     */
    struct SCollisionDetectionVolumeEntityNotification
    {
      EntityID entityID = UnknownEntityID;
      sbio::VolumeID volumeID = UnknownVolumeID;
      sbio::VolumeID contactedVolumeID = UnknownVolumeID;
      EntityID contactedEntityID = UnknownEntityID;
    };

    /**
     * @brief Reports that an entity animation has stopped.
     *
     * Invariants:
     * - `entityID` identifies the entity whose animation stopped.
     */
    struct SAnimationStopNotification
    {
      EntityID entityID = UnknownEntityID;
    };

    /**
     * @brief Reports an IG-defined event notification.
     *
     * Invariants:
     * - Event identifier and payload words are preserved exactly as emitted by the IG.
     */
    struct SEventNotification
    {
      ImageGeneratorEventID EventID = UnknownImageGeneratorEventID;
      uint32_t EventData1 = {0};
      uint32_t EventData2 = {0};
      uint32_t EventData3 = {0};
    };

    /**
     * @brief Reports an IG-defined message notification.
     *
     * Invariants:
     * - `MessageID` identifies the message kind.
     * - `sData` contains any implementation-defined message payload text.
     */
    struct SImageGeneratorNotification
    {
      ImageGeneratorMessageID MessageID = UnknownImageGeneratorMessageID;
      std::string sData;
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
