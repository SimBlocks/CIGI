//Copyright SimBlocks LLC 2016-2026
/**
 * @file ErrorLogPage.h
 * @brief Declares the CErrorLogPage class for error log display in the HostEmulator application.
 *
 * Provides the CErrorLogPage class for displaying and managing error logs in the HostEmulator GUI.
 * Integrates with the event system to receive and display error events, and provides a clear log action.
 * Inherits from CNotebookPage for notebook integration and IHostCigiEventListener for event handling.
 *
 * @see CErrorLogPage
 * @see CNotebookPage
 * @see sbio::cigi::host::IHostCigiEventListener
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_ERROR_LOG_PAGE_H
#define SIMBLOCKS_HOST_EMULATOR_ERROR_LOG_PAGE_H

#include "HostCigiLib/HostCigiEvent.h"
#include "wxWidgetsUtilities/NotebookPage.h"
#include <algorithm>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <wx/bookctrl.h>
#include <wx/wx.h>

/**
 * @brief Error log page for displaying error messages in the HostEmulator GUI.
 *
 * Subscribes to HostCigiEvent when a global dispatcher is available. Appends errors immediately and clears the
 * previous batch before displaying the thirty-first error. Callbacks manipulate controls and must run on the GUI thread.
 */
class CErrorLogPage : public CNotebookPage, sbio::cigi::host::IHostCigiEventListener
{
public:
  /**
   * @brief Constructs the error log page.
   * @param pParent Parent window owning this page through the wxWidgets hierarchy.
   */
  CErrorLogPage(wxWindow* pParent);
  /**
   * @brief Unregisters this listener from the current global dispatcher when available; child controls follow wxWidgets ownership.
   */
  virtual ~CErrorLogPage();

  /**
   * @brief Appends an error, clearing stored/displayed errors first when the count would exceed thirty.
   * @param args Borrowed event arguments; sError is copied into the log and appended to the control.
   */
  virtual void OnHostCigiErrorEvent(const sbio::cigi::host::HostCigiErrorEventArgs& args) override;

  DECLARE_EVENT_TABLE()
  /**
   * @brief Clears displayed and stored errors and resets the counter to zero.
   * @param event Unused command event.
   */
  void OnClearErrorLog(wxCommandEvent& event);

private:
  std::string m_sErrorLog;///< Error log string
  int counter = 0;///< Error count
  wxTextCtrl* m_pTextCtrl = nullptr;///< Text control for displaying errors
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
