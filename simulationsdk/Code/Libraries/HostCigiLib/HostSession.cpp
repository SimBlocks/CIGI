//Copyright SimBlocks LLC 2016-2026
#include "HostSession.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiEvent.h"
#include "Poco/Exception.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/StopWatch.h"
#include "UtilitiesLib/UDPReceiveSocket.h"
#include "UtilitiesLib/UDPSendSocket.h"
#include "UtilitiesLib/Buffer.h"
#include <array>
#include <iostream>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

CHostSession::CHostSession()
{
  memset(m_sendBuffer, 0, MAX_UDP_SIZE);

  m_pDisconnectedTimer = make_unique<CStopWatch>();
}

CHostSession::~CHostSession()
{
}

CigiDatabaseNumber CHostSession::GetDatabaseNumber() const
{
  return m_DatabaseNumber;
}

EHostSessionDatabaseState CHostSession::GetDatabaseState() const
{
  return m_eDatabaseState;
}

sbio::cigi::EIGMode CHostSession::GetActualIGMode() const
{
  return m_ActualIGMode;
}

sbio::cigi::EIGMode CHostSession::GetDesiredIGMode() const
{
  return m_DesiredIGMode;
}

int CHostSession::GetFrameCount() const
{
  return m_HostFrameNumber.Value();
}

bool sbio::cigi::host::CHostSession::GetLoggingEnabled() const
{
  return m_bLoggingEnabled;
}

double CHostSession::GetSessionTime() const
{
  return m_pSessionStopWatch->GetElapsedSeconds();
}

sbio::SessionID CHostSession::GetSessionID() const
{
  return m_SessionID;
}

void CHostSession::Initialize()
{
  m_pSessionStopWatch = make_unique<CStopWatch>();
  m_pSessionStopWatch->Start();

  m_bConnected = false;
  m_bHasReportedWaitingForConnection = false;
  m_pSocketHostToIG.reset();
  m_pSocketIGToHost.reset();

  {
    stringstream ss;
    ss << "Initializing Host session " << m_SessionID.Value() << "\n"
       << "  Host -> IG target: " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << "\n"
       << "  IG -> Host listen port: " << hostSetupOptions.igToHostPort << "\n"
       << "  Host IP setting: " << hostSetupOptions.hostIPAddress << "\n"
       << "  IG IP setting: " << hostSetupOptions.igIPAddress << "\n"
       << "  CIGI Version: " << ConvertCigiVersionToString(hostSetupOptions.eCigiVersion) << "\n"
       << "  Synchronization Mode: " << ConvertCigiSynchronizationModeToString(hostSetupOptions.eSynchronizationMode);
    HostCigiErrorEventArgs args;
    args.sError = ss.str();
    Event::Raise<HostCigiEvent>(args);
  }

  try
  {
    m_pSocketHostToIG = make_unique<CUDPSendSocket>(hostSetupOptions.igIPAddress, hostSetupOptions.hostToIGPort);
    m_pSocketIGToHost = make_unique<CUDPReceiveSocket>(hostSetupOptions.igToHostPort);

    stringstream ss;
    ss << "Host session " << m_SessionID.Value() << " sockets created. Waiting for IG packets on local UDP port " << hostSetupOptions.igToHostPort << " while sending IG Control to " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ".";
    HostCigiErrorEventArgs args;
    args.sError = ss.str();
    Event::Raise<HostCigiEvent>(args);
  }
  catch (const Poco::IOException& ex)
  {
    m_pSocketHostToIG.reset();
    m_pSocketIGToHost.reset();

    stringstream ss;
    ss << "Network error during initialization for Host session " << m_SessionID.Value() << ". " << ex.message() << " Target IG endpoint: " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ", local receive port: " << hostSetupOptions.igToHostPort << ".";
    HostCigiErrorEventArgs args;
    args.sError = ss.str();
    Event::Raise<HostCigiEvent>(args);
  }
  catch (const Poco::Exception& ex)
  {
    m_pSocketHostToIG.reset();
    m_pSocketIGToHost.reset();

    stringstream ss;
    ss << "Failed to initialize Host session " << m_SessionID.Value() << ". " << ex.message() << " Target IG endpoint: " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ", local receive port: " << hostSetupOptions.igToHostPort << ".";
    HostCigiErrorEventArgs args;
    args.sError = ss.str();
    Event::Raise<HostCigiEvent>(args);
  }
}

