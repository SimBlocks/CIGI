//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolTypes.h
 * @brief Declares symbol identifiers, enums, and value types used by SymbolLib.
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_TYPES_H
#define SIMBLOCKS_SYMBOL_TYPES_H

#include "GlobalHeaders/CommonTypes.h"
#include "EntityLib/EntityTypes.h"
#include "MathLib/MathTypes.h"
#include "ViewLib/ViewTypes.h"
#include <vector>
#include <limits>

#ifdef max
#undef max
#endif

using namespace sbio::math;

namespace sbio
{
  namespace symbol
  {
    /** @brief Strong identifier for a symbol. */
    STRONG_TYPE(SymbolID, uint16_t);
    /** @brief Strong identifier for a symbol surface; the maximum `uint16_t` value represents unknown. */
    STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(SymbolSurfaceID, uint16_t, std::numeric_limits<uint16_t>::max());

    /**
     * @brief Lifecycle state for a symbol surface.
     */
    enum class ESurfaceState
    {
      UNKNOWN = -1,
      ACTIVE,
      DESTROYED
    };

    /**
     * @brief Target type for a symbol surface attachment.
     */
    enum class EAttachType
    {
      UNKNOWN = -1,
      ENTITY,
      VIEW
    };

    /**
     * @brief Billboard mode for entity-attached symbol surfaces.
     */
    enum class EBillboard
    {
      UNKNOWN = -1,
      NON_BILLBOARD,
      BILLBOARD
    };

    /**
     * @brief Defines a symbol surface and its texture-coordinate bounds.
     */
    struct SSymbolSurfaceDefinition
    {
      sbio::symbol::SymbolSurfaceID surfaceID = sbio::symbol::UnknownSymbolSurfaceID;///< Symbol surface identifier.
      ESurfaceState eSurfaceState = ESurfaceState::UNKNOWN;///< Current surface state.
      float width = 0;///< Surface width.
      float height = 0;///< Surface height.
      sbio::math::Vec2f uvMin;///< Minimum UV coordinate on the surface.
      sbio::math::Vec2f uvMax;///< Maximum UV coordinate on the surface.
    };

    /**
     * @brief Defines a symbol surface attached to an entity with orientation offsets.
     */
    struct SEntitySymbolSurfaceDefinition : SSymbolSurfaceDefinition
    {
      sbio::EntityID entityID = UnknownEntityID;///< Entity that owns the surface.
      sbio::math::Vec3f offsetToSurface;///< Offset from the entity reference point to the surface.
      sbio::math::Degrees yaw = UnknownDegrees;///< Yaw angle applied to the surface.
      sbio::math::Degrees90 pitch = UnknownDegrees90;///< Pitch angle applied to the surface.
      sbio::math::Degrees180 roll = UnknownDegrees180;///< Roll angle applied to the surface.
    };

    /**
     * @brief Defines an entity-attached billboard symbol surface.
     */
    struct SEntityBillboardSymbolSurfaceDefinition : SSymbolSurfaceDefinition
    {
      sbio::EntityID entityID = UnknownEntityID;///< Entity that owns the billboard surface.
      bool bPerspectiveGrowthEnabled = false;///< `true` when perspective growth is enabled.
      sbio::math::Vec3f offsetToEntity;///< Offset from the center of the surface to the entity reference point.
    };

    /**
     * @brief Defines a symbol surface positioned relative to a view viewport.
     */
    struct SViewSymbolSurfaceDefinition : SSymbolSurfaceDefinition
    {
      sbio::ViewID viewID = UnknownViewID;///< View that owns the surface.
      float fLeft = 0;///< Distance from the viewport left edge to the surface left boundary as a fraction of viewport width.
      float fRight = 0;///< Distance from the viewport left edge to the surface right boundary as a fraction of viewport width.
      float fTop = 0;///< Distance from the viewport bottom edge to the surface top boundary as a fraction of viewport height.
      float fBottom = 0;///< Distance from the viewport bottom edge to the surface bottom boundary as a fraction of viewport height.
    };

    /**
     * @brief Base symbol definition containing the symbol identifier.
     */
    struct SSymbol
    {
      sbio::symbol::SymbolID symbolID = UnknownSymbolID;///< Symbol identifier.
    };

    /**
     * @brief Text alignment within a text symbol's layout region.
     */
    enum class ETextAlignment
    {
      UNKNOWN = -1,
      TOP_LEFT,
      TOP_CENTER,
      TOP_RIGHT,
      CENTER_LEFT,
      CENTER,
      CENTER_RIGHT,
      BOTTOM_LEFT,
      BOTTOM_CENTER,
      BOTTOM_RIGHT,
    };

    /**
     * @brief Text layout direction for a text symbol.
     */
    enum class ETextOrientation
    {
      UNKNOWN = -1,
      LEFT_TO_RIGHT,
      TOP_TO_BOTTOM,
      RIGHT_TO_LEFT,
      BOTTOM_TO_TOP,
    };

