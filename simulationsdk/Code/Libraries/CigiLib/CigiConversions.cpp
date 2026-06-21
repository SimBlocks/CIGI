//Copyright SimBlocks LLC 2016-2026
#include "CigiConversions.h"
#include "CigiLib.h"
#include "SymbolLib/SymbolTypes.h"
#include "GlobalHeaders/Globals.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::symbol;

namespace sbio
{
  namespace cigi
  {
    TBodyEulerRotation ConvertToBodyEulerRotation(const TCigiBodyEulerRotation& cigiBodyEulerRotation)
    {
      TBodyEulerRotation bodyEulerRotation;
      bodyEulerRotation.pitch = cigiBodyEulerRotation.pitch;
      bodyEulerRotation.roll = cigiBodyEulerRotation.roll;
      bodyEulerRotation.yaw = cigiBodyEulerRotation.yaw;

      return bodyEulerRotation;
    }

    TCigiBodyEulerRotation ConvertToCigiBodyEulerRotation(const TBodyEulerRotation& bodyEulerRotation)
    {
      TCigiBodyEulerRotation cigiBodyEulerRotation;
      cigiBodyEulerRotation.pitch = bodyEulerRotation.pitch;
      cigiBodyEulerRotation.roll = bodyEulerRotation.roll;
      cigiBodyEulerRotation.yaw = bodyEulerRotation.yaw;

      return cigiBodyEulerRotation;
    }

    CigiBodyCoordinates ConvertBodyCoordinatesToCigiBodyCoordinates(const BodyCoordinates& bodyCoordinates)
    {
      //CIGI body coordinates are forward, right, down
      //Body coordinates are right, forward, up
      CigiBodyCoordinates cigiBodyCoordinates;
      cigiBodyCoordinates.Forward() = bodyCoordinates.Forward();
      cigiBodyCoordinates.Right() = bodyCoordinates.Right();
      cigiBodyCoordinates.Down() = -bodyCoordinates.Up();

      return cigiBodyCoordinates;
    }

    BodyCoordinates ConvertCigiBodyCoordinatesToBodyCoordinates(CigiBodyCoordinates cigiBodyCoordinates)
    {
      //CIGI body coordinates are forward, right, down
      //Body coordinates are right, forward, up
      return BodyCoordinates(cigiBodyCoordinates.Right(), cigiBodyCoordinates.Forward(), -cigiBodyCoordinates.Down());
    }

    TBodyTransform ConvertCigiBodyCoordinatesToBodyCoordinates(TCigiBodyTransform transform)
    {
      TBodyTransform t;
      t.pos = ConvertCigiBodyCoordinatesToBodyCoordinates(transform.pos);
      t.rotation = ConvertCigiBodyRotationToBodyRotation(transform.rotation);

      return t;
    }

    TBodyRotation ConvertCigiBodyRotationToBodyRotation(TCigiBodyRotation cigiBodyRotation)
    {
      //CIGI body coordinates are forward, right, down
      //Body coordinates are right, forward, up
      TCigiBodyMatrix m = cigiBodyRotation.toRotationMatrix();
      CigiBodyCoordinates forward = m.getCol(0);
      CigiBodyCoordinates right = m.getCol(1);
      CigiBodyCoordinates down = m.getCol(2);

      TBodyMatrix bodyMatrix;
      bodyMatrix.setCol(0, ConvertCigiBodyCoordinatesToBodyCoordinates(right));
      bodyMatrix.setCol(1, ConvertCigiBodyCoordinatesToBodyCoordinates(forward));
      bodyMatrix.setCol(2, ConvertCigiBodyCoordinatesToBodyCoordinates(down.negated()));

      return TBodyRotation(bodyMatrix);
    }

    TCigiBodyRotation ConvertBodyRotationToCigiBodyRotation(TBodyRotation bodyRotation)
    {
      //Body coordinates are right, forward, up
      //CIGI body coordinates are forward, right, down
      TBodyMatrix m = bodyRotation.toRotationMatrix();
      BodyCoordinates right = m.getCol(0);
      BodyCoordinates forward = m.getCol(1);
      BodyCoordinates up = m.getCol(2);

      TCigiBodyMatrix cigiBodyMatrix;
      cigiBodyMatrix.setCol(0, ConvertBodyCoordinatesToCigiBodyCoordinates(forward));
      cigiBodyMatrix.setCol(1, ConvertBodyCoordinatesToCigiBodyCoordinates(right));
      cigiBodyMatrix.setCol(2, ConvertBodyCoordinatesToCigiBodyCoordinates(up.negated()));

      return TCigiBodyRotation(cigiBodyMatrix);
    }
  }
}

namespace sbio
{
  namespace cigi
  {
    TGeocentricTransform SetupTopLevelTransformation(const SGeodeticCoordinates& geodeticCoordinates, const TCigiBodyEulerRotation& rotation)
    {
      TGeocentricTransform transformation;
      transformation.pos = ConvertGeodeticToGeocentricCoordinates(geodeticCoordinates);
      transformation.rotation = SetupTopLevelEntityRotation(rotation, geodeticCoordinates.latitude, geodeticCoordinates.longitude);

      return transformation;
    }
  }
}

namespace sbio
{
  namespace cigi
  {
    TCigiBodyRotation SetupCigiObjectRotation(const TCigiBodyEulerRotation& rotation)
    {
      Quaternion4d q;
      q.setIdentity();

      //Rotate about yaw, then pitch, then roll
      q = q * AxisRotation(DegreesToRadians(rotation.yaw).Value(), Vec3::UnitZ());
      q = q * AxisRotation(DegreesToRadians(rotation.pitch).Value(), Vec3::UnitY());
      q = q * AxisRotation(DegreesToRadians(rotation.roll).Value(), Vec3::UnitX());

      return TCigiBodyRotation(q);
    }

    TGeocentricRotation SetupTopLevelEntityRotation(const TCigiBodyEulerRotation& rotation, Latitude latitude, Longitude longitude)
    {
      //The order of rotation is about the Z, Y, and then X axes(i.e., yaw, pitch, and then roll).
      //An entity’s yaw is the measure of the angle formed from True North to the entity’s + X axis.
      //This angle is specified in degrees and is positive clockwise if looking along the + Z axis.
      //An entity’s pitch is the measure of the angle between the reference plane and the entity’s + X axis.
      //This angle is specified in degrees and is positive above(away from the ellipsoid) the reference plane.
      //Roll is the measure of the angle between the reference plane and the entity’s + Y axis
      //along a plane perpendicular to the entity’s X axis.
      //In other words, it is the angle of rotation about the X axis after yaw and pitch have been applied.
      //Roll is also specified in degrees and is positive clockwise from the point of view of looking along the +X axis.
      TBodyRotation bodyRotation;
      //Rotate about yaw, then pitch, then roll
      bodyRotation = ConvertCigiBodyRotationToBodyRotation(SetupCigiObjectRotation(rotation));
      return GetGeocentricRotation(latitude, longitude) * bodyRotation;
    }
  }
}

