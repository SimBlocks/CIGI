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
   * @brief Replaces font-name choices with names registered for the selected category in the script runtime.
   * @param event Category-selection event supplying the category string. Missing host/runtime produces empty choices.
   */
  void OnFontCategory(wxCommandEvent& event);
  /**
   * @brief Resolves the selected runtime font and submits the text definition through the active session.
   * @param event Unused command event; no-op without a host/runtime, otherwise requires an active session.
   * Text of at least 235 bytes triggers an error notification and is truncated to 235 bytes before submission.
   * Font lookup results are not validated here. Removes the active wxLog target before submitting.
   */
  void OnSend(wxCommandEvent& event);

private:
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
