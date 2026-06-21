//Copyright SimBlocks LLC 2016-2026
/**
 * @file IGCIGILibTypes.h
 * @brief Declares types and options for SimBlocks CIGI IG library configuration and setup.
 *
 * Provides enums and structs for configuring packet logging, host settings, and image generator setup options for SimBlocks CIGI IG integration.
 * Integrates with SimBlocks CIGI, IG, and image generator libraries for simulation configuration and communication.
 *
 * @see sbio::cigi::ig::EPacketLoggerState
 * @see sbio::cigi::ig::SHostSettings
 * @see sbio::cigi::ig::SIGSetupOptions
 */
#pragma once
#ifndef SIMBLOCKS_IG_CIGI_LIB_TYPES_H
#define SIMBLOCKS_IG_CIGI_LIB_TYPES_H

#include "CigiLib/CigiTypes.h"
#include <list>
#include <string>
#include <filesystem>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Selects how host-to-IG packets are recorded or replayed for one image-generator session.
       */
      enum class EPacketLoggerState
      {
        NONE = 0,///< Do not read from or write to a packet log.
        READ,///< Replay packets from a packet log.
        WRITE,///< Record received packets to a packet log.
      };

      /**
       * @brief Defines one host endpoint used by an image generator.
       */
      struct SHostSettings
      {
        uint16_t hostToIGPort = 0;///< UDP port used for host-to-IG traffic.
        uint16_t igToHostPort = 0;///< UDP port used for IG-to-host traffic.
        std::string hostIPAddress;///< Host IP address or name.
      };

      /**
       * @brief Captures the startup configuration for one CIGI image generator.
       */
      struct SIGSetupOptions
      {
        sbio::ImageGeneratorID imageGeneratorID = sbio::UnknownImageGeneratorID;///< Logical image-generator identifier.
        std::list<SHostSettings> hostSettings;///< Host endpoints to initialize for the session.
        std::string igIPAddress;///< Local IG address used for outbound communication.
        sbio::cigi::ECigiVersion eCigiVersion = sbio::cigi::ECigiVersion::UNKNOWN_VERSION;///< CIGI protocol version to speak.
        sbio::cigi::ECigiSynchronizationMode eSynchronizationMode = sbio::cigi::ECigiSynchronizationMode::UNKNOWN;///< Frame-timing mode for host communication.
        bool bInterpolationEnabled = false;///< Initial interpolation state used for entity updates.
        std::filesystem::path pathToCigiSisoConversionsFile;///< Optional path to the CIGI-to-SISO entity conversion table.
        EPacketLoggerState ePacketLoggerState = EPacketLoggerState::NONE;///< Packet-log operating mode.
        bool bLogPacketText = false;///< Enables human-readable CIGI message logging (independent of binary packet logging).
        bool bDatabaseControlledByIG = false;///< When `true`, database selection is controlled internally by the IG.
        sbio::cigi::CigiDatabaseNumber defaultIGControlledDatabaseID = sbio::cigi::UnknownCigiDatabaseNumber;///< Default database identifier used when the IG controls database selection.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
