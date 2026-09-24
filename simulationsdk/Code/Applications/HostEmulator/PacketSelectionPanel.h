//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketSelectionPanel.h
 * @brief Declares the PacketSelectionPanel class for packet selection in the HostEmulator application.
 *
 * Provides the PacketSelectionPanel class for displaying and managing packet selection controls in the HostEmulator GUI.
 * Integrates with wxWidgets for GUI management and supports user interaction for selecting CIGI or protocol packets.
 *
 * @see PacketSelectionPanel
 */
#pragma once

#include <wx/wx.h>

/**
 * @brief Placeholder panel containing explanatory text and an unbound button.
 *
 * Does not populate a packet list or implement packet selection; CPacketsPage provides the active editor selector.
 */
class PacketSelectionPanel : public wxPanel
{
public:
  /**
   * @brief Constructs the packet selection panel.
   * @param title wxWidgets window name, not a visible panel caption.
   * @param parent Parent panel owning this window through the wxWidgets hierarchy.
   */
  PacketSelectionPanel(const wxString& title, wxPanel* parent);
};

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
