//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityPositionPanel.h
 * @brief Declares the CEntityPositionPanel class for entity position control in the HostEmulator application.
 *
 * Provides the CEntityPositionPanel class for managing entity position packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports attach state selection and dynamic UI updates for entity position packets.
 *
 * @see CEntityPositionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_POSITION_PACKET_PANEL_H
#define SIMBLOCKS_ENTITY_POSITION_PACKET_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Entity position panel for managing entity position packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending entity position packets, including attach state selection.
 */
class CEntityPositionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the entity position panel.
   * @param pParentWindow Parent window pointer.
   */
  CEntityPositionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending entity position packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Handles the attach state selection event for updating the UI.
   * @param event The command event.
   */
  void OnAttachStateSelection(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
