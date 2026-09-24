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
#include <cstddef>
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
       * @brief Named callback scheduled against the active session's elapsed time.
       *
       * `Update()` invokes due callbacks in insertion order when the session time is strictly greater
       * than `fTime`. This stores an absolute deadline, not a relative delay.
       */
      struct SWaitCallback
      {
        float fTime = 0;///< Deadline in elapsed session seconds.
        std::string sCallback;///< Name of a no-argument script function to invoke.
      };

      /**
       * @brief Manages script execution and event handling for the CIGI host emulator.
       *
       * Supports callback registration, configuration loading, and event-driven script execution for simulation automation and extensibility.
       * Integrates with ChaiScript for scripting support and provides access to cloud types, component classes, layers, surface conditions, pixel replication modes, and font
       * management.
       *
       * Ownership:
       * - `CScriptRuntime` owns the embedded ChaiScript engine.
       * - Lookup accessors return values or references into containers owned by the runtime.
       *
       * Construction binds the process-wide script-runtime pointer and baseline engine state; runtimes
       * are not independent. Event callbacks run only while a script is active and the event's session
       * matches the host's selected session. Missing script handlers are ignored; script errors raise
       * host error events and mark execution stopped.
       *
       * Name lookups are case-sensitive. Cloud, component, layer, surface, and pixel-mode registration
       * appends list entries but preserves the first mapping for a duplicate name. Reverse lookups with
       * multiple names for one value return an unspecified matching name from an unordered map.
       */
      class CScriptRuntime : public sbio::cigi::host::IHostCigiEventListener
      {
      public:
        /**
         * @brief Creates a script runtime rooted at a scripts directory.
         * @param scriptsPath Directory containing script files and runtime configuration.
         *
         * Creates the engine and built-in bindings, captures baseline state, and registers this listener
         * with the global event dispatcher when available. Does not evaluate `configure.chai`.
         */
        CScriptRuntime(const std::filesystem::path& scriptsPath);
        /**
         * @brief Unregisters this listener from the current global dispatcher and destroys the engine.
         *
         * Clears the global runtime pointer if it still points here. Does not send an IG reset.
         */
        ~CScriptRuntime();

        /**
         * @brief Appends a callback to the deferred execution list.
         * @param sCallback Callback name and absolute deadline in active-session seconds; copied without validation.
         */
        void AddWaitCallback(const SWaitCallback& sCallback);

        /**
         * @brief Executes a script file within a named script category.
         * @param sCategory Subdirectory or logical category used to locate the script.
         * @param sFilename Script filename relative to the category.
         *
         * Restores baseline engine state, cancels pending callbacks, evaluates
         * `scriptsPath / sCategory / sFilename`, and invokes `Start()` if present. Marks execution active
         * until stopped or an error occurs, even after the file and `Start()` return. Caught evaluation
         * and standard exceptions stop execution, cancel callbacks, and raise error events.
         */
        void Execute(const std::string& sCategory, const std::string& sFilename);
        /**
         * @brief Resolves a cloud-type name or numeric text.
         * @param sCloudType Registered name, or text passed to `atoi` when unregistered.
         * @return Registered value, otherwise the `atoi` result converted to `uint8_t` and wrapped as `CloudType`.
         */
        sbio::CloudType GetCloudType(const std::string& sCloudType) const;
        /**
         * @brief Finds a registered name for a cloud type.
         * @param cloudType Value to look up.
         * @return A matching name, or an empty string if no name is registered.
         */
        std::string GetCloudTypeString(const sbio::CloudType& cloudType) const;
        /**
         * @brief Resolves a registered font by category and name.
         * @param sCategory Exact category name.
         * @param sFontName Exact font name within the category.
         * @return First matching registration's ID, or `UnknownFontID` if none exists.
         */
        FontID GetFontID(const std::string& sCategory, const std::string& sFontName);
        /**
         * @brief Lists registered font categories.
         * @return Copy of unique category names in first-registration order.
         */
        std::list<std::string> GetFontCategories() const;
        /**
         * @brief Lists fonts registered in a category.
         * @param sCategory Exact category name.
         * @return Copy of names in registration order, including duplicates; empty if the category is absent.
         */
        std::list<std::string> GetFontNames(const std::string& sCategory) const;
        /**
         * @brief Lists registered cloud-type names.
         * @return Runtime-owned list in registration order, including duplicates; valid for this runtime's lifetime.
         */
        const std::list<std::string>& GetCloudTypes() const;
        /**
         * @brief Lists registered component-class names.
         * @return Runtime-owned list in registration order, including duplicates; valid for this runtime's lifetime.
         */
        const std::list<std::string>& GetComponentClasses() const;
        /**
         * @brief Lists registered pixel-replication names.
         * @return Runtime-owned list in registration order, including duplicates; valid for this runtime's lifetime.
         */
        const std::list<std::string>& GetPixelReplicationModes() const;
        /**
         * @brief Lists registered surface-condition names.
         * @return Runtime-owned list in registration order, including duplicates; valid for this runtime's lifetime.
         */
        const std::list<std::string>& GetSurfaceConditions() const;
        /**
         * @brief Finds a registered name for the supplied component-class identifier.
         * @param componentClassID Component-class identifier (named in the out-of-line definition).
         * @return A matching name, or an empty string if no name is registered.
         */
        std::string GetComponentClassString(sbio::cigi::CigiComponentClassID) const;
        /**
         * @brief Resolves a component-class name or numeric text.
         * @param sComponentClass Registered name, or text passed to `atoi` when unregistered.
         * @return Registered ID, otherwise the `atoi` result converted to `uint8_t` and wrapped as an ID.
         */
        sbio::cigi::CigiComponentClassID GetComponentClassID(const std::string& sComponentClass) const;
        /**
         * @brief Looks up registered image-generator message text.
         * @param nMessageID Message identifier.
         * @return Stored message, or an empty string if the identifier is absent.
         */
        std::string GetImageGeneratorMessage(int nMessageID) const;
        /**
         * @brief Resolves a pixel-replication name or numeric text.
         * @param sPixelReplicationMode Registered name, or text passed to `atoi` when unregistered.
         * @return Registered mode, otherwise the `atoi` result converted to `uint8_t` and wrapped as a mode.
         */
        PixelReplicationMode GetPixelReplicationModeID(const std::string& sPixelReplicationMode) const;
        /**
         * @brief Finds a registered name for a pixel-replication mode.
         * @param cloudType Pixel-replication mode to look up; despite the parameter name, not a cloud type.
         * @return A matching name, or an empty string if no name is registered.
         */
        std::string GetPixelReplicationModeString(const PixelReplicationMode& cloudType) const;
        /**
         * @brief Resolves a surface-condition name or numeric text.
         * @param sSurfaceCondition Registered name, or text passed to `atoi` when unregistered.
         * @return Registered ID, otherwise the `atoi` result converted to `uint16_t` and wrapped as an ID.
         */
        sbio::SurfaceConditionID GetSurfaceConditionID(const std::string& sSurfaceCondition) const;
        /**
         * @brief Resolves a layer name or numeric text.
         * @param sLayerID Registered name, or text passed to `atoi` when unregistered.
         * @return Registered ID, otherwise `atoi(sLayerID.c_str())`; no numeric validation is performed.
         */
        int GetLayerID(const std::string& sLayerID) const;
        /**
         * @brief Finds a registered name for a layer identifier.
         * @param layerID Value to look up.
         * @return A matching name, or an empty string if no name is registered.
         */
        std::string GetLayerIDString(const int& layerID) const;
        /**
         * @brief Lists registered layer names.
         * @return Runtime-owned list in registration order, including duplicates; valid for this runtime's lifetime.
         */
        const std::list<std::string>& GetLayers() const;
        /**
         * @brief Reports whether the runtime is accepting script events and deferred callbacks.
         * @return Execution flag set by `Execute()` and cleared on stop/error; not a thread-running indicator.
         */
        bool IsScriptRunning() const;
        /**
         * @brief Evaluates `configure.chai` beneath the scripts directory.
         *
         * Caught evaluation and standard exceptions raise error events. Existing registrations and partial
         * script effects are not rolled back or cleared, so repeated calls can append duplicate list entries.
         */
        void LoadConfiguration();
        /**
         * @brief Invokes script `OnDatabaseLoaded()` when defined and the active-session filter passes.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnDatabaseLoaded(const HostCigiDatabaseLoadedEventArgs& args) override;
        /**
         * @brief Forwards the response to script `OnHATResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiHatResponseEvent(const HostCigiHatResponseEventArgs& args) override;
        /**
         * @brief Forwards the response to script `OnHOTResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiHotResponseEvent(const HostCigiHotResponseEventArgs& args) override;
        /**
         * @brief Forwards the response to script `OnHATHOTExtendedResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiHatHotExtendedResponseEvent(const HostCigiHatHotExtendedResponseEventArgs& args) override;
        /**
         * @brief Forwards the response to script `OnLineOfSightEntityResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightEntityResponseEvent(const HostCigiLineOfSightEntityResponseEventArgs& args) override;
        /**
         * @brief Forwards the response to script `OnLineOfSightResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightResponseEvent(const HostCigiLineOfSightResponseEventArgs& args) override;
        /**
         * @brief Invokes script `OnLineOfSightExtendedGeodeticCoordinatesResponse()` with converted response data.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Invokes script `OnLineOfSightExtendedEntityGeodeticCoordinatesResponse()` with converted data.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(
          const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Invokes script `OnLineOfSightExtendedEntityCoordinatesResponse()` with converted response data.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args) override;
        /**
         * @brief Forwards contact data to script `OnCollisionSegmentResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionSegmentNotificationEvent(const HostCigiCollisionDetectionSegmentNotificationEventArgs& args) override;
        /**
         * @brief Forwards contact data to script `OnCollisionSegmentEntityResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionSegmentEntityNotificationEvent(const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs& args) override;
        /**
         * @brief Forwards contact data to script `OnCollisionVolumeResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionVolumeNotificationEvent(const HostCigiCollisionDetectionVolumeNotificationEventArgs& args) override;
        /**
         * @brief Forwards contact data to script `OnCollisionVolumeEntityResponse()` when eligible and defined.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiCollisionDetectionVolumeEntityNotificationEvent(const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs& args) override;
        /**
         * @brief Converts position data and calls the matching geodetic, parent, or articulated script handler.
         * @param args Borrowed position response; concrete response type must match `ePositionResponseType`.
         */
        virtual void OnHostCigiPositionResponseEvent(const HostCigiPositionResponseEventArgs& args);
        /**
         * @brief Forwards queried weather conditions to the corresponding script callback when eligible.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiWeatherConditionsResponseEvent(const HostCigiWeatherConditionsResponseEventArgs& args) override;
        /**
         * @brief Forwards queried aerosol concentration to the corresponding script callback when eligible.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiAerosolConcentrationResponseEvent(const HostCigiAerosolConcentrationResponseEventArgs& args) override;
        /**
         * @brief Forwards queried maritime conditions to the corresponding script callback when eligible.
         * @param args Event data supplied with the dispatch.
         */
        virtual void OnHostCigiMaritimeSurfaceConditionsResponseEvent(const HostCigiMaritimeSurfaceConditionsResponseEventArgs& args) override;
        /**
         * @brief Forwards queried terrestrial conditions to the corresponding script callback when eligible.
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
         *
         * Appends a registration without deduplication; `GetFontID()` returns the first matching entry.
         */
        void RegisterFontID(sbio::FontID fontID, const std::string& sCategory, const std::string& sFontName);

        /**
         * @brief Registers an image-generator message identifier used by scripts.
         * @param nMessageID Message id numeric value.
         * @param sMessage Message text value.
         *
         * Replaces any message already registered for `nMessageID`.
         */
        void RegisterImageGeneratorMessage(int nMessageID, const std::string& sMessage);

        /**
         * @brief Cancels wait callbacks and requests IG reset on the active session, if one exists.
         *
         * Does not clear the script-running flag or reset the engine. The session may reject the request
         * when disconnected; this method does not check the `SetIGControl()` result.
         */
        void SendReset();

        /**
         * @brief Marks execution stopped, cancels callbacks, restores baseline engine state, and requests IG reset.
         *
         * Engine restoration errors are not caught here.
         */
        void Stop();

        /**
         * @brief Invokes callbacks whose deadlines precede the active session's current time.
         *
         * Does nothing unless execution is active. Due callbacks run in insertion order, not deadline order;
         * callbacks added while dispatching wait for a later update. Stop/reset/replacement cancels remaining
         * callbacks in the current batch. Missing functions and caught exceptions stop execution and raise errors.
         * The active session must have been initialized before its stopwatch is queried.
         */
        void Update();

      private:
        /**
         * @brief Tests whether an event belongs to the host's currently selected session.
         * @param args Event carrying a session identifier.
         * @return `true` for a matching active session; `false` if the host or session is absent or IDs differ.
         */
        bool IsActiveSessionEvent(const HostCigiEventArgs& args) const;
        /** @brief Clears queued waits and invalidates callback batches already detached by `Update()`. */
        void CancelWaitCallbacks();
        /**
         * @brief Stops execution, cancels callbacks, and raises a formatted script error event.
         * @param context Operation or callback name included in the diagnostic.
         * @param error Error details included in the diagnostic.
         */
        void HandleScriptError(const std::string& context, const std::string& error);

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
         * @brief One ordered registration associating a font identifier with an exact category/name pair.
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
        std::size_t m_WaitCallbackGeneration = 0;
      };
    }
  }
}
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
