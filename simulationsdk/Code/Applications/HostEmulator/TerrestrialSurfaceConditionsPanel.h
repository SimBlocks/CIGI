//Copyright SimBlocks LLC 2016-2026
/**
 * @file TerrestrialSurfaceConditionsPanel.h
 * @brief Declares the CTerrestrialSurfaceConditionsPanel class for terrestrial surface conditions control in the HostEmulator application.
 *
 * Provides the CTerrestrialSurfaceConditionsPanel class for managing terrestrial surface conditions packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CTerrestrialSurfaceConditionsPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_TERRESTRIAL_SURFACE_CONDITIONS_PANEL_H
#define SIMBLOCKS_TERRESTRIAL_SURFACE_CONDITIONS_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Terrestrial surface conditions panel for managing surface conditions packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending terrestrial surface conditions packets.
 */
class CTerrestrialSurfaceConditionsPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the terrestrial surface conditions panel.
   * @param pParentWindow Parent window pointer.
   */
  CTerrestrialSurfaceConditionsPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending terrestrial surface conditions packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