namespace sbio
{
  namespace cigi
  {
    EActiveState ConvertRegion(CIGI::V40::EnvironmentalRegionCtrl::RegionState eRegionState)
    {
      switch (eRegionState)
      {
      default:
        return EActiveState::UNKNOWN;
        break;
      case CIGI::V40::EnvironmentalRegionCtrl::eRegionState_Inactive:
        return EActiveState::INACTIVE_STANDBY;
        break;
      case CIGI::V40::EnvironmentalRegionCtrl::eRegionState_Active:
        return EActiveState::ACTIVE;
        break;
      case CIGI::V40::EnvironmentalRegionCtrl::eRegionState_Destroyed:
        return EActiveState::DESTROYED;
        break;
      }
    }

    sbio::Percentage ConvertAlphaToPercentage(uint8_t nAlpha)
    {
      return Percentage(nAlpha / 255.0f);
    }

    sbio::Percentage ConvertAlphaToPercentage(float fAlpha)
    {
      return Percentage(fAlpha / 255.0f);
    }

    sbio::Percentage ConvertAlphaToPercentage(int nAlpha)
    {
      return Percentage(nAlpha / 255.0f);
    }

    EDegreeOfFreedom ConvertDegreeOfFreedom(CIGI::V40::ShortArticulatedPartCtrl::DOFSelect eDOF)
    {
      switch (eDOF)
      {
      default:
        return EDegreeOfFreedom::UNKNOWN;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_NotUsed:
        return EDegreeOfFreedom::NOT_USED;
        break;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_XOffset:
        return EDegreeOfFreedom::X_OFFSET;
        break;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_YOffset:
        return EDegreeOfFreedom::Y_OFFSET;
        break;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_ZOffset:
        return EDegreeOfFreedom::Z_OFFSET;
        break;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_Yaw:
        return EDegreeOfFreedom::YAW;
        break;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_Pitch:
        return EDegreeOfFreedom::PITCH;
        break;
      case CIGI::V40::ShortArticulatedPartCtrl::DOFSelect::eDOFSelect_Roll:
        return EDegreeOfFreedom::ROLL;
        break;
      }
    }

    ETextAlignment ConvertTextAlignment(CIGI::V40::SymbolTextDefinition::Alignment eAlignment)
    {
      switch (eAlignment)
      {
      default:
        return ETextAlignment::UNKNOWN;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomCenter:
        return ETextAlignment::BOTTOM_CENTER;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomLeft:
        return ETextAlignment::BOTTOM_LEFT;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomRight:
        return ETextAlignment::BOTTOM_RIGHT;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_Center:
        return ETextAlignment::CENTER;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_CenterLeft:
        return ETextAlignment::CENTER_LEFT;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_CenterRight:
        return ETextAlignment::CENTER_RIGHT;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_TopCenter:
        return ETextAlignment::TOP_CENTER;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_TopLeft:
        return ETextAlignment::TOP_LEFT;
        break;
      case CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_TopRight:
        return ETextAlignment::TOP_RIGHT;
        break;
      }
    }

    ETextOrientation ConvertTextOrientation(CIGI::V40::SymbolTextDefinition::Orientation eOrientation)
    {
      switch (eOrientation)
      {
      case CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_BottomToTop:
        return ETextOrientation::BOTTOM_TO_TOP;
        break;
      case CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_LeftToRight:
        return ETextOrientation::LEFT_TO_RIGHT;
        break;
      case CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_RightToLeft:
        return ETextOrientation::RIGHT_TO_LEFT;
        break;
      case CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_TopToBottom:
        return ETextOrientation::TOP_TO_BOTTOM;
        break;
      }

      return ETextOrientation::UNKNOWN;
    }

    ECigiScope ConvertScope(CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope eScope)
    {
      switch (eScope)
      {
      case CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Global:
        return ECigiScope::GLOBAL;
      case CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Regional:
        return ECigiScope::REGIONAL;
      case CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Entity:
        return ECigiScope::ENTITY;
      }

      return ECigiScope::UNKNOWN;
    }

    ECigiScope ConvertScope(CIGI::V40::WaveCtrl::Scope eScope)
    {
      switch (eScope)
      {
      case CIGI::V40::WaveCtrl::Scope::eScope_Global:
        return ECigiScope::GLOBAL;
      case CIGI::V40::WaveCtrl::Scope::eScope_Regional:
        return ECigiScope::REGIONAL;
      case CIGI::V40::WaveCtrl::Scope::eScope_Entity:
        return ECigiScope::ENTITY;
      }

      return ECigiScope::UNKNOWN;
    }

    ECigiScope ConvertScope(CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope eScope)
    {
      switch (eScope)
      {
      case CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Global:
        return ECigiScope::GLOBAL;
      case CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Regional:
        return ECigiScope::REGIONAL;
      case CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope::eScope_Entity:
        return ECigiScope::ENTITY;
      }

      return ECigiScope::UNKNOWN;
    }

    sbio::symbol::EPrimitiveType ConvertPrimitiveGrp(CIGI::V40::SymbolPolygonDefinition::PrimitiveType ePrimitiveGrp)
    {
      switch (ePrimitiveGrp)
      {
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_Point:
        return EPrimitiveType::POINT;
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_Line:
        return EPrimitiveType::LINE;
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_LineStrip:
        return EPrimitiveType::LINE_STRIP;
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_LineLoop:
        return EPrimitiveType::LINE_LOOP;
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_Triangle:
        return EPrimitiveType::TRIANGLE;
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleStrip:
        return EPrimitiveType::TRIANGLE_STRIP;
      case CIGI::V40::SymbolPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleFan:
        return EPrimitiveType::TRIANGLE_FAN;
      default:
        return EPrimitiveType::UNKNOWN;
      }
    }

    sbio::symbol::EPrimitiveType ConvertPrimitiveGrp(CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType ePrimitive)
    {
      switch (ePrimitive)
      {
      case CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType::ePrimitiveType_Triangle:
        return EPrimitiveType::TRIANGLE;
      case CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleStrip:
        return EPrimitiveType::TRIANGLE_STRIP;
      case CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType::ePrimitiveType_TriangleFan:
        return EPrimitiveType::TRIANGLE_FAN;
      default:
        return EPrimitiveType::UNKNOWN;
      }
    }

    EProjectionMode ConvertProjectionMode(CIGI::V40::ViewDefinition::ProjectionType eProjection)
    {
      if (eProjection == CIGI::V40::ViewDefinition::ProjectionType::eProjectionType_Perspective)
      {
        return EProjectionMode::PERSPECTIVE;
      }
      else if (eProjection == CIGI::V40::ViewDefinition::ProjectionType::eProjectionType_Orthographic)
      {
        return EProjectionMode::ORTHOGRAPHIC;
      }

      return EProjectionMode::UNKNOWN;
    }

