//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostCigiLibTypes.h
 * @brief Declares types, enums, and configuration structures for HostCigiLib module integration.
 *
 * Provides strong type definitions, configuration structures, and enums for host-side CIGI protocol integration.
 * Includes host globals, setup options, and session database state management for simulation and image generator interoperability.
 *
 * @see sbio::cigi::host::SHostCigiLibGlobals
 * @see sbio::cigi::host::SHostSetupOptions
 * @see sbio::cigi::host::EHostSessionDatabaseState
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HOST_LIB_TYPES_H
#define SIMBLOCKS_CIGI_HOST_LIB_TYPES_H

#include "GlobalHeaders/Globals.h"
#include "CigiLib/CigiTypes.h"
#include "HostCigiLib/HostCigiLibDeclarations.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>
#include <vector>

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief HostCigiLib global settings made available to the module at runtime.
       *
       * Ownership:
       * - `pHost` is non-owning.
       * - The pointed-to host object must outlive any library code that reads these globals.
       */
      struct SHostCigiLibGlobals : sbio::SGlobals
      {
        sbio::cigi::host::CHost* pHost = nullptr;///< Pointer to host instance
      };

      /**
       * @brief Configuration options for a single host session.
       *
       * Each entry in `SHostSetupOptions::sessions` overrides or supplements the top-level
       * connection defaults for one logical session.
       */
      struct SHostSessionSetupOptions
      {
        sbio::SessionID sessionID = sbio::SessionID(0);///< Logical session identifier
        uint16_t hostToIGPort = 0;///< Port for host-to-IG communication
        uint16_t igToHostPort = 0;///< Port for IG-to-host communication
        std::string hostIPAddress;///< Host IP address
        std::string igIPAddress;///< IG IP address
        sbio::DatabaseID defaultDatabaseID = sbio::UnknownDatabaseID;///< Default database ID for the session
      };

      /**
       * @brief Configuration options for initializing the CIGI host library.
       *
       * The top-level port and address fields describe the default connection used when a
       * single session is configured. When `sessions` contains entries, each session may supply
       * its own endpoint information while still sharing the remaining host-wide settings.
       */
      struct SHostSetupOptions
      {
        uint16_t hostToIGPort = 0;///< Port for host-to-IG communication
        uint16_t igToHostPort = 0;///< Port for IG-to-host communication
        std::string hostIPAddress;///< Host IP address
        std::string igIPAddress;///< IG IP address
        ECigiVersion eCigiVersion = ECigiVersion::UNKNOWN_VERSION;///< CIGI protocol version
        sbio::cigi::ECigiSynchronizationMode eSynchronizationMode = sbio::cigi::ECigiSynchronizationMode::ASYNCHRONOUS;///< Host synchronization mode
        sbio::DatabaseID defaultDatabaseID = sbio::UnknownDatabaseID;///< Default database ID
        bool bigEndianByteOrder = false;///< Use big endian byte order
        std::filesystem::path pathToCigiSisoConversionsFile;///< Path to CIGI-SISO conversion file
        bool bEnableScripts = true;///< Enable scripting support
        bool bDatabaseIGControlled = false;///< Database controlled by IG
        std::vector<SHostSessionSetupOptions> sessions;///< Per-session connection settings
      };

      /**
       * @brief Session database state for host-side CIGI protocol management.
       *
       * The host updates this state as it requests database changes and receives responses from
       * the image generator during session startup or database switching.
       */
      enum class EHostSessionDatabaseState
      {
        NO_DATABASE,///< No database has been requested to be loaded
        LOAD_DATABASE_REQUESTED,///< Request a database to be loaded
        LOADING_ACKNOWLEDGED,///< Host acknowledges that the IG has begun loading a database
        LOADED,///< The database has been loaded
        IG_CONTROLLED,///< The database is fully controlled by the IG
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
