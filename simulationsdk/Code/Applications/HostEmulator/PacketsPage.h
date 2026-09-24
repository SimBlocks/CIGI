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
 * Constructs the packet editors as wxWidgets children, lists their names in sorted order, and initially shows
 * IG Control. The panel map holds non-owning pointers; the window hierarchy owns their lifetime.
 */
class CPacketsPage : public CNotebookPage
{
public:
  /**
   * @brief Constructs the packets page.
   * @param pParent Parent window owning this notebook page through the wxWidgets hierarchy.
   */
  CPacketsPage(wxWindow* pParent);

  /**
   * @brief Selects a packet by name and displays the corresponding panel.
   * @param sPacket Exact, case-sensitive editor name. Unknown names hide the old panel and create a null map entry;
   * no replacement panel is shown. This does not change the packet-name list box selection.
   */
  void SelectPacket(const std::string& sPacket);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the packet list box event.
   * @param event List-box event whose string supplies the packet editor name passed to SelectPacket().
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
