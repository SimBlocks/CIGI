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

const int MAX_OVERFLOW_BYTES = 4 * 1024 * 1024;
const int MAX_OVERFLOW_PACKETS = 1024;

CHostSession::CHostSession()
{
  memset(m_sendBuffer, 0, MAX_UDP_SIZE);

  m_pDisconnectedTimer = make_unique<CStopWatch>();
}

CHostSession::~CHostSession()
{
}

void CHostSession::RaiseSessionEvent(HostCigiEventArgs& args) const
{
  args.sessionID = m_SessionID;
  Event::Raise<HostCigiEvent>(args);
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

bool CHostSession::GetHostTimestamp(uint32_t& timestamp) const
{
  timestamp = 0;
  if (!m_pSessionStopWatch || !m_pSessionStopWatch->IsRunning())
  {
    return false;
  }

  // CIGI uses 10-microsecond ticks. Unsigned conversion wraps modulo 2^32.
  timestamp = static_cast<uint32_t>(m_pSessionStopWatch->GetElapsedMicroseconds() / 10);
  return true;
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
    HostCigiMessageEventArgs args;
    args.sMessage = ss.str();
    RaiseSessionEvent(args);
  }

  try
  {
    m_pSocketHostToIG = make_unique<CUDPSendSocket>(hostSetupOptions.igIPAddress, hostSetupOptions.hostToIGPort);
    m_pSocketIGToHost = make_unique<CUDPReceiveSocket>(hostSetupOptions.igToHostPort);

    if (!m_pSocketHostToIG->IsOpen() || !m_pSocketIGToHost->IsOpen())
    {
      m_pSocketHostToIG.reset();
      m_pSocketIGToHost.reset();

      stringstream ss;
      ss << "Failed to initialize sockets for Host session " << m_SessionID.Value() << ". Target IG endpoint: " << hostSetupOptions.igIPAddress << ":"
         << hostSetupOptions.hostToIGPort << ", local receive port: " << hostSetupOptions.igToHostPort << ".";
      HostCigiErrorEventArgs args;
      args.sError = ss.str();
      RaiseSessionEvent(args);
      return;
    }

    stringstream ss;
    ss << "Host session " << m_SessionID.Value() << " sockets created. Waiting for IG packets on local UDP port " << hostSetupOptions.igToHostPort
       << " while sending IG Control to " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ".";
    HostCigiMessageEventArgs args;
    args.sMessage = ss.str();
    RaiseSessionEvent(args);
  }
  catch (const Poco::IOException& ex)
  {
    m_pSocketHostToIG.reset();
    m_pSocketIGToHost.reset();

    stringstream ss;
    ss << "Network error during initialization for Host session " << m_SessionID.Value() << ". " << ex.message() << " Target IG endpoint: " << hostSetupOptions.igIPAddress << ":"
       << hostSetupOptions.hostToIGPort << ", local receive port: " << hostSetupOptions.igToHostPort << ".";
    HostCigiErrorEventArgs args;
    args.sError = ss.str();
    RaiseSessionEvent(args);
  }
  catch (const Poco::Exception& ex)
  {
    m_pSocketHostToIG.reset();
    m_pSocketIGToHost.reset();

    stringstream ss;
    ss << "Failed to initialize Host session " << m_SessionID.Value() << ". " << ex.message() << " Target IG endpoint: " << hostSetupOptions.igIPAddress << ":"
       << hostSetupOptions.hostToIGPort << ", local receive port: " << hostSetupOptions.igToHostPort << ".";
    HostCigiErrorEventArgs args;
    args.sError = ss.str();
    RaiseSessionEvent(args);
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

bool CHostSession::QueueOverflowPacket(const void* packet, int nSize)
{
  if (packet == nullptr || nSize <= 0)
  {
    return false;
  }

  if (m_OverflowBuffers.size() >= MAX_OVERFLOW_PACKETS || nSize > MAX_OVERFLOW_BYTES - m_nOverflowBytes)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Outgoing packet queue limit reached (4 MiB or 1024 packets). New packet was rejected.";
    RaiseSessionEvent(args);
    return false;
  }

  std::unique_ptr<TBuffer<char>> pBuffer = std::make_unique<TBuffer<char>>(nSize);
  memcpy(pBuffer->GetBuffer(), packet, nSize);
  m_OverflowBuffers.push_back(std::move(pBuffer));
  m_nOverflowBytes += nSize;
  return true;
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
    m_nOverflowBytes -= pQueuedPacket->GetSize();
    m_OverflowBuffers.pop_front();
  }
}

