//Copyright SimBlocks LLC 2016-2026
#include "LineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler.h"
#include "IGCigiLib/CigiProjectionConversions.h"
#include "MathLib/CoordinateConversions.h"
#include "IGCigiLib.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "UtilitiesLib/Logger.h"

using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::terrain;

extern SIGCigiLibGlobals g_CigiLibGlobals;

CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticExtended& request) : CLineOfSightRequestHandler(), m_Request(request)
{
}

bool CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::Handle()
{
  if (g_CigiLibGlobals.pEventMessenger == nullptr)
  {
    if (g_CigiLibGlobals.pLogger != nullptr)
    {
      g_CigiLibGlobals.pLogger->LogError("CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::Handle: IGCigiLib event messenger is not initialized.");
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

const sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::GetRequest() const
{
  return m_Request;
}

sbio::cigi::SLineOfSightRequest& CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::GetRequestRef()
{
  return m_Request;
}

bool CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::ResolveStartPoint(GeocentricCoordinates& point)
{
  point = ConvertCigiGeodeticToWorldCoordinates(m_Request.sourceGeodeticCoordinates);
  return true;
}

bool CLineOfSightSegmentRequestGeodeticToGeodeticExtendedHandler::ResolveEndPoint(GeocentricCoordinates& point)
{
  point = ConvertCigiGeodeticToWorldCoordinates(m_Request.destinationGeodeticCoordinates);
  return true;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