    /**
     * @brief Defines a text symbol and text layout attributes.
     */
    struct SSymbolTextDefinition : SSymbol
    {
      sbio::FontID fontID = UnknownFontID;///< Font identifier.
      float fFontSize = 0;///< Font size.
      ETextAlignment eTextAlignment = ETextAlignment::UNKNOWN;///< Text alignment.
      ETextOrientation eTextOrientation = ETextOrientation::UNKNOWN;///< Text orientation.
      std::string sText;///< Text content.
    };

    /**
     * @brief Drawing style for untextured geometric symbols.
     */
    enum class EDrawingStyle
    {
      UNKNOWN = -1,
      LINE,
      FILL
    };

    /**
     * @brief Defines one circle or annular circle element in UV space.
     */
    struct SCircleProperties
    {
      sbio::STextureCoordinateUV centerUV;///< Center point in UV coordinates.
      float fRadius = 0;///< Outer radius.
      float fInnerRadius = 0;///< Inner radius.
      Degrees startAngle = UnknownDegrees;///< Start angle.
      Degrees endAngle = UnknownDegrees;///< End angle.
    };

    /**
     * @brief Defines a circle symbol made from one or more circle elements.
     */
    struct SSymbolCircle : SSymbol
    {
      uint16_t stipplePattern = 0;///< Line stipple pattern.
      EDrawingStyle eDrawingStyle = EDrawingStyle::UNKNOWN;///< Drawing style for the circle symbol.
      float fLineWidth = 0;///< Line width.
      float fStipplePatternLength = 0;///< Stipple pattern length.
      std::vector<SCircleProperties> circles;///< Circle elements.
    };

    /**
     * @brief Primitive topology used to interpret symbol vertices.
     */
    enum class EPrimitiveType
    {
      UNKNOWN = -1,
      POINT,
      LINE,
      LINE_STRIP,
      LINE_LOOP,
      TRIANGLE,
      TRIANGLE_STRIP,
      TRIANGLE_FAN,
    };

    /**
     * @brief Defines a polygon symbol with vertices and line drawing attributes.
     */
    struct SSymbolPolygon : SSymbol
    {
      uint16_t nStipplePattern = 0;///< Line stipple pattern.
      EPrimitiveType ePrimitiveType = EPrimitiveType::UNKNOWN;///< Primitive type used to interpret vertices.
      float fLineWidth = 0;///< Line width.
      float fStipplePatternLength = 0;///< Stipple pattern length.
      std::vector<sbio::math::Vec2f> vertices;///< Polygon vertices in surface coordinates.
    };

    /**
     * @brief Source category used by a symbol clone.
     */
    enum class ESymbolSourceType
    {
      UNKNOWN = -1,
      SYMBOL,
      SYMBOL_TEMPLATE,
    };

    /**
     * @brief Defines a symbol cloned from another symbol or symbol template.
     */
    struct SSymbolClone : SSymbol
    {
      SymbolID sourceID = UnknownSymbolID;///< Source symbol or template identifier.
      ESymbolSourceType eSymbolSourceType = ESymbolSourceType::UNKNOWN;///< Type of source referenced by `sourceID`.
    };

    /**
     * @brief Visibility and lifecycle state for a symbol.
     */
    enum class ESymbolState
    {
      UNKNOWN = -1,
      HIDDEN,
      VISIBLE,
      DESTROYED,
    };

    /**
     * @brief Flash-cycle control mode for symbol updates.
     */
    enum class EFlashControl
    {
      UNKNOWN = -1,
      CONTINUE = 0,
      RESET
    };

    /**
     * @brief Full control state for positioning, visibility, color, scale, and flashing of a symbol.
     */
    struct SSymbolControl
    {
      sbio::symbol::SymbolID symbolID = UnknownSymbolID;///< Symbol to control.
      ESymbolState eSymbolState = ESymbolState::UNKNOWN;///< Requested symbol state.
      sbio::symbol::SymbolID parentSymbolID = UnknownSymbolID;///< Parent symbol identifier.
      EAttachState eAttachState = EAttachState::UNKNOWN;///< Attachment state relative to the parent symbol.
      EFlashControl eFlashControl = EFlashControl::UNKNOWN;///< Flash state update selector.
      bool bInheritColor = false;///< `true` when the symbol inherits color from its parent.
      sbio::symbol::SymbolSurfaceID surfaceID = UnknownSymbolSurfaceID;///< Surface on which the symbol is displayed.
      uint8_t nLayerID = 0;///< Rendering layer identifier.
      Percentage flashDutyCyclePercentage = UnknownPercentage;///< Flash duty cycle percentage.
      float fFlashPeriod = 0;///< Flash period in seconds.
      float fPositionU = 0;///< Symbol U position on the surface.
      float fPositionV = 0;///< Symbol V position on the surface.
      Degrees fRotation = UnknownDegrees;///< Symbol rotation angle.
      SColor32 color;///< Symbol color.
      float fScaleU = 0;///< Symbol scale in U.
      float fScaleV = 0;///< Symbol scale in V.
      bool bPositionSet = true;///< `true` when position fields should be applied.
      bool bScaleSet = true;///< `true` when scale fields should be applied.
      bool bSetColor = true;///< `true` when `color` should be applied.
    };

