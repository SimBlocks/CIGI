//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiLib.h
 * @brief Core declarations and conversion utilities for the CigiLib module.
 *
 * Provides conversion functions, type declarations, and protocol integration for CIGI (Common Image Generator Interface)
 * protocol support. Includes conversion between CIGI protocol types and internal SDK types, as well as utility functions
 * for opcodes, entity types, coordinate systems, and protocol-specific enums. Integrates with GeographicLib for geospatial
 * math and supports CIGI 3.3 and 4.0 packet types.
 *
 * @see sbio::cigi namespace
 * @see CIGI::V40
 * @see CIGI::V33
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_CIGI_LIB_H
#define SIMBLOCKS_CIGI_CIGI_LIB_H

#include "CigiTypeDeclarations.h"
#include "libCIGI/Packets/4_0/EntityCtrl.h"
#include "libCIGI/Packets/4_0/EntityPosition.h"
#include "libCIGI/Packets/4_0/IGCtrl.h"
#include "libCIGI/Packets/4_0/SoF.h"
#include "libCIGI/Packets/4_0/ShortArticulatedPartCtrl.h"
#include "libCIGI/Packets/4_0/TerrestrialSurfaceConditionsCtrl.h"
#include "libCIGI/Packets/4_0/HATHOTResponse.h"
#include "CigiLib/CigiTypes.h"
#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/TransverseMercator.hpp>

#include <memory>
#include <string>

namespace sbio
{
  namespace cigi
  {
    /**
     * @brief Initializes module-level state used by `CigiLib`.
     *
     * Side effects:
     * - Populates internal lookup tables used by opcode and enum-to-string helpers.
     * - Prepares any process-wide resources expected by the library's conversion routines.
     */
    void InitCigiLib();

    /**
     * @brief Returns a display name for a CIGI 3.3 opcode.
     * @param eOpCode Opcode value to format.
     * @return Human-readable opcode name used by logs and diagnostics.
     */
    std::string GetCigiOpCodeName(const sbio::cigi::ECigiOpCodeV3& eOpCode);

    /**
     * @brief Returns a display name for a CIGI 4.0 opcode.
     * @param eOpCode Opcode value to format.
     * @return Human-readable opcode name used by logs and diagnostics.
     */
    std::string GetCigiOpCodeName(const sbio::cigi::ECigiOpCodeV4& eOpCode);

    /**
     * @brief Builds the opcode-name lookup tables used by `GetCigiOpCodeName()`.
     */
    void SetupCigiOpCodeNames();

    /// @name Core protocol enum conversions
    /// Helpers in this section translate between libCIGI enums and SDK-owned enums used by the rest of the codebase.
    /// @{
    /**
     * @brief Converts to active state.
     * @param eEntityState Entity state enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EActiveState ConvertToActiveState(CIGI::V40::EntityCtrl::EntityState eEntityState);
    /**
     * @brief Converts to clamp.
     * @param eClamp Clamp enumeration value.
     * @return Converted value.
     */
    CIGI::V40::EntityPosition::GroundOceanClamp ConvertToClamp(sbio::EClamp eClamp);
    /**
     * @brief Converts to dofselect.
     * @param eDOF Dof enumeration value.
     * @return Converted value.
     */
    CIGI::V40::ShortArticulatedPartCtrl::DOFSelect ConvertToDOFSelect(sbio::EDegreeOfFreedom eDOF);
    /**
     * @brief Converts cigi dofselect to string.
     * @param eDOF Dof enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiDOFSelectToString(sbio::EDegreeOfFreedom eDOF);
    /**
     * @brief Converts to igmode.
     * @param eIGMode Igmode enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EIGMode ConvertToIGMode(CIGI::V40::IGCtrl::IGMode eIGMode);
    /**
     * @brief Converts to igmode.
     * @param eIGMode Igmode enumeration value.
     * @return Converted value.
     */
    sbio::cigi::EIGMode ConvertToIGMode(CIGI::V40::SoF::IGMode eIGMode);
    /**
     * @brief Converts response type to string.
     * @param eResponseType Response type enumeration value.
     * @return Converted value.
     */
    std::string ConvertResponseTypeToString(CIGI::V40::HATHOTResponse::Type eResponseType);
    /**
     * @brief Converts to terrestrial surface scope.
     * @param scope Scope value.
     * @return Converted value.
     */
    CIGI::V40::TerrestrialSurfaceConditionsCtrl::Scope ConvertToTerrestrialSurfaceScope(sbio::cigi::ECigiScope scope);

