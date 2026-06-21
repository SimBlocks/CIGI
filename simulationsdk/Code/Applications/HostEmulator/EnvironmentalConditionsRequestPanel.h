//Copyright SimBlocks LLC 2016-2026
/**
 * @file EnvironmentalConditionsRequestPanel.h
 * @brief Declares the CEnvironmentalConditionsRequestPanel class for environmental conditions request in the HostEmulator application.
 *
 * Provides the CEnvironmentalConditionsRequestPanel class for managing environmental conditions request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CEnvironmentalConditionsRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ENVIRONMENTAL_CONDITIONS_REQUEST_PANEL_H
#define SIMBLOCKS_ENVIRONMENTAL_CONDITIONS_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Environmental conditions request panel for managing request packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending environmental conditions request packets.
 */
class CEnvironmentalConditionsRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the environmental conditions request panel.
   * @param pParentWindow Parent window pointer.
   */
  CEnvironmentalConditionsRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending environmental conditions request packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