void CHostSession::ClearQueuedPackets()
{
  m_nSendBufferLength = 0;
  m_OverflowBuffers.clear();
  m_nOverflowBytes = 0;
}

void CHostSession::ClearSessionData()
{
  ClearQueuedPackets();
  m_PendingDatabaseLoadedNotification.reset();
  m_LineOfSightRequestCoordinateSystems.clear();
}

bool CHostSession::Pack(const void* packet, int nSize)
{
  if (packet == nullptr || nSize <= 0)
  {
    return false;
  }

  if (nSize > MAX_UDP_SIZE)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Packet size exceeds maximum UDP payload and cannot be deferred to the next frame.";

    cout << args.sError << endl;
    RaiseSessionEvent(args);
    return false;
  }

  if (m_nSendBufferLength + nSize <= MAX_UDP_SIZE && m_OverflowBuffers.empty())
  {
    memcpy(&m_sendBuffer[m_nSendBufferLength], packet, nSize);
    m_nSendBufferLength += nSize;
  }
  else
  {
    return QueueOverflowPacket(packet, nSize);
  }

  return true;
}

bool CHostSession::Pack(const void* basePacket, int nBasePacketSize, const void* recordsPacket, int nRecordsPacketSize)
{
  if (basePacket == nullptr || nBasePacketSize <= 0)
  {
    return Pack(recordsPacket, nRecordsPacketSize);
  }

  if (recordsPacket == nullptr || nRecordsPacketSize <= 0)
  {
    return Pack(basePacket, nBasePacketSize);
  }

  if (nBasePacketSize > MAX_UDP_SIZE || nRecordsPacketSize > MAX_UDP_SIZE - nBasePacketSize)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Packet size exceeds maximum UDP payload and cannot be deferred to the next frame.";

    cout << args.sError << endl;
    RaiseSessionEvent(args);
    return false;
  }

  const int nTotalSize = nBasePacketSize + nRecordsPacketSize;
  std::unique_ptr<TBuffer<char>> pBuffer = std::make_unique<TBuffer<char>>(nTotalSize);
  memcpy(pBuffer->GetBuffer(), basePacket, nBasePacketSize);
  memcpy(pBuffer->GetBuffer() + nBasePacketSize, recordsPacket, nRecordsPacketSize);
  return Pack(pBuffer->GetBuffer(), pBuffer->GetSize());
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
  m_bValidPacketReceived = false;

  if (n <= 0)
  {
    if (!m_bConnected && !m_bHasReportedWaitingForConnection)
    {
      stringstream ss;
      ss << "No IG packets received yet for Host session " << m_SessionID.Value() << "."
         << " Verify IG is running, sending to " << hostSetupOptions.hostIPAddress << ":" << hostSetupOptions.igToHostPort << ", and listening on " << hostSetupOptions.hostToIGPort
         << ".";
      HostCigiErrorEventArgs args;
      args.sError = ss.str();
      RaiseSessionEvent(args);
      m_bHasReportedWaitingForConnection = true;
    }
  }
  else
  {
    bDataReceived = true;
    // a message has been received from the client
    // process each packet in the message
    int nRemainingBytes = n;
    uint8_t* pBuf = buffer.data();

    while (nRemainingBytes > 0)
    {
      int nPacketSize = ProcessPacket(pBuf, nRemainingBytes);
      if (nPacketSize <= 0 || nPacketSize > nRemainingBytes)
      {
        cout << "Error reading packets. Check CIGI version or packet size." << endl;
        HostCigiErrorEventArgs args;
        args.sError = "Error reading packets. Check CIGI version or packet size.";
        RaiseSessionEvent(args);
        break;
      }

      pBuf += nPacketSize;
      nRemainingBytes -= nPacketSize;
    }
  }

  if (m_bValidPacketReceived)
  {
    if (!m_bConnected)
    {
      cout << "Connected to IG!" << endl;
      m_bConnected = true;

      stringstream ss;
      ss << "Connected to IG for Host session " << m_SessionID.Value() << ". Received IG traffic on local port " << hostSetupOptions.igToHostPort
         << " after sending Host traffic to " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ".";
      HostCigiMessageEventArgs args;
      args.sMessage = ss.str();
      RaiseSessionEvent(args);
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
      ss << "Disconnected from IG for Host session " << m_SessionID.Value() << ". No IG packets were received for more than 2 seconds on local port "
         << hostSetupOptions.igToHostPort << ". Expected IG target endpoint is " << hostSetupOptions.hostIPAddress << ":" << hostSetupOptions.igToHostPort
         << ", and Host continues sending IG Control to " << hostSetupOptions.igIPAddress << ":" << hostSetupOptions.hostToIGPort << ".";
      HostCigiMessageEventArgs args;
      args.sMessage = ss.str();
      RaiseSessionEvent(args);
      m_bHasReportedWaitingForConnection = false;
    }
  }

  // Dispatch only after SetIGControl and its protocol-specific overrides have returned.
  if (m_bConnected && m_PendingDatabaseLoadedNotification)
  {
    HostCigiDatabaseLoadedEventArgs args;
    args.eDatabaseID = *m_PendingDatabaseLoadedNotification;
    m_PendingDatabaseLoadedNotification.reset();
    RaiseSessionEvent(args);
  }

  return bDataReceived;
}

