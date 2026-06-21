//Copyright SimBlocks LLC 2016-2026
/**
 * @file StringUtilities.h
 * @brief Declares string utility functions for common string operations.
 *
 * Provides functions for converting buffers to hex, finding the longest common prefix,
 * splitting strings, and stripping prefixes. Includes support for file path-aware prefix matching.
 */
#pragma once

#ifndef SIMBLOCKS_UTILITIES_STRING_UTILITIES_H
#define SIMBLOCKS_UTILITIES_STRING_UTILITIES_H

#include <cstdint>
#include <vector>
#include <string>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Converts a buffer to a hexadecimal character buffer.
     * @param inBuffer Pointer to the input buffer.
     * @param sizeOfBuffer Size of the input buffer in bytes.
     * @return Pointer to a string containing the hexadecimal representation.
     */
    std::string ConvertToHex(unsigned char* inBuffer, int sizeOfBuffer);

    /**
     * @brief Finds the longest prefix shared by all given strings.
     * @param strings List of strings to compare.
     * @param isFilePath If true, treats slashes ('/' and '\\') as equivalent for file paths.
     * @return The longest common prefix among the strings.
     */
    std::string FindLongestCommonPrefix(const std::vector<std::string>& strings, bool isFilePath = false);

    /**
     * @brief Splits a string by a delimiter character.
     * @param s The string to split.
     * @param delimiter The delimiter character.
     * @return Vector of substrings.
     */
    std::vector<std::string> SplitString(const std::string& s, char delimiter);

    /**
     * @brief Removes a prefix from a string if present.
     * @param s The string to process.
     * @param sPrefix The prefix to remove.
     * @return The string with the prefix removed, if it was present.
     */
    std::string StripPrefix(const std::string& s, const std::string& sPrefix);

    /**
     * @brief Generates a UUID string.
     * @return The generated UUID as a string.
     */
    std::string GenerateUUID();
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