    /**
     * @brief Attribute selector for compact symbol-control updates.
     */
    enum class EAttributeSelect
    {
      UNKNOWN = -1,
      NONE,
      SURFACEID,
      PARENTSYMBOLID,
      LAYER,
      FLASHDUTYCYCLEPERCENTAGE,
      FLASHPERIOD,
      POSITIONU,
      POSITIONV,
      ROTATION,
      COLOR,
      SCALEU,
      SCALEV
    };

    /**
     * @brief Value storage for one compact symbol-control attribute.
     */
    union SAttributeValue
    {
      /// Integer attribute value.
      uint32_t n;
      /// Floating-point attribute value.
      float f;
      /// Color attribute value.
      SColor32 rgba;
    };

    /**
     * @brief Compact symbol control state carrying up to two selected attribute updates.
     */
    struct SShortSymbolControl
    {
      sbio::symbol::SymbolID symbolID = UnknownSymbolID;///< Symbol to control.
      ESymbolState eSymbolState = ESymbolState::UNKNOWN;///< Requested symbol state.
      sbio::EAttachState eAttachState = sbio::EAttachState::UNKNOWN;///< Attachment state relative to the parent symbol.
      EFlashControl eFlashControl = EFlashControl::UNKNOWN;///< Flash state update selector.
      bool bInheritColor = false;///< `true` when the symbol inherits color from its parent.
      EAttributeSelect eAttributeSelect1 = EAttributeSelect::UNKNOWN;///< Attribute represented by `attributeValue1`.
      EAttributeSelect eAttributeSelect2 = EAttributeSelect::UNKNOWN;///< Attribute represented by `attributeValue2`.
      SAttributeValue attributeValue1 = {0};///< First selected attribute value.
      SAttributeValue attributeValue2 = {0};///< Second selected attribute value.
    };

    /**
     * @brief Texture sampling filter mode.
     */
    enum class ETextureFilterMode
    {
      UNKNOWN = -1,
      NEAREST,
      LINEAR,
    };

    /**
     * @brief Texture coordinate wrapping mode.
     */
    enum class ETextureWrapMode
    {
      UNKNOWN = -1,
      REPEAT,
      CLAMP,
    };

    /**
     * @brief Defines one textured circle or annular circle element.
     */
    struct STexturedCircleProperties
    {
      sbio::math::Vec2f centerUV;///< Center point in UV coordinates.
      float fRadius = 0;///< Outer radius.
      float fInnerRadius = 0;///< Inner radius.
      sbio::math::Degrees startAngle = UnknownDegrees;///< Start angle.
      sbio::math::Degrees endAngle = UnknownDegrees;///< End angle.
      sbio::math::Vec2f centerTextureST;///< Center point in texture ST coordinates.
      float fTextureMapRadius = 0;///< Texture map radius.
      float fTextureMapRotation = 0;///< Texture map rotation.
    };

    /**
     * @brief Defines a textured circle symbol made from one or more textured circle elements.
     */
    struct SSymbolTexturedCircle : SSymbol
    {
      TextureID textureID = UnknownTextureID;///< Texture applied to the circle symbol.
      ETextureFilterMode eTextureFilter = ETextureFilterMode::UNKNOWN;///< Texture filtering mode.
      ETextureWrapMode eTextureWrap = ETextureWrapMode::UNKNOWN;///< Texture wrapping mode.
      std::vector<STexturedCircleProperties> circles;///< Textured circle elements.
    };

    /**
     * @brief Vertex for a textured polygon symbol.
     */
    struct SSymbolTexturedPolygonVertex
    {
      sbio::math::Vec2f uv;///< Vertex position in UV coordinates.
      sbio::math::Vec2f textureCoordinateST;///< Vertex texture coordinate in ST coordinates.
    };

    /**
     * @brief Defines a textured polygon symbol with vertices and texture attributes.
     */
    struct SSymbolTexturedPolygon : SSymbol
    {
      sbio::TextureID textureID = UnknownTextureID;///< Texture applied to the polygon symbol.
      EPrimitiveType ePrimitiveType = EPrimitiveType::UNKNOWN;///< Primitive type used to interpret vertices.
      ETextureFilterMode eTextureFilterMode = ETextureFilterMode::UNKNOWN;///< Texture filtering mode.
      ETextureWrapMode eTextureWrapMode = ETextureWrapMode::UNKNOWN;///< Texture wrapping mode.
      std::vector<SSymbolTexturedPolygonVertex> vertices;///< Textured polygon vertices.
    };

    /**
     * @brief Runtime category for a symbol geometry definition.
     */
    enum class ESymbolType
    {
      UNKNOWN = -1,
      CIRCLE,
      POLYGON,
      TEXT,
      TEXTURED_CIRCLE,
      TEXTURED_POLYGON,
      TEMPLATE,
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
