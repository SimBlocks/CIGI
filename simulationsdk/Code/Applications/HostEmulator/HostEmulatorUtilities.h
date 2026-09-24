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
 * @brief Converts wxString text through ToStdString(), atof(), and a float cast.
 * @param s Numeric text to convert using the C library's current locale and prefix-parsing rules.
 * @return Parsed value narrowed to float, or zero if no conversion is possible. No validation status is returned.
 */
float ToFloat(wxString s);

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
