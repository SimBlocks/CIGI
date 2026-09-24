//Copyright SimBlocks LLC 2016-2026
/**
 * @file CIGIHostOptions.h
 * @brief Declares options and configuration structures for the CIGIHost application.
 *
 * Provides the SCigiHostSetupOptions structure for extended host setup options, including script category and script name.
 * Declares the CCIGIHostOptions class for loading and managing host configuration options from file for the CIGIHost application.
 *
 * @see SCigiHostSetupOptions
 * @see CCIGIHostOptions
 * @see sbio::cigi::host::SHostSetupOptions
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HOST_OPTIONS_H
#define SIMBLOCKS_CIGI_HOST_OPTIONS_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "HostCigiLib/HostCigiLibTypes.h"
#include <string>

/**
 * @brief Host-library setup options plus the script selection used by the CIGIHost console application.
 *
 * Inherited fields use the defaults from sbio::cigi::host::SHostSetupOptions; both script strings initially are empty.
 * The application passes the inherited configuration to CHost::Initialize() and the script selection to
 * CScriptRuntime::Execute() when the user requests script execution. This structure stores values only; it does not
 * open a connection, validate a script, or execute one.
 */
struct SCigiHostSetupOptions : sbio::cigi::host::SHostSetupOptions
{
  std::string sScriptCategory;///< Category/subdirectory passed to the script runtime to locate the selected script.
  std::string sScript;///< Script filename passed to the runtime relative to the selected category.
};

/**
 * @brief Stores application options and overlays recognized values from a POCO INI configuration file.
 *
 * Each load modifies the current hostSetupOptions rather than replacing it with fresh defaults. Fields not present
 * in the file, including inherited settings that this loader does not expose, retain their existing values.
 * Options may also be edited directly before or after loading. Loading does not reconfigure a running host.
 */
class CCIGIHostOptions
{
public:
  /**
   * @brief Applies recognized INI keys to the currently stored options.
   * @param filePath Configuration path inspected and opened as supplied; no application-data prefix is added.
   *
   * A nonexistent path leaves all options unchanged. For an existing file, only keys reported present by POCO
   * are read. The supported keys and their effects are:
   * - `hostToIGPort`, `igToHostPort`: read as integers and converted to uint16_t without port-range validation.
   * - `hostIPAddress`, `igIPAddress`: stored as strings without address validation.
   * - `cigiVersion`: "3.3" selects VERSION_3_3, "4.0" selects VERSION_4_0, and any other value selects UNKNOWN_VERSION.
   * - `synchronizationMode`: lowercased before matching "synchronous" or "asynchronous"; other values leave the
   *   current synchronization mode unchanged.
   * - `defaultDatabase`: read as an integer, converted to uint8_t, and wrapped in DatabaseID without range validation.
   * - `bigEndianByteOrder`: read using POCO's boolean conversion.
   * - `PathToCigiSisoConversionsFile`: joined to the process-global g_globals.applicationsDataPath using filesystem
   *   path concatenation, not resolved relative to filePath. The referenced conversion file is not checked here.
   * - `scriptCategory`, `script`: copied into sScriptCategory and sScript without checking script availability.
   *
   * Filesystem and POCO file-reading, parsing, and conversion exceptions propagate to the caller. Updates are not
   * transactional: fields applied before an exception remain changed. Unrecognized keys are not applied.
   */
  void LoadOptions(const std::filesystem::path& filePath);

public:
  SCigiHostSetupOptions hostSetupOptions;///< Owned, mutable options initialized from type defaults and overlaid by LoadOptions().
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
