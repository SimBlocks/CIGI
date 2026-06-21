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
 * @brief Extended host setup options for CIGIHost, including script configuration.
 */
struct SCigiHostSetupOptions : sbio::cigi::host::SHostSetupOptions
{
  std::string sScriptCategory;///< Script category name
  std::string sScript;///< Script file name
};

/**
 * @brief Manages loading and storage of CIGIHost configuration options.
 */
class CCIGIHostOptions
{
public:
  /**
   * @brief Loads options from a configuration file.
   * @param filePath Path to the configuration file.
   */
  void LoadOptions(const std::filesystem::path& filePath);

public:
  SCigiHostSetupOptions hostSetupOptions;///< Host setup options
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
