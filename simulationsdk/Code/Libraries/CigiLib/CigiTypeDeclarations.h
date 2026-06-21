//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiTypeDeclarations.h
 * @brief Forward declarations and type definitions for CIGI protocol integration.
 *
 * Provides forward declarations for CIGI protocol types, enums, and structures used throughout the simulation SDK.
 * Supports CIGI-to-SISO entity type conversion, protocol packet integration, and type-safe interoperability between
 * CIGI, engine, and entity systems. Reduces dependencies and improves compile times by avoiding unnecessary includes.
 *
 * @see sbio::cigi::CCigiEntityTypes
 * @see sbio::entity::SEntityType
 * @see sbio::ShortEntityTypeID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TYPE_DECLARATIONS_H
#define SIMBLOCKS_CIGI_TYPE_DECLARATIONS_H

#include <stdint.h>

namespace sbio
{
  struct MotionTrackerID;

  namespace cigi
  {
    struct CigiComponentClassID;

    struct SAtmosphere;
    struct SCelestialSphere;
    struct SChildEntityPosition;
    struct SCigiAnimationControl;
    struct SCigiEntityAcceleration;
    struct SCigiArticulatedPartAcceleration;
    struct SCigiArticulatedPart;
    struct SCigiArticulatedPartVelocityControl;
    struct SCigiAtmosphereControl;
    struct SCigiCelestialSphereControl;
    struct SCollisionDetectionCuboidDefinition;
    struct SCollisionDetectionSegmentDefinition;
    struct SCollisionDetectionSphereDefinition;
    struct SCigiComponentControl;
    struct SCigiConformalClampedEntityPosition;
    struct SCigiEarthReferenceModel;
    struct SCigiEntityVelocityControl;
    struct SCigiEnvironmentalRegion;
    struct SCigiIgControl;
    struct SCigiMaritimeSurfaceCondition;
    struct SCigiSensorControl;
    struct SCigiShortArticulatedPart;
    struct SCigiShortComponentControl;
    struct SCigiSpatialWeatherCondition;
    struct SCigiTerrestrialSurfaceCondition;
    struct SCigiViewControl;
    struct SCigiViewDefinition;
    struct SCigiWaveCondition;
    struct SCigiWeatherCondition;
    struct SEntityControl;
    struct SEnvironmentalConditionsRequest;
    struct SBaseHATHOTRequest;
    struct SHATHOTGlobalRequest;
    struct SHATHOTEntityRequest;
    struct SLineOfSightSegmentRequestGeodeticToGeodeticBasic;
    struct SLineOfSightSegmentRequestGeodeticToEntityBasic;
    struct SLineOfSightSegmentRequestEntityToGeodeticBasic;
    struct SLineOfSightSegmentRequestEntityToEntityBasic;
    struct SLineOfSightSegmentRequestGeodeticToGeodeticExtended;
    struct SLineOfSightSegmentRequestGeodeticToEntityExtended;
    struct SLineOfSightSegmentRequestEntityToGeodeticExtended;
    struct SLineOfSightSegmentRequestEntityToEntityExtended;
    struct SLineOfSightVectorRequestGeodeticBasic;
    struct SLineOfSightVectorRequestEntityBasic;
    struct SLineOfSightVectorRequestGeodeticExtended;
    struct SLineOfSightVectorRequestEntityExtended;
    struct STopLevelEntityPosition;
    struct SPositionRequest;
    struct SMotionTrackerViewControl;
    struct SMotionTrackerViewGroupControl;

    enum class ECigiOpCodeV3 : uint8_t;
    enum class ECigiOpCodeV4 : uint16_t;
    enum class ECigiScope;
    enum class ECigiVersion;

    enum class EActiveState;
    enum class EAnimationFramePositionReset;
    enum class EAnimationLoopMode;
    enum class EAnimationState;
    enum class EEarthReferenceModel;

    class CCigiEntityTypes;
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
