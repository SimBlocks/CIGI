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
 */
class CHostEmulatorOptions
{
public:
  /**
   * @brief Loads options from a configuration file.
   * @param filePath Path to the configuration file.
   */
  void LoadOptions(const std::filesystem::path& filePath);

public:
  sbio::cigi::host::SHostSetupOptions hostSetupOptions;///< Host setup options
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
