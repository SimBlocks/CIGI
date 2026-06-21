//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolControlPanel.h
 * @brief Declares the CSymbolControlPanel class for symbol control in the HostEmulator application.
 *
 * Provides the CSymbolControlPanel class for managing symbol control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CSymbolControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_CONTROL_PANEL_H
#define SIMBLOCKS_SYMBOL_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Symbol control panel for managing symbol packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol control packets.
 */
class CSymbolControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol control panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending symbol control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
