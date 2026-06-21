//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiTypes.h
 * @brief Declares types, enums, and strong types for CIGI protocol integration and simulation SDK interoperability.
 *
 * Provides strong type definitions, enums, and structures for CIGI protocol support, including packet headers, entity IDs,
 * coordinate systems, sensor and weather types, and protocol-specific enums. Supports conversion, configuration, and management
 * of CIGI protocol data for simulation and image generator interoperability.
 *
 * @see sbio::cigi namespace
 * @see CIGI::V40
 * @see CIGI::V33
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TYPES_H
#define SIMBLOCKS_CIGI_TYPES_H

#include "SymbolLib/SymbolTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include "MathLib/MathTypes.h"
#include <stdint.h>
#include <vector>

namespace sbio
{
  namespace cigi
  {
    /**
     * @brief Body-frame vector used by CIGI entity and component packets.
     *
     * The axis convention is forward, right, down, which forms a right-handed coordinate system.
     */
    STRONG_AXES(CigiBodyCoordinates, Forward, Right, Down);

    /**
     * @brief North-east-down vector used by CIGI position, rate, and offset data.
     *
     * The axis convention is north, east, down.
     */
    STRONG_AXES(CigiNEDCoordinates, North, East, Down);

    /** @brief Quaternion rotation expressed in the CIGI body-coordinate basis. */
    typedef SQuaternion<CigiBodyCoordinates> TCigiBodyRotation;

    /**
     * @brief Euler rotation expressed in the CIGI north-east-down basis.
     *
     * The yaw, pitch, and roll components use the axis basis encoded by `CigiNEDCoordinates`.
     */
    typedef SEulerRotation<CigiNEDCoordinates> TCigiNEDEulerRotation;

    /**
     * @brief Euler rotation expressed in the CIGI body-coordinate basis.
     *
     * The yaw, pitch, and roll components use the axis basis encoded by `CigiBodyCoordinates`.
     */
    typedef SEulerRotation<CigiBodyCoordinates> TCigiBodyEulerRotation;

    /** @brief Transform whose source and destination coordinate bases are both CIGI body coordinates. */
    typedef STransform<CigiBodyCoordinates, CigiBodyCoordinates> TCigiBodyTransform;

    /** @brief 3x3 matrix whose axis basis is CIGI body coordinates. */
    typedef StrongMat3<CigiBodyCoordinates> TCigiBodyMatrix;
  }

