//Copyright SimBlocks LLC 2016-2026
/**
 * @file IGControlPacketPanel.h
 * @brief Declares the CIGControlPacketPanel class for IG control packet management in the HostEmulator application.
 *
 * Provides the CIGControlPacketPanel class for managing image generator (IG) control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CIGControlPacketPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_IG_CONTROL_PACKET_PANEL_H
#define SIMBLOCKS_IG_CONTROL_PACKET_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief IG control packet panel for managing IG control packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending IG control packets.
 */
class CIGControlPacketPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the IG control packet panel.
   * @param pParentWindow Parent window pointer.
   */
  CIGControlPacketPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending IG control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