    /**
     * @brief Converts an SDK IG mode to a user-facing string.
      * @param eIGMode Igmode enumeration value.
      * @return Converted value.
     */
    std::string ConvertToString(sbio::cigi::EIGMode eIGMode);
    /// @}

    /// @name Text parsing helpers
    /// These helpers parse configuration and script strings into strongly typed CIGI-related values.
    /// @{
    /**
     * @brief Converts to extended entity type.
     * @param type Type value.
     * @return Converted value.
     */
    CIGI::V40::EntityCtrl::ExtendedEntityType ConvertToExtendedEntityType(sbio::cigi::EExtendedEntityType type);
    /**
     * @brief Converts cigi extended entity type to string.
     * @param type Type value.
     * @return Converted value.
     */
    std::string ConvertCigiExtendedEntityTypeToString(const EExtendedEntityType& type);
    /**
     * @brief Converts cigi string to coordinate system.
     * @param sCoordinateSystem Coordinate system text value.
     * @return Converted value.
     */
    sbio::ETopLevelCoordinateSystem ConvertCigiStringToCoordinateSystem(const std::string& sCoordinateSystem);
    /**
     * @brief Converts cigi string to degree of freedom.
     * @param sDegreeOfFreedom Degree of freedom text value.
     * @return Converted value.
     */
    sbio::EDegreeOfFreedom ConvertCigiStringToDegreeOfFreedom(const std::string& sDegreeOfFreedom);
    /**
     * @brief Converts cigi string to active state.
     * @param sActiveState Active state text value.
     * @return Converted value.
     */
    sbio::cigi::EActiveState ConvertCigiStringToActiveState(const std::string& sActiveState);
    /**
     * @brief Converts cigi string to merge state.
     * @param sMergeState Merge state text value.
     * @return Converted value.
     */
    sbio::cigi::EMergeState ConvertCigiStringToMergeState(const std::string& sMergeState);
    /**
     * @brief Converts cigi merge state to string.
     * @param eMergeState Merge state enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiMergeStateToString(const EMergeState& eMergeState);
    /**
     * @brief Converts cigi layer idto string.
     * @param value Value value.
     * @return Converted value.
     */
    std::string ConvertCigiLayerIDToString(const uint8_t value);
    /**
     * @brief Converts cigi string to object coordinate system.
     * @param sCoordinateSystem Coordinate system text value.
     * @return Converted value.
     */
    sbio::EObjectCoordinateSystem ConvertCigiStringToObjectCoordinateSystem(const std::string& sCoordinateSystem);
    /**
     * @brief Converts cigi ob coord sys to string.
     * @param eCoordinateSystem Coordinate system enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiObCoordSysToString(const EObjectCoordinateSystem& eCoordinateSystem);
    /**
     * @brief Converts cigi string to frame position reset.
     * @param sAnimationFramePositionReset Animation frame position reset text value.
     * @return Converted value.
     */
    sbio::cigi::EAnimationFramePositionReset ConvertCigiStringToFramePositionReset(const std::string& sAnimationFramePositionReset);
    /**
     * @brief Converts cigi animation frame position reset to string.
     * @param eAnimationFramePositionReset Animation frame position reset enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiAnimationFramePositionResetToString(const EAnimationFramePositionReset& eAnimationFramePositionReset);
    /**
     * @brief Converts cigi string to animation state.
     * @param sAnimationState Animation state text value.
     * @return Converted value.
     */
    sbio::cigi::EAnimationState ConvertCigiStringToAnimationState(const std::string& sAnimationState);
    /**
     * @brief Converts cigi string to animation loop mode.
     * @param sAnimationLoopMode Animation loop mode text value.
     * @return Converted value.
     */
    sbio::cigi::EAnimationLoopMode ConvertCigiStringToAnimationLoopMode(const std::string& sAnimationLoopMode);
    /**
     * @brief Converts cigi string to request type.
     * @param sRequestType Request type text value.
     * @return Converted value.
     */
    sbio::cigi::ERequestType ConvertCigiStringToRequestType(const std::string& sRequestType);
    /**
     * @brief Converts cigi request type to string.
     * @param eRequestType Request type enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiRequestTypeToString(const ERequestType& eRequestType);
    /**
     * @brief Converts cigi string to scope.
     * @param sScope Scope text value.
     * @return Converted value.
     */
    sbio::cigi::ECigiScope ConvertCigiStringToScope(const std::string& sScope);
    /**
     * @brief Converts cigi animation state to string.
     * @param eAnimationState Animation state enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiAnimationStateToString(const EAnimationState& eAnimationState);
    /**
     * @brief Converts cigi animation loop mode to string.
     * @param eAnimationLoopMode Animation loop mode enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiAnimationLoopModeToString(const EAnimationLoopMode& eAnimationLoopMode);
    /**
     * @brief Converts cigi coordinate system to string.
     * @param eCoordinateSystem Coordinate system enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiCoordinateSystemToString(const ETopLevelCoordinateSystem& eCoordinateSystem);
    /**
     * @brief Converts cigi string to igmode.
     * @param sIGMode Igmode text value.
     * @return Converted value.
     */
    sbio::cigi::EIGMode ConvertCigiStringToIGMode(const std::string& sIGMode);
    /**
     * @brief Converts cigi string to mirror mode.
     * @param sMirrorMode Mirror mode text value.
     * @return Converted value.
     */
    sbio::EMirrorMode ConvertCigiStringToMirrorMode(const std::string& sMirrorMode);
    /**
     * @brief Converts cigi mirror mode to string.
     * @param eMirrorMode Mirror mode enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiMirrorModeToString(const EMirrorMode& eMirrorMode);
    /**
     * @brief Converts cigi pixel rep mode to string.
     * @param pixelMode Pixel mode value.
     * @return Converted value.
     */
    std::string ConvertCigiPixelRepModeToString(const uint8_t pixelMode);
    /**
     * @brief Converts cigi string to object class.
     * @param sObjectClass Object class text value.
     * @return Converted value.
     */
    sbio::cigi::EObjectClass ConvertCigiStringToObjectClass(const std::string& sObjectClass);
    /**
     * @brief Converts cigi object class to string.
     * @param eObjectClass Object class enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiObjectClassToString(const EObjectClass& eObjectClass);
    /**
     * @brief Converts cigi string to symbol state.
     * @param ssymbolState Ssymbol state value.
     * @return Converted value.
     */
    sbio::symbol::ESymbolState ConvertCigiStringToSymbolState(const std::string& ssymbolState);
    /**
     * @brief Converts cigi symbol state to string.
     * @param eSymbolState Symbol state enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiSymbolStateToString(sbio::symbol::ESymbolState eSymbolState);
    /**
     * @brief Converts cigi string to polarity.
     * @param sPolarity Polarity text value.
     * @return Converted value.
     */
    sbio::cigi::EPolarity ConvertCigiStringToPolarity(const std::string& sPolarity);
    /**
     * @brief Converts cigi polarity to string.
     * @param ePolarity Polarity enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiPolarityToString(const EPolarity& ePolarity);
    /**
     * @brief Converts cigi active state to string.
     * @param eActiveState Active state enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiActiveStateToString(const EActiveState& eActiveState);
    /**
     * @brief Converts cigi string to sensor track.
     * @param sSensorTrack Sensor track text value.
     * @return Converted value.
     */
    sbio::cigi::ESensorTrack ConvertCigiStringToSensorTrack(const std::string& sSensorTrack);
    /**
     * @brief Converts cigi string to update mode.
     * @param sUpdateMode Update mode text value.
     * @return Converted value.
     */
    sbio::cigi::EUpdateMode ConvertCigiStringToUpdateMode(const std::string& sUpdateMode);
    /**
     * @brief Converts cigi string to volume type.
     * @param sVolumeType Volume type text value.
     * @return Converted value.
     */
    sbio::cigi::EVolumeType ConvertCigiStringToVolumeType(const std::string& sVolumeType);
    /**
     * @brief Converts cigi volume type to string.
     * @param eVolumeType Volume type enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiVolumeTypeToString(const EVolumeType& eVolumeType);
    /**
     * @brief Converts cigi string to wave breaker type.
     * @param sWaveBreakerType Wave breaker type text value.
     * @return Converted value.
     */
    sbio::cigi::EWaveBreakerType ConvertCigiStringToWaveBreakerType(const std::string& sWaveBreakerType);
    /**
     * @brief Converts cigi string to track mode.
     * @param sTrackMode Track mode text value.
     * @return Converted value.
     */
    sbio::cigi::ETrackMode ConvertCigiStringToTrackMode(const std::string& sTrackMode);
    /**
     * @brief Converts cigi track mode to string.
     * @param eTrackMode Track mode enumeration value.
     * @return Converted value.
     */
    std::string ConvertCigiTrackModeToString(const ETrackMode& eTrackMode);
    /**
     * @brief Converts cigi string to surface state.
     * @param sSurfaceState Surface state text value.
     * @return Converted value.
     */
    sbio::symbol::ESurfaceState ConvertCigiStringToSurfaceState(const std::string& sSurfaceState);
    /**
     * @brief Converts cigi string to attach type.
     * @param sAttachType Attach type text value.
     * @return Converted value.
     */
    sbio::symbol::EAttachType ConvertCigiStringToAttachType(const std::string& sAttachType);
    /**
     * @brief Converts cigi string to billboard.
     * @param sBillboard Billboard text value.
     * @return Converted value.
     */
    sbio::symbol::EBillboard ConvertCigiStringToBillboard(const std::string& sBillboard);
    /**
     * @brief Converts cigi string to alignment.
     * @param sAlignment Alignment text value.
     * @return Converted value.
     */
    sbio::symbol::ETextAlignment ConvertCigiStringToAlignment(const std::string& sAlignment);
    /**
     * @brief Converts cigi string to orientation.
     * @param sOrientation Orientation text value.
     * @return Converted value.
     */
    sbio::symbol::ETextOrientation ConvertCigiStringToOrientation(const std::string& sOrientation);
    /**
     * @brief Converts cigi font idto string.
     * @param fontID Font id value.
     * @return Converted value.
     */
    std::string ConvertCigiFontIDToString(const uint8_t& fontID);
    /**
     * @brief Converts cigi string to drawing style.
     * @param sDrawingStyle Drawing style text value.
     * @return Converted value.
     */
    sbio::symbol::EDrawingStyle ConvertCigiStringToDrawingStyle(const std::string& sDrawingStyle);
    /**
     * @brief Converts cigi string to primitive polygon.
     * @param sPrimitiveType Primitive type text value.
     * @return Converted value.
     */
    sbio::symbol::EPrimitiveType ConvertCigiStringToPrimitivePolygon(const std::string& sPrimitiveType);
    /**
     * @brief Converts cigi string to attribute select.
     * @param sAttributeSelect Attribute select text value.
     * @return Converted value.
     */
    sbio::symbol::EAttributeSelect ConvertCigiStringToAttributeSelect(const std::string& sAttributeSelect);
    /**
     * @brief Converts cigi string to esymbol state.
     * @param sSymbolState Symbol state text value.
     * @return Converted value.
     */
    sbio::symbol::ESymbolState ConvertCigiStringToESymbolState(const std::string& sSymbolState);
    /**
     * @brief Converts cigi string to texture filter mode.
     * @param sTextureMode Texture mode text value.
     * @return Converted value.
     */
    sbio::symbol::ETextureFilterMode ConvertCigiStringToTextureFilterMode(const std::string& sTextureMode);
    /**
     * @brief Converts cigi string to texture wrap mode.
     * @param sTextureRepeat Texture repeat text value.
     * @return Converted value.
     */
    sbio::symbol::ETextureWrapMode ConvertCigiStringToTextureWrapMode(const std::string& sTextureRepeat);
    /**
     * @brief Converts cigi string to source type.
     * @param sourceType Source type value.
     * @return Converted value.
     */
    sbio::symbol::ESymbolSourceType ConvertCigiStringToSourceType(const std::string& sourceType);
    /**
     * @brief Converts cigi string to attach state.
     * @param attachState Attach state value.
     * @return Converted value.
     */
    sbio::EAttachState ConvertCigiStringToAttachState(const std::string& attachState);
    /**
     * @brief Converts cigi string to flash control.
     * @param flashControl Flash control value.
     * @return Converted value.
     */
    sbio::symbol::EFlashControl ConvertCigiStringToFlashControl(const std::string& flashControl);
    /**
     * @brief Converts cigi string to view type.
     * @param viewType View type value.
     * @return Converted value.
     */
    sbio::cigi::EViewType ConvertCigiStringToViewType(const std::string& viewType);
    /**
     * @brief Converts cigi string to eclamp.
     * @param clamp Clamp value.
     * @return Converted value.
     */
    sbio::EClamp ConvertCigiStringToEClamp(const std::string& clamp);
    /**
     * @brief Converts cigi eclamp to string.
     * @param eclamp Eclamp value.
     * @return Converted value.
     */
    std::string ConvertCigiEClampToString(const EClamp& eclamp);
    /// @}
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
