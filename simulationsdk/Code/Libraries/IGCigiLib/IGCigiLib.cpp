//Copyright SimBlocks LLC 2016-2026
#include "IGCigiLib.h"
#include "CigiLib/CigiLib.h"
#include "GlobalHeaders/Globals.h"
#include "CigiEvent.h"
#include "CigiMessageLogger.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "CigiLib/CigiEntityTypes.h"
#include "MathLib/Projection.h"

using namespace std;
using namespace sbio;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;

SIGCigiLibGlobals g_CigiLibGlobals;

SIGCigiLibGlobals* InitIGCigiLib(const SGlobals& globals, const SIGCigiLibParams& params)
{
  g_CigiLibGlobals.pEventDispatcher = globals.pEventDispatcher.get();
  g_CigiLibGlobals.pEntityManager = params.pEntityManager.get();
  g_CigiLibGlobals.pViewManager = params.pViewManager.get();
  g_CigiLibGlobals.pSymbolSurfaceManager = params.pSymbolSurfaceManager.get();
  g_CigiLibGlobals.pLogger = globals.pLogger.get();
  g_CigiLibGlobals.dataPath = globals.librariesDataPath / "CigiLib";
  g_CigiLibGlobals.applicationDataPath = globals.currentApplicationDataPath;
  g_CigiLibGlobals.pEventMessenger = params.pEventMessenger;

  if (g_CigiLibGlobals.pLogger != nullptr)
  {
    if (g_CigiLibGlobals.pEventMessenger == nullptr)
    {
      g_CigiLibGlobals.pLogger->LogWarning("IGCigiLib initialized without an event messenger.");
    }

    if (g_CigiLibGlobals.pEntityManager == nullptr)
    {
      g_CigiLibGlobals.pLogger->LogWarning("IGCigiLib initialized without an entity manager.");
    }
  }

  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->RegisterEvent<IGCIGIEvent>(make_unique<IGCIGIEventHandler>());
  }
  else if (g_CigiLibGlobals.pLogger != nullptr)
  {
    g_CigiLibGlobals.pLogger->LogError("IGCigiLib initialization requires an event dispatcher to register IGCIGIEvent.");
  }

  SetupCigiOpCodeNames();

  return &g_CigiLibGlobals;
}

void SetIGCigiLibImageGenerator(sbio::cigi::ig::CCigiImageGenerator* pImageGenerator)
{
  g_CigiLibGlobals.pImageGenerator = pImageGenerator;
}

void UninitIGCigiLib()
{
  g_CigiLibGlobals.pProjection.reset();
  g_CigiLibGlobals.pCigiMessageLogger.reset();
  g_CigiLibGlobals.pCigiEntityTypes.reset();
  g_CigiLibGlobals.pImageGenerator = nullptr;
  g_CigiLibGlobals.pEventDispatcher = nullptr;
  g_CigiLibGlobals.pEntityManager = nullptr;
  g_CigiLibGlobals.pViewManager = nullptr;
  g_CigiLibGlobals.pSymbolSurfaceManager = nullptr;
  g_CigiLibGlobals.pLogger = nullptr;
  g_CigiLibGlobals.pEventMessenger = nullptr;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
