//Copyright SimBlocks LLC 2016-2026
#include "ImageGenerator.h"
#include "CigiEvent.h"
#include "CigiLib/CigiConversions.h"
#include "CigiView.h"
#include "EngineLib/DatabaseLoader.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "ViewLib/ViewManager.h"
#include "EntityLib/EntityManager.h"
#include "SymbolLib/SymbolSurfaceManager.h"
#include "GlobalHeaders/Globals.h"
#include "IGCigiLib/IGCigiLib.h"
#include "CigiLib/CigiEntityTypes.h"
#include "IGCigiLib/PacketSenderV3.h"
#include "IGCigiLib/PacketSenderV4.h"
#include "IGCigiLib/PacketSenders.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGResponseEventDispatcher.h"
#include "EnvironmentalRegionHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/FlatEarthProjection.h"
#include "MathLib/UTMProjection.h"
#include "PacketHandler.h"
#include "PacketHandlerV3.h"
#include "PacketHandlerV4.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/StopWatch.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

const CigiDatabaseNumber DefaultDatabaseNumber(0);//Default Database Number from CIGI ICD

CCigiImageGenerator::CCigiImageGenerator(const sbio::cigi::ig::SIGSetupOptions& setupOptions) :
  m_SetupOptions(setupOptions), m_eDatabaseState(EIGDatabaseState::NO_DATABASE), m_eOperationMode(EIGMode::RESET), m_LastHostFrameNumber(0), m_ImageGeneratorFrameNumber(0), m_DatabaseNumber(DefaultDatabaseNumber), m_bTimeStampValid(false), m_eEarthReferenceModel(EEarthReferenceModel::WGS84)
{
  m_pDatabaseLoader = std::make_unique<CDatabaseLoader>();
  m_pExportedFunctionsEventDispatcher = std::make_unique<CIGResponseEventDispatcher>();
  g_CigiLibGlobals.pCigiEntityTypes = std::make_unique<CCigiEntityTypes>();

  if (setupOptions.bDatabaseControlledByIG)
  {
    m_eDatabaseState = EIGDatabaseState::IG_CONTROLLED;
  }

  if (setupOptions.bLogPacketText)
  {
    g_CigiLibGlobals.pCigiMessageLogger = std::make_unique<CCigiMessageLogger>(g_CigiLibGlobals.applicationDataPath);
    g_CigiLibGlobals.pCigiMessageLogger->ClearLog();
  }
  else
  {
    g_CigiLibGlobals.pCigiMessageLogger.reset();
  }
}

CCigiImageGenerator::~CCigiImageGenerator()
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->UnregisterListener<IGCIGIEvent>(this);
  }
}

void CCigiImageGenerator::BeginLoadingDatabase(CigiDatabaseNumber databaseNumber)
{
  stringstream ss;
  ss << "Begin Loading Database " << databaseNumber << endl;
  g_CigiLibGlobals.pLogger->LogInformation(ss);

  m_eDatabaseState = EIGDatabaseState::LOADING;

  DatabaseID databaseID = DatabaseID(databaseNumber.Value());
  m_pDatabaseLoader->BeginLoadingDatabase(databaseID);
}

void CCigiImageGenerator::UnloadDatabase()
{
  m_eDatabaseState = EIGDatabaseState::NO_DATABASE;
  m_DatabaseNumber = DefaultDatabaseNumber;

  m_pDatabaseLoader->UnloadCurrentDatabase();
}

CigiDatabaseNumber CCigiImageGenerator::GetCurrentDatabaseID() const
{
  return m_DatabaseNumber;
}

sbio::engine::CDatabaseLoader* CCigiImageGenerator::GetDatabaseLoader() const
{
  return m_pDatabaseLoader.get();
}

EIGDatabaseState CCigiImageGenerator::GetDatabaseState() const
{
  return m_eDatabaseState;
}

