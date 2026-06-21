//Copyright SimBlocks LLC 2016-2026
/**
 * @file SensorControlPanel.h
 * @brief Declares the CSensorControlPanel class for sensor control in the HostEmulator application.
 *
 * Provides the CSensorControlPanel class for managing sensor control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CSensorControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SENSOR_CONTROL_PANEL_H
#define SIMBLOCKS_SENSOR_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Sensor control panel for managing sensor packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending sensor control packets.
 */
class CSensorControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the sensor control panel.
   * @param pParentWindow Parent window pointer.
   */
  CSensorControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending sensor control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