  /** @brief Strong identifier for a CIGI session. */
  STRONG_TYPE(SessionID, uint32_t);
  /** @brief Strong identifier for an animation. */
  STRONG_TYPE(AnimationID, uint16_t);
  /** @brief Strong identifier for an atmosphere control target. */
  STRONG_TYPE(AtmosphereID, uint32_t);
  /** @brief Strong identifier for a celestial sphere control target. */
  STRONG_TYPE(CelestialSphereID, uint32_t);
  /** @brief Strong identifier for a cloud type; `UINT8_MAX` represents the unknown value. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(CloudType, uint8_t, UINT8_MAX);
  /** @brief Strong identifier for a component event. */
  STRONG_TYPE(ComponentEventID, uint32_t);
  /** @brief Strong identifier for an entity-scoped wave. */
  STRONG_TYPE(EntityWaveID, uint8_t);
  /** @brief Strong identifier for a global layered weather object. */
  STRONG_TYPE(GlobalLayeredWeatherID, uint8_t);
  /** @brief Strong identifier for a global sea surface object. */
  STRONG_TYPE(GlobalSeaSurfaceID, uint32_t);
  /** @brief Strong identifier for a global terrain surface object. */
  STRONG_TYPE(GlobalTerrainSurfaceID, uint32_t);
  /** @brief Strong identifier for a global wave object. */
  STRONG_TYPE(GlobalWaveID, uint8_t);
  /** @brief Strong image-generator error code value. */
  STRONG_TYPE(ImageGeneratorErrorCode, uint8_t);
  /** @brief Strong identifier for a CIGI host. */
  STRONG_TYPE(HostID, uint32_t);
  /** @brief Strong identifier for a HAT/HOT request or response. */
  STRONG_TYPE(HATHOTID, uint16_t);
  /** @brief Strong identifier for a line-of-sight request. */
  STRONG_TYPE(LineOfSightRequestID, uint16_t);
  /** @brief Strong identifier for a motion tracker. */
  STRONG_TYPE(MotionTrackerID, uint8_t);
  /** @brief Strong identifier for a region. */
  STRONG_TYPE(RegionID, uint16_t);
  /** @brief Strong identifier for a regional layered weather object. */
  STRONG_TYPE(RegionalLayeredWeatherID, uint8_t);
  /** @brief Strong identifier for a regional terrain surface object. */
  STRONG_TYPE(RegionalTerrainSurfaceID, uint32_t);
  /** @brief Strong identifier for a regional wave object. */
  STRONG_TYPE(RegionalWaveID, uint8_t);
  /** @brief Strong identifier for a line-of-sight or collision segment. */
  STRONG_TYPE(SegmentID, uint8_t);
  /** @brief Strong identifier for a surface condition; `UINT16_MAX` represents the unknown value. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(SurfaceConditionID, uint16_t, UINT16_MAX);
  /** @brief Strong identifier for a collision volume. */
  STRONG_TYPE(VolumeID, uint8_t);
  /** @brief Strong weather severity value; `UINT8_MAX` represents the unknown value. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(WeatherSeverity, uint8_t, UINT8_MAX);
  /** @brief Strong temperature value in degrees Celsius; quiet NaN represents the unknown value. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(TemperatureCelsius, float, std::numeric_limits<float>::quiet_NaN());

  /**
   * @brief Stores horizontal and vertical wind speed components.
   *
   * This is a passive value type used by CIGI weather and environmental response structures.
   *
   * Invariants:
   * - `horizontalWindSpeed` and `verticalWindSpeed` are stored exactly as assigned.
   * - No unit conversion or range validation is performed by this type.
   */
  struct SWindSpeed
  {
    /// Horizontal wind speed component.
    float horizontalWindSpeed = 0;
    /// Vertical wind speed component.
    float verticalWindSpeed = 0;
  };

  namespace cigi
  {
    /** @brief Percentage value encoded as an integer constrained to the inclusive range 0 through 100. */
    RANGED_STRONG_INT(CigiIntPercentage, uint8_t, 0, 100);
    /** @brief Percentage value encoded as a float constrained to the inclusive range 0 through 100. */
    RANGED_STRONG_FLOAT(CigiFloatPercentage, float, 0, 100);
    /**
     * @brief Strong CIGI database number.
     *
     * The value is expected to match `DatabaseID` once a database is loaded and may be negative while loading.
     */
    STRONG_TYPE(CigiDatabaseNumber, int8_t);//will match DatabaseID once loaded. will be negative while loading.
    /** @brief Strong identifier for a CIGI component. */
    STRONG_TYPE(CigiComponentID, uint16_t);
    /** @brief Strong identifier for a CIGI component class. */
    STRONG_TYPE(CigiComponentClassID, uint8_t);
    /** @brief CIGI month value constrained to the inclusive range 1 through 12. */
    RANGED_STRONG_INT(CigiMonth, uint8_t, 1, 12);//cigi month is 1-based

    /** @brief Strong identifier for a sensor. */
    STRONG_TYPE(SensorID, uint8_t)
    /** @brief Sensor gain value constrained to the inclusive range 0 through 1. */
    RANGED_STRONG_FLOAT(SensorGain, float, 0, 1);
    /** @brief Sensor level value constrained to the inclusive range 0 through 1. */
    RANGED_STRONG_FLOAT(SensorLevel, float, 0, 1);
    /** @brief Sensor noise value constrained to the inclusive range 0 through 1. */
    RANGED_STRONG_FLOAT(SensorNoise, float, 0, 1);

    /**
     * @brief Describes one IG-reported error with an implementation-defined priority.
     *
     * This value type is used to carry one error code and its relative priority so callers can choose which
     * error to report when multiple conditions are present.
     *
     * Invariants:
     * - `nPriority` and `nErrorCode` are stored exactly as assigned.
     * - The type does not enforce any priority ordering rules.
     */
    struct SImageGeneratorError
    {
      /// Relative priority used by callers when ordering or selecting errors.
      int nPriority = 0;
      /// Image-generator error code associated with this entry.
      sbio::ImageGeneratorErrorCode nErrorCode = UnknownImageGeneratorErrorCode;
    };

