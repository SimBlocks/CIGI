//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightSegmentRequestGeodeticToEntityBasicHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "MathLib/CoordinateConversions.h"
#include "EntityLib/EntityManager.h"
#include "EntityLib/Entity.h"
#include "UtilitiesLib/Logger.h"
#include "IGCigiLib.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CLineOfSightSegmentRequestGeodeticToEntityBasicHandler::CLineOfSightSegmentRequestGeodeticToEntityBasicHandler(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& request) : CLineOfSightRequestHandler(), m_Request(request)
{
}

bool CLineOfSightSegmentRequestGeodeticToEntityBasicHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestGeodeticToEntityHandler::Handle: IGCigiLib event messenger is not initialized.");
    }
    return false;
  }

  SLineOfSightSegmentRequestBasicMessage data;
  data.LosID = m_Request.requestID;
  data.AlphaThreshold = m_Request.nAlphaThreshold / 255.f;
  data.MaterialMask = m_Request.nMaterialMask;

  if (!ResolveStartPoint(data.Start))
  {
    return false;
  }

  if (!ResolveEndPoint(data.End))
  {
    return false;
  }

  g_CigiLibGlobals.pEventMessenger->SendLineOfSightSegmentRequestBasicMessage(data);

  return true;
}

bool CLineOfSightSegmentRequestGeodeticToEntityBasicHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  point = ConvertCigiGeodeticToWorldCoordinates(m_Request.sourceGeodeticCoordinates);
  return true;
}

const sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestGeodeticToEntityBasicHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestGeodeticToEntityBasicHandler::GetRequestRef()
{
  return m_Request;
}

bool CLineOfSightSegmentRequestGeodeticToEntityBasicHandler::ResolveEndPoint(GeocentricCoordinates& point)
{
  EntityID entityID = m_Request.destinationEntityID;

  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestGeodeticToEntityHandler::ResolveEndPoint: IGCigiLib entity manager is not initialized.");
    }
    return false;
  }

  auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(entityID);
  if (entity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CLineOfSightSegmentRequestGeodeticToEntityHandler::ResolveEndPoint: Entity " << entityID.Value() << " does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
    }
    return false;
  }

  // Convert the destination offset from body coordinates to geocentric coordinates, then add it to the entity's geocentric position.
  auto EntityWorldTransform = entity->GetWorldTransform();
  auto EntityGeocPos = EntityWorldTransform.pos;
  auto offsetENU = ConvertCigiBodyCoordinatesToBodyCoordinates(CigiBodyCoordinates(m_Request.destinationOffset));
  auto worldOffset = EntityWorldTransform.rotation * offsetENU;
  point = EntityGeocPos + worldOffset;
  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
