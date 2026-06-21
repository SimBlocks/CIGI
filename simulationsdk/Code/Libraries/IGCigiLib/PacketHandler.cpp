//Copyright SimBlocks LLC 2016-2026
#include "PacketHandler.h"
#include "AccelerationControlHandler.h"
#include "AnimationControlHandler.h"
#include "ArticulatedPartHandler.h"
#include "AtmosphereControlHandler.h"
#include "CelestialSphereHandler.h"
#include "CollisionControlHandler.h"
#include "ComponentControlHandler.h"
#include "ViewLib/ViewManager.h"
#include "EntityControlHandler.h"
#include "EntityLib/EntityManager.h"
#include "EnvironmentalRegionHandler.h"
#include "IGCigiLib/IGCigiLib.h"
#include "IGCigiLib/SensorControlHandler.h"
#include "ImageGenerator.h"
#include "MaritimeSurfaceConditionPacketHandler.h"
#include "SymbolHandler.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/UDPReceiveSocket.h"
#include "UtilitiesLib/PacketLoggerWriter.h"
#include "UtilitiesLib/PacketLoggerReader.h"
#include "VelocityControlHandler.h"
#include "ViewHandler.h"
#include "WaveControlHandler.h"
#include "TerrainHandler.h"
#include "MotionTrackerControlHandler.h"
#include "EarthReferenceModelHandler.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include <memory>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::network;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiPacketHandler::CCigiPacketHandler(CCigiImageGenerator& imageGenerator, int nHostToIgPort) : m_ImageGenerator(imageGenerator)
{
  SIGSetupOptions options = imageGenerator.GetSetupOptions();

  if (options.ePacketLoggerState == EPacketLoggerState::WRITE)
  {
    m_pPacketLoggerWriter = std::make_unique<CPacketLoggerWriter>(g_CigiLibGlobals.applicationDataPath / "IGPacketLogger.log");
  }
  else if (options.ePacketLoggerState == EPacketLoggerState::READ)
  {
    m_pPacketLoggerReader = std::make_unique<CPacketLoggerReader>(g_CigiLibGlobals.applicationDataPath / "IGPacketLogger.log");
  }

  m_pAccelerationControlHandler = make_unique<CCigiAccelerationControlHandler>();
  m_pAnimationControlHandler = make_unique<CCigiAnimationControlHandler>();
  m_pArticulatedPartHandler = make_unique<CCigiArticulatedPartHandler>();
  m_pAtmosphereControlHandler = make_unique<CCigiAtmosphereControlHandler>();
  m_pCelestialSphereHandler = make_unique<CCigiCelestialSphereHandler>();
  m_pComponentControlHandler = make_unique<CCigiComponentControlHandler>();
  m_pTerrainHandler = make_unique<CCigiTerrainHandler>(imageGenerator);
  m_pEntityControlHandler = make_unique<CCigiEntityControlHandler>(m_ImageGenerator, m_pTerrainHandler.get());
  m_pEnvironmentalRegionHandler = make_unique<CCigiEnvironmentalRegionHandler>();
  m_pMaritimeSurfaceConditionPacketHandler = make_unique<CCigiMaritimeSurfaceConditionPacketHandler>(m_pEnvironmentalRegionHandler.get());
  m_pSymbolHandler = make_unique<CCigiSymbolHandler>();
  m_pVelocityControlHandler = make_unique<CCigiVelocityControlHandler>();
  m_pViewHandler = make_unique<CCigiViewHandler>(&m_ImageGenerator);
  m_pWaveControlHandler = make_unique<CCigiWaveControlHandler>(m_pEnvironmentalRegionHandler.get());
  m_pCollisionControlHandler = make_unique<CCigiCollisionControlHandler>();
  m_pSensorControlHandler = make_unique<CCigiSensorControlHandler>();
  m_pMotionTrackerControlHandler = make_unique<CCigiMotionTrackerControlHandler>();

  m_pSocketHostToIG = make_unique<CUDPReceiveSocket>(nHostToIgPort);

  if (m_ImageGenerator.GetSynchronizationMode() == ECigiSynchronizationMode::ASYNCHRONOUS)
  {
    m_pSocketHostToIG->SetBlocking(false);
  }
}

CCigiPacketHandler::~CCigiPacketHandler()
{
}

CCigiTerrainHandler* CCigiPacketHandler::GetTerrainHandler() const
{
  return m_pTerrainHandler.get();
};

CCigiEnvironmentalRegionHandler* CCigiPacketHandler::GetEnvironmentalRegionHandler() const
{
  return m_pEnvironmentalRegionHandler.get();
}

