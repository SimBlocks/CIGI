//Copyright SimBlocks LLC 2016-2026
/**
 * @file main.cpp
 * @brief Entry point and main loop for the CIGI test image generator application.
 *
 * Implements the main function, initialization, and main loop for the CIGI test image generator.
 * Sets up global managers, loads configuration, and runs the image generator simulation loop.
 * Integrates with SimBlocks entity, view, symbol, and CIGI libraries for simulation management.
 *
 * @see STestImageGeneratorGlobals
 * @see CCigiImageGenerator
 * @see CTestCigiEventHandler
 * @see CCIGITestImageGeneratorOptions
 */
#include "EngineLib/DatabaseLoader.h"
#include "SymbolLib/SymbolSurfaceManager.h"
#include "ViewLib/ViewManager.h"
#include "EntityLib/EntityLib.h"
#include "EntityLib/EntityManager.h"
#include "ViewLib/ViewLib.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/CigiView.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/CigiSymbolGeometryFactory.h"
#include "TestCigiEventHandler.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/StopWatch.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "ViewLib/IViewCreator.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EngineLib/EngineLib.h"
#include "SymbolLib/Symbol.h"
#include "SymbolLib/SymbolLib.h"
#include "IGCigiLib/IGCIGILibTypes.h"
#include "IGCigiLib/ImageGenerator.h"
#include "IGCigiLib/CigiViewGroup.h"
#include "IGCigiLib/PacketSender.h"
#include "CIGITestImageGeneratorOptions.h"
#include "main.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef _WIN32
#include <conio.h>// Windows-specific header for _kbhit() and _getch()
#else
#include "UtilitiesLib/LinuxUtilities.h"
#endif

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::entity;
using namespace sbio::symbol;
using namespace sbio::engine;
using namespace sbio::view;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig;

STestImageGeneratorGlobals globals;

std::unique_ptr<CImageGeneratorEventMessenger> g_ImageGeneratorEventMessenger;
unique_ptr<CTestCigiEventHandler> g_pEventHandler;
std::unique_ptr<CCigiImageGenerator> g_pImageGenerator;

void CreateSingleView(ViewID viewID)
{
  unique_ptr<CView> pCigiView = make_unique<CCigiView>(viewID);
  pCigiView->SetNearPlane(.1f);
  pCigiView->SetFarPlane(1000);
  pCigiView->SetLeftHalfAngle(-45);
  pCigiView->SetRightHalfAngle(45);
  pCigiView->SetTopHalfAngle(30);
  pCigiView->SetBottomHalfAngle(30);
  globals.pViewManager->AddView(std::move(pCigiView));
}

void CreateViewGroup()
{
  unique_ptr<CView> pCigiView0 = make_unique<CCigiView>(ViewID(0));
  pCigiView0->SetNearPlane(.1f);
  pCigiView0->SetFarPlane(1000);
  pCigiView0->SetLeftHalfAngle(-45);
  pCigiView0->SetRightHalfAngle(45);
  pCigiView0->SetTopHalfAngle(30);
  pCigiView0->SetBottomHalfAngle(30);
  globals.pViewManager->AddView(std::move(pCigiView0));

  unique_ptr<CView> pCigiView1 = make_unique<CCigiView>(ViewID(1));
  pCigiView1->SetNearPlane(.1f);
  pCigiView1->SetFarPlane(1000);
  pCigiView1->SetLeftHalfAngle(-45);
  pCigiView1->SetRightHalfAngle(45);
  pCigiView1->SetTopHalfAngle(30);
  pCigiView1->SetBottomHalfAngle(30);
  globals.pViewManager->AddView(std::move(pCigiView1));

  unique_ptr<CView> pCigiView2 = make_unique<CCigiView>(ViewID(2));
  pCigiView2->SetNearPlane(.1f);
  pCigiView2->SetFarPlane(1000);
  pCigiView2->SetLeftHalfAngle(-45);
  pCigiView2->SetRightHalfAngle(45);
  pCigiView2->SetTopHalfAngle(30);
  pCigiView2->SetBottomHalfAngle(30);
  globals.pViewManager->AddView(std::move(pCigiView2));

  std::unique_ptr<CCigiViewGroup> pViewGroup = std::make_unique<CCigiViewGroup>(ViewGroupID(1));
  pViewGroup->AddViewID(ViewID(0));
  pViewGroup->AddViewID(ViewID(1));
  pViewGroup->AddViewID(ViewID(2));
  pViewGroup->SetCenterViewID(ViewID(1));

  globals.pViewManager->AddViewGroup(std::move(pViewGroup));
}