    EMirrorMode ConvertMirrorMode(CIGI::V40::ViewDefinition::MirrorMode eMirror)
    {
      if (eMirror == CIGI::V40::ViewDefinition::eMirrorMode_None)
      {
        return EMirrorMode::NONE;
      }
      else if (eMirror == CIGI::V40::ViewDefinition::eMirrorMode_Horizontal)
      {
        return EMirrorMode::HORIZONTAL;
      }
      else if (eMirror == CIGI::V40::ViewDefinition::eMirrorMode_Vertical)
      {
        return EMirrorMode::VERTICAL;
      }
      else if (eMirror == CIGI::V40::ViewDefinition::eMirrorMode_Both)
      {
        return EMirrorMode::HORIZONTAL_AND_VERTICAL;
      }

      return EMirrorMode::UNKNOWN;
    }

    ESymbolState ConvertSymbolState(CIGI::V40::SymbolCtrl::SymbolState eSymbolState)
    {
      switch (eSymbolState)
      {
      case CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Destroyed:
        return ESymbolState::DESTROYED;
      case CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Hidden:
        return ESymbolState::HIDDEN;
      case CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Visible:
        return ESymbolState::VISIBLE;
      }

      return ESymbolState::UNKNOWN;
    }

    sbio::cigi::EVolumeType ConvertCollisionVolumeType(CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType eVolumeType)
    {
      switch (eVolumeType)
      {
      case CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType::eVolumeType_Cuboid:
        return sbio::cigi::EVolumeType::CUBOID;
      case CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType::eVolumeType_Sphere:
        return sbio::cigi::EVolumeType::SPHERE;
      }
      return sbio::cigi::EVolumeType::UNKNOWN;
    }

    sbio::cigi::EMergeState ConvertMergeState(CIGI::V40::EnvironmentalRegionCtrl::Merge eMergeState)
    {
      switch (eMergeState)
      {
      case CIGI::V40::EnvironmentalRegionCtrl::Merge::eMerge_UseLast:
        return EMergeState::USE_LAST;
      case CIGI::V40::EnvironmentalRegionCtrl::Merge::eMerge_Merge:
        return EMergeState::MERGE;
      }

      return EMergeState::UNKNOWN;
    }

    EObjectCoordinateSystem ConvertCoordinateSystem(CIGI::V40::VelocityCtrl::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_WorldParent:
        return EObjectCoordinateSystem::WORLD;
        break;
      case CIGI::V40::VelocityCtrl::CoordinateSystem::eCoordinateSystem_Local:
        return EObjectCoordinateSystem::LOCAL;
        break;
      }

      return EObjectCoordinateSystem::UNKNOWN;
    }

    EObjectCoordinateSystem ConvertCoordinateSystem(CIGI::V40::PositionRequest::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
        return EObjectCoordinateSystem::WORLD;
      case CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_ParentEntity:
        return EObjectCoordinateSystem::PARENT;
        break;
      case CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Submodel:
        return EObjectCoordinateSystem::LOCAL;
        break;
      }

