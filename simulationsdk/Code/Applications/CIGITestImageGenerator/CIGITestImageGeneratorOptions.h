//Copyright SimBlocks LLC 2016-2026
/**
 * @file CIGITestImageGeneratorOptions.h
 * @brief Declares the CCIGITestImageGeneratorOptions class for configuring CIGI test image generator options.
 *
 * Provides the CCIGITestImageGeneratorOptions class for loading and accessing image generator setup options for CIGI test applications.
 * Integrates with SimBlocks CIGI and IG libraries for simulation configuration.
 *
 * @see CCIGITestImageGeneratorOptions
 * @see sbio::cigi::ig::SIGSetupOptions
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_TEST_IMAGEGENERATOR_OPTIONS_H
#define SIMBLOCKS_CIGI_TEST_IMAGEGENERATOR_OPTIONS_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiLib/IGCIGILibTypes.h"
#include <string>

/**
 * @brief Stores image-generator setup options and applies configuration from a POCO INI file.
 *
 * Each load starts from SIGSetupOptions defaults and replaces the current options only on success.
 * Loading does not initialize or reconfigure an image generator. Callers may also edit m_SetupOptions.
 */
class CCIGITestImageGeneratorOptions
{
public:
  /**
   * @brief Gets the current image generator setup options.
   * @return Borrowed const reference to the live options, valid for this object's lifetime. Subsequent loads or
   * direct edits change the referenced value; this is not a snapshot.
   */
  const sbio::cigi::ig::SIGSetupOptions& GetOptions() const;
  /**
   * @brief Replaces the setup options with defaults overridden by recognized INI keys.
   * @param filePath INI path opened as supplied, without adding a data-directory prefix.
   * @return True when reading and applying the configuration completes; false after a caught POCO or standard
   * exception. Success does not mean the resulting settings are valid for image-generator initialization.
   *
   * Missing keys retain SIGSetupOptions defaults, not values from previous loads or direct edits.
   * Read or parse failures leave the current options unchanged.
   *
   * Supported keys:
   * - `imageGeneratorID`: integer converted to uint16_t and wrapped as ImageGeneratorID.
   * - `hostToIGPort`, `igToHostPort`, `hostIPAddress`: populate one SHostSettings entry if any of these keys is
   *   present. Missing fields retain SHostSettings defaults. No host keys produces an empty host list.
   * - `igIPAddress`: copied as a string.
   * - `cigiVersion`: "3.3" or "4.0" selects the corresponding version; other values select UNKNOWN_VERSION.
   * - `synchronizationMode`: compared after lowercasing; only "synchronous" selects synchronous operation.
   *   An unrecognized value selects asynchronous operation; an absent key retains the default UNKNOWN mode.
   * - `PathToCigiSisoConversionsFile`: joined to globals.applicationsDataPath using filesystem path rules,
   *   not resolved relative to the INI file. The referenced file is not checked here.
   * - `PacketLogger`: exact values "None", "Read", and "Write" select the packet logger state; other values retain the default.
   * - `PacketTextLogger`: true only when the lowercased value is "true".
   * - `databaseControl`: "Host" selects host control and "IG" selects IG control; other values retain the default.
   */
  bool LoadOptions(const std::filesystem::path& filePath);

public:
  sbio::cigi::ig::SIGSetupOptions m_SetupOptions;///< Owned, mutable options; replaced on successful load and preserved on read or parse failure.
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
