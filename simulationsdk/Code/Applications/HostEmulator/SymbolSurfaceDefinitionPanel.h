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
   * @brief Handles the send button event for sending symbol surface definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Handles the entity view ID radio button event for updating the UI.
   * @param event The command event.
   */
  void OnEntityViewIDRadio(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
