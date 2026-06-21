//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostCigiLib.h
 * @brief Core declarations and initialization for the HostCigiLib module.
 *
 * Provides conversion functions, type declarations, and initialization utilities for the HostCigiLib module.
 * Supports CIGI protocol integration, host session management, and IG-to-Host communication for simulation interoperability.
 * Includes global initialization and conversion helpers for host-side CIGI protocol support.
 *
 * @see sbio::cigi::host namespace
 * @see sbio::cigi::host::CHost
 * @see sbio::SGlobals
 * @see InitHostCigiLib
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HOST_LIB_H
#define SIMBLOCKS_CIGI_HOST_LIB_H

#include "CigiLib/CigiTypes.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiLibDeclarations.h"
#include "HostCigiLibTypes.h"
#include "libCIGI/Packets/4_0/EntityCtrl.h"
#include "libCIGI/Packets/4_0/IGCtrl.h"
#include <string>

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Converts a host session database state to a user-facing string.
       *
       * @param eHostSessionDatabaseState Database state to format.
       * @return Text used by the host-side tools and logs for the supplied state.
       */
      std::string ConvertToString(sbio::cigi::host::EHostSessionDatabaseState eHostSessionDatabaseState);

      /**
       * @brief Maps the shared active-state enum to the CIGI 4.0 entity-state enum.
       *
       * @param state Shared entity active state.
       * @return Matching `CIGI::V40::EntityCtrl::EntityState` value.
       */
      CIGI::V40::EntityCtrl::EntityState ConvertToEntityState(sbio::cigi::EActiveState state);

      /**
       * @brief Converts the library's IG mode enum to the CIGI 4.0 `IGCtrl` representation.
       *
       * @param eIGMode Host-side IG mode value.
       * @return Matching `CIGI::V40::IGCtrl::IGMode` value.
       */
      CIGI::V40::IGCtrl::IGMode ToIGMode(sbio::cigi::EIGMode eIGMode);

      /**
       * @brief Initializes HostCigiLib global services.
       *
       * @param globals Application globals containing shared services and paths.
       * @param pHost Non-owning pointer to the active host runtime.
       *
       * Side effects:
       * - Stores the supplied globals in HostCigiLib module state.
       * - Makes the host pointer available to code that consults the module globals.
       *
       * Ownership:
       * - `pHost` is non-owning and must outlive any HostCigiLib code that reads it.
       */
      void InitHostCigiLib(const sbio::SGlobals& globals, sbio::cigi::host::CHost* pHost);
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
