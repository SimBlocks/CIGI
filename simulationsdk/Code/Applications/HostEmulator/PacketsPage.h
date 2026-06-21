//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketsPage.h
 * @brief Declares the CPacketsPage class for packet management in the HostEmulator application.
 *
 * Provides the CPacketsPage class for managing and selecting protocol packets in the HostEmulator GUI.
 * Integrates with wxWidgets for GUI management and supports user interaction for selecting and displaying packet panels.
 * Inherits from CNotebookPage for notebook integration and provides event handlers for packet selection.
 *
 * @see CPacketsPage
 * @see CNotebookPage
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_PACKETS_PAGE_H
#define SIMBLOCKS_HOST_EMULATOR_PACKETS_PAGE_H

#include "wxWidgetsUtilities/NotebookPage.h"
#include <unordered_map>
#include <wx/wx.h>

/**
 * @brief Packets page for managing and selecting protocol packets in the HostEmulator GUI.
 *
 * Supports user interaction for selecting and displaying packet panels.
 */
class CPacketsPage : public CNotebookPage
{
public:
  /**
   * @brief Constructs the packets page.
   * @param pParent Parent window pointer.
   */
  CPacketsPage(wxWindow* pParent);

  /**
   * @brief Selects a packet by name and displays the corresponding panel.
   * @param sPacket The packet name to select.
   */
  void SelectPacket(const std::string& sPacket);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the packet list box event.
   * @param event The command event.
   */
  void OnListBox(wxCommandEvent& event);

private:
  std::unordered_map<std::string, wxPanel*> m_Panels;///< Map of packet names to panels
  std::string m_sCurrentPacketSelection;///< Currently selected packet name
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
