//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolClonePanel.h
 * @brief Declares the CSymbolClonePanel class for symbol clone control in the HostEmulator application.
 *
 * Provides the CSymbolClonePanel class for managing symbol clone packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CSymbolClonePanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_CLONE_PANEL_H
#define SIMBLOCKS_SYMBOL_CLONE_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Symbol clone panel for managing symbol clone packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol clone packets.
 */
class CSymbolClonePanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol clone panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolClonePanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending symbol clone packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
