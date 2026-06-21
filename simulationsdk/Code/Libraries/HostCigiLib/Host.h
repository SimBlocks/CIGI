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
         */
        CHost();
        /**
         * @brief Destroys CHost instances.
         */
        ~CHost();

        /**
         * @brief Returns the entity-type conversion table used by the host.
         * @return Non-owning pointer to the owned entity-type registry, or `nullptr` before initialization.
         */
        sbio::cigi::CCigiEntityTypes* GetEntityTypes() const;

        /**
         * @brief Returns the configuration used to initialize the host.
          * @return Host setup options value.
         */
        const SHostSetupOptions& GetHostSetupOptions() const;

        /**
         * @brief Returns the currently active session.
         * @return Non-owning pointer to the active session, or `nullptr` when no active session exists.
         */
        CHostSession* GetHostSession() const;

        /**
         * @brief Returns a session by logical session identifier.
         * @param sessionID Session identifier to look up.
         * @return Non-owning pointer to the matching session, or `nullptr` if not found.
         */
        CHostSession* GetHostSession(sbio::SessionID sessionID) const;

        /**
         * @brief Lists all configured session identifiers.
          * @return Session ids value.
         */
        std::vector<sbio::SessionID> GetSessionIDs() const;

        /**
         * @brief Returns the session identifier currently selected for UI and command routing.
          * @return Active session id value.
         */
        sbio::SessionID GetActiveSessionID() const;

        /**
         * @brief Selects the session that subsequent host operations target.
         * @param sessionID Session identifier to activate.
         * @return `true` when the session exists and becomes active; otherwise `false`.
         */
        bool SetActiveSessionID(sbio::SessionID sessionID);

        /**
         * @brief Returns the scripting runtime associated with the host.
         * @return Non-owning pointer to the owned script runtime, or `nullptr` when scripting is disabled.
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
         */
        void Initialize(const SHostSetupOptions& options);

        /**
         * @brief Loads the optional CIGI-to-SISO entity conversion file configured for the host.
         */
        void LoadCigiToSisoEntityEnumerationConversionFile();

        /**
         * @brief Advances the host runtime by one application update step.
         * @param fDeltaTime Elapsed simulation or wall-clock time in seconds.
         */
        void Update(double fDeltaTime);

      protected:
        /**
         * @brief Processes inbound packets for the configured sessions.
         * @return `true` when packet processing succeeds for the current update step.
         */
        bool ProcessPackets();

        /**
         * @brief Flushes queued outbound packets for the configured sessions.
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
