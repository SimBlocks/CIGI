//Copyright SimBlocks LLC 2016-2026
/**
 * @file SymbolTextDefinitionPanel.h
 * @brief Declares the CSymbolTextDefinitionPanel class for symbol text definition in the HostEmulator application.
 *
 * Provides the CSymbolTextDefinitionPanel class for managing symbol text definition packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports font category selection and dynamic UI updates for symbol text packets.
 *
 * @see CSymbolTextDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_SYMBOL_TEXT_DEFINITION_H
#define SIMBLOCKS_SYMBOL_TEXT_DEFINITION_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Symbol text definition panel for managing symbol text packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending symbol text definition packets, including font category selection.
 */
class CSymbolTextDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the symbol text definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CSymbolTextDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the font category selection event for updating the UI.
   * @param event The command event.
   */
  void OnFontCategory(wxCommandEvent& event);
  /**
   * @brief Handles the send button event for sending symbol text definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