bool CHostSession::IsConnected() const
{
  return m_bConnected;
}

void CHostSession::StoreLineOfSightRequestCoordinateSystem(LineOfSightRequestID requestID, ETopLevelCoordinateSystem eResponseCoordinateSystem)
{
  m_LineOfSightRequestCoordinateSystems[requestID] = eResponseCoordinateSystem;
}

ETopLevelCoordinateSystem CHostSession::GetLineOfSightRequestCoordinateSystem(LineOfSightRequestID requestID) const
{
  auto it = m_LineOfSightRequestCoordinateSystems.find(requestID);
  if (it == m_LineOfSightRequestCoordinateSystems.end())
  {
    return ETopLevelCoordinateSystem::UNKNOWN;
  }

  return it->second;
}

void CHostSession::QueueOverflowPacket(const void* packet, int nSize)
{
  std::unique_ptr<TBuffer<char>> pBuffer = std::make_unique<TBuffer<char>>(nSize);
  memcpy(pBuffer->GetBuffer(), packet, nSize);
  m_OverflowBuffers.push_back(std::move(pBuffer));
}

void CHostSession::MoveQueuedPacketsToSendBuffer()
{
  while (!m_OverflowBuffers.empty())
  {
    auto& pQueuedPacket = m_OverflowBuffers.front();
    if (m_nSendBufferLength + pQueuedPacket->GetSize() > MAX_UDP_SIZE)
    {
      break;
    }

    memcpy(&m_sendBuffer[m_nSendBufferLength], pQueuedPacket->GetBuffer(), pQueuedPacket->GetSize());
    m_nSendBufferLength += pQueuedPacket->GetSize();
    m_OverflowBuffers.pop_front();
  }
}

void CHostSession::Pack(const void* packet, int nSize)
{
  if (packet == nullptr || nSize <= 0)
  {
    return;
  }

  if (nSize > MAX_UDP_SIZE)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Packet size exceeds maximum UDP payload and cannot be deferred to the next frame.";

    cout << args.sError << endl;
    Event::Raise<HostCigiEvent>(args);
    return;
  }

  if (m_nSendBufferLength + nSize <= MAX_UDP_SIZE && m_OverflowBuffers.empty())
  {
    memcpy(&m_sendBuffer[m_nSendBufferLength], packet, nSize);
    m_nSendBufferLength += nSize;
  }
  else
  {
    QueueOverflowPacket(packet, nSize);
  }
}

void CHostSession::Pack(const void* basePacket, int nBasePacketSize, const void* recordsPacket, int nRecordsPacketSize)
{
  if (basePacket == nullptr || nBasePacketSize <= 0)
  {
    Pack(recordsPacket, nRecordsPacketSize);
    return;
  }

  if (recordsPacket == nullptr || nRecordsPacketSize <= 0)
  {
    Pack(basePacket, nBasePacketSize);
    return;
  }

  int nTotalSize = nBasePacketSize + nRecordsPacketSize;
  if (nTotalSize > MAX_UDP_SIZE)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Packet size exceeds maximum UDP payload and cannot be deferred to the next frame.";

    cout << args.sError << endl;
    Event::Raise<HostCigiEvent>(args);
    return;
  }

  std::unique_ptr<TBuffer<char>> pBuffer = std::make_unique<TBuffer<char>>(nTotalSize);
  memcpy(pBuffer->GetBuffer(), basePacket, nBasePacketSize);
  memcpy(pBuffer->GetBuffer() + nBasePacketSize, recordsPacket, nRecordsPacketSize);
  Pack(pBuffer->GetBuffer(), pBuffer->GetSize());
}

