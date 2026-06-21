//Copyright SimBlocks LLC 2016-2026
/**
 * @file ArticulatedPartControlPanel.h
 * @brief Declares the CArticulatedPartControlPanel class for articulated part control in the HostEmulator application.
 *
 * Provides the CArticulatedPartControlPanel class for managing articulated part control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CArticulatedPartControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ARTICULATED_PART_CONTROL_PANEL_H
#define SIMBLOCKS_ARTICULATED_PART_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Articulated part control panel for managing articulated part packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending articulated part control packets.
 */
class CArticulatedPartControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the articulated part control panel.
   * @param pParentWindow Parent window pointer.
   */
  CArticulatedPartControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending articulated part control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