sbio::cigi::EEarthReferenceModel CCigiImageGenerator::GetEarthReferenceModel() const
{
  return m_eEarthReferenceModel;
}

const std::list<SImageGeneratorError>& CCigiImageGenerator::GetErrors() const
{
  return m_Errors;
}

FrameNumber CCigiImageGenerator::GetFrameNumber() const
{
  return m_ImageGeneratorFrameNumber;
}

EIGMode CCigiImageGenerator::GetOperationMode() const
{
  return m_eOperationMode;
}

FrameNumber CCigiImageGenerator::GetLastHostFrameNumber() const
{
  return m_LastHostFrameNumber;
}

sbio::cigi::ig::CCigiPacketSenders* CCigiImageGenerator::GetPacketSenders() const
{
  return m_pPacketSenders.get();
}

sbio::cigi::ig::CCigiMessageLogger* CCigiImageGenerator::GetCigiMessageLogger() const
{
  return g_CigiLibGlobals.pCigiMessageLogger.get();
}

sbio::cigi::ig::CIGResponseEventDispatcher* CCigiImageGenerator::GetExportedFunctionsEventDispatcher() const
{
  return m_pExportedFunctionsEventDispatcher.get();
}

std::vector<SCigiTerrestrialSurfaceCondition> CCigiImageGenerator::QueryTerrestrialSurfaceCondition(const SGeodeticCoordinates& query) const
{
  if (m_pPacketHandlers.empty())
  {
    return std::vector<SCigiTerrestrialSurfaceCondition>();
  }

  // For now, just query the first packet handler. Revisit this if intending to support multiple hosts sending environmental region packets.
  CCigiPacketHandler* pPacketHandler = m_pPacketHandlers.front().get();
  if (pPacketHandler == nullptr)
  {
    return std::vector<SCigiTerrestrialSurfaceCondition>();
  }

  CCigiEnvironmentalRegionHandler* pEnvironmentalRegionHandler = pPacketHandler->GetEnvironmentalRegionHandler();
  if (pEnvironmentalRegionHandler == nullptr)
  {
    return std::vector<SCigiTerrestrialSurfaceCondition>();
  }

  return pEnvironmentalRegionHandler->QueryTerrestrialSurface(query);
}

bool CCigiImageGenerator::GetIgControlReceived() const
{
  return m_bIgControlReceived;
}

bool CCigiImageGenerator::GetIgControlReceivedAfterPlay() const
{
  return m_bIGControlReceivedAfterPlay;
}

sbio::cigi::ig::SIGSetupOptions CCigiImageGenerator::GetSetupOptions() const
{
  return m_SetupOptions;
}

double CCigiImageGenerator::GetTimeInSeconds() const
{
  return m_PlayTimer->GetElapsedSeconds();
}

uint32_t CCigiImageGenerator::GetTimeStamp() const
{
  //timestamp is in 10 microseconds
  return static_cast<uint32_t>(m_PlayTimer->GetElapsedMicroseconds() / 10);
}

// check to see if CigiSiso path is set, if so, try and load it
void CCigiImageGenerator::LoadCigiToSisoEntityEnumerationConversionFile()
{
  // load Cigi to Siso Conversion File
  if (!m_SetupOptions.pathToCigiSisoConversionsFile.empty())
  {
    try
    {
      g_CigiLibGlobals.pCigiEntityTypes->LoadCigiToSisoConversionFileCsv(m_SetupOptions.pathToCigiSisoConversionsFile);
    }
    catch (std::exception& e)
    {
      stringstream ss;
      ss << "Failed to load Cigi to Siso conversion file: " << e.what() << std::endl;
      g_CigiLibGlobals.pLogger->LogError(ss.str());
    }
  }
  else
  {
    g_CigiLibGlobals.pLogger->LogError("Did not set path to Cigi to Siso conversion file.");
  }
}

ECigiSynchronizationMode CCigiImageGenerator::GetSynchronizationMode() const
{
  return m_SetupOptions.eSynchronizationMode;
}

