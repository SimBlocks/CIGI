//Copyright SimBlocks LLC 2016-2026
/**
 * @file AnimationControlPanel.h
 * @brief Declares the CAnimationControlPanel class for animation control in the HostEmulator application.
 *
 * Provides the CAnimationControlPanel class for managing animation control packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 *
 * @see CAnimationControlPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_ANIMATION_CONTROL_PANEL_H
#define SIMBLOCKS_ANIMATION_CONTROL_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Animation control panel for managing animation packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending animation control packets.
 */
class CAnimationControlPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the animation control panel.
   * @param pParentWindow Parent window pointer.
   */
  CAnimationControlPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the send button event for sending animation control packets.
   * @param event The command event.
   */
  void OnSend(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
