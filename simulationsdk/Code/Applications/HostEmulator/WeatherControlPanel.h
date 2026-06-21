//Copyright SimBlocks LLC 2016-2026
/**
 * @file WeatherControlPanel.h
 * @brief Declares the CWeatherControlPanel class for weather control in the HostEmulator application.
 *
 * Provides the CWeatherControlPanel class for managing weather control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CWeatherControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_WEATHER_CONTROL_PACKET_PANEL_H
#define SIMBLOCKS_WEATHER_CONTROL_PACKET_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Weather control panel for managing weather packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending weather control packets.
 */
class CWeatherControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the weather control panel.
   * @param pParentWindow Parent window pointer.
   */
  CWeatherControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending weather control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
