//Copyright SimBlocks LLC 2016-2026
#include "Host.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiEntityTypes.h"
#include "HostCigiLib.h"
#include "HostCigiEvent.h"
#include "HostCigiLibTypes.h"
#include "HostSessionV3_3.h"
#include "HostSessionV4.h"
#include "UtilitiesLib/StopWatch.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

CHost::CHost()
{
  m_pCigiEntityTypes = std::make_unique<CCigiEntityTypes>();
}

CHost::~CHost()
{
}

sbio::cigi::CCigiEntityTypes* CHost::GetEntityTypes() const
{
  return m_pCigiEntityTypes.get();
}

const SHostSetupOptions& CHost::GetHostSetupOptions() const
{
  return m_HostSetupOptions;
}

CHostSession* CHost::GetHostSession() const
{
  return GetHostSession(m_ActiveSessionID);
}

CHostSession* CHost::GetHostSession(sbio::SessionID sessionID) const
{
  auto it = m_Sessions.find(sessionID);

  if (it == m_Sessions.end())
  {
    return nullptr;
  }

  return it->second.get();
}

std::vector<sbio::SessionID> CHost::GetSessionIDs() const
{
  std::vector<sbio::SessionID> sessionIDs;
  sessionIDs.reserve(m_Sessions.size());

  for (const auto& it : m_Sessions)
  {
    sessionIDs.push_back(it.first);
  }

  return sessionIDs;
}

sbio::SessionID CHost::GetActiveSessionID() const
{
  return m_ActiveSessionID;
}

bool CHost::SetActiveSessionID(sbio::SessionID sessionID)
{
  if (m_Sessions.find(sessionID) == m_Sessions.end())
  {
    return false;
  }

  m_ActiveSessionID = sessionID;
  return true;
}

sbio::cigi::host::CScriptRuntime* CHost::GetScriptRuntime() const
{
  return m_pScriptRuntime.get();
}

