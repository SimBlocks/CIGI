//Copyright SimBlocks LLC 2016-2026
#include "EntityLib.h"
#include "EntityEnumerations.h"
#include "EntityManager.h"
#include "GlobalHeaders/Globals.h"
#include "tinyxml2.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::entity;
using namespace tinyxml2;

SEntityLibSettings g_EntityLibSettings;

void InitEntityLib(const SGlobals& globals, const sbio::entity::SEntityLibParams& params)
{
  g_EntityLibSettings.pEntityManager = params.pEntityManager.get();
  g_EntityLibSettings.pEventDispatcher = globals.pEventDispatcher.get();
  g_EntityLibSettings.pLogger = globals.pLogger.get();
  g_EntityLibSettings.dataPath = globals.librariesDataPath / "EntityLib";

  if (g_EntityLibSettings.pEntityManager != nullptr)
  {
    g_EntityLibSettings.pEntityManager->Init();
  }
}

void UninitEntityLib()
{
  g_EntityLibSettings.pEntityManager = nullptr;
  g_EntityLibSettings.pEventDispatcher = nullptr;
  g_EntityLibSettings.pLogger = nullptr;
}

EClamp ToClamp(const std::string& sClamp)
{
  if (sClamp == "NONE" || sClamp == "No Clamp")
  {
    return EClamp::NONE;
  }
  else if (sClamp == "CONFORMAL" || sClamp == "Conformal")
  {
    return EClamp::CONFORMAL;
  }
  else if (sClamp == "NON_CONFORMAL" || sClamp == "Non-Conformal")
  {
    return EClamp::NON_CONFORMAL;
  }

  return EClamp::UNKNOWN;
}

sbio::EObjectCoordinateSystem ToEntityCoordinateSystem(const std::string& sCoordinateSystem)
{
  if (sCoordinateSystem == "WORLD")
  {
    return EObjectCoordinateSystem::WORLD;
  }
  else if (sCoordinateSystem == "PARENT")
  {
    return EObjectCoordinateSystem::PARENT;
  }
  else if (sCoordinateSystem == "LOCAL")
  {
    return EObjectCoordinateSystem::LOCAL;
  }

  return EObjectCoordinateSystem::UNKNOWN;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
