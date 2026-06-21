//Copyright SimBlocks LLC 2016-2026
/**
 * @file ConformalClampedEntityPositionPanel.h
 * @brief Declares the CConformalClampedEntityPositionPanel class for conformal clamped entity position control in the HostEmulator application.
 *
 * Provides the CConformalClampedEntityPositionPanel class for managing conformal clamped entity position packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CConformalClampedEntityPositionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_CONFORMAL_CLAMPED_POSITION_PANEL_H
#define SIMBLOCKS_CONFORMAL_CLAMPED_POSITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Conformal clamped entity position panel for managing position packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending conformal clamped entity position packets.
 */
class CConformalClampedEntityPositionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the conformal clamped entity position panel.
   * @param pParentWindow Parent window pointer.
   */
  CConformalClampedEntityPositionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending conformal clamped entity position packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
