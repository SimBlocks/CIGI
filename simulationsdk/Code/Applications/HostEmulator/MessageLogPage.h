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
 * Buffers message text until UpdateGUI() appends it to the control. Subscribes to HostCigiEvent when the global
 * dispatcher is available. Callbacks and GUI refresh access unsynchronized state and are intended for the GUI thread.
 */
class CMessageLogPage : public CNotebookPage, sbio::cigi::host::IHostCigiEventListener
{
public:
  /**
   * @brief Constructs the message log page.
   * @param pParent Parent window owning this page through the wxWidgets hierarchy.
   */
  CMessageLogPage(wxWindow* pParent);
  /**
   * @brief Unregisters this listener from the current global dispatcher when available; child controls follow wxWidgets ownership.
   */
  virtual ~CMessageLogPage();

  /**
   * @brief Appends generic message text to the pending buffer without refreshing the control.
   * @param args Borrowed event arguments; sMessage is copied into the buffer.
   */
  virtual void OnHostCigiMessageEvent(const sbio::cigi::host::HostCigiMessageEventArgs& args) override;
  /**
   * @brief Copies formatted packet text into the pending buffer without refreshing the control.
   * @param args Borrowed event arguments; sDataMessage.str() is appended with newline separators.
   */
  virtual void OnHostCigiDataMessageEvent(const sbio::cigi::host::HostCigiDataMessageEventArgs& args) override;
  /**
   * @brief Clears both displayed text and pending messages immediately.
   * @param args Unused clear-event arguments.
   */
  virtual void OnHostCigiClearMessageEvent(const sbio::cigi::host::HostCigiClearMessageEventArgs& args) override;

  /**
   * @brief Appends pending text with newline separators and empties the buffer; does nothing when it is empty.
   */
  void UpdateGUI();

  DECLARE_EVENT_TABLE()
  /**
   * @brief Clears displayed and pending text without restoring the initial heading.
   * @param event Unused command event.
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
