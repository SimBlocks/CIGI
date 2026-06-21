//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightVectorRequestEntityBasicHandler.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/IGCigiLib.h"
#include "CigiLib/CigiConversions.h"
#include "MathLib/Math.h"
#include "UtilitiesLib/Logger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CLineOfSightVectorRequestEntityBasicHandler::CLineOfSightVectorRequestEntityBasicHandler(const sbio::cigi::SLineOfSightVectorRequestEntityBasic& lineOfSightRequest) : CLineOfSightRequestHandler(), m_Request(lineOfSightRequest)
{
}

bool CLineOfSightVectorRequestEntityBasicHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightVectorRequestEntityBasicHandler::Handle: IGCigiLib event messenger is not initialized.");
    }
    return false;
  }

  SLineOfSightVectorRequestBasicMessage data;
  data.LosID = m_Request.requestID;
  data.AlphaThreshold = m_Request.nAlphaThreshold / 255.f;
  data.MaterialMask = m_Request.nMaterialMask;

  if (!ResolveStartPoint(data.Start))
  {
    return false;
  }

  const auto rotation = GetRotation();

  Quaternion4d vectorDir;
  vectorDir.setIdentity();
  vectorDir = vectorDir * AxisRotation(DegreesToRadians(Degrees(-m_Request.azimuth.Value())).Value(), Vec3::UnitZ());
  vectorDir = vectorDir * AxisRotation(DegreesToRadians(Degrees(m_Request.elevation.Value())).Value(), Vec3::UnitX());

  const auto direction = GeocentricCoordinates(rotation * (vectorDir * Vec3::UnitY()));
  const auto origin = data.Start;
  data.Start = origin + direction * m_Request.fMinimumRange;
  data.End = origin + direction * m_Request.fMaximumRange;

  g_CigiLibGlobals.pEventMessenger->SendLineOfSightVectorRequestBasicMessage(data);

  return true;
}

const sbio::cigi::SLineOfSightRequest& CLineOfSightVectorRequestEntityBasicHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightVectorRequestEntityBasicHandler::GetRequestRef()
{
  return m_Request;
}

sbio::math::Quaternion4d CLineOfSightVectorRequestEntityBasicHandler::GetRotation() const
{
  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightVectorRequestEntityHandler::GetRotation: IGCigiLib entity manager is not initialized.");
    }
    Quaternion4d rotation;
    rotation.setIdentity();
    return rotation;
  }

  CEntity* pEntity = g_CigiLibGlobals.pEntityManager->GetEntity(m_Request.sourceEntityID);

  if (pEntity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CLineOfSightVectorRequestEntityHandler::GetRotation: Entity " << m_Request.sourceEntityID.Value() << " does not exist." << std::endl;
      g_CigiLibGlobals.pLogger->LogInformation(ss);
    }
    Quaternion4d rotation;
    rotation.setIdentity();
    return rotation;
  }

  return pEntity->GetWorldTransform().rotation;
}

bool CLineOfSightVectorRequestEntityBasicHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  if (g_CigiLibGlobals.pEntityManager == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightVectorRequestEntityHandler::ResolveStartPoint: IGCigiLib entity manager is not initialized.");
    }
    return false;
  }

  auto entity = g_CigiLibGlobals.pEntityManager->GetEntity(m_Request.sourceEntityID);
  if (entity == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "CLineOfSightVectorRequestEntityHandler::ResolveStartPoint: Entity " << m_Request.sourceEntityID.Value() << " does not exist." << std::endl;
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

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
