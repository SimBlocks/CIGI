//Copyright SimBlocks LLC 2016-2026
/**
 * @file CelestialSphereControlPanel.h
 * @brief Declares the CCelestialSphereControlPanel class for celestial sphere control in the HostEmulator application.
 *
 * Provides the CCelestialSphereControlPanel class for managing celestial sphere control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CCelestialSphereControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_CELESTIAL_SPHERE_CONTROL_PANEL_H
#define SIMBLOCKS_CELESTIAL_SPHERE_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Celestial sphere control panel for managing celestial sphere packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending celestial sphere control packets.
 */
class CCelestialSphereControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the celestial sphere control panel.
   * @param pParentWindow Parent window pointer.
   */
  CCelestialSphereControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending celestial sphere control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
