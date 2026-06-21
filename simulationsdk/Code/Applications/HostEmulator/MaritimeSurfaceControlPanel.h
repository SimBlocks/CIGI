//Copyright SimBlocks LLC 2016-2026
/**
 * @file MaritimeSurfaceControlPanel.h
 * @brief Declares the CMaritimeSurfaceControlPanel class for maritime surface control in the HostEmulator application.
 *
 * Provides the CMaritimeSurfaceControlPanel class for managing maritime surface control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CMaritimeSurfaceControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_MARITIME_SURFACE_CONTROL_PANEL_H
#define SIMBLOCKS_MARITIME_SURFACE_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Maritime surface control panel for managing maritime surface packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending maritime surface control packets.
 */
class CMaritimeSurfaceControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the maritime surface control panel.
   * @param pParentWindow Parent window pointer.
   */
  CMaritimeSurfaceControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending maritime surface control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
