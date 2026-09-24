//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostEmulatorOptions.h
 * @brief Declares options and configuration structures for the HostEmulator application.
 *
 * Provides the CHostEmulatorOptions class for loading and managing host configuration options from file for the HostEmulator application.
 * Includes host setup options for CIGI protocol integration and simulation interoperability.
 *
 * @see CHostEmulatorOptions
 * @see sbio::cigi::host::SHostSetupOptions
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_OPTIONS_H
#define SIMBLOCKS_HOST_EMULATOR_OPTIONS_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "HostCigiLib/HostCigiLibTypes.h"
#include <string>

/**
 * @brief Manages loading and storage of HostEmulator configuration options.
 *
 * Each successfully opened configuration starts from SHostSetupOptions defaults, then applies file overrides and
 * the application's CIGI 4.0 fallback. Missing keys do not inherit values from previous loads or direct edits.
 * Loading options does not initialize a host.
 */
class CHostEmulatorOptions
{
public:
  /**
   * @brief Resets options to defaults after opening a POCO INI file, then applies its settings without transactional rollback.
   * @param filePath INI path opened as supplied; missing files and POCO read/conversion failures propagate as exceptions.
   *
   * Accepts hostToIGPort and igToHostPort only in [0, 65535]; invalid ranges retain the default ports. Address
   * strings are copied without validation. cigiVersion selects 3.3 only for "3.3" and otherwise defaults to 4.0,
   * including when absent. synchronizationMode is lowercased before matching "synchronous" or "asynchronous";
   * an absent or unrecognized value retains the default asynchronous mode. defaultDatabase is narrowed to uint8_t without range
   * validation. bigEndianByteOrder uses POCO's boolean conversion. PathToCigiSisoConversionsFile is joined to
   * g_globals.applicationsDataPath, not the INI directory. databaseControl recognizes exact "IG" and "Host" values.
   *
   * After top-level fields are read, clears sessions and reads Session0 through Session(sessionCount-1). Each entry
   * copies top-level endpoints/database before applying its own keys; sessionID defaults to UnknownSessionID.
   * Session port ranges are checked as above; invalid overrides retain the inherited top-level ports. Database and
   * session IDs are narrowed without validation. Missing or nonpositive sessionCount leaves the list empty.
   * Failure to construct the INI reader leaves existing options untouched.
   */
  void LoadOptions(const std::filesystem::path& filePath);

public:
  sbio::cigi::host::SHostSetupOptions hostSetupOptions;///< Host setup options
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
