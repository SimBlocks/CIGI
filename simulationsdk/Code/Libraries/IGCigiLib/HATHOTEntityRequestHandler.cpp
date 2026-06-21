//Copyright SimBlocks LLC 2016-2026
#include "HATHOTEntityRequestHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "IGCigiLib.h"
#include "CigiLib/CigiConversions.h"
#include "EntityLib/EntityManager.h"
#include "EntityLib/Entity.h"
#include "MathLib/CoordinateConversions.h"
#include "UtilitiesLib/Logger.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::math;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CHATHOTEntityRequestHandler::CHATHOTEntityRequestHandler(const sbio::cigi::SHATHOTEntityRequest& request) : CHATHOTRequestHandler(), m_Request(request)
{
}

sbio::ETopLevelCoordinateSystem CHATHOTEntityRequestHandler::GetCoordinateSystem() const
{
  return ETopLevelCoordinateSystem::ENTITY;
}

sbio::EntityID CHATHOTEntityRequestHandler::GetEntityID() const
{
  return m_Request.entityID;
}

sbio::math::SGeodeticCoordinates CHATHOTEntityRequestHandler::GetGeodeticCoordinates() const
{
  return {};
}

sbio::math::Vec3 CHATHOTEntityRequestHandler::GetOffset() const
{
  return m_Request.offset;
}

bool CHATHOTEntityRequestHandler::Handle()
{
  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CHATHOTEntityRequestHandler::Handle: IGCigiLib entity manager is not initialized.");
    }
    return false;
  }

  auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(m_Request.entityID);
  if (entity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CHATHOTEntityRequestHandler::Handle: Entity " << m_Request.entityID.Value() << " does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
    }
    return false;
  }

  auto entityWorldTransform = entity->GetWorldTransform();
  auto offsetENU = ConvertCigiBodyCoordinatesToBodyCoordinates(CigiBodyCoordinates(m_Request.offset));
  auto worldOffset = entityWorldTransform.rotation * offsetENU;
  auto point = entityWorldTransform.pos + worldOffset;

  return SendRequest(sbio::cigi::ig::ConvertCigiWorldToGeodeticCoordinates(point));
}

const sbio::cigi::SBaseHATHOTRequest& CHATHOTEntityRequestHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SBaseHATHOTRequest& CHATHOTEntityRequestHandler::GetRequestRef()
{
  return m_Request;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
