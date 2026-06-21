//Copyright SimBlocks LLC 2016-2026
/**
 * @file PositionRequestPanel.h
 * @brief Declares the CPositionRequestPanel class for position request control in the HostEmulator application.
 *
 * Provides the CPositionRequestPanel class for managing position request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CPositionRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_POSITION_REQUEST_PANEL_H
#define SIMBLOCKS_POSITION_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Position request panel for managing position request packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending position request packets.
 */
class CPositionRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the position request panel.
   * @param pParentWindow Parent window pointer.
   */
  CPositionRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending position request packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
