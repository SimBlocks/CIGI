//Copyright SimBlocks LLC 2016-2026
/**
 * @file StringUtilities.cpp
 * @brief Implements string utility functions for common string operations.
 *
 * This file provides the implementation of functions for converting buffers to hex,
 * finding the longest common prefix, splitting strings, and stripping prefixes. Includes
 * support for file path-aware prefix matching.
 *
 * @see sbio::utils namespace
 */
#include "StringUtilities.h"
#include "Poco/UUIDGenerator.h"

using namespace std;

namespace sbio
{
  namespace utils
  {
    // Hexadecimal character lookup table.
    char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    /**
     * @brief Converts a buffer to a hexadecimal character buffer.
     * @param inBuffer Pointer to the input buffer.
     * @param sizeOfBufferInBytes Size of the input buffer in bytes.
     * @return Pointer to a string containing the hexadecimal representation.
     */
    string ConvertToHex(unsigned char* inBuffer, int sizeOfBufferInBytes)
    {
      if (inBuffer == nullptr || sizeOfBufferInBytes <= 0)
      {
        return "";
      }

      string hexString;
      hexString.resize(static_cast<size_t>(sizeOfBufferInBytes) * 2);// Each byte becomes two hex characters

      for (int i = 0; i < sizeOfBufferInBytes; i++)
      {
        hexString[i * 2] = hex_chars[(*inBuffer & 0xF0) >> 4];
        hexString[i * 2 + 1] = hex_chars[(*inBuffer++ & 0x0F)];
      }

      return hexString;
    }

    /**
     * @brief Finds the longest prefix shared by all given strings.
     * @param strings List of strings to compare.
     * @param isFilePath If true, treats slashes ('/' and '\\') as equivalent for file paths.
     * @return The longest common prefix among the strings.
     */
    std::string FindLongestCommonPrefix(const std::vector<std::string>& strings, bool isFilePath)
    {
      if (strings.empty())
      {
        return "";
      }

      // Start with the first string as the prefix
      std::string prefix = strings[0];

      // Loop through every string and cut off parts of the prefix that they don't have in common
      for (const std::string& str : strings)
      {
        size_t i = 0;
        // Count indices until hitting the end of either the prefix or the current string.
        while (i < prefix.length() && i < str.length())
        {
          // If the characters at this position are the same, increment i to check the next character position.
          if (prefix[i] == str[i])
          {
            i++;
          }
          // If this is a path and both characters at this position are a slash, the file path is still equivalent.
          else if (isFilePath && (prefix[i] == '\\' || prefix[i] == '/') && (str[i] == '\\' || str[i] == '/'))
          {
            // Note: When finding a common prefix for a list of filepaths, this prevents differing slashes from always failing to make a prefix
            i++;
          }
          // Otherwise, if the two characters are different, then the previous i is the last character they had in common
          else
          {
            // Quit the loop. i is now the last index that the prefix and the current string had in common
            break;
          }
        }
        // Cut off any part of the prefix that it didn't have in common with this string. This will happen for every string.
        prefix = prefix.substr(0, i);

        // If the prefix is empty, then the strings have nothing in common. We can stop here, no need to keep checking.
        if (prefix.empty())
        {
          break;// Early exit if no common prefix
        }
      }

      return prefix;
    }

    /**
     * @brief Splits a string by a delimiter character.
     * @param s The string to split.
     * @param delimiter The delimiter character.
     * @return Vector of substrings.
     */
    std::vector<std::string> SplitString(const std::string& s, char delimiter)
    {
      vector<string> tokens;
      size_t start = 0;
      size_t end = s.find(delimiter);

      while (end != std::string::npos)
      {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
      }

      tokens.push_back(s.substr(start));
      return tokens;
    }

    /**
     * @brief Removes a prefix from a string if present.
     * @param s The string to process.
     * @param sPrefix The prefix to remove.
     * @return The string with the prefix removed, if it was present.
     */
    std::string StripPrefix(const std::string& s, const std::string& sPrefix)
    {
      if (s.compare(0, sPrefix.length(), sPrefix) != 0)
      {
        return s;
      }

      return s.substr(sPrefix.length());
    }

    Poco::UUIDGenerator g_uuidGenerator;

    std::string GenerateUUID()
    {
      string guid = g_uuidGenerator.createRandom().toString();
      guid.erase(std::remove(guid.begin(), guid.end(), '-'), guid.end());
      return guid;
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
