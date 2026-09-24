//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostEmulatorGuiFrame.h
 * @brief Declares the CHostEmulatorGuiFrame class for the HostEmulator GUI main window.
 *
 * Provides the CHostEmulatorGuiFrame class for managing the main GUI window, event handling, and user interface
 * components for the HostEmulator application. Supports page navigation, GUI updates, and integration with the
 * message log and configuration options. Integrates with wxWidgets for GUI management and SimBlocks SDK for
 * simulation interoperability.
 *
 * @see CHostEmulatorGuiFrame
 * @see CMessageLogPage
 * @see CNotebookPage
 */
#pragma once
#ifndef SIMBLOCKS_EMULATOR_GUI_FRAME_H
#define SIMBLOCKS_EMULATOR_GUI_FRAME_H

#include "CigiLib/CigiTypes.h"
#include <vector>
#include <wx/wx.h>
#include <wx/notebook.h>

//_T macro from wx.h conflicts with _T macro from CigiLib/CigiConversions.h, so undefine it here to avoid conflicts.
#ifdef _T
#undef _T
#endif

class CMessageLogPage;

/**
 * @brief Main GUI frame for the HostEmulator application.
 *
 * Creates the packet, script, message-log, and error-log pages, menu bar, and four-field status bar. Child controls
 * follow wxWidgets ownership. Closing requests application shutdown before scheduling frame destruction.
 */
class CHostEmulatorGuiFrame : public wxFrame
{
public:
  /**
   * @brief Constructs a `CHostEmulatorGuiFrame` instance.
   */
  CHostEmulatorGuiFrame();

  /**
   * @brief Calls application Uninitialize() when available, then schedules frame destruction.
   * @param event Unused close notification; this callback does not veto closure.
   */
  void OnClose(wxCloseEvent& event);
  /**
   * @brief Copies dialog port, recognized version, and byte-order values into frame fields, then destroys the dialog.
   * @param event Event whose source must be a button parented by SetupDialog.
   * This callback displays the receive port but does not reinitialize the host or store the address/database fields.
   */
  void OnOk(wxCommandEvent& event);
  /**
   * @brief Calls OnSelected() on the newly selected notebook page.
   * @param event Event supplying a valid page index; the page must derive from CNotebookPage.
   */
  void OnPageChanged(wxNotebookEvent& event);
  /**
   * @brief Flushes the message-log buffer and refreshes the script-status label when their pages are available.
   */
  void UpdateGUI();

  /**
   * @brief Allocates a button-bar sizer and fits the supplied frame; no button actions are bound here.
   * @param parent Non-null frame used to parent the Play/Pause button and receive sizing hints.
   * @return Newly allocated sizer for the caller to install or release. Contains Play/Pause and Stop controls;
   * the current implementation default-constructs the Stop control without creating its native window.
   */
  wxFlexGridSizer* MakeButtonBar(wxFrame* parent);

public:
  bool m_bFrameActive = false;///< Indicates if the frame is active

  long m_hostToIGPort = 5001;///< Host-to-IG port
  long m_igToHostPort = 5000;///< IG-to-host port
  std::string m_ipAddress = "192.168.1.1";///< IP address
  sbio::cigi::ECigiVersion m_versionID = sbio::cigi::ECigiVersion::VERSION_4_0;///< CIGI protocol version
  long m_defaultDatabase = 1;///< Default database ID
  bool m_bigEndianByteOrder = false;///< Use big endian byte order

private:
  wxFrame* m_pParent = nullptr;///< Parent frame pointer
  wxMenuBar* m_pMenuBar = nullptr;///< Menu bar pointer
  wxMenu* m_pFileMenu = nullptr;///< File menu pointer
  wxSizer* m_pSizer = nullptr;///< Main sizer pointer
  wxNotebook* m_pNotebook = nullptr;///< Notebook (tab control) pointer
  CMessageLogPage* m_pMessageLogPage = nullptr;///< Message log page pointer

  DECLARE_EVENT_TABLE()
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
