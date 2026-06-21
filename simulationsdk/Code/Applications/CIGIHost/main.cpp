//Copyright SimBlocks LLC 2016-2026
/**
 * @file main.cpp
 * @brief Entry point for the CIGIHost application.
 *
 * Initializes global resources, loads host options, and manages the main event loop for the CIGIHost application.
 * Handles user input for running scripts and resetting the host, and maintains the main update loop with frame rate control.
 * Integrates with the CIGI protocol host, event dispatcher, and logger for simulation interoperability.
 *
 * @see CCIGIHostOptions
 * @see sbio::cigi::host::CHost
 * @see sbio::cigi::host::CScriptRuntime
 * @see sbio::utils::CEventDispatcher
 * @see sbio::utils::CLogger
 */
#include "GlobalHeaders/Globals.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostSession.h"
#include "HostCigiLib/HostCigiLib.h"
#include "MathLib/MathTypes.h"
#include "Poco/Net/NetException.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/StopWatch.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "CIGIHostOptions.h"
#include "CigiLib/CigiLib.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#ifdef _WIN32
#include <conio.h>// Windows-specific header for _kbhit() and _getch()
#else
#include "UtilitiesLib/LinuxUtilities.h"
#endif

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

SGlobals g_globals;

CHost* g_pHost = nullptr;

const long SLEEP_MS = 17;

/**
 * @brief Main entry point for the CIGIHost application.
 *
 * Initializes global resources, loads host options, and manages the main event loop.
 * Handles user input for running scripts and resetting the host.
 */
int main()
{
  SGlobals globals;
  globals.applicationsDataPath = GetSdkPath() / "Data" / "Applications";
  globals.librariesDataPath = GetSdkPath() / "Data" / "Libraries";
  globals.thirdPartyPath = sbio::utils::GetEnvVariablePath("SBIO_THIRD_PARTY");
  globals.pEventDispatcher = make_shared<CEventDispatcher>();
  globals.pLogger = make_shared<CLogger>("Host");

  InitCigiLib();
  InitUtilitiesLib(globals);

  CCIGIHostOptions hostOptions;
  hostOptions.LoadOptions(globals.applicationsDataPath / "CIGI Host" / "CIGI Host.ini");

  g_pHost = new CHost();
  InitHostCigiLib(globals, g_pHost);

  g_pHost->Initialize(hostOptions.hostSetupOptions);

  CStopWatch stopWatch;

  cout << "\nCommands:" << endl;
  cout << "1 - Run Script" << endl;
  cout << "2 - Reset" << endl;
  cout << endl;

  try
  {
    while (true)
    {
      double fDeltaTime = stopWatch.GetElapsedSeconds();

      stopWatch.Reset();
      stopWatch.Start();

      g_pHost->Update(fDeltaTime);

      if (_kbhit())
      {
        char c = static_cast<char>(_getch());
        if (c == '1')
        {
          cout << "\nRunning Script!" << endl;
          g_pHost->GetScriptRuntime()->Execute(hostOptions.hostSetupOptions.sScriptCategory, hostOptions.hostSetupOptions.sScript);
        }
        else if (c == '2')
        {
          cout << "\nReset!" << endl;
          g_pHost->GetScriptRuntime()->SendReset();
        }
      }

      double fDuration = stopWatch.GetElapsedMilliSeconds();

      //maintain frame rate
      if (fDuration < SLEEP_MS)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MS));
      }
    }
  }
  catch (const Poco::Net::NetException& e)
  {
    std::cerr << e.displayText() << std::endl;
  }

  return 0;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