bool RunIG()
{
  CStopWatch stopWatch;

  for (int n = 0; n < 9; ++n)
  {
    CreateSingleView(ViewID(static_cast<uint16_t>(n)));
  }

  CCIGITestImageGeneratorOptions options;
  const std::filesystem::path optionsPath = GetSdkPath() / "Data" / "Applications" / "CIGI ImageGenerator" / "CIGITestImageGenerator.ini";
  if (!options.LoadOptions(optionsPath))
  {
    cout << "Failed to load options file: " << optionsPath << endl;
    return false;
  }

  SIGSetupOptions setupOptions = options.GetOptions();

  g_pImageGenerator = std::make_unique<CCigiImageGenerator>(setupOptions);
  if (g_pImageGenerator->GetCigiMessageLogger() != nullptr)
  {
    cout << "Logging CIGI messages to: " << g_pImageGenerator->GetCigiMessageLogger()->GetLogFilePath() << endl;
  }

  globals.pImageGenerator = g_pImageGenerator.get();
  SetIGCigiLibImageGenerator(g_pImageGenerator.get());

  g_pImageGenerator->Initialize();

  globals.pLogger->LogInformation("IG Initialized");

  if (setupOptions.eCigiVersion == ECigiVersion::VERSION_3_3)
  {
    cout << "CIGI Version 3.3" << endl;
  }
  else if (setupOptions.eCigiVersion == ECigiVersion::VERSION_4_0)
  {
    cout << "CIGI Version 4.0" << endl;
  }
  else
  {
    cout << "Unknown CIGI Version" << endl;
  }

  cout << "\nCommands:" << endl;
  cout << "1 - Play" << endl;
  cout << "2 - Stop" << endl;
  cout << endl;

  bool bPlaying = false;

  while (true)
  {
    if (_kbhit())
    {
      char c = static_cast<char>(_getch());
      if (c == '1')
      {
        bPlaying = true;
        cout << "\nPlaying!" << endl;
        g_pImageGenerator->StartPlaying();
      }
      else if (c == '2')
      {
        bPlaying = false;
        cout << "\nStopped!" << endl;
        g_pImageGenerator->StopPlaying();
      }
    }

    if (bPlaying)
    {
      if (g_pImageGenerator->GetTimeInSeconds() > 0)
      {
        /*stringstream ss;
        ss << "\nTime: " << g_pImageGenerator->GetTimeInSeconds();
        globals.pLogger->LogDebug(ss.str());*/
      }

      g_pImageGenerator->Update(.015);
      g_pEventHandler->Update();
    }

    Sleep(15);
  }
}

int main()
{
  std::filesystem::path dir = GetCurrentWorkingDirectory();
  cout << "Running ImageGenerator.exe" << endl;
  cout << "Current directory: " << dir << endl;

  globals.applicationsDataPath = GetSdkPath() / "Data" / "Applications";
  globals.currentApplicationDataPath = globals.applicationsDataPath / "CIGI ImageGenerator";
  globals.librariesDataPath = GetSdkPath() / "Data" / "Libraries";
  globals.thirdPartyPath = sbio::utils::GetEnvVariablePath("SBIO_THIRD_PARTY");
  cout << "Applications Data Path: " << globals.applicationsDataPath << endl;
  cout << "Current Application Data Path: " << globals.currentApplicationDataPath << endl;
  cout << "Libraries Data Path: " << globals.librariesDataPath << endl;

  globals.pEventDispatcher = make_shared<CEventDispatcher>();
  globals.pEntityManager = make_shared<CEntityManager>();
  globals.pViewManager = make_shared<CViewManager>();
  globals.pSymbolSurfaceManager = make_shared<CSymbolSurfaceManager>();
  globals.pLogger = make_shared<CLogger>("Test ImageGenerator", true, true, ELogLevel::LOG_DEBUG);

  SEntityLibParams entityLibParams;
  entityLibParams.pEntityManager = globals.pEntityManager;
  InitEntityLib(globals, entityLibParams);
  InitUtilitiesLib(globals);

  g_pEventHandler = std::make_unique<CTestCigiEventHandler>();
  g_ImageGeneratorEventMessenger = std::make_unique<CImageGeneratorEventMessenger>(g_pEventHandler.get());

  SIGCigiLibParams cigiParams;
  cigiParams.pEventMessenger = g_ImageGeneratorEventMessenger.get();
  cigiParams.pEntityManager = globals.pEntityManager;
  cigiParams.pSymbolSurfaceManager = globals.pSymbolSurfaceManager;
  cigiParams.pViewManager = globals.pViewManager;
  InitIGCigiLib(globals, cigiParams);

  SViewLibParams viewLibParams;
  viewLibParams.pViewManager = globals.pViewManager;
  InitViewLib(globals, viewLibParams);

  SEngineLibParams engineLibParams;
  engineLibParams.pEventMessenger = g_ImageGeneratorEventMessenger.get();
  InitEngineLib(globals, engineLibParams);

  std::unique_ptr<CSymbolGeometryFactory> pSymbolGeometryFactory = std::make_unique<CCigiSymbolGeometryFactory>();
  InitSymbolLib(globals, std::move(pSymbolGeometryFactory));

  std::this_thread::sleep_for(std::chrono::seconds(1));

  if (!RunIG())
  {
    return 1;
  }

  return 0;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
