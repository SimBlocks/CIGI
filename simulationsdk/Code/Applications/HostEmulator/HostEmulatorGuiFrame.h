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

class CMessageLogPage;

/**
 * @brief Main GUI frame for the HostEmulator application.
 *
 * Manages the main window, event handling, and user interface components.
 */
class CHostEmulatorGuiFrame : public wxFrame
{
public:
  /**
   * @brief Constructs a `CHostEmulatorGuiFrame` instance.
   */
  CHostEmulatorGuiFrame();

  /**
   * @brief Handles the close event.
   * @param event Event object associated with the callback.
   */
  void OnClose(wxCloseEvent& event);
  /**
   * @brief Handles the ok event.
   * @param event Event object associated with the callback.
   */
  void OnOk(wxCommandEvent& event);
  /**
   * @brief Handles the page changed event.
   * @param event Event object associated with the callback.
   */
  void OnPageChanged(wxNotebookEvent& event);
  /**
   * @brief Updates gui.
   */
  void UpdateGUI();

  /**
   * @brief Creates button bar.
   * @param parent Parent value.
   * @return Created button bar.
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
