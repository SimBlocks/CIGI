//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightSegmentRequestPanel.h
 * @brief Declares the CLineOfSightSegmentRequestPanel class for line of sight segment request control in the HostEmulator application.
 *
 * Provides the CLineOfSightSegmentRequestPanel class for managing line of sight segment request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Supports radio button events for source and destination point coordinate system selection.
 *
 * @see CLineOfSightSegmentRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_LINE_OF_SIGHT_SEGMENT_REQUEST_PANEL_H
#define SIMBLOCKS_LINE_OF_SIGHT_SEGMENT_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Line of sight segment request panel for managing segment request packet input in the HostEmulator GUI.
 *
 * Provides user interface for entering and sending line of sight segment request packets, including radio button selection for source and destination point coordinate systems.
 */
class CLineOfSightSegmentRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the line of sight segment request panel.
   * @param pParentWindow Parent window pointer.
   */
  CLineOfSightSegmentRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Relabels destination fields for the current geodetic/entity coordinate selection without converting values.
   * @param event Unused command event; reads the radio control's current selection.
   */
  void OnDestinationPointCoordinateSystemRadio(wxCommandEvent& event);
  /**
   * @brief Submits the basic segment request matching the selected source/destination coordinate systems.
   * @param event Unused command event; requires an active session.
   *
   * Unrecognized coordinate combinations send nothing. The entity-to-geodetic branch currently leaves destination
   * coordinates at their request defaults rather than reading the destination fields. Submission does not confirm delivery.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Relabels source fields for the current geodetic/entity coordinate selection without converting values.
   * @param event Unused command event; reads the radio control's current selection.
   */
  void OnSourcePointCoordinateSystemRadio(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