void CHost::Initialize(const SHostSetupOptions& options)
{
  cout << "Initializing host..." << endl;

  {
    HostCigiMessageEventArgs args;
    std::stringstream ss;
    ss << "Initializing HostEmulator networking"
       << "\n  Default Host -> IG target: " << options.igIPAddress << ":" << options.hostToIGPort << "\n  Default IG -> Host listen port: " << options.igToHostPort << "\n  CIGI Version: "
       << (options.eCigiVersion == ECigiVersion::VERSION_3_3   ? "3.3"
           : options.eCigiVersion == ECigiVersion::VERSION_4_0 ? "4.0"
                                                               : "Unknown")
       << "\n  Synchronization Mode: "
       << (options.eSynchronizationMode == ECigiSynchronizationMode::SYNCHRONOUS    ? "Synchronous"
           : options.eSynchronizationMode == ECigiSynchronizationMode::ASYNCHRONOUS ? "Asynchronous"
                                                                                    : "Unknown")
       << "\n  Configured Session Count: " << options.sessions.size();
    args.sMessage = ss.str();
    Event::Raise<HostCigiEvent>(args);
  }

  if (options.bEnableScripts)
  {
    m_pScriptRuntime = make_unique<CScriptRuntime>(g_HostCigiLibGlobals.applicationsDataPath / "HostEmulator\\Scripts\\");
  }
  else
  {
    m_pScriptRuntime.reset();
  }

  m_HostSetupOptions = options;
  m_eSynchronizationMode = options.eSynchronizationMode;
  if (m_eSynchronizationMode == ECigiSynchronizationMode::UNKNOWN)
  {
    m_eSynchronizationMode = ECigiSynchronizationMode::ASYNCHRONOUS;
  }
  LoadCigiToSisoEntityEnumerationConversionFile();

  m_Sessions.clear();

  std::vector<SHostSessionSetupOptions> sessionOptions = m_HostSetupOptions.sessions;
  if (sessionOptions.empty())
  {
    SHostSessionSetupOptions defaultSessionOptions;
    defaultSessionOptions.sessionID = sbio::SessionID(0);
    defaultSessionOptions.hostToIGPort = m_HostSetupOptions.hostToIGPort;
    defaultSessionOptions.igToHostPort = m_HostSetupOptions.igToHostPort;
    defaultSessionOptions.hostIPAddress = m_HostSetupOptions.hostIPAddress;
    defaultSessionOptions.igIPAddress = m_HostSetupOptions.igIPAddress;
    defaultSessionOptions.defaultDatabaseID = m_HostSetupOptions.defaultDatabaseID;
    sessionOptions.push_back(defaultSessionOptions);
  }

  for (const auto& sessionOption : sessionOptions)
  {
    std::unique_ptr<CHostSession> pHostSession;
    if (options.eCigiVersion == ECigiVersion::VERSION_3_3)
    {
      pHostSession = make_unique<CHostSessionV3_3>();
    }
    else if (options.eCigiVersion == ECigiVersion::VERSION_4_0)
    {
      pHostSession = make_unique<CHostSessionV4>();
    }
    else
    {
      HostCigiErrorEventArgs args;
      args.sError = "Unsupported CIGI version configured for host initialization: " + std::to_string(static_cast<int>(options.eCigiVersion));
      cout << "Error: " << args.sError << endl;
      Event::Raise<HostCigiEvent>(args);
      return;
    }

    pHostSession->SetSessionID(sessionOption.sessionID);

    SHostSetupOptions sessionHostOptions = m_HostSetupOptions;
    sessionHostOptions.hostToIGPort = sessionOption.hostToIGPort;
    sessionHostOptions.igToHostPort = sessionOption.igToHostPort;
    sessionHostOptions.hostIPAddress = sessionOption.hostIPAddress;
    sessionHostOptions.igIPAddress = sessionOption.igIPAddress;
    sessionHostOptions.defaultDatabaseID = sessionOption.defaultDatabaseID;
    sessionHostOptions.sessions.clear();

    pHostSession->hostSetupOptions = sessionHostOptions;
    pHostSession->Reset();
    pHostSession->Initialize();
    pHostSession->SetByteSwapEnabled(!sessionHostOptions.bigEndianByteOrder);
    m_Sessions[sessionOption.sessionID] = std::move(pHostSession);
  }

  if (!m_Sessions.empty())
  {
    if (!SetActiveSessionID(m_ActiveSessionID))
    {
      m_ActiveSessionID = m_Sessions.begin()->first;
    }
  }
  else
  {
    m_ActiveSessionID = sbio::SessionID(0);
  }

  if (m_pScriptRuntime != nullptr)
  {
    m_pScriptRuntime->LoadConfiguration();
  }
}

void CHost::LoadCigiToSisoEntityEnumerationConversionFile()
{
  if (!m_HostSetupOptions.pathToCigiSisoConversionsFile.empty())
  {
    try
    {
      m_pCigiEntityTypes->LoadCigiToSisoConversionFileCsv(m_HostSetupOptions.pathToCigiSisoConversionsFile);
    }
    catch (std::exception& e)
    {
      cout << "Error: Failed to load Cigi to Siso conversion file: " << e.what() << std::endl;
    }
  }
  else
  {
    cout << "Error: Did not set path to Cigi to Siso conversion file." << endl;
  }
}

bool CHost::ProcessPackets()
{
  bool retval = false;
  for (auto& it : m_Sessions)
  {
    auto& pHostSession = it.second;
    bool sessionConnected = pHostSession->ProcessPackets();

    if (sessionConnected)
    {
      //make sure to process all packets
      while (pHostSession->ProcessPackets())
      {
      }

      retval = true;
    }
  }
  return retval;
}

void CHost::SendPackets()
{
  for (auto& it : m_Sessions)
  {
    auto& pHostSession = it.second;
    pHostSession->SendPackets();
  }
}

void CHost::Update(double fDeltaTime)
{
  if (m_pScriptRuntime)
  {
    m_pScriptRuntime->Update();
  }

  if (m_eSynchronizationMode == ECigiSynchronizationMode::ASYNCHRONOUS)
  {
    SendPackets();
  }

  ProcessPackets();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
