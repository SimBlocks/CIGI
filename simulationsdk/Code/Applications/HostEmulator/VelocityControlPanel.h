//Copyright SimBlocks LLC 2016-2026
/**
 * @file VelocityControlPanel.h
 * @brief Declares the CVelocityControlPanel class for velocity control in the HostEmulator application.
 *
 * Provides the CVelocityControlPanel class for managing velocity control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CVelocityControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_VELOCITY_CONTROL_PACKET_PANEL_H
#define SIMBLOCKS_VELOCITY_CONTROL_PACKET_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Velocity control panel for managing velocity packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending velocity control packets.
 */
class CVelocityControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the velocity control panel.
   * @param pParentWindow Parent window pointer.
   */
  CVelocityControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending velocity control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
