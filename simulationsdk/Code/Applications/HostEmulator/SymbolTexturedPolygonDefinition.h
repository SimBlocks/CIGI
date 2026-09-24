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
   * @brief Inserts one grid row at the tracked insertion index and increments that index.
   * @param event Unused command event.
   */
  void OnAddRow(wxCommandEvent& event);
  /**
   * @brief Clears all cell contents and resets the insertion index to zero; does not delete grid rows.
   * @param event Unused command event.
   */
  void OnRemoveRow(wxCommandEvent& event);
  /**
   * @brief Submits a textured polygon using rows with both position and texture-coordinate pairs populated.
   * @param event Unused command event; requires an active host session.
   * Skips incomplete rows. Converts cell values with ToFloat(); does not validate texture availability or confirm delivery.
   */
  void OnSend(wxCommandEvent& event);

private:
  int m_nNumRows = {10};///< Next insertion index, reset to zero by clearing cells without deleting rows.
  wxGrid* m_pGrid = nullptr;///< Grid control for symbol textured polygon data entry
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
