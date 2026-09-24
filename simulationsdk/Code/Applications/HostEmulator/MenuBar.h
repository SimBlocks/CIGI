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
 * Builds File, Test, and Help menus and binds setup, exit, IG launch, and about actions. Ownership passes to the
 * frame when installed with SetMenuBar(); individual menu objects are owned by the menu bar.
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
   * @brief Displays the application's wxWidgets About dialog.
   * @param event Unused command event.
   */
  void OnAbout(wxCommandEvent& event);
  /**
   * @brief Delegates IG launch to the application, or does nothing when it is unavailable.
   * @param event Unused command event.
   */
  void OnLaunchIG(wxCommandEvent& event);
  /**
   * @brief Requests a forced close of the application's top window when both are available.
   * @param event Unused command event.
   */
  void OnQuit(wxCommandEvent& event);
  /**
   * @brief Constructs and displays a modal SetupDialog; configuration changes are handled by that dialog.
   * @param event Unused command event.
   */
  void OnSetup(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
