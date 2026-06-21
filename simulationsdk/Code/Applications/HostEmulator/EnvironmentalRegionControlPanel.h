//Copyright SimBlocks LLC 2016-2026
/**
 * @file EnvironmentalRegionControlPanel.h
 * @brief Declares the CEnvironmentalRegionControlPanel class for environmental region control in the HostEmulator application.
 *
 * Provides the CEnvironmentalRegionControlPanel class for managing environmental region control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CEnvironmentalRegionControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ENVIRONMENTAL_REGION_CONTROL_PANEL_H
#define SIMBLOCKS_ENVIRONMENTAL_REGION_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Environmental region control panel for managing region packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending environmental region control packets.
 */
class CEnvironmentalRegionControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the environmental region control panel.
   * @param pParentWindow Parent window pointer.
   */
  CEnvironmentalRegionControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending environmental region control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
