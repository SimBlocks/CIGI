//Copyright SimBlocks LLC 2016-2026
#include "HostEmulatorOptions.h"
#include "CigiLib/CigiTypes.h"
#include "GlobalHeaders/Globals.h"
#include "HostEmulatorDefinitions.h"
#include "Poco/Util/IniFileConfiguration.h"
#include <memory>

using namespace std;
using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::host;
using namespace Poco;
using namespace Poco::Util;

extern SHostEmulatorGlobals g_globals;

void CHostEmulatorOptions::LoadOptions(const std::filesystem::path& filePath)
{
  AutoPtr<IniFileConfiguration> pConf(new IniFileConfiguration(filePath.string()));

  if (pConf->has("hostToIGPort"))
  {
    int hostToIGPort = pConf->getInt("hostToIGPort");
    if (hostToIGPort >= 0 && hostToIGPort <= 65535)
    {
      hostSetupOptions.hostToIGPort = static_cast<uint16_t>(hostToIGPort);
    }
  }

  if (pConf->has("igToHostPort"))
  {
    int igToHostPort = pConf->getInt("igToHostPort");
    if (igToHostPort >= 0 && igToHostPort <= 65535)
    {
      hostSetupOptions.igToHostPort = static_cast<uint16_t>(igToHostPort);
    }
  }

  if (pConf->has("hostIPAddress"))
  {
    hostSetupOptions.hostIPAddress = pConf->getString("hostIPAddress");
  }

  if (pConf->has("igIPAddress"))
  {
    hostSetupOptions.igIPAddress = pConf->getString("igIPAddress");
  }

  if (pConf->has("cigiVersion"))
  {
    string sCigiVersion = pConf->getString("cigiVersion");
    if (sCigiVersion == "3.3")
    {
      hostSetupOptions.eCigiVersion = ECigiVersion::VERSION_3_3;
    }
    else if (sCigiVersion == "4.0")
    {
      hostSetupOptions.eCigiVersion = ECigiVersion::VERSION_4_0;
    }
    else
    {
      //default to 4.0 if an unrecognized version is specified
      hostSetupOptions.eCigiVersion = ECigiVersion::VERSION_4_0;
    }
  }
  else
  {
    //default to 4.0 if an unrecognized version is specified
    hostSetupOptions.eCigiVersion = ECigiVersion::VERSION_4_0;
  }

  if (pConf->has("synchronizationMode"))
  {
    string sSynchronizationMode = pConf->getString("synchronizationMode");

    //make synchronization mode lowercase
    std::transform(sSynchronizationMode.begin(), sSynchronizationMode.end(), sSynchronizationMode.begin(), ::tolower);

    if (sSynchronizationMode == "synchronous")
    {
      hostSetupOptions.eSynchronizationMode = ECigiSynchronizationMode::SYNCHRONOUS;
    }
    else if (sSynchronizationMode == "asynchronous")
    {
      hostSetupOptions.eSynchronizationMode = ECigiSynchronizationMode::ASYNCHRONOUS;
    }
  }

  if (pConf->has("defaultDatabase"))
  {
    hostSetupOptions.defaultDatabaseID = DatabaseID(static_cast<uint8_t>(pConf->getInt("defaultDatabase")));
  }

  if (pConf->has("bigEndianByteOrder"))
  {
    hostSetupOptions.bigEndianByteOrder = pConf->getBool("bigEndianByteOrder");
  }

  if (pConf->has("PathToCigiSisoConversionsFile"))
  {
    hostSetupOptions.pathToCigiSisoConversionsFile = g_globals.applicationsDataPath / pConf->getString("PathToCigiSisoConversionsFile");
  }

  if (pConf->has("databaseControl"))
  {
    if (pConf->getString("databaseControl") == "IG")
    {
      hostSetupOptions.bDatabaseIGControlled = true;
    }
    else if (pConf->getString("databaseControl") == "Host")
    {
      hostSetupOptions.bDatabaseIGControlled = false;
    }
  }

  hostSetupOptions.sessions.clear();

  if (pConf->has("sessionCount"))
  {
    int sessionCount = pConf->getInt("sessionCount");
    for (int nSessionIndex = 0; nSessionIndex < sessionCount; ++nSessionIndex)
    {
      const std::string prefix = "Session" + std::to_string(nSessionIndex) + ".";

      SHostSessionSetupOptions sessionSetupOptions;
      sessionSetupOptions.sessionID = UnknownSessionID;
      sessionSetupOptions.hostToIGPort = hostSetupOptions.hostToIGPort;
      sessionSetupOptions.igToHostPort = hostSetupOptions.igToHostPort;
      sessionSetupOptions.hostIPAddress = hostSetupOptions.hostIPAddress;
      sessionSetupOptions.igIPAddress = hostSetupOptions.igIPAddress;
      sessionSetupOptions.defaultDatabaseID = hostSetupOptions.defaultDatabaseID;

      if (pConf->has(prefix + "sessionID"))
      {
        sessionSetupOptions.sessionID = sbio::SessionID(static_cast<uint32_t>(pConf->getInt(prefix + "sessionID")));
      }

      if (pConf->has(prefix + "hostToIGPort"))
      {
        int hostToIGPort = pConf->getInt(prefix + "hostToIGPort");
        if (hostToIGPort >= 0 && hostToIGPort <= 65535)
        {
          sessionSetupOptions.hostToIGPort = static_cast<uint16_t>(hostToIGPort);
        }
      }

      if (pConf->has(prefix + "igToHostPort"))
      {
        int igToHostPort = pConf->getInt(prefix + "igToHostPort");
        if (igToHostPort >= 0 && igToHostPort <= 65535)
        {
          sessionSetupOptions.igToHostPort = static_cast<uint16_t>(igToHostPort);
        }
      }

      if (pConf->has(prefix + "hostIPAddress"))
      {
        sessionSetupOptions.hostIPAddress = pConf->getString(prefix + "hostIPAddress");
      }

      if (pConf->has(prefix + "igIPAddress"))
      {
        sessionSetupOptions.igIPAddress = pConf->getString(prefix + "igIPAddress");
      }

      if (pConf->has(prefix + "defaultDatabase"))
      {
        sessionSetupOptions.defaultDatabaseID = DatabaseID(static_cast<uint8_t>(pConf->getInt(prefix + "defaultDatabase")));
      }

      hostSetupOptions.sessions.push_back(sessionSetupOptions);
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
