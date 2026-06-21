//Copyright SimBlocks LLC 2016-2026
/**
 * @file MenuBar.h
 * @brief Declares the CMenuBar class for the HostEmulator application's menu bar.
 *
 * Provides the CMenuBar class for managing the main menu bar, event handling, and user interface actions
 * in the HostEmulator application. Integrates with wxWidgets for GUI management and event-driven menu actions.
 *
 * @see CMenuBar
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_MENU_BAR_H
#define SIMBLOCKS_HOST_EMULATOR_MENU_BAR_H

#include <wx/wx.h>

/**
 * @brief Main menu bar for the HostEmulator application.
 *
 * Manages menu actions and event handling for the application.
 */
class CMenuBar : public wxMenuBar
{
public:
  /**
   * @brief Constructs a `CMenuBar` instance.
   */
  CMenuBar();

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the about event.
   * @param event Event object associated with the callback.
   */
  void OnAbout(wxCommandEvent& event);
  /**
   * @brief Handles the launch ig event.
   * @param event Event object associated with the callback.
   */
  void OnLaunchIG(wxCommandEvent& event);
  /**
   * @brief Handles the quit event.
   * @param event Event object associated with the callback.
   */
  void OnQuit(wxCommandEvent& event);
  /**
   * @brief Handles the setup event.
   * @param event Event object associated with the callback.
   */
  void OnSetup(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