    /**
     * @brief Identifies the CIGI protocol revision used for packet serialization and parsing.
     */
    enum class ECigiVersion
    {
      UNKNOWN_VERSION,
      VERSION_3_3,
      VERSION_4_0,
    };

    /**
     * @brief Enumerates the CIGI 3.3 opcodes supported by the SDK.
     *
     * The values map directly to the opcode field encoded in CIGI 3.x packet headers.
     */
    enum class ECigiOpCodeV3 : uint8_t
    {
      //HOST TO IG
      IG_CONTROL = 1,
      ENTITY_CONTROL = 2,
      CONFORMAL_CLAMPED_ENTITY_CONTROL = 3,
      COMPONENT_CONTROL = 4,
      SHORT_COMPONENT_CONTROL = 5,
      ARTICULATED_PART_CONTROL = 6,
      SHORT_ARTICULATED_PART_CONTROL = 7,
      RATE_CONTROL = 8,
      CELESTIAL_SPHERE_CONTROL = 9,
      ATMOSPHERE_CONTROL = 10,
      ENVIRONMENTAL_REGION_CONTROL = 11,
      WEATHER_CONTROL = 12,
      MARITIME_SURFACE_CONDITIONS_CONTROL = 13,
      WAVE_CONTROL = 14,
      TERRESTRIAL_SURFACE_CONDITIONS_CONTROL = 15,
      VIEW_CONTROL = 16,
      SENSOR_CONTROL = 17,
      MOTION_TRACKER_CONTROL = 18,
      EARTH_REFERENCE_MODEL_DEFINITION = 19,
      TRAJECTORY_DEFINITION = 20,
      VIEW_DEFINITION = 21,
      COLLISION_DETECTION_SEGMENT_DEFINITION = 22,
      COLLISION_DETECTION_VOLUME_DEFINITION = 23,
      HAT_HOT_REQUEST = 24,
      LINE_OF_SIGHT_SEGMENT_REQUEST = 25,
      LINE_OF_SIGHT_VECTOR_REQUEST = 26,
      POSITION_REQUEST = 27,
      ENVIRONMENTAL_CONDITIONS_REQUEST = 28,
      SYMBOL_SURFACE_DEFINITION = 29,
      SYMBOL_TEXT_DEFINITION = 30,
      SYMBOL_CIRCLE_DEFINITION = 31,
      SYMBOL_LINE_DEFINITION = 32,
      SYMBOL_CLONE = 33,
      SYMBOL_CONTROL = 34,
      SHORT_SYMBOL_CONTROL = 35,

      //IG TO HOST
      START_OF_FRAME = 101,
      HAT_HOT_RESPONSE = 102,
      HAT_HOT_EXTENDED_RESPONSE = 103,
      LINE_OF_SIGHT_RESPONSE = 104,
      LINE_OF_SIGHT_EXTENDED_RESPONSE = 105,
      SENSOR_RESPONSE = 106,
      SENSOR_EXTENDED_RESPONSE = 107,
      POSITION_RESPONSE = 108,
      WEATHER_CONDITIONS_RESPONSE = 109,
      AEROSOL_CONCENTRATION_RESPONSE = 110,
      MARITIME_SURFACE_CONDITIONS_RESPONSE = 111,
      TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE = 112,
      COLLISION_DETECTION_SEGMENT_NOTIFICATION = 113,
      COLLISION_DETECTION_VOLUME_NOTIFICATION = 114,
      ANIMATION_STOP_NOTIFICATION = 115,
      EVENT_NOTIFICATION = 116,
      IMAGE_GENERATOR_MESSAGE = 117,

      //EXTENSION PACKETS
    };

