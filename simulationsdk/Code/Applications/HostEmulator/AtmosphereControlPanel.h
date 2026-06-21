//Copyright SimBlocks LLC 2016-2026
/**
 * @file AtmosphereControlPanel.h
 * @brief Declares the CAtmosphereControlPanel class for atmosphere control in the HostEmulator application.
 *
 * Provides the CAtmosphereControlPanel class for managing atmosphere control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CAtmosphereControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ATMOSPHERE_CONTROL_PANEL_H
#define SIMBLOCKS_ATMOSPHERE_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Atmosphere control panel for managing atmosphere packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending atmosphere control packets.
 */
class CAtmosphereControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the atmosphere control panel.
   * @param pParentWindow Parent window pointer.
   */
  CAtmosphereControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending atmosphere control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
