//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightSegmentRequestEntityToEntityExtendedHandler.h"
#include "MathLib/CoordinateConversions.h"
#include "EntityLib/EntityManager.h"
#include "EntityLib/Entity.h"
#include "UtilitiesLib/Logger.h"
#include "IGCigiLib.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CLineOfSightSegmentRequestEntityToEntityExtendedHandler::CLineOfSightSegmentRequestEntityToEntityExtendedHandler(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& request) : CLineOfSightRequestHandler(), m_Request(request)
{
}

bool CLineOfSightSegmentRequestEntityToEntityExtendedHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestEntityToEntityExtendedHandler::Handle: IGCigiLib event messenger is not initialized.");
    }
    return false;
  }

  SLineOfSightSegmentRequestExtendedMessage data;
  data.LosID = m_Request.requestID;
  data.AlphaThreshold = m_Request.nAlphaThreshold / 255.f;
  data.MaterialMask = m_Request.nMaterialMask;
  data.eResponseCoordinateSystem = m_Request.eResponseCoordinateSystem;

  if (!ResolveStartPoint(data.Start))
  {
    return false;
  }

  if (!ResolveEndPoint(data.End))
  {
    return false;
  }

  g_CigiLibGlobals.pEventMessenger->SendLineOfSightSegmentRequestExtendedMessage(data);

  return true;
}

const sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestEntityToEntityExtendedHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestEntityToEntityExtendedHandler::GetRequestRef()
{
  return m_Request;
}

bool CLineOfSightSegmentRequestEntityToEntityExtendedHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestEntityToEntityHandler::ResolveStartPoint: IGCigiLib entity manager is not initialized.");
    }
    return false;
  }

  auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(m_Request.sourceEntityID);
  if (entity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CLineOfSightSegmentRequestEntityToEntityHandler::ResolveStartPoint: Entity " << m_Request.sourceEntityID.Value() << " does not exist." << std::endl;
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

bool CLineOfSightSegmentRequestEntityToEntityExtendedHandler::ResolveEndPoint(GeocentricCoordinates& point)
{
  EntityID destinationEntityID = m_Request.destinationEntityID != m_Request.sourceEntityID ? m_Request.destinationEntityID : m_Request.sourceEntityID;

  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestEntityToEntityHandler::ResolveEndPoint: IGCigiLib entity manager is not initialized.");
    }
    return false;
  }

  auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(destinationEntityID);
  if (entity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CLineOfSightSegmentRequestEntityToEntityHandler::ResolveEndPoint: Entity " << destinationEntityID.Value() << " does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
    }
    return false;
  }

  // Apply the offset in the entity's local body coordinates, which are defined as:
  // X axis points forward, Y axis points right, and Z axis points down.
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
