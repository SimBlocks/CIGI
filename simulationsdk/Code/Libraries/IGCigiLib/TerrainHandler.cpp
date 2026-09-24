//Copyright SimBlocks LLC 2016-2026
#include "TerrainHandler.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "IGCigiLib.h"
#include "PacketSender.h"
#include "IGCigiLib/PacketSenders.h"
#include "EntityLib/EntityManager.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include "MathLib/Math.h"
#include "CigiEntity.h"
#include "CigiLib/CigiLib.h"
#include "MathLib/CoordinateConversions.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "TerrainCigiEvent.h"
#include "HATHOTEntityRequestHandler.h"
#include "HATHOTGlobalRequestHandler.h"
#include "LineOfSightSegmentRequestGeodeticToGeodeticBasicHandler.h"
#include "LineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler.h"
#include "LineOfSightSegmentRequestGeodeticToEntityBasicHandler.h"
#include "LineOfSightSegmentRequestGeodeticToEntityExtendedHandler.h"
#include "LineOfSightSegmentRequestEntityToGeodeticBasicHandler.h"
#include "LineOfSightSegmentRequestEntityToGeodeticExtendedHandler.h"
#include "LineOfSightSegmentRequestEntityToEntityBasicHandler.h"
#include "LineOfSightSegmentRequestEntityToEntityExtendedHandler.h"
#include "LineOfSightVectorRequestGeodeticBasicHandler.h"
#include "LineOfSightVectorRequestGeodeticExtendedHandler.h"
#include "LineOfSightVectorRequestEntityBasicHandler.h"
#include "LineOfSightVectorRequestEntityExtendedHandler.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::entity;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

namespace
{
  template <typename T>
  void LogHostTerrainRequest(const T& request, bool isFromHost)
  {
    if (isFromHost && g_CigiLibGlobals.pCigiMessageLogger != nullptr)
    {
      g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(request);
    }
  }
}

CCigiTerrainHandler::~CCigiTerrainHandler()
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->UnregisterListener<TerrainCigiEvent>(static_cast<ITerrainCigiEventListener*>(this));
    g_CigiLibGlobals.pEventDispatcher->UnregisterListener<IGCIGIEvent>(static_cast<IIGCIGIEventListener*>(this));
  }
}

