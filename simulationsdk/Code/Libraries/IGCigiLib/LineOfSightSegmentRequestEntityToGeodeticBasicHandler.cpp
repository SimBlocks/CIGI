//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightSegmentRequestEntityToGeodeticBasicHandler.h"
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

CLineOfSightSegmentRequestEntityToGeodeticBasicHandler::CLineOfSightSegmentRequestEntityToGeodeticBasicHandler(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& request) : CLineOfSightRequestHandler(), m_Request(request)
{
}

bool CLineOfSightSegmentRequestEntityToGeodeticBasicHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestEntityToGeodeticHandler::Handle: IGCigiLib event messenger is not initialized.");
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

bool CLineOfSightSegmentRequestEntityToGeodeticBasicHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestEntityToGeodeticHandler::ResolveStartPoint: IGCigiLib entity manager is not initialized.");
    }
    return false;
  }

  auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(m_Request.sourceEntityID);
  if (entity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CLineOfSightSegmentRequestEntityToGeodeticHandler::ResolveStartPoint: Entity " << m_Request.sourceEntityID.Value() << " does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
    }
    return false;
  }

  auto EntityWorldTransform = entity->GetWorldTransform();
  auto EntityGeocPos = EntityWorldTransform.pos;
  auto offsetENU = ConvertCigiBodyCoordinatesToBodyCoordinates(m_Request.sourceOffset);
  auto worldOffset = EntityWorldTransform.rotation * offsetENU;
  point = EntityGeocPos + worldOffset;
  return true;
}

const sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestEntityToGeodeticBasicHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestEntityToGeodeticBasicHandler::GetRequestRef()
{
  return m_Request;
}

bool CLineOfSightSegmentRequestEntityToGeodeticBasicHandler::ResolveEndPoint(GeocentricCoordinates& point)
{
  point = ConvertCigiGeodeticToWorldCoordinates(m_Request.destinationGeodeticCoordinates);
  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
