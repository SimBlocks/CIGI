//Copyright SimBlocks LLC 2016-2026
/**
 * @file FileSystemUtilities.h
 * @brief Declares file and directory utility functions.
 *
 * Provides functions for file and directory operations, such as reading and writing binary files,
 * querying and manipulating paths, checking file and directory existence, and working with environment variables.
 *
 * Functions include:
 * - Reading and writing binary file contents
 * - Querying current working directory, executable directory, and file names
 * - Directory and file existence checks
 * - Directory creation, deletion, and copying
 * - File extension and name manipulation
 * - Environment variable access and modification
 * - Listing files and subdirectories
 */
#pragma once
#ifndef SIMBLOCKS_FILE_SYSTEM_UTILITIES_H
#define SIMBLOCKS_FILE_SYSTEM_UTILITIES_H

#include "GlobalHeaders/CommonDeclarations.h"
#include "UtilitiesDeclarations.h"
#include <filesystem>
#include <memory>
#include <string>
#include <list>
#include <vector>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Reads the contents of a binary file into a buffer.
     * @param filePath Path to the file.
     * @return Unique pointer to a buffer containing the file contents.
     */
    std::unique_ptr<sbio::utils::CBuffer> ReadBinaryFileContents(const std::filesystem::path& filePath);

    /**
     * @brief Writes binary data to a file.
     * @param filePath Path to the file.
     * @param pBuffer Pointer to the data buffer.
     * @param nFileSize Size of the data in bytes.
     */
    void WriteBinaryFileContents(const std::filesystem::path& filePath, char* pBuffer, int nFileSize);

    /** @brief Gets the current working directory. */
    std::filesystem::path GetCurrentWorkingDirectory();
    /** @brief Gets the directory of the running executable. */
    std::filesystem::path GetExecutableDirectory();
    /** @brief Gets the file name of the running executable (without extension). */
    std::filesystem::path GetExecutableFileName();
    /** @brief Gets the full path of the running executable. */
    std::filesystem::path GetExecutablePath();
    /** @brief Gets the extension of a file. */
    std::string GetExtension(const std::filesystem::path& filePath);
    /** @brief Extracts the file name from a path. */
    std::string GetFileName(const std::filesystem::path& filePath);
    /** @brief Gets the preferred drive for file operations. */
    std::string GetPreferredDrive();
    /** @brief Removes path and extension from a file name. */
    std::string GetStrippedName(const std::filesystem::path& filePath);
    /** @brief Checks if a directory exists. */
    bool DirectoryExists(const std::filesystem::path& sDirectory);
    /** @brief Checks if a file exists. */
    bool FileExists(const std::filesystem::path& filePath);
    /** @brief Checks if a file is locked. */
    bool IsFileLocked(const std::filesystem::path& filePath);
    /** @brief Checks if a path is a directory. */
    bool IsDirectory(const std::filesystem::path& filePath);
    /** @brief Checks if a file has an extension. */
    bool HasExtension(const std::filesystem::path& filePath);
    /** @brief Deletes a file. */
    bool DeleteFile(const std::filesystem::path& filePath);
    /** @brief Copies a directory and its contents. */
    void CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath);
    /** @brief Creates a directory. */
    bool CreateDirectory(const std::filesystem::path& directoryPath);
    /** @brief Creates a file. */
    bool CreateFile(const std::filesystem::path& filePath);
    /** @brief Finds files by extension in a directory. */
    std::vector<std::string> FindFilesByExtension(const std::filesystem::path& directoryPath, const std::string& sExtension);
    /** @brief Gets the current date and time as a string. */
    std::string GetDateTime();
    /** @brief Gets the value of an environment variable as a path. */
    std::filesystem::path GetEnvVariablePath(const std::string& sEnvironmentVariableName);
    /** @brief Gets the value of an environment variable as a string. */
    std::string GetEnvVariableString(const std::string& sEnvironmentVariableName);
    /** @brief Gets the last directory name from a path. */
    std::string GetLastDirectoryName(const std::filesystem::path& directoryPath);
    /** @brief Gets the path to the last directory in a path. */
    std::filesystem::path GetLastDirectoryPath(const std::filesystem::path& directoryPath);
    /** @brief Gets the SDK path by discovering the SDK root from runtime paths or from SBIO_SIMULATION_SDK. */
    std::filesystem::path GetSdkPath();
    /** @brief Gets a list of file names in a directory. */
    std::list<std::string> GetFilenamesInDirectory(const std::filesystem::path& directoryPath);
    /** @brief Gets a list of subdirectory names in a directory. */
    std::list<std::string> GetSubdirectoryNames(const std::filesystem::path& directoryPath);
    /** @brief Gets a list of subdirectory paths in a directory. */
    std::list<std::filesystem::path> GetSubdirectoryPaths(const std::filesystem::path& directoryPath);
    /** @brief Replaces the extension of a file. */
    std::string ReplaceExtension(const std::filesystem::path& filePath, const std::string& sExtension);
    /** @brief Sets the value of an environment variable. */
    void SetEnvVariable(const std::string& sEnvironmentVariableName, const std::string& sValue);
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
