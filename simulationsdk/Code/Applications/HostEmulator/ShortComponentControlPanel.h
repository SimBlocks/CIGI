//Copyright SimBlocks LLC 2016-2026
/**
 * @file ShortComponentControlPanel.h
 * @brief Declares the CShortComponentControlPanel class for short component control in the HostEmulator application.
 *
 * Provides the CShortComponentControlPanel class for managing short component control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CShortComponentControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SHORT_COMPONENT_CONTROL_PANEL_H
#define SIMBLOCKS_SHORT_COMPONENT_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Short component control panel for managing short component packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending short component control packets.
 */
class CShortComponentControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the short component control panel.
   * @param pParentWindow Parent window pointer.
   */
  CShortComponentControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending short component control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
