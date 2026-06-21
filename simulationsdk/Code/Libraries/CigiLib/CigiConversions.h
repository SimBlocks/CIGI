//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiConversions.h
 * @brief Conversion utilities between CIGI protocol types and internal SDK types.
 *
 * Provides functions for converting between CIGI protocol types (from libCIGI) and internal simulation SDK types.
 * Includes conversion for rotations, coordinates, symbol types, enums, and other protocol-specific data structures.
 * Supports CIGI 3.3 and 4.0 packet types, symbol and view system integration, and geospatial math conversions.
 *
 * @see sbio::math
 * @see sbio::symbol
 * @see CIGI::V40
 * @see CIGI::V33
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_CONVERSIONS
#define SIMBLOCKS_CIGI_CONVERSIONS

#include "CigiLib/CigiTypeDeclarations.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "GlobalHeaders/CommonTypes.h"
#include "MathLib/MathTypes.h"
#include "libCIGI/Packets/4_0/AccelerationCtrl.h"
#include "libCIGI/Packets/4_0/AerosolConcentrationResponse.h"
#include "libCIGI/Packets/4_0/AnimationCtrl.h"
#include "libCIGI/Packets/4_0/AnimationStopNotification.h"
#include "libCIGI/Packets/4_0/ArticulatedPartCtrl.h"
#include "libCIGI/Packets/4_0/AtmosphereCtrl.h"
#include "libCIGI/Packets/4_0/CelestialSphereCtrl.h"
#include "libCIGI/Packets/4_0/CollisionDetectionSegmentDefinition.h"
#include "libCIGI/Packets/4_0/CollisionDetectionSegmentNotification.h"
#include "libCIGI/Packets/4_0/CollisionDetectionVolumeDefinition.h"
#include "libCIGI/Packets/4_0/CollisionDetectionVolumeNotification.h"
#include "libCIGI/Packets/4_0/ComponentCtrl.h"
#include "libCIGI/Packets/4_0/ConformalClampedEntityPosition.h"
#include "libCIGI/Packets/4_0/EarthReferenceModelDefinition.h"
#include "libCIGI/Packets/4_0/EntityCtrl.h"
#include "libCIGI/Packets/4_0/EntityPosition.h"
#include "libCIGI/Packets/4_0/EnvironmentalConditionsRequest.h"
#include "libCIGI/Packets/4_0/EnvironmentalRegionCtrl.h"
#include "libCIGI/Packets/4_0/EventNotification.h"
#include "libCIGI/Packets/4_0/HATHOTExtendedResponse.h"
#include "libCIGI/Packets/4_0/HATHOTRequest.h"
#include "libCIGI/Packets/4_0/HATHOTResponse.h"
#include "libCIGI/Packets/4_0/IGCtrl.h"
#include "libCIGI/Packets/4_0/IGMessage.h"
#include "libCIGI/Packets/4_0/LineOfSightExtendedResponse.h"
#include "libCIGI/Packets/4_0/LineOfSightResponse.h"
#include "libCIGI/Packets/4_0/LineOfSightSegmentRequest.h"
#include "libCIGI/Packets/4_0/LineOfSightVectorRequest.h"
#include "libCIGI/Packets/4_0/MaritimeSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/4_0/MaritimeSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/MotionTrackerCtrl.h"
#include "libCIGI/Packets/4_0/PositionRequest.h"
#include "libCIGI/Packets/4_0/PositionResponse.h"
#include "libCIGI/Packets/4_0/SensorCtrl.h"
#include "libCIGI/Packets/4_0/SensorExtendedResponse.h"
#include "libCIGI/Packets/4_0/SensorResponse.h"
#include "libCIGI/Packets/4_0/ShortArticulatedPartCtrl.h"
#include "libCIGI/Packets/4_0/ShortComponentCtrl.h"
#include "libCIGI/Packets/4_0/ShortSymbolCtrl.h"
#include "libCIGI/Packets/4_0/SoF.h"
#include "libCIGI/Packets/4_0/SymbolCircleDefinition.h"
#include "libCIGI/Packets/4_0/SymbolClone.h"
#include "libCIGI/Packets/4_0/SymbolCtrl.h"
#include "libCIGI/Packets/4_0/SymbolPolygonDefinition.h"
#include "libCIGI/Packets/4_0/SymbolSurfaceDefinition.h"
#include "libCIGI/Packets/4_0/SymbolTextDefinition.h"
#include "libCIGI/Packets/4_0/SymbolTexturedCircleDefinition.h"
#include "libCIGI/Packets/4_0/SymbolTexturedPolygonDefinition.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsResponse.h"
#include "libCIGI/Packets/4_0/VelocityCtrl.h"
#include "libCIGI/Packets/4_0/ViewCtrl.h"
#include "libCIGI/Packets/4_0/ViewDefinition.h"
#include "libCIGI/Packets/4_0/WaveCtrl.h"
#include "libCIGI/Packets/4_0/WeatherConditionsResponse.h"
#include "libCIGI/Packets/4_0/WeatherCtrl.h"
#include "libCIGI/Packets/3_3/IGCtrl.h"
#include "libCIGI/Packets/3_3/SoF.h"
#include "libCIGI/Packets/3_3/LineOfSightSegmentRequest.h"
#include "libCIGI/Packets/3_3/LineOfSightVectorRequest.h"

