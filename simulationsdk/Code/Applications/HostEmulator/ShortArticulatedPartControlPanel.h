//Copyright SimBlocks LLC 2016-2026
/**
 * @file ShortArticulatedPartControlPanel.h
 * @brief Declares the CShortArticulatedPartControlPanel class for short articulated part control in the HostEmulator application.
 *
 * Provides the CShortArticulatedPartControlPanel class for managing short articulated part control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CShortArticulatedPartControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SHORT_ARTICULATED_CONTROL_PANEL_H
#define SIMBLOCKS_SHORT_ARTICULATED_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Short articulated part control panel for managing short articulated part packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending short articulated part control packets.
 */
class CShortArticulatedPartControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the short articulated part control panel.
   * @param pParentWindow Parent window pointer.
   */
  CShortArticulatedPartControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending short articulated part control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