void CHostSession::Reset()
{
  cout << "Resetting host" << endl;
  m_bConnected = false;
  m_bValidPacketReceived = false;
  m_bHasReportedWaitingForConnection = false;
  m_HostFrameNumber = FrameNumber(0);
  m_LastReceivedIGFrame = FrameNumber(0);
  m_DesiredIGMode = EIGMode::RESET;
  m_ActualIGMode = EIGMode::UNKNOWN;
  m_DatabaseNumber = CigiDatabaseNumber(0);
  m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;
  m_bIGControlledDatabaseRequested = false;
  ClearSessionData();
  m_pDisconnectedTimer->Stop();
  m_pDisconnectedTimer->Reset();

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
    ClearQueuedPackets();
    return;
  }

  uint8_t buffer[MAX_UDP_SIZE];
  uint8_t* pBuffer = buffer;
  SendIGControl(pBuffer);
  const int nControlSize = static_cast<int>(pBuffer - buffer);
  if (nControlSize <= 0 || nControlSize > MAX_UDP_SIZE)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Cannot send packets without a valid IG Control packet.";
    cout << args.sError << endl;
    RaiseSessionEvent(args);
    return;
  }

  const int nPayloadCapacity = MAX_UDP_SIZE - nControlSize;
  std::string sendError;
  int nOffset = 0;
  while (nOffset < m_nSendBufferLength)
  {
    const int nPacketSize = GetOutgoingPacketSize(reinterpret_cast<uint8_t*>(m_sendBuffer) + nOffset, m_nSendBufferLength - nOffset);
    if (nPacketSize <= 0 || nPacketSize > m_nSendBufferLength - nOffset)
    {
      sendError = "Error deferring overflow packets. Check CIGI version or packet size.";
      nOffset = m_nSendBufferLength;
      break;
    }

    if (nPacketSize > nPayloadCapacity)
    {
      sendError = "Packet cannot fit in a UDP payload alongside IG Control and was discarded.";
      nOffset += nPacketSize;
      continue;
    }

    if ((pBuffer - buffer) + nPacketSize > MAX_UDP_SIZE)
    {
      break;
    }

    memcpy(pBuffer, m_sendBuffer + nOffset, nPacketSize);
    pBuffer += nPacketSize;
    nOffset += nPacketSize;
  }

  if (!m_pSocketHostToIG->Send(reinterpret_cast<char*>(buffer), static_cast<int>(pBuffer - buffer)))
  {
    return;
  }

  // Keep deferred packets ahead of packets already in the overflow queue.
  m_nSendBufferLength -= nOffset;
  memmove(m_sendBuffer, m_sendBuffer + nOffset, m_nSendBufferLength);
  MoveQueuedPacketsToSendBuffer();

  ++m_HostFrameNumber;

  if (!sendError.empty())
  {
    HostCigiErrorEventArgs args;
    args.sError = sendError;
    cout << args.sError << endl;
    RaiseSessionEvent(args);
  }
}

void CHostSession::SetLoggingEnabled(bool bEnabled)
{
  m_bLoggingEnabled = bEnabled;
}

