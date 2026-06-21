//Copyright SimBlocks LLC 2016-2026
/**
 * @file CollisionDetectionVolumeDefinitionPanel.h
 * @brief Declares the CCollisionDetectionVolumeDefinitionPanel class for collision detection volume control in the HostEmulator application.
 *
 * Provides the CCollisionDetectionVolumeDefinitionPanel class for managing collision detection volume packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CCollisionDetectionVolumeDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_COLLISION_DETECTION_VOLUME_DEFINITION_PANEL_H
#define SIMBLOCKS_COLLISION_DETECTION_VOLUME_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Collision detection volume definition panel for managing volume packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending collision detection volume definition packets.
 */
class CCollisionDetectionVolumeDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the collision detection volume definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CCollisionDetectionVolumeDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending collision detection volume definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Handles the volume type selection event.
   * @param event The command event.
   */
  void OnVolumeType(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
