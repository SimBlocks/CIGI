//Copyright SimBlocks LLC 2016-2026
/**
 * @file AccelerationControlPanel.h
 * @brief Declares the CAccelerationControlPanel class for acceleration control in the HostEmulator application.
 *
 * Provides the CAccelerationControlPanel class for managing acceleration control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CAccelerationControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ACCELERATION_CONTROL_PANEL_H
#define SIMBLOCKS_ACCELERATION_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Acceleration control panel for managing acceleration packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending acceleration control packets.
 */
class CAccelerationControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the acceleration control panel.
   * @param pParentWindow Parent window pointer.
   */
  CAccelerationControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending acceleration control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
