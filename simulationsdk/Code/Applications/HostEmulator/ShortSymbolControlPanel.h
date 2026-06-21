//Copyright SimBlocks LLC 2016-2026
/**
 * @file ShortSymbolControlPanel.h
 * @brief Declares the CShortSymbolControlPanel class for short symbol control in the HostEmulator application.
 *
 * Provides the CShortSymbolControlPanel class for managing short symbol control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CShortSymbolControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SHORT_SYMBOL_CONTROL_PANEL_H
#define SIMBLOCKS_SHORT_SYMBOL_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Short symbol control panel for managing short symbol packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending short symbol control packets.
 */
class CShortSymbolControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the short symbol control panel.
   * @param pParentWindow Parent window pointer.
   */
  CShortSymbolControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending short symbol control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