void CCigiPacketHandler::Handle(const SCigiIgControl& igControl)
{
  bool bModeChanged = false;
  stringstream ss;

  if (m_ImageGenerator.GetIgControlReceived() == true)
  {
    ss << "Received More Than One IG Control Packet"
       << "\n"
       << "Ignoring Undefined Behavior Of: "
       << "IG CONTROL"
       << "\n";
    g_CigiLibGlobals.pLogger->LogInformation(ss);
    return;
  }
  else
  {
    m_ImageGenerator.SetIgControlReceived(true);
  }

  if (m_ImageGenerator.GetOperationMode() != igControl.eIgMode)
  {
    ss << "IG Mode: " << ConvertToString(igControl.eIgMode) << endl;
    g_CigiLibGlobals.pLogger->LogInformation(ss);

    bModeChanged = true;
  }

  m_ImageGenerator.SetOperationMode(igControl.eIgMode);
  m_ImageGenerator.SetInteropolationEnabled(igControl.bSmoothingEnabled);
  //m_ImageGenerator.SetTimestamp(igControl.timestamp);
  m_ImageGenerator.SetLastHostFrameNumber((FrameNumber)igControl.hostFrameNumber);
  //m_ImageGenerator.SetTimestampValid(igControl.bTimestampValid);

  if (m_ImageGenerator.GetSynchronizationMode() == ECigiSynchronizationMode::ASYNCHRONOUS && !igControl.bTimestampValid)
  {
    //g_CigiLibGlobals.pLogger->LogError("TimeStampValid must be set to Valid because timestamp is required for asynchronous operation.");
  }

  if (m_ImageGenerator.GetOperationMode() == EIGMode::RESET)
  {
    if (bModeChanged)
    {
      if (!m_ImageGenerator.GetSetupOptions().bDatabaseControlledByIG)
      {
        if (m_ImageGenerator.GetDatabaseState() == EIGDatabaseState::LOADED)
        {
          m_ImageGenerator.UnloadDatabase();
        }
      }

      g_CigiLibGlobals.pLogger->LogInformation("Resetting IG");
      g_CigiLibGlobals.pEntityManager->Reset();
      g_CigiLibGlobals.pViewManager->Reset();
      m_pComponentControlHandler->Reset();
      m_pSymbolHandler->Reset();
      m_pTerrainHandler->Reset();
      m_pCollisionControlHandler->Reset();
    }
  }
  else
  {
    if (m_ImageGenerator.GetSetupOptions().bDatabaseControlledByIG)
    {
      //ignore database number if database is IG-controlled
    }
    else if (m_ImageGenerator.GetDatabaseState() == EIGDatabaseState::WAIT_FOR_ACKNOWLEDGE)
    {
      //The IG shall indicate that the database is being loaded by negating the value and placing it in the Database Number parameter of the Start of Frame packet.
      //The Host shall then acknowledge this change by setting the Database Number parameter of the IG Control packet to zero (0).
      if (igControl.databaseNumber.Value() == 0)
      {
        m_ImageGenerator.BeginLoadingDatabase(m_ImageGenerator.GetCurrentDatabaseID());
      }
    }
    else if (igControl.databaseNumber.Value() > 0)//check if a database should be loaded
    {
      m_ImageGenerator.QueueLoadingDatabase(igControl.databaseNumber);
    }
  }
}

void CCigiPacketHandler::CheckHostConnection(std::string sHostAddress, double fCurrentTime)
{
  SHostConnectionInfo hostInfo;
  hostInfo.sHostAddress = sHostAddress;
  hostInfo.fLastMessageReceivedTime = fCurrentTime;

  bool bFound = false;

  //iterate through existing connections to check for timeouts
  for (auto it = m_HostConnections.begin(); it != m_HostConnections.end(); ++it)
  {
    if (it->sHostAddress == sHostAddress)
    {
      bFound = true;
      it->fLastMessageReceivedTime = fCurrentTime;

      //send connected message if previously disconnected
      if (!it->bConnected)
      {
        it->bConnected = true;
        SendConnectedToHostMessage(sHostAddress);
      }
    }
  }

  //add new host
  if (!bFound)
  {
    hostInfo.bConnected = true;
    m_HostConnections.push_back(hostInfo);
    SendConnectedToHostMessage(sHostAddress);
  }
}

void CCigiPacketHandler::CheckHostDisconnection(double fCurrentTime)
{
  //iterate through existing connections to check for timeouts
  for (auto it = m_HostConnections.begin(); it != m_HostConnections.end(); ++it)
  {
    double fTimeSinceLastMessage = fCurrentTime - it->fLastMessageReceivedTime;
    if (it->bConnected && fTimeSinceLastMessage > 5.0)//5 seconds without a message means the host is disconnected
    {
      SendDisconnectedFromHostMessage(it->sHostAddress);
      it->bConnected = false;
    }
  }
}

void CCigiPacketHandler::SendConnectedToHostMessage(std::string sHostAddress)
{
  SHostConnectedMessage data;
  data.sHostIP = sHostAddress;
  g_CigiLibGlobals.pEventMessenger->SendHostConnectedMessage(data);
}

void CCigiPacketHandler::SendDisconnectedFromHostMessage(std::string sHostAddress)
{
  SHostDisconnectedMessage data;
  data.sHostIP = sHostAddress;
  g_CigiLibGlobals.pEventMessenger->SendHostDisconnectedMessage(data);
}

void CCigiPacketHandler::Update()
{
  m_pEntityControlHandler->Update();
  m_pTerrainHandler->Update(m_ImageGenerator.GetLastHostFrameNumber());

  double fCurrentTime = m_ImageGenerator.GetTimeInSeconds();
  CheckHostDisconnection(fCurrentTime);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
