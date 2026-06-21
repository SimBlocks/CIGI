//Copyright SimBlocks LLC 2016-2026
#include "CIGIHostOptions.h"
#include "CigiLib/CigiTypes.h"
#include "GlobalHeaders/Globals.h"
#include "Poco/Util/IniFileConfiguration.h"
#include <memory>

using namespace std;
using namespace sbio;
using namespace sbio::cigi;
using namespace Poco;
using namespace Poco::Util;

extern SGlobals g_globals;

void CCIGIHostOptions::LoadOptions(const std::filesystem::path& filePath)
{
  if (!std::filesystem::exists(filePath))
  {
    return;
  }

  AutoPtr<IniFileConfiguration> pConf(new IniFileConfiguration(filePath.string()));

  if (pConf->has("hostToIGPort"))
  {
    hostSetupOptions.hostToIGPort = static_cast<uint16_t>(pConf->getInt("hostToIGPort"));
  }

  if (pConf->has("igToHostPort"))
  {
    hostSetupOptions.igToHostPort = static_cast<uint16_t>(pConf->getInt("igToHostPort"));
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
      hostSetupOptions.eCigiVersion = ECigiVersion::UNKNOWN_VERSION;
    }
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

  if (pConf->has("scriptCategory"))
  {
    hostSetupOptions.sScriptCategory = pConf->getString("scriptCategory");
  }

  if (pConf->has("script"))
  {
    hostSetupOptions.sScript = pConf->getString("script");
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
