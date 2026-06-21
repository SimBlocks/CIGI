//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityControlPanel.h
 * @brief Declares the CEntityControlPanel class for entity control in the HostEmulator application.
 *
 * Provides the CEntityControlPanel class for managing entity control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports entity type selection and dynamic UI updates for short and extended entity types.
 *
 * @see CEntityControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_CONTROL_PANEL_H
#define SIMBLOCKS_ENTITY_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Entity control panel for managing entity packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending entity control packets, including entity type selection.
 */
class CEntityControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the entity control panel.
   * @param pParentWindow Parent window pointer.
   */
  CEntityControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending entity control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Handles the entity type selection event for updating the UI.
   * @param event The command event.
   */
  void OnEntityTypeSelection(wxCommandEvent& event);

private:
  wxBoxSizer* m_ShortEntityTypeSizer;///< Sizer for short entity type controls
  wxBoxSizer* m_ExtendedEntityTypeSizer;///< Sizer for extended entity type controls
};
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
