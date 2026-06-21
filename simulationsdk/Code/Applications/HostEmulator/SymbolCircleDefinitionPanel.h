//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolCircleDefinitionPanel.h
 * @brief Declares the CSymbolCircleDefinitionPanel class for symbol circle definition in the HostEmulator application.
 *
 * Provides the CSymbolCircleDefinitionPanel class for managing symbol circle definition packet input, row management, and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports adding and removing rows in a grid for symbol circle data entry.
 *
 * @see CSymbolCircleDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_DEFINITION_PANEL_H
#define SIMBLOCKS_SYMBOL_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"
#include "wx/grid.h"

/**
 * @brief Symbol circle definition panel for managing symbol circle packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol circle definition packets, including grid row management.
 */
class CSymbolCircleDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol circle definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolCircleDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the add row button event for adding rows to the grid.
   * @param event The command event.
   */
  void OnAddRow(wxCommandEvent& event);
  /**
   * @brief Handles the remove row button event for removing rows from the grid.
   * @param event The command event.
   */
  void OnRemoveRow(wxCommandEvent& event);
  /**
   * @brief Handles the send button event for sending symbol circle definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
  int m_nNumRows = {10};///< Number of rows in the grid
  wxGrid* m_pGrid = nullptr;///< Grid control for symbol circle data entry
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
