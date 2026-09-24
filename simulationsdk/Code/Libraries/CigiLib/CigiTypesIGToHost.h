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
     */
    STRONG_TYPE(ImageGeneratorMessageID, uint16_t);

    /**
     * @brief Strong type for image-generator-defined event identifiers.
     *
     * The numeric assignments are IG-specific and are preserved exactly as reported by the IG.
     */
    STRONG_TYPE(ImageGeneratorEventID, uint16_t);

    // Start Of Frame
    /**
     * @brief Stores the IG state reported in a start-of-frame packet.
     */
    struct SCigiStartOfFrame
    {
      ECigiVersion eVersion = ECigiVersion::UNKNOWN_VERSION;///< Reported CIGI protocol version.
      CigiDatabaseNumber databaseID = UnknownCigiDatabaseNumber;///< Reported database number.
      EIGMode eIGMode = EIGMode::UNKNOWN;///< Current IG operating mode.
      bool bTimestampValid = false;///< Whether `microseconds` is valid.
      bool bEarthReferenceModel = false;///< Earth-reference-model flag reported by the IG.
      FrameNumber igFrameNumber = UnknownFrameNumber;///< Current IG frame number.
      Microsecond microseconds = UnknownMicrosecond;///< IG timestamp in microseconds.
      FrameNumber lastHostFrameNumber = UnknownFrameNumber;///< Last host frame reported by the IG.
      bool bOverframing = false;///< Whether the IG reports overframing.
      bool bPagingTerrain = false;///< Whether the IG reports terrain paging.
      bool bExcessiveVariableLengthData = false;///< Whether the IG reports excessive variable-length data.
    };

    /**
     * @brief Base data shared by HAT, HOT, and extended HAT/HOT responses.
     */
    struct SBaseHATHOTResponse
    {
      HATHOTID HATHOTID = UnknownHATHOTID;///< Identifier of the corresponding HAT/HOT request.
      uint64_t requestGeneration = 0;///< RequestGeneration from the engine request; not serialized.
      bool bValid = false;///< Whether the terrain result is valid.
      uint8_t hostFrameLSN = 0;///< Least significant nibble of the associated host frame number.
    };

    /**
     * @brief Reports height above terrain for a HAT request.
     */
    struct SHeightAboveTerrainResponse : SBaseHATHOTResponse
    {
      double heightAboveTerrain = 0;///< Test-point height above terrain; negative when below terrain.
    };

    /**
     * @brief Reports terrain height relative to the WGS84 ellipsoid for a HOT request.
     */
    struct SHeightOfTerrainResponse : SBaseHATHOTResponse
    {
      HeightRelativeToWGS84Ellipsoid heightOfTerrain = UnknownHeightRelativeToWGS84Ellipsoid;///< Terrain height above the WGS84 ellipsoid, in meters.
    };

    /**
     * @brief Reports the extended HAT/HOT response that carries both heights and surface details.
     * @note Surface fields are meaningful only when `bValid` is `true`.
     */
    struct SHATHOTExtendedResponse : SBaseHATHOTResponse
    {
      double heightAboveTerrain = 0;///< Test-point height above terrain; negative when below terrain.
      sbio::math::HeightRelativeToWGS84Ellipsoid heightOfTerrain = UnknownHeightRelativeToWGS84Ellipsoid;///< Terrain height above the WGS84 ellipsoid, in meters.
      sbio::MaterialID materialCode = UnknownMaterialID;///< Terrain material identifier.
      sbio::math::Degrees180 normalVectorAzimuth = UnknownDegrees180;///< Surface-normal azimuth, in degrees in [-180, 180].
      sbio::math::Degrees90 normalVectorElevation = UnknownDegrees90;///< Surface-normal elevation above the horizontal plane, in degrees in [-90, 90].
    };

    /**
     * @brief Stores the basic line-of-sight response payload.
     */
    struct SLineOfSightResponse
    {
      LineOfSightRequestID lineOfSightRequestID = UnknownLineOfSightRequestID;///< Identifier of the corresponding LOS request.
      uint64_t requestGeneration = 0;///< RequestGeneration from the engine request; not serialized.
      bool bValid = false;///< Whether the LOS result is valid.
      uint8_t hostFrameLSN = 0;///< Least significant nibble of the associated host frame number.
      uint8_t responseCount = 0;///< Response count reported for the request.
      double dRange = 0;///< Range reported by the LOS query.
    };

    /**
     * @brief Stores the basic line-of-sight response when the hit result identifies an entity.
     * @note `entityID` identifies the hit entity when `bValid` is `true`.
     */
    struct SLineOfSightEntityResponse
    {
      LineOfSightRequestID lineOfSightRequestID = UnknownLineOfSightRequestID;///< Identifier of the corresponding LOS request.
      uint64_t requestGeneration = 0;///< RequestGeneration from the engine request; not serialized.
      bool bValid = false;///< Whether the LOS result is valid.
      uint8_t hostFrameLSN = 0;///< Least significant nibble of the associated host frame number.
      uint8_t responseCount = 0;///< Response count reported for the request.
      double dRange = 0;///< Range reported by the LOS query.
      EntityID entityID = UnknownEntityID;///< Entity identified by the valid result.

      // The IG should set the visible parameter to false if the segment intersected one or more polygons before reaching the destination point.
      // If the LOS segment destination point is within the body of a target entity model,
      // then the IG should set this parameter to false and the Entity ID parameter to the ID of that entity.
      // The IG should set the visible parameter to true if the segment did not intersect with any polygons before reaching the destination point.
      bool bVisible = false;///< Whether the destination is visible without an intervening intersection.
    };

    /**
     * @brief Base data shared by extended line-of-sight responses.
     * @note `dRange` is meaningful only when `bRangeValid` is `true`.
     */
    struct SBaseLineOfSightExtendedResponse
    {
      LineOfSightRequestID lineOfSightRequestID = UnknownLineOfSightRequestID;///< Identifier of the corresponding LOS request.
      uint64_t requestGeneration = 0;///< RequestGeneration from the engine request; not serialized.
      bool bValid = false;///< Whether the intersection result is valid.
      bool bRangeValid = false;///< Whether `dRange` is valid.
      bool bVisible = false;///< Visibility reported by the LOS query.
      uint8_t hostFrameLSN = 0;///< Least significant nibble of the associated host frame number.
      uint8_t responseCount = 0;///< Response count reported for the request.
      double dRange = 0;///< Reported range; meaningful when `bRangeValid` is true.
      SColor32 surfaceColor;///< Color of the intersected surface.
      sbio::MaterialID materialCode = UnknownMaterialID;///< Material identifier of the intersected surface.
      float fNormalVectorAzimuth = {0};///< Azimuth of the surface normal.
      float fNormalVectorElevation = {0};///< Elevation of the surface normal.
    };

    /**
     * @brief Extended line-of-sight response whose hit location is expressed in geodetic coordinates.
     * Used when no entity is hit, regardless of the requested response coordinate system.
     */
    struct SLineOfSightExtendedGeodeticCoordinatesResponse : SBaseLineOfSightExtendedResponse
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;///< Geodetic hit position.
    };

    /**
     * @brief Extended entity-hit response whose hit location is expressed in geodetic coordinates.
     * Used when an entity is hit and geodetic response coordinates were requested.
     */
    struct SLineOfSightExtendedEntityGeodeticCoordinatesResponse : SBaseLineOfSightExtendedResponse
    {
      EntityID entityID = UnknownEntityID;///< Entity intersected by the query.
      sbio::math::SGeodeticCoordinates geodeticCoordinates;///< Geodetic hit position.
    };

    /**
     * @brief Extended line-of-sight response whose hit location is expressed relative to an entity.
     * Used when an entity is hit and entity-relative response coordinates were requested.
     */
    struct SLineOfSightExtendedEntityCoordinatesResponse : SBaseLineOfSightExtendedResponse
    {
      EntityID entityID = UnknownEntityID;///< Entity intersected by the query and defining the response frame.
      sbio::cigi::CigiBodyCoordinates offset;///< Hit position relative to that entity, in CIGI axes.
    };

    /**
     * @brief Reports the current state of a basic sensor response.
     */
    struct SSensorResponse
    {
      SensorID sensorID = UnknownSensorID;///< Reporting sensor identifier.
      ESensorStatus eSensorStatus = ESensorStatus::UKNOWN;///< Reported tracking status.
      ViewID viewID = UnknownViewID;///< View associated with the sensor.
      sbio::math::Vec2f gateSize;///< Tracking-gate dimensions.
      sbio::math::Vec2f gatePosition;///< Tracking-gate position.
      FrameNumber hostFrameNumber = UnknownFrameNumber;///< Host frame associated with the response.
    };

    /**
     * @brief Reports the current state of an extended sensor response using a geodetic track point.
     */
    struct SSensorExtendedResponse
    {
      ViewID viewID = UnknownViewID;///< View associated with the sensor.
      SensorID sensorID = UnknownSensorID;///< Reporting sensor identifier.
      ESensorStatus eSensorStatus = ESensorStatus::UKNOWN;///< Reported tracking status.
      sbio::math::Vec2f gateSize;///< Tracking-gate dimensions.
      sbio::math::Vec2f gatePosition;///< Tracking-gate position.
      FrameNumber hostFrameNumber = UnknownFrameNumber;///< Host frame associated with the response.
      sbio::math::SGeodeticCoordinates trackPoint;///< Geodetic location of the tracked point.
    };

    /**
     * @brief Reports the current state of an extended sensor response associated with an entity.
     */
    struct SSensorExtendedEntityResponse
    {
      ViewID viewID = UnknownViewID;///< View associated with the sensor.
      EntityID entityID = UnknownEntityID;///< Entity associated with the tracked point.
      SensorID sensorID = UnknownSensorID;///< Reporting sensor identifier.
      ESensorStatus eSensorStatus = ESensorStatus::UKNOWN;///< Reported tracking status.
      sbio::math::Vec2f gateSize;///< Tracking-gate dimensions.
      sbio::math::Vec2f gatePosition;///< Tracking-gate position.
      FrameNumber hostFrameNumber = UnknownFrameNumber;///< Host frame associated with the response.
      sbio::math::SGeodeticCoordinates trackPoint;///< Geodetic location of the tracked point.
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
     */
    struct SBasePositionResponse
    {
      EObjectClass eObjectClass = EObjectClass::UNKNOWN;///< Class of the reported object.
      uint16_t objectID = 0;///< Object identifier interpreted according to `eObjectClass`.
      ArticulatedPartID articulatedPartID = UnknownArticulatedPartID;///< Part identifier for an articulated-part response.
      sbio::cigi::TCigiBodyEulerRotation rotation;///< Reported CIGI Euler orientation in degrees.
    };

    /**
     * @brief Position response expressed in geodetic coordinates.
     */
    struct SPositionResponseGeodeticCoordinates : SBasePositionResponse
    {
      sbio::math::SGeodeticCoordinates geodeticCoordinates;///< Reported world position in geodetic coordinates.
    };

    /**
     * @brief Position response expressed as an offset from a parent entity.
     */
    struct SPositionResponseParentEntityCoordinates : SBasePositionResponse
    {
      CigiBodyCoordinates offset;///< Position relative to the parent entity, in CIGI axes.
    };

    /**
     * @brief Position response expressed as an offset from an articulated part.
     */
    struct SPositionResponseArticulatedPartCoordinates : SBasePositionResponse
    {
      sbio::cigi::CigiBodyCoordinates offset;///< Position relative to the articulated part, in CIGI axes.
    };

    /**
     * @brief Reports weather conditions at the location requested by the host.
     */
    struct SWeatherConditionsResponse
    {
      uint8_t requestID = 0;///< Identifier of the environmental conditions request.
      Percentage humidity = UnknownPercentage;///< Reported humidity.
      float fAirTemperature = 0;///< Reported air temperature.
      float fVisibilityRange = 0;///< Reported visibility range.
      SWindSpeed windSpeedHorVer = {0, 0};///< Horizontal and vertical wind-speed components.
      float fWindDirection = 0;///< Reported wind direction.
      float fBarometricPressure = 0;///< Reported barometric pressure.
    };

    /**
     * @brief Reports aerosol concentration for one weather layer.
     */
    struct SAerosolConcentrationResponse
    {
      uint8_t requestID = 0;///< Identifier of the environmental conditions request.
      uint8_t layerID = 0;///< Weather layer to which the concentration applies.
      float fAerosolConcentration = 0;///< Reported aerosol concentration.
    };

    /**
     * @brief Reports maritime surface conditions at the location requested by the host.
     */
    struct SMaritimeSurfaceConditionsResponse
    {
      uint8_t requestID = 0;///< Identifier of the environmental conditions request.
      sbio::math::HeightRelativeToWGS84Ellipsoid fSeaSurfaceHeight = sbio::math::UnknownHeightRelativeToWGS84Ellipsoid;///< Sea height above the WGS84 ellipsoid, in meters.
      sbio::TemperatureCelsius fSurfaceWaterTemperature = sbio::UnknownTemperatureCelsius;///< Surface-water temperature in degrees Celsius.
      Percentage surfaceClarity = UnknownPercentage;///< Water clarity; 100% denotes pristine water and 0% extremely turbid water.
    };

    /**
     * @brief Reports the terrestrial surface condition at the requested location.
     */
    struct STerrestrialSurfaceConditionsResponse
    {
      uint8_t requestID = 0;///< Identifier of the environmental conditions request.
      uint32_t surfaceConditionID = 0;///< Reported terrestrial surface condition identifier.
    };

    /**
     * @brief Reports a non-entity collision detected by a collision segment.
     */
    struct SCollisionDetectionSegmentNotification
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the collision segment.
      sbio::SegmentID segmentID = UnknownSegmentID;///< Segment that detected the collision.
      MaterialID materialCode = UnknownMaterialID;///< Material identifier of the contacted surface.
      float fIntersectionDistance = 0;///< Intersection distance reported along the segment.
    };

    /**
     * @brief Reports an entity collision detected by a collision segment.
     */
    struct SCollisionDetectionSegmentEntityNotification
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the collision segment.
      EntityID contactedEntityID = UnknownEntityID;///< Entity contacted by the segment.
      sbio::SegmentID segmentID = UnknownSegmentID;///< Segment that detected the collision.
      MaterialID materialCode = UnknownMaterialID;///< Material identifier of the contacted surface.
      float fIntersectionDistance = 0;///< Intersection distance reported along the segment.
    };

    /**
     * @brief Reports a non-entity collision detected by a collision volume.
     */
    struct SCollisionDetectionVolumeNotification
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the collision volume.
      sbio::VolumeID volumeID = UnknownVolumeID;///< Volume that detected the collision.
      sbio::VolumeID contactedVolumeID = UnknownVolumeID;///< Contacted volume identifier carried by the notification.
    };

    /**
     * @brief Reports an entity collision detected by a collision volume.
     */
    struct SCollisionDetectionVolumeEntityNotification
    {
      EntityID entityID = UnknownEntityID;///< Entity containing the collision volume.
      sbio::VolumeID volumeID = UnknownVolumeID;///< Volume that detected the collision.
      sbio::VolumeID contactedVolumeID = UnknownVolumeID;///< Contacted volume on the other entity.
      EntityID contactedEntityID = UnknownEntityID;///< Entity contacted by the volume.
    };

    /**
     * @brief Reports that an entity animation has stopped.
     */
    struct SAnimationStopNotification
    {
      EntityID entityID = UnknownEntityID;///< Entity whose animation stopped.
    };

    /**
     * @brief Reports an IG-defined event notification.
     */
    struct SEventNotification
    {
      ImageGeneratorEventID EventID = UnknownImageGeneratorEventID;///< IG-defined event identifier.
      uint32_t EventData1 = {0};///< First IG-defined event payload word.
      uint32_t EventData2 = {0};///< Second IG-defined event payload word.
      uint32_t EventData3 = {0};///< Third IG-defined event payload word.
    };

    /**
     * @brief Reports an IG-defined message notification.
     */
    struct SImageGeneratorNotification
    {
      ImageGeneratorMessageID MessageID = UnknownImageGeneratorMessageID;///< IG-defined message identifier.
      std::string sData;///< Message payload retained as a string.
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