void CCigiTerrainHandler::OnEntityRemoved(sbio::EntityID entityID)
{
  for (auto it = HATHOTRequests.begin(); it != HATHOTRequests.end();)
  {
    if (it->second->GetCoordinateSystem() == ETopLevelCoordinateSystem::ENTITY && it->second->GetEntityID() == entityID)
    {
      HATHOTUpdateRequests.erase(it->first);
      it = HATHOTRequests.erase(it);
    }
    else
    {
      ++it;
    }
  }

  for (auto it = m_LineOfSightRequests.begin(); it != m_LineOfSightRequests.end();)
  {
    if (it->second->ReferencesEntity(entityID))
    {
      LineOfSightUpdateRequests.erase(it->first);
      it = m_LineOfSightRequests.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiHatResponseEvent(const CigiHatResponseEventArgs& data)
{
  auto req = HATHOTRequests.find(HATHOTID(data.hatResponse.HATHOTID));
  if (req == HATHOTRequests.end() || req->second->GetRequestGeneration() != data.hatResponse.requestGeneration)
  {
    return;
  }

  CHATHOTRequestHandler* pRequest = req->second.get();
  SHeightAboveTerrainResponse response;
  response.HATHOTID = pRequest->GetRequestID();

  if (pRequest->GetUpdatePeriod().Value() != 0)
  {
    response.hostFrameLSN = data.hatResponse.hostFrameLSN;
  }

  response.bValid = data.hatResponse.bValid;
  response.heightAboveTerrain = data.hatResponse.heightAboveTerrain;

  m_ImageGenerator.GetPacketSenders()->SendHatResponse(response);

  // If this is a one shot request, remove it
  if (pRequest->GetUpdatePeriod().Value() == 0)
  {
    HATHOTRequests.erase(req);
  }
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiHotResponseEvent(const CigiHotResponseEventArgs& data)
{
  auto req = HATHOTRequests.find(HATHOTID(data.hotResponse.HATHOTID));
  if (req == HATHOTRequests.end() || req->second->GetRequestGeneration() != data.hotResponse.requestGeneration)
  {
    return;
  }

  CHATHOTRequestHandler* pRequest = req->second.get();
  SHeightOfTerrainResponse response;
  response.HATHOTID = pRequest->GetRequestID();

  if (pRequest->GetUpdatePeriod().Value() != 0)
  {
    response.hostFrameLSN = data.hotResponse.hostFrameLSN;
  }

  response.bValid = data.hotResponse.bValid;
  if (data.hotResponse.bValid)
  {
    response.heightOfTerrain = HeightRelativeToWGS84Ellipsoid(data.hotResponse.heightOfTerrain.Value());
  }

  m_ImageGenerator.GetPacketSenders()->SendHotResponse(response);

  // If this is a one shot request, remove it
  if (pRequest->GetUpdatePeriod().Value() == 0)
  {
    HATHOTRequests.erase(req);
  }
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiHatHotExtendedResponseEvent(const CigiHatHotExtendedResponseEventArgs& data)
{
  auto req = HATHOTRequests.find(HATHOTID(data.hatHotExtendedResponse.HATHOTID));
  if (req == HATHOTRequests.end() || req->second->GetRequestGeneration() != data.hatHotExtendedResponse.requestGeneration)
  {
    return;
  }

  CHATHOTRequestHandler* pRequest = req->second.get();
  EntityID entityID = pRequest->GetEntityID();

  SHATHOTExtendedResponse response;
  response.HATHOTID = pRequest->GetRequestID();

  if (pRequest->GetUpdatePeriod().Value() != 0)
  {
    response.hostFrameLSN = data.hatHotExtendedResponse.hostFrameLSN;
  }

  response.bValid = data.hatHotExtendedResponse.bValid;
  response.heightOfTerrain = data.hatHotExtendedResponse.heightOfTerrain;
  response.heightAboveTerrain = data.hatHotExtendedResponse.heightAboveTerrain;
  response.materialCode = data.hatHotExtendedResponse.materialCode;
  response.normalVectorAzimuth = data.hatHotExtendedResponse.normalVectorAzimuth;
  response.normalVectorElevation = data.hatHotExtendedResponse.normalVectorElevation;

  m_ImageGenerator.GetPacketSenders()->SendHatHotExtendedResponse(response);

  // remove one shot request
  if (pRequest->GetUpdatePeriod().Value() == 0)
  {
    HATHOTRequests.erase(req);
  }
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiLineOfSightResponseEvent(const CigiLineOfSightResponseEventArgs& args)
{
  auto losID = LineOfSightRequestID(args.lineOfSightResponse.lineOfSightRequestID);
  auto iter = m_LineOfSightRequests.find(losID);
  if (iter == m_LineOfSightRequests.end() || iter->second->GetRequestGeneration() != args.lineOfSightResponse.requestGeneration)
  {
    return;
  }

  CLineOfSightRequestHandler* pRequest = iter->second.get();
  auto response = args.lineOfSightResponse;
  if (response.bValid)
  {
    response.dRange += pRequest->GetRangeOffset();
  }
  if (pRequest->GetUpdatePeriod().Value() == 0)
  {
    m_ImageGenerator.GetPacketSenders()->SendLineOfSightResponse(response);
    if (pRequest->RecordResponse(args.lineOfSightResponse.responseCount))
    {
      m_LineOfSightRequests.erase(iter);
    }
    return;
  }

  m_ImageGenerator.GetPacketSenders()->SendLineOfSightResponse(response);
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiLineOfSightEntityResponseEvent(const CigiLineOfSightEntityResponseEventArgs& args)
{
  auto losID = LineOfSightRequestID(args.lineOfSightEntityResponse.lineOfSightRequestID);
  auto iter = m_LineOfSightRequests.find(losID);
  if (iter == m_LineOfSightRequests.end() || iter->second->GetRequestGeneration() != args.lineOfSightEntityResponse.requestGeneration)
  {
    return;
  }

  CLineOfSightRequestHandler* pRequest = iter->second.get();
  auto response = args.lineOfSightEntityResponse;
  if (response.bValid)
  {
    response.dRange += pRequest->GetRangeOffset();
  }
  if (pRequest->GetUpdatePeriod().Value() == 0)
  {
    m_ImageGenerator.GetPacketSenders()->SendLineOfSightEntityResponse(response);
    if (pRequest->RecordResponse(args.lineOfSightEntityResponse.responseCount))
    {
      m_LineOfSightRequests.erase(iter);
    }
    return;
  }

  m_ImageGenerator.GetPacketSenders()->SendLineOfSightEntityResponse(response);
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const CigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args)
{
  auto losID = LineOfSightRequestID(args.lineOfSightExtendedGeodeticCoordinatesResponse.lineOfSightRequestID);
  auto iter = m_LineOfSightRequests.find(losID);
  if (iter == m_LineOfSightRequests.end() || iter->second->GetRequestGeneration() != args.lineOfSightExtendedGeodeticCoordinatesResponse.requestGeneration)
  {
    return;
  }

  CLineOfSightRequestHandler* pRequest = iter->second.get();

  auto response = args.lineOfSightExtendedGeodeticCoordinatesResponse;
  if (response.bRangeValid)
  {
    response.dRange += pRequest->GetRangeOffset();
  }

  m_ImageGenerator.GetPacketSenders()->SendLineOfSightExtendedGeodeticCoordinatesResponse(response);

  if (pRequest->GetUpdatePeriod().Value() == 0 && pRequest->RecordResponse(response.responseCount))
  {
    m_LineOfSightRequests.erase(iter);
  }
}

namespace
{
  void CopyLineOfSightExtendedResponseBase(const SBaseLineOfSightExtendedResponse& source, SBaseLineOfSightExtendedResponse& destination)
  {
    destination.lineOfSightRequestID = source.lineOfSightRequestID;
    destination.bValid = source.bValid;
    destination.bRangeValid = source.bRangeValid;
    destination.bVisible = source.bVisible;
    destination.hostFrameLSN = source.hostFrameLSN;
    destination.responseCount = source.responseCount;
    destination.dRange = source.dRange;
    destination.surfaceColor = source.surfaceColor;
    destination.materialCode = source.materialCode;
    destination.fNormalVectorAzimuth = source.fNormalVectorAzimuth;
    destination.fNormalVectorElevation = source.fNormalVectorElevation;
    destination.requestGeneration = source.requestGeneration;
  }
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(
  const CigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args)
{
  auto losID = LineOfSightRequestID(args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.lineOfSightRequestID);
  auto iter = m_LineOfSightRequests.find(losID);
  if (iter == m_LineOfSightRequests.end() || iter->second->GetRequestGeneration() != args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.requestGeneration)
  {
    return;
  }

  CLineOfSightRequestHandler* pRequest = iter->second.get();
  auto response = args.lineOfSightExtendedEntityGeodeticCoordinatesResponse;
  if (response.bRangeValid)
  {
    response.dRange += pRequest->GetRangeOffset();
  }

  {
    if (response.entityID == UnknownEntityID)
    {
      // send line of sight extended geodetic coordinates response with the geodetic coordinates from the intersection point
      SLineOfSightExtendedGeodeticCoordinatesResponse geodeticResponse;
      CopyLineOfSightExtendedResponseBase(response, geodeticResponse);
      geodeticResponse.geodeticCoordinates = response.geodeticCoordinates;

      m_ImageGenerator.GetPacketSenders()->SendLineOfSightExtendedGeodeticCoordinatesResponse(geodeticResponse);
    }
    else
    {
      // send line of sight extended entity response with geodetic coordinates response from the intersection point
      SLineOfSightExtendedEntityGeodeticCoordinatesResponse geodeticResponse;
      CopyLineOfSightExtendedResponseBase(response, geodeticResponse);
      geodeticResponse.entityID = response.entityID;
      geodeticResponse.geodeticCoordinates = response.geodeticCoordinates;

      m_ImageGenerator.GetPacketSenders()->SendLineOfSightExtendedEntityGeodeticCoordinatesResponse(geodeticResponse);
    }
  }

  if (pRequest->GetUpdatePeriod().Value() == 0 && pRequest->RecordResponse(response.responseCount))
  {
    m_LineOfSightRequests.erase(iter);
  }
}

void sbio::cigi::ig::CCigiTerrainHandler::OnCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const CigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args)
{
  auto losID = LineOfSightRequestID(args.lineOfSightExtendedEntityCoordinatesResponse.lineOfSightRequestID);
  auto iter = m_LineOfSightRequests.find(losID);
  if (iter == m_LineOfSightRequests.end() || iter->second->GetRequestGeneration() != args.lineOfSightExtendedEntityCoordinatesResponse.requestGeneration)
  {
    return;
  }

  CLineOfSightRequestHandler* pRequest = iter->second.get();
  auto response = args.lineOfSightExtendedEntityCoordinatesResponse;
  if (response.bRangeValid)
  {
    response.dRange += pRequest->GetRangeOffset();
  }

  m_ImageGenerator.GetPacketSenders()->SendLineOfSightExtendedEntityCoordinatesResponse(response);

  if (pRequest->GetUpdatePeriod().Value() == 0 && pRequest->RecordResponse(response.responseCount))
  {
    m_LineOfSightRequests.erase(iter);
  }
}

CCigiTerrainHandler::CCigiTerrainHandler(CCigiImageGenerator& ImageGenerator) : m_ImageGenerator(ImageGenerator)
{
  if (g_CigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_CigiLibGlobals.pEventDispatcher->RegisterEvent<TerrainCigiEvent>(std::make_unique<TerrainCigiEventHandler>());
    g_CigiLibGlobals.pEventDispatcher->RegisterListener<TerrainCigiEvent>(static_cast<ITerrainCigiEventListener*>(this));
    g_CigiLibGlobals.pEventDispatcher->RegisterListener<IGCIGIEvent>(static_cast<IIGCIGIEventListener*>(this));
  }
  else if (g_CigiLibGlobals.pLogger != nullptr)
  {
    g_CigiLibGlobals.pLogger->LogError("CCigiTerrainHandler requires an event dispatcher to register TerrainCigiEvent.");
  }
}

template <class RequestType, class Hash>
bool CCigiTerrainHandler::CheckHost(const SHATHOTGlobalRequest& req, std::unordered_set<RequestType, Hash>& updateRequests,
                                    std::unordered_map<RequestType, std::unique_ptr<CHATHOTRequestHandler>, Hash>& requests, bool isFromHost)
{
  if (isFromHost)
  {
    if (req.updatePeriod.Value() != 0)
    {
      updateRequests.insert(req.requestID);
    }
    else
    {
      updateRequests.erase(req.requestID);
    }
    requests[req.requestID] = std::make_unique<CHATHOTGlobalRequestHandler>(req);
    requests[req.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }
  else if (req.updatePeriod.Value() == 0)
  {
    return false;
  }
  return true;
}

template <class RequestType, class Hash>
bool CCigiTerrainHandler::CheckHost(const SHATHOTEntityRequest& req, std::unordered_set<RequestType, Hash>& updateRequests,
                                    std::unordered_map<RequestType, std::unique_ptr<CHATHOTRequestHandler>, Hash>& requests, bool isFromHost)
{
  if (isFromHost)
  {
    if (req.updatePeriod.Value() != 0)
    {
      updateRequests.insert(req.requestID);
    }
    else
    {
      updateRequests.erase(req.requestID);
    }
    requests[req.requestID] = std::make_unique<CHATHOTEntityRequestHandler>(req);
    requests[req.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }
  else if (req.updatePeriod.Value() == 0)
  {
    return false;
  }
  return true;
}

template <class RequestType, class Hash>
void CCigiTerrainHandler::UpdateHATHOTRequests(std::unordered_set<RequestType, Hash>& updateRequests,
                                               std::unordered_map<RequestType, std::unique_ptr<CHATHOTRequestHandler>, Hash>& requests, int lastHostFrameNumber)
{
  const FrameNumber igFrameNumber = m_ImageGenerator.GetFrameNumber();
  for (auto it = updateRequests.begin(); it != updateRequests.end();)
  {
    auto request = requests.find(*it);
    // Skip any null requests (prevents crashes when requests are erased)
    if (request == requests.end() || request->second == nullptr)
    {
      it = updateRequests.erase(it);
      continue;
    }
    CHATHOTRequestHandler* p_request = request->second.get();
    if (p_request->GetUpdatePeriod().Value() == 0)
    {
      it = updateRequests.erase(it);
      continue;
    }

    if (p_request->IsUpdateDue(igFrameNumber))
    {
      p_request->SetLastUpdateFrame(igFrameNumber);
      p_request->SetLastHostFrameNumber(FrameNumber(lastHostFrameNumber));
      if (!p_request->Handle())
      {
        requests.erase(*it);
        it = updateRequests.erase(it);
        continue;
      }
    }

    it++;
  }
}

void CCigiTerrainHandler::UpdateLineOfSightRequests(int lastHostFrameNumber)
{
  const FrameNumber igFrameNumber = m_ImageGenerator.GetFrameNumber();
  auto it = m_LineOfSightRequests.begin();

  std::list<LineOfSightRequestID> removeList;

  while (it != m_LineOfSightRequests.end())
  {
    CLineOfSightRequestHandler* pRequest = it->second.get();

    if (pRequest->GetUpdatePeriod().Value() != 0 && pRequest->IsUpdateDue(igFrameNumber))
    {
      pRequest->SetLastUpdateFrame(igFrameNumber);
      pRequest->SetLastHostFrameNumber(FrameNumber(lastHostFrameNumber));
      if (!pRequest->Handle())
      {
        removeList.push_back(it->first);
      }
    }

    it++;
  }

  for (LineOfSightRequestID id : removeList)
  {
    auto it1 = m_LineOfSightRequests.find(id);
    if (it1 != m_LineOfSightRequests.end())
    {
      m_LineOfSightRequests.erase(it1);
    }
    auto it2 = LineOfSightUpdateRequests.find(id);
    if (it2 != LineOfSightUpdateRequests.end())
    {
      LineOfSightUpdateRequests.erase(it2);
    }
  }
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestGeodeticToGeodeticBasic& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestGeodeticToGeodeticBasicHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestGeodeticToGeodeticBasicHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestGeodeticToGeodeticExtended& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestGeodeticToEntityBasic& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestGeodeticToEntityBasicHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestGeodeticToEntityBasicHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestGeodeticToEntityExtended& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestGeodeticToEntityExtendedHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestGeodeticToEntityExtendedHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestEntityToGeodeticBasic& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestEntityToGeodeticBasicHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestEntityToGeodeticBasicHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestEntityToGeodeticExtended& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestEntityToGeodeticExtendedHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestEntityToGeodeticExtendedHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestEntityToEntityBasic& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestEntityToEntityBasicHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestEntityToEntityBasicHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const SLineOfSightSegmentRequestEntityToEntityExtended& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightSegmentRequestEntityToEntityExtendedHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightSegmentRequestEntityToEntityExtendedHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightVectorRequestGeodeticBasicHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightVectorRequestGeodeticBasicHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const sbio::cigi::SLineOfSightVectorRequestGeodeticExtended& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightVectorRequestGeodeticExtendedHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightVectorRequestGeodeticExtendedHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightVectorRequestEntityBasicHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightVectorRequestEntityBasicHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;

  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool CCigiTerrainHandler::Handle(const sbio::cigi::SLineOfSightVectorRequestEntityExtended& request, bool isFromHost)
{
  LogHostTerrainRequest(request, isFromHost);

  if (isFromHost)
  {
    if (request.updatePeriod.Value() != 0)
    {
      LineOfSightUpdateRequests.insert(request.requestID);
    }
    m_LineOfSightRequests[request.requestID] = std::make_unique<CLineOfSightVectorRequestEntityExtendedHandler>(request);
    m_LineOfSightRequests[request.requestID]->SetLastUpdateFrame(m_ImageGenerator.GetFrameNumber());
  }

  CLineOfSightVectorRequestEntityExtendedHandler requestHandler(request);
  CLineOfSightRequestHandler* activeRequest = &requestHandler;
  if (isFromHost)
  {
    activeRequest = m_LineOfSightRequests.at(request.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      LineOfSightUpdateRequests.erase(request.requestID);
      m_LineOfSightRequests.erase(request.requestID);
    }
    return false;
  }

  return true;
}

bool sbio::cigi::ig::CCigiTerrainHandler::Handle(const sbio::cigi::SHATHOTGlobalRequest& req, bool isFromHost)
{
  LogHostTerrainRequest(req, isFromHost);

  bool result = CheckHost(req, HATHOTUpdateRequests, HATHOTRequests, isFromHost);

  CHATHOTGlobalRequestHandler requestHandler(req);
  CHATHOTRequestHandler* activeRequest = &requestHandler;
  if (isFromHost)
  {
    activeRequest = HATHOTRequests.at(req.requestID).get();
  }

  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      HATHOTUpdateRequests.erase(req.requestID);
      HATHOTRequests.erase(req.requestID);
    }
    return false;
  }

  return result;
}

bool sbio::cigi::ig::CCigiTerrainHandler::Handle(const sbio::cigi::SHATHOTEntityRequest& req, bool isFromHost)
{
  LogHostTerrainRequest(req, isFromHost);

  bool result = CheckHost(req, HATHOTUpdateRequests, HATHOTRequests, isFromHost);

  CHATHOTEntityRequestHandler requestHandler(req);
  CHATHOTRequestHandler* activeRequest = &requestHandler;
  if (isFromHost)
  {
    activeRequest = HATHOTRequests.at(req.requestID).get();
  }
  if (!activeRequest->Handle())
  {
    if (isFromHost)
    {
      HATHOTUpdateRequests.erase(req.requestID);
      HATHOTRequests.erase(req.requestID);
    }
    return false;
  }

  return result;
}

void sbio::cigi::ig::CCigiTerrainHandler::Handle(const sbio::cigi::STopLevelEntityPosition& topLevelEntityPosition)
{
  auto* pEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(topLevelEntityPosition.entityID));
  if (pEntity == nullptr || pEntity->IsChild())
  {
    return;
  }

  // Clamping is performed by the engine through the entity transform message, not a host HAT/HOT query.
  TCigiBodyEulerRotation rotation;
  rotation.yaw = topLevelEntityPosition.rotation.yaw;
  rotation.pitch = topLevelEntityPosition.rotation.pitch;
  rotation.roll = topLevelEntityPosition.rotation.roll;
  pEntity->SetTopLevelTransform(topLevelEntityPosition.geodeticCoordinates, rotation);
  pEntity->SetClamp(topLevelEntityPosition.eClamp);
  pEntity->SendUpdateMessage();
}

void sbio::cigi::ig::CCigiTerrainHandler::Update(FrameNumber lastHostFrameNumber)
{
  UpdateHATHOTRequests(HATHOTUpdateRequests, HATHOTRequests, lastHostFrameNumber.Value());
  UpdateLineOfSightRequests(lastHostFrameNumber.Value());
}

void sbio::cigi::ig::CCigiTerrainHandler::Reset()
{
  m_ImageGenerator.ResetTerrainResponseDispatcher();
  HATHOTUpdateRequests.clear();
  HATHOTRequests.clear();
  LineOfSightUpdateRequests.clear();
  m_LineOfSightRequests.clear();
}

bool CCigiTerrainHandler::ResolvePoint(const sbio::math::SGeodeticCoordinates& coordinates, const sbio::math::Vec3& offset, sbio::ETopLevelCoordinateSystem coordinateSystem,
                                       sbio::EntityID entityID, GeocentricCoordinates& point)
{
  if (coordinateSystem == sbio::ETopLevelCoordinateSystem::GEODETIC)
  {
    point = ConvertCigiGeodeticToWorldCoordinates(coordinates);
  }
  else
  {
    auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(entityID);
    if (entity == nullptr)
    {
      std::stringstream ss;
      ss << "TerrainHandler: Entity " << entityID.Value() << " does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
      return false;
    }

    // Convert the offset from body coordinates to world coordinates, then add that offset to the entity's world position to get the final point
    auto EntityWorldTransform = entity->GetWorldTransform();
    auto EntityGeocPos = EntityWorldTransform.pos;
    auto offsetENU = ConvertCigiBodyCoordinatesToBodyCoordinates(CigiBodyCoordinates(offset));
    auto worldOffset = EntityWorldTransform.rotation * offsetENU;
    point = EntityGeocPos + worldOffset;
  }
  return true;
}

inline uint8_t sbio::cigi::ig::CCigiTerrainHandler::GetHostFrameNumberLSN(FrameNumber frameNumber)
{
  return ((uint8_t*)&frameNumber)[0];
}

void sbio::cigi::ig::CCigiTerrainHandler::GetAzimuthAndElevation(const GeocentricCoordinates& normal, const sbio::math::SGeodeticCoordinates& coord, Degrees180& azimuth,
                                                                 Degrees90& elevation)
{
  math::Vec3 normalReferencePlane = ConvertGeocentricToReferencePlaneCoordinates(normal, coord).toVec3().normalized();
  math::Vec3 up(0, 0, -1);
  math::Vec3 projU = normalReferencePlane - normalReferencePlane.dot(up) * up;
  azimuth = Degrees180(sbio::math::RadiansToDegrees(Radians((float)acos(projU.dot(math::Vec3(1, 0, 0))))).Value());
  elevation = Degrees90(sbio::math::RadiansToDegrees(Radians((float)acos(projU.dot(normalReferencePlane)))).Value());
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
