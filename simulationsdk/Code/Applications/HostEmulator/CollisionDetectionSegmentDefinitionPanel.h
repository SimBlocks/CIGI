//Copyright SimBlocks LLC 2016-2026
/**
 * @file CollisionDetectionSegmentDefinitionPanel.h
 * @brief Declares the CCollisionDetectionSegmentDefinitionPanel class for collision detection segment control in the HostEmulator application.
 *
 * Provides the CCollisionDetectionSegmentDefinitionPanel class for managing collision detection segment packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CCollisionDetectionSegmentDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_COLLISION_DETECTION_SEGMENT_DEFINITION_PANEL_H
#define SIMBLOCKS_COLLISION_DETECTION_SEGMENT_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Collision detection segment definition panel for managing segment packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending collision detection segment definition packets.
 */
class CCollisionDetectionSegmentDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the collision detection segment definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CCollisionDetectionSegmentDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending collision detection segment definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
