//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTRequestPanel.h
 * @brief Declares the CHATHOTRequestPanel class for HAT/HOT request control in the HostEmulator application.
 *
 * Provides the CHATHOTRequestPanel class for managing Height Above Terrain/Height On Terrain (HAT/HOT) request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports radio button events for ID and coordinate system selection.
 *
 * @see CHATHOTRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_HAT_HOT_REQUEST_PANEL_H
#define SIMBLOCKS_HAT_HOT_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief HAT/HOT request panel for managing HAT/HOT packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending HAT/HOT request packets, including radio button selection for ID and coordinate system.
 */
class CHATHOTRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the HAT/HOT request panel.
   * @param pParentWindow Parent window pointer.
   */
  CHATHOTRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the ID radio button event.
   * @param event The command event.
   */
  void OnIDRadio(wxCommandEvent& event);
  /**
   * @brief Handles the coordinate system radio button event.
   * @param event The command event.
   */
  void OnCoordinateSystemRadio(wxCommandEvent& event);

  /**
   * @brief Handles the send button event for sending HAT/HOT request packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
