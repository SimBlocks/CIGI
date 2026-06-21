//Copyright SimBlocks LLC 2016-2026
/**
 * @file ScriptRuntime.h
 * @brief Declares the CScriptRuntime class for script execution and event handling in the CIGI host emulator.
 *
 * Provides the CScriptRuntime class for managing and executing scripts, handling CIGI protocol events, and integrating
 * with the host emulator. Supports callback registration, configuration loading, and event-driven script execution for
 * simulation automation and extensibility. Integrates with ChaiScript for scripting support and provides access to
 * cloud types, component classes, layers, surface conditions, pixel replication modes, and font management.
 *
 * @see sbio::cigi::host::CScriptRuntime
 * @see sbio::cigi::host::IHostCigiEventListener
 * @see HostCigiEventArgs
 * @see chaiscript::ChaiScript
 */
#pragma once
#ifndef SIMBLOCKS_SCRIPT_RUNTIME_H
#define SIMBLOCKS_SCRIPT_RUNTIME_H

#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/HostCigiEvent.h"
#include <string>
#include <list>
#include <unordered_map>
#include <filesystem>
#include <memory>

namespace chaiscript
{
  class ChaiScript;
}

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      /**
       * @brief Wait callback structure for script execution timing.
       *
       * Instances are queued by the script runtime and executed after their associated delay
       * has elapsed during subsequent `Update()` calls.
       */
      struct SWaitCallback
      {
        float fTime = 0;///< Time to wait before callback
        std::string sCallback;///< Callback function name
      };

      /**
       * @brief Manages script execution and event handling for the CIGI host emulator.
       *
       * Supports callback registration, configuration loading, and event-driven script execution for simulation automation and extensibility.
       * Integrates with ChaiScript for scripting support and provides access to cloud types, component classes, layers, surface conditions, pixel replication modes, and font management.
       *
       * Ownership:
       * - `CScriptRuntime` owns the embedded ChaiScript engine.
       * - Lookup accessors return values or references into containers owned by the runtime.
       */
      class CScriptRuntime : public sbio::cigi::host::IHostCigiEventListener
      {
      public:
        /**
         * @brief Creates a script runtime rooted at a scripts directory.
         * @param scriptsPath Directory containing script files and runtime configuration.
         */
        CScriptRuntime(const std::filesystem::path& scriptsPath);
        /**
         * @brief Destroys CScriptRuntime instances.
         */
        ~CScriptRuntime();

        /**
         * @brief Schedules a callback to run after a delay managed by `Update()`.
          * @param sCallback Callback text value.
         */
        void AddWaitCallback(const SWaitCallback& sCallback);

        /**
         * @brief Executes a script file within a named script category.
         * @param sCategory Subdirectory or logical category used to locate the script.
         * @param sFilename Script filename relative to the category.
         */
        void Execute(const std::string& sCategory, const std::string& sFilename);
        /**
         * @brief Gets cloud type.
         * @param sCloudType Cloud type text value.
         * @return Cloud type value.
         */
        sbio::CloudType GetCloudType(const std::string& sCloudType) const;
        /**
         * @brief Gets cloud type string.
         * @param cloudType Cloud type value.
         * @return Cloud type string value.
         */
        std::string GetCloudTypeString(const sbio::CloudType& cloudType) const;
        /**
         * @brief Gets font id.
         * @param sCategory Category text value.
         * @param sFontName Font name text value.
         * @return Font id value.
         */
        FontID GetFontID(const std::string& sCategory, const std::string& sFontName);
        /**
         * @brief Gets font categories.
         * @return Font categories value.
         */
        std::list<std::string> GetFontCategories() const;
        /**
         * @brief Gets font names.
         * @param sCategory Category text value.
         * @return Font names value.
         */
        std::list<std::string> GetFontNames(const std::string& sCategory) const;
        /**
         * @brief Gets cloud types.
         * @return Cloud types value.
         */
        const std::list<std::string>& GetCloudTypes() const;
        /**
         * @brief Gets component classes.
         * @return Component classes value.
         */
        const std::list<std::string>& GetComponentClasses() const;
        /**
         * @brief Gets pixel replication modes.
         * @return Pixel replication modes value.
         */
        const std::list<std::string>& GetPixelReplicationModes() const;
        /**
         * @brief Gets surface conditions.
         * @return Surface conditions value.
         */
        const std::list<std::string>& GetSurfaceConditions() const;
        /**
         * @brief Gets component class string.
         * @return Component class string value.
         */
        std::string GetComponentClassString(sbio::cigi::CigiComponentClassID) const;
        /**
         * @brief Gets component class id.
         * @param sComponentClass Component class text value.
         * @return Component class id value.
         */
        sbio::cigi::CigiComponentClassID GetComponentClassID(const std::string& sComponentClass) const;
        /**
         * @brief Gets image generator message.
         * @param nMessageID Message id numeric value.
         * @return Image generator message value.
         */
        std::string GetImageGeneratorMessage(int nMessageID) const;
        /**
         * @brief Gets pixel replication mode id.
         * @param sPixelReplicationMode Pixel replication mode text value.
         * @return Pixel replication mode id value.
         */
        PixelReplicationMode GetPixelReplicationModeID(const std::string& sPixelReplicationMode) const;
        /**
         * @brief Gets pixel replication mode string.
         * @param cloudType Cloud type value.
         * @return Pixel replication mode string value.
         */
        std::string GetPixelReplicationModeString(const PixelReplicationMode& cloudType) const;
        /**
         * @brief Gets surface condition id.
         * @param sSurfaceCondition Surface condition text value.
         * @return Surface condition id value.
         */
        sbio::SurfaceConditionID GetSurfaceConditionID(const std::string& sSurfaceCondition) const;
        /**
         * @brief Gets layer id.
         * @param sLayerID Layer id text value.
         * @return Layer id value.
         */
        int GetLayerID(const std::string& sLayerID) const;
        /**
         * @brief Gets layer idstring.
         * @param layerID Layer id value.
         * @return Layer idstring value.
         */
        std::string GetLayerIDString(const int& layerID) const;
        /**
         * @brief Gets layers.
         * @return Layers value.
         */
        const std::list<std::string>& GetLayers() const;
        /**
         * @brief Determines whether script running.
         * @return `true` when the condition is met; otherwise `false`.
         */
        bool IsScriptRunning() const;
        /**
         * @brief Loads runtime lookup tables and script configuration from disk.
         */
        void LoadConfiguration();
        /**
         * @brief Handles the database loaded event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnDatabaseLoaded(const HostCigiDatabaseLoadedEventArgs& args) override;
        /**
         * @brief Handles the host cigi hat response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiHatResponseEvent(const HostCigiHatResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi hot response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiHotResponseEvent(const HostCigiHotResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi hat hot extended response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiHatHotExtendedResponseEvent(const HostCigiHatHotExtendedResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi line of sight entity response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightEntityResponseEvent(const HostCigiLineOfSightEntityResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi line of sight response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightResponseEvent(const HostCigiLineOfSightResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi line of sight extended geodetic coordinates response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi line of sight extended entity geodetic coordinates response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi line of sight extended entity coordinates response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi collision detection segment notification event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionSegmentNotificationEvent(const HostCigiCollisionDetectionSegmentNotificationEventArgs& args) override;
        /**
         * @brief Handles the host cigi collision detection segment entity notification event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionSegmentEntityNotificationEvent(const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs& args) override;
        /**
         * @brief Handles the host cigi collision detection volume notification event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionVolumeNotificationEvent(const HostCigiCollisionDetectionVolumeNotificationEventArgs& args) override;
        /**
         * @brief Handles the host cigi collision detection volume entity notification event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionVolumeEntityNotificationEvent(const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs& args) override;
        /**
         * @brief Handles the host cigi position response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiPositionResponseEvent(const HostCigiPositionResponseEventArgs& args);
        /**
         * @brief Handles the host cigi weather conditions response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiWeatherConditionsResponseEvent(const HostCigiWeatherConditionsResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi aerosol concentration response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiAerosolConcentrationResponseEvent(const HostCigiAerosolConcentrationResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi maritime surface conditions response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiMaritimeSurfaceConditionsResponseEvent(const HostCigiMaritimeSurfaceConditionsResponseEventArgs& args) override;
        /**
         * @brief Handles the host cigi terrestrial surface conditions response event event.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiTerrestrialSurfaceConditionsResponseEvent(const HostCigiTerrestrialSurfaceConditionsResponseEventArgs& args) override;
        /**
         * @brief Registers a textual cloud type for use by scripts and UI helpers.
          * @param eCloudType Cloud type enumeration value.
          * @param sCloudType Cloud type text value.
         */
        void RegisterCloudType(sbio::CloudType eCloudType, const std::string& sCloudType);

        /**
         * @brief Registers a textual component class mapping.
          * @param componentClassID Component class id value.
          * @param sComponentClass Component class text value.
         */
        void RegisterComponentClass(sbio::cigi::CigiComponentClassID componentClassID, const std::string& sComponentClass);

        /**
         * @brief Registers a textual layer identifier mapping.
          * @param nLayerID Layer id numeric value.
          * @param sLayerID Layer id text value.
         */
        void RegisterLayerID(int nLayerID, const std::string& sLayerID);

        /**
         * @brief Registers a textual surface-condition mapping.
          * @param surfaceConditionID Surface condition id value.
          * @param sSurfaceConditionName Surface condition name text value.
         */
        void RegisterSurfaceConditionID(sbio::SurfaceConditionID surfaceConditionID, const std::string& sSurfaceConditionName);

        /**
         * @brief Registers a textual pixel-replication-mode mapping.
          * @param pixelReplicationModeID Pixel replication mode id value.
          * @param sPixelReplicationModeName Pixel replication mode name text value.
         */
        void RegisterPixelReplicationMode(PixelReplicationMode pixelReplicationModeID, const std::string& sPixelReplicationModeName);

        /**
         * @brief Registers a font name within a script-visible category.
          * @param fontID Font id value.
          * @param sCategory Category text value.
          * @param sFontName Font name text value.
         */
        void RegisterFontID(sbio::FontID fontID, const std::string& sCategory, const std::string& sFontName);

        /**
         * @brief Registers an image-generator message identifier used by scripts.
          * @param nMessageID Message id numeric value.
          * @param sMessage Message text value.
         */
        void RegisterImageGeneratorMessage(int nMessageID, const std::string& sMessage);

        /**
         * @brief Requests that the currently executing script sequence stop and reset.
         */
        void SendReset();

        /**
         * @brief Stops active script execution.
         */
        void Stop();

        /**
         * @brief Advances timers and executes deferred callbacks.
         */
        void Update();

      private:
        std::unique_ptr<chaiscript::ChaiScript> m_pChaiScript;///< Owned ChaiScript engine
        bool m_bIsExecutingScript = false;///< Script execution state
        std::filesystem::path m_scriptsPath;///< Path to scripts directory

        std::list<std::string> m_CloudTypes;///< Registered cloud types
        std::unordered_map<std::string, sbio::CloudType> m_CloudTypeValues;///< Cloud type lookup

        std::list<std::string> m_ComponentClasses;///< Registered component classes
        std::unordered_map<std::string, sbio::cigi::CigiComponentClassID> m_ComponentClassValues;///< Component class lookup

        std::list<std::string> m_Layers;///< Registered layers
        std::unordered_map<std::string, int> m_LayerValues;///< Layer lookup

        std::list<std::string> m_SurfaceConditions;///< Registered surface conditions
        std::unordered_map<std::string, sbio::SurfaceConditionID> m_SurfaceConditionValues;///< Surface condition lookup

        std::list<std::string> m_PixelReplicationModes;///< Registered pixel replication modes
        std::unordered_map<std::string, PixelReplicationMode> m_PixelReplicationModeValues;///< Pixel replication mode lookup

        /**
         * @brief Stores font category id data.
         */
        struct SFontCategoryID
        {
          sbio::FontID fontID = UnknownFontID;///< Font ID
          std::string sCategory;///< Font category
          std::string sFontName;///< Font name
        };

        std::list<SFontCategoryID> m_Fonts;///< Registered fonts
        std::unordered_map<int, std::string> m_ImageGeneratorMessages;///< Image generator messages

        std::list<SWaitCallback> m_WaitCallbacks;///< Wait callbacks for script timing
      };
    }
  }
}
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
