//Copyright SimBlocks LLC 2016-2026
/**
 * @file WaveControlPanel.h
 * @brief Declares the CWaveControlPanel class for wave control in the HostEmulator application.
 *
 * Provides the CWaveControlPanel class for managing wave control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CWaveControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_WAVE_CONTROL_PANEL_H
#define SIMBLOCKS_WAVE_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Wave control panel for managing wave packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending wave control packets.
 */
class CWaveControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the wave control panel.
   * @param pParentWindow Parent window pointer.
   */
  CWaveControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending wave control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
