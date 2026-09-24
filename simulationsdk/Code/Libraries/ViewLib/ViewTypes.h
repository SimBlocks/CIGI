//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewTypes.h
 * @brief Declares types, enums, and strong types for view identification and projection modes.
 *
 * Provides strong type definitions and enums for view IDs, group IDs, view types, and projection modes.
 * Supports configuration and management of views and view groups in the view system.
 *
 * @see sbio::view::CView
 * @see sbio::view::CViewGroup
 * @see sbio::EProjectionMode
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_TYPES_H
#define SIMBLOCKS_VIEW_TYPES_H

#include "GlobalHeaders/CommonTypes.h"

namespace sbio
{
  /** @brief View identifier with UINT16_MAX as UnknownViewID, rejected by manager insertion and group membership. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(ViewID, uint16_t, UINT16_MAX);
  /** @brief View-group identifier; UnknownViewGroupID is rejected by manager insertion. */
  STRONG_TYPE(ViewGroupID, uint8_t);
  /** @brief Strongly typed numeric view category, distinct from view and group identifiers. */
  STRONG_TYPE(ViewType, uint8_t);

  /**
   * @brief Projection modes for views.
   */
  enum class EProjectionMode
  {
    UNKNOWN = -1,///< Unknown projection mode
    PERSPECTIVE = 0,///< Perspective projection
    ORTHOGRAPHIC,///< Orthographic projection
  };

  /** @brief Selects horizontal and/or vertical mirroring for view implementations that support it. */
  enum class EMirrorMode
  {
    UNKNOWN = -1,///< Mirror mode is unspecified.
    NONE = 0,///< No mirroring.
    HORIZONTAL,///< Horizontal mirroring only.
    VERTICAL,///< Vertical mirroring only.
    HORIZONTAL_AND_VERTICAL,///< Mirroring in both directions.
  };
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
