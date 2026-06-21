//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewControlPanel.h
 * @brief Declares the CViewControlPacketPanel class for view control in the HostEmulator application.
 *
 * Provides the CViewControlPacketPanel class for managing view control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CViewControlPacketPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_CONTROL_PACKET_PANEL_H
#define SIMBLOCKS_VIEW_CONTROL_PACKET_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief View control packet panel for managing view control packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending view control packets.
 */
class CViewControlPacketPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the view control packet panel.
   * @param pParentWindow Parent window pointer.
   */
  CViewControlPacketPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending view control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