    /**
     * @brief Enumerates the CIGI 4.0 opcodes supported by the SDK.
     *
     * The values map directly to the opcode field encoded in CIGI 4.x packet headers.
     */
    enum class ECigiOpCodeV4 : uint16_t
    {
      //HOST TO IG
      IG_CONTROL = 0x00,
      ENTITY_POSITION = 0x01,
      CONFORMAL_CLAMPED_ENTITY_POSITION = 0x02,
      COMPONENT_CONTROL = 0x03,
      SHORT_COMPONENT_CONTROL = 0x04,
      ARTICULATED_PART_CONTROL = 0x05,
      SHORT_ARTICULATED_PART_CONTROL = 0x06,
      VELOCITY_CONTROL = 0x07,
      CELESTIAL_SPHERE_CONTROL = 0x08,
      ATMOSPHERE_CONTROL = 0x09,
      ENVIRONMENTAL_REGION_CONTROL = 0x0A,
      WEATHER_CONTROL = 0x0B,
      MARITIME_SURFACE_CONDITIONS_CONTROL = 0x0C,
      WAVE_CONTROL = 0x0D,
      TERRESTRIAL_SURFACE_CONDITIONS_CONTROL = 0x0E,
      VIEW_CONTROL = 0x0F,
      SENSOR_CONTROL = 0x10,
      MOTION_TRACKER_CONTROL = 0x11,
      EARTH_REFERENCE_MODEL_DEFINITION = 0x12,
      ACCELERATION_CONTROL = 0x13,
      VIEW_DEFINITION = 0x14,
      COLLISION_DETECTION_SEGMENT_DEFINITION = 0x15,
      COLLISION_DETECTION_VOLUME_DEFINITION = 0x16,
      HAT_HOT_REQUEST = 0x17,
      LINE_OF_SIGHT_SEGMENT_REQUEST = 0x18,
      LINE_OF_SIGHT_VECTOR_REQUEST = 0x19,
      POSITION_REQUEST = 0x1A,
      ENVIRONMENTAL_CONDITIONS_REQUEST = 0x1B,
      SYMBOL_SURFACE_DEFINITION = 0x1C,
      SYMBOL_TEXT_DEFINITION = 0x1D,
      SYMBOL_CIRCLE_DEFINITION = 0x1E,
      SYMBOL_POLYGON_DEFINITION = 0x1F,
      SYMBOL_CLONE = 0x20,
      SYMBOL_CONTROL = 0x21,
      SHORT_SYMBOL_CONTROL = 0x22,
      SYMBOL_TEXTURED_CIRICLE_DEFINITION = 0x23,
      SYMBOL_TEXTURED_POLYGON_DEFINITION = 0x24,
      ENTITY_CONTROL = 0x25,
      ANIMATION_CONTROL = 0x26,

      //IG TO HOST
      START_OF_FRAME = 0xFFFF,
      HAT_HOT_RESPONSE = 0x0FFF,
      HAT_HOT_EXTENDED_RESPONSE = 0x0FFE,
      LINE_OF_SIGHT_RESPONSE = 0x0FFD,
      LINE_OF_SIGHT_EXTENDED_RESPONSE = 0x0FFC,
      SENSOR_RESPONSE = 0x0FFB,
      SENSOR_EXTENDED_RESPONSE = 0x0FFA,
      POSITION_RESPONSE = 0x0FF9,
      WEATHER_CONDITIONS_RESPONSE = 0x0FF8,
      AEROSOL_CONCENTRATION_RESPONSE = 0x0FF7,
      MARITIME_SURFACE_CONDITIONS_RESPONSE = 0x0FF6,
      TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE = 0x0FF5,
      COLLISION_DETECTION_SEGMENT_NOTIFICATION = 0x0FF4,
      COLLISION_DETECTION_VOLUME_NOTIFICATION = 0x0FF3,
      ANIMATION_STOP_NOTIFICATION = 0x0FF2,
      EVENT_NOTIFICATION = 0x0FF1,
      IMAGE_GENERATOR_MESSAGE = 0x0FF0,

      //EXTENSION PACKETS
      FIRST_EXTENSION = 0xFFFE,
    };

    /**
     * @brief Stores the fixed header fields used by a CIGI 3.x packet.
     *
     * Invariants:
     * - `nPacketSize` is the size field carried on the wire for the corresponding packet payload.
     * - `eOpCode` identifies the packet type encoded by the header.
     */
    struct SCigiPacketHeaderV3
    {
      /// CIGI 3.x packet opcode.
      ECigiOpCodeV3 eOpCode = ECigiOpCodeV3::IG_CONTROL;
      /// CIGI 3.x packet size field.
      uint8_t nPacketSize = 0;
    };