void CCigiImageGenerator::Initialize()
{
  g_CigiLibGlobals.pEventDispatcher->RegisterListener<IGCIGIEvent>(this);
  m_PlayTimer = std::make_unique<CStopWatch>();

  if (GetSynchronizationMode() == ECigiSynchronizationMode::ASYNCHRONOUS)
  {
    m_bTimeStampValid = true;
  }
  else
  {
    m_bTimeStampValid = false;
  }

  m_pPacketSenders = std::make_unique<CCigiPacketSenders>();

  for (auto host : m_SetupOptions.hostSettings)
  {
    m_pPacketSenders->AddHost(*this, host.hostIPAddress, host.igToHostPort, m_SetupOptions.eCigiVersion);

    if (m_SetupOptions.eCigiVersion == ECigiVersion::VERSION_3_3)
    {
      g_CigiLibGlobals.pLogger->LogInformation("Initializing Image Generator CIGI Version 3.3");
      unique_ptr<CCigiPacketHandlerV3> pPacketHandler = make_unique<CCigiPacketHandlerV3>(*this, host.hostIPAddress, host.hostToIGPort, host.igToHostPort);

      m_pPacketHandlers.push_back(std::move(pPacketHandler));
    }
    else if (m_SetupOptions.eCigiVersion == ECigiVersion::VERSION_4_0)
    {
      g_CigiLibGlobals.pLogger->LogInformation("Initializing Image Generator CIGI Version 4.0");
      unique_ptr<CCigiPacketHandlerV4> pPacketHandler = make_unique<CCigiPacketHandlerV4>(*this, host.hostIPAddress, host.hostToIGPort, host.igToHostPort);

      m_pPacketHandlers.push_back(std::move(pPacketHandler));
    }
  }

  LoadCigiToSisoEntityEnumerationConversionFile();

  SendPackets();//send start of frame packet
}

bool CCigiImageGenerator::IsTimeStampValid() const
{
  return m_bTimeStampValid;
}

void CCigiImageGenerator::OnDatabaseLoadedEvent()
{
  stringstream ss;
  ss << "On Database Loaded " << m_DatabaseNumber << endl;
  g_CigiLibGlobals.pLogger->LogInformation(ss);

  m_eDatabaseState = EIGDatabaseState::LOADED;
  m_eOperationMode = EIGMode::OPERATE;

  g_CigiLibGlobals.pProjection.reset();

  DatabaseID databaseID = DatabaseID(m_DatabaseNumber.Value());
  SDatabaseProjection* pDatabaseProjection = m_pDatabaseLoader->GetDatabaseProjection(databaseID);
  if (pDatabaseProjection != nullptr)
  {
    SGeodeticCoordinates geodetic;
    geodetic.latitude = pDatabaseProjection->originLatitude;
    geodetic.longitude = pDatabaseProjection->originLongitude;

    unique_ptr<CProjection> pProjection;
    if (pDatabaseProjection->projectionType == EDatabaseProjectionType::UTM)
    {
      pProjection = make_unique<CUTMProjection>();
    }
    else if (pDatabaseProjection->projectionType == EDatabaseProjectionType::FLAT_EARTH)
    {
      pProjection = make_unique<CFlatEarthProjection>();
    }

    if (pProjection != nullptr)
    {
      pProjection->Init(geodetic);
      g_CigiLibGlobals.pProjection = std::move(pProjection);
    }
  }
}

void CCigiImageGenerator::OnDatabaseLoadingFailedEvent()
{
  m_eDatabaseState = EIGDatabaseState::LOADING_FAILED;
}

void CCigiImageGenerator::OnEntityRemoved(sbio::EntityID entityID)
{
}

void CCigiImageGenerator::OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args)
{
  m_Errors.push_back(args.error);
}

void CCigiImageGenerator::ProcessPackets()
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->SetFrameNumber(GetFrameNumber());
  }

  for (auto&& pPacketHandler : m_pPacketHandlers)
  {
    pPacketHandler->ProcessPackets();
    pPacketHandler->Update();
  }
}

