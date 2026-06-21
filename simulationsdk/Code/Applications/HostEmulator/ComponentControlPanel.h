//Copyright SimBlocks LLC 2016-2026
/**
 * @file ComponentControlPanel.h
 * @brief Declares the CComponentControlPanel class for component control in the HostEmulator application.
 *
 * Provides the CComponentControlPanel class for managing component control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CComponentControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_COMPONENT_CONTROL_PANEL_H
#define SIMBLOCKS_COMPONENT_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Component control panel for managing component packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending component control packets.
 */
class CComponentControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the component control panel.
   * @param pParentWindow Parent window pointer.
   */
  CComponentControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending component control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
