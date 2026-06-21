//Copyright SimBlocks LLC 2016-2026
/**
 * @file WindowsUtilities.cpp
 * @brief Windows-specific implementation for Utilities Library.
 *
 * This file provides Windows-specific utility functions for retrieving the executable's
 * directory and file name, using the Windows API and C++ standard library features.
 *
 * @details
 * - Implements GetExecutableDirectory() and GetExecutableFileName() for Windows.
 * - Uses GetModuleFileNameW to retrieve the path to the running executable.
 * - Utilizes std::filesystem for path manipulation.
 *
 * @see sbio::utils::GetExecutableDirectory
 * @see sbio::utils::GetExecutableFileName
 */
#include "UtilitiesLib.h"
#include <Windows.h>
#include "GlobalHeaders/CommonTypes.h"
#include "GlobalHeaders/Globals.h"
#include "Buffer.h"
#include <direct.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace sbio;
using namespace sbio::utils;
using namespace std;
using namespace std::filesystem;

std::wstring Utf8ToWideString(const std::string& value)
{
  if (value.empty())
  {
    return std::wstring();
  }

  // Calculate the required size for the wide string buffer, including the null terminator.
  const int requiredSize = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
  if (requiredSize <= 0)
  {
    return std::wstring();
  }

  // Create a wide string with the required size, excluding the null terminator.
  std::wstring result(static_cast<size_t>(requiredSize - 1), L'\0');
  if (MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &result[0], requiredSize) == 0)
  {
    return std::wstring();
  }

  return result;
}

std::string WideToUtf8String(const wchar_t* value)
{
  // Check for null or empty input.
  if (value == nullptr || value[0] == L'\0')
  {
    return std::string();
  }

  // Calculate the required size for the UTF-8 string buffer, including the null terminator.
  const int requiredSize = WideCharToMultiByte(CP_UTF8, 0, value, -1, nullptr, 0, nullptr, nullptr);
  if (requiredSize <= 1)
  {
    return std::string();
  }

  // Create a string with the required size, excluding the null terminator.
  std::string result(static_cast<size_t>(requiredSize - 1), '\0');
  if (WideCharToMultiByte(CP_UTF8, 0, value, -1, &result[0], requiredSize, nullptr, nullptr) == 0)
  {
    return std::string();
  }

  return result;
}

namespace sbio
{
  namespace utils
  {
    std::filesystem::path GetExecutablePath()
    {
      std::wstring buffer(MAX_PATH, L'\0');

      while (true)
      {
        DWORD length = GetModuleFileNameW(nullptr, &buffer[0], static_cast<DWORD>(buffer.size()));

        if (length == 0)
        {
          return path();
        }

        if (length < buffer.size())
        {
          buffer.resize(length);
          return std::filesystem::path(buffer);
        }

        buffer.resize(buffer.size() * 2);
      }
    }

    /**
     * @brief Gets the directory path of the currently running executable.
     *
     * Uses the Windows API to retrieve the full path to the executable, then extracts
     * and returns the parent directory as a path.
     *
     * @return The directory containing the executable.
     */
    std::filesystem::path GetExecutableDirectory()
    {
      return GetExecutablePath().parent_path();
    }

    /**
     * @brief Gets the file name (without extension) of the currently running executable.
     *
     * Uses the Windows API to retrieve the full path to the executable, then extracts
     * and returns the file name (without extension) as a path.
     *
     * @return The executable file name without extension.
     */
    std::filesystem::path GetExecutableFileName()
    {
      return GetExecutablePath().stem();
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