void CCigiImageGenerator::QueueLoadingDatabase(CigiDatabaseNumber databaseNumber)
{
  if (databaseNumber != m_DatabaseNumber)
  {
    m_pDatabaseLoader->UnloadCurrentDatabase();
    g_CigiLibGlobals.pEntityManager->Reset();
    stringstream ss;
    ss << "Queue Loading Database " << databaseNumber;
    g_CigiLibGlobals.pLogger->LogInformation(ss);

    m_eDatabaseState = EIGDatabaseState::WAIT_FOR_ACKNOWLEDGE;
    m_DatabaseNumber = databaseNumber;
  }
}

void CCigiImageGenerator::SendPackets()
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->SetFrameNumber(GetFrameNumber());
  }

  m_pPacketSenders->SendPackets();
}

void CCigiImageGenerator::SetCurrentDatabaseNumber(CigiDatabaseNumber databaseNumber)
{
  m_DatabaseNumber = databaseNumber;
}

void CCigiImageGenerator::Update(double fTimeStepSeconds)
{
  ProcessPackets();

  g_CigiLibGlobals.pEntityManager->Update(false, fTimeStepSeconds);
  g_CigiLibGlobals.pViewManager->Update();
  g_CigiLibGlobals.pSymbolSurfaceManager->Update();
  SetIgControlReceived(false);

  if (m_SetupOptions.eSynchronizationMode == ECigiSynchronizationMode::SYNCHRONOUS)
  {
    // During synchronous operation, the IG shall send an IG-to-Host message to the Host to indicate a new frame.
    // The IG-to-Host message also contains mission function responses, event notifications, and other IG data.
    // functions as a �heartbeat� that dictates the timing of data transfers between the IG and Host.
    SendPackets();

    // NOTE:
    // Per the CIGI specification, the synchronous IG-to-Host message transmission
    // is usually driven by a vertical sync (VSync) signal from the display system.
  }
  else if (m_SetupOptions.eSynchronizationMode == ECigiSynchronizationMode::ASYNCHRONOUS)
  {
    // During asynchronous operation, the Host may send data to the IG at arbitrary intervals.
    // The IG may in turn send messages to the Host containing IG status and mission function data
    m_bTimeStampValid = true;
    SendPackets();
  }

  ++m_ImageGeneratorFrameNumber;
}

void CCigiImageGenerator::SetOperationMode(EIGMode eOperationMode)
{
  m_eOperationMode = eOperationMode;
}

void CCigiImageGenerator::SetInteropolationEnabled(bool bInterpolationEnabled)
{
  m_SetupOptions.bInterpolationEnabled = bInterpolationEnabled;
}

void CCigiImageGenerator::SetLastHostFrameNumber(FrameNumber lastHostFrameNumber)
{
  m_LastHostFrameNumber = lastHostFrameNumber;
}

void CCigiImageGenerator::SetIgControlReceived(bool bReceived)
{
  m_bIgControlReceived = bReceived;
}

void CCigiImageGenerator::SetIgControlReceivedAfterPlay(bool bReceived)
{
  m_bIGControlReceivedAfterPlay = bReceived;
}

void CCigiImageGenerator::StartPlaying()
{
  m_bIgControlReceived = false;
  m_bIGControlReceivedAfterPlay = false;

  if (m_SetupOptions.bDatabaseControlledByIG)
  {
    m_eDatabaseState = EIGDatabaseState::IG_CONTROLLED;
    m_eOperationMode = EIGMode::OPERATE;
  }
  else
  {
    m_eDatabaseState = EIGDatabaseState::NO_DATABASE;
  }

  m_DatabaseNumber = DefaultDatabaseNumber;
  m_PlayTimer->Start();
}

void CCigiImageGenerator::StopPlaying()
{
  m_PlayTimer->Reset();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
