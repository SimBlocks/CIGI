//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolPolygonDefinitionPanel.h
 * @brief Declares the CSymbolPolygonDefinitionPanel class for symbol polygon definition in the HostEmulator application.
 *
 * Provides the CSymbolPolygonDefinitionPanel class for managing symbol polygon definition packet input, row management, and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports adding and removing rows in a grid for symbol polygon data entry.
 *
 * @see CSymbolPolygonDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_POLYGON_DEFINITION_PANEL_H
#define SIMBLOCKS_SYMBOL_POLYGON_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"
#include "wx/grid.h"

/**
 * @brief Symbol polygon definition panel for managing symbol polygon packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol polygon definition packets, including grid row management.
 */
class CSymbolPolygonDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol polygon definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolPolygonDefinitionPanel(wxWindow* pParentWindow);

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
   * @brief Handles the send button event for sending symbol polygon definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
  int m_nNumRows = {10};///< Number of rows in the grid
  wxGrid* m_pGrid = nullptr;///< Grid control for symbol polygon data entry
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
