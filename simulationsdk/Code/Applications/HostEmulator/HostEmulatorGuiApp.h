//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostEmulatorGuiApp.h
 * @brief Declares the CHostEmulatorGuiApp class for the HostEmulator GUI application.
 *
 * Provides the CHostEmulatorGuiApp class for managing the main application lifecycle, event handling, and integration
 * with the host, view manager, and GUI frame. Supports initialization, event-driven updates, and launching the image generator.
 * Integrates with wxWidgets for GUI management and SimBlocks SDK for simulation interoperability.
 *
 * @see CHostEmulatorGuiApp
 * @see CHostEmulatorGuiFrame
 * @see sbio::cigi::host::CHost
 * @see sbio::cigi::host::CHostSession
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_GUI_APP_H
#define SIMBLOCKS_HOST_EMULATOR_GUI_APP_H

#include "HostCigiLib/HostCigiLibDeclarations.h"
#include "HostEmulatorGuiFrame.h"
#include "UtilitiesLib/StopWatch.h"
#include <memory>
#include <wx/wx.h>

#ifdef _WIN32
#include <windows.h>
#else
struct PROCESS_INFORMATION;
#endif

class CScriptRuntime;

/**
 * @brief Main application class for the HostEmulator GUI.
 *
 * Owns the CIGI host and drives it with a 30-millisecond wxWidgets timer; a separate 250-millisecond timer refreshes
 * GUI state. The frame and controls follow wxWidgets lifetime management. Initialization installs global services
 * used by the packet editors, so those editors require a successfully initialized application.
 */
class CHostEmulatorGuiApp : public wxApp
{
public:
  /**
   * @brief Publishes this instance through the application global and initializes process/update bookkeeping.
   */
  CHostEmulatorGuiApp();
  /**
   * @brief Destroys the application and owned host; does not call Uninitialize() to stop timers or the launched IG.
   */
  ~CHostEmulatorGuiApp();

  /**
   * @brief Gets the application-owned host.
   * @return Borrowed host pointer, or nullptr before host creation; valid until host replacement or application destruction.
   */
  sbio::cigi::host::CHost* GetHost() const;
  /**
   * @brief Gets the cached GUI session selection.
   * @return Identifier initialized from the host's active session during OnInit(); defaults to session zero.
   */
  sbio::SessionID GetSelectedSessionID() const;
  /**
   * @brief Attempts to launch the test image generator using the platform-specific launcher.
   *
   * On Windows, uses CreateProcess with the SDK vc143/x64 release directory as the working directory and retains
   * process handles for shutdown. Other platforms invoke the relative executable through system(). Launch failures
   * are not reported by this method.
   */
  void LaunchImageGenerator();

  /**
   * @brief Initializes libraries, loads view/host configuration, shows the frame, and starts both timers.
   * @return True when setup reaches timer startup; false after a caught exception is displayed in a message box.
   */
  virtual bool OnInit();
  /**
   * @brief Stops both timers and, on Windows, terminates the launched IG and closes its process/thread handles.
   *
   * Requires both timers to have been created. Does not destroy the host, delete timers, or clear closed handles;
   * this is not a general-purpose rollback for partially completed initialization or a repeatable teardown API.
   */
  void Uninitialize();

  /**
   * @brief Reads a SetupDialog, validates its address/database, and reinitializes the host using CIGI 4.0.
   * @param event Event whose source must be a button parented directly by the SetupDialog.
   *
   * Validation failures raise a host error event and show a message box. On success destroys the dialog. This
   * application-level callback differs from SetupDialog::OnOk(), which updates the selected session's options.
   */
  void OnOk(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Restarts the stopwatch and updates the host with the measured elapsed seconds.
   * @param timerEvent Unused timer notification; the initialized host must be available.
   */
  void OnHostUpdateTimer(wxTimerEvent& timerEvent);
  /**
   * @brief Refreshes status fields from the active host session, then updates the frame's log and script pages.
   * @param timerEvent Unused timer notification; the frame and GUI timer must be initialized.
   */
  void OnGuiUpdateTimer(wxTimerEvent& timerEvent);

private:
  PROCESS_INFORMATION m_IGProcessInformation;///< Process information for launched image generator
  wxTimer* m_pHostUpdateTimer = nullptr;///< Timer for host updates
  wxTimer* m_pGuiUpdateTimer = nullptr;///< Timer for GUI updates
  std::unique_ptr<sbio::cigi::host::CHost> m_pHost;///< Host instance
  CHostEmulatorGuiFrame* pFrame = nullptr;///< Main GUI frame
  int previousFrameNumber = 0;///< Previous frame number for update tracking
  sbio::SessionID m_SelectedSessionID = sbio::SessionID(0);///< Active session selected in the GUI
  sbio::utils::CStopWatch stopWatch;///< Stopwatch for timing
};

DECLARE_APP(CHostEmulatorGuiApp)

/**
 * @brief Gets the host's currently active session.
 * @return Borrowed session pointer, or nullptr if the application, host, or active session is unavailable.
 * Do not retain across host reinitialization or destruction.
 */
sbio::cigi::host::CHostSession* GetHostSession();
/**
 * @brief Looks up a host session by identifier.
 * @param sessionID Session to find.
 * @return Borrowed session pointer, or nullptr if the application, host, or requested session is unavailable.
 * Do not retain across host reinitialization or destruction.
 */
sbio::cigi::host::CHostSession* GetHostSession(sbio::SessionID sessionID);

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
