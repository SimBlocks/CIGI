//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolSurfaceDefinitionPanel.h
 * @brief Declares the CSymbolSurfaceDefinitionPanel class for symbol surface definition in the HostEmulator application.
 *
 * Provides the CSymbolSurfaceDefinitionPanel class for managing symbol surface definition packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports entity view ID radio button events for dynamic UI updates.
 *
 * @see CSymbolSurfaceDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_SURFACE_DEFINITION_PANEL_H
#define SIMBLOCKS_SYMBOL_SURFACE_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Symbol surface definition panel for managing symbol surface packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol surface definition packets, including entity view ID selection.
 */
class CSymbolSurfaceDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol surface definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolSurfaceDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Submits an entity, entity-billboard, or view surface definition based on attachment and billboard selections.
   * @param event Unused command event; requires an active session for supported selections.
   * Relabeled controls retain their internal keys; view bounds are read from the shared offset and yaw controls.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Relabels shared ID/position fields for an entity attachment or view viewport bounds, without converting values.
   * @param event Radio event; "Entity" selects entity labels, other strings select view labels.
   */
  void OnEntityViewIDRadio(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