bool CHostSession::ProcessPackets()
{
  if (!m_pSocketIGToHost)
  {
    m_bConnected = false;
    return false;
  }

  std::array<uint8_t, MAX_UDP_SIZE> buffer = {};
  int n = 0;

  try
  {
    n = m_pSocketIGToHost->Receive(reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()));
  }
  catch (const Poco::Exception&)
  {
    m_bConnected = false;
    return false;
  }

  bool bDataReceived = false;

  if (n <= 0)
  {
    if (!m_bConnected && !m_bHasReportedWaitingForConnection)
    {
      stringstream ss;
      ss << "No IG packets received yet for Host session " << m_SessionID.Value() << "."
         << " Verify IG is running, sending to " << hostSetupOptions.hostIPAddress << ":" << hostSetupOptions.igToHostPort << ", and listening on " << hostSetupOptions.hostToIGPort << ".";
      HostCigiErrorEventArgs args;
      args.sError = ss.str();
      Event::Raise<HostCigiEvent>(args);
      m_bHasReportedWaitingForConnection = true;
    }
  }
  else
  {
    bDataReceived = true;
    //a message has been received from the client
    //process each packet in the message
    int nRemainingBytes = n;
    uint8_t* pBuf = buffer.data();

    while (nRemainingBytes > 0)
    {
      int nPacketSize = ProcessPacket(pBuf, nRemainingBytes);
      if (nPacketSize <= 0)
      {
        cout << "Error reading packets. Check CIGI version or packet size." << endl;
        HostCigiErrorEventArgs args;
        args.sError = "Error reading packets. Check CIGI version or packet size.";
        Event::Raise<HostCigiEvent>(args);
        break;
      }

      pBuf += nPacketSize;
      nRemainingBytes -= nPacketSize;
    }
  }

  if (bDataReceived)
  {
    if (!m_bConnected)
    {
      cout << "Connected to IG!" << endl;
      m_bConnected = true;

      stringstream ss;
      ss << "Connected to IG for Host session " << m_SessionID.Value() << ". Received IG traffic on local port " << hostSetupOptions.igToHostPort << " after sending Host traffic to " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ".";
      HostCigiErrorEventArgs args;
      args.sError = ss.str();
      Event::Raise<HostCigiEvent>(args);
    }

    m_bHasReportedWaitingForConnection = false;
    m_pDisconnectedTimer->Stop();
  }
  else if (!m_pDisconnectedTimer->IsRunning())
  {
    m_pDisconnectedTimer->Reset();
    m_pDisconnectedTimer->Start();
  }
  else if (m_pDisconnectedTimer->IsRunning() && m_pDisconnectedTimer->GetElapsedSeconds() > 2)
  {
    if (m_bConnected)
    {
      cout << "Disconnected from IG!" << endl;
      m_bConnected = false;

      stringstream ss;
      ss << "Disconnected from IG for Host session " << m_SessionID.Value() << ". No IG packets were received for more than 2 seconds on local port " << hostSetupOptions.igToHostPort << ". Expected IG target endpoint is " << hostSetupOptions.hostIPAddress << ":" << hostSetupOptions.igToHostPort
         << ", and Host continues sending IG Control to " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ".";
      HostCigiErrorEventArgs args;
      args.sError = ss.str();
      Event::Raise<HostCigiEvent>(args);
      m_bHasReportedWaitingForConnection = false;
    }
  }

  return bDataReceived;
}

void CHostSession::Reset()
{
  cout << "Resetting host" << endl;
  m_bConnected = false;
  m_bHasReportedWaitingForConnection = false;
  m_HostFrameNumber = FrameNumber(0);
  m_nSendBufferLength = 0;
  m_OverflowBuffers.clear();
  m_LineOfSightRequestCoordinateSystems.clear();
  if (m_pSocketHostToIG)
  {
    m_pSocketHostToIG->Close();
    m_pSocketHostToIG.reset();
  }

  if (m_pSocketIGToHost)
  {
    m_pSocketIGToHost->Close();
    m_pSocketIGToHost.reset();
  }
  cout << "Host reset" << endl;
}

