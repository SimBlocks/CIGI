//Copyright SimBlocks LLC 2016-2026
#include "EngineLib/DatabaseLoader.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::engine;
using namespace sbio::ig::database;

extern SEngineLibGlobals g_EngineLibGlobals;

CDatabaseLoader::CDatabaseLoader()
{
}

CDatabaseLoader::~CDatabaseLoader()
{
}

void CDatabaseLoader::AddDatabaseProjection(sbio::DatabaseID databaseID, const sbio::SDatabaseProjection& databaseProjection)
{
  m_DatabaseProjections.insert(std::pair<sbio::DatabaseID, sbio::SDatabaseProjection>(databaseID, databaseProjection));
}

void sbio::engine::CDatabaseLoader::BeginLoadingDatabase(sbio::DatabaseID databaseID)
{
  SLoadDatabaseMessage data;
  data.DatabaseID = databaseID;

  if (g_EngineLibGlobals.pImageGeneratorEventMessenger != nullptr)
  {
    g_EngineLibGlobals.pImageGeneratorEventMessenger->SendLoadDatabaseMessage(data);
  }
}

void sbio::engine::CDatabaseLoader::UnloadCurrentDatabase()
{
  if (g_EngineLibGlobals.pImageGeneratorEventMessenger != nullptr)
  {
    g_EngineLibGlobals.pImageGeneratorEventMessenger->SendUnloadDatabaseMessage();
  }
}

void sbio::engine::CDatabaseLoader::AddDatabase(sbio::DatabaseID databaseID, const sbio::SDatabaseProjection& databaseProjection)
{
  m_DatabaseProjections.insert(std::pair<sbio::DatabaseID, sbio::SDatabaseProjection>(databaseID, databaseProjection));
}

sbio::SDatabaseProjection* sbio::engine::CDatabaseLoader::GetDatabaseProjection(sbio::DatabaseID databaseID)
{
  auto find = m_DatabaseProjections.find(databaseID);
  if (find != m_DatabaseProjections.end())
  {
    return &find->second;
  }
  return nullptr;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
