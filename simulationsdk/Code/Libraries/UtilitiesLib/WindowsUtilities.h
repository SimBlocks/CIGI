//Copyright SimBlocks LLC 2016-2026
#pragma once

#include <string>

/**
 * @brief Returns the result of `Utf8ToWideString`.
 * @param value Value value.
 * @return Wide-character string converted from UTF-8 input.
 */
std::wstring Utf8ToWideString(const std::string& value);
/**
 * @brief Returns the result of `WideToUtf8String`.
 * @param value Value value.
 * @return UTF-8 string converted from wide-character input.
 */
std::string WideToUtf8String(const wchar_t* value);

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026