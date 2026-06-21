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
 * Manages application lifecycle, event handling, and integration with host and GUI frame.
 */
class CHostEmulatorGuiApp : public wxApp
{
public:
  /**
   * @brief Constructs a `CHostEmulatorGuiApp` instance.
   */
  CHostEmulatorGuiApp();
  /**
   * @brief Destroys CHostEmulatorGuiApp instances.
   */
  ~CHostEmulatorGuiApp();

  /**
   * @brief Gets host.
   * @return Pointer to host instance.
   */
  sbio::cigi::host::CHost* GetHost() const;
  /**
   * @brief Gets selected session id.
   * @return Selected session id value.
   */
  sbio::SessionID GetSelectedSessionID() const;
  /**
   * @brief Launches image generator.
   */
  void LaunchImageGenerator();

  /**
   * @brief Handles the init event.
   * @return `true` when application initialization succeeds; otherwise `false`.
   */
  virtual bool OnInit();
  /**
   * @brief Releases HostEmulator application resources before shutdown.
   */
  void Uninitialize();

  /**
   * @brief Handles the ok event.
   * @param event Event object.
   */
  void OnOk(wxCommandEvent& event);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the host update timer event.
   * @param timerEvent Timer event.
   */
  void OnHostUpdateTimer(wxTimerEvent& timerEvent);
  /**
   * @brief Handles the gui update timer event.
   * @param timerEvent Timer event associated with the callback.
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
 * @brief Gets the current host session instance.
 * @return Pointer to the current host session.
 */
sbio::cigi::host::CHostSession* GetHostSession();
/**
 * @brief Gets host session.
 * @param sessionID Session identifier.
 * @return Host session value.
 */
sbio::cigi::host::CHostSession* GetHostSession(sbio::SessionID sessionID);

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
