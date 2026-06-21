//Copyright SimBlocks LLC 2016-2026
/**
 * @file MessageLogPage.h
 * @brief Declares the CMessageLogPage class for message log display in the HostEmulator application.
 *
 * Provides the CMessageLogPage class for displaying and managing message logs in the HostEmulator GUI.
 * Integrates with the event system to receive and display various message events, including data, weather, and sensor messages.
 * Inherits from CNotebookPage for notebook integration and IHostCigiEventListener for event handling.
 *
 * @see CMessageLogPage
 * @see CNotebookPage
 * @see sbio::cigi::host::IHostCigiEventListener
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_MESSAGE_LOG_PAGE_H
#define SIMBLOCKS_HOST_EMULATOR_MESSAGE_LOG_PAGE_H

#include "wxWidgetsUtilities/NotebookPage.h"
#include "HostCigiLib/HostCigiEvent.h"
#include <unordered_map>
#include <wx/bookctrl.h>
#include <wx/wx.h>

/**
 * @brief Message log page for displaying messages in the HostEmulator GUI.
 *
 * Receives message events and displays them in a text control. Allows clearing the message log and updating the GUI.
 */
class CMessageLogPage : public CNotebookPage, sbio::cigi::host::IHostCigiEventListener
{
public:
  /**
   * @brief Constructs the message log page.
   * @param pParent Parent window pointer.
   */
  CMessageLogPage(wxWindow* pParent);
  /**
   * @brief Destroys CMessageLogPage instances.
   */
  virtual ~CMessageLogPage();

  /**
   * @brief Handles message events and updates the message log display.
   * @param args Message event arguments.
   */
  virtual void OnHostCigiMessageEvent(const sbio::cigi::host::HostCigiMessageEventArgs& args) override;
  /**
   * @brief Handles the host cigi data message event event.
   * @param args Event data supplied with the dispatch.
   */
  virtual void OnHostCigiDataMessageEvent(const sbio::cigi::host::HostCigiDataMessageEventArgs& args) override;
  /**
   * @brief Handles the host cigi clear message event event.
   * @param args Event data supplied with the dispatch.
   */
  virtual void OnHostCigiClearMessageEvent(const sbio::cigi::host::HostCigiClearMessageEventArgs& args) override;

  /**
   * @brief Updates the GUI with the latest message log content.
   */
  void UpdateGUI();

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the clear message log button event.
   * @param event The command event.
   */
  void OnClearMessageLog(wxCommandEvent& event);

private:
  wxTextCtrl* m_pTextCtrl = nullptr;///< Text control for displaying messages
  int counter = 0;///< Message count
  std::string m_sTextToAppend;///< Text to append to the log
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
