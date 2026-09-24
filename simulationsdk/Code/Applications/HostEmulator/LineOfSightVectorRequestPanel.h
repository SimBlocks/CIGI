//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightVectorRequestPanel.h
 * @brief Declares the CLineOfSightVectorRequestPanel class for line of sight vector request control in the HostEmulator application.
 *
 * Provides the CLineOfSightVectorRequestPanel class for managing line of sight vector request packet input and sending in the HostEmulator GUI.
 * Inherits from CBasePacketPanel for base packet panel functionality and integrates with wxWidgets for GUI management.
 * Provides source and response coordinate controls; current sending code builds basic requests only.
 *
 * @see CLineOfSightVectorRequestPanel
 * @see CBasePacketPanel
 */
#pragma once
#ifndef SIMBLOCKS_LINE_OF_SIGHT_VECTOR_REQUEST_PANEL_H
#define SIMBLOCKS_LINE_OF_SIGHT_VECTOR_REQUEST_PANEL_H

#include "wxWidgetsUtilities/BasePacketPanel.h"

/**
 * @brief Line of sight vector request panel for managing vector request packet input in the HostEmulator GUI.
 *
 * Edits geodetic or entity-relative basic vector requests. The Extended Response and Response Coordinate System
 * controls are not consumed by OnSend(); they do not select an extended request in this implementation.
 */
class CLineOfSightVectorRequestPanel : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs the line of sight vector request panel.
   * @param pParentWindow Parent window pointer.
   */
  CLineOfSightVectorRequestPanel(wxWindow* pParentWindow);

  DECLARE_EVENT_TABLE()
  /**
   * @brief Attempts to relabel destination fields using a destination coordinate radio selection.
   * @param event Unused command event.
   * This legacy helper is not bound by the panel, whose constructor creates no destination controls.
   */
  void OnDestinationPointCoordinateSystemRadio(wxCommandEvent& event);
  /**
   * @brief Submits a basic geodetic or entity-relative vector request from the current controls.
   * @param event Unused command event; requires an active session for recognized source coordinates.
   * Ignores extended-response and response-coordinate controls; unrecognized source coordinates send nothing.
   */
  void OnSend(wxCommandEvent& event);
  /**
   * @brief Relabels source fields as geodetic coordinates or entity offsets without converting values.
   * @param event Unused command event; the current source radio selection controls the labels.
   */
  void OnSourcePointCoordinateSystemRadio(wxCommandEvent& event);
  /**
   * @brief No-op response-coordinate callback; the panel's event table currently routes that event to the source callback.
   * @param event Unused command event.
   */
  void OnResponseCoordinateSystemRadio(wxCommandEvent& event);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
