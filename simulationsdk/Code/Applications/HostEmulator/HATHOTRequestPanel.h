//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTRequestPanel.h
 * @brief Declares the CHATHOTRequestPanel class for HAT/HOT request control in the HostEmulator application.
 *
 * Provides the CHATHOTRequestPanel class for managing Height Above Terrain/Height Of Terrain (HAT/HOT) request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports radio button events for ID and coordinate system selection.
 *
 * @see CHATHOTRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_HAT_HOT_REQUEST_PANEL_H
#define SIMBLOCKS_HAT_HOT_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief HAT/HOT request panel for managing HAT/HOT packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending HAT/HOT request packets, including radio button selection for ID and coordinate system.
 */
class CHATHOTRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the HAT/HOT request panel.
   * @param pParentWindow Parent window pointer.
   */
  CHATHOTRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Relabels the request ID for HAT, HOT, or Extended without changing its stored value.
   * @param event Radio event supplying the request-type string; other strings leave the label unchanged.
   */
  void OnIDRadio(wxCommandEvent& event);
  /**
   * @brief Relabels the position fields as geodetic coordinates or entity offsets, without converting their values.
   * @param event Radio event; "Geodetic" selects latitude/longitude/altitude labels, all other strings select offsets.
   */
  void OnCoordinateSystemRadio(wxCommandEvent& event);

  /**
   * @brief Builds a global or entity-relative HAT/HOT request from the controls and submits it to the active session.
   * @param event Unused command event. Does nothing when no session exists; submission does not confirm a response.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
