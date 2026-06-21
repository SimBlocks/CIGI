//Copyright SimBlocks LLC 2016-2026
/**
 * @file HostCigiLibDeclarations.h
 * @brief Forward declarations and type definitions for HostCigiLib module classes and enums.
 *
 * Provides forward declarations for host-side CIGI protocol integration, including host, session, and view manager classes,
 * setup options, and session database state enums. Reduces dependencies and improves compile times by avoiding unnecessary includes.
 *
 * @see sbio::cigi::host::CHost
 * @see sbio::cigi::host::CHostSession
 * @see sbio::cigi::host::CHostViewManager
 * @see sbio::cigi::host::SHostSetupOptions
 * @see sbio::cigi::host::EHostSessionDatabaseState
 */
#pragma once
#ifndef SIMBLOCKS_HOST_CIGI_LIG_DECLARATIONS_H
#define SIMBLOCKS_HOST_CIGI_LIG_DECLARATIONS_H

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      class CHost;
      class CHostSession;
      class CHostViewManager;

      struct SHostSetupOptions;

      enum class EHostSessionDatabaseState;
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
