//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostEmulatorUtilities.h
 * @brief Utility functions for the HostEmulator application.
 *
 * Provides utility functions for type conversion and other helper operations used in the HostEmulator application.
 * Integrates with wxWidgets for GUI and string handling.
 *
 * @see ToFloat
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_UTILITIES_H
#define SIMBLOCKS_HOST_EMULATOR_UTILITIES_H

#include <wx/wx.h>

/**
 * @brief Converts a wxString to a float value.
 * @param s The wxString to convert.
 * @return The converted float value.
 */
float ToFloat(wxString s);

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
