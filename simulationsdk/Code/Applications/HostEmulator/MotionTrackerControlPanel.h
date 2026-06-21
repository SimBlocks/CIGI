//Copyright SimBlocks LLC 2016-2026
/**
 * @file MotionTrackerControlPanel.h
 * @brief Declares the CMotionTrackerControlPanel class for motion tracker control in the HostEmulator application.
 *
 * Provides the CMotionTrackerControlPanel class for managing motion tracker control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports radio button events for motion tracker options and dynamic UI updates.
 *
 * @see CMotionTrackerControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_MOTION_TRACKER_CONTROL_PANEL_H
#define SIMBLOCKS_MOTION_TRACKER_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Motion tracker control panel for managing motion tracker packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending motion tracker control packets, including radio button selection for options.
 */
class CMotionTrackerControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the motion tracker control panel.
   * @param pParentWindow Parent window pointer.
   */
  CMotionTrackerControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the radio button event for motion tracker options.
   * @param event The command event.
   */
  void OnRadio(wxCommandEvent& event);
  /**
   * @brief Handles the send button event for sending motion tracker control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
  wxStaticText* m_pIDText = nullptr;///< Static text control for displaying ID
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
