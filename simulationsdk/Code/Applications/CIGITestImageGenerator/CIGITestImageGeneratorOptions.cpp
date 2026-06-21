//Copyright SimBlocks LLC 2016-2026
#include "CIGITestImageGeneratorOptions.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiLib.h"
#include "main.h"
#include "UtilitiesLib/Logger.h"
#include "Poco/Exception.h"
#include "Poco/Util/IniFileConfiguration.h"
#include <iostream>
#include <memory>

using namespace std;
using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace Poco;
using namespace Poco::Util;

extern STestImageGeneratorGlobals globals;

const sbio::cigi::ig::SIGSetupOptions& CCIGITestImageGeneratorOptions::GetOptions() const
{
  return m_SetupOptions;
}

bool CCIGITestImageGeneratorOptions::LoadOptions(const std::filesystem::path& filePath)
{
  try
  {
    AutoPtr<IniFileConfiguration> pConf(new IniFileConfiguration(filePath.string()));

    if (pConf->has("imageGeneratorID"))
    {
      m_SetupOptions.imageGeneratorID = ImageGeneratorID(static_cast<uint16_t>(pConf->getInt("imageGeneratorID")));
    }

    SHostSettings hostSettings;
    bool bHasHostSettings = false;

    if (pConf->has("hostToIGPort"))
    {
      hostSettings.hostToIGPort = static_cast<uint16_t>(pConf->getInt("hostToIGPort"));
      bHasHostSettings = true;
    }

    if (pConf->has("igToHostPort"))
    {
      hostSettings.igToHostPort = static_cast<uint16_t>(pConf->getInt("igToHostPort"));
      bHasHostSettings = true;
    }

    if (pConf->has("hostIPAddress"))
    {
      hostSettings.hostIPAddress = pConf->getString("hostIPAddress");
      bHasHostSettings = true;
    }

    if (bHasHostSettings)
    {
      m_SetupOptions.hostSettings.push_back(hostSettings);
    }

    if (pConf->has("igIPAddress"))
    {
      m_SetupOptions.igIPAddress = pConf->getString("igIPAddress");
    }

    if (pConf->has("cigiVersion"))
    {
      string sCigiVersion = pConf->getString("cigiVersion");
      if (sCigiVersion == "3.3")
      {
        m_SetupOptions.eCigiVersion = ECigiVersion::VERSION_3_3;
      }
      else if (sCigiVersion == "4.0")
      {
        m_SetupOptions.eCigiVersion = ECigiVersion::VERSION_4_0;
      }
      else
      {
        m_SetupOptions.eCigiVersion = ECigiVersion::UNKNOWN_VERSION;
      }
    }

    if (pConf->has("synchronizationMode"))
    {
      string sSynchronizationMode = pConf->getString("synchronizationMode");
      std::transform(sSynchronizationMode.begin(), sSynchronizationMode.end(), sSynchronizationMode.begin(), ::tolower);

      if (sSynchronizationMode == "synchronous")
      {
        m_SetupOptions.eSynchronizationMode = ECigiSynchronizationMode::SYNCHRONOUS;
      }
      else if (sSynchronizationMode == "asynchronous")
      {
        m_SetupOptions.eSynchronizationMode = ECigiSynchronizationMode::ASYNCHRONOUS;
      }
      else
      {
        m_SetupOptions.eSynchronizationMode = ECigiSynchronizationMode::ASYNCHRONOUS;
      }
    }
    else
    {
      m_SetupOptions.eSynchronizationMode = ECigiSynchronizationMode::ASYNCHRONOUS;
    }

    if (pConf->has("PathToCigiSisoConversionsFile"))
    {
      m_SetupOptions.pathToCigiSisoConversionsFile = globals.applicationsDataPath / pConf->getString("PathToCigiSisoConversionsFile");
    }

    if (pConf->has("PacketLogger"))
    {
      string s = pConf->getString("PacketLogger");

      if (s == "None")
      {
        m_SetupOptions.ePacketLoggerState = EPacketLoggerState::NONE;
      }
      else if (s == "Read")
      {
        m_SetupOptions.ePacketLoggerState = EPacketLoggerState::READ;
      }
      else if (s == "Write")
      {
        m_SetupOptions.ePacketLoggerState = EPacketLoggerState::WRITE;
      }
    }

    if (pConf->has("PacketTextLogger"))
    {
      string s = pConf->getString("PacketTextLogger");
      std::transform(s.begin(), s.end(), s.begin(), ::tolower);
      m_SetupOptions.bLogPacketText = s == "true";
    }

    if (pConf->has("databaseControl"))
    {
      string s = pConf->getString("databaseControl");

      if (s == "Host")
      {
        m_SetupOptions.bDatabaseControlledByIG = false;
      }
      else if (s == "IG")
      {
        m_SetupOptions.bDatabaseControlledByIG = true;
      }
    }

    return true;
  }
  catch (const Poco::Exception& e)
  {
    stringstream ss;
    ss << "Failed to load CIGITestImageGenerator options from '" << filePath.string() << "': " << e.displayText();
    if (globals.pLogger)
    {
      globals.pLogger->LogError(ss.str());
    }
    else
    {
      cerr << ss.str() << endl;
    }
  }
  catch (const std::exception& e)
  {
    stringstream ss;
    ss << "Failed to load CIGITestImageGenerator options from '" << filePath.string() << "': " << e.what();
    if (globals.pLogger)
    {
      globals.pLogger->LogError(ss.str());
    }
    else
    {
      cerr << ss.str() << endl;
    }
  }

  return false;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
