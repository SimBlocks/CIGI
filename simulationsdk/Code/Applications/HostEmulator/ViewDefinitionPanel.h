//Copyright SimBlocks LLC 2016-2026
/**
 * @file ViewDefinitionPanel.h
 * @brief Declares the CViewDefinitionPanel class for view definition in the HostEmulator application.
 *
 * Provides the CViewDefinitionPanel class for managing view definition packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CViewDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_DEFINITION_PANEL_H
#define SIMBLOCKS_VIEW_DEFINITION_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief View definition panel for managing view definition packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending view definition packets.
 */
class CViewDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the view definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CViewDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending view definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
