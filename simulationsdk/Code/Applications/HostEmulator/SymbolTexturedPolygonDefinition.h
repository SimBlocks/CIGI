//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolTexturedPolygonDefinition.h
 * @brief Declares the CSymbolTexturedPolygonDefinitionPanel class for symbol textured polygon definition in the HostEmulator application.
 *
 * Provides the CSymbolTexturedPolygonDefinitionPanel class for managing symbol textured polygon definition packet input, row management, and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports adding and removing rows in a grid for symbol textured polygon data entry.
 *
 * @see CSymbolTexturedPolygonDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_TEXTURED_POLYGON_DEFINITION_PANEL_H
#define SIMBLOCKS_SYMBOL_TEXTURED_POLYGON_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"
#include "wx/grid.h"

/**
 * @brief Symbol textured polygon definition panel for managing symbol textured polygon packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol textured polygon definition packets, including grid row management.
 */
class CSymbolTexturedPolygonDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol textured polygon definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolTexturedPolygonDefinitionPanel(wxWindow* pParentWindow);

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
   * @brief Handles the send button event for sending symbol textured polygon definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
  int m_nNumRows = {10};///< Number of rows in the grid
  wxGrid* m_pGrid = nullptr;///< Grid control for symbol textured polygon data entry
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
