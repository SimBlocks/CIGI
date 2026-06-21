//Copyright SimBlocks LLC 2016-2026
/**
 * @file FileSystemUtilities.cpp
 * @brief Implements file and directory utility functions.
 *
 * This file provides the implementation of functions for file and directory operations,
 * such as reading and writing binary files, querying and manipulating paths, checking
 * file and directory existence, and working with environment variables.
 *
 * Functions include:
 * - Reading and writing binary file contents
 * - Querying current working directory, executable directory, and file names
 * - Directory and file existence checks
 * - Directory creation, deletion, and copying
 * - File extension and name manipulation
 * - Environment variable access and modification
 * - Listing files and subdirectories
 *
 * @see sbio::utils namespace
 */
#include "FileSystemUtilities.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "UtilitiesLib/Logger.h"
#include "GlobalHeaders/CommonTypes.h"
#include "GlobalHeaders/Globals.h"
#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <limits>
#include <string>
#include <system_error>

#ifdef _WIN32
#include "UtilitiesLib/WindowsUtilities.h"
#else
#include "UtilitiesLib/LinuxUtilities.h"
#endif

using namespace std;
using namespace sbio;
using namespace sbio::utils;

namespace sbio
{
  namespace utils
  {
    extern SUtilitiesGlobals g_UtilitiesGlobals;

    std::filesystem::path GetCurrentWorkingDirectory()
    {
#ifdef _WIN32
      wchar_t* dir = _wgetcwd(nullptr, 0);
      if (dir == nullptr)
      {
        return "";
      }

      std::filesystem::path currentWorkingDirectory(dir);
      free(dir);
      return currentWorkingDirectory;
#else
      char* dir = getcwd(NULL, 0);

      if (dir == 0)
      {
        return "";
      }

      std::filesystem::path currentWorkingDirectory(dir);
      free(dir);
      return currentWorkingDirectory;
#endif
    }

    bool DirectoryExists(const std::filesystem::path& directoryPath)
    {
      std::error_code errorCode;
      const bool exists = std::filesystem::exists(directoryPath, errorCode);
      if (errorCode || !exists)
      {
        return false;
      }

      return std::filesystem::is_directory(directoryPath, errorCode) && !errorCode;
    }

    bool FileExists(const std::filesystem::path& filePath)
    {
      std::error_code errorCode;
      const bool exists = std::filesystem::exists(filePath, errorCode);
      if (errorCode || !exists)
      {
        return false;
      }

      return std::filesystem::is_regular_file(filePath, errorCode) && !errorCode;
    }

    bool IsFileLocked(const std::filesystem::path& filePath)
    {
      std::error_code errorCode;
      const bool exists = std::filesystem::exists(filePath, errorCode);
      if (errorCode || !exists)
      {
        return false;
      }

      errorCode.clear();
      if (!std::filesystem::is_regular_file(filePath, errorCode) || errorCode)
      {
        return false;
      }

      std::fstream fs(filePath, std::ios::in | std::ios::out | std::ios::binary);
      if (fs.is_open())
      {
        fs.close();
        return false;
      }

      return true;
    }

    bool IsDirectory(const std::filesystem::path& filePath)
    {
      std::error_code errorCode;
      return std::filesystem::is_directory(filePath, errorCode) && !errorCode;
    }

    std::string GetExtension(const std::filesystem::path& filePath)
    {
      return filePath.extension().string();
    }

    bool HasExtension(const std::filesystem::path& filePath)
    {
      return filePath.has_extension();
    }

    std::string GetFileName(const std::filesystem::path& filePath)
    {
      return filePath.filename().string();
    }

    std::string GetPreferredDrive()
    {
      const std::filesystem::path thirdPartyPath = GetEnvVariablePath("SBIO_THIRD_PARTY");

      // if the environment variable is not set
      if (thirdPartyPath.empty())
      {
        //iterate through drive letters to find the first available drive
        for (char drive = 'C'; drive <= 'Z'; ++drive)
        {
          std::string path = std::string(1, drive) + ":\\";
          if (std::filesystem::exists(path))
          {
            return path;
          }
        }
      }

      return thirdPartyPath.root_path().string();
    }

    std::string GetStrippedName(const std::filesystem::path& filePath)
    {
      return filePath.stem().string();
    }

    bool DeleteFile(const std::filesystem::path& filePath)
    {
      std::error_code errorCode;
      if (!std::filesystem::exists(filePath, errorCode) || errorCode)
      {
        return false;
      }

      try
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogDebug("Deleting File: " + filePath.string());
        }

