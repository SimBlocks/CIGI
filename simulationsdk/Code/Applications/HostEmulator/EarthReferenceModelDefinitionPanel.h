//Copyright SimBlocks LLC 2016-2026
/**
 * @file EarthReferenceModelDefinitionPanel.h
 * @brief Declares the CEarthReferenceModelDefinitionPanel class for earth reference model control in the HostEmulator application.
 *
 * Provides the CEarthReferenceModelDefinitionPanel class for managing earth reference model packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CEarthReferenceModelDefinitionPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_EARTH_REFERENCE_PACKET_PANEL_H
#define SIMBLOCKS_EARTH_REFERENCE_PACKET_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Earth reference model definition panel for managing earth reference packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending earth reference model definition packets.
 */
class CEarthReferenceModelDefinitionPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the earth reference model definition panel.
   * @param pParentWindow Parent window pointer.
   */
  CEarthReferenceModelDefinitionPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending earth reference model definition packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
