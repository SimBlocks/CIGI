//Copyright SimBlocks LLC 2016-2026
#pragma once

#ifndef SIMBLOCKS_ENGINE_DATABASE_LOADER_H
#define SIMBLOCKS_ENGINE_DATABASE_LOADER_H

#include "GlobalHeaders/CommonTypes.h"
#include <unordered_map>

namespace sbio
{
  namespace engine
  {
    /**
     * @brief Manages the logic for initiating and tracking the loading and unloading of databases in the engine.
     *
     * The CDatabaseLoader class provides methods to request the loading or unloading of specific databases by their unique IDs.
     * While the actual database loading is performed by the engine, this class maintains the state and mapping of database IDs
     * to their associated projection data, and provides interfaces to add, retrieve, and manage database projections.
     *
     * Responsibilities:
     * - Cache projection metadata for known databases.
     * - Forward load and unload requests through `g_EngineLibGlobals.pImageGeneratorEventMessenger`.
     *
     * Ownership:
     * - Owns the projection records stored in `m_DatabaseProjections`.
     * - Does not own the global event messenger consulted when sending requests.
     */
    class CDatabaseLoader
    {
    public:
      /**
       * @brief Constructs a new CDatabaseLoader instance.
       */
      CDatabaseLoader();
      /**
       * @brief Destroys the CDatabaseLoader instance.
       */
      virtual ~CDatabaseLoader();

      /**
       * @brief Adds a projection for a specific database.
       * @param databaseID The unique identifier for the database.
       * @param databaseProjection The projection information to associate with the database.
       *
       * Side effects:
       * - Inserts a new mapping when `databaseID` is not already present.
       */
      void AddDatabaseProjection(sbio::DatabaseID databaseID, const sbio::SDatabaseProjection& databaseProjection);
      /**
       * @brief Initiates the loading process for the specified database. The actual loading is performed by the engine.
       * @param databaseID The unique identifier for the database to load.
       *
       * Side effects:
       * - Sends `SLoadDatabaseMessage` through the configured image-generator event messenger when one is available.
       *
       * Failure cases:
       * - If no event messenger is configured, the request is ignored.
       */
      void BeginLoadingDatabase(sbio::DatabaseID databaseID);
      /**
       * @brief Initiates the unloading process for the currently loaded database. The actual unloading is performed by the engine.
       *
       * Side effects:
       * - Sends the unload-database message through the configured image-generator event messenger when one is available.
       *
       * Failure cases:
       * - If no event messenger is configured, the request is ignored.
       */
      void UnloadCurrentDatabase();
      /**
       * @brief Adds a database and its projection information.
       * @param databaseID The unique identifier for the database.
       * @param databaseProjection The projection information to associate with the database.
       *
       * This is currently equivalent to `AddDatabaseProjection()`.
       */
      void AddDatabase(sbio::DatabaseID databaseID, const sbio::SDatabaseProjection& databaseProjection);
      /**
       * @brief Retrieves the projection information for a specific database.
       * @param databaseID The unique identifier for the database.
       * @return Pointer to the SDatabaseProjection if found, nullptr otherwise.
       *
       * Ownership:
       * - Returns a non-owning pointer into `m_DatabaseProjections`.
       */
      sbio::SDatabaseProjection* GetDatabaseProjection(sbio::DatabaseID databaseID);

    private:
      /**
       * @brief Internal map of database IDs to their projection information.
       */
      std::unordered_map<sbio::DatabaseID, sbio::SDatabaseProjection, StrongTypeHash<sbio::DatabaseID>> m_DatabaseProjections;
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
