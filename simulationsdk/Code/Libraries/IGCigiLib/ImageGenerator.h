//Copyright SimBlocks LLC 2016-2026
/**
 * @file ImageGenerator.h
 * @brief Declares the CCigiImageGenerator class for SimBlocks CIGI IG image generator management and event handling.
 *
 * Provides the CCigiImageGenerator class for managing image generator state, database loading, packet processing, event handling, and simulation control in the SimBlocks CIGI IG library.
 * Inherits from IIGCIGIEventListener and integrates with SimBlocks CIGI, engine, math, and utility types for simulation and event management.
 * Supports database operations, frame and mode management, error reporting, and event listener callbacks.
 *
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::ig::IIGCIGIEventListener
 * @see sbio::cigi::ig::SIGSetupOptions
 * @see sbio::cigi::ig::EIGDatabaseState
 * @see sbio::cigi::ig::EIGMode
 * @see sbio::cigi::ig::ECigiSynchronizationMode
 * @see sbio::cigi::ig::SImageGeneratorError
 * @see sbio::cigi::ig::SImageGeneratorErrorEventArgs
 * @see sbio::cigi::ig::IGCIGIEvent
 * @see sbio::cigi::ig::IGCIGIEventHandler
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_IMAGE_GENERATOR_H
#define SIMBLOCKS_CIGI_IMAGE_GENERATOR_H

#include "EngineLib/EngineTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiLib/IGCIGILibTypes.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "Poco/Net/DatagramSocket.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include "IGCigiLib/CigiEvent.h"
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Image generator runtime for one configured CIGI IG endpoint.
       *
       * Manages database state, host communication, packet processing, response generation, and
       * high-level IG operating mode.
       */
      class CCigiImageGenerator : public IIGCIGIEventListener
      {
      public:
        /**
         * @brief Constructs an image generator with setup options.
         * @param setupOptions Setup options for the image generator.
         *
         * The supplied options are copied and used later by `Initialize()` to construct the
         * version-specific packet handlers and senders.
         */
        CCigiImageGenerator(const sbio::cigi::ig::SIGSetupOptions& setupOptions);
        /**
         * @brief Destroys the image generator.
         */
        ~CCigiImageGenerator();

        /**
         * @brief Begins loading a database.
         * @param databaseNumber Database identifier.
         */
        virtual void BeginLoadingDatabase(CigiDatabaseNumber databaseNumber);
        /**
         * @brief Unloads the current database.
         */
        virtual void UnloadDatabase();
        /**
         * @brief Gets the current database state.
         * @return Database state.
         */
        EIGDatabaseState GetDatabaseState() const;
        /**
         * @brief Gets the current frame number.
         * @return Frame number.
         */
        FrameNumber GetFrameNumber() const;
        /**
         * @brief Gets the current operation mode.
         * @return Operation mode.
         */
        EIGMode GetOperationMode() const;
        /**
         * @brief Gets the synchronization mode.
         * @return Synchronization mode.
         */
        ECigiSynchronizationMode GetSynchronizationMode() const;
        /**
         * @brief Gets the last host frame number.
         * @return Last host frame number.
         */
        FrameNumber GetLastHostFrameNumber() const;
        /**
         * @brief Gets whether IG control was received.
         * @return True if received, false otherwise.
         */
        bool GetIgControlReceived() const;
        /**
         * @brief Gets whether IG control was received after play.
         * @return True if received, false otherwise.
         */
        bool GetIgControlReceivedAfterPlay() const;
        /**
         * @brief Gets the setup options for the image generator.
         * @return Copy of the setup options captured at construction.
         */
        sbio::cigi::ig::SIGSetupOptions GetSetupOptions() const;
        /**
         * @brief Gets the current time in seconds.
         * @return Time in seconds.
         */
        double GetTimeInSeconds() const;
        /**
         * @brief Gets the current timestamp.
         * @return Timestamp value.
         */
        uint32_t GetTimeStamp() const;
        /**
         * @brief Initializes the image generator.
         */
        virtual void Initialize();
        /**
         * @brief Gets whether the timestamp is valid.
         * @return True if valid, false otherwise.
         */
        bool IsTimeStampValid() const;
        /**
         * @brief Processes incoming packets.
         */
        virtual void ProcessPackets();

        /**
         * @brief Queries the current terrestrial surface conditions at a geodetic location.
         * @param query Geodetic query location.
         * @return Current terrestrial surface conditions at that location, one per unique surface-condition ID.
         */
        std::vector<SCigiTerrestrialSurfaceCondition> QueryTerrestrialSurfaceCondition(const sbio::math::SGeodeticCoordinates& query) const;

        /**
         * @brief Queues a database for loading.
         * @param databaseNumber Database identifier.
         */
        virtual void QueueLoadingDatabase(CigiDatabaseNumber databaseNumber);
        /**
         * @brief Sends outgoing packets.
         */
        virtual void SendPackets();
        /**
         * @brief Updates the image generator state.
         * @param fTimeStepSeconds Time step in seconds.
         */
        virtual void Update(double fTimeStepSeconds);
        /**
         * @brief Sets the operation mode.
         * @param eOperationMode Operation mode value.
         */
        void SetOperationMode(EIGMode eOperationMode);
        /**
         * @brief Sets interpolation enabled/disabled.
         * @param bInterpolationEnabled True to enable, false to disable.
         */
        void SetInteropolationEnabled(bool bInterpolationEnabled);
        /**
         * @brief Sets IG control received flag.
         * @param bReceived True if received, false otherwise.
         */
        void SetIgControlReceived(bool bReceived);
        /**
         * @brief Sets IG control received after play flag.
         * @param bReceived True if received, false otherwise.
         */
        void SetIgControlReceivedAfterPlay(bool bReceived);
        /**
         * @brief Sets the last host frame number.
         * @param lastHostFrameNumber Frame number value.
         */
        void SetLastHostFrameNumber(FrameNumber lastHostFrameNumber);
        /**
         * @brief Starts playing the image generator.
         */
        void StartPlaying();
        /**
         * @brief Stops playing the image generator.
         */
        void StopPlaying();

        // IIGCIGIEventListener overrides
        /**
         * @brief Called when database loaded event occurs.
         */
        void OnDatabaseLoadedEvent() override;
        /**
         * @brief Called when database loading failed event occurs.
         */
        void OnDatabaseLoadingFailedEvent() override;
        /**
         * @brief Called when entity removed event occurs.
         * @param entityID Removed entity ID.
         */
        virtual void OnEntityRemoved(sbio::EntityID entityID) override;
        /**
         * @brief Called when image generator error event occurs.
         * @param args Error event arguments.
         */
        void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) override;

        /**
         * @brief Gets the database loader.
         * @return Non-owning pointer to the owned database loader, or `nullptr` before initialization.
         */
        sbio::engine::CDatabaseLoader* GetDatabaseLoader() const;
        /**
         * @brief Gets the current database ID.
         * @return Database identifier.
         */
        CigiDatabaseNumber GetCurrentDatabaseID() const;
        /**
         * @brief Gets the earth reference model.
         * @return Earth reference model value.
         */
        sbio::cigi::EEarthReferenceModel GetEarthReferenceModel() const;
        /**
         * @brief Gets the list of errors.
         * @return Read-only reference to the accumulated image-generator errors.
         */
        const std::list<SImageGeneratorError>& GetErrors() const;
        /**
         * @brief Sets the current database number.
         * @param databaseNumber Database identifier.
         */
        void SetCurrentDatabaseNumber(CigiDatabaseNumber databaseNumber);
        /**
         * @brief Gets the packet senders.
         * @return Non-owning pointer to the owned packet-sender fan-out object, or `nullptr` before initialization.
         */
        sbio::cigi::ig::CCigiPacketSenders* GetPacketSenders() const;
        sbio::cigi::ig::CCigiMessageLogger* GetCigiMessageLogger() const;
        /**
         * @brief Gets the exported functions event dispatcher.
         * @return Non-owning pointer to the owned exported-response dispatcher.
         */
        sbio::cigi::ig::CIGResponseEventDispatcher* GetExportedFunctionsEventDispatcher() const;

      protected:
        /**
           * @brief Loads the CIGI to SISO entity enumeration conversion file.
           */
        void LoadCigiToSisoEntityEnumerationConversionFile();

      protected:
        sbio::cigi::ig::SIGSetupOptions m_SetupOptions;///< Session configuration supplied at construction.
        FrameNumber m_LastHostFrameNumber = UnknownFrameNumber;///< Most recent host frame number accepted from IG control.
        FrameNumber m_ImageGeneratorFrameNumber = UnknownFrameNumber;///< Current IG frame counter.
        EIGDatabaseState m_eDatabaseState = EIGDatabaseState::UNKNOWN;///< Current database lifecycle state.
        std::list<SImageGeneratorError> m_Errors;///< Accumulated image-generator errors observed during the session.
        sbio::cigi::EEarthReferenceModel m_eEarthReferenceModel = sbio::cigi::EEarthReferenceModel::UNKNOWN;///< Currently selected earth-reference model.
        bool m_bTimeStampValid = false;///< Indicates whether timestamp output is currently valid.
        bool m_bIgControlReceived = false;///< Indicates whether an IG control packet has been accepted for the current frame.
        bool m_bIGControlReceivedAfterPlay = false;///< Indicates whether IG control has been seen since playback started.
        CigiDatabaseNumber m_DatabaseNumber = UnknownCigiDatabaseNumber;///< Current or queued database identifier.
        EIGMode m_eOperationMode = EIGMode::UNKNOWN;///< Current IG operating mode.
        std::list<std::unique_ptr<sbio::cigi::ig::CCigiPacketHandler>> m_pPacketHandlers;///< Owned host-to-IG packet handlers for all configured hosts.
        std::unique_ptr<sbio::cigi::ig::CCigiPacketSenders> m_pPacketSenders;///< Owned fan-out sender for IG-to-host responses.
        std::unique_ptr<sbio::engine::CDatabaseLoader> m_pDatabaseLoader;///< Owned database loader for scene loading and unloading.
        std::unique_ptr<sbio::utils::CStopWatch> m_PlayTimer;///< Owned session timer used for timestamps and elapsed-time queries.
        std::unique_ptr<sbio::cigi::ig::CIGResponseEventDispatcher> m_pExportedFunctionsEventDispatcher;///< Owned dispatcher for exported IG response events.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
