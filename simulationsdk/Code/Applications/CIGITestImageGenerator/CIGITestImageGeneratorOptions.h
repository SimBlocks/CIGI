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
 * @brief Options class for configuring the CIGI test image generator.
 *
 * Loads and provides access to SIGSetupOptions for image generator configuration.
 */
class CCIGITestImageGeneratorOptions
{
public:
  /**
   * @brief Gets the current image generator setup options.
   * @return Reference to SIGSetupOptions.
   */
  const sbio::cigi::ig::SIGSetupOptions& GetOptions() const;
  /**
   * @brief Loads image generator options from a file.
   * @param filePath Path to the options file.
   * @return True if the options were loaded successfully; otherwise false.
   */
  bool LoadOptions(const std::filesystem::path& filePath);

public:
  sbio::cigi::ig::SIGSetupOptions m_SetupOptions;///< Image generator setup options
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
