//Copyright SimBlocks LLC 2016-2026
#include "CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include <string>
#include <unordered_map>

using namespace std;
using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::symbol;

typedef unordered_map<ECigiOpCodeV3, string> TCigiOpCodeNamesV3;
typedef unordered_map<ECigiOpCodeV4, string> TCigiOpCodeNamesV4;
TCigiOpCodeNamesV3 OpCodeNamesV3;
TCigiOpCodeNamesV4 OpCodeNamesV4;

namespace sbio
{
  namespace cigi
  {
    std::string GetCigiOpCodeName(const ECigiOpCodeV3& eOpCode)
    {
      TCigiOpCodeNamesV3::const_iterator it = OpCodeNamesV3.find(eOpCode);
      if (it == OpCodeNamesV3.end())
      {
        return "";
      }

      return it->second;
    }

    std::string GetCigiOpCodeName(const ECigiOpCodeV4& eOpCode)
    {
      TCigiOpCodeNamesV4::const_iterator it = OpCodeNamesV4.find(eOpCode);
      if (it == OpCodeNamesV4.end())
      {
        return "";
      }

      return it->second;
    }

    void SetupCigiOpCodeNamesV3()
    {
      //HOST TO IG
      OpCodeNamesV3[ECigiOpCodeV3::IG_CONTROL] = "IG_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::ENTITY_CONTROL] = "ENTITY_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::CONFORMAL_CLAMPED_ENTITY_CONTROL] = "CONFORMAL_CLAMPED_ENTITY_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::COMPONENT_CONTROL] = "COMPONENT_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::SHORT_COMPONENT_CONTROL] = "SHORT_COMPONENT_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::ARTICULATED_PART_CONTROL] = "ARTICULATED_PART_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::SHORT_ARTICULATED_PART_CONTROL] = "SHORT_ARTICULATED_PART_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::RATE_CONTROL] = "RATE_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::CELESTIAL_SPHERE_CONTROL] = "CELESTIAL_SPHERE_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::ATMOSPHERE_CONTROL] = "ATMOSPHERE_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::ENVIRONMENTAL_REGION_CONTROL] = "ENVIRONMENTAL_REGION_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::WEATHER_CONTROL] = "WEATHER_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::MARITIME_SURFACE_CONDITIONS_CONTROL] = "MARITIME_SURFACE_CONDITIONS_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::WAVE_CONTROL] = "WAVE_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL] = "TERRESTRIAL_SURFACE_CONDITIONS_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::VIEW_CONTROL] = "VIEW_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::SENSOR_CONTROL] = "SENSOR_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::MOTION_TRACKER_CONTROL] = "MOTION_TRACKER_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::EARTH_REFERENCE_MODEL_DEFINITION] = "EARTH_REFERENCE_MODEL_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::TRAJECTORY_DEFINITION] = "TRAJECTORY_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::VIEW_DEFINITION] = "VIEW_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_DEFINITION] = "COLLISION_DETECTION_SEGMENT_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_DEFINITION] = "COLLISION_DETECTION_VOLUME_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::HAT_HOT_REQUEST] = "HAT_HOT_REQUEST";
      OpCodeNamesV3[ECigiOpCodeV3::LINE_OF_SIGHT_SEGMENT_REQUEST] = "LINE_OF_SIGHT_SEGMENT_REQUEST";
      OpCodeNamesV3[ECigiOpCodeV3::LINE_OF_SIGHT_VECTOR_REQUEST] = "LINE_OF_SIGHT_VECTOR_REQUEST";
      OpCodeNamesV3[ECigiOpCodeV3::POSITION_REQUEST] = "POSITION_REQUEST";
      OpCodeNamesV3[ECigiOpCodeV3::ENVIRONMENTAL_CONDITIONS_REQUEST] = "ENVIRONMENTAL_CONDITIONS_REQUEST";
      OpCodeNamesV3[ECigiOpCodeV3::SYMBOL_SURFACE_DEFINITION] = "SYMBOL_SURFACE_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::SYMBOL_TEXT_DEFINITION] = "SYMBOL_TEXT_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::SYMBOL_CIRCLE_DEFINITION] = "SYMBOL_CIRCLE_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::SYMBOL_LINE_DEFINITION] = "SYMBOL_LINE_DEFINITION";
      OpCodeNamesV3[ECigiOpCodeV3::SYMBOL_CLONE] = "SYMBOL_CLONE";
      OpCodeNamesV3[ECigiOpCodeV3::SYMBOL_CONTROL] = "SYMBOL_CONTROL";
      OpCodeNamesV3[ECigiOpCodeV3::SHORT_SYMBOL_CONTROL] = "SHORT_SYMBOL_CONTROL";

      //IG TO HOST
      OpCodeNamesV3[ECigiOpCodeV3::START_OF_FRAME] = "START_OF_FRAME";
      OpCodeNamesV3[ECigiOpCodeV3::HAT_HOT_RESPONSE] = "HAT_HOT_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::HAT_HOT_EXTENDED_RESPONSE] = "HAT_HOT_EXTENDED_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::LINE_OF_SIGHT_RESPONSE] = "LINE_OF_SIGHT_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::LINE_OF_SIGHT_EXTENDED_RESPONSE] = "LINE_OF_SIGHT_EXTENDED_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::SENSOR_RESPONSE] = "SENSOR_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::SENSOR_EXTENDED_RESPONSE] = "SENSOR_EXTENDED_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::POSITION_RESPONSE] = "POSITION_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::WEATHER_CONDITIONS_RESPONSE] = "WEATHER_CONDITIONS_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::AEROSOL_CONCENTRATION_RESPONSE] = "AEROSOL_CONCENTRATION_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::MARITIME_SURFACE_CONDITIONS_RESPONSE] = "MARITIME_SURFACE_CONDITIONS_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE] = "TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE";
      OpCodeNamesV3[ECigiOpCodeV3::COLLISION_DETECTION_SEGMENT_NOTIFICATION] = "COLLISION_DETECTION_SEGMENT_NOTIFICATION";
      OpCodeNamesV3[ECigiOpCodeV3::COLLISION_DETECTION_VOLUME_NOTIFICATION] = "COLLISION_DETECTION_VOLUME_NOTIFICATION";
      OpCodeNamesV3[ECigiOpCodeV3::ANIMATION_STOP_NOTIFICATION] = "ANIMATION_STOP_NOTIFICATION";
      OpCodeNamesV3[ECigiOpCodeV3::EVENT_NOTIFICATION] = "EVENT_NOTIFICATION";
      OpCodeNamesV3[ECigiOpCodeV3::IMAGE_GENERATOR_MESSAGE] = "IMAGE_GENERATOR_MESSAGE";
    }

    void SetupCigiOpCodeNamesV4()
    {
      //HOST TO IG
      OpCodeNamesV4[ECigiOpCodeV4::IG_CONTROL] = "IG_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::ENTITY_POSITION] = "ENTITY_POSITION";
      OpCodeNamesV4[ECigiOpCodeV4::CONFORMAL_CLAMPED_ENTITY_POSITION] = "CONFORMAL_CLAMPED_ENTITY_POSITION";
      OpCodeNamesV4[ECigiOpCodeV4::COMPONENT_CONTROL] = "COMPONENT_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::SHORT_COMPONENT_CONTROL] = "SHORT_COMPONENT_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::ARTICULATED_PART_CONTROL] = "ARTICULATED_PART_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::SHORT_ARTICULATED_PART_CONTROL] = "SHORT_ARTICULATED_PART_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::VELOCITY_CONTROL] = "VELOCITY_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::CELESTIAL_SPHERE_CONTROL] = "CELESTIAL_SPHERE_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::ATMOSPHERE_CONTROL] = "ATMOSPHERE_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::ENVIRONMENTAL_REGION_CONTROL] = "ENVIRONMENTAL_REGION_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::WEATHER_CONTROL] = "WEATHER_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_CONTROL] = "MARITIME_SURFACE_CONDITIONS_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::WAVE_CONTROL] = "WAVE_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_CONTROL] = "TERRESTRIAL_SURFACE_CONDITIONS_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::VIEW_CONTROL] = "VIEW_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::SENSOR_CONTROL] = "SENSOR_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::MOTION_TRACKER_CONTROL] = "MOTION_TRACKER_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::EARTH_REFERENCE_MODEL_DEFINITION] = "EARTH_REFERENCE_MODEL_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::ACCELERATION_CONTROL] = "ACCELERATION_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::VIEW_DEFINITION] = "VIEW_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_DEFINITION] = "COLLISION_DETECTION_SEGMENT_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_DEFINITION] = "COLLISION_DETECTION_VOLUME_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::HAT_HOT_REQUEST] = "HAT_HOT_REQUEST";
      OpCodeNamesV4[ECigiOpCodeV4::LINE_OF_SIGHT_SEGMENT_REQUEST] = "LINE_OF_SIGHT_SEGMENT_REQUEST";
      OpCodeNamesV4[ECigiOpCodeV4::LINE_OF_SIGHT_VECTOR_REQUEST] = "LINE_OF_SIGHT_VECTOR_REQUEST";
      OpCodeNamesV4[ECigiOpCodeV4::POSITION_REQUEST] = "POSITION_REQUEST";
      OpCodeNamesV4[ECigiOpCodeV4::ENVIRONMENTAL_CONDITIONS_REQUEST] = "ENVIRONMENTAL_CONDITIONS_REQUEST";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_SURFACE_DEFINITION] = "SYMBOL_SURFACE_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_TEXT_DEFINITION] = "SYMBOL_TEXT_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_CIRCLE_DEFINITION] = "SYMBOL_CIRCLE_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_POLYGON_DEFINITION] = "SYMBOL_POLYGON_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_CLONE] = "SYMBOL_CLONE";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_CONTROL] = "SYMBOL_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::SHORT_SYMBOL_CONTROL] = "SHORT_SYMBOL_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_TEXTURED_CIRICLE_DEFINITION] = "SYMBOL_TEXTURED_CIRICLE_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::SYMBOL_TEXTURED_POLYGON_DEFINITION] = "SYMBOL_TEXTURED_POLYGON_DEFINITION";
      OpCodeNamesV4[ECigiOpCodeV4::ENTITY_CONTROL] = "ENTITY_CONTROL";
      OpCodeNamesV4[ECigiOpCodeV4::ANIMATION_CONTROL] = "ANIMATION_CONTROL";

      //IG TO HOST
      OpCodeNamesV4[ECigiOpCodeV4::START_OF_FRAME] = "START_OF_FRAME";
      OpCodeNamesV4[ECigiOpCodeV4::HAT_HOT_RESPONSE] = "HAT_HOT_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::HAT_HOT_EXTENDED_RESPONSE] = "HAT_HOT_EXTENDED_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::LINE_OF_SIGHT_RESPONSE] = "LINE_OF_SIGHT_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::LINE_OF_SIGHT_EXTENDED_RESPONSE] = "LINE_OF_SIGHT_EXTENDED_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::SENSOR_RESPONSE] = "SENSOR_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::SENSOR_EXTENDED_RESPONSE] = "SENSOR_EXTENDED_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::POSITION_RESPONSE] = "POSITION_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::WEATHER_CONDITIONS_RESPONSE] = "WEATHER_CONDITIONS_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::AEROSOL_CONCENTRATION_RESPONSE] = "AEROSOL_CONCENTRATION_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::MARITIME_SURFACE_CONDITIONS_RESPONSE] = "MARITIME_SURFACE_CONDITIONS_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE] = "TERRESTRIAL_SURFACE_CONDITIONS_RESPONSE";
      OpCodeNamesV4[ECigiOpCodeV4::COLLISION_DETECTION_SEGMENT_NOTIFICATION] = "COLLISION_DETECTION_SEGMENT_NOTIFICATION";
      OpCodeNamesV4[ECigiOpCodeV4::COLLISION_DETECTION_VOLUME_NOTIFICATION] = "COLLISION_DETECTION_VOLUME_NOTIFICATION";
      OpCodeNamesV4[ECigiOpCodeV4::ANIMATION_STOP_NOTIFICATION] = "ANIMATION_STOP_NOTIFICATION";
      OpCodeNamesV4[ECigiOpCodeV4::EVENT_NOTIFICATION] = "EVENT_NOTIFICATION";
      OpCodeNamesV4[ECigiOpCodeV4::IMAGE_GENERATOR_MESSAGE] = "IMAGE_GENERATOR_MESSAGE";
    }

    void SetupCigiOpCodeNames()
    {
      SetupCigiOpCodeNamesV3();
      SetupCigiOpCodeNamesV4();
    }

    CIGI::V40::EntityCtrl::ExtendedEntityType ConvertToExtendedEntityType(EExtendedEntityType type)
    {
      if (type == EExtendedEntityType::SHORT)
      {
        return CIGI::V40::EntityCtrl::ExtendedEntityType::eExtendedEntityType_Short;
      }
      return CIGI::V40::EntityCtrl::ExtendedEntityType::eExtendedEntityType_Extended;
    }

    std::string ConvertCigiExtendedEntityTypeToString(const EExtendedEntityType& type)
    {
      if (type == EExtendedEntityType::SHORT)
      {
        return "Short";
      }
      else if (type == EExtendedEntityType::EXTENDED)
      {
        return "Extended";
      }
      return "Unknown";
    }

    ETopLevelCoordinateSystem ConvertCigiStringToCoordinateSystem(const std::string& sCoordinateSystem)
    {
      if (sCoordinateSystem == "Geodetic")
      {
        return ETopLevelCoordinateSystem::GEODETIC;
      }
      else if (sCoordinateSystem == "Entity")
      {
        return ETopLevelCoordinateSystem::ENTITY;
      }

      return ETopLevelCoordinateSystem::UNKNOWN;
    }

    std::string ConvertCigiCoordinateSystemToString(const ETopLevelCoordinateSystem& eCoordinateSystem)
    {
      if (eCoordinateSystem == ETopLevelCoordinateSystem::GEODETIC)
      {
        return "Geodetic";
      }
      else if (eCoordinateSystem == ETopLevelCoordinateSystem::ENTITY)
      {
        return "Entity";
      }
      return "Unknown";
    }

    sbio::EDegreeOfFreedom ConvertCigiStringToDegreeOfFreedom(const std::string& sDegreeOfFreedom)
    {
      if (sDegreeOfFreedom == "Not Used")
      {
        return EDegreeOfFreedom::NOT_USED;
      }
      else if (sDegreeOfFreedom == "X Offset")
      {
        return EDegreeOfFreedom::X_OFFSET;
      }
      else if (sDegreeOfFreedom == "Y Offset")
      {
        return EDegreeOfFreedom::Y_OFFSET;
      }
      else if (sDegreeOfFreedom == "Z Offset")
      {
        return EDegreeOfFreedom::Z_OFFSET;
      }
      else if (sDegreeOfFreedom == "Yaw")
      {
        return EDegreeOfFreedom::YAW;
      }
      else if (sDegreeOfFreedom == "Pitch")
      {
        return EDegreeOfFreedom::PITCH;
      }
      else if (sDegreeOfFreedom == "Roll")
      {
        return EDegreeOfFreedom::ROLL;
      }

      return EDegreeOfFreedom::UNKNOWN;
    }

    sbio::cigi::EActiveState ConvertCigiStringToActiveState(const std::string& sActiveState)
    {
      if (sActiveState == "Inactive")
      {
        return EActiveState::INACTIVE_STANDBY;
      }
      else if (sActiveState == "Standby")
      {
        return EActiveState::INACTIVE_STANDBY;
      }
      else if (sActiveState == "Active")
      {
        return EActiveState::ACTIVE;
      }
      else if (sActiveState == "Destroyed")
      {
        return EActiveState::DESTROYED;
      }

      return EActiveState::UNKNOWN;
    }

    std::string ConvertCigiActiveStateToString(const EActiveState& eActiveState)
    {
      if (eActiveState == EActiveState::INACTIVE_STANDBY)
      {
        return "Inactive";
      }
      else if (eActiveState == EActiveState::ACTIVE)
      {
        return "Active";
      }
      else if (eActiveState == EActiveState::DESTROYED)
      {
        return "Destroyed";
      }
      return "Unknown";
    }

    sbio::cigi::EMergeState ConvertCigiStringToMergeState(const std::string& sMergeState)
    {
      if (sMergeState == "UseLast")
      {
        return EMergeState::USE_LAST;
      }
      else if (sMergeState == "Use Last")
      {
        return EMergeState::USE_LAST;
      }
      else if (sMergeState == "Merge")
      {
        return EMergeState::MERGE;
      }

      return EMergeState::UNKNOWN;
    }

    std::string ConvertCigiMergeStateToString(const EMergeState& eMergeState)
    {
      if (eMergeState == EMergeState::USE_LAST)
      {
        return "Use Last";
      }
      else if (eMergeState == EMergeState::MERGE)
      {
        return "Merge";
      }
      return "Unknown";
    }

    std::string ConvertCigiLayerIDToString(const uint8_t value)
    {
      if (value == 0)
      {
        return "Ground Fog";
      }
      else if (value == 1)
      {
        return "Cloud Layer 1";
      }
      else if (value == 2)
      {
        return "Cloud Layer 2";
      }
      else if (value == 3)
      {
        return "Cloud Layer 3";
      }
      else if (value == 4)
      {
        return "Rain";
      }
      else if (value == 5)
      {
        return "Snow";
      }
      else if (value == 6)
      {
        return "Sleet";
      }
      else if (value == 7)
      {
        return "Hail";
      }
      else if (value == 8)
      {
        return "Sand";
      }
      else if (value == 9)
      {
        return "Dust";
      }
      else if (value >= 10 && value <= 255)
      {
        return "Defined by IG";
      }
      else
      {
        return "Unknown";
      }
    }

    EObjectCoordinateSystem ConvertCigiStringToObjectCoordinateSystem(const std::string& sCoordinateSystem)
    {
      if (sCoordinateSystem == "Geodetic" || sCoordinateSystem == "World")
      {
        return EObjectCoordinateSystem::WORLD;
      }
      else if (sCoordinateSystem == "World/Parent" || sCoordinateSystem == "Parent Entity" || sCoordinateSystem == "Parent")
      {
        return EObjectCoordinateSystem::PARENT;
      }
      else if (sCoordinateSystem == "Local" || sCoordinateSystem == "Submodel")
      {
        return EObjectCoordinateSystem::LOCAL;
      }

      return EObjectCoordinateSystem::UNKNOWN;
    }

    std::string ConvertCigiObCoordSysToString(const EObjectCoordinateSystem& eCoordinateSystem)
    {
      if (eCoordinateSystem == EObjectCoordinateSystem::WORLD)
      {
        return "World";
      }
      else if (eCoordinateSystem == EObjectCoordinateSystem::PARENT)
      {
        return "Parent";
      }
      else if (eCoordinateSystem == EObjectCoordinateSystem::LOCAL)
      {
        return "Local";
      }
      return "Unknown";
    }

    sbio::cigi::EAnimationFramePositionReset ConvertCigiStringToFramePositionReset(const std::string& sAnimationFramePositionReset)
    {
      if (sAnimationFramePositionReset == "Continue")
      {
        return EAnimationFramePositionReset::CONTINUE;
      }
      else if (sAnimationFramePositionReset == "Reset")
      {
        return EAnimationFramePositionReset::RESET;
      }
      else
      {
        return EAnimationFramePositionReset::UNKNOWN;
      }
    }

    std::string ConvertCigiAnimationFramePositionResetToString(const EAnimationFramePositionReset& eAnimationFramePositionReset)
    {
      if (eAnimationFramePositionReset == EAnimationFramePositionReset::CONTINUE)
      {
        return "Continue";
      }
      else if (eAnimationFramePositionReset == EAnimationFramePositionReset::RESET)
      {
        return "Reset";
      }
      return "Unknown";
    }

    sbio::cigi::EAnimationState ConvertCigiStringToAnimationState(const std::string& sAnimationState)
    {
      if (sAnimationState == "Play")
      {
        return EAnimationState::PLAY;
      }
      else if (sAnimationState == "Stop")
      {
        return EAnimationState::STOP;
      }
      else if (sAnimationState == "Continue")
      {
        return EAnimationState::CONTINUE;
      }
      else if (sAnimationState == "Pause")
      {
        return EAnimationState::PAUSE;
      }
      else
      {
        return EAnimationState::UNKNOWN;
      }
    }

    std::string ConvertCigiAnimationStateToString(const EAnimationState& eAnimationState)
    {
      if (eAnimationState == EAnimationState::STOP)
      {
        return "Stop";
      }
      else if (eAnimationState == EAnimationState::PLAY)
      {
        return "Play";
      }
      else if (eAnimationState == EAnimationState::CONTINUE)
      {
        return "Continue";
      }
      else if (eAnimationState == EAnimationState::PAUSE)
      {
        return "Pause";
      }
      return "Unknown";
    }

    sbio::cigi::EAnimationLoopMode ConvertCigiStringToAnimationLoopMode(const std::string& sAnimationLoopMode)
    {
      if (sAnimationLoopMode == "One-Shot")
      {
        return EAnimationLoopMode::ONE_SHOT;
      }
      else if (sAnimationLoopMode == "Continuous")
      {
        return EAnimationLoopMode::CONTINUOUS;
      }
      else
      {
        return EAnimationLoopMode::UNKNOWN;
      }
    }

    std::string ConvertCigiAnimationLoopModeToString(const EAnimationLoopMode& eAnimationLoopMode)
    {
      if (eAnimationLoopMode == EAnimationLoopMode::ONE_SHOT)
      {
        return "One-Shot";
      }
      else if (eAnimationLoopMode == EAnimationLoopMode::CONTINUOUS)
      {
        return "Continuous";
      }
      return "Unknown";
    }

    sbio::cigi::ERequestType ConvertCigiStringToRequestType(const std::string& sRequestType)
    {
      if (sRequestType == "HAT")
      {
        return ERequestType::HAT;
      }
      else if (sRequestType == "HOT")
      {
        return ERequestType::HOT;
      }
      else if (sRequestType == "Extended")
      {
        return ERequestType::EXTENDED;
      }

      return ERequestType::UNKNOWN;
    }

    std::string ConvertCigiRequestTypeToString(const ERequestType& eRequestType)
    {
      if (eRequestType == ERequestType::HAT)
      {
        return "HAT";
      }
      else if (eRequestType == ERequestType::HOT)
      {
        return "HOT";
      }
      else if (eRequestType == ERequestType::EXTENDED)
      {
        return "Extended";
      }
      return "Unknown";
    }

    std::string ConvertResponseTypeToString(CIGI::V40::HATHOTResponse::Type eResponseType)
    {
      if (eResponseType == CIGI::V40::HATHOTResponse::Type::eType_HAT)
      {
        return "HAT";
      }
      else if (eResponseType == CIGI::V40::HATHOTResponse::Type::eType_HOT)
      {
        return "HOT";
      }
      return "Unknown";
    }

    /// <summary>
    /// Converts the EScope enum to the ScopeGrp enum
    /// Default value is Global
    /// </summary>
    CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope ConvertToTerrestrialSurfaceScope(ECigiScope scope)
    {
      if (scope == ECigiScope::GLOBAL)
      {
        return CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Global;
      }
      else if (scope == ECigiScope::REGIONAL)
      {
        return CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Regional;
      }
      else if (scope == ECigiScope::ENTITY)
      {
        return CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Entity;
      }

      return CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Global;
    }

    sbio::cigi::EAnimationFramePositionReset ToAnimationFramePositionReset(const std::string& sAnimationFramePositionReset)
    {
      if (sAnimationFramePositionReset == "CONTINUE")
      {
        return EAnimationFramePositionReset::CONTINUE;
      }
      else if (sAnimationFramePositionReset == "RESET")
      {
        return EAnimationFramePositionReset::RESET;
      }
      else
      {
        return EAnimationFramePositionReset::UNKNOWN;
      }
    }

    CIGI::V40::EntityPosition::GroundOceanClamp ConvertToClamp(sbio::EClamp eClamp)
    {
      switch (eClamp)
      {
      default:
      case EClamp::UNKNOWN:
      {
        return CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_NoClamp;
      }
      case EClamp::NONE:
      {
        return CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_NoClamp;
      }
      case EClamp::CONFORMAL:
      {
        return CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_Conformal;
      }
      case EClamp::NON_CONFORMAL:
      {
        return CIGI::V40::EntityPosition::GroundOceanClamp::eGroundOceanClamp_NonConformal;
      }
      }
    }

    CIGI::V40::ShortArticulatedPartCtrl::DOFSelect ConvertToDOFSelect(sbio::EDegreeOfFreedom eDOF)
    {
      switch (eDOF)
      {
      default:
      case EDegreeOfFreedom::UNKNOWN:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_NotUsed;
      }
      case EDegreeOfFreedom::NOT_USED:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_NotUsed;
      }
      case EDegreeOfFreedom::YAW:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_Yaw;
      }
      case EDegreeOfFreedom::PITCH:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_Pitch;
      }
      case EDegreeOfFreedom::ROLL:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_Roll;
      }
      case EDegreeOfFreedom::X_OFFSET:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_XOffset;
      }
      case EDegreeOfFreedom::Y_OFFSET:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_YOffset;
      }
      case EDegreeOfFreedom::Z_OFFSET:
      {
        return CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_ZOffset;
      }
      }
    }

    std::string ConvertCigiDOFSelectToString(sbio::EDegreeOfFreedom eDOF)
    {
      if (eDOF == EDegreeOfFreedom::NOT_USED)
      {
        return "Not Used";
      }
      else if (eDOF == EDegreeOfFreedom::X_OFFSET)
      {
        return "X Offset";
      }
      else if (eDOF == EDegreeOfFreedom::Y_OFFSET)
      {
        return "Y Offset";
      }
      else if (eDOF == EDegreeOfFreedom::Z_OFFSET)
      {
        return "Z Offset";
      }
      else if (eDOF == EDegreeOfFreedom::YAW)
      {
        return "Yaw";
      }
      else if (eDOF == EDegreeOfFreedom::PITCH)
      {
        return "Pitch";
      }
      else if (eDOF == EDegreeOfFreedom::ROLL)
      {
        return "Roll";
      }
      return "Unknown";
    }

    sbio::cigi::EIGMode ConvertToIGMode(CIGI::V40::IGCtrl::IGMode eIGMode)
    {
      switch (eIGMode)
      {
      default:
        return EIGMode::UNKNOWN;
      case CIGI::V40::IGCtrl::IGMode::eIGMode_Reset:
        return EIGMode::RESET;
      case CIGI::V40::IGCtrl::IGMode::eIGMode_Debug:
        return EIGMode::DEBUG;
      case CIGI::V40::IGCtrl::IGMode::eIGMode_Operate:
        return EIGMode::OPERATE;
      }
    }

    sbio::cigi::EIGMode ConvertToIGMode(CIGI::V40::SoF::IGMode eIGMode)
    {
      switch (eIGMode)
      {
      default:
        return EIGMode::UNKNOWN;
      case CIGI::V40::SoF::IGMode::eIGMode_Reset:
        return EIGMode::RESET;
      case CIGI::V40::SoF::IGMode::eIGMode_Debug:
        return EIGMode::DEBUG;
      case CIGI::V40::SoF::IGMode::eIGMode_Operate:
        return EIGMode::OPERATE;
      case CIGI::V40::SoF::IGMode::eIGMode_OfflineMaintenance:
        return EIGMode::MAINTENANCE;
      }
    }

    std::string ConvertToString(EIGMode eIGMode)
    {
      switch (eIGMode)
      {
      default:
      case EIGMode::UNKNOWN:
      {
        return "UNKNOWN";
      }
      case EIGMode::DEBUG:
      {
        return "DEBUG";
      }
      case EIGMode::OPERATE:
      {
        return "OPERATE";
      }
      case EIGMode::RESET:
      {
        return "RESET";
      }
      case EIGMode::MAINTENANCE:
      {
        return "MAINTENANCE";
      }
      }
    }

    EIGMode ConvertCigiStringToIGMode(const std::string& s)
    {
      if (s == "Reset" || s == "Standby" || s == "RESET")
      {
        return EIGMode::RESET;//same as standby
      }
      else if (s == "Operate" || s == "OPERATE")
      {
        return EIGMode::OPERATE;
      }
      else if (s == "Debug" || s == "DEBUG")
      {
        return EIGMode::DEBUG;
      }
      else if (s == "Maintenance" || s == "MAINTENANCE")
      {
        return EIGMode::MAINTENANCE;
      }

      return EIGMode::UNKNOWN;
    }

    sbio::EMirrorMode ConvertCigiStringToMirrorMode(const std::string& sMirrorMode)
    {
      if (sMirrorMode == "None")
      {
        return EMirrorMode::NONE;
      }
      else if (sMirrorMode == "Horizontal")
      {
        return EMirrorMode::HORIZONTAL;
      }
      else if (sMirrorMode == "Vertical")
      {
        return EMirrorMode::VERTICAL;
      }
      else if (sMirrorMode == "Horizontal and Vertical")
      {
        return EMirrorMode::HORIZONTAL_AND_VERTICAL;
      }

      return EMirrorMode::UNKNOWN;
    }

    std::string ConvertCigiMirrorModeToString(const EMirrorMode& eMirrorMode)
    {
      if (eMirrorMode == EMirrorMode::NONE)
      {
        return "None";
      }
      else if (eMirrorMode == EMirrorMode::HORIZONTAL)
      {
        return "Horizontal";
      }
      else if (eMirrorMode == EMirrorMode::VERTICAL)
      {
        return "Vertical";
      }
      else if (eMirrorMode == EMirrorMode::HORIZONTAL_AND_VERTICAL)
      {
        return "Horizontal and Vertical";
      }

      return "Unknown";
    }

    std::string ConvertCigiPixelRepModeToString(const uint8_t pixelMode)
    {
      if (pixelMode == 0)
      {
        return "None";
      }
      else if (pixelMode == 1)
      {
        return "1 x 2";
      }
      else if (pixelMode == 2)
      {
        return "2 x 1";
      }
      else if (pixelMode == 3)
      {
        return "2 x 2";
      }
      else if (pixelMode >= 4 && pixelMode <= 7)
      {
        return "Defined by IG";
      }
      return "Unknown";
    }

    sbio::cigi::EObjectClass ConvertCigiStringToObjectClass(const std::string& sObjectClass)
    {
      if (sObjectClass == "Entity")
      {
        return EObjectClass::ENTITY;
      }
      else if (sObjectClass == "Articulated Part")
      {
        return EObjectClass::ARTICULATED_PART;
      }
      else if (sObjectClass == "View")
      {
        return EObjectClass::VIEW;
      }
      else if (sObjectClass == "View Group")
      {
        return EObjectClass::VIEW_GROUP;
      }
      else if (sObjectClass == "Motion Tracker")
      {
        return EObjectClass::MOTION_TRACKER;
      }

      return EObjectClass::UNKNOWN;
    }

    std::string ConvertCigiObjectClassToString(const EObjectClass& eObjectClass)
    {
      if (eObjectClass == EObjectClass::ENTITY)
      {
        return "Entity";
      }
      else if (eObjectClass == EObjectClass::ARTICULATED_PART)
      {
        return "Articulated Part";
      }
      else if (eObjectClass == EObjectClass::VIEW)
      {
        return "View";
      }
      else if (eObjectClass == EObjectClass::VIEW_GROUP)
      {
        return "View Group";
      }
      else if (eObjectClass == EObjectClass::MOTION_TRACKER)
      {
        return "Motion Tracker";
      }

      return "Unknown";
    }

    ESymbolState ConvertCigiStringToSymbolState(const std::string& ssymbolState)
    {
      if (ssymbolState == "Hidden")
      {
        return ESymbolState::HIDDEN;
      }
      else if (ssymbolState == "Visible")
      {
        return ESymbolState::VISIBLE;
      }
      else if (ssymbolState == "Destroyed")
      {
        return ESymbolState::DESTROYED;
      }

      return ESymbolState::UNKNOWN;
    }

    std::string ConvertCigiSymbolStateToString(ESymbolState eSymbolState)
    {
      if (eSymbolState == ESymbolState::HIDDEN)
      {
        return "Hidden";
      }
      else if (eSymbolState == ESymbolState::VISIBLE)
      {
        return "Visible";
      }
      else if (eSymbolState == ESymbolState::DESTROYED)
      {
        return "Destroyed";
      }

      return "Unknown";
    }

    sbio::cigi::EPolarity ConvertCigiStringToPolarity(const std::string& sPolarity)
    {
      if (sPolarity == "White Hot")
      {
        return EPolarity::WHITE_HOT;
      }
      else if (sPolarity == "Black Hot")
      {
        return EPolarity::BLACK_HOT;
      }

      return EPolarity::UNKNOWN;
    }

    std::string ConvertCigiPolarityToString(const EPolarity& ePolarity)
    {
      if (ePolarity == EPolarity::WHITE_HOT)
      {
        return "White Hot";
      }
      else if (ePolarity == EPolarity::BLACK_HOT)
      {
        return "Black Hot";
      }
      return "Unknown";
    }

    sbio::cigi::EActiveState ConvertToActiveState(CIGI::V40::EntityCtrl::EntityState eEntityState)
    {
      if (eEntityState == CIGI::V40::EntityCtrl::EntityState::eEntityState_InactiveStandby)
      {
        return EActiveState::INACTIVE_STANDBY;
      }
      else if (eEntityState == CIGI::V40::EntityCtrl::EntityState::eEntityState_Active)
      {
        return EActiveState::ACTIVE;
      }
      else if (eEntityState == CIGI::V40::EntityCtrl::EntityState::eEntityState_Destroyed)
      {
        return EActiveState::DESTROYED;
      }

      return EActiveState::UNKNOWN;
    }

    sbio::cigi::ESensorTrack ConvertCigiStringToSensorTrack(const std::string& sSensorTrack)
    {
      if (sSensorTrack == "White")
      {
        return ESensorTrack::WHITE;
      }
      else if (sSensorTrack == "Black")
      {
        return ESensorTrack::BLACK;
      }

      return ESensorTrack::UNKNOWN;
    }

    sbio::cigi::ECigiScope ConvertCigiStringToScope(const std::string& sScope)
    {
      if (sScope == "Global")
      {
        return ECigiScope::GLOBAL;
      }
      else if (sScope == "Regional")
      {
        return ECigiScope::REGIONAL;
      }
      else if (sScope == "Entity")
      {
        return ECigiScope::ENTITY;
      }

      return ECigiScope::UNKNOWN;
    }

    sbio::cigi::EUpdateMode ConvertCigiStringToUpdateMode(const std::string& sUpdateMode)
    {
      if (sUpdateMode == "One-Shot")
      {
        return EUpdateMode::ONE_SHOT;
      }
      else if (sUpdateMode == "Continuous")
      {
        return EUpdateMode::CONTINUOUS;
      }

      return EUpdateMode::UNKNOWN;
    }

    sbio::cigi::EVolumeType ConvertCigiStringToVolumeType(const std::string& sVolumeType)
    {
      if (sVolumeType == "Sphere")
      {
        return EVolumeType::SPHERE;
      }
      else if (sVolumeType == "Cuboid")
      {
        return EVolumeType::CUBOID;
      }

      return EVolumeType::UNKNOWN;
    }

    std::string ConvertCigiVolumeTypeToString(const EVolumeType& eVolumeType)
    {
      if (eVolumeType == EVolumeType::SPHERE)
      {
        return "Sphere";
      }
      else if (eVolumeType == EVolumeType::CUBOID)
      {
        return "Cuboid";
      }

      return "Unknown";
    }

    sbio::cigi::EWaveBreakerType ConvertCigiStringToWaveBreakerType(const std::string& sWaveBreakerType)
    {
      if (sWaveBreakerType == "Plunging")
      {
        return EWaveBreakerType::PLUNGING;
      }
      else if (sWaveBreakerType == "Spilling")
      {
        return EWaveBreakerType::SPILLING;
      }
      else if (sWaveBreakerType == "Surging")
      {
        return EWaveBreakerType::SURGING;
      }

      return EWaveBreakerType::UNKNOWN;
    };

    ETrackMode ConvertCigiStringToTrackMode(const std::string& sTrackMode)
    {
      if (sTrackMode == "Off")
      {
        return ETrackMode::OFF;
      }
      else if (sTrackMode == "Force Correlate")
      {
        return ETrackMode::FORCE_CORRELATE;
      }
      else if (sTrackMode == "Scene")
      {
        return ETrackMode::SCENE;
      }
      else if (sTrackMode == "Target")
      {
        return ETrackMode::TARGET;
      }
      else if (sTrackMode == "Ship")
      {
        return ETrackMode::SHIP;
      }

      return ETrackMode::UNKNOWN;
    }

    std::string ConvertCigiTrackModeToString(const ETrackMode& eTrackMode)
    {
      if (eTrackMode == ETrackMode::OFF)
      {
        return "Off";
      }
      else if (eTrackMode == ETrackMode::FORCE_CORRELATE)
      {
        return "Force Correlate";
      }
      else if (eTrackMode == ETrackMode::SCENE)
      {
        return "Scene";
      }
      else if (eTrackMode == ETrackMode::TARGET)
      {
        return "Target";
      }
      else if (eTrackMode == ETrackMode::SHIP)
      {
        return "Ship";
      }
      return "Unknown";
    }

    ESurfaceState ConvertCigiStringToSurfaceState(const std::string& sSurfaceState)
    {
      if (sSurfaceState == "Active")
      {
        return ESurfaceState::ACTIVE;
      }
      else if (sSurfaceState == "Destroyed")
      {
        return ESurfaceState::DESTROYED;
      }
      return ESurfaceState::UNKNOWN;
    }

    EAttachType ConvertCigiStringToAttachType(const std::string& sAttachType)
    {
      if (sAttachType == "Entity")
      {
        return EAttachType::ENTITY;
      }
      else if (sAttachType == "View")
      {
        return EAttachType::VIEW;
      }
      return EAttachType::UNKNOWN;
    }

    EBillboard ConvertCigiStringToBillboard(const std::string& sBillboard)
    {
      if (sBillboard == "Non-Billboard")
      {
        return EBillboard::NON_BILLBOARD;
      }
      else if (sBillboard == "Billboard")
      {
        return EBillboard::BILLBOARD;
      }
      return EBillboard::UNKNOWN;
    }

    ETextAlignment ConvertCigiStringToAlignment(const std::string& sAlignment)
    {
      if (sAlignment == "Top Left")
      {
        return ETextAlignment::TOP_LEFT;
      }
      else if (sAlignment == "Top Center")
      {
        return ETextAlignment::TOP_CENTER;
      }
      else if (sAlignment == "Top Right")
      {
        return ETextAlignment::TOP_RIGHT;
      }
      else if (sAlignment == "Center Left")
      {
        return ETextAlignment::CENTER_LEFT;
      }
      else if (sAlignment == "Center")
      {
        return ETextAlignment::CENTER;
      }
      else if (sAlignment == "Center Right")
      {
        return ETextAlignment::CENTER_RIGHT;
      }
      else if (sAlignment == "Bottom Left")
      {
        return ETextAlignment::BOTTOM_LEFT;
      }
      else if (sAlignment == "Bottom Center")
      {
        return ETextAlignment::BOTTOM_CENTER;
      }
      else if (sAlignment == "Bottom Right")
      {
        return ETextAlignment::BOTTOM_RIGHT;
      }
      return ETextAlignment::UNKNOWN;
    }

    ETextOrientation ConvertCigiStringToOrientation(const std::string& sOrientation)
    {
      if (sOrientation == "Left to Right")
      {
        return ETextOrientation::LEFT_TO_RIGHT;
      }
      else if (sOrientation == "Top to Bottom")
      {
        return ETextOrientation::TOP_TO_BOTTOM;
      }
      else if (sOrientation == "Right to Left")
      {
        return ETextOrientation::RIGHT_TO_LEFT;
      }
      else if (sOrientation == "Bottom to Top")
      {
        return ETextOrientation::BOTTOM_TO_TOP;
      }
      return ETextOrientation::UNKNOWN;
    }

    std::string ConvertCigiFontIDToString(const uint8_t& fontID)
    {
      if (fontID == 1 || fontID == 9)
      {
        return "Sans Serif";
      }
      else if (fontID == 2 || fontID == 10)
      {
        return "Sans Serif Bold";
      }
      else if (fontID == 3 || fontID == 11)
      {
        return "Sans Serif Italic";
      }
      else if (fontID == 4 || fontID == 12)
      {
        return "Sans Serif Bold Italic";
      }
      else if (fontID == 5 || fontID == 13)
      {
        return "Serif";
      }
      else if (fontID == 6 || fontID == 14)
      {
        return "Serif Bold";
      }
      else if (fontID == 7 || fontID == 15)
      {
        return "Serif Italic";
      }
      else if (fontID == 8 || fontID == 16)
      {
        return "Serif Bold Italic";
      }
      else
      {
        return "";
      }
    }

    EDrawingStyle ConvertCigiStringToDrawingStyle(const std::string& sDrawingStyle)
    {
      if (sDrawingStyle == "Line")
      {
        return EDrawingStyle::LINE;
      }
      else if (sDrawingStyle == "Fill")
      {
        return EDrawingStyle::FILL;
      }
      return EDrawingStyle::UNKNOWN;
    }

    EPrimitiveType ConvertCigiStringToPrimitivePolygon(const std::string& sPrimitiveType)
    {
      if (sPrimitiveType == "Point")
      {
        return EPrimitiveType::POINT;
      }
      else if (sPrimitiveType == "Line")
      {
        return EPrimitiveType::LINE;
      }
      else if (sPrimitiveType == "Line Strip")
      {
        return EPrimitiveType::LINE_STRIP;
      }
      else if (sPrimitiveType == "Line Loop")
      {
        return EPrimitiveType::LINE_LOOP;
      }
      else if (sPrimitiveType == "Triangle")
      {
        return EPrimitiveType::TRIANGLE;
      }
      else if (sPrimitiveType == "Triangle Strip")
      {
        return EPrimitiveType::TRIANGLE_STRIP;
      }
      else if (sPrimitiveType == "Triangle Fan")
      {
        return EPrimitiveType::TRIANGLE_FAN;
      }
      return EPrimitiveType::UNKNOWN;
    }

    EAttributeSelect ConvertCigiStringToAttributeSelect(const std::string& sAttributeSelect)
    {
      if (sAttributeSelect == "None")
      {
        return EAttributeSelect::NONE;
      }
      else if (sAttributeSelect == "Surface ID")
      {
        return EAttributeSelect::SURFACEID;
      }
      else if (sAttributeSelect == "Parent Symbol ID")
      {
        return EAttributeSelect::PARENTSYMBOLID;
      }
      else if (sAttributeSelect == "Layer")
      {
        return EAttributeSelect::LAYER;
      }
      else if (sAttributeSelect == "Flash Duty Cycle Percentage")
      {
        return EAttributeSelect::FLASHDUTYCYCLEPERCENTAGE;
      }
      else if (sAttributeSelect == "Flash Period")
      {
        return EAttributeSelect::FLASHPERIOD;
      }
      else if (sAttributeSelect == "Position U")
      {
        return EAttributeSelect::POSITIONU;
      }
      else if (sAttributeSelect == "Position V")
      {
        return EAttributeSelect::POSITIONV;
      }
      else if (sAttributeSelect == "Rotation")
      {
        return EAttributeSelect::ROTATION;
      }
      else if (sAttributeSelect == "Color")
      {
        return EAttributeSelect::COLOR;
      }
      else if (sAttributeSelect == "Scale U")
      {
        return EAttributeSelect::SCALEU;
      }
      else if (sAttributeSelect == "Scale V")
      {
        return EAttributeSelect::SCALEV;
      }
      return EAttributeSelect::UNKNOWN;
    }

    ESymbolState ConvertCigiStringToESymbolState(const std::string& sSymbolState)
    {
      if (sSymbolState == "Hidden")
      {
        return ESymbolState::HIDDEN;
      }
      else if (sSymbolState == "Visible")
      {
        return ESymbolState::VISIBLE;
      }
      else if (sSymbolState == "Destroyed")
      {
        return ESymbolState::DESTROYED;
      }
      return ESymbolState::UNKNOWN;
    }

    ETextureFilterMode ConvertCigiStringToTextureFilterMode(const std::string& sTextureMode)
    {
      if (sTextureMode == "Nearest")
      {
        return ETextureFilterMode::NEAREST;
      }
      else if (sTextureMode == "Linear")
      {
        return ETextureFilterMode::LINEAR;
      }
      return ETextureFilterMode::UNKNOWN;
    }

    ETextureWrapMode ConvertCigiStringToTextureWrapMode(const std::string& sTextureRepeat)
    {
      if (sTextureRepeat == "Repeat")
      {
        return ETextureWrapMode::REPEAT;
      }
      else if (sTextureRepeat == "Clamp")
      {
        return ETextureWrapMode::CLAMP;
      }
      return ETextureWrapMode::UNKNOWN;
    }

    /*sbio::cigi::ETextureFilterPolygon ConvertCigiStringToTextureFilterPolygon(const std::string& sTextureMode)
    {
      if (sTextureMode == "Nearest")
      {
        return sbio::cigi::ETextureFilterPolygon::NEAREST;
      }
      else if (sTextureMode == "Linear")
      {
        return sbio::cigi::ETextureFilterPolygon::LINEAR;
      }
      return sbio::cigi::ETextureFilterPolygon::NEAREST;
    }*/

    /*sbio::cigi::ETextureWrapPolygon ConvertCigiStringToTextureWrapPolygon(const std::string& sTextureWrap)
    {
      if (sTextureWrap == "Repeat")
      {
        return sbio::cigi::ETextureWrapPolygon::REPEAT;
      }
      else if (sTextureWrap == "Clamp")
      {
        return sbio::cigi::ETextureWrapPolygon::CLAMP;
      }
      return sbio::cigi::ETextureWrapPolygon::REPEAT;
    }*/

    ESymbolSourceType ConvertCigiStringToSourceType(const std::string& sourceType)
    {
      if (sourceType == "Symbol")
      {
        return ESymbolSourceType::SYMBOL;
      }
      else if (sourceType == "Symbol Template")
      {
        return ESymbolSourceType::SYMBOL_TEMPLATE;
      }
      return ESymbolSourceType::UNKNOWN;
    }

    EAttachState ConvertCigiStringToAttachState(const std::string& attachState)
    {
      if (attachState == "Detach")
      {
        return EAttachState::DETACH;
      }
      else if (attachState == "Attach")
      {
        return EAttachState::ATTACH;
      }
      return EAttachState::UNKNOWN;
    }

    EFlashControl ConvertCigiStringToFlashControl(const std::string& flashControl)
    {
      if (flashControl == "Continue")
      {
        return EFlashControl::CONTINUE;
      }
      else if (flashControl == "Reset")
      {
        return EFlashControl::RESET;
      }
      return EFlashControl::UNKNOWN;
    }

    EViewType ConvertCigiStringToViewType(const std::string& viewType)
    {
      if (viewType == "View")
      {
        return EViewType::VIEW;
      }
      else if (viewType == "View Group")
      {
        return EViewType::VIEW_GROUP;
      }
      return EViewType::UNKNOWN;
    }

    sbio::EClamp ConvertCigiStringToEClamp(const std::string& clamp)
    {
      if (clamp == "No Clamp")
      {
        return sbio::EClamp::NONE;
      }
      else if (clamp == "Non-Conformal")
      {
        return sbio::EClamp::NON_CONFORMAL;
      }
      else if (clamp == "Conformal")
      {
        return sbio::EClamp::CONFORMAL;
      }
      return sbio::EClamp::UNKNOWN;
    }

    std::string ConvertCigiEClampToString(const EClamp& eclamp)
    {
      if (eclamp == EClamp::NONE)
      {
        return "No Clamp";
      }
      else if (eclamp == EClamp::NON_CONFORMAL)
      {
        return "Non-Conformal";
      }
      else if (eclamp == EClamp::CONFORMAL)
      {
        return "Conformal";
      }
      return "Unknown";
    }

    void InitCigiLib()
    {
      SetupCigiOpCodeNames();
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