      return EObjectCoordinateSystem::UNKNOWN;
    }

    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
        return ETopLevelCoordinateSystem::GEODETIC;
        break;
      case CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity:
        return ETopLevelCoordinateSystem::ENTITY;
        break;
      }
      return ETopLevelCoordinateSystem::UNKNOWN;
    }

    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V33::LineOfSightVectorRequest::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
        return ETopLevelCoordinateSystem::GEODETIC;
        break;
      case CIGI::V33::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity:
        return ETopLevelCoordinateSystem::ENTITY;
        break;
      }
      return ETopLevelCoordinateSystem::UNKNOWN;
    }

    ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
        return ETopLevelCoordinateSystem::GEODETIC;
        break;
      case CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity:
        return ETopLevelCoordinateSystem::ENTITY;
        break;
      }

      return ETopLevelCoordinateSystem::UNKNOWN;
    }

    ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V40::LineOfSightVectorRequest::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
        return ETopLevelCoordinateSystem::GEODETIC;
        break;
      case CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity:
        return ETopLevelCoordinateSystem::ENTITY;
        break;
      }

      return ETopLevelCoordinateSystem::UNKNOWN;
    }

    ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V40::HATHOTRequest::CoordinateSystem eCoordSysGroup)
    {
      switch (eCoordSysGroup)
      {
      case CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Geodetic:
        return ETopLevelCoordinateSystem::GEODETIC;
        break;
      case CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Entity:
        return ETopLevelCoordinateSystem::ENTITY;
        break;
      }
      return ETopLevelCoordinateSystem::UNKNOWN;
    }

    ESurfaceState ConvertToSurfaceState(CIGI::V40::SymbolSurfaceDefinition::SurfaceState surfaceState)
    {
      if (surfaceState == CIGI::V40::SymbolSurfaceDefinition::SurfaceState::eSurfaceState_Active)
      {
        return ESurfaceState::ACTIVE;
      }
      else if (surfaceState == CIGI::V40::SymbolSurfaceDefinition::SurfaceState::eSurfaceState_Destroyed)
      {
        return ESurfaceState::DESTROYED;
      }

      return ESurfaceState::UNKNOWN;
    }

    EObjectClass ConvertObjectClass(CIGI::V40::PositionRequest::ObjectClass eObjectClass)
    {
      switch (eObjectClass)
      {
      case CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ArticulatedPart:
        return EObjectClass::ARTICULATED_PART;
      case CIGI::V40::PositionRequest::ObjectClass::eObjectClass_Entity:
        return EObjectClass::ENTITY;
      case CIGI::V40::PositionRequest::ObjectClass::eObjectClass_MotionTracker:
        return EObjectClass::MOTION_TRACKER;
      case CIGI::V40::PositionRequest::ObjectClass::eObjectClass_View:
        return EObjectClass::VIEW;
      case CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ViewGroup:
        return EObjectClass::VIEW_GROUP;
      }

      return EObjectClass::UNKNOWN;
    }

    EObjectClass ConvertObjectClass(CIGI::V40::PositionResponse::ObjectClass eObjectClass)
    {
      switch (eObjectClass)
      {
      case CIGI::V40::PositionResponse::ObjectClass::eObjectClass_ArticulatedPart:
        return EObjectClass::ARTICULATED_PART;
      case CIGI::V40::PositionResponse::ObjectClass::eObjectClass_Entity:
        return EObjectClass::ENTITY;
      case CIGI::V40::PositionResponse::ObjectClass::eObjectClass_MotionTracker:
        return EObjectClass::MOTION_TRACKER;
      case CIGI::V40::PositionResponse::ObjectClass::eObjectClass_View:
        return EObjectClass::VIEW;
      case CIGI::V40::PositionResponse::ObjectClass::eObjectClass_ViewGroup:
        return EObjectClass::VIEW_GROUP;
      }

      return EObjectClass::UNKNOWN;
    }

    CIGI::V40::PositionRequest::ObjectClass ConvertObjectClass(EObjectClass eObjectClass)
    {
      switch (eObjectClass)
      {
      case EObjectClass::ARTICULATED_PART:
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ArticulatedPart;
      case EObjectClass::ENTITY:
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_Entity;
      case EObjectClass::MOTION_TRACKER:
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_MotionTracker;
      case EObjectClass::VIEW:
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_View;
      case EObjectClass::VIEW_GROUP:
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ViewGroup;
      default:
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_Entity;
      }
    }

    sbio::symbol::ETextureFilterMode ConvertTextureFilterMode(CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode eTextureFilterMode)
    {
      switch (eTextureFilterMode)
      {
      case CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode::eTextureFilterMode_Nearest:
        return ETextureFilterMode::NEAREST;
      case CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode::eTextureFilterMode_Linear:
        return ETextureFilterMode::LINEAR;
      }

      return ETextureFilterMode::UNKNOWN;
    }

    sbio::symbol::ETextureFilterMode ConvertTextureFilterMode(CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode eTextureFilterMode)
    {
      switch (eTextureFilterMode)
      {
      case CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode::eTextureFilterMode_Nearest:
        return ETextureFilterMode::NEAREST;
      case CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode::eTextureFilterMode_Linear:
        return ETextureFilterMode::LINEAR;
      }

      return ETextureFilterMode::UNKNOWN;
    }

    sbio::symbol::ETextureWrapMode ConvertTextureWrapMode(CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp eTextureWrapMode)
    {
      switch (eTextureWrapMode)
      {
      case CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Clamp:
        return ETextureWrapMode::CLAMP;
      case CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Repeat:
        return ETextureWrapMode::REPEAT;
      }

      return ETextureWrapMode::UNKNOWN;
    }

    sbio::symbol::ETextureWrapMode ConvertTextureWrapMode(CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp eTextureWrapMode)
    {
      switch (eTextureWrapMode)
      {
      case CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Clamp:
        return ETextureWrapMode::CLAMP;
      case CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Repeat:
        return ETextureWrapMode::REPEAT;
      }

      return ETextureWrapMode::UNKNOWN;
    }

    sbio::cigi::ERequestType ConvertRequestType(CIGI::V40::HATHOTRequest::Type eRequestType)
    {
      switch (eRequestType)
      {
      case CIGI::V40::HATHOTRequest::Type::eType_HAT:
        return ERequestType::HAT;
      case CIGI::V40::HATHOTRequest::Type::eType_HOT:
        return ERequestType::HOT;
      case CIGI::V40::HATHOTRequest::Type::eType_Extended:
        return ERequestType::EXTENDED;
      default:
        return ERequestType::UNKNOWN;
      }
    }

    sbio::symbol::EDrawingStyle ConvertDrawingStyle(CIGI::V40::SymbolCircleDefinition::DrawingStyle drawingStyle)
    {
      if (drawingStyle == CIGI::V40::SymbolCircleDefinition::DrawingStyle::eDrawingStyle_Line)
      {
        return EDrawingStyle::LINE;
      }
      else if (drawingStyle == CIGI::V40::SymbolCircleDefinition::DrawingStyle::eDrawingStyle_Fill)
      {
        return EDrawingStyle::FILL;
      }

      return EDrawingStyle::UNKNOWN;
    }

    sbio::symbol::ESymbolSourceType ConvertSymbolSourceType(CIGI::V40::SymbolClone::SourceType eSymbolSource)
    {
      if (eSymbolSource == CIGI::V40::SymbolClone::SourceType::eSourceType_Symbol)
      {
        return ESymbolSourceType::SYMBOL;
      }
      else if (eSymbolSource == CIGI::V40::SymbolClone::SourceType::eSourceType_SymbolTemplate)
      {
        return ESymbolSourceType::SYMBOL_TEMPLATE;
      }

      return ESymbolSourceType::UNKNOWN;
    }

    EAttachState ConvertAttachState(CIGI::V40::EntityPosition::AttachState eAttachState)
    {
      switch (eAttachState)
      {
      case CIGI::V40::EntityPosition::AttachState::eAttachState_Attach:
        return EAttachState::ATTACH;
      case CIGI::V40::EntityPosition::AttachState::eAttachState_Detach:
        return EAttachState::DETACH;
      }
      return EAttachState::UNKNOWN;
    }

    EAttachState ConvertAttachState(CIGI::V40::SymbolCtrl::AttachState eAttachState)
    {
      switch (eAttachState)
      {
      case CIGI::V40::SymbolCtrl::AttachState::eAttachState_Attach:
        return EAttachState::ATTACH;
      case CIGI::V40::SymbolCtrl::AttachState::eAttachState_Detach:
        return EAttachState::DETACH;
      }
      return EAttachState::UNKNOWN;
    }

    CIGI::V40::SymbolSurfaceDefinition::AttachType ConvertToAttachState(EAttachType attachType)
    {
      if (attachType == EAttachType::ENTITY)
      {
        return CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_Entity;
      }
      else if (attachType == EAttachType::VIEW)
      {
        return CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_View;
      }
      return CIGI::V40::SymbolSurfaceDefinition::AttachType::eAttachType_Entity;
    }

    CIGI::V40::SymbolSurfaceDefinition::SurfaceState ConvertToSurfaceState(ESurfaceState surfaceState)
    {
      if (surfaceState == ESurfaceState::ACTIVE)
      {
        return CIGI::V40::SymbolSurfaceDefinition::SurfaceState::eSurfaceState_Active;
      }
      return CIGI::V40::SymbolSurfaceDefinition::SurfaceState::eSurfaceState_Destroyed;
    }

    CIGI::V40::SymbolSurfaceDefinition::Billboard ConvertToBillboard(EBillboard billboard)
    {
      if (billboard == EBillboard::BILLBOARD)
      {
        return CIGI::V40::SymbolSurfaceDefinition::Billboard::eBillboard_Billboard;
      }
      return CIGI::V40::SymbolSurfaceDefinition::Billboard::eBillboard_NonBillboard;
    }

    uint8_t ConvertToCigiAlpha(sbio::Percentage alpha)
    {
      return static_cast<uint8_t>(alpha.Value() * 255.0f + .5f);
    }

    EFlashControl ConvertToFlashControl(CIGI::V40::SymbolCtrl::FlashControl eFlashControl)
    {
      if (eFlashControl == CIGI::V40::SymbolCtrl::FlashControl::eFlashControl_Continue)
      {
        return EFlashControl::CONTINUE;
      }
      else if (eFlashControl == CIGI::V40::SymbolCtrl::FlashControl::eFlashControl_Reset)
      {
        return EFlashControl::RESET;
      }

      return EFlashControl::UNKNOWN;
    }

    sbio::cigi::CigiFloatPercentage ConvertToCigiFloatPercentage(sbio::Percentage percentage)
    {
      return CigiFloatPercentage(percentage.Value() * 100.0f);
    }

    sbio::cigi::CigiIntPercentage ConvertToCigiIntPercentage(sbio::Percentage percentage)
    {
      return CigiIntPercentage(static_cast<uint8_t>(percentage.Value() * 100.0f + .5f));
    }

    sbio::cigi::CigiMonth ConvertToCigiMonth(sbio::Month month)
    {
      return CigiMonth(month.Value() + 1);
    }

    sbio::cigi::CigiTerrestrialSurfaceSeverity ConvertToCigiTerrestrailSurfaceConditionsSeverity(sbio::Percentage percentage)
    {
      return CigiTerrestrialSurfaceSeverity(static_cast<uint8_t>(percentage.Value() * 31.0 + .5f));
    }

    sbio::Month ConvertToMonth(sbio::cigi::CigiMonth cigiMonth)
    {
      return Month(cigiMonth.Value() - 1);
    }

    sbio::Percentage ConvertToPercentage(CigiFloatPercentage cigiPercentage)
    {
      return Percentage(cigiPercentage.Value() / 100.0f);
    }

    sbio::Percentage ConvertToPercentage(sbio::cigi::CigiIntPercentage cigiPercentage)
    {
      return Percentage(static_cast<float>(cigiPercentage.Value()) / 100.0f);
    }

    sbio::Percentage ConvertToPercentage(sbio::cigi::CigiTerrestrialSurfaceSeverity cigiTerrestrialSurfaceSeverity)
    {
      return Percentage(cigiTerrestrialSurfaceSeverity.Value() / 31.0f);
    }

    CIGI::V40::SymbolTextDefinition::Alignment ConvertToAlignment(ETextAlignment align)
    {
      if (align == ETextAlignment::BOTTOM_CENTER)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomCenter;
      }
      else if (align == ETextAlignment::BOTTOM_LEFT)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomLeft;
      }
      else if (align == ETextAlignment::BOTTOM_RIGHT)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomRight;
      }
      else if (align == ETextAlignment::TOP_LEFT)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_TopLeft;
      }
      else if (align == ETextAlignment::TOP_CENTER)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_TopCenter;
      }
      else if (align == ETextAlignment::TOP_RIGHT)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_TopRight;
      }
      else if (align == ETextAlignment::CENTER_LEFT)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_CenterLeft;
      }
      else if (align == ETextAlignment::CENTER_RIGHT)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_CenterRight;
      }
      else if (align == ETextAlignment::CENTER)
      {
        return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_Center;
      }
      return CIGI::V40::SymbolTextDefinition::Alignment::eAlignment_BottomCenter;
    }

    std::string ConvertCigiAlignmentToString(ETextAlignment eAlignment)
    {
      if (eAlignment == ETextAlignment::TOP_LEFT)
      {
        return "Top Left";
      }
      else if (eAlignment == ETextAlignment::TOP_CENTER)
      {
        return "Top Center";
      }
      else if (eAlignment == ETextAlignment::TOP_RIGHT)
      {
        return "Top Right";
      }
      else if (eAlignment == ETextAlignment::CENTER_LEFT)
      {
        return "Center Left";
      }
      else if (eAlignment == ETextAlignment::CENTER)
      {
        return "Center";
      }
      else if (eAlignment == ETextAlignment::CENTER_RIGHT)
      {
        return "Center Right";
      }
      else if (eAlignment == ETextAlignment::BOTTOM_LEFT)
      {
        return "Bottom Left";
      }
      else if (eAlignment == ETextAlignment::BOTTOM_CENTER)
      {
        return "Bottom Center";
      }
      else if (eAlignment == ETextAlignment::BOTTOM_RIGHT)
      {
        return "Bottom Right";
      }
      return "Unknown";
    }

    CIGI::V40::SymbolTextDefinition::Orientation ConvertToOrientation(ETextOrientation orientation)
    {
      if (orientation == ETextOrientation::BOTTOM_TO_TOP)
      {
        return CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_BottomToTop;
      }
      else if (orientation == ETextOrientation::LEFT_TO_RIGHT)
      {
        return CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_LeftToRight;
      }
      else if (orientation == ETextOrientation::RIGHT_TO_LEFT)
      {
        return CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_RightToLeft;
      }
      else if (orientation == ETextOrientation::TOP_TO_BOTTOM)
      {
        return CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_TopToBottom;
      }
      return CIGI::V40::SymbolTextDefinition::Orientation::eOrientation_BottomToTop;
    }

    std::string ConvertCigiOrientationToString(ETextOrientation eOrientation)
    {
      if (eOrientation == ETextOrientation::LEFT_TO_RIGHT)
      {
        return "Left to Right";
      }
      else if (eOrientation == ETextOrientation::TOP_TO_BOTTOM)
      {
        return "Top to Bottom";
      }
      else if (eOrientation == ETextOrientation::RIGHT_TO_LEFT)
      {
        return "Right to Left";
      }
      else if (eOrientation == ETextOrientation::BOTTOM_TO_TOP)
      {
        return "Bottom to Top";
      }
      return "Unknown";
    }

    CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode ConvertToTextureFilterMode(ETextureFilterMode mode)
    {
      if (mode == ETextureFilterMode::LINEAR)
      {
        return CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode::eTextureFilterMode_Linear;
      }
      else if (mode == ETextureFilterMode::NEAREST)
      {
        return CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode::eTextureFilterMode_Nearest;
      }
      return CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode::eTextureFilterMode_Nearest;
    }

    CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp ConvertToTexturedCircleTextureWrapMode(ETextureWrapMode mode)
    {
      if (mode == ETextureWrapMode::CLAMP)
      {
        return CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Clamp;
      }
      else if (mode == ETextureWrapMode::REPEAT)
      {
        return CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Repeat;
      }
      return CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Repeat;
    }

    CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode ConvertToTextureFilter(ETextureFilterMode filter)
    {
      if (filter == ETextureFilterMode::LINEAR)
      {
        return CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode::eTextureFilterMode_Linear;
      }
      else if (filter == ETextureFilterMode::NEAREST)
      {
        return CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode::eTextureFilterMode_Nearest;
      }
      return CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode::eTextureFilterMode_Nearest;
    }

    CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp ConvertToTexturedPolygonTextureWrapMode(sbio::symbol::ETextureWrapMode mode)
    {
      if (mode == ETextureWrapMode::CLAMP)
      {
        return CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Clamp;
      }
      else if (mode == ETextureWrapMode::REPEAT)
      {
        return CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Repeat;
      }
      return CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp::eTextureRepeatOrClamp_Repeat;
    }

    std::string ConvertInheritAlphaToString(bool inheritAlpha)
    {
      if (inheritAlpha)
      {
        return "Inherited";
      }
      return "Not Inherited";
    }

    std::string ConvertECigiVersionToString(ECigiVersion eCigiVersion)
    {
      switch (eCigiVersion)
      {
      case ECigiVersion::VERSION_3_3:
        return "VERSION_3_3";
      case ECigiVersion::VERSION_4_0:
        return "VERSION_4_0";
      case ECigiVersion::UNKNOWN_VERSION:
      default:
        return "UNKNOWN_VERSION";
      }
    }

    std::string ConvertEIGModeToString(EIGMode eIGMode)
    {
      switch (eIGMode)
      {
      case EIGMode::RESET:
        return "RESET";
      case EIGMode::OPERATE:
        return "OPERATE";
      case EIGMode::DEBUG:
        return "DEBUG";
      case EIGMode::MAINTENANCE:
        return "MAINTENANCE";
      case EIGMode::UNKNOWN:
      default:
        return "UNKNOWN";
      }
    }

    std::string ConvertEObjectClassToString(EObjectClass eObjectClass)
    {
      return ConvertObjectClassToString(eObjectClass);
    }

    std::string ConvertESensorStatusToString(ESensorStatus eSensorStatus)
    {
      switch (eSensorStatus)
      {
      case ESensorStatus::SEARCHING_FOR_TARGET:
        return "Searching for target";
      case ESensorStatus::TRACKING_TARGET:
        return "Tracking target";
      case ESensorStatus::IMPENDING_BREAKLOCK:
        return "Impending breaklock";
      case ESensorStatus::BREAKLOCK:
        return "Breaklock";
      case ESensorStatus::UKNOWN:
      default:
        return "Unknown";
      }
    }

    std::string ConvertSensorStatusToString(CIGI::V40::SensorResponse::SensorStatus sensorStatus)
    {
      if (sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_SearchingForTarget)
      {
        return "Searching for target";
      }
      else if (sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_TrackingTarget)
      {
        return "Tracking target";
      }
      else if (sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_ImpendingBreaklock)
      {
        return "Impending breaklock";
      }
      else if (sensorStatus == CIGI::V40::SensorResponse::SensorStatus::eSensorStatus_Breaklock)
      {
        return "Breaklock";
      }
      else
      {
        return "Unknown";
      }
    }

    std::string ConvertObjectClassToString(CIGI::V40::PositionRequest::ObjectClass objectClass)
    {
      if (objectClass == CIGI::V40::PositionRequest::ObjectClass::eObjectClass_Entity)
      {
        return "Entity";
      }
      else if (objectClass == CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ArticulatedPart)
      {
        return "Articulated Part";
      }
      else if (objectClass == CIGI::V40::PositionRequest::ObjectClass::eObjectClass_View)
      {
        return "View";
      }
      else if (objectClass == CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ViewGroup)
      {
        return "View Group";
      }
      else if (objectClass == CIGI::V40::PositionRequest::ObjectClass::eObjectClass_MotionTracker)
      {
        return "Motion Tracker";
      }
      else
      {
        return "Unknown";
      }
    }

    std::string ConvertObjectClassToString(CIGI::V40::PositionResponse::ObjectClass objectClass)
    {
      if (objectClass == CIGI::V40::PositionResponse::ObjectClass::eObjectClass_Entity)
      {
        return "Entity";
      }
      else if (objectClass == CIGI::V40::PositionResponse::ObjectClass::eObjectClass_ArticulatedPart)
      {
        return "Articulated Part";
      }
      else if (objectClass == CIGI::V40::PositionResponse::ObjectClass::eObjectClass_View)
      {
        return "View";
      }
      else if (objectClass == CIGI::V40::PositionResponse::ObjectClass::eObjectClass_ViewGroup)
      {
        return "View Group";
      }
      else if (objectClass == CIGI::V40::PositionResponse::ObjectClass::eObjectClass_MotionTracker)
      {
        return "Motion Tracker";
      }
      else
      {
        return "Unknown";
      }
    }

    std::string ConvertObjectClassToString(EObjectClass objectClass)
    {
      if (objectClass == EObjectClass::ENTITY)
      {
        return "Entity";
      }
      else if (objectClass == EObjectClass::ARTICULATED_PART)
      {
        return "Articulated Part";
      }
      else if (objectClass == EObjectClass::VIEW)
      {
        return "View";
      }
      else if (objectClass == EObjectClass::VIEW_GROUP)
      {
        return "View Group";
      }
      else if (objectClass == EObjectClass::MOTION_TRACKER)
      {
        return "Motion Tracker";
      }
      else
      {
        return "Unknown";
      }
    }

    std::string ConvertCoordSysGeoParSubToString(CIGI::V40::PositionResponse::CoordinateSystem coordSys)
    {
      if (coordSys == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Geodetic)
      {
        return "Geodetic";
      }
      else if (coordSys == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_ParentEntity)
      {
        return "Parent Entity";
      }
      else if (coordSys == CIGI::V40::PositionResponse::CoordinateSystem::eCoordinateSystem_Submodel)
      {
        return "Submodel";
      }
      else
      {
        return "Unknown";
      }
    }

    std::string ConvertCoordSysGeoParSubToString(CIGI::V40::PositionRequest::CoordinateSystem coordSys)
    {
      if (coordSys == CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic)
      {
        return "Geodetic";
      }
      else if (coordSys == CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_ParentEntity)
      {
        return "Parent Entity";
      }
      else if (coordSys == CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Submodel)
      {
        return "Submodel";
      }
      else
      {
        return "Unknown";
      }
    }

    CIGI::V40::ViewDefinition::MirrorMode ConvertToMirrorModeGrp(EMirrorMode eMirrorMode)
    {
      if (eMirrorMode == EMirrorMode::NONE)
      {
        return CIGI::V40::ViewDefinition::MirrorMode::eMirrorMode_None;
      }
      else if (eMirrorMode == EMirrorMode::HORIZONTAL)
      {
        return CIGI::V40::ViewDefinition::MirrorMode::eMirrorMode_Horizontal;
      }
      else if (eMirrorMode == EMirrorMode::VERTICAL)
      {
        return CIGI::V40::ViewDefinition::MirrorMode::eMirrorMode_Vertical;
      }
      else if (eMirrorMode == EMirrorMode::HORIZONTAL_AND_VERTICAL)
      {
        return CIGI::V40::ViewDefinition::MirrorMode::eMirrorMode_Both;
      }

      return CIGI::V40::ViewDefinition::MirrorMode::eMirrorMode_None;
    }

    CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope ConvertToCigiScopeMaritime(ECigiScope scope)
    {
      if (scope == ECigiScope::GLOBAL)
      {
        return CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Global;
      }
      else if (scope == ECigiScope::REGIONAL)
      {
        return CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Regional;
      }
      else if (scope == ECigiScope::ENTITY)
      {
        return CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Entity;
      }
      return CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope::eScope_Global;
    }

    std::string ConvertCigiScopeToString(ECigiScope scope)
    {
      if (scope == ECigiScope::GLOBAL)
      {
        return "Global";
      }
      else if (scope == ECigiScope::REGIONAL)
      {
        return "Regional";
      }
      else if (scope == ECigiScope::ENTITY)
      {
        return "Entity";
      }
      return "Unknown";
    }

    CIGI::V40::SensorCtrl::TrackMode ConvertToTrackModeGrp(ETrackMode mode)
    {
      if (mode == ETrackMode::FORCE_CORRELATE)
      {
        return CIGI::V40::SensorCtrl::TrackMode::eTrackMode_ForceCorrelate;
      }
      else if (mode == ETrackMode::SCENE)
      {
        return CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Scene;
      }
      else if (mode == ETrackMode::TARGET)
      {
        return CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Target;
      }
      else if (mode == ETrackMode::SHIP)
      {
        return CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Ship;
      }
      return CIGI::V40::SensorCtrl::TrackMode::eTrackMode_Off;
    }

    /// <summary>
    /// Converts the EPoarity enum to the PolarityGrp enum
    /// BLACK_HOT converts to BlackHot
    /// WHITE_HOT converts to WhiteHot
    /// WhiteHot is the default
    /// </summary>
    CIGI::V40::SensorCtrl::Polarity ConvertToPolarityGrp(EPolarity polarity)
    {
      if (polarity == EPolarity::BLACK_HOT)
      {
        return CIGI::V40::SensorCtrl::Polarity::ePolarity_BlackHot;
      }
      else if (polarity == EPolarity::WHITE_HOT)
      {
        return CIGI::V40::SensorCtrl::Polarity::ePolarity_WhiteHot;
      }
      return CIGI::V40::SensorCtrl::Polarity::ePolarity_WhiteHot;
    }

    CIGI::V40::SensorCtrl::Track ConvertToTrackGrp(ESensorTrack track)
    {
      if (track == ESensorTrack::BLACK)
      {
        return CIGI::V40::SensorCtrl::Track::eTrack_Black;
      }
      else if (track == ESensorTrack::WHITE)
      {
        return CIGI::V40::SensorCtrl::Track::eTrack_White;
      }
      return CIGI::V40::SensorCtrl::Track::eTrack_White;
    }

    CIGI::V40::WaveCtrl::BreakerType ConvertToBreakerGrp(EWaveBreakerType breaker)
    {
      if (breaker == EWaveBreakerType::PLUNGING)
      {
        return CIGI::V40::WaveCtrl::BreakerType::eBreakerType_Plunging;
      }
      else if (breaker == EWaveBreakerType::SPILLING)
      {
        return CIGI::V40::WaveCtrl::BreakerType::eBreakerType_Spilling;
      }
      else if (breaker == EWaveBreakerType::SURGING)
      {
        return CIGI::V40::WaveCtrl::BreakerType::eBreakerType_Surging;
      }
      return CIGI::V40::WaveCtrl::BreakerType::eBreakerType_Plunging;
    }

    CIGI::V40::WaveCtrl::Scope ConvertToWaveScope(ECigiScope eCigiScope)
    {
      switch (eCigiScope)
      {
      default:
      case ECigiScope::UNKNOWN:
      {
        return CIGI::V40::WaveCtrl::Scope::eScope_Global;
      }
      case ECigiScope::ENTITY:
      {
        return CIGI::V40::WaveCtrl::Scope::eScope_Entity;
      }
      case ECigiScope::GLOBAL:
      {
        return CIGI::V40::WaveCtrl::Scope::eScope_Global;
      }
      case ECigiScope::REGIONAL:
      {
        return CIGI::V40::WaveCtrl::Scope::eScope_Regional;
      }
      }
    }

    std::string EnabledToString(bool enabled)
    {
      if (enabled)
      {
        return "Enabled";
      }
      return "Disabled";
    }

    std::string EnableToString(bool enable)
    {
      if (enable)
      {
        return "Enable";
      }
      return "Disable";
    }

    std::string ValidToString(bool valid)
    {
      if (valid)
      {
        return "Valid";
      }
      return "Invalid";
    }

    std::string VisibleToString(bool visible)
    {
      if (visible)
      {
        return "Visible";
      }
      return "Occluded";
    }

    std::string ConvertAttachStateToString(bool attached)
    {
      if (attached)
      {
        return "Attach";
      }
      return "Detach";
    }

    CIGI::V40::SymbolCtrl::SymbolState ConvertToSymbolStateGrp(ESymbolState eSymbolControlState)
    {
      switch (eSymbolControlState)
      {
      default:
      case ESymbolState::UNKNOWN:
      {
        return CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Hidden;
      }
      case ESymbolState::DESTROYED:
      {
        return CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Destroyed;
      }
      case ESymbolState::HIDDEN:
      {
        return CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Hidden;
      }
      case ESymbolState::VISIBLE:
      {
        return CIGI::V40::SymbolCtrl::SymbolState::eSymbolState_Visible;
      }
      }
    }

    CIGI::V40::SymbolCtrl::AttachState ConvertToAttachState(EAttachState eAttachState)
    {
      switch (eAttachState)
      {
      default:
      case EAttachState::UNKNOWN:
      {
        return CIGI::V40::SymbolCtrl::AttachState::eAttachState_Detach;
      }
      case EAttachState::ATTACH:
      {
        return CIGI::V40::SymbolCtrl::AttachState::eAttachState_Attach;
      }
      case EAttachState::DETACH:
      {
        return CIGI::V40::SymbolCtrl::AttachState::eAttachState_Detach;
      }
      }
    }

    CIGI::V40::SymbolCtrl::FlashControl ConvertToFlashCtrlGrp(EFlashControl eFlashControl)
    {
      switch (eFlashControl)
      {
      default:
      case EFlashControl::UNKNOWN:
      {
        return CIGI::V40::SymbolCtrl::FlashControl::eFlashControl_Reset;
      }
      case EFlashControl::RESET:
      {
        return CIGI::V40::SymbolCtrl::FlashControl::eFlashControl_Reset;
      }
      case EFlashControl::CONTINUE:
      {
        return CIGI::V40::SymbolCtrl::FlashControl::eFlashControl_Continue;
      }
      }
    }

    CIGI::V40::SymbolCtrl::InheritColor ConvertToInheritColorGrp(bool bInheritColor)
    {
      if (bInheritColor)
      {
        return CIGI::V40::SymbolCtrl::InheritColor::eInheritColor_Inherited;
      }

      return CIGI::V40::SymbolCtrl::InheritColor::eInheritColor_NotInherited;
    }

    CIGI::V40::LineOfSightSegmentRequest::RequestType ConvertToLOSRequestType(bool type)
    {
      if (type)
      {
        return CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Extended;
      }
      return CIGI::V40::LineOfSightSegmentRequest::RequestType::eRequestType_Basic;
    }

    CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem ConvertToCoordSysGrp(ETopLevelCoordinateSystem coordSys)
    {
      if (coordSys == ETopLevelCoordinateSystem::ENTITY)
      {
        return CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Entity;
      }
      else if (coordSys == ETopLevelCoordinateSystem::GEODETIC)
      {
        return CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      return CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    }

    CIGI::V40::PositionRequest::CoordinateSystem ConvertToCoordSysGrp(EObjectCoordinateSystem eObjectCoordinateSystem)
    {
      switch (eObjectCoordinateSystem)
      {
      default:
      case EObjectCoordinateSystem::UNKNOWN:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      case EObjectCoordinateSystem::LOCAL:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Submodel;
      }
      case EObjectCoordinateSystem::PARENT:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_ParentEntity;
      }
      case EObjectCoordinateSystem::WORLD:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      }
    }

    CIGI::V40::SymbolCircleDefinition::DrawingStyle ConvertToDrawingStyleGrp(EDrawingStyle drawingStyle)
    {
      if (drawingStyle == EDrawingStyle::LINE)
      {
        return CIGI::V40::SymbolCircleDefinition::DrawingStyle::eDrawingStyle_Line;
      }
      return CIGI::V40::SymbolCircleDefinition::DrawingStyle::eDrawingStyle_Fill;
    }

    CIGI::V40::HATHOTRequest::CoordinateSystem ConvertToHatHotCoordSys(ETopLevelCoordinateSystem coordSys)
    {

      if (coordSys == ETopLevelCoordinateSystem::ENTITY)
      {
        return CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Entity;
      }
      else if (coordSys == ETopLevelCoordinateSystem::GEODETIC)
      {
        return CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      return CIGI::V40::HATHOTRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    }

    CIGI::V40::HATHOTRequest::Type ConvertToRequestType(ERequestType requestType)
    {
      if (requestType == ERequestType::HAT)
      {
        return CIGI::V40::HATHOTRequest::Type::eType_HAT;
      }
      else if (requestType == ERequestType::HOT)
      {
        return CIGI::V40::HATHOTRequest::Type::eType_HOT;
      }
      else if (requestType == ERequestType::EXTENDED)
      {
        return CIGI::V40::HATHOTRequest::Type::eType_Extended;
      }
      return CIGI::V40::HATHOTRequest::Type::eType_HAT;
    }

    CIGI::V40::EnvironmentalRegionCtrl::RegionState ConvertToRegionStateGrp(EActiveState state)
    {
      if (state == EActiveState::ACTIVE)
      {
        return CIGI::V40::EnvironmentalRegionCtrl::RegionState::eRegionState_Active;
      }
      else if (state == EActiveState::DESTROYED)
      {
        return CIGI::V40::EnvironmentalRegionCtrl::RegionState::eRegionState_Destroyed;
      }
      else if (state == EActiveState::INACTIVE_STANDBY)
      {
        return CIGI::V40::EnvironmentalRegionCtrl::RegionState::eRegionState_Inactive;
      }
      return CIGI::V40::EnvironmentalRegionCtrl::RegionState::eRegionState_Inactive;
    }

    CIGI::V40::EnvironmentalRegionCtrl::Merge ConvertToMergeControlGrp(EMergeState eMergeState)
    {
      if (eMergeState == EMergeState::MERGE)
      {
        return CIGI::V40::EnvironmentalRegionCtrl::Merge::eMerge_Merge;
      }
      return CIGI::V40::EnvironmentalRegionCtrl::Merge::eMerge_UseLast;
    }

    CIGI::V40::LineOfSightVectorRequest::CoordinateSystem ConvertToVectCoordSysGrp(ETopLevelCoordinateSystem coordSys)
    {
      if (coordSys == ETopLevelCoordinateSystem::ENTITY)
      {
        return CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Entity;
      }
      else if (coordSys == ETopLevelCoordinateSystem::GEODETIC)
      {
        return CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      return CIGI::V40::LineOfSightVectorRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
    }

    CIGI::V40::PositionRequest::CoordinateSystem ConvertToCigiCoordSys(EObjectCoordinateSystem eObjectCoordinateSystem)
    {
      switch (eObjectCoordinateSystem)
      {
      case EObjectCoordinateSystem::UNKNOWN:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      case EObjectCoordinateSystem::LOCAL:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Submodel;
      }
      case EObjectCoordinateSystem::PARENT:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_ParentEntity;
      }
      case EObjectCoordinateSystem::WORLD:
      {
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
      default:
        return CIGI::V40::PositionRequest::CoordinateSystem::eCoordinateSystem_Geodetic;
      }
    }

    CIGI::V40::PositionRequest::UpdateMode ConvertToUpdateModeGrp(EUpdateMode eUpdateMode)
    {
      switch (eUpdateMode)
      {
      default:
      case EUpdateMode::UNKNOWN:
      {
        return CIGI::V40::PositionRequest::UpdateMode::eUpdateMode_OneShot;
        break;
      }
      case EUpdateMode::ONE_SHOT:
      {
        return CIGI::V40::PositionRequest::UpdateMode::eUpdateMode_OneShot;
        break;
      }
      case EUpdateMode::CONTINUOUS:
      {
        return CIGI::V40::PositionRequest::UpdateMode::eUpdateMode_Continuous;
        break;
      }
      }
    }

    CIGI::V40::PositionRequest::ObjectClass GetCigiObjectClassGrp(EObjectClass eObjectClass)
    {
      switch (eObjectClass)
      {
      default:
      case EObjectClass::UNKNOWN:
      {
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_Entity;
        break;
      }
      case EObjectClass::ENTITY:
      {
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_Entity;
        break;
      }
      case EObjectClass::ARTICULATED_PART:
      {
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ArticulatedPart;
        break;
      }
      case EObjectClass::VIEW:
      {
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_View;
        break;
      }
      case EObjectClass::VIEW_GROUP:
      {
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_ViewGroup;
        break;
      }
      case EObjectClass::MOTION_TRACKER:
      {
        return CIGI::V40::PositionRequest::ObjectClass::eObjectClass_MotionTracker;
        break;
      }
      }
    }

    CIGI::V40::SoF::IGMode ConvertFrom(EIGMode igMode)
    {
      switch (igMode)
      {
      case EIGMode::RESET://same as Standby
      {
        return CIGI::V40::SoF::IGMode::eIGMode_Reset;
      }
      case EIGMode::OPERATE:
      {
        return CIGI::V40::SoF::IGMode::eIGMode_Operate;
      }
      case EIGMode::DEBUG:
      {
        return CIGI::V40::SoF::IGMode::eIGMode_Debug;
      }
      case EIGMode::MAINTENANCE:
      {
        return CIGI::V40::SoF::IGMode::eIGMode_OfflineMaintenance;
      }
      }

      return CIGI::V40::SoF::IGMode::eIGMode_Reset;
    }

    sbio::cigi::EIGMode ConvertToIGMode33(CIGI::V33::IGCtrl::IGMode eIGMode)
    {
      switch (eIGMode)
      {
      default:
        return EIGMode::UNKNOWN;
      case CIGI::V33::IGCtrl::IGMode::eIGMode_Reset:
        return EIGMode::RESET;
      case CIGI::V33::IGCtrl::IGMode::eIGMode_Debug:
        return EIGMode::DEBUG;
      case CIGI::V33::IGCtrl::IGMode::eIGMode_Operate:
        return EIGMode::OPERATE;
      }
    }

    sbio::cigi::EIGMode ConvertToIGMode33(CIGI::V33::SoF::IGMode eIGMode)
    {
      switch (eIGMode)
      {
      default:
        return EIGMode::UNKNOWN;
      case CIGI::V33::SoF::IGMode::eIGMode_Reset:
        return EIGMode::RESET;
      case CIGI::V33::SoF::IGMode::eIGMode_Debug:
        return EIGMode::DEBUG;
      case CIGI::V33::SoF::IGMode::eIGMode_Operate:
        return EIGMode::OPERATE;
      }
    }

    std::string ConvertCigiVersionToString(ECigiVersion eCigiVersion)
    {
      switch (eCigiVersion)
      {
      case ECigiVersion::VERSION_3_3:
        return "3.3";
      case ECigiVersion::VERSION_4_0:
        return "4.0";
      default:
        return "Unknown";
      }
    }

    std::string ConvertCigiSynchronizationModeToString(ECigiSynchronizationMode eSynchronizationMode)
    {
      switch (eSynchronizationMode)
      {
      case ECigiSynchronizationMode::SYNCHRONOUS:
        return "Synchronous";
      case ECigiSynchronizationMode::ASYNCHRONOUS:
        return "Asynchronous";
      default:
        return "Unknown";
      }
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
