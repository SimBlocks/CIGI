//Copyright SimBlocks LLC 2016-2026
#include "ViewLib.h"
#include "ViewManager.h"
#include "IViewCreator.h"
#include "GlobalHeaders/Globals.h"
#include "ViewGroup.h"

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::view;

SViewLibSettings g_ViewLibSettings;

void InitViewLib(const SGlobals& globals, const SViewLibParams& params)
{
  g_ViewLibSettings.pViewManager = params.pViewManager.get();
  g_ViewLibSettings.pEventDispatcher = globals.pEventDispatcher.get();
  g_ViewLibSettings.pLogger = globals.pLogger.get();
  g_ViewLibSettings.dataPath = globals.librariesDataPath / "ViewLib";
}

void UninitViewLib()
{
  g_ViewLibSettings.pViewManager = nullptr;
  g_ViewLibSettings.pEventDispatcher = nullptr;
  g_ViewLibSettings.pLogger = nullptr;
}

sbio::EProjectionMode ToProjectionMode(const std::string& sProjectionMode)
{
  if (sProjectionMode == "PERSPECTIVE")
  {
    return EProjectionMode::PERSPECTIVE;
  }
  else if (sProjectionMode == "ORTHOGRAPHIC")
  {
    return EProjectionMode::ORTHOGRAPHIC;
  }

  return EProjectionMode::UNKNOWN;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