void CHostSession::SetWireByteOrder(bool bBigEndian)
{
  // Determine the native byte order of the host system
  const uint16_t endianCheck = 1;
  const bool bNativeBigEndian = reinterpret_cast<const uint8_t*>(&endianCheck)[0] == 0;

  // Enable byte swapping if the desired byte order differs from the native byte order
  SetByteSwapEnabled(bBigEndian != bNativeBigEndian);
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

void CHostSession::UpdateDatabaseState(CigiDatabaseNumber reportedDatabaseNumber)
{
  if (hostSetupOptions.bDatabaseIGControlled)
  {
    m_eDatabaseState = EHostSessionDatabaseState::IG_CONTROLLED;
    return;
  }

  const int databaseNumber = reportedDatabaseNumber.Value();
  if ((m_eDatabaseState == EHostSessionDatabaseState::LOADED || m_eDatabaseState == EHostSessionDatabaseState::LOADING_ACKNOWLEDGED) &&
      (m_ActualIGMode == EIGMode::RESET || databaseNumber == 0))
  {
    m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;
    ClearSessionData();

    HostCigiMessageEventArgs args;
    args.sMessage = "IG reset or unloaded database " + std::to_string(m_DatabaseNumber.Value()) + ". The database must be requested again.";
    RaiseSessionEvent(args);
    return;
  }

  if (m_DesiredIGMode != EIGMode::OPERATE ||
      (m_eDatabaseState != EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED && m_eDatabaseState != EHostSessionDatabaseState::LOADING_ACKNOWLEDGED))
  {
    return;
  }

  if (databaseNumber == -128)
  {
    // Retain the requested number for diagnostics and explicit retry, but stop sending the failed request.
    m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;
    m_PendingDatabaseLoadedNotification.reset();

    HostCigiErrorEventArgs args;
    args.sError = "IG failed to load database " + std::to_string(m_DatabaseNumber.Value()) + " (reported database -128).";
    RaiseSessionEvent(args);
    return;
  }

  if (databaseNumber < 0 && -databaseNumber == m_DatabaseNumber.Value())
  {
    m_eDatabaseState = EHostSessionDatabaseState::LOADING_ACKNOWLEDGED;
  }
  else if (databaseNumber > 0 && databaseNumber == m_DatabaseNumber.Value() && m_ActualIGMode != EIGMode::RESET)
  {
    m_eDatabaseState = EHostSessionDatabaseState::LOADED;
    HostCigiDatabaseLoadedEventArgs args;
    args.eDatabaseID = sbio::DatabaseID(static_cast<uint8_t>(databaseNumber));
    RaiseSessionEvent(args);
  }
}

bool CHostSession::SetIGControl(CigiDatabaseNumber databaseID, bool bEntityTypeSubstitutionEnabled, EIGMode eIGMode, bool bSmoothingEnabled)
{
  if (eIGMode != EIGMode::RESET && eIGMode != EIGMode::OPERATE && eIGMode != EIGMode::DEBUG)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Unsupported IG mode. Expected Reset, Operate, or Debug.";
    RaiseSessionEvent(args);
    return false;
  }

  // Only allow sending IGControl while connected
  if (!m_bConnected)
  {
    if (GetLoggingEnabled())
    {
      HostCigiErrorEventArgs args;
      args.sError = "Cannot send IGControl because not connected to IG.\n";
      RaiseSessionEvent(args);
    }

    return false;
  }

  m_DesiredIGMode = eIGMode;

  if (m_DesiredIGMode == EIGMode::RESET)
  {
    ClearSessionData();
    m_DatabaseNumber = CigiDatabaseNumber(0);
    m_eDatabaseState = EHostSessionDatabaseState::NO_DATABASE;
    m_bIGControlledDatabaseRequested = false;
  }
  else if (databaseID.Value() == 0)
  {
    return true;
  }
  else if (m_eDatabaseState == EHostSessionDatabaseState::IG_CONTROLLED)
  {
    if (!m_bIGControlledDatabaseRequested || m_DatabaseNumber != databaseID)
    {
      m_DatabaseNumber = databaseID;
      m_bIGControlledDatabaseRequested = true;
      m_PendingDatabaseLoadedNotification = sbio::DatabaseID(databaseID.Value());
    }
  }
  else if (m_DatabaseNumber != databaseID || (m_eDatabaseState == EHostSessionDatabaseState::NO_DATABASE && databaseID.Value() > 0))
  {
    m_DatabaseNumber = databaseID;
    m_eDatabaseState = EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED;
  }

  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