namespace sbio
{
  namespace cigi
  {
    /// @name Rotation and transform conversions
    /// These helpers bridge between SDK math types and the body-frame conventions used by CIGI packets.
    /// @{
    /**
     * @brief Converts to body euler rotation.
     * @param cigiBodyEulerRotation Rotation value.
     * @return Converted value.
     */
    sbio::math::TBodyEulerRotation ConvertToBodyEulerRotation(const sbio::cigi::TCigiBodyEulerRotation& cigiBodyEulerRotation);
    /**
     * @brief Converts to cigi body euler rotation.
     * @param bodyEulerRotation Rotation value.
     * @return Converted value.
     */
    sbio::cigi::TCigiBodyEulerRotation ConvertToCigiBodyEulerRotation(const sbio::math::TBodyEulerRotation& bodyEulerRotation);
    /**
     * @brief Converts body coordinates to cigi body coordinates.
     * @param bodyCoordinates Coordinate value.
     * @return Converted value.
     */
    sbio::cigi::CigiBodyCoordinates ConvertBodyCoordinatesToCigiBodyCoordinates(const sbio::math::BodyCoordinates& bodyCoordinates);
    /**
     * @brief Converts cigi body coordinates to body coordinates.
     * @param vOffsetBodyCoordinates Coordinate value.
     * @return Converted value.
     */
    sbio::math::BodyCoordinates ConvertCigiBodyCoordinatesToBodyCoordinates(sbio::cigi::CigiBodyCoordinates vOffsetBodyCoordinates);
    /**
     * @brief Converts cigi body coordinates to body coordinates.
     * @param transform Transform value.
     * @return Converted value.
     */
    sbio::math::TBodyTransform ConvertCigiBodyCoordinatesToBodyCoordinates(sbio::cigi::TCigiBodyTransform transform);
    /**
     * @brief Converts cigi body rotation to body rotation.
     * @param cigiBodyRotation Rotation value.
     * @return Converted value.
     */
    sbio::math::TBodyRotation ConvertCigiBodyRotationToBodyRotation(sbio::cigi::TCigiBodyRotation cigiBodyRotation);
    /**
     * @brief Converts body rotation to cigi body rotation.
     * @param bodyRotation Rotation value.
     * @return Converted value.
     */
    sbio::cigi::TCigiBodyRotation ConvertBodyRotationToCigiBodyRotation(sbio::math::TBodyRotation bodyRotation);

    /**
     * @brief Creates a CIGI body-frame rotation from Euler angles.
     *
     * The resulting rotation matrix matches the object-coordinate conventions expected by the
     * entity-related CIGI packet serializers.
     * @param rotation Euler rotation used to build the body-frame rotation.
     * @return CIGI body-frame rotation matrix.
     */
    sbio::cigi::TCigiBodyRotation SetupCigiObjectRotation(const sbio::cigi::TCigiBodyEulerRotation& rotation);

