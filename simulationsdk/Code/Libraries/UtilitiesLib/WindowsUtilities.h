//Copyright SimBlocks LLC 2016-2026
#pragma once

#include <filesystem>
#include <string>

/**
 * @brief Identifies filesystem entries that should not be traversed during copying.
 * @param path Filesystem entry to inspect.
 * @return `true` when the entry should be skipped; otherwise `false`.
 */
bool ShouldSkipFileSystemEntry(const std::filesystem::path& path);

/**
 * @brief Converts a UTF-8 string to a wide-character string.
 * @param value UTF-8 encoded string to convert.
 * @return Wide-character string converted from UTF-8 input.
 */
std::wstring Utf8ToWideString(const std::string& value);
/**
 * @brief Converts a wide-character string to UTF-8.
 * @param value Null-terminated wide-character string to convert.
 * @return UTF-8 string converted from wide-character input.
 */
std::string WideToUtf8String(const wchar_t* value);

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026