//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightVectorRequestPanel.h
 * @brief Declares the CLineOfSightVectorRequestPanel class for line of sight vector request control in the HostEmulator application.
 *
 * Provides the CLineOfSightVectorRequestPanel class for managing line of sight vector request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports radio button events for source, destination, and response coordinate system selection.
 *
 * @see CLineOfSightVectorRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_LINE_OF_SIGHT_VECTOR_REQUEST_PANEL_H
#define SIMBLOCKS_LINE_OF_SIGHT_VECTOR_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Line of sight vector request panel for managing vector request packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending line of sight vector request packets, including radio button selection for source, destination, and response coordinate systems.
 */
class CLineOfSightVectorRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the line of sight vector request panel.
   * @param pParentWindow Parent window pointer.
   */
  CLineOfSightVectorRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the destination point coordinate system radio button event.
   * @param event The command event.
   */
  void OnDestinationPointCoordinateSystemRadio(wxCommandEvent& event);
  /**
   * @brief Handles the send button event for sending line of sight vector request packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Handles the source point coordinate system radio button event.
   * @param event The command event.
   */
  void OnSourcePointCoordinateSystemRadio(wxCommandEvent& event);
  /**
   * @brief Handles the response coordinate system radio button event.
   * @param event The command event.
   */
  void OnResponseCoordinateSystemRadio(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