    /**
     * @brief Builds a top-level geocentric rotation for an entity located at a geodetic position.
     * @param rotation Body-frame Euler rotation for the entity.
     * @param latitude Geodetic latitude of the entity.
     * @param longitude Geodetic longitude of the entity.
     * @return Geocentric rotation for the top-level entity.
     */
    sbio::math::TGeocentricRotation SetupTopLevelEntityRotation(const sbio::cigi::TCigiBodyEulerRotation& rotation, sbio::math::Latitude latitude, sbio::math::Longitude longitude);

    /**
     * @brief Builds a full geocentric transform for a top-level entity pose.
     * @param geodeticCoordinates Geodetic position of the entity.
     * @param rotation Body-frame Euler rotation for the entity.
     * @return Geocentric transform for the top-level entity pose.
     */
    sbio::math::TGeocentricTransform SetupTopLevelTransformation(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::TCigiBodyEulerRotation& rotation);
    /// @}

    /// @name Scalar and enum conversions
    /// @{
    /**
     * @brief Converts alpha to percentage.
     * @param fAlpha Alpha floating-point value.
     * @return Converted value.
     */
    sbio::Percentage ConvertAlphaToPercentage(float fAlpha);
    /**
     * @brief Converts alpha to percentage.
     * @param nAlpha Alpha numeric value.
     * @return Converted value.
     */
    sbio::Percentage ConvertAlphaToPercentage(int nAlpha);
    /**
     * @brief Converts degree of freedom.
     * @param eDOF Dof enumeration value.
     * @return Converted value.
     */
    sbio::EDegreeOfFreedom ConvertDegreeOfFreedom(CIGI::V40::ShortArticulatedPartCtrl::DOFSelect eDOF);
    /**
     * @brief Converts text alignment.
     * @param eAlignment Alignment enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ETextAlignment ConvertTextAlignment(CIGI::V40::SymbolTextDefinition::Alignment eAlignment);
    /**
     * @brief Converts text orientation.
     * @param eOrientation Orientation enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ETextOrientation ConvertTextOrientation(CIGI::V40::SymbolTextDefinition::Orientation eOrientation);
    /**
     * @brief Converts region.
     * @param eRegionState Region state enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EActiveState ConvertRegion(CIGI::V40::EnvironmentalRegionCtrl::RegionState eRegionState);
    /**
     * @brief Converts scope.
     * @param eScope Scope enumeration value.
     * @return Converted value.
     */
    sbio::cigi::ECigiScope ConvertScope(CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope eScope);
    /**
     * @brief Converts scope.
     * @param eScope Scope enumeration value.
     * @return Converted value.
     */
    sbio::cigi::ECigiScope ConvertScope(CIGI::V40::WaveCtrl::Scope eScope);
    /**
     * @brief Converts scope.
     * @param eScope Scope enumeration value.
     * @return Converted value.
     */
    sbio::cigi::ECigiScope ConvertScope(CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope eScope);
    /**
     * @brief Converts primitive grp.
     * @param ePrimitiveGrp Primitive grp enumeration value.
     * @return Converted value.
     */
    sbio::symbol::EPrimitiveType ConvertPrimitiveGrp(CIGI::V40::SymbolPolygonDefinition::PrimitiveType ePrimitiveGrp);
    /**
     * @brief Converts primitive grp.
     * @param ePrimitive Primitive enumeration value.
     * @return Converted value.
     */
    sbio::symbol::EPrimitiveType ConvertPrimitiveGrp(CIGI::V40::SymbolTexturedPolygonDefinition::PrimitiveType ePrimitive);
    /**
     * @brief Converts projection mode.
     * @param eProjection Projection enumeration value.
     * @return Converted value.
     */
    sbio::EProjectionMode ConvertProjectionMode(CIGI::V40::ViewDefinition::ProjectionType eProjection);
    /**
     * @brief Converts mirror mode.
     * @param eMirror Mirror enumeration value.
     * @return Converted value.
     */
    sbio::EMirrorMode ConvertMirrorMode(CIGI::V40::ViewDefinition::MirrorMode eMirror);
    /**
     * @brief Converts symbol state.
     * @param eSymbolState Symbol state enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ESymbolState ConvertSymbolState(CIGI::V40::SymbolCtrl::SymbolState eSymbolState);
    /**
     * @brief Converts attach state.
     * @param eAttachState Attach state enumeration value.
     * @return Converted value.
     */
    sbio::EAttachState ConvertAttachState(CIGI::V40::EntityPosition::AttachState eAttachState);
    /**
     * @brief Converts attach state.
     * @param eAttachState Attach state enumeration value.
     * @return Converted value.
     */
    sbio::EAttachState ConvertAttachState(CIGI::V40::SymbolCtrl::AttachState eAttachState);
    /**
     * @brief Converts collision volume type.
     * @param eVolumeType Volume type enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EVolumeType ConvertCollisionVolumeType(CIGI::V40::CollisionDetectionVolumeDefinition::VolumeType eVolumeType);
    /**
     * @brief Converts merge state.
     * @param eMergeState Merge state enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EMergeState ConvertMergeState(CIGI::V40::EnvironmentalRegionCtrl::Merge eMergeState);
    /**
     * @brief Converts coordinate system.
     * @param eCoordSysGroup Coord sys group enumeration value.
     * @return Converted value.
     */
    sbio::EObjectCoordinateSystem ConvertCoordinateSystem(CIGI::V40::VelocityCtrl::CoordinateSystem eCoordSysGroup);
    /**
     * @brief Converts coordinate system.
     * @param eCoordinateSystem Coordinate system enumeration value.
     * @return Converted value.
     */
    sbio::EObjectCoordinateSystem ConvertCoordinateSystem(CIGI::V40::PositionRequest::CoordinateSystem eCoordinateSystem);
    /**
     * @brief Converts coordinate system.
     * @param eCoordSysGroup Coord sys group enumeration value.
     * @return Converted value.
     */
    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V33::LineOfSightSegmentRequest::CoordinateSystem eCoordSysGroup);
    /**
     * @brief Converts coordinate system.
     * @param eCoordSysGroup Coord sys group enumeration value.
     * @return Converted value.
     */
    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem eCoordSysGroup);
    /**
     * @brief Converts coordinate system.
     * @param eCoordSysGroup Coord sys group enumeration value.
     * @return Converted value.
     */
    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V33::LineOfSightVectorRequest::CoordinateSystem eCoordSysGroup);
    /**
     * @brief Converts coordinate system.
     * @param eCoordSysGroup Coord sys group enumeration value.
     * @return Converted value.
     */
    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V40::LineOfSightVectorRequest::CoordinateSystem eCoordSysGroup);
    /**
     * @brief Converts coordinate system.
     * @param eCoordSysGroup Coord sys group enumeration value.
     * @return Converted value.
     */
    sbio::ETopLevelCoordinateSystem ConvertCoordinateSystem(CIGI::V40::HATHOTRequest::CoordinateSystem eCoordSysGroup);
    /**
     * @brief Converts to surface state.
     * @param eSurfaceState Surface state enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ESurfaceState ConvertToSurfaceState(CIGI::V40::SymbolSurfaceDefinition::SurfaceState eSurfaceState);
    /**
     * @brief Converts object class.
     * @param eObjectClass Object class enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EObjectClass ConvertObjectClass(CIGI::V40::PositionRequest::ObjectClass eObjectClass);
    /**
     * @brief Converts object class.
     * @param eObjectClass Object class enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EObjectClass ConvertObjectClass(CIGI::V40::PositionResponse::ObjectClass eObjectClass);
    /**
     * @brief Converts object class.
     * @param eObjectClass Object class enumeration value.
     * @return Converted value.
     */
    CIGI::V40::PositionRequest::ObjectClass ConvertObjectClass(EObjectClass eObjectClass);
    /**
     * @brief Converts texture filter mode.
     * @param eTextureFilterMode Texture filter mode enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ETextureFilterMode ConvertTextureFilterMode(CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode eTextureFilterMode);
    /**
     * @brief Converts texture filter mode.
     * @param eTextureFilterMode Texture filter mode enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ETextureFilterMode ConvertTextureFilterMode(CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode eTextureFilterMode);
    /**
     * @brief Converts texture wrap mode.
     * @param eTextureWrapMode Texture wrap mode enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ETextureWrapMode ConvertTextureWrapMode(CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp eTextureWrapMode);
    /**
     * @brief Converts texture wrap mode.
     * @param eTextureWrapMode Texture wrap mode enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ETextureWrapMode ConvertTextureWrapMode(CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp eTextureWrapMode);
    /**
     * @brief Converts request type.
     * @param eRequestType Request type enumeration value.
     * @return Converted value.
     */
    sbio::cigi::ERequestType ConvertRequestType(CIGI::V40::HATHOTRequest::Type eRequestType);
    /**
     * @brief Converts drawing style.
     * @param drawingStyle Drawing style value.
     * @return Converted value.
     */
    sbio::symbol::EDrawingStyle ConvertDrawingStyle(CIGI::V40::SymbolCircleDefinition::DrawingStyle drawingStyle);
    /**
     * @brief Converts symbol source type.
     * @param eSymbolSource Symbol source enumeration value.
     * @return Converted value.
     */
    sbio::symbol::ESymbolSourceType ConvertSymbolSourceType(CIGI::V40::SymbolClone::SourceType eSymbolSource);

    /**
     * @brief Converts cigi alignment to string.
     * @param eAlignment Alignment enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiAlignmentToString(sbio::symbol::ETextAlignment eAlignment);
    /**
     * @brief Converts cigi orientation to string.
     * @param eOrientation Orientation enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiOrientationToString(sbio::symbol::ETextOrientation eOrientation);
    /**
     * @brief Converts to alignment.
     * @param align Align value.
     * @return Converted value.
     */
    CIGI::V40::SymbolTextDefinition::Alignment ConvertToAlignment(sbio::symbol::ETextAlignment align);
    /**
     * @brief Converts to billboard.
     * @param billboard Billboard value.
     * @return Converted value.
     */
    CIGI::V40::SymbolSurfaceDefinition::Billboard ConvertToBillboard(sbio::symbol::EBillboard billboard);
    /**
     * @brief Converts to orientation.
     * @param orientation Orientation value.
     * @return Converted value.
     */
    CIGI::V40::SymbolTextDefinition::Orientation ConvertToOrientation(sbio::symbol::ETextOrientation orientation);
    /**
     * @brief Converts to surface state.
     * @param eSurfaceState Surface state enumeration value.
     * @return Converted value.
     */
    CIGI::V40::SymbolSurfaceDefinition::SurfaceState ConvertToSurfaceState(sbio::symbol::ESurfaceState eSurfaceState);
    /**
     * @brief Converts to texture filter mode.
     * @param mode Mode value.
     * @return Converted value.
     */
    CIGI::V40::SymbolTexturedCircleDefinition::TextureFilterMode ConvertToTextureFilterMode(sbio::symbol::ETextureFilterMode mode);
    /**
     * @brief Converts to textured circle texture wrap mode.
     * @param mode Mode value.
     * @return Converted value.
     */
    CIGI::V40::SymbolTexturedCircleDefinition::TextureRepeatOrClamp ConvertToTexturedCircleTextureWrapMode(sbio::symbol::ETextureWrapMode mode);
    /**
     * @brief Converts to texture filter.
     * @param filter Filter value.
     * @return Converted value.
     */
    CIGI::V40::SymbolTexturedPolygonDefinition::TextureFilterMode ConvertToTextureFilter(sbio::symbol::ETextureFilterMode filter);
    /**
     * @brief Converts to textured polygon texture wrap mode.
     * @param mode Mode value.
     * @return Converted value.
     */
    CIGI::V40::SymbolTexturedPolygonDefinition::TextureRepeatOrClamp ConvertToTexturedPolygonTextureWrapMode(sbio::symbol::ETextureWrapMode mode);
    /**
     * @brief Converts inherit alpha to string.
     * @param inheritAlpha Inherit alpha value.
     * @return Converted value.
     */
    std::string ConvertInheritAlphaToString(bool inheritAlpha);
    /**
     * @brief Converts cigi version to string.
     * @param eCigiVersion CIGI version enumeration value.
     * @return Converted value.
     */
    std::string ConvertECigiVersionToString(ECigiVersion eCigiVersion);
    /**
     * @brief Converts IG mode to string.
     * @param eIGMode IG mode enumeration value.
     * @return Converted value.
     */
    std::string ConvertEIGModeToString(EIGMode eIGMode);
    /**
     * @brief Converts object class to string.
     * @param eObjectClass Object class enumeration value.
     * @return Converted value.
     */
    std::string ConvertEObjectClassToString(EObjectClass eObjectClass);
    /**
     * @brief Converts sensor status to string.
     * @param eSensorStatus Sensor status enumeration value.
     * @return Converted value.
     */
    std::string ConvertESensorStatusToString(ESensorStatus eSensorStatus);
    /**
     * @brief Converts sensor status to string.
     * @param eSensorStatus Sensor status enumeration value.
     * @return Converted value.
     */
    std::string ConvertSensorStatusToString(CIGI::V40::SensorResponse::SensorStatus eSensorStatus);
    /**
     * @brief Converts object class to string.
     * @param eObjectClass Object class enumeration value.
     * @return Converted value.
     */
    std::string ConvertObjectClassToString(CIGI::V40::PositionResponse::ObjectClass eObjectClass);
    /**
     * @brief Converts object class to string.
     * @param objectClass Object class value.
     * @return Converted value.
     */
    std::string ConvertObjectClassToString(EObjectClass objectClass);
    /**
     * @brief Converts coord sys geo par sub to string.
     * @param eCoordSys Coord sys enumeration value.
     * @return Converted value.
     */
    std::string ConvertCoordSysGeoParSubToString(CIGI::V40::PositionResponse::CoordinateSystem eCoordSys);
    /**
     * @brief Converts coord sys geo par sub to string.
     * @param eCoordSys Coord sys enumeration value.
     * @return Converted value.
     */
    std::string ConvertCoordSysGeoParSubToString(CIGI::V40::PositionRequest::CoordinateSystem eCoordSys);
    /**
     * @brief Converts to mirror mode grp.
     * @param eMirrorMode Mirror mode enumeration value.
     * @return Converted value.
     */
    CIGI::V40::ViewDefinition::MirrorMode ConvertToMirrorModeGrp(EMirrorMode eMirrorMode);
    /**
     * @brief Converts to cigi scope maritime.
     * @param scope Scope value.
     * @return Converted value.
     */
    CIGI::V40::MaritimeSurfaceConditionsCtrl::Scope ConvertToCigiScopeMaritime(ECigiScope scope);
    /**
     * @brief Converts cigi scope to string.
     * @param scope Scope value.
     * @return Converted value.
     */
    std::string ConvertCigiScopeToString(ECigiScope scope);
    /**
     * @brief Converts to track mode grp.
     * @param mode Mode value.
     * @return Converted value.
     */
    CIGI::V40::SensorCtrl::TrackMode ConvertToTrackModeGrp(ETrackMode mode);
    /**
     * @brief Converts to polarity grp.
     * @param polarity Polarity value.
     * @return Converted value.
     */
    CIGI::V40::SensorCtrl::Polarity ConvertToPolarityGrp(EPolarity polarity);
    /**
     * @brief Converts to track grp.
     * @param polarity Polarity value.
     * @return Converted value.
     */
    CIGI::V40::SensorCtrl::Track ConvertToTrackGrp(ESensorTrack polarity);
    /**
     * @brief Converts to breaker grp.
     * @param breaker Breaker value.
     * @return Converted value.
     */
    CIGI::V40::WaveCtrl::BreakerType ConvertToBreakerGrp(EWaveBreakerType breaker);
    /**
     * @brief Converts to wave scope.
     * @param eCigiScope Cigi scope enumeration value.
     * @return Converted value.
     */
    CIGI::V40::WaveCtrl::Scope ConvertToWaveScope(ECigiScope eCigiScope);
    /**
     * @brief Converts attach state to string.
     * @param attached Attached value.
     * @return Converted value.
     */
    std::string ConvertAttachStateToString(bool attached);
    /**
     * @brief Converts to symbol state grp.
     * @param eSymbolControlState Symbol control state enumeration value.
     * @return Converted value.
     */
    CIGI::V40::SymbolCtrl::SymbolState ConvertToSymbolStateGrp(sbio::symbol::ESymbolState eSymbolControlState);
    /**
     * @brief Converts to attach state.
     * @param eAttachState Attach state enumeration value.
     * @return Converted value.
     */
    CIGI::V40::SymbolCtrl::AttachState ConvertToAttachState(EAttachState eAttachState);
    /**
     * @brief Converts to flash ctrl grp.
     * @param eFlashControl Flash control enumeration value.
     * @return Converted value.
     */
    CIGI::V40::SymbolCtrl::FlashControl ConvertToFlashCtrlGrp(sbio::symbol::EFlashControl eFlashControl);
    /**
     * @brief Converts to inherit color grp.
     * @param bInheritColor Whether inherit color.
     * @return Converted value.
     */
    CIGI::V40::SymbolCtrl::InheritColor ConvertToInheritColorGrp(bool bInheritColor);
    /**
     * @brief Converts to losrequest type.
     * @param type Type value.
     * @return Converted value.
     */
    CIGI::V40::LineOfSightSegmentRequest::RequestType ConvertToLOSRequestType(bool type);
    /**
     * @brief Converts to coord sys grp.
     * @param coordSys Coord sys value.
     * @return Converted value.
     */
    CIGI::V40::LineOfSightSegmentRequest::CoordinateSystem ConvertToCoordSysGrp(ETopLevelCoordinateSystem coordSys);
    /**
     * @brief Converts to coord sys grp.
     * @param eObjectCoordinateSystem Object coordinate system enumeration value.
     * @return Converted value.
     */
    CIGI::V40::PositionRequest::CoordinateSystem ConvertToCoordSysGrp(EObjectCoordinateSystem eObjectCoordinateSystem);
    /**
     * @brief Converts to drawing style grp.
     * @param drawingStyle Drawing style value.
     * @return Converted value.
     */
    CIGI::V40::SymbolCircleDefinition::DrawingStyle ConvertToDrawingStyleGrp(sbio::symbol::EDrawingStyle drawingStyle);
    /**
     * @brief Converts to hat hot coord sys.
     * @param coordSys Coord sys value.
     * @return Converted value.
     */
    CIGI::V40::HATHOTRequest::CoordinateSystem ConvertToHatHotCoordSys(ETopLevelCoordinateSystem coordSys);
    /**
     * @brief Converts to request type.
     * @param requestType Request type value.
     * @return Converted value.
     */
    CIGI::V40::HATHOTRequest::Type ConvertToRequestType(ERequestType requestType);
    /**
     * @brief Converts to region state grp.
     * @param state State value.
     * @return Converted value.
     */
    CIGI::V40::EnvironmentalRegionCtrl::RegionState ConvertToRegionStateGrp(EActiveState state);
    /**
     * @brief Converts to merge control grp.
     * @param eMergeState Merge state enumeration value.
     * @return Converted value.
     */
    CIGI::V40::EnvironmentalRegionCtrl::Merge ConvertToMergeControlGrp(EMergeState eMergeState);
    /**
     * @brief Converts to vect coord sys grp.
     * @param coordSys Coord sys value.
     * @return Converted value.
     */
    CIGI::V40::LineOfSightVectorRequest::CoordinateSystem ConvertToVectCoordSysGrp(ETopLevelCoordinateSystem coordSys);
    /**
     * @brief Converts to cigi coord sys.
     * @param eObjectCoordinateSystem Object coordinate system enumeration value.
     * @return Converted value.
     */
    CIGI::V40::PositionRequest::CoordinateSystem ConvertToCigiCoordSys(EObjectCoordinateSystem eObjectCoordinateSystem);
    /**
     * @brief Converts to update mode grp.
     * @param eUpdateMode Update mode enumeration value.
     * @return Converted value.
     */
    CIGI::V40::PositionRequest::UpdateMode ConvertToUpdateModeGrp(EUpdateMode eUpdateMode);
    /**
     * @brief Gets cigi object class grp.
     * @param eObjectClass Object class enumeration value.
     * @return Cigi object class grp value.
     */
    CIGI::V40::PositionRequest::ObjectClass GetCigiObjectClassGrp(EObjectClass eObjectClass);

    /**
     * @brief Converts an SDK IG mode to the CIGI 4.0 start-of-frame representation.
      * @param igMode Ig mode value.
      * @return Converted value.
     */
    CIGI::V40::SoF::IGMode ConvertFrom(EIGMode igMode);

    /**
     * @brief Converts a CIGI 3.3 IG control mode to the SDK enum.
      * @param eIGMode Igmode enumeration value.
      * @return Converted value.
     */
    sbio::cigi::EIGMode ConvertToIGMode33(CIGI::V33::IGCtrl::IGMode eIGMode);

    /**
     * @brief Converts a CIGI 3.3 start-of-frame mode to the SDK enum.
      * @param eIGMode Igmode enumeration value.
      * @return Converted value.
     */
    sbio::cigi::EIGMode ConvertToIGMode33(CIGI::V33::SoF::IGMode eIGMode);

    /**
     * @brief Converts to flash control.
     * @param eFlashControl Flash control enumeration value.
     * @return Converted value.
     */
    sbio::symbol::EFlashControl ConvertToFlashControl(CIGI::V40::SymbolCtrl::FlashControl eFlashControl);
    /**
     * @brief Converts to cigi alpha.
     * @param alpha Alpha value.
     * @return Converted value.
     */
    uint8_t ConvertToCigiAlpha(sbio::Percentage alpha);
    /**
     * @brief Converts to cigi float percentage.
     * @param percentage Percentage value.
     * @return Converted value.
     */
    sbio::cigi::CigiFloatPercentage ConvertToCigiFloatPercentage(sbio::Percentage percentage);
    /**
     * @brief Converts to cigi int percentage.
     * @param percentage Percentage value.
     * @return Converted value.
     */
    sbio::cigi::CigiIntPercentage ConvertToCigiIntPercentage(sbio::Percentage percentage);
    /**
     * @brief Converts to cigi month.
     * @param month Month value.
     * @return Converted value.
     */
    sbio::cigi::CigiMonth ConvertToCigiMonth(sbio::Month month);
    /**
     * @brief Converts to cigi terrestrail surface conditions severity.
     * @param percentage Percentage value.
     * @return Converted value.
     */
    sbio::cigi::CigiTerrestrialSurfaceSeverity ConvertToCigiTerrestrailSurfaceConditionsSeverity(sbio::Percentage percentage);
    /**
     * @brief Converts to month.
     * @param cigiMonth Cigi month value.
     * @return Converted value.
     */
    sbio::Month ConvertToMonth(sbio::cigi::CigiMonth cigiMonth);
    /**
     * @brief Converts to percentage.
     * @param cigiPercentage Cigi percentage value.
     * @return Converted value.
     */
    sbio::Percentage ConvertToPercentage(sbio::cigi::CigiFloatPercentage cigiPercentage);
    /**
     * @brief Converts to percentage.
     * @param cigiPercentage Cigi percentage value.
     * @return Converted value.
     */
    sbio::Percentage ConvertToPercentage(sbio::cigi::CigiIntPercentage cigiPercentage);
    /**
     * @brief Converts to percentage.
     * @param cigiTerrestrialSurfaceSeverity Cigi terrestrial surface severity value.
     * @return Converted value.
     */
    sbio::Percentage ConvertToPercentage(sbio::cigi::CigiTerrestrialSurfaceSeverity cigiTerrestrialSurfaceSeverity);
    /// @}

    /// @name Diagnostic string helpers
    /// @{
    std::string EnabledToString(bool enabled);
    /**
     * @brief Enables to string.
     * @param enable Enable value.
     * @return String representation of the enabled state.
     */
    std::string EnableToString(bool enable);
    /**
     * @brief Returns the result of `ValidToString`.
     * @param valid Valid value.
     * @return String representation of the valid state.
     */
    std::string ValidToString(bool valid);
    /**
     * @brief Returns the result of `VisibleToString`.
     * @param visible Visible value.
     * @return String representation of the visible state.
     */
    std::string VisibleToString(bool visible);
    /// @}

    std::string ConvertCigiVersionToString(ECigiVersion eCigiVersion);
    /**
     * @brief Converts cigi synchronization mode to string.
     * @param eSynchronizationMode Synchronization mode enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiSynchronizationModeToString(ECigiSynchronizationMode eSynchronizationMode);
  }
}

#endif /* CIGI_CONVERSIONS */
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