void CHostSession::SendPackets()
{
  if (!m_pSocketHostToIG)
  {
    m_bConnected = false;
    m_nSendBufferLength = 0;
    m_OverflowBuffers.clear();
    return;
  }

  int nBytesToSend = 0;
  int nOffset = 0;
  while (nOffset < m_nSendBufferLength)
  {
    int nPacketSize = GetOutgoingPacketSize(reinterpret_cast<uint8_t*>(m_sendBuffer) + nOffset, m_nSendBufferLength - nOffset);
    if (nPacketSize <= 0)
    {
      HostCigiErrorEventArgs args;
      args.sError = "Error deferring overflow packets. Check CIGI version or packet size.";
      cout << args.sError << endl;
      Event::Raise<HostCigiEvent>(args);
      break;
    }

    if (nBytesToSend + nPacketSize <= MAX_UDP_SIZE)
    {
      nBytesToSend += nPacketSize;
    }
    else
    {
      QueueOverflowPacket(m_sendBuffer + nOffset, nPacketSize);
    }

    nOffset += nPacketSize;
  }

  m_nSendBufferLength = nBytesToSend;

  uint8_t buffer[MAX_UDP_SIZE];

  uint8_t* pBuffer = buffer;

  SendIGControl(pBuffer);

  if (m_nSendBufferLength > MAX_UDP_SIZE)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Send buffer exceeds maximum UDP payload.";
    cout << args.sError << endl;
    Event::Raise<HostCigiEvent>(args);
  }
  else if ((pBuffer - buffer) + m_nSendBufferLength > MAX_UDP_SIZE)
  {
    //send IG Control
    int64_t bufferSize = pBuffer - buffer;
    m_pSocketHostToIG->Send((char*)buffer, (int)bufferSize);

    //send other packets
    memcpy(buffer, m_sendBuffer, m_nSendBufferLength);
    m_pSocketHostToIG->Send((char*)buffer, m_nSendBufferLength);
  }
  else
  {
    //send other packets in addition to IG Control packets
    memcpy(pBuffer, m_sendBuffer, m_nSendBufferLength);
    pBuffer += m_nSendBufferLength;

    int64_t bufferSize = pBuffer - buffer;
    m_pSocketHostToIG->Send((char*)buffer, (int)bufferSize);
  }

  //always reset send buffer length
  m_nSendBufferLength = 0;
  MoveQueuedPacketsToSendBuffer();

  ++m_HostFrameNumber;
}

void CHostSession::SetLoggingEnabled(bool bEnabled)
{
  m_bLoggingEnabled = bEnabled;
}

void CHostSession::SetByteSwapEnabled(bool bEnabled)
{
  m_bByteSwap = bEnabled;
}

void CHostSession::SetSessionID(sbio::SessionID sessionID)
{
  m_SessionID = sessionID;
}

void CHostSession::NotifyStartOfFrameReceived()
{
  // if synchronization mode is synchronous, send packets immediately upon receiving start of frame
  if (hostSetupOptions.eSynchronizationMode == ECigiSynchronizationMode::SYNCHRONOUS)
  {
    SendPackets();
  }
}

bool CHostSession::SetIGControl(CigiDatabaseNumber databaseID, bool bEntityTypeSubstitutionEnabled, EIGMode eIGMode, bool bSmoothingEnabled)
{
  // Only allow sending IGControl while connected
  if (!m_bConnected)
  {
    if (GetLoggingEnabled())
    {
      HostCigiErrorEventArgs args;
      args.sError = "Cannot send IGControl because not connected to IG.\n";
      Event::Raise<HostCigiEvent>(args);
    }

    return false;
  }

  m_DesiredIGMode = eIGMode;

  if (m_DesiredIGMode == EIGMode::RESET)
  {
    m_DatabaseNumber = CigiDatabaseNumber(0);
    m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;
  }
  else if (m_eDatabaseState == EHostSessionDatabaseState::IG_CONTROLLED)
  {
    // if the IG controls the database, send a database loaded event notification
    HostCigiDatabaseLoadedEventArgs args;
    args.eDatabaseID = sbio::DatabaseID(databaseID.Value());
    Event::Raise<HostCigiEvent>(args);
  }
  else if (m_DatabaseNumber != databaseID)
  {
    m_DatabaseNumber = databaseID;
    m_eDatabaseState = EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED;
  }

  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
