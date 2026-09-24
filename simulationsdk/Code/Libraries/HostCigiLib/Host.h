//Copyright SimBlocks LLC 2016-2026
/**
 * @file Host.h
 * @brief Declares the CHost class for managing CIGI host sessions and protocol integration.
 *
 * Provides the CHost class for managing host sessions, entity types, and protocol integration in a CIGI-based simulation.
 * Supports session management, entity type conversion, script runtime integration, and host-to-IG communication.
 * Includes initialization, update, and packet processing logic for simulation interoperability.
 *
 * @see sbio::cigi::host::CHost
 * @see sbio::cigi::host::CHostSession
 * @see sbio::cigi::CCigiEntityTypes
 * @see sbio::cigi::host::CScriptRuntime
 * @see SHostSetupOptions
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HOST_H
#define SIMBLOCKS_CIGI_HOST_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "CigiLib/CigiTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include "HostCigiLib/HostCigiLibDeclarations.h"
#include "HostCigiLib/HostCigiLibTypes.h"
#include "HostCigiLib/ScriptRuntime.h"
#include <map>
#include <memory>
#include <vector>

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Manages CIGI host sessions, entity types, and protocol integration.
       *
       * Supports session management, entity type conversion, script runtime integration, and host-to-IG communication.
       *
       * Ownership:
       * - `CHost` owns its `CHostSession`, `CScriptRuntime`, and `CCigiEntityTypes` instances.
       * - Accessors return non-owning pointers to those owned objects.
       */
      class CHost
      {
      public:
        /**
         * @brief Constructs a `CHost` instance.
         *
         * Creates the entity-type registry; sessions and scripting are created by `Initialize()`.
         */
        CHost();
        /**
         * @brief Destroys the owned sessions, script runtime, and entity-type registry.
         */
        ~CHost();

        /**
         * @brief Returns the entity-type conversion table used by the host.
         * @return Non-owning pointer to the registry created by the constructor, valid for this host's lifetime.
         */
        sbio::cigi::CCigiEntityTypes* GetEntityTypes() const;

        /**
         * @brief Returns the configuration used to initialize the host.
         * @return Reference to stored options, owned by this host; default options before initialization.
         */
        const SHostSetupOptions& GetHostSetupOptions() const;

        /**
         * @brief Returns the currently active session.
         * @return Non-owning pointer to the active session, or `nullptr` when no active session exists.
         *         Reinitializing or destroying the host invalidates pointers to replaced sessions.
         */
        CHostSession* GetHostSession() const;

        /**
         * @brief Returns a session by logical session identifier.
         * @param sessionID Session identifier to look up.
         * @return Non-owning pointer to the matching session, or `nullptr` if not found.
         *         Reinitializing or destroying the host invalidates pointers to replaced sessions.
         */
        CHostSession* GetHostSession(sbio::SessionID sessionID) const;

        /**
         * @brief Lists all configured session identifiers.
         * @return Copy of the session identifiers in ascending order; empty when no sessions exist.
         */
        std::vector<sbio::SessionID> GetSessionIDs() const;

        /**
         * @brief Returns the session identifier currently selected for UI and command routing.
         * @return Selected identifier; initially zero, which need not identify an existing session.
         */
        sbio::SessionID GetActiveSessionID() const;

        /**
         * @brief Selects the session that subsequent host operations target.
         * @param sessionID Session identifier to activate.
         * @return `true` when the session exists and becomes active; `false` if it does not exist
         * or switching sessions would redirect a running script.
         */
        bool SetActiveSessionID(sbio::SessionID sessionID);

        /**
         * @brief Returns the scripting runtime associated with the host.
         * @return Non-owning runtime pointer, or `nullptr` before initialization or when scripting is disabled.
         *         Reinitialization can replace the runtime and invalidate this pointer.
         */
        sbio::cigi::host::CScriptRuntime* GetScriptRuntime() const;

        /**
         * @brief Initializes sessions and optional subsystems from the supplied options.
         * @param options Host-wide setup options.
         *
         * Side effects:
         * - Creates protocol-specific session objects.
         * - Initializes optional scripting support.
         * - Stores `options` for later queries.
         *
         * An empty session list creates session zero from the top-level settings. Otherwise, zero ports,
         * empty addresses, and unknown database IDs inherit the top-level values. Duplicate session IDs
         * or duplicate nonzero effective receive ports raise an error event and leave existing state intact.
         * Only CIGI 3.3 and 4.0 are supported; other versions raise an error after existing sessions are cleared.
         * Replaces the script runtime and sessions on valid configuration; socket failures are reported by events.
         * An unknown synchronization mode uses asynchronous host updates.
         */
        void Initialize(const SHostSetupOptions& options);

        /**
         * @brief Loads the optional CIGI-to-SISO entity conversion file configured for the host.
         *
         * Uses the stored CSV path. An empty path or a caught standard exception is reported to stdout.
         */
        void LoadCigiToSisoEntityEnumerationConversionFile();

        /**
         * @brief Updates scripting, sends asynchronous traffic, and processes incoming traffic for all sessions.
         * @param fDeltaTime Unused; script timing comes from the active session's stopwatch.
         */
        void Update(double fDeltaTime);

      protected:
        /**
         * @brief Processes up to 64 received datagrams per session to bound receive work.
         * @return `true` if any session received a datagram, including malformed traffic; otherwise `false`.
         */
        bool ProcessPackets();

        /**
         * @brief Calls `CHostSession::SendPackets()` once for each session; deferred packets may remain queued.
         */
        void SendPackets();

      private:
        SHostSetupOptions m_HostSetupOptions;///< Host setup options
        sbio::cigi::ECigiSynchronizationMode m_eSynchronizationMode = ECigiSynchronizationMode::UNKNOWN;///< Synchronization mode
        sbio::SessionID m_ActiveSessionID = sbio::SessionID(0);///< Currently active session

        std::map<sbio::SessionID, std::unique_ptr<CHostSession>> m_Sessions;///< Host sessions
        std::unique_ptr<sbio::cigi::host::CScriptRuntime> m_pScriptRuntime;///< Script runtime
        std::unique_ptr<sbio::cigi::CCigiEntityTypes> m_pCigiEntityTypes;///< CIGI entity types
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