    /**
     * @brief Stores the fixed header fields used by a CIGI 4.x packet.
     *
     * Invariants:
     * - `nPacketSize` is the size field carried on the wire for the corresponding packet payload.
     * - `eOpCode` identifies the packet type encoded by the header.
     */
    struct SCigiPacketHeaderV4
    {
      /// CIGI 4.x packet size field.
      uint16_t nPacketSize = 0;
      /// CIGI 4.x packet opcode.
      ECigiOpCodeV4 eOpCode = ECigiOpCodeV4::IG_CONTROL;
    };

    /**
     * @brief Describes how the host times outbound packets relative to start-of-frame packets.
     */
    enum class ECigiSynchronizationMode
    {
      UNKNOWN = -1,
      SYNCHRONOUS = 0,///< Host sends packets immediately after receiving a start-of-frame packet.
      ASYNCHRONOUS,///< Host packet processing is not synchronized to start-of-frame packet receipt; this may cause zero, one, or two host packets to be processed in a single frame.
    };

    /**
     * @brief High-level operating mode reported by or requested from an image generator.
     */
    enum class EIGMode
    {
      UNKNOWN = -1,
      RESET = 0,///< Reset mode; same as standby.
      OPERATE = 1,
      DEBUG = 2,
      MAINTENANCE = 3,///< Maintenance mode; can only be initiated by the image generator.
    };

    /**
     * @brief Identifies the application scope for environment and surface-control packets.
     */
    enum class ECigiScope
    {
      UNKNOWN = -1,
      GLOBAL = 0,
      REGIONAL = 1,
      ENTITY = 2,
    };

    /**
     * @brief Indicates whether an entity control packet uses the short or extended entity type form.
     */
    enum class EExtendedEntityType
    {
      UNKNOWN = -1,
      SHORT,
      EXTENDED,
    };

    /**
     * @brief Wave breaker shape classification used by maritime surface and wave state data.
     */
    enum class EWaveBreakerType
    {
      UNKNOWN = -1,
      PLUNGING,
      SPILLING,
      SURGING,
    };

    /**
     * @brief Active-state classification shared by CIGI-controlled objects.
     */
    enum class EActiveState
    {
      UNKNOWN = -1,
      INACTIVE_STANDBY,
      ACTIVE,
      DESTROYED,
    };

    /**
     * @brief Merge behavior requested for state updates that can be combined with previous values.
     */
    enum class EMergeState
    {
      UNKNOWN = -1,
      USE_LAST,
      MERGE,
    };

    /**
     * @brief Tracks database-loading state as reported by an image generator.
     */
    enum class EIGDatabaseState
    {
      UNKNOWN = -1,
      IG_CONTROLLED,///< Image generator controls database loading and host database requests are ignored.
      NO_DATABASE,///< No database has been requested for loading.
      WAIT_FOR_ACKNOWLEDGE,///< Waiting for acknowledgement before loading.
      LOADING,///< Image generator has begun loading a database.
      LOADING_FAILED,///< An error occurred while loading the database.
      LOADED,///< Database has been loaded by the image generator.
    };

    /**
     * @brief Sensor or tracker mode used by CIGI tracking controls and responses.
     * @note Values 5 through 7 are image-generator defined.
     */
    enum class ETrackMode
    {
      UNKNOWN = -1,
      OFF = 0,
      FORCE_CORRELATE,
      SCENE,
      TARGET,
      SHIP,
    };

    /**
     * @brief Sensor image polarity selection.
     */
    enum class EPolarity
    {
      UNKNOWN = -1,
      WHITE_HOT = 0,
      BLACK_HOT,
    };

    /**
     * @brief Sensor track gate color selection.
     */
    enum class ESensorTrack
    {
      UNKNOWN = -1,
      WHITE = 0,
      BLACK,
    };

    /**
     * @brief Identifies whether a view-related packet targets one view or a view group.
     */
    enum class EViewType
    {
      UNKNOWN = -1,
      VIEW = 0,
      VIEW_GROUP
    };

    /**
     * @brief Update cadence requested for values that can be sent once or continuously.
     */
    enum class EUpdateMode
    {
      UNKNOWN,
      ONE_SHOT,
      CONTINUOUS,
    };

    /**
     * @brief Object class used by CIGI attach, collision, and request/response data.
     */
    enum class EObjectClass
    {
      UNKNOWN = -1,
      ENTITY,
      ARTICULATED_PART,
      VIEW,
      VIEW_GROUP,
      MOTION_TRACKER,
    };