        return std::filesystem::remove(filePath, errorCode) && !errorCode;
      }
      catch (...)
      {
        return false;
      }
    }

    void CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath)
    {
      try
      {
        if (!DirectoryExists(sourceDirectoryPath))
        {
          return;
        }

        if (!DirectoryExists(destinationDirectoryPath))
        {
          CreateDirectory(destinationDirectoryPath);
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(sourceDirectoryPath))
        {
          const auto& path = entry.path();
          std::filesystem::path filePath = std::filesystem::relative(path, sourceDirectoryPath);
          auto destinationPath = destinationDirectoryPath / filePath;

          if (std::filesystem::is_directory(path))
          {
            CreateDirectory(destinationPath);
          }
          else if (std::filesystem::is_regular_file(path))
          {
            std::filesystem::copy_file(path, destinationPath, std::filesystem::copy_options::overwrite_existing);
          }
        }
      }
      catch (const std::exception& e)
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError(e.what());
        }
      }
    }

    bool CreateDirectory(const std::filesystem::path& directoryPath)
    {
      if (std::filesystem::exists(directoryPath))
      {
        return false;
      }
      try
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogDebug("Creating Directory: " + directoryPath.string());
        }

        return std::filesystem::create_directories(directoryPath);
      }
      catch (...)
      {
        return false;
      }
    }

    bool CreateFile(const std::filesystem::path& filePath)
    {
      if (g_UtilitiesGlobals.pLogger != nullptr)
      {
        g_UtilitiesGlobals.pLogger->LogDebug("Creating File: " + filePath.string());
      }

      std::ofstream fout(filePath);
      fout.close();

      return std::filesystem::exists(filePath);
    }

    std::vector<std::string> FindFilesByExtension(const std::filesystem::path& directoryPath, const std::string& sExtension)
    {
      std::vector<std::string> files;

      if (!DirectoryExists(directoryPath))
      {
        return files;
      }

      std::error_code errorCode;
      std::filesystem::directory_iterator it(directoryPath, errorCode);
      if (errorCode)
      {
        return files;
      }

      const std::filesystem::directory_iterator end;

      while (it != end)
      {
        std::error_code entryErrorCode;
        if (it->is_regular_file(entryErrorCode) && !entryErrorCode && it->path().extension() == sExtension)
        {
          files.push_back(it->path().string());
        }

        it.increment(errorCode);
        if (errorCode)
        {
          return files;
        }
      }

      return files;
    }

    std::string GetDateTime()
    {
      auto now = std::chrono::system_clock::now();
      auto in_time_t = std::chrono::system_clock::to_time_t(now);

      std::tm localTime = {};
#ifdef _WIN32
      if (localtime_s(&localTime, &in_time_t) != 0)
      {
        return "";
      }
#else
      if (localtime_r(&in_time_t, &localTime) == nullptr)
      {
        return "";
      }
#endif

      std::stringstream ss;
      ss << std::put_time(&localTime, "%Y-%m-%d %X");
      return ss.str();
    }

    std::string GetLastDirectoryName(const std::filesystem::path& filePath)
    {
      std::error_code errorCode;
      if (std::filesystem::is_directory(filePath, errorCode) && !errorCode)
      {
        std::string sFilename = filePath.filename().string();
        if (sFilename.empty())
        {
          return filePath.parent_path().filename().string();
        }

        return sFilename;
      }

      return filePath.parent_path().filename().string();
    }

    std::filesystem::path GetLastDirectoryPath(const std::filesystem::path& filePath)
    {
      std::error_code errorCode;
      if (std::filesystem::is_directory(filePath, errorCode) && !errorCode)
      {
        return filePath;
      }

      return filePath.parent_path();
    }

    std::filesystem::path trim_after_folder(const std::filesystem::path& fullPath, const std::string& folderName)
    {
      std::filesystem::path result;

      for (const auto& part : fullPath)
      {
        result /= part;
        if (part == folderName)
        {
          break;
        }
      }

      return result;
    }

    bool IsSdkRootPath(const std::filesystem::path& sdkPath)
    {
      if (sdkPath.empty())
      {
        return false;
      }

      return DirectoryExists(sdkPath / "Data" / "Applications") && DirectoryExists(sdkPath / "Data" / "Libraries");
    }

    std::filesystem::path FindSdkRootPath(const std::filesystem::path& startPath)
    {
      if (startPath.empty())
      {
        return std::filesystem::path();
      }

      std::filesystem::path currentPath = startPath;
      std::error_code errorCode;
      if (std::filesystem::is_regular_file(currentPath, errorCode) && !errorCode)
      {
        currentPath = currentPath.parent_path();
      }

      while (!currentPath.empty())
      {
        if (IsSdkRootPath(currentPath))
        {
          return currentPath;
        }

        const std::filesystem::path parentPath = currentPath.parent_path();
        if (parentPath == currentPath)
        {
          break;
        }

        currentPath = parentPath;
      }

      return std::filesystem::path();
    }

    std::filesystem::path GetSdkPath()
    {
      const std::filesystem::path sdkExecutablePath = FindSdkRootPath(GetExecutableDirectory());
      if (!sdkExecutablePath.empty())
      {
        return sdkExecutablePath;
      }

      const std::filesystem::path sdkCurrentWorkingDirectoryPath = FindSdkRootPath(GetCurrentWorkingDirectory());
      if (!sdkCurrentWorkingDirectoryPath.empty())
      {
        return sdkCurrentWorkingDirectoryPath;
      }

      const std::filesystem::path sdkEnvironmentPath = GetEnvVariablePath("SBIO_SIMULATION_SDK");
      if (!sdkEnvironmentPath.empty())
      {
        if (IsSdkRootPath(sdkEnvironmentPath) || DirectoryExists(sdkEnvironmentPath))
        {
          return sdkEnvironmentPath;
        }

        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogWarning("SBIO_SIMULATION_SDK does not point to an existing directory: " + sdkEnvironmentPath.string());
        }
      }

      if (g_UtilitiesGlobals.pLogger != nullptr)
      {
        g_UtilitiesGlobals.pLogger->LogError("Unable to locate the SDK root path. Set SBIO_SIMULATION_SDK or run from within an SDK directory containing Data/Applications and Data/Libraries.");
      }

      return std::filesystem::path();
    }

    std::list<std::string> GetFilenamesInDirectory(const std::filesystem::path& directoryPath)
    {
      list<string> sFiles;

      if (!DirectoryExists(directoryPath))
      {
        return sFiles;
      }

      std::error_code errorCode;
      std::filesystem::directory_iterator it(directoryPath, errorCode);
      if (errorCode)
      {
        return sFiles;
      }

      const std::filesystem::directory_iterator end;

      while (it != end)
      {
        std::error_code entryErrorCode;
        if (it->is_regular_file(entryErrorCode) && !entryErrorCode)
        {
          sFiles.push_back(it->path().filename().string());
        }

        it.increment(errorCode);
        if (errorCode)
        {
          return sFiles;
        }
      }

      return sFiles;
    }

    std::list<std::string> GetSubdirectoryNames(const std::filesystem::path& directoryPath)
    {
      std::list<string> subdirectories;

      if (!DirectoryExists(directoryPath))
      {
        return subdirectories;
      }

      std::error_code errorCode;
      std::filesystem::directory_iterator it(directoryPath, errorCode);
      if (errorCode)
      {
        return subdirectories;
      }

      const std::filesystem::directory_iterator end;

      while (it != end)
      {
        std::error_code entryErrorCode;
        if (it->is_directory(entryErrorCode) && !entryErrorCode)
        {
          subdirectories.push_back(it->path().filename().string());
        }

        it.increment(errorCode);
        if (errorCode)
        {
          return subdirectories;
        }
      }

      return subdirectories;
    }

    std::list<std::filesystem::path> GetSubdirectoryPaths(const std::filesystem::path& directoryPath)
    {
      std::list<std::filesystem::path> subdirectories;

      if (!DirectoryExists(directoryPath))
      {
        return subdirectories;
      }

      std::error_code errorCode;
      std::filesystem::directory_iterator it(directoryPath, errorCode);
      if (errorCode)
      {
        return subdirectories;
      }

      const std::filesystem::directory_iterator end;

      while (it != end)
      {
        std::error_code entryErrorCode;
        if (it->is_directory(entryErrorCode) && !entryErrorCode)
        {
          subdirectories.push_back(it->path());
        }

        it.increment(errorCode);
        if (errorCode)
        {
          return subdirectories;
        }
      }

      return subdirectories;
    }

    void InitUtilitiesLib(const SGlobals& globals)
    {
      g_UtilitiesGlobals.pEventDispatcher = globals.pEventDispatcher;
      g_UtilitiesGlobals.pLogger = globals.pLogger;
    }

    std::string ReplaceExtension(const std::filesystem::path& filePath, const std::string& sExtension)
    {
      std::filesystem::path p = filePath;
      p.replace_extension(sExtension);

      return p.string();
    }

    void SetEnvVariable(const std::string& sEnvironmentVariableName, const std::string& sValue)
    {
#ifdef _WIN32
      const std::wstring variableName = Utf8ToWideString(sEnvironmentVariableName);
      const std::wstring value = Utf8ToWideString(sValue);
      _wputenv_s(variableName.c_str(), value.c_str());
#else
      setenv(sEnvironmentVariableName.c_str(), sValue.c_str(), 1);
#endif
    }

    void UninitUtilitiesLib()
    {
      g_UtilitiesGlobals.pEventDispatcher = nullptr;
      g_UtilitiesGlobals.pLogger = nullptr;
    }

    std::unique_ptr<CBuffer> ReadBinaryFileContents(const std::filesystem::path& filePath)
    {
      if (g_UtilitiesGlobals.pLogger != nullptr)
      {
        g_UtilitiesGlobals.pLogger->LogDebug("ReadBinaryFileContents: " + filePath.string());
      }

      std::ifstream fin(filePath, std::ios::binary);
      if (!fin.is_open())
      {
        return nullptr;
      }

      //get length of file
      fin.seekg(0, std::ios::end);
      const std::ifstream::pos_type endPosition = fin.tellg();
      if (endPosition < 0)
      {
        return nullptr;
      }

      const std::uintmax_t fileSize = static_cast<std::uintmax_t>(endPosition);
      if (fileSize > static_cast<std::uintmax_t>(std::numeric_limits<int>::max()))
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("ReadBinaryFileContents file too large: " + filePath.string());
        }

        return nullptr;
      }

      fin.seekg(0, std::ios::beg);
      if (!fin.good())
      {
        return nullptr;
      }

      const int size = static_cast<int>(fileSize);
      std::unique_ptr<CBuffer> pBuf = std::make_unique<CBuffer>(size);
      if (size == 0)
      {
        return pBuf;
      }

      //read contents of file into buffer
      fin.read(pBuf->GetBuffer(), static_cast<std::streamsize>(size));
      if (!fin || fin.gcount() != static_cast<std::streamsize>(size))
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("ReadBinaryFileContents failed to read full file: " + filePath.string());
        }

        return nullptr;
      }

      return pBuf;
    }

    void WriteBinaryFileContents(const std::filesystem::path& filePath, char* pBuffer, int nFileSize)
    {
      if (g_UtilitiesGlobals.pLogger != nullptr)
      {
        g_UtilitiesGlobals.pLogger->LogDebug("WriteBinaryFileContents: " + filePath.string());
      }

      if (nFileSize < 0)
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents received an invalid file size: " + filePath.string());
        }

        return;
      }

      if (pBuffer == nullptr && nFileSize > 0)
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents received a null buffer: " + filePath.string());
        }

        return;
      }

      std::ofstream fout(filePath, std::ios::binary);
      if (!fout.is_open())
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents failed to open file: " + filePath.string());
        }

        return;
      }

      //read contents of buffer into file
      fout.seekp(0, std::ios::beg);
      if (!fout.good())
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents failed to seek file: " + filePath.string());
        }

        return;
      }

      if (nFileSize == 0)
      {
        return;
      }

      // write buffer contents to file
      fout.write(pBuffer, static_cast<std::streamsize>(nFileSize));

      if (!fout.good())
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents failed to write file: " + filePath.string());
        }

        return;
      }

      // flush file
      fout.flush();

      if (!fout.good())
      {
        if (g_UtilitiesGlobals.pLogger != nullptr)
        {
          g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents failed to flush file: " + filePath.string());
        }

        return;
      }

      // close file
      fout.close();

      if (fout.fail() && g_UtilitiesGlobals.pLogger != nullptr)
      {
        g_UtilitiesGlobals.pLogger->LogError("WriteBinaryFileContents failed to close file: " + filePath.string());
      }
    }

    std::filesystem::path GetEnvVariablePath(const std::string& sEnvironmentVariableName)
    {
#ifdef _WIN32
      wchar_t* buf = nullptr;
      size_t size = 0;

      // On Windows, environment variables are stored as UTF-16 strings,
      // so we need to retrieve them as wide strings and convert to UTF-8.
      const std::wstring variableName = Utf8ToWideString(sEnvironmentVariableName);
      if (_wdupenv_s(&buf, &size, variableName.c_str()) == 0 && buf != nullptr)
      {
        std::filesystem::path p = WideToUtf8String(buf);
        free(buf);
        return p;
      }
#else
      // On POSIX systems, environment variables are typically stored as UTF-8 strings, so we can directly retrieve them.
      const char* buf = getenv(sEnvironmentVariableName.c_str());
      if (buf != nullptr)
      {
        std::filesystem::path p = std::filesystem::path(buf).string();
        return p;
      }
#endif

      return std::filesystem::path();
    }

    std::string GetEnvVariableString(const std::string& sEnvironmentVariableName)
    {
      std::string s;

#ifdef _WIN32
      wchar_t* buf = nullptr;
      size_t size = 0;

      // Windows environment variables are stored as wide strings, so we need to convert the variable name to wide string
      // and then convert the result back to UTF-8 string.
      const std::wstring variableName = Utf8ToWideString(sEnvironmentVariableName);
      if (_wdupenv_s(&buf, &size, variableName.c_str()) == 0 && buf != nullptr)
      {
        s = WideToUtf8String(buf);
        free(buf);
      }
#else
      // On POSIX systems, environment variables are typically stored as UTF-8 strings, so we can directly retrieve them.
      const char* buf = getenv(sEnvironmentVariableName.c_str());
      if (buf != nullptr)
      {
        s = buf;
      }
#endif

      return s;
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
