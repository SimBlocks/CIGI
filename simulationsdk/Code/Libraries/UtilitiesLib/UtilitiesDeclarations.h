//Copyright SimBlocks LLC 2016-2026

/**
 * @file UtilitiesDeclarations.h
 * @brief Forward declarations and API macros for utilities library.
 *
 * This header provides forward declarations for core utility classes and interfaces
 * used throughout the SimBlocks utilities library, as well as DLL import/export macros.
 *
 * @details
 * - Provides DLL import/export macros for Windows dynamic/shared library usage.
 * - Declares the main utility classes, interfaces, and templates used in the library.
 * - Includes forward declarations for networking classes from Poco.
 * - Ensures minimal dependencies for files that only require type information.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_DECLARATIONS_H
#define SIMBLOCKS_UTILITIES_DECLARATIONS_H

#ifdef _WIN32
#ifdef SIMBLOCKS_UTILITIES_LIBRARY_EXPORTS
#define SIMBLOCKS_UTILITIES_LIBRARYAPI __declspec(dllexport)
#else
#define SIMBLOCKS_UTILITIES_LIBRARYAPI __declspec(dllimport)
#endif
#else
#define SIMBLOCKS_UTILITIES_LIBRARYAPI
#endif

/**
 * @namespace Poco::Net
 * @brief Forward declarations for Poco networking classes used in utilities.
 */
namespace Poco
{
  namespace Net
  {
    class SocketAddress;
    class DatagramSocket;
  }
}

/**
 * @namespace sbio::utils
 * @brief Forward declarations for utility classes, templates, and interfaces.
 */
namespace sbio
{
  namespace utils
  {
    // Core utility classes
    class CBuffer;
    class CEventDispatcher;
    class CLogger;
    class CLoggerManager;
    class CPacketLoggerReader;
    class CPacketLoggerWriter;
    class CStopWatch;
    class Event;
    class EventArgs;
    class EventHandler;
    class CTask;
    class CUDPSendSocket;
    class CUDPReceiveSocket;
    class CUDPReceiveMulticastSocket;
    class CZipCache;
    class ziplock;

    template <typename T>
    class TBuffer;

    enum class ELogLevel;

    // Utility interfaces
    struct IEventListener;
    struct ILogger;
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