    /**
     * @brief Collision volume geometry type.
     */
    enum class EVolumeType
    {
      UNKNOWN = -1,
      SPHERE = 0,
      CUBOID,
    };

    /**
     * @brief HAT/HOT request type classification.
     */
    enum class ERequestType
    {
      UNKNOWN = -1,
      HAT = 0,
      HOT,
      EXTENDED,
    };

    /** @brief Response type classification using the same values as `ERequestType`. */
    typedef ERequestType EResponseType;

    /**
     * @brief Animation playback loop mode.
     */
    enum class EAnimationLoopMode
    {
      UNKNOWN = -1,
      ONE_SHOT,
      CONTINUOUS,
    };

    /**
     * @brief Animation playback state requested by animation control.
     */
    enum class EAnimationState
    {
      UNKNOWN = -1,
      STOP,
      PLAY,
      CONTINUE,
      PAUSE
    };

    /**
     * @brief Animation frame-position behavior when playback state changes.
     */
    enum class EAnimationFramePositionReset
    {
      UNKNOWN = -1,
      CONTINUE,
      RESET,
    };

    /**
     * @brief Captures the persistent control state associated with one entity.
     *
     * This structure groups the flags and parent relationship derived from entity control packets.
     *
     * Invariants:
     * - `m_ParentID` is meaningful only when `m_bHasParent` is `true`.
     * - All other members are stored exactly as assigned.
     */
    struct SEntityState
    {
      /// Active state associated with the entity.
      EActiveState m_eActiveState;
      /// Indicates whether `m_ParentID` contains a parent entity ID.
      bool m_bHasParent;
      /// Indicates whether alpha should be inherited from the parent entity.
      bool m_bInheritAlpha;
      /// Entity alpha value stored from control state.
      float m_Alpha;
      /// Indicates whether collision reporting is enabled for the entity.
      bool m_bCollisionReportingEnabled;
      /// Indicates whether smoothing is enabled for the entity.
      bool m_SmoothingEnabled;
      /// Parent entity ID when `m_bHasParent` is true; otherwise `UnknownEntityID` by default.
      EntityID m_ParentID = UnknownEntityID;
    };

    /**
     * @brief Stores global celestial sphere control state.
     *
     * Invariants:
     * - Members mirror the last applied control values exactly.
     * - No cross-field validation is performed.
     */
    struct SCelestialSphere
    {
      /// Indicates whether ephemeris behavior is enabled.
      bool bEphemerisEnabled = false;
      /// Indicates whether the sun is enabled.
      bool bSunEnabled = false;
      /// Indicates whether the moon is enabled.
      bool bMoonEnabled = false;
      /// Indicates whether stars are enabled.
      bool bStarsEnabled = false;
      /// Star intensity value.
      float fStarIntensity = 0;
    };

    /**
     * @brief Stores atmospheric state values used by CIGI environment control and response paths.
     *
     * Invariants:
     * - Members are stored exactly as assigned.
     * - This type does not enforce units or value ranges beyond the member types.
     */
    struct SAtmosphere
    {
      /// Humidity value.
      float fHumidity = 0;
      /// Air temperature in degrees Celsius.
      sbio::TemperatureCelsius fAirTemperature = sbio::UnknownTemperatureCelsius;
      /// Visibility range value.
      float fVisibilityRange = 0;
      /// Horizontal wind speed component.
      float fHorizontalWindSpeed = 0;
      /// Vertical wind speed component.
      float fVerticalWindSpeed = 0;
      /// Wind direction value.
      float fWindDirection = 0;
      /// Barometric pressure value.
      float fBarometricPressure = 0;
    };

    /**
     * @brief Sensor tracking status reported by sensor response data.
     */
    enum class ESensorStatus
    {
      UKNOWN = -1,
      SEARCHING_FOR_TARGET,
      TRACKING_TARGET,
      IMPENDING_BREAKLOCK,
      BREAKLOCK,
    };

    /**
     * @brief Earth reference model selection used by CIGI earth-reference model data.
     */
    enum class EEarthReferenceModel
    {
      UNKNOWN = -1,
      WGS84,
      HOST_DEFINED,
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
