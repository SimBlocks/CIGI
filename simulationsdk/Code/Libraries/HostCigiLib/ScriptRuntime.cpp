//Copyright SimBlocks LLC 2016-2026
#include "ScriptRuntime.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostCigiLib/HostCigiLib.h"
#include "HostCigiLib/HostSession.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "ViewLib/ViewLib.h"
#include "EntityLib/EntityLib.h"
#include "MathLib/MathTypes.h"
#include <boost/algorithm/string.hpp>
#include <chaiscript/chaiscript.hpp>
#include <chaiscript/chaiscript_stdlib.hpp>
#include <chaiscript/utility/utility.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <algorithm>
#include <memory>

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::symbol;
using namespace sbio::cigi;
using namespace sbio::cigi::host;
using namespace chaiscript;

CScriptRuntime* g_pScriptRuntime = nullptr;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

chaiscript::ChaiScript::State base_state;
std::map<std::string, chaiscript::Boxed_Value> base_locals;

void registerCloudType(int nCloudType, const std::string& sCloudType)
{
  g_pScriptRuntime->RegisterCloudType(CloudType(static_cast<uint8_t>(nCloudType)), sCloudType);
}

void CScriptRuntime::OnDatabaseLoaded(const HostCigiDatabaseLoadedEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnDatabaseLoaded"))
  {
    auto pFuncOnDatabaseLoaded = m_pChaiScript->eval<std::function<void()>>("OnDatabaseLoaded");

    try
    {
      pFuncOnDatabaseLoaded();
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnDatabaseLoaded Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptHatHotResponse
{
  bool Valid = false;
  double Height = 0;
  int HatHotId = 0;
  int HostFrameLSN = 0;
};

void CScriptRuntime::OnHostCigiHatResponseEvent(const HostCigiHatResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnHATResponse"))
  {
    auto pFuncOnHatHotResponse = m_pChaiScript->eval<std::function<void(SScriptHatHotResponse&)>>("OnHATResponse");
    try
    {
      SScriptHatHotResponse hatHotResponse;
      hatHotResponse.Valid = args.hatResponse.bValid;
      hatHotResponse.Height = args.hatResponse.heightAboveTerrain;
      hatHotResponse.HatHotId = args.hatResponse.HATHOTID.Value();
      hatHotResponse.HostFrameLSN = args.hatResponse.hostFrameLSN;

      pFuncOnHatHotResponse(hatHotResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnHATResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void CScriptRuntime::OnHostCigiHotResponseEvent(const HostCigiHotResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnHOTResponse"))
  {
    auto pFuncOnHatHotResponse = m_pChaiScript->eval<std::function<void(SScriptHatHotResponse&)>>("OnHOTResponse");
    try
    {
      SScriptHatHotResponse hatHotResponse;
      hatHotResponse.Valid = args.hotResponse.bValid;
      hatHotResponse.Height = args.hotResponse.heightOfTerrain.Value();
      hatHotResponse.HatHotId = args.hotResponse.HATHOTID.Value();
      hatHotResponse.HostFrameLSN = args.hotResponse.hostFrameLSN;

      pFuncOnHatHotResponse(hatHotResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnHOTResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptHatHotExtendedResponse
{
  bool Valid = false;
  double Hat = 0;
  double Hot = 0;
  double NormalAzimuth = 0;
  double NormalElevation = 0;
  int HatHotId = 0;
  int HostFrameLSN = 0;
  long MaterialCode = 0;
};

void CScriptRuntime::OnHostCigiHatHotExtendedResponseEvent(const HostCigiHatHotExtendedResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnHATHOTExtendedResponse"))
  {
    auto pFuncOnHatHotExtendedResponse = m_pChaiScript->eval<std::function<void(SScriptHatHotExtendedResponse&)>>("OnHATHOTExtendedResponse");
    try
    {
      // Copy the CIGI response data into a struct that the chai script can use
      SScriptHatHotExtendedResponse hatHotExtendedResponse;
      hatHotExtendedResponse.Valid = args.hatHotExtendedResponse.bValid;
      hatHotExtendedResponse.Hat = args.hatHotExtendedResponse.heightAboveTerrain;
      hatHotExtendedResponse.Hot = args.hatHotExtendedResponse.heightOfTerrain.Value();
      hatHotExtendedResponse.NormalAzimuth = args.hatHotExtendedResponse.normalVectorAzimuth.Value();
      hatHotExtendedResponse.NormalElevation = args.hatHotExtendedResponse.normalVectorElevation.Value();
      hatHotExtendedResponse.HatHotId = args.hatHotExtendedResponse.HATHOTID.Value();
      hatHotExtendedResponse.HostFrameLSN = args.hatHotExtendedResponse.hostFrameLSN;
      hatHotExtendedResponse.MaterialCode = args.hatHotExtendedResponse.materialCode.Value();

      // Call the chai script function
      pFuncOnHatHotExtendedResponse(hatHotExtendedResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnHATHOTExtendedResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptLineOfSightResponse
{
  int RequestID = 0;
  bool Valid = false;
  double Range = 0;
  int ResponseCount = 0;
  int HostFrameLSN = 0;
};

void CScriptRuntime::OnHostCigiLineOfSightResponseEvent(const HostCigiLineOfSightResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnLineOfSightResponse"))
  {
    auto pFuncOnLineOfSightResponse = m_pChaiScript->eval<std::function<void(SScriptLineOfSightResponse&)>>("OnLineOfSightResponse");
    try
    {
      // Copy the CIGI response data into a struct that the chai script can use
      SScriptLineOfSightResponse lineOfSightResponse;
      lineOfSightResponse.RequestID = args.lineOfSightResponse.lineOfSightRequestID.Value();
      lineOfSightResponse.Valid = args.lineOfSightResponse.bValid;
      lineOfSightResponse.Range = args.lineOfSightResponse.dRange;
      lineOfSightResponse.ResponseCount = args.lineOfSightResponse.responseCount;
      lineOfSightResponse.HostFrameLSN = args.lineOfSightResponse.hostFrameLSN;

      // Call the chai script function
      pFuncOnLineOfSightResponse(lineOfSightResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnLineOfSightResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptLineOfSightEntityResponse
{
  int RequestID = 0;
  int EntityID = 0;
  bool Valid = false;
  bool Visible = false;
  double Range = 0;
  int ResponseCount = 0;
  int HostFrameLSN = 0;
};

void CScriptRuntime::OnHostCigiLineOfSightEntityResponseEvent(const HostCigiLineOfSightEntityResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnLineOfSightEntityResponse"))
  {
    auto pFuncOnLineOfSightEntityResponse = m_pChaiScript->eval<std::function<void(SScriptLineOfSightEntityResponse&)>>("OnLineOfSightEntityResponse");
    try
    {
      // Copy the CIGI response data into a struct that the chai script can use
      SScriptLineOfSightEntityResponse lineOfSightEntityResponse;
      lineOfSightEntityResponse.RequestID = args.lineOfSightEntityResponse.lineOfSightRequestID.Value();
      lineOfSightEntityResponse.EntityID = args.lineOfSightEntityResponse.entityID.Value();
      lineOfSightEntityResponse.Valid = args.lineOfSightEntityResponse.bValid;
      lineOfSightEntityResponse.Visible = args.lineOfSightEntityResponse.bVisible;
      lineOfSightEntityResponse.Range = args.lineOfSightEntityResponse.dRange;
      lineOfSightEntityResponse.ResponseCount = args.lineOfSightEntityResponse.responseCount;
      lineOfSightEntityResponse.HostFrameLSN = args.lineOfSightEntityResponse.hostFrameLSN;
      // Call the chai script function
      pFuncOnLineOfSightEntityResponse(lineOfSightEntityResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnLineOfSightResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptLineOfSightExtendedGeodeticCoordinatesResponse
{
  // Standard Data
  int RequestID = 0;
  bool Valid = false;
  double Range = 0;
  int ResponseCount = 0;
  int HostFrameLSN = 0;

  // Extended Data
  bool RangeValid = false;
  float NormalAzimuth = 0;
  float NormalElevation = 0;
  int MaterialCode = 0;
  int SurfaceColorR = 0;
  int SurfaceColorG = 0;
  int SurfaceColorB = 0;
  int SurfaceColorA = 0;

  // Geodetic Coordinates
  double Latitude = 0;
  double Longitude = 0;
  double Altitude = 0;
};

struct SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse
{
  // Standard Data
  int RequestID = 0;
  bool Valid = false;
  double Range = 0;
  int ResponseCount = 0;
  int HostFrameLSN = 0;

  // Extended Data
  bool RangeValid = false;
  float NormalAzimuth = 0;
  float NormalElevation = 0;
  int MaterialCode = 0;
  int SurfaceColorR = 0;
  int SurfaceColorG = 0;
  int SurfaceColorB = 0;
  int SurfaceColorA = 0;

  // entity with Geodetic Coordinates data
  int EntityID = 0;
  bool Visible = false;
  double Latitude = 0;
  double Longitude = 0;
  double Altitude = 0;
};

struct SScriptLineOfSightExtendedEntityCoordinatesResponse
{
  // Standard Data
  int RequestID = 0;
  bool Valid = false;
  double Range = 0;
  int ResponseCount = 0;
  int HostFrameLSN = 0;

  // Extended Data
  bool RangeValid = false;
  float NormalAzimuth = 0;
  float NormalElevation = 0;
  int MaterialCode = 0;
  int SurfaceColorR = 0;
  int SurfaceColorG = 0;
  int SurfaceColorB = 0;
  int SurfaceColorA = 0;

  // entity with offset data
  int EntityID = 0;
  bool Visible = false;
  double X = 0;
  double Y = 0;
  double Z = 0;
};

template <typename TScriptResponse, typename TResponse>
void CopyLineOfSightExtendedResponseBase(TScriptResponse& destination, const TResponse& source)
{
  destination.RequestID = source.lineOfSightRequestID.Value();
  destination.Valid = source.bValid;
  destination.Range = source.dRange;
  destination.ResponseCount = source.responseCount;
  destination.HostFrameLSN = source.hostFrameLSN;
  destination.RangeValid = source.bRangeValid;
  destination.NormalAzimuth = source.fNormalVectorAzimuth;
  destination.NormalElevation = source.fNormalVectorElevation;
  destination.MaterialCode = source.materialCode.Value();
  destination.SurfaceColorR = source.surfaceColor.r;
  destination.SurfaceColorG = source.surfaceColor.g;
  destination.SurfaceColorB = source.surfaceColor.b;
  destination.SurfaceColorA = source.surfaceColor.a;
}

void CScriptRuntime::OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedGeodeticCoordinatesResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnLineOfSightExtendedGeodeticCoordinatesResponse"))
  {
    auto pFuncOnLineOfSightExtendedGeodeticCoordinatesResponse = m_pChaiScript->eval<std::function<void(SScriptLineOfSightExtendedGeodeticCoordinatesResponse&)>>("OnLineOfSightExtendedGeodeticCoordinatesResponse");
    try
    {
      SScriptLineOfSightExtendedGeodeticCoordinatesResponse lineOfSightExtendedGeodeticCoordinatesResponse;
      CopyLineOfSightExtendedResponseBase(lineOfSightExtendedGeodeticCoordinatesResponse, args.lineOfSightExtendedGeodeticCoordinatesResponse);
      lineOfSightExtendedGeodeticCoordinatesResponse.Latitude = args.lineOfSightExtendedGeodeticCoordinatesResponse.geodeticCoordinates.latitude.Value();
      lineOfSightExtendedGeodeticCoordinatesResponse.Longitude = args.lineOfSightExtendedGeodeticCoordinatesResponse.geodeticCoordinates.longitude.Value();
      lineOfSightExtendedGeodeticCoordinatesResponse.Altitude = args.lineOfSightExtendedGeodeticCoordinatesResponse.geodeticCoordinates.altitude.Value();

      pFuncOnLineOfSightExtendedGeodeticCoordinatesResponse(lineOfSightExtendedGeodeticCoordinatesResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnHostCigiLineOfSightExtendedGeodeticCoordinatesResponseEvent Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void CScriptRuntime::OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnLineOfSightExtendedEntityGeodeticCoordinatesResponse"))
  {
    auto pFuncOnLineOfSightExtendedEntityGeodeticCoordinatesResponse = m_pChaiScript->eval<std::function<void(SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse&)>>("OnLineOfSightExtendedEntityGeodeticCoordinatesResponse");
    try
    {
      SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse lineOfSightExtendedEntityGeodeticCoordinatesResponse;
      CopyLineOfSightExtendedResponseBase(lineOfSightExtendedEntityGeodeticCoordinatesResponse, args.lineOfSightExtendedEntityGeodeticCoordinatesResponse);
      lineOfSightExtendedEntityGeodeticCoordinatesResponse.EntityID = args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.entityID.Value();
      lineOfSightExtendedEntityGeodeticCoordinatesResponse.Visible = args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.bVisible;
      lineOfSightExtendedEntityGeodeticCoordinatesResponse.Latitude = args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.geodeticCoordinates.latitude.Value();
      lineOfSightExtendedEntityGeodeticCoordinatesResponse.Longitude = args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.geodeticCoordinates.longitude.Value();
      lineOfSightExtendedEntityGeodeticCoordinatesResponse.Altitude = args.lineOfSightExtendedEntityGeodeticCoordinatesResponse.geodeticCoordinates.altitude.Value();

      pFuncOnLineOfSightExtendedEntityGeodeticCoordinatesResponse(lineOfSightExtendedEntityGeodeticCoordinatesResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnHostCigiLineOfSightExtendedEntityGeodeticCoordinatesResponseEvent Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void CScriptRuntime::OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent(const HostCigiLineOfSightExtendedEntityCoordinatesResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnLineOfSightExtendedEntityCoordinatesResponse"))
  {
    auto pFuncOnLineOfSightExtendedEntityCoordinatesResponse = m_pChaiScript->eval<std::function<void(SScriptLineOfSightExtendedEntityCoordinatesResponse&)>>("OnLineOfSightExtendedEntityCoordinatesResponse");
    try
    {
      SScriptLineOfSightExtendedEntityCoordinatesResponse lineOfSightExtendedEntityCoordinatesResponse;
      CopyLineOfSightExtendedResponseBase(lineOfSightExtendedEntityCoordinatesResponse, args.lineOfSightExtendedEntityCoordinatesResponse);
      lineOfSightExtendedEntityCoordinatesResponse.EntityID = args.lineOfSightExtendedEntityCoordinatesResponse.entityID.Value();
      lineOfSightExtendedEntityCoordinatesResponse.Visible = args.lineOfSightExtendedEntityCoordinatesResponse.bVisible;
      lineOfSightExtendedEntityCoordinatesResponse.X = args.lineOfSightExtendedEntityCoordinatesResponse.offset[0];
      lineOfSightExtendedEntityCoordinatesResponse.Y = args.lineOfSightExtendedEntityCoordinatesResponse.offset[1];
      lineOfSightExtendedEntityCoordinatesResponse.Z = args.lineOfSightExtendedEntityCoordinatesResponse.offset[2];
      pFuncOnLineOfSightExtendedEntityCoordinatesResponse(lineOfSightExtendedEntityCoordinatesResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnHostCigiLineOfSightExtendedEntityCoordinatesResponseEvent Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptCollisionSegmentResponse
{
  int EntityID = 0;
  int SegmentID = 0;
  float IntersectionDistance = 0;
  int MaterialCode = 0;
};

struct SScriptCollisionSegmentEntityResponse
{
  int EntityID = 0;
  int SegmentID = 0;
  float IntersectionDistance = 0;
  int MaterialCode = 0;
  int ContactedEntityID = 0;
};

void CScriptRuntime::OnHostCigiCollisionDetectionSegmentNotificationEvent(const HostCigiCollisionDetectionSegmentNotificationEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnCollisionSegmentResponse"))
  {
    auto pFuncOnCollisionSegmentResponse = m_pChaiScript->eval<std::function<void(SScriptCollisionSegmentResponse&)>>("OnCollisionSegmentResponse");
    try
    {
      // Copy the CIGI response data into a struct that the chai script can use
      SScriptCollisionSegmentResponse collisionSegmentResponse;
      collisionSegmentResponse.EntityID = args.collisionDetectionSegmentNotification.entityID.Value();
      collisionSegmentResponse.SegmentID = args.collisionDetectionSegmentNotification.segmentID.Value();
      collisionSegmentResponse.IntersectionDistance = args.collisionDetectionSegmentNotification.fIntersectionDistance;
      collisionSegmentResponse.MaterialCode = args.collisionDetectionSegmentNotification.materialCode.Value();

      // Call the chai script function
      pFuncOnCollisionSegmentResponse(collisionSegmentResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnCollisionSegmentResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void CScriptRuntime::OnHostCigiCollisionDetectionSegmentEntityNotificationEvent(const HostCigiCollisionDetectionSegmentEntityNotificationEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnCollisionSegmentEntityResponse"))
  {
    auto pFuncOnCollisionSegmentEntityResponse = m_pChaiScript->eval<std::function<void(SScriptCollisionSegmentEntityResponse&)>>("OnCollisionSegmentEntityResponse");
    try
    {
      SScriptCollisionSegmentEntityResponse collisionSegmentResponse;
      collisionSegmentResponse.EntityID = args.collisionDetectionSegmentEntityNotification.entityID.Value();
      collisionSegmentResponse.SegmentID = args.collisionDetectionSegmentEntityNotification.segmentID.Value();
      collisionSegmentResponse.IntersectionDistance = args.collisionDetectionSegmentEntityNotification.fIntersectionDistance;
      collisionSegmentResponse.MaterialCode = args.collisionDetectionSegmentEntityNotification.materialCode.Value();
      collisionSegmentResponse.ContactedEntityID = args.collisionDetectionSegmentEntityNotification.contactedEntityID.Value();

      pFuncOnCollisionSegmentEntityResponse(collisionSegmentResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnCollisionSegmentEntityResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptCollisionVolumeResponse
{
  int EntityID = 0;
  int VolumeID = 0;
  int ContactedVolumeID = 0;
};

struct SScriptCollisionVolumeEntityResponse
{
  int EntityID = 0;
  int VolumeID = 0;
  int ContactedEntityID = 0;
  int ContactedVolumeID = 0;
};

void CScriptRuntime::OnHostCigiCollisionDetectionVolumeNotificationEvent(const HostCigiCollisionDetectionVolumeNotificationEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnCollisionVolumeResponse"))
  {
    auto pFuncOnCollisionVolumeResponse = m_pChaiScript->eval<std::function<void(SScriptCollisionVolumeResponse&)>>("OnCollisionVolumeResponse");
    try
    {
      // Copy the CIGI response data into a struct that the chai script can use
      SScriptCollisionVolumeResponse collisionVolumeResponse;
      collisionVolumeResponse.EntityID = args.collisionDetectionVolumeNotification.entityID.Value();
      collisionVolumeResponse.VolumeID = args.collisionDetectionVolumeNotification.volumeID.Value();
      collisionVolumeResponse.ContactedVolumeID = args.collisionDetectionVolumeNotification.contactedVolumeID.Value();

      // Call the chai script function
      pFuncOnCollisionVolumeResponse(collisionVolumeResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnCollisionVolumeResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void CScriptRuntime::OnHostCigiCollisionDetectionVolumeEntityNotificationEvent(const HostCigiCollisionDetectionVolumeEntityNotificationEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnCollisionVolumeEntityResponse"))
  {
    auto pFuncOnCollisionVolumeEntityResponse = m_pChaiScript->eval<std::function<void(SScriptCollisionVolumeEntityResponse&)>>("OnCollisionVolumeEntityResponse");
    try
    {
      SScriptCollisionVolumeEntityResponse collisionVolumeResponse;
      collisionVolumeResponse.EntityID = args.collisionDetectionVolumeEntityNotification.entityID.Value();
      collisionVolumeResponse.VolumeID = args.collisionDetectionVolumeEntityNotification.volumeID.Value();
      collisionVolumeResponse.ContactedEntityID = args.collisionDetectionVolumeEntityNotification.contactedEntityID.Value();
      collisionVolumeResponse.ContactedVolumeID = args.collisionDetectionVolumeEntityNotification.contactedVolumeID.Value();

      pFuncOnCollisionVolumeEntityResponse(collisionVolumeResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnCollisionVolumeEntityResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

struct SScriptPositionResponseGeodeticCoordinates
{
  double Latitude = 0;
  double Longitude = 0;
  double Altitude = 0;
  string ObjectClass;
  int objectID = 0;
  float Roll = 0;
  float Pitch = 0;
  float Yaw = 0;
};

struct SScriptPositionResponseParentEntityCoordinates
{
  string ObjectClass;
  int objectID = 0;
  float Roll = 0;
  float Pitch = 0;
  float Yaw = 0;
  float xOffset = 0;
  float yOffset = 0;
  float zOffset = 0;
};

struct SScriptPositionResponseArticulatedPartCoordinates
{
  string ObjectClass;
  int objectID = 0;
  float Roll = 0;
  float Pitch = 0;
  float Yaw = 0;
  int ArticulatedPartID = 0;
  float xOffset = 0;
  float yOffset = 0;
  float zOffset = 0;
};

void CScriptRuntime::OnHostCigiPositionResponseEvent(const HostCigiPositionResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");

  if (args.ePositionResponseType == EPositionResponseType::GEODETIC && pFunctionExists("OnPositionResponseGeodetic"))
  {
    auto pFuncOnPositionResponseGeodetic = m_pChaiScript->eval<std::function<void(SScriptPositionResponseGeodeticCoordinates&)>>("OnPositionResponseGeodetic");

    try
    {
      SScriptPositionResponseGeodeticCoordinates scriptPositionResponse;
      SPositionResponseGeodeticCoordinates& positionResponse = static_cast<SPositionResponseGeodeticCoordinates&>(args.positionResponse);
      scriptPositionResponse.Latitude = positionResponse.geodeticCoordinates.latitude.Value();
      scriptPositionResponse.Longitude = positionResponse.geodeticCoordinates.longitude.Value();
      scriptPositionResponse.Altitude = positionResponse.geodeticCoordinates.altitude.Value();
      scriptPositionResponse.ObjectClass = ConvertCigiObjectClassToString(positionResponse.eObjectClass);
      scriptPositionResponse.objectID = positionResponse.objectID;
      scriptPositionResponse.Yaw = positionResponse.rotation.yaw.Value();
      scriptPositionResponse.Pitch = positionResponse.rotation.pitch.Value();
      scriptPositionResponse.Roll = positionResponse.rotation.roll.Value();

      pFuncOnPositionResponseGeodetic(scriptPositionResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnPositionResponseGeodetic Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
  else if (args.ePositionResponseType == EPositionResponseType::PARENT && pFunctionExists("OnPositionResponseParentEntity"))
  {
    auto pFuncOnPositionResponseParentEntity = m_pChaiScript->eval<std::function<void(SScriptPositionResponseParentEntityCoordinates&)>>("OnPositionResponseParentEntity");

    try
    {
      SScriptPositionResponseParentEntityCoordinates scriptPositionResponse;
      SPositionResponseParentEntityCoordinates& positionResponse = static_cast<SPositionResponseParentEntityCoordinates&>(args.positionResponse);
      scriptPositionResponse.xOffset = (float)positionResponse.offset[0];
      scriptPositionResponse.yOffset = (float)positionResponse.offset[1];
      scriptPositionResponse.zOffset = (float)positionResponse.offset[2];
      scriptPositionResponse.ObjectClass = ConvertCigiObjectClassToString(positionResponse.eObjectClass);
      scriptPositionResponse.objectID = positionResponse.objectID;
      scriptPositionResponse.Yaw = positionResponse.rotation.yaw.Value();
      scriptPositionResponse.Pitch = positionResponse.rotation.pitch.Value();
      scriptPositionResponse.Roll = positionResponse.rotation.roll.Value();

      pFuncOnPositionResponseParentEntity(scriptPositionResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnPositionResponseParentEntity Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
  else if (args.ePositionResponseType == EPositionResponseType::ARTICULATED && pFunctionExists("OnPositionResponseArticulatedPart"))
  {
    auto pFuncOnPositionResponseArticulatedPart = m_pChaiScript->eval<std::function<void(SScriptPositionResponseArticulatedPartCoordinates&)>>("OnPositionResponseArticulatedPart");

    try
    {
      SScriptPositionResponseArticulatedPartCoordinates scriptPositionResponse;
      SPositionResponseArticulatedPartCoordinates& positionResponse = static_cast<SPositionResponseArticulatedPartCoordinates&>(args.positionResponse);
      scriptPositionResponse.xOffset = static_cast<float>(positionResponse.offset[0]);
      scriptPositionResponse.yOffset = static_cast<float>(positionResponse.offset[1]);
      scriptPositionResponse.zOffset = static_cast<float>(positionResponse.offset[2]);
      scriptPositionResponse.ObjectClass = ConvertCigiObjectClassToString(positionResponse.eObjectClass);
      scriptPositionResponse.objectID = positionResponse.objectID;
      scriptPositionResponse.Yaw = positionResponse.rotation.yaw.Value();
      scriptPositionResponse.Pitch = positionResponse.rotation.pitch.Value();
      scriptPositionResponse.Roll = positionResponse.rotation.roll.Value();
      scriptPositionResponse.ArticulatedPartID = positionResponse.articulatedPartID.Value();

      pFuncOnPositionResponseArticulatedPart(scriptPositionResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnPositionResponseArticulatedPart Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void CScriptRuntime::RegisterCloudType(sbio::CloudType eCloudType, const std::string& sCloudType)
{
  m_CloudTypes.push_back(sCloudType);
  m_CloudTypeValues.insert(std::make_pair(sCloudType, eCloudType));
}

void registerComponentClass(uint8_t nValue, const string& sClass)
{
  g_pScriptRuntime->RegisterComponentClass(CigiComponentClassID(nValue), sClass);
}

void CScriptRuntime::RegisterComponentClass(sbio::cigi::CigiComponentClassID componentClassID, const std::string& sComponentClass)
{
  m_ComponentClasses.push_back(sComponentClass);
  m_ComponentClassValues.insert(std::make_pair(sComponentClass, componentClassID));
}

void registerLayerID(int nLayerID, const std::string& sLayerID)
{
  g_pScriptRuntime->RegisterLayerID(nLayerID, sLayerID);
}

void registerSurfaceConditionID(uint16_t nLSurfaceConditionID, const std::string& sSurfaceConditionName)
{
  g_pScriptRuntime->RegisterSurfaceConditionID(SurfaceConditionID(nLSurfaceConditionID), sSurfaceConditionName);
}

void registerPixelReplicationMode(uint8_t nPixelReplicationID, const std::string& sPixelReplicationName)
{
  g_pScriptRuntime->RegisterPixelReplicationMode(PixelReplicationMode(nPixelReplicationID), sPixelReplicationName);
}

void registerFontID(uint8_t nFontID, const std::string& sCategory, const std::string& sFontName)
{
  g_pScriptRuntime->RegisterFontID(FontID(nFontID), sCategory, sFontName);
}

void registerImageGeneratorMessage(uint8_t nMessageID, const std::string& sMessage)
{
  g_pScriptRuntime->RegisterImageGeneratorMessage(nMessageID, sMessage);
}

const std::list<std::string>& CScriptRuntime::GetLayers() const
{
  return m_Layers;
}

bool CScriptRuntime::IsScriptRunning() const
{
  return m_bIsExecutingScript;
}

void CScriptRuntime::RegisterLayerID(int nLayerID, const std::string& sLayerID)
{
  m_Layers.push_back(sLayerID);
  m_LayerValues.insert(std::make_pair(sLayerID, nLayerID));
}

void CScriptRuntime::RegisterSurfaceConditionID(SurfaceConditionID surfaceConditionID, const std::string& sSurfaceConditionName)
{
  m_SurfaceConditions.push_back(sSurfaceConditionName);
  m_SurfaceConditionValues.insert(std::make_pair(sSurfaceConditionName, surfaceConditionID));
}

void CScriptRuntime::RegisterPixelReplicationMode(PixelReplicationMode pixelReplicationID, const std::string& sPixelReplicationName)
{
  m_PixelReplicationModes.push_back(sPixelReplicationName);
  m_PixelReplicationModeValues.insert(std::make_pair(sPixelReplicationName, pixelReplicationID));
}

void CScriptRuntime::RegisterFontID(sbio::FontID fontID, const std::string& sCategory, const std::string& sFontName)
{
  SFontCategoryID font;
  font.fontID = fontID;
  font.sCategory = sCategory;
  font.sFontName = sFontName;

  m_Fonts.push_back(font);
}

void CScriptRuntime::RegisterImageGeneratorMessage(int nMessageID, const std::string& sMessage)
{
  m_ImageGeneratorMessages[nMessageID] = sMessage;
}

void CScriptRuntime::SendReset()
{
  m_WaitCallbacks.clear();

  SCigiIgControl cigiIGControl;
  cigiIGControl.databaseNumber = CigiDatabaseNumber(0);
  cigiIGControl.bEntityTypeSubstitutionEnabled = false;
  cigiIGControl.eIgMode = sbio::cigi::EIGMode::RESET;
  cigiIGControl.bSmoothingEnabled = false;

  if (g_HostCigiLibGlobals.pHost == nullptr)
  {
    return;
  }

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  if (pHostSession == nullptr)
  {
    return;
  }

  pHostSession->SetIGControl(cigiIGControl.databaseNumber, cigiIGControl.bEntityTypeSubstitutionEnabled, cigiIGControl.eIgMode, cigiIGControl.bSmoothingEnabled);
}

void CScriptRuntime::Stop()
{
  m_bIsExecutingScript = false;
  m_WaitCallbacks.clear();

  if (m_pChaiScript != nullptr)
  {
    m_pChaiScript->set_state(base_state);
    m_pChaiScript->set_locals(base_locals);
    m_pChaiScript->eval("def function_exists(String name) { return contains(get_function_objects(), fun(elem){ return elem.first == name; }); }");
  }

  SendReset();
}

void CScriptRuntime::Update()
{
  if (m_pChaiScript == nullptr)
  {
    return;
  }

  if (!m_bIsExecutingScript)
  {
    return;
  }

  if (g_HostCigiLibGlobals.pHost == nullptr)
  {
    m_bIsExecutingScript = false;
    return;
  }

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  if (pHostSession == nullptr)
  {
    m_bIsExecutingScript = false;
    return;
  }

  double fCurrentSessionTime = pHostSession->GetSessionTime();

  //check for any wait callbacks
  auto it = m_WaitCallbacks.begin();
  while (it != m_WaitCallbacks.end())
  {
    if (it->fTime < fCurrentSessionTime)
    {
      auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");

      if (!pFunctionExists(it->sCallback))
      {
        HostCigiErrorEventArgs args;
        stringstream ss;
        ss << "Script function " << it->sCallback << " does not exist!\n";
        args.sError = ss.str();
        Event::Raise<HostCigiEvent>(args);

        m_bIsExecutingScript = false;
        return;
      }

      auto pCallbackFunction = m_pChaiScript->eval<std::function<void()>>(it->sCallback);
      try
      {
        pCallbackFunction();
      }
      catch (const chaiscript::exception::eval_error& e)
      {
        HostCigiErrorEventArgs args;
        stringstream ss;
        ss << "Script " << it->sCallback << " Error\n" << e.pretty_print() << '\n';
        args.sError = ss.str();
        Event::Raise<HostCigiEvent>(args);

        m_bIsExecutingScript = false;
        return;
      }
    }

    ++it;
  }

  m_WaitCallbacks.remove_if([fCurrentSessionTime](const SWaitCallback& waitCallback) { return waitCallback.fTime < fCurrentSessionTime; });

  //make sure database is loaded before calling OnUpdate
  if ((pHostSession->GetDatabaseState() == EHostSessionDatabaseState::NO_DATABASE) || (pHostSession->GetDatabaseState() == EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED) || (pHostSession->GetDatabaseState() == EHostSessionDatabaseState::LOADING_ACKNOWLEDGED))
  {
    return;
  }

  //call OnUpdate
  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnUpdate"))
  {
    auto pFuncOnUpdate = m_pChaiScript->eval<std::function<void()>>("OnUpdate");

    try
    {
      pFuncOnUpdate();
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs args;
      stringstream ss;
      ss << "Script OnUpdate Error\n" << e.pretty_print() << '\n';
      args.sError = ss.str();
      Event::Raise<HostCigiEvent>(args);

      m_bIsExecutingScript = false;
    }
  }
}

void addEntity(uint16_t nEntityID, uint16_t nEntityType)
{
  ShortEntityTypeID entityTypeID(nEntityType);
  SEntityControl entityControl;
  entityControl.entityID = EntityID(nEntityID);
  entityControl.eState = EActiveState::ACTIVE;
  entityControl.eExtendedEntityType = EExtendedEntityType::SHORT;
  entityControl.shortEntityTypeID = entityTypeID;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  if (pHostSession != nullptr)
  {
    pHostSession->SendEntityControl(entityControl);
  }
}

struct SScriptIGControl
{
public:
  uint8_t DatabaseNumber = 0;
  bool EntityTypeSubstitutionEnabled = false;
  string IGMode;
  bool SmoothingEnable = false;
};

void sendIGControl(SScriptIGControl igControl)
{
  SCigiIgControl cigiIGControl;
  cigiIGControl.databaseNumber = CigiDatabaseNumber(igControl.DatabaseNumber);
  cigiIGControl.bEntityTypeSubstitutionEnabled = igControl.EntityTypeSubstitutionEnabled;
  cigiIGControl.eIgMode = ConvertCigiStringToIGMode(igControl.IGMode);
  cigiIGControl.bSmoothingEnabled = igControl.SmoothingEnable;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SetIGControl(cigiIGControl.databaseNumber, cigiIGControl.bEntityTypeSubstitutionEnabled, cigiIGControl.eIgMode, cigiIGControl.bSmoothingEnabled);
}

void RegisterIGControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendIGControl), "sendIGControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptIGControl>(*m, "IGControl", {constructor<SScriptIGControl()>(), constructor<SScriptIGControl(const SScriptIGControl&)>()},
                                                   {
                                                     {fun(&SScriptIGControl::DatabaseNumber), "DatabaseNumber"},
                                                     {fun(&SScriptIGControl::EntityTypeSubstitutionEnabled), "EntityTypeSubstitutionEnabled"},
                                                     {fun(&SScriptIGControl::IGMode), "IGMode"},
                                                     {fun(&SScriptIGControl::SmoothingEnable), "SmoothingEnable"},
                                                   });

  chai.add(m);
}

struct SScriptViewDefinition
{
  uint16_t ViewID = 0;
  uint8_t ViewGroupID = 0;
  bool NearEnabled = false;
  bool FarEnabled = false;
  bool LeftEnabled = false;
  bool RightEnabled = false;
  bool TopEnabled = false;
  bool BottomEnabled = false;
  string MirrorMode;
  string PixelReplicationMode;
  string ProjectionMode;
  bool Reorder = false;
  uint8_t ViewType = 0;
  float Near = 0;
  float Far = 0;
  float Left = 0;
  float Right = 0;
  float Top = 0;
  float Bottom = 0;
};

void sendViewDefinition(SScriptViewDefinition viewDefinition)
{
  SCigiViewDefinition cigiViewDefinition;
  cigiViewDefinition.viewID = ViewID(viewDefinition.ViewID);
  cigiViewDefinition.viewGroupID = ViewGroupID(viewDefinition.ViewGroupID);
  cigiViewDefinition.bNearEnabled = viewDefinition.NearEnabled;
  cigiViewDefinition.bFarEnabled = viewDefinition.FarEnabled;
  cigiViewDefinition.bLeftEnabled = viewDefinition.LeftEnabled;
  cigiViewDefinition.bRightEnabled = viewDefinition.RightEnabled;
  cigiViewDefinition.bTopEnabled = viewDefinition.TopEnabled;
  cigiViewDefinition.bBottomEnabled = viewDefinition.BottomEnabled;
  cigiViewDefinition.eMirrorMode = ConvertCigiStringToMirrorMode(viewDefinition.MirrorMode);
  cigiViewDefinition.pixelReplicationMode = PixelReplicationMode(g_pScriptRuntime->GetPixelReplicationModeID(viewDefinition.PixelReplicationMode));
  cigiViewDefinition.eProjectionMode = ToProjectionMode(boost::to_upper_copy(viewDefinition.ProjectionMode));
  cigiViewDefinition.bReorder = viewDefinition.Reorder;
  cigiViewDefinition.viewType = ViewType(viewDefinition.ViewType);
  cigiViewDefinition.fNear = viewDefinition.Near;
  cigiViewDefinition.fFar = viewDefinition.Far;
  cigiViewDefinition.fLeft = viewDefinition.Left;
  cigiViewDefinition.fRight = viewDefinition.Right;
  cigiViewDefinition.fTop = viewDefinition.Top;
  cigiViewDefinition.fBottom = viewDefinition.Bottom;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendViewDefinition(cigiViewDefinition);
}

void RegisterViewDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendViewDefinition), "sendViewDefinition");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptViewDefinition>(*m, "ViewDefinition", {constructor<SScriptViewDefinition()>(), constructor<SScriptViewDefinition(const SScriptViewDefinition&)>()},
                                                        {
                                                          {fun(&SScriptViewDefinition::ViewID), "ViewID"},
                                                          {fun(&SScriptViewDefinition::ViewGroupID), "GroupID"},
                                                          {fun(&SScriptViewDefinition::NearEnabled), "NearEnable"},
                                                          {fun(&SScriptViewDefinition::FarEnabled), "FarEnable"},
                                                          {fun(&SScriptViewDefinition::LeftEnabled), "LeftEnable"},
                                                          {fun(&SScriptViewDefinition::RightEnabled), "RightEnable"},
                                                          {fun(&SScriptViewDefinition::TopEnabled), "TopEnable"},
                                                          {fun(&SScriptViewDefinition::BottomEnabled), "BottomEnable"},
                                                          {fun(&SScriptViewDefinition::MirrorMode), "MirrorMode"},
                                                          {fun(&SScriptViewDefinition::PixelReplicationMode), "PixelReplicationMode"},
                                                          {fun(&SScriptViewDefinition::ProjectionMode), "ProjectionMode"},
                                                          {fun(&SScriptViewDefinition::Reorder), "Reorder"},
                                                          {fun(&SScriptViewDefinition::ViewType), "ViewType"},
                                                          {fun(&SScriptViewDefinition::Near), "Near"},
                                                          {fun(&SScriptViewDefinition::Far), "Far"},
                                                          {fun(&SScriptViewDefinition::Left), "Left"},
                                                          {fun(&SScriptViewDefinition::Right), "Right"},
                                                          {fun(&SScriptViewDefinition::Top), "Top"},
                                                          {fun(&SScriptViewDefinition::Bottom), "Bottom"},
                                                        });

  chai.add(m);
}

struct SScriptTopLevelEntityPosition
{
  uint16_t EntityID = 0;
  string AttachState;
  float Yaw = 0;
  float Pitch = 0;
  float Roll = 0;
  string Clamp;
  double Latitude = 0;
  double Longitude = 0;
  double Altitude = 0;
};

void sendTopLevelEntityPosition(SScriptTopLevelEntityPosition topLevelEntityPosition)
{
  STopLevelEntityPosition cigiTopLevelEntityPosition;
  cigiTopLevelEntityPosition.entityID = EntityID(topLevelEntityPosition.EntityID);
  cigiTopLevelEntityPosition.bAttached = (topLevelEntityPosition.AttachState == "Attach");
  cigiTopLevelEntityPosition.rotation.yaw = Degrees(topLevelEntityPosition.Yaw);
  cigiTopLevelEntityPosition.rotation.pitch = Degrees(topLevelEntityPosition.Pitch);
  cigiTopLevelEntityPosition.rotation.roll = Degrees(topLevelEntityPosition.Roll);
  cigiTopLevelEntityPosition.eClamp = ToClamp(topLevelEntityPosition.Clamp);
  cigiTopLevelEntityPosition.geodeticCoordinates.latitude = Latitude(topLevelEntityPosition.Latitude);
  cigiTopLevelEntityPosition.geodeticCoordinates.longitude = Longitude(topLevelEntityPosition.Longitude);
  cigiTopLevelEntityPosition.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(topLevelEntityPosition.Altitude);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendTopLevelEntityPosition(cigiTopLevelEntityPosition);
}

void RegisterTopLevelEntityPosition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendTopLevelEntityPosition), "sendTopLevelEntityPosition");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptTopLevelEntityPosition>(*m, "TopLevelEntityPosition", {constructor<SScriptTopLevelEntityPosition()>(), constructor<SScriptTopLevelEntityPosition(const SScriptTopLevelEntityPosition&)>()},
                                                                {
                                                                  {fun(&SScriptTopLevelEntityPosition::EntityID), "EntityID"},
                                                                  {fun(&SScriptTopLevelEntityPosition::AttachState), "AttachState"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Yaw), "Yaw"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Pitch), "Pitch"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Roll), "Roll"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Clamp), "Clamp"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Latitude), "Latitude"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Longitude), "Longitude"},
                                                                  {fun(&SScriptTopLevelEntityPosition::Altitude), "Altitude"},
                                                                });

  chai.add(m);
}

struct SScriptChildEntityPosition
{
  uint16_t EntityID = 0;
  string AttachState;
  float Yaw = 0;
  float Pitch = 0;
  float Roll = 0;
  uint16_t ParentID = 0;
  float XOffset = 0;
  float YOffset = 0;
  float ZOffset = 0;
};

void sendChildEntityPosition(SScriptChildEntityPosition childEntityPosition)
{
  SChildEntityPosition entityPosition;

  SChildEntityPosition cigiChildEntityPosition;
  cigiChildEntityPosition.entityID = EntityID(childEntityPosition.EntityID);
  cigiChildEntityPosition.bAttached = (childEntityPosition.AttachState == "Attach");
  cigiChildEntityPosition.rotation.yaw = Degrees(childEntityPosition.Yaw);
  cigiChildEntityPosition.rotation.pitch = Degrees(childEntityPosition.Pitch);
  cigiChildEntityPosition.rotation.roll = Degrees(childEntityPosition.Roll);
  cigiChildEntityPosition.parentID = EntityID(childEntityPosition.ParentID);
  cigiChildEntityPosition.offset[0] = childEntityPosition.XOffset;
  cigiChildEntityPosition.offset[1] = childEntityPosition.YOffset;
  cigiChildEntityPosition.offset[2] = childEntityPosition.ZOffset;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendChildEntityPosition(cigiChildEntityPosition);
}

void RegisterChildEntityPosition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendChildEntityPosition), "sendChildEntityPosition");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptChildEntityPosition>(*m, "ChildEntityPosition", {constructor<SScriptChildEntityPosition()>(), constructor<SScriptChildEntityPosition(const SScriptChildEntityPosition&)>()},
                                                             {
                                                               {fun(&SScriptChildEntityPosition::EntityID), "EntityID"},
                                                               {fun(&SScriptChildEntityPosition::AttachState), "AttachState"},
                                                               {fun(&SScriptChildEntityPosition::Yaw), "Yaw"},
                                                               {fun(&SScriptChildEntityPosition::Pitch), "Pitch"},
                                                               {fun(&SScriptChildEntityPosition::Roll), "Roll"},
                                                               {fun(&SScriptChildEntityPosition::ParentID), "ParentID"},
                                                               {fun(&SScriptChildEntityPosition::XOffset), "XOffset"},
                                                               {fun(&SScriptChildEntityPosition::YOffset), "YOffset"},
                                                               {fun(&SScriptChildEntityPosition::ZOffset), "ZOffset"},
                                                             });

  chai.add(m);
}

struct SScriptConformalClampedEntityPosition
{
  uint16_t EntityID = 0;
  float Yaw = 0;
  double Latitude = 0;
  double Longitude = 0;
};

void sendConformalClampedEntityPosition(SScriptConformalClampedEntityPosition conformalClampedEntityPosition)
{
  SCigiConformalClampedEntityPosition cigiConformalClampedEntityPosition;
  cigiConformalClampedEntityPosition.entityID = EntityID(conformalClampedEntityPosition.EntityID);
  cigiConformalClampedEntityPosition.fYaw = Degrees(conformalClampedEntityPosition.Yaw);
  cigiConformalClampedEntityPosition.latitude = Latitude(conformalClampedEntityPosition.Latitude);
  cigiConformalClampedEntityPosition.longitude = Longitude(conformalClampedEntityPosition.Longitude);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendConformalClampedEntityPosition(cigiConformalClampedEntityPosition);
}

void RegisterConformalClampedEntityPosition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendConformalClampedEntityPosition), "sendConformalClampedEntityPosition");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptConformalClampedEntityPosition>(*m, "ConformalClampedEntityPosition", {constructor<SScriptConformalClampedEntityPosition()>(), constructor<SScriptConformalClampedEntityPosition(const SScriptConformalClampedEntityPosition&)>()},
                                                                        {
                                                                          {fun(&SScriptConformalClampedEntityPosition::EntityID), "EntityID"},
                                                                          {fun(&SScriptConformalClampedEntityPosition::Yaw), "Yaw"},
                                                                          {fun(&SScriptConformalClampedEntityPosition::Latitude), "Latitude"},
                                                                          {fun(&SScriptConformalClampedEntityPosition::Longitude), "Longitude"},
                                                                        });

  chai.add(m);
}

struct SScriptEntityVelocityControl
{
  uint16_t EntityID = 0;
  float XLinearVelocity = 0;
  float YLinearVelocity = 0;
  float ZLinearVelocity = 0;
  float RollAngularVelocity = 0;
  float PitchAngularVelocity = 0;
  float YawAngularVelocity = 0;
  string CoordinateSystem;
};

void sendEntityVelocityControl(SScriptEntityVelocityControl entityVelocityControl)
{
  SCigiEntityVelocityControl cigiEntityVelocityControl;
  cigiEntityVelocityControl.entityID = EntityID(entityVelocityControl.EntityID);
  cigiEntityVelocityControl.linearVelocity[0] = entityVelocityControl.XLinearVelocity;
  cigiEntityVelocityControl.linearVelocity[1] = entityVelocityControl.YLinearVelocity;
  cigiEntityVelocityControl.linearVelocity[2] = entityVelocityControl.ZLinearVelocity;
  cigiEntityVelocityControl.angularVelocity.yaw = Degrees(entityVelocityControl.YawAngularVelocity);
  cigiEntityVelocityControl.angularVelocity.pitch = Degrees(entityVelocityControl.PitchAngularVelocity);
  cigiEntityVelocityControl.angularVelocity.roll = Degrees(entityVelocityControl.RollAngularVelocity);
  cigiEntityVelocityControl.coordinateSystem = ConvertCigiStringToObjectCoordinateSystem(entityVelocityControl.CoordinateSystem);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendVelocityControl(cigiEntityVelocityControl);
}

void RegisterEntityVelocityControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEntityVelocityControl), "sendEntityVelocityControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptEntityVelocityControl>(*m, "EntityVelocityControl", {constructor<SScriptEntityVelocityControl()>(), constructor<SScriptEntityVelocityControl(const SScriptEntityVelocityControl&)>()},
                                                               {
                                                                 {fun(&SScriptEntityVelocityControl::EntityID), "EntityID"},
                                                                 {fun(&SScriptEntityVelocityControl::XLinearVelocity), "XLinearVelocity"},
                                                                 {fun(&SScriptEntityVelocityControl::YLinearVelocity), "YLinearVelocity"},
                                                                 {fun(&SScriptEntityVelocityControl::ZLinearVelocity), "ZLinearVelocity"},
                                                                 {fun(&SScriptEntityVelocityControl::RollAngularVelocity), "RollAngularVelocity"},
                                                                 {fun(&SScriptEntityVelocityControl::PitchAngularVelocity), "PitchAngularVelocity"},
                                                                 {fun(&SScriptEntityVelocityControl::YawAngularVelocity), "YawAngularVelocity"},
                                                                 {fun(&SScriptEntityVelocityControl::CoordinateSystem), "CoordinateSystem"},
                                                               });

  chai.add(m);
}

struct SScriptArticulatedPartVelocityControl
{
  uint16_t EntityID = 0;
  uint8_t ArticulatedPartID = 0;
  float XLinearVelocity = 0;
  float YLinearVelocity = 0;
  float ZLinearVelocity = 0;
  float RollAngularVelocity = 0;
  float PitchAngularVelocity = 0;
  float YawAngularVelocity = 0;
};

void sendArticulatedPartVelocityControl(SScriptArticulatedPartVelocityControl articulatedPartVelocityControl)
{
  SCigiArticulatedPartVelocityControl cigiArticulatedPartVelocityControl;
  cigiArticulatedPartVelocityControl.entityID = EntityID(articulatedPartVelocityControl.EntityID);
  cigiArticulatedPartVelocityControl.articulatedPartID = ArticulatedPartID(articulatedPartVelocityControl.ArticulatedPartID);
  cigiArticulatedPartVelocityControl.linearVelocity[0] = articulatedPartVelocityControl.XLinearVelocity;
  cigiArticulatedPartVelocityControl.linearVelocity[1] = articulatedPartVelocityControl.YLinearVelocity;
  cigiArticulatedPartVelocityControl.linearVelocity[2] = articulatedPartVelocityControl.ZLinearVelocity;
  cigiArticulatedPartVelocityControl.angularVelocity.yaw = Degrees(articulatedPartVelocityControl.YawAngularVelocity);
  cigiArticulatedPartVelocityControl.angularVelocity.pitch = Degrees(articulatedPartVelocityControl.PitchAngularVelocity);
  cigiArticulatedPartVelocityControl.angularVelocity.roll = Degrees(articulatedPartVelocityControl.RollAngularVelocity);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendVelocityControl(cigiArticulatedPartVelocityControl);
}

void RegisterArticulatedPartVelocityControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendArticulatedPartVelocityControl), "sendArticulatedPartVelocityControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptArticulatedPartVelocityControl>(*m, "ArticulatedPartVelocityControl", {constructor<SScriptArticulatedPartVelocityControl()>(), constructor<SScriptArticulatedPartVelocityControl(const SScriptArticulatedPartVelocityControl&)>()},
                                                                        {
                                                                          {fun(&SScriptArticulatedPartVelocityControl::EntityID), "EntityID"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::ArticulatedPartID), "ArticulatedPartID"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::XLinearVelocity), "XLinearVelocity"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::YLinearVelocity), "YLinearVelocity"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::ZLinearVelocity), "ZLinearVelocity"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::RollAngularVelocity), "RollAngularVelocity"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::PitchAngularVelocity), "PitchAngularVelocity"},
                                                                          {fun(&SScriptArticulatedPartVelocityControl::YawAngularVelocity), "YawAngularVelocity"},
                                                                        });

  chai.add(m);
}

struct SScriptArticulatedPartControl
{
  uint16_t EntityID = 0;
  uint8_t ArticulatedPartID = 0;
  bool Enabled = false;
  bool XOffsetEnabled = false;
  bool YOffsetEnabled = false;
  bool ZOffsetEnabled = false;
  float XOffset = 0;
  float YOffset = 0;
  float ZOffset = 0;
  bool YawEnabled = false;
  bool PitchEnabled = false;
  bool RollEnabled = false;
  float Roll = 0;
  float Pitch = 0;
  float Yaw = 0;
};

void sendArticulatedPartControl(SScriptArticulatedPartControl articulatedPartControl)
{
  SCigiArticulatedPart cigiArticulatedPart;
  cigiArticulatedPart.entityID = EntityID(articulatedPartControl.EntityID);
  cigiArticulatedPart.articulatedPartID = ArticulatedPartID(articulatedPartControl.ArticulatedPartID);
  cigiArticulatedPart.bEnabled = articulatedPartControl.Enabled;
  cigiArticulatedPart.bOffsetEnabled[0] = articulatedPartControl.XOffsetEnabled;
  cigiArticulatedPart.bOffsetEnabled[1] = articulatedPartControl.YOffsetEnabled;
  cigiArticulatedPart.bOffsetEnabled[2] = articulatedPartControl.ZOffsetEnabled;

  cigiArticulatedPart.offset[0] = articulatedPartControl.XOffset;
  cigiArticulatedPart.offset[1] = articulatedPartControl.YOffset;
  cigiArticulatedPart.offset[2] = articulatedPartControl.ZOffset;
  cigiArticulatedPart.bYawEnabled = articulatedPartControl.YawEnabled;
  cigiArticulatedPart.bPitchEnabled = articulatedPartControl.PitchEnabled;
  cigiArticulatedPart.bRollEnabled = articulatedPartControl.RollEnabled;

  cigiArticulatedPart.rotation.yaw = Degrees(articulatedPartControl.Yaw);
  cigiArticulatedPart.rotation.pitch = Degrees(articulatedPartControl.Pitch);
  cigiArticulatedPart.rotation.roll = Degrees(articulatedPartControl.Roll);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendArticulatedPartControl(cigiArticulatedPart);
}

void RegisterArticulatedPartControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendArticulatedPartControl), "sendArticulatedPartControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptArticulatedPartControl>(*m, "ArticulatedPartControl", {constructor<SScriptArticulatedPartControl()>(), constructor<SScriptArticulatedPartControl(const SScriptArticulatedPartControl&)>()},
                                                                {
                                                                  {fun(&SScriptArticulatedPartControl::EntityID), "EntityID"},
                                                                  {fun(&SScriptArticulatedPartControl::ArticulatedPartID), "ArticulatedPartID"},
                                                                  {fun(&SScriptArticulatedPartControl::Enabled), "Enabled"},
                                                                  {fun(&SScriptArticulatedPartControl::XOffsetEnabled), "XOffsetEnabled"},
                                                                  {fun(&SScriptArticulatedPartControl::YOffsetEnabled), "YOffsetEnabled"},
                                                                  {fun(&SScriptArticulatedPartControl::ZOffsetEnabled), "ZOffsetEnabled"},
                                                                  {fun(&SScriptArticulatedPartControl::XOffset), "XOffset"},
                                                                  {fun(&SScriptArticulatedPartControl::YOffset), "YOffset"},
                                                                  {fun(&SScriptArticulatedPartControl::ZOffset), "ZOffset"},
                                                                  {fun(&SScriptArticulatedPartControl::YawEnabled), "YawEnabled"},
                                                                  {fun(&SScriptArticulatedPartControl::PitchEnabled), "PitchEnabled"},
                                                                  {fun(&SScriptArticulatedPartControl::RollEnabled), "RollEnabled"},
                                                                  {fun(&SScriptArticulatedPartControl::Roll), "Roll"},
                                                                  {fun(&SScriptArticulatedPartControl::Pitch), "Pitch"},
                                                                  {fun(&SScriptArticulatedPartControl::Yaw), "Yaw"},
                                                                });

  chai.add(m);
}

struct SScriptShortArticulatedPart
{
  uint16_t EntityID = 0;
  uint8_t ArticulatedPartID1 = 0;
  uint8_t ArticulatedPartID2 = 0;
  bool ArticulatedPart1Enabled = false;
  bool ArticulatedPart2Enabled = false;
  string DOFSelect1;
  string DOFSelect2;
  float DOF1 = 0;
  float DOF2 = 0;
};

void sendShortArticulatedPartControl(SScriptShortArticulatedPart shortArticulatedPartControl)
{
  // Note: If DOF Select 1 and DOF Select 2 refer to the same degree of freedom for the same articulated part,
  // then DOF 2 (i.e.,, the �last-in� value) shall take priority over DOF 1.
  // --- '*\(^o^)/*' ---
  SCigiShortArticulatedPart cigiShortArticulatedPart;
  cigiShortArticulatedPart.entityID = EntityID(shortArticulatedPartControl.EntityID);
  cigiShortArticulatedPart.articulatedPartID1 = ArticulatedPartID(shortArticulatedPartControl.ArticulatedPartID1);
  cigiShortArticulatedPart.articulatedPartID2 = ArticulatedPartID(shortArticulatedPartControl.ArticulatedPartID2);
  cigiShortArticulatedPart.bArticulatedPart1Enabled = shortArticulatedPartControl.ArticulatedPart1Enabled;
  cigiShortArticulatedPart.bArticulatedPart2Enabled = shortArticulatedPartControl.ArticulatedPart2Enabled;
  cigiShortArticulatedPart.eDOF1 = ConvertCigiStringToDegreeOfFreedom(shortArticulatedPartControl.DOFSelect1);
  cigiShortArticulatedPart.eDOF2 = ConvertCigiStringToDegreeOfFreedom(shortArticulatedPartControl.DOFSelect2);
  cigiShortArticulatedPart.fDOF1 = shortArticulatedPartControl.DOF1;
  cigiShortArticulatedPart.fDOF2 = shortArticulatedPartControl.DOF2;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendShortArticulatedPartControl(cigiShortArticulatedPart);
}

void RegisterShortArticulatedPartControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendShortArticulatedPartControl), "sendShortArticulatedPartControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptShortArticulatedPart>(*m, "ShortArticulatedPartControl", {constructor<SScriptShortArticulatedPart()>(), constructor<SScriptShortArticulatedPart(const SScriptShortArticulatedPart&)>()},
                                                              {
                                                                {fun(&SScriptShortArticulatedPart::EntityID), "EntityID"},
                                                                {fun(&SScriptShortArticulatedPart::ArticulatedPartID1), "ArticulatedPartID1"},
                                                                {fun(&SScriptShortArticulatedPart::ArticulatedPartID2), "ArticulatedPartID2"},
                                                                {fun(&SScriptShortArticulatedPart::ArticulatedPart1Enabled), "ArticulatedPart1Enabled"},
                                                                {fun(&SScriptShortArticulatedPart::ArticulatedPart2Enabled), "ArticulatedPart2Enabled"},
                                                                {fun(&SScriptShortArticulatedPart::DOFSelect1), "DOFSelect1"},
                                                                {fun(&SScriptShortArticulatedPart::DOFSelect2), "DOFSelect2"},
                                                                {fun(&SScriptShortArticulatedPart::DOF1), "DOF1"},
                                                                {fun(&SScriptShortArticulatedPart::DOF2), "DOF2"},
                                                              });

  chai.add(m);
}

struct SScriptViewControl
{
  uint16_t ViewID = 0;
  uint8_t ViewGroupID = 0;
  uint16_t EntityID = 0;
  bool XOffsetEnabled = false;
  bool YOffsetEnabled = false;
  bool ZOffsetEnabled = false;
  float XOffset = false;
  float YOffset = false;
  float ZOffset = false;
  bool YawEnabled = false;
  bool PitchEnabled = false;
  bool RollEnabled = false;
  float Yaw = 0;
  float Pitch = 0;
  float Roll = 0;
};

void sendViewControl(SScriptViewControl viewControl)
{
  SCigiViewControl cigiViewControl;
  cigiViewControl.viewID = ViewID(viewControl.ViewID);
  cigiViewControl.viewGroupID = ViewGroupID(viewControl.ViewGroupID);
  cigiViewControl.entityID = EntityID(viewControl.EntityID);
  cigiViewControl.offsetEnabled[0] = viewControl.XOffsetEnabled;
  cigiViewControl.offsetEnabled[1] = viewControl.YOffsetEnabled;
  cigiViewControl.offsetEnabled[2] = viewControl.ZOffsetEnabled;
  cigiViewControl.offset[0] = viewControl.XOffset;
  cigiViewControl.offset[1] = viewControl.YOffset;
  cigiViewControl.offset[2] = viewControl.ZOffset;
  cigiViewControl.bYawEnabled = viewControl.YawEnabled;
  cigiViewControl.bPitchEnabled = viewControl.PitchEnabled;
  cigiViewControl.bRollEnabled = viewControl.RollEnabled;
  cigiViewControl.rotation.yaw = Degrees(viewControl.Yaw);
  cigiViewControl.rotation.pitch = Degrees(viewControl.Pitch);
  cigiViewControl.rotation.roll = Degrees(viewControl.Roll);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendViewControl(cigiViewControl);
}

void RegisterViewControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendViewControl), "sendViewControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptViewControl>(*m, "ViewControl", {constructor<SScriptViewControl()>(), constructor<SScriptViewControl(const SScriptViewControl&)>()},
                                                     {
                                                       {fun(&SScriptViewControl::ViewID), "ViewID"},
                                                       {fun(&SScriptViewControl::ViewGroupID), "ViewGroupID"},
                                                       {fun(&SScriptViewControl::EntityID), "EntityID"},
                                                       {fun(&SScriptViewControl::XOffsetEnabled), "XOffsetEnabled"},
                                                       {fun(&SScriptViewControl::YOffsetEnabled), "YOffsetEnabled"},
                                                       {fun(&SScriptViewControl::ZOffsetEnabled), "ZOffsetEnabled"},
                                                       {fun(&SScriptViewControl::XOffset), "XOffset"},
                                                       {fun(&SScriptViewControl::YOffset), "YOffset"},
                                                       {fun(&SScriptViewControl::ZOffset), "ZOffset"},
                                                       {fun(&SScriptViewControl::YawEnabled), "YawEnabled"},
                                                       {fun(&SScriptViewControl::PitchEnabled), "PitchEnabled"},
                                                       {fun(&SScriptViewControl::RollEnabled), "RollEnabled"},
                                                       {fun(&SScriptViewControl::Yaw), "Yaw"},
                                                       {fun(&SScriptViewControl::Pitch), "Pitch"},
                                                       {fun(&SScriptViewControl::Roll), "Roll"},
                                                     });

  chai.add(m);
}

struct SScriptComponentData
{
  uint16_t Values[6] = {0, 0, 0, 0, 0, 0};
};

uint16_t& componentDataValueAt(SScriptComponentData& data, size_t index)
{
  return data.Values[index];
}

uint16_t componentDataValueAtConst(const SScriptComponentData& data, size_t index)
{
  return data.Values[index];
}

struct SScriptComponentControl
{
  uint16_t InstanceID = 0;
  uint16_t ComponentID = 0;
  string ComponentClass;
  uint8_t ComponentState = 0;
  SScriptComponentData ComponentData;
};

void sendComponentControl(SScriptComponentControl scriptControl)
{
  SCigiComponentControl cigiComponentControl;
  cigiComponentControl.key.nInstanceID = scriptControl.InstanceID;
  cigiComponentControl.key.componentID = CigiComponentID(scriptControl.ComponentID);
  cigiComponentControl.key.componentClassID = g_pScriptRuntime->GetComponentClassID(scriptControl.ComponentClass);
  cigiComponentControl.state.nComponentState = scriptControl.ComponentState;
  cigiComponentControl.state.componentData[0] = scriptControl.ComponentData.Values[0];
  cigiComponentControl.state.componentData[1] = scriptControl.ComponentData.Values[1];
  cigiComponentControl.state.componentData[2] = scriptControl.ComponentData.Values[2];
  cigiComponentControl.state.componentData[3] = scriptControl.ComponentData.Values[3];
  cigiComponentControl.state.componentData[4] = scriptControl.ComponentData.Values[4];
  cigiComponentControl.state.componentData[5] = scriptControl.ComponentData.Values[5];

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendComponentControl(cigiComponentControl);
}

void RegisterComponentControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendComponentControl), "sendComponentControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptComponentControl>(*m, "ComponentControl", {constructor<SScriptComponentControl()>(), constructor<SScriptComponentControl(const SScriptComponentControl&)>()},
                                                          {
                                                            {fun(&SScriptComponentControl::InstanceID), "InstanceID"},
                                                            {fun(&SScriptComponentControl::ComponentID), "ComponentID"},
                                                            {fun(&SScriptComponentControl::ComponentClass), "ComponentClass"},
                                                            {fun(&SScriptComponentControl::ComponentState), "ComponentState"},
                                                            {fun(&SScriptComponentControl::ComponentData), "ComponentData"},
                                                          });
  chaiscript::utility::add_class<SScriptComponentData>(*m, "ComponentData", {constructor<SScriptComponentData()>(), constructor<SScriptComponentData(const SScriptComponentData&)>()},
                                                       {
                                                         {fun(&SScriptComponentData::Values), "Values"},
                                                       });
  m->add(chaiscript::fun(&componentDataValueAt), "[]");
  m->add(chaiscript::fun(&componentDataValueAtConst), "[]");

  chai.add(m);
}

struct SScriptShortComponentData
{
  uint16_t Values[2] = {0, 0};
};

uint16_t& shortComponentDataValueAt(SScriptShortComponentData& data, size_t index)
{
  return data.Values[index];
}

uint16_t shortComponentDataValueAtConst(const SScriptShortComponentData& data, size_t index)
{
  return data.Values[index];
}

struct SScriptShortComponentControl
{
  uint16_t InstanceID = 0;
  uint16_t ComponentID = 0;
  string ComponentClass;
  uint8_t ComponentState = 0;
  SScriptShortComponentData ComponentData;
};

void sendShortComponentControl(SScriptShortComponentControl scriptShortComponentControl)
{
  SCigiShortComponentControl cigiShortComponentControl;
  cigiShortComponentControl.nInstanceID = scriptShortComponentControl.InstanceID;
  cigiShortComponentControl.componentID = CigiComponentID(scriptShortComponentControl.ComponentID);
  cigiShortComponentControl.componentClassID = g_pScriptRuntime->GetComponentClassID(scriptShortComponentControl.ComponentClass);
  cigiShortComponentControl.nComponentState = scriptShortComponentControl.ComponentState;
  cigiShortComponentControl.componentData1 = scriptShortComponentControl.ComponentData.Values[0];
  cigiShortComponentControl.componentData2 = scriptShortComponentControl.ComponentData.Values[1];

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendShortComponentControl(cigiShortComponentControl);
}

void RegisterShortComponentControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendShortComponentControl), "sendShortComponentControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptShortComponentControl>(*m, "ShortComponentControl", {constructor<SScriptShortComponentControl()>(), constructor<SScriptShortComponentControl(const SScriptShortComponentControl&)>()},
                                                               {
                                                                 {fun(&SScriptShortComponentControl::InstanceID), "InstanceID"},
                                                                 {fun(&SScriptShortComponentControl::ComponentID), "ComponentID"},
                                                                 {fun(&SScriptShortComponentControl::ComponentClass), "ComponentClass"},
                                                                 {fun(&SScriptShortComponentControl::ComponentState), "ComponentState"},
                                                                 {fun(&SScriptShortComponentControl::ComponentData), "ComponentData"},
                                                               });
  chaiscript::utility::add_class<SScriptShortComponentData>(*m, "ShortComponentData", {constructor<SScriptShortComponentData()>(), constructor<SScriptShortComponentData(const SScriptShortComponentData&)>()},
                                                            {
                                                              {fun(&SScriptShortComponentData::Values), "Values"},
                                                            });
  m->add(chaiscript::fun(&shortComponentDataValueAt), "[]");
  m->add(chaiscript::fun(&shortComponentDataValueAtConst), "[]");

  chai.add(m);
}

struct SScriptEntityControl
{
  bool ExtendedEntityType = false;
  uint8_t EntityKind = 0;
  uint8_t EntityDomain = 0;
  uint16_t EntityCountry = 0;
  uint8_t EntityCategory = 0;
  uint8_t EntitySubCategory = 0;
  uint8_t EntitySpecific = 0;
  uint8_t EntityExtra = 0;
  uint8_t ShortEntityType = 0;
  uint16_t EntityID = 0;
  string State;
  bool CollisionReportingEnable = false;
  bool InheritAlpha = false;
  bool SmoothingEnable = false;
  uint8_t Alpha = 0;
};

void sendEntityControl(SScriptEntityControl scriptEntityControl)
{
  SEntityControl cigiEntityControl;
  cigiEntityControl.entityType.entityCategoryID = SisoEntityCategoryID(scriptEntityControl.EntityCategory);
  cigiEntityControl.entityType.entityCountryID = SisoEntityCountryID(scriptEntityControl.EntityCountry);
  cigiEntityControl.entityType.entityDomainID = SisoEntityDomainID(scriptEntityControl.EntityDomain);
  cigiEntityControl.entityType.entityExtraID = SisoEntityExtraID(scriptEntityControl.EntityExtra);
  cigiEntityControl.entityType.entityKindID = SisoEntityKindID(scriptEntityControl.EntityKind);
  cigiEntityControl.entityType.entitySpecificID = SisoEntitySpecificID(scriptEntityControl.EntitySpecific);
  cigiEntityControl.entityType.entitySubCategoryID = SisoEntitySubCategoryID(scriptEntityControl.EntitySubCategory);
  cigiEntityControl.entityID = EntityID(scriptEntityControl.EntityID);
  cigiEntityControl.eState = ConvertCigiStringToActiveState(scriptEntityControl.State);
  cigiEntityControl.bCollisionReportingEnabled = scriptEntityControl.CollisionReportingEnable;
  cigiEntityControl.bInheritAlpha = scriptEntityControl.InheritAlpha;
  cigiEntityControl.bSmoothingEnabled = scriptEntityControl.SmoothingEnable;
  cigiEntityControl.alpha = scriptEntityControl.Alpha;

  if (scriptEntityControl.ExtendedEntityType == true)
  {
    cigiEntityControl.eExtendedEntityType = EExtendedEntityType::EXTENDED;
  }
  else if (scriptEntityControl.ExtendedEntityType == false)
  {
    cigiEntityControl.eExtendedEntityType = EExtendedEntityType::SHORT;
    cigiEntityControl.shortEntityTypeID = ShortEntityTypeID(scriptEntityControl.ShortEntityType);
  }

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendEntityControl(cigiEntityControl);
}

void RegisterEntityControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEntityControl), "sendEntityControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptEntityControl>(*m, "EntityControl", {constructor<SScriptEntityControl()>(), constructor<SScriptEntityControl(const SScriptEntityControl&)>()},
                                                       {
                                                         {fun(&SScriptEntityControl::ExtendedEntityType), "ExtendedEntityType"},
                                                         {fun(&SScriptEntityControl::EntityKind), "EntityKind"},
                                                         {fun(&SScriptEntityControl::EntityDomain), "EntityDomain"},
                                                         {fun(&SScriptEntityControl::EntityCountry), "EntityCountry"},
                                                         {fun(&SScriptEntityControl::EntityCategory), "EntityCategory"},
                                                         {fun(&SScriptEntityControl::EntitySubCategory), "EntitySubCategory"},
                                                         {fun(&SScriptEntityControl::EntitySpecific), "EntitySpecific"},
                                                         {fun(&SScriptEntityControl::EntityExtra), "EntityExtra"},
                                                         {fun(&SScriptEntityControl::ShortEntityType), "ShortEntityType"},
                                                         {fun(&SScriptEntityControl::EntityID), "EntityID"},
                                                         {fun(&SScriptEntityControl::State), "State"},
                                                         {fun(&SScriptEntityControl::CollisionReportingEnable), "CollisionReportingEnable"},
                                                         {fun(&SScriptEntityControl::InheritAlpha), "InheritAlpha"},
                                                         {fun(&SScriptEntityControl::SmoothingEnable), "SmoothingEnable"},
                                                         {fun(&SScriptEntityControl::Alpha), "Alpha"},
                                                       });

  chai.add(m);
}

struct SScriptAnimationControl
{
  uint16_t AnimationID = 0;
  string AnimationState;
  string AnimationFramePositionReset;
  string AnimationLoopMode;
  bool InheritAlpha = false;
  float Alpha = 0;
  float AnimationSpeed = 0;
  uint16_t EntityID = 0;
};

void sendAnimationControl(SScriptAnimationControl scriptAnimationControl)
{
  SCigiAnimationControl cigiAnimationControl;
  cigiAnimationControl.animationID = AnimationID(scriptAnimationControl.AnimationID);
  cigiAnimationControl.eAnimationState = ConvertCigiStringToAnimationState(scriptAnimationControl.AnimationState);
  cigiAnimationControl.eAnimationFramePositionReset = ConvertCigiStringToFramePositionReset(scriptAnimationControl.AnimationFramePositionReset);
  cigiAnimationControl.eAnimationLoopMode = ConvertCigiStringToAnimationLoopMode(scriptAnimationControl.AnimationLoopMode);
  cigiAnimationControl.bInheritAlpha = scriptAnimationControl.InheritAlpha;
  cigiAnimationControl.alpha = ConvertAlphaToPercentage(scriptAnimationControl.Alpha);
  cigiAnimationControl.fAnimationSpeed = scriptAnimationControl.AnimationSpeed;
  cigiAnimationControl.entityID = EntityID(scriptAnimationControl.EntityID);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendAnimationControl(cigiAnimationControl);
}

void RegisterAnimationControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendAnimationControl), "sendAnimationControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptAnimationControl>(*m, "AnimationControl", {constructor<SScriptAnimationControl()>(), constructor<SScriptAnimationControl(const SScriptAnimationControl&)>()},
                                                          {
                                                            {fun(&SScriptAnimationControl::AnimationID), "AnimationID"},
                                                            {fun(&SScriptAnimationControl::AnimationState), "AnimationState"},
                                                            {fun(&SScriptAnimationControl::AnimationFramePositionReset), "AnimationFramePositionReset"},
                                                            {fun(&SScriptAnimationControl::AnimationLoopMode), "AnimationLoopMode"},
                                                            {fun(&SScriptAnimationControl::InheritAlpha), "InheritAlpha"},
                                                            {fun(&SScriptAnimationControl::Alpha), "Alpha"},
                                                            {fun(&SScriptAnimationControl::AnimationSpeed), "AnimationSpeed"},
                                                            {fun(&SScriptAnimationControl::EntityID), "EntityID"},
                                                          });

  chai.add(m);
}

struct SScriptCelestialSphereControl
{
  bool ContinuousTimeOfDayEnable = false;
  bool SunEnable = false;
  bool MoonEnable = false;
  bool StarFieldEnable = false;
  bool DateTimeValid = false;
  uint8_t Hour = 0;
  uint8_t Minute = 0;
  float Seconds = 0;
  uint16_t Year = 0;
  uint8_t Month = 0;
  uint8_t Day = 0;
  float StarFieldIntensity = 0;
};

void sendCelestialSphereControl(SScriptCelestialSphereControl scriptCelestialSphereControl)
{
  SCigiCelestialSphereControl cigiCelestialSphereControl;
  cigiCelestialSphereControl.bContinuousTimeOfDayEnable = scriptCelestialSphereControl.ContinuousTimeOfDayEnable;
  cigiCelestialSphereControl.bSunEnable = scriptCelestialSphereControl.SunEnable;
  cigiCelestialSphereControl.bMoonEnable = scriptCelestialSphereControl.MoonEnable;
  cigiCelestialSphereControl.bStarFieldEnable = scriptCelestialSphereControl.StarFieldEnable;
  cigiCelestialSphereControl.bDateTimeValid = scriptCelestialSphereControl.DateTimeValid;
  cigiCelestialSphereControl.hour = Hour(scriptCelestialSphereControl.Hour);
  cigiCelestialSphereControl.minute = Minute(scriptCelestialSphereControl.Minute);
  cigiCelestialSphereControl.second = Second(scriptCelestialSphereControl.Seconds);
  cigiCelestialSphereControl.year = Year(scriptCelestialSphereControl.Year);
  CigiMonth month = CigiMonth(scriptCelestialSphereControl.Month);
  cigiCelestialSphereControl.month = ConvertToMonth(month);
  cigiCelestialSphereControl.day = Day(scriptCelestialSphereControl.Day);

  CigiFloatPercentage starFieldIntensity = CigiFloatPercentage(scriptCelestialSphereControl.StarFieldIntensity);
  cigiCelestialSphereControl.starFieldIntensity = ConvertToPercentage(starFieldIntensity);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendCelestialSphereControl(cigiCelestialSphereControl);
}

void RegisterCelestialSphereControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendCelestialSphereControl), "sendCelestialSphereControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptCelestialSphereControl>(*m, "CelestialSphereControl", {constructor<SScriptCelestialSphereControl()>(), constructor<SScriptCelestialSphereControl(const SScriptCelestialSphereControl&)>()},
                                                                {
                                                                  {fun(&SScriptCelestialSphereControl::ContinuousTimeOfDayEnable), "ContinuousTimeOfDayEnable"},
                                                                  {fun(&SScriptCelestialSphereControl::SunEnable), "SunEnable"},
                                                                  {fun(&SScriptCelestialSphereControl::MoonEnable), "MoonEnable"},
                                                                  {fun(&SScriptCelestialSphereControl::StarFieldEnable), "StarFieldEnable"},
                                                                  {fun(&SScriptCelestialSphereControl::DateTimeValid), "DateTimeValid"},
                                                                  {fun(&SScriptCelestialSphereControl::Hour), "Hour"},
                                                                  {fun(&SScriptCelestialSphereControl::Minute), "Minute"},
                                                                  {fun(&SScriptCelestialSphereControl::Seconds), "Seconds"},
                                                                  {fun(&SScriptCelestialSphereControl::Year), "Year"},
                                                                  {fun(&SScriptCelestialSphereControl::Month), "Month"},
                                                                  {fun(&SScriptCelestialSphereControl::Day), "Day"},
                                                                  {fun(&SScriptCelestialSphereControl::StarFieldIntensity), "StarFieldIntensity"},
                                                                });

  chai.add(m);
}

struct SScriptAtmosphereControl
{
  bool AtmosphericModelEnable = false;
  uint8_t GlobalHumidity = 0;
  float GlobalAirTemp = 0;
  float GlobalVisibility = 0;
  float GlobalHorizontalWindSpeed = 0;
  float GlobalVerticalWindSpeed = 0;
  float GlobalWindDirection = 0;
  float GlobalBarometricPressure = 0;
};

void sendAtmosphereControl(SScriptAtmosphereControl scriptCelestialSphereControl)
{
  SCigiAtmosphereControl cigiAtmosphereControl;
  cigiAtmosphereControl.bAtmosphereModelEnable = scriptCelestialSphereControl.AtmosphericModelEnable;

  CigiIntPercentage globalHumidity = CigiIntPercentage(scriptCelestialSphereControl.GlobalHumidity);
  cigiAtmosphereControl.globalHumidity = ConvertToPercentage(globalHumidity);

  cigiAtmosphereControl.fGlobalAirTemp = scriptCelestialSphereControl.GlobalAirTemp;
  cigiAtmosphereControl.fGlobalVisibility = scriptCelestialSphereControl.GlobalVisibility;
  cigiAtmosphereControl.fGlobalHorizontalWindSpeed = scriptCelestialSphereControl.GlobalHorizontalWindSpeed;
  cigiAtmosphereControl.fGlobalVerticalWindSpeed = scriptCelestialSphereControl.GlobalVerticalWindSpeed;
  cigiAtmosphereControl.globalWindDirection = Degrees(scriptCelestialSphereControl.GlobalWindDirection);
  cigiAtmosphereControl.fGlobalBarometricPressure = scriptCelestialSphereControl.GlobalBarometricPressure;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendAtmosphereControl(cigiAtmosphereControl);
}

void RegisterAtmosphereControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendAtmosphereControl), "sendAtmosphereControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptAtmosphereControl>(*m, "AtmosphereControl", {constructor<SScriptAtmosphereControl()>(), constructor<SScriptAtmosphereControl(const SScriptAtmosphereControl&)>()},
                                                           {
                                                             {fun(&SScriptAtmosphereControl::AtmosphericModelEnable), "AtmosphericModelEnable"},
                                                             {fun(&SScriptAtmosphereControl::GlobalHumidity), "GlobalHumidity"},
                                                             {fun(&SScriptAtmosphereControl::GlobalAirTemp), "GlobalAirTemp"},
                                                             {fun(&SScriptAtmosphereControl::GlobalVisibility), "GlobalVisibility"},
                                                             {fun(&SScriptAtmosphereControl::GlobalHorizontalWindSpeed), "GlobalHorizontalWindSpeed"},
                                                             {fun(&SScriptAtmosphereControl::GlobalVerticalWindSpeed), "GlobalVerticalWindSpeed"},
                                                             {fun(&SScriptAtmosphereControl::GlobalWindDirection), "GlobalWindDirection"},
                                                             {fun(&SScriptAtmosphereControl::GlobalBarometricPressure), "GlobalBarometricPressure"},
                                                           });

  chai.add(m);
}

struct SScriptWeatherControl
{
  string LayerID;
  float Humidity = 0;
  bool WeatherEnable = false;
  bool BottomScudEnable = false;
  bool RandomWindsEnable = false;
  bool RandomLightningEnable = false;
  string CloudType;
  string Scope;
  uint8_t Severity = 0;
  bool TopScudEnable = false;
  uint16_t EntityID = 0;
  uint16_t RegionID = 0;
  float AirTemperature = 0;
  float VisibilityRange = 0;
  float BottomScudFrequency = 0;
  float Coverage = 0;
  float BaseElevation = 0;
  float Thickness = 0;
  float BottomTransitionBandThickness = 0;
  float HorizontalWindSpeed = 0;
  float VerticalWindSpeed = 0;
  float WindDirection = 0;
  float BarometricPressure = 0;
  float AerosolConcentration = 0;
  float TopScudFrequency = 0;
  float TopTransitionBandThickness = 0;
};

void sendWeatherControl(SScriptWeatherControl scriptWeatherControl)
{
  ECigiScope eScope = ConvertCigiStringToScope(scriptWeatherControl.Scope);
  int nLayerID = g_pScriptRuntime->GetLayerID(scriptWeatherControl.LayerID);
  EntityID entityID = EntityID(scriptWeatherControl.EntityID);
  RegionID regionID = RegionID(scriptWeatherControl.RegionID);

  SCigiWeatherCondition cigiWeatherCondition;
  CigiFloatPercentage humidity = CigiFloatPercentage(scriptWeatherControl.Humidity);
  cigiWeatherCondition.humidity = ConvertToPercentage(humidity);
  cigiWeatherCondition.bWeatherEnabled = scriptWeatherControl.WeatherEnable;
  cigiWeatherCondition.bBottomScudEnabled = scriptWeatherControl.BottomScudEnable;
  cigiWeatherCondition.bRandomWindsEnabled = scriptWeatherControl.RandomWindsEnable;
  cigiWeatherCondition.bRandomLightningEnabled = scriptWeatherControl.RandomLightningEnable;
  cigiWeatherCondition.cloudType = g_pScriptRuntime->GetCloudType(scriptWeatherControl.CloudType);
  cigiWeatherCondition.severity = WeatherSeverity(scriptWeatherControl.Severity);
  cigiWeatherCondition.bTopScudEnabled = scriptWeatherControl.TopScudEnable;
  cigiWeatherCondition.fAirTemperature = TemperatureCelsius(scriptWeatherControl.AirTemperature);
  cigiWeatherCondition.fVisibilityRange = scriptWeatherControl.VisibilityRange;

  CigiFloatPercentage bottomScudFrequency = CigiFloatPercentage(scriptWeatherControl.BottomScudFrequency);
  cigiWeatherCondition.bottomScudFrequency = ConvertToPercentage(bottomScudFrequency);
  CigiFloatPercentage coverage = CigiFloatPercentage(scriptWeatherControl.Coverage);
  cigiWeatherCondition.coverage = ConvertToPercentage(coverage);
  cigiWeatherCondition.fBarometricPressure = scriptWeatherControl.BarometricPressure;
  cigiWeatherCondition.fAerosolConcentration = scriptWeatherControl.AerosolConcentration;
  CigiFloatPercentage topScudFrequency = CigiFloatPercentage(scriptWeatherControl.TopScudFrequency);
  cigiWeatherCondition.topScudFrequency = ConvertToPercentage(topScudFrequency);

  SCigiSpatialWeatherCondition cigiSpatialWeatherCondition;
  cigiSpatialWeatherCondition.fBaseElevation = scriptWeatherControl.BaseElevation;
  cigiSpatialWeatherCondition.fThickness = scriptWeatherControl.Thickness;
  cigiSpatialWeatherCondition.fBottomTransitionBandThickness = scriptWeatherControl.BottomTransitionBandThickness;
  cigiSpatialWeatherCondition.fTopTransitionBandThickness = scriptWeatherControl.TopTransitionBandThickness;

  cigiWeatherCondition.HorizontalWindSpeed = scriptWeatherControl.HorizontalWindSpeed;
  cigiWeatherCondition.VerticalWindSpeed = scriptWeatherControl.VerticalWindSpeed;
  cigiWeatherCondition.WindDirection = scriptWeatherControl.WindDirection;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  if (eScope == ECigiScope::GLOBAL)
  {
    pHostSession->SendWeatherControl(GlobalLayeredWeatherID(static_cast<uint8_t>(nLayerID)), cigiWeatherCondition, cigiSpatialWeatherCondition);
  }
  else if (eScope == ECigiScope::REGIONAL)
  {
    pHostSession->SendWeatherControl(regionID, RegionalLayeredWeatherID(static_cast<uint8_t>(nLayerID)), cigiWeatherCondition, cigiSpatialWeatherCondition);
  }
  else if (eScope == ECigiScope::ENTITY)
  {
    pHostSession->SendWeatherControl(entityID, cigiWeatherCondition);
  }
}

void RegisterWeatherControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendWeatherControl), "sendWeatherControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptWeatherControl>(*m, "WeatherControl", {constructor<SScriptWeatherControl()>(), constructor<SScriptWeatherControl(const SScriptWeatherControl&)>()},
                                                        {
                                                          {fun(&SScriptWeatherControl::LayerID), "LayerID"},
                                                          {fun(&SScriptWeatherControl::Humidity), "Humidity"},
                                                          {fun(&SScriptWeatherControl::WeatherEnable), "WeatherEnable"},
                                                          {fun(&SScriptWeatherControl::BottomScudEnable), "BottomScudEnable"},
                                                          {fun(&SScriptWeatherControl::RandomWindsEnable), "RandomWindsEnable"},
                                                          {fun(&SScriptWeatherControl::RandomLightningEnable), "RandomLightningEnable"},
                                                          {fun(&SScriptWeatherControl::CloudType), "CloudType"},
                                                          {fun(&SScriptWeatherControl::Scope), "Scope"},
                                                          {fun(&SScriptWeatherControl::Severity), "Severity"},
                                                          {fun(&SScriptWeatherControl::TopScudEnable), "TopScudEnable"},
                                                          {fun(&SScriptWeatherControl::EntityID), "EntityID"},
                                                          {fun(&SScriptWeatherControl::RegionID), "RegionID"},
                                                          {fun(&SScriptWeatherControl::AirTemperature), "AirTemperature"},
                                                          {fun(&SScriptWeatherControl::VisibilityRange), "VisibilityRange"},
                                                          {fun(&SScriptWeatherControl::BottomScudFrequency), "BottomScudFrequency"},
                                                          {fun(&SScriptWeatherControl::Coverage), "Coverage"},
                                                          {fun(&SScriptWeatherControl::BaseElevation), "BaseElevation"},
                                                          {fun(&SScriptWeatherControl::Thickness), "Thickness"},
                                                          {fun(&SScriptWeatherControl::BottomTransitionBandThickness), "BottomTransitionBandThickness"},
                                                          {fun(&SScriptWeatherControl::HorizontalWindSpeed), "HorizontalWindSpeed"},
                                                          {fun(&SScriptWeatherControl::VerticalWindSpeed), "VerticalWindSpeed"},
                                                          {fun(&SScriptWeatherControl::WindDirection), "WindDirection"},
                                                          {fun(&SScriptWeatherControl::BarometricPressure), "BarometricPressure"},
                                                          {fun(&SScriptWeatherControl::AerosolConcentration), "AerosolConcentration"},
                                                          {fun(&SScriptWeatherControl::TopScudFrequency), "TopScudFrequency"},
                                                          {fun(&SScriptWeatherControl::TopTransitionBandThickness), "TopTransitionBandThickness"},
                                                        });

  chai.add(m);
}

struct SScriptEnvironmentalRegion
{
  uint16_t RegionID = 0;
  string RegionState;
  string MergeWeatherProperties;
  string MergeAerosolConcentrations;
  string MergeMaritimeSurfaceConditions;
  string MergeTerrestrialSurfaceConditions;
  double Latitude = 0;
  double Longitude = 0;
  float SizeX = 0;
  float SizeY = 0;
  float CornerRadius = 0;
  float Rotation = 0;
  float TransitionPerimeter = 0;
};

void sendEnvironmentalRegionControl(SScriptEnvironmentalRegion scriptEnvironmentalRegion)
{
  SCigiEnvironmentalRegion cigiEnvironmentalRegion;
  cigiEnvironmentalRegion.regionID = RegionID(scriptEnvironmentalRegion.RegionID);
  cigiEnvironmentalRegion.eRegionState = ConvertCigiStringToActiveState(scriptEnvironmentalRegion.RegionState);
  cigiEnvironmentalRegion.eMergeWeatherProperties = ConvertCigiStringToMergeState(scriptEnvironmentalRegion.MergeWeatherProperties);
  cigiEnvironmentalRegion.eMergeAerosolConcentrations = ConvertCigiStringToMergeState(scriptEnvironmentalRegion.MergeAerosolConcentrations);
  cigiEnvironmentalRegion.eMergeMaritimeSurfaceConditions = ConvertCigiStringToMergeState(scriptEnvironmentalRegion.MergeMaritimeSurfaceConditions);
  cigiEnvironmentalRegion.eMergeTerrestrialSurfaceConditions = ConvertCigiStringToMergeState(scriptEnvironmentalRegion.MergeTerrestrialSurfaceConditions);
  cigiEnvironmentalRegion.latitude = Latitude(scriptEnvironmentalRegion.Latitude);
  cigiEnvironmentalRegion.longitude = Longitude(scriptEnvironmentalRegion.Longitude);
  cigiEnvironmentalRegion.size[0] = scriptEnvironmentalRegion.SizeX;
  cigiEnvironmentalRegion.size[1] = scriptEnvironmentalRegion.SizeY;
  cigiEnvironmentalRegion.fCornerRadius = scriptEnvironmentalRegion.CornerRadius;
  cigiEnvironmentalRegion.fRotation = Degrees180(scriptEnvironmentalRegion.Rotation);
  cigiEnvironmentalRegion.fTransition = scriptEnvironmentalRegion.TransitionPerimeter;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendEnvironmentalRegionControl(cigiEnvironmentalRegion);
}

void RegisterEnvironmentalRegionControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEnvironmentalRegionControl), "sendEnvironmentalRegionControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptEnvironmentalRegion>(*m, "EnvironmentalRegionControl", {constructor<SScriptEnvironmentalRegion()>(), constructor<SScriptEnvironmentalRegion(const SScriptEnvironmentalRegion&)>()},
                                                             {
                                                               {fun(&SScriptEnvironmentalRegion::RegionID), "RegionID"},
                                                               {fun(&SScriptEnvironmentalRegion::RegionState), "RegionState"},
                                                               {fun(&SScriptEnvironmentalRegion::MergeWeatherProperties), "MergeWeatherProperties"},
                                                               {fun(&SScriptEnvironmentalRegion::MergeAerosolConcentrations), "MergeAerosolConcentrations"},
                                                               {fun(&SScriptEnvironmentalRegion::MergeMaritimeSurfaceConditions), "MergeMaritimeSurfaceConditions"},
                                                               {fun(&SScriptEnvironmentalRegion::MergeTerrestrialSurfaceConditions), "MergeTerrestrialSurfaceConditions"},
                                                               {fun(&SScriptEnvironmentalRegion::Latitude), "Latitude"},
                                                               {fun(&SScriptEnvironmentalRegion::Longitude), "Longitude"},
                                                               {fun(&SScriptEnvironmentalRegion::SizeX), "SizeX"},
                                                               {fun(&SScriptEnvironmentalRegion::SizeY), "SizeY"},
                                                               {fun(&SScriptEnvironmentalRegion::CornerRadius), "CornerRadius"},
                                                               {fun(&SScriptEnvironmentalRegion::Rotation), "Rotation"},
                                                               {fun(&SScriptEnvironmentalRegion::TransitionPerimeter), "TransitionPerimeter"},
                                                             });

  chai.add(m);
}

struct SScriptEarthReferenceModel
{
  bool CustomERMEnable = false;
  double EquatorialRadius = 0;
  double Flattening = 0;
};

void sendEarthReferenceModelDefinition(SScriptEarthReferenceModel scriptEarthReferenceDefinition)
{
  SCigiEarthReferenceModel cigiEarthReferenceModel;
  if (scriptEarthReferenceDefinition.CustomERMEnable == false)
  {
    cigiEarthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::WGS84;
  }
  else
  {
    cigiEarthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::HOST_DEFINED;
  }

  cigiEarthReferenceModel.fEquatorialRadius = scriptEarthReferenceDefinition.EquatorialRadius;
  cigiEarthReferenceModel.fFlattening = scriptEarthReferenceDefinition.Flattening;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendEarthReferenceModelDefinition(cigiEarthReferenceModel);
}

void RegisterEarthReferenceDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEarthReferenceModelDefinition), "sendEarthReferenceModelDefinition");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptEarthReferenceModel>(*m, "EarthReferenceModelDefinition", {constructor<SScriptEarthReferenceModel()>(), constructor<SScriptEarthReferenceModel(const SScriptEarthReferenceModel&)>()},
                                                             {
                                                               {fun(&SScriptEarthReferenceModel::CustomERMEnable), "CustomERMEnable"},
                                                               {fun(&SScriptEarthReferenceModel::EquatorialRadius), "EquatorialRadius"},
                                                               {fun(&SScriptEarthReferenceModel::Flattening), "Flattening"},
                                                             });

  chai.add(m);
}

struct SScriptEntityAccelerationControl
{
  uint16_t EntityID = 0;
  string CoordinateSystem;
  float XLinearAcceleration = 0;
  float YLinearAcceleration = 0;
  float ZLinearAcceleration = 0;
  float RollAngularAcceleration = 0;
  float PitchAngularAcceleration = 0;
  float YawAngularAcceleration = 0;
};

void sendEntityAccelerationControl(SScriptEntityAccelerationControl scriptAcceleration)
{
  SCigiEntityAcceleration cigiAcceleration;
  cigiAcceleration.entityID = EntityID(scriptAcceleration.EntityID);
  cigiAcceleration.eCoordinateSystem = ConvertCigiStringToObjectCoordinateSystem(scriptAcceleration.CoordinateSystem);
  cigiAcceleration.linearAcceleration[0] = scriptAcceleration.XLinearAcceleration;
  cigiAcceleration.linearAcceleration[1] = scriptAcceleration.YLinearAcceleration;
  cigiAcceleration.linearAcceleration[2] = scriptAcceleration.ZLinearAcceleration;
  cigiAcceleration.angularAcceleration.roll = Degrees(scriptAcceleration.RollAngularAcceleration);
  cigiAcceleration.angularAcceleration.pitch = Degrees(scriptAcceleration.PitchAngularAcceleration);
  cigiAcceleration.angularAcceleration.yaw = Degrees(scriptAcceleration.YawAngularAcceleration);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendEntityAccelerationControl(cigiAcceleration);
}

void RegisterEntityAccelerationControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEntityAccelerationControl), "sendEntityAccelerationControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptEntityAccelerationControl>(*m, "EntityAccelerationControl", {constructor<SScriptEntityAccelerationControl()>(), constructor<SScriptEntityAccelerationControl(const SScriptEntityAccelerationControl&)>()},
                                                                   {
                                                                     {fun(&SScriptEntityAccelerationControl::EntityID), "EntityID"},
                                                                     {fun(&SScriptEntityAccelerationControl::CoordinateSystem), "CoordinateSystem"},
                                                                     {fun(&SScriptEntityAccelerationControl::XLinearAcceleration), "XLinearAcceleration"},
                                                                     {fun(&SScriptEntityAccelerationControl::YLinearAcceleration), "YLinearAcceleration"},
                                                                     {fun(&SScriptEntityAccelerationControl::ZLinearAcceleration), "ZLinearAcceleration"},
                                                                     {fun(&SScriptEntityAccelerationControl::RollAngularAcceleration), "RollAngularAcceleration"},
                                                                     {fun(&SScriptEntityAccelerationControl::PitchAngularAcceleration), "PitchAngularAcceleration"},
                                                                     {fun(&SScriptEntityAccelerationControl::YawAngularAcceleration), "YawAngularAcceleration"},
                                                                   });

  chai.add(m);
}

struct SScriptArticulatedPartAccelerationControl
{
  uint16_t EntityID = 0;
  uint8_t ArticulatedPartID = 0;
  float XLinearAcceleration = 0;
  float YLinearAcceleration = 0;
  float ZLinearAcceleration = 0;
  float RollAngularAcceleration = 0;
  float PitchAngularAcceleration = 0;
  float YawAngularAcceleration = 0;
};

void sendArticulatedPartAccelerationControl(SScriptArticulatedPartAccelerationControl scriptAcceleration)
{
  SCigiArticulatedPartAcceleration cigiAcceleration;
  cigiAcceleration.entityID = EntityID(scriptAcceleration.EntityID);
  cigiAcceleration.articulatedPartID = ArticulatedPartID(scriptAcceleration.ArticulatedPartID);
  cigiAcceleration.linearAcceleration[0] = scriptAcceleration.XLinearAcceleration;
  cigiAcceleration.linearAcceleration[1] = scriptAcceleration.YLinearAcceleration;
  cigiAcceleration.linearAcceleration[2] = scriptAcceleration.ZLinearAcceleration;
  cigiAcceleration.angularAcceleration.roll = Degrees(scriptAcceleration.RollAngularAcceleration);
  cigiAcceleration.angularAcceleration.pitch = Degrees(scriptAcceleration.PitchAngularAcceleration);
  cigiAcceleration.angularAcceleration.yaw = Degrees(scriptAcceleration.YawAngularAcceleration);
  cigiAcceleration.eCoordinateSystem = EObjectCoordinateSystem::LOCAL;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendArticulatedPartAccelerationControl(cigiAcceleration);
}

void RegisterArticulatedPartAccelerationControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendArticulatedPartAccelerationControl), "sendArticulatedPartAccelerationControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptArticulatedPartAccelerationControl>(*m, "ArticulatedPartAccelerationControl", {constructor<SScriptArticulatedPartAccelerationControl()>(), constructor<SScriptArticulatedPartAccelerationControl(const SScriptArticulatedPartAccelerationControl&)>()},
                                                                            {
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::EntityID), "EntityID"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::ArticulatedPartID), "ArticulatedPartID"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::XLinearAcceleration), "XLinearAcceleration"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::YLinearAcceleration), "YLinearAcceleration"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::ZLinearAcceleration), "ZLinearAcceleration"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::RollAngularAcceleration), "RollAngularAcceleration"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::PitchAngularAcceleration), "PitchAngularAcceleration"},
                                                                              {fun(&SScriptArticulatedPartAccelerationControl::YawAngularAcceleration), "YawAngularAcceleration"},
                                                                            });

  chai.add(m);
}

struct SScriptHATHOTGlobalRequest
{
  uint16_t HATHOTID = 0;
  uint8_t UpdatePeriod = 0;
  float Latitude = 0;
  float Longitude = 0;
  float Altitude = 0;
  string RequestType;
};

struct SScriptHATHOTEntityRequest
{
  uint16_t HATHOTID = 0;
  uint16_t EntityID = 0;
  uint8_t UpdatePeriod = 0;
  double XOffset = 0;
  double YOffset = 0;
  double ZOffset = 0;
  string RequestType;
};

void sendHATHOTGlobalRequest(SScriptHATHOTGlobalRequest scriptHATHOTRequest)
{
  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  auto requestType = ConvertCigiStringToRequestType(scriptHATHOTRequest.RequestType);

  SHATHOTGlobalRequest request;
  request.requestID = HATHOTID(scriptHATHOTRequest.HATHOTID);
  request.updatePeriod = UpdatePeriod(scriptHATHOTRequest.UpdatePeriod);
  request.geodeticCoordinates.latitude = Latitude(scriptHATHOTRequest.Latitude);
  request.geodeticCoordinates.longitude = Longitude(scriptHATHOTRequest.Longitude);
  request.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptHATHOTRequest.Altitude);
  request.eRequestType = requestType;
  pHostSession->SendHatHotRequest(request);
}

void sendHATHOTEntityRequest(SScriptHATHOTEntityRequest scriptHATHOTRequest)
{
  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  auto requestType = ConvertCigiStringToRequestType(scriptHATHOTRequest.RequestType);
  sbio::math::Vec3 offset(scriptHATHOTRequest.XOffset, scriptHATHOTRequest.YOffset, scriptHATHOTRequest.ZOffset);

  SHATHOTEntityRequest request;
  request.requestID = HATHOTID(scriptHATHOTRequest.HATHOTID);
  request.updatePeriod = UpdatePeriod(scriptHATHOTRequest.UpdatePeriod);
  request.entityID = EntityID(scriptHATHOTRequest.EntityID);
  request.offset = offset;
  request.eRequestType = requestType;
  pHostSession->SendHatHotRequest(request);
}

void RegisterHATHOTGlobalRequest(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendHATHOTGlobalRequest), "sendHATHOTGlobalRequest");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptHATHOTGlobalRequest>(*m, "HATHOTGlobalRequest",
                                                             {
                                                               constructor<SScriptHATHOTGlobalRequest()>(),
                                                               constructor<SScriptHATHOTGlobalRequest(const SScriptHATHOTGlobalRequest&)>(),
                                                             },
                                                             {
                                                               {fun(&SScriptHATHOTGlobalRequest::HATHOTID), "HATHOTID"},
                                                               {fun(&SScriptHATHOTGlobalRequest::UpdatePeriod), "UpdatePeriod"},
                                                               {fun(&SScriptHATHOTGlobalRequest::Latitude), "Latitude"},
                                                               {fun(&SScriptHATHOTGlobalRequest::Longitude), "Longitude"},
                                                               {fun(&SScriptHATHOTGlobalRequest::Altitude), "Altitude"},
                                                               {fun(&SScriptHATHOTGlobalRequest::RequestType), "RequestType"},
                                                             });

  chai.add(m);
}

void RegisterHATHOTEntityRequest(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendHATHOTEntityRequest), "sendHATHOTEntityRequest");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptHATHOTEntityRequest>(*m, "HATHOTEntityRequest", {constructor<SScriptHATHOTEntityRequest()>(), constructor<SScriptHATHOTEntityRequest(const SScriptHATHOTEntityRequest&)>()},
                                                             {
                                                               {fun(&SScriptHATHOTEntityRequest::HATHOTID), "HATHOTID"},
                                                               {fun(&SScriptHATHOTEntityRequest::EntityID), "EntityID"},
                                                               {fun(&SScriptHATHOTEntityRequest::UpdatePeriod), "UpdatePeriod"},
                                                               {fun(&SScriptHATHOTEntityRequest::XOffset), "XOffset"},
                                                               {fun(&SScriptHATHOTEntityRequest::YOffset), "YOffset"},
                                                               {fun(&SScriptHATHOTEntityRequest::ZOffset), "ZOffset"},
                                                               {fun(&SScriptHATHOTEntityRequest::RequestType), "RequestType"},
                                                             });

  chai.add(m);
}

void Pause(float fSeconds)
{
  float fMilliseconds = fSeconds * 1000;
  int nMilliseconds = static_cast<int>(fMilliseconds);

  boost::this_thread::sleep_for(boost::chrono::milliseconds(nMilliseconds));
}

void Wait(float fSeconds, string sCallback)
{
  if (g_HostCigiLibGlobals.pHost == nullptr)
  {
    return;
  }

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  if (pHostSession == nullptr)
  {
    return;
  }

  SWaitCallback waitCallback;
  waitCallback.fTime = static_cast<float>(pHostSession->GetSessionTime()) + fSeconds;
  waitCallback.sCallback = sCallback;

  g_pScriptRuntime->AddWaitCallback(waitCallback);
}

struct SScriptGlobalMaritimeSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  bool WhitecapEnable = false;
  float SeaSurfaceHeight = 0;
  float SurfaceWaterTemperature = 0;
  float SurfaceClarity = 0;
};

struct SScriptRegionMaritimeSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  bool WhitecapEnable = false;
  float SeaSurfaceHeight = 0;
  float SurfaceWaterTemperature = 0;
  float SurfaceClarity = 0;
  uint16_t RegionID = 0;
};

struct SScriptEntityMaritimeSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  bool WhitecapEnable = false;
  float SeaSurfaceHeight = 0;
  float SurfaceWaterTemperature = 0;
  float SurfaceClarity = 0;
  uint16_t EntityID = 0;
};

struct SScriptMaritimeSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  bool WhitecapEnable = false;
  string Scope;
  float SeaSurfaceHeight = 0;
  float SurfaceWaterTemperature = 0;
  float SurfaceClarity = 0;
  uint16_t RegionID = 0;
  uint16_t EntityID = 0;
};

void sendGlobalMaritimeSurfaceConditionsControl(SScriptGlobalMaritimeSurfaceConditionsControl scriptMaritimeSurfaceConditionsControl)
{
  SCigiMaritimeSurfaceCondition cigiMaritimeSurfaceCondition;
  cigiMaritimeSurfaceCondition.bActive = scriptMaritimeSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiMaritimeSurfaceCondition.bWhitecapEnabled = scriptMaritimeSurfaceConditionsControl.WhitecapEnable;
  cigiMaritimeSurfaceCondition.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(scriptMaritimeSurfaceConditionsControl.SeaSurfaceHeight);
  cigiMaritimeSurfaceCondition.fSurfaceWaterTemperature = TemperatureCelsius(scriptMaritimeSurfaceConditionsControl.SurfaceWaterTemperature);
  CigiFloatPercentage surfaceClarity = CigiFloatPercentage(scriptMaritimeSurfaceConditionsControl.SurfaceClarity);
  cigiMaritimeSurfaceCondition.surfaceClarity = ConvertToPercentage(surfaceClarity);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();

  pSession->SendGlobalMaritimeSurfaceConditionsControl(cigiMaritimeSurfaceCondition);
}

void sendRegionMaritimeSurfaceConditionsControl(SScriptRegionMaritimeSurfaceConditionsControl scriptMaritimeSurfaceConditionsControl)
{
  SCigiMaritimeSurfaceCondition cigiMaritimeSurfaceCondition;
  cigiMaritimeSurfaceCondition.bActive = scriptMaritimeSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiMaritimeSurfaceCondition.bWhitecapEnabled = scriptMaritimeSurfaceConditionsControl.WhitecapEnable;
  cigiMaritimeSurfaceCondition.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(scriptMaritimeSurfaceConditionsControl.SeaSurfaceHeight);
  cigiMaritimeSurfaceCondition.fSurfaceWaterTemperature = TemperatureCelsius(scriptMaritimeSurfaceConditionsControl.SurfaceWaterTemperature);
  CigiFloatPercentage surfaceClarity = CigiFloatPercentage(scriptMaritimeSurfaceConditionsControl.SurfaceClarity);
  cigiMaritimeSurfaceCondition.surfaceClarity = ConvertToPercentage(surfaceClarity);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();

  RegionID regionID = RegionID(scriptMaritimeSurfaceConditionsControl.RegionID);
  pSession->SendRegionMaritimeSurfaceConditionsControl(regionID, cigiMaritimeSurfaceCondition);
}

void sendEntityMaritimeSurfaceConditionsControl(SScriptEntityMaritimeSurfaceConditionsControl scriptMaritimeSurfaceConditionsControl)
{
  SCigiMaritimeSurfaceCondition cigiMaritimeSurfaceCondition;
  cigiMaritimeSurfaceCondition.bActive = scriptMaritimeSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiMaritimeSurfaceCondition.bWhitecapEnabled = scriptMaritimeSurfaceConditionsControl.WhitecapEnable;
  cigiMaritimeSurfaceCondition.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(scriptMaritimeSurfaceConditionsControl.SeaSurfaceHeight);
  cigiMaritimeSurfaceCondition.fSurfaceWaterTemperature = TemperatureCelsius(scriptMaritimeSurfaceConditionsControl.SurfaceWaterTemperature);
  CigiFloatPercentage surfaceClarity = CigiFloatPercentage(scriptMaritimeSurfaceConditionsControl.SurfaceClarity);
  cigiMaritimeSurfaceCondition.surfaceClarity = ConvertToPercentage(surfaceClarity);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();

  EntityID entityID = EntityID(scriptMaritimeSurfaceConditionsControl.EntityID);
  pSession->SendEntityMaritimeSurfaceConditionsControl(entityID, cigiMaritimeSurfaceCondition);
}

void sendMaritimeSurfaceConditionsControl(SScriptMaritimeSurfaceConditionsControl scriptMaritimeSurfaceConditionsControl)
{
  SCigiMaritimeSurfaceCondition cigiMaritimeSurfaceCondition;
  cigiMaritimeSurfaceCondition.bActive = scriptMaritimeSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiMaritimeSurfaceCondition.bWhitecapEnabled = scriptMaritimeSurfaceConditionsControl.WhitecapEnable;
  cigiMaritimeSurfaceCondition.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(scriptMaritimeSurfaceConditionsControl.SeaSurfaceHeight);
  cigiMaritimeSurfaceCondition.fSurfaceWaterTemperature = TemperatureCelsius(scriptMaritimeSurfaceConditionsControl.SurfaceWaterTemperature);
  CigiFloatPercentage surfaceClarity = CigiFloatPercentage(scriptMaritimeSurfaceConditionsControl.SurfaceClarity);
  cigiMaritimeSurfaceCondition.surfaceClarity = ConvertToPercentage(surfaceClarity);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  ECigiScope scope = ConvertCigiStringToScope(scriptMaritimeSurfaceConditionsControl.Scope);

  if (scope == ECigiScope::ENTITY)
  {
    EntityID entityID = EntityID(scriptMaritimeSurfaceConditionsControl.EntityID);
    pSession->SendEntityMaritimeSurfaceConditionsControl(entityID, cigiMaritimeSurfaceCondition);
  }
  else if (scope == ECigiScope::REGIONAL)
  {
    RegionID regionID = RegionID(scriptMaritimeSurfaceConditionsControl.RegionID);
    pSession->SendRegionMaritimeSurfaceConditionsControl(regionID, cigiMaritimeSurfaceCondition);
  }
  else if (scope == ECigiScope::GLOBAL)
  {
    pSession->SendGlobalMaritimeSurfaceConditionsControl(cigiMaritimeSurfaceCondition);
  }
}

void RegisterGlobalMaritimeSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendGlobalMaritimeSurfaceConditionsControl), "sendGlobalMaritimeSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptGlobalMaritimeSurfaceConditionsControl>(*m, "GlobalMaritimeSurfaceConditionsControl",
                                                                                {constructor<SScriptGlobalMaritimeSurfaceConditionsControl()>(), constructor<SScriptGlobalMaritimeSurfaceConditionsControl(const SScriptGlobalMaritimeSurfaceConditionsControl&)>()},
                                                                                {
                                                                                  {fun(&SScriptGlobalMaritimeSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                                  {fun(&SScriptGlobalMaritimeSurfaceConditionsControl::WhitecapEnable), "WhitecapEnable"},
                                                                                  {fun(&SScriptGlobalMaritimeSurfaceConditionsControl::SeaSurfaceHeight), "SeaSurfaceHeight"},
                                                                                  {fun(&SScriptGlobalMaritimeSurfaceConditionsControl::SurfaceWaterTemperature), "SurfaceWaterTemperature"},
                                                                                  {fun(&SScriptGlobalMaritimeSurfaceConditionsControl::SurfaceClarity), "SurfaceClarity"},
                                                                                });

  chai.add(m);
}

void RegisterRegionMaritimeSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendRegionMaritimeSurfaceConditionsControl), "sendRegionMaritimeSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptRegionMaritimeSurfaceConditionsControl>(*m, "RegionMaritimeSurfaceConditionsControl",
                                                                                {constructor<SScriptRegionMaritimeSurfaceConditionsControl()>(), constructor<SScriptRegionMaritimeSurfaceConditionsControl(const SScriptRegionMaritimeSurfaceConditionsControl&)>()},
                                                                                {
                                                                                  {fun(&SScriptRegionMaritimeSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                                  {fun(&SScriptRegionMaritimeSurfaceConditionsControl::WhitecapEnable), "WhitecapEnable"},
                                                                                  {fun(&SScriptRegionMaritimeSurfaceConditionsControl::RegionID), "RegionID"},
                                                                                  {fun(&SScriptRegionMaritimeSurfaceConditionsControl::SeaSurfaceHeight), "SeaSurfaceHeight"},
                                                                                  {fun(&SScriptRegionMaritimeSurfaceConditionsControl::SurfaceWaterTemperature), "SurfaceWaterTemperature"},
                                                                                  {fun(&SScriptRegionMaritimeSurfaceConditionsControl::SurfaceClarity), "SurfaceClarity"},
                                                                                });

  chai.add(m);
}

void RegisterEntityMaritimeSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEntityMaritimeSurfaceConditionsControl), "sendEntityMaritimeSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptEntityMaritimeSurfaceConditionsControl>(*m, "EntityMaritimeSurfaceConditionsControl",
                                                                                {constructor<SScriptEntityMaritimeSurfaceConditionsControl()>(), constructor<SScriptEntityMaritimeSurfaceConditionsControl(const SScriptEntityMaritimeSurfaceConditionsControl&)>()},
                                                                                {
                                                                                  {fun(&SScriptEntityMaritimeSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                                  {fun(&SScriptEntityMaritimeSurfaceConditionsControl::WhitecapEnable), "WhitecapEnable"},
                                                                                  {fun(&SScriptEntityMaritimeSurfaceConditionsControl::EntityID), "EntityID"},
                                                                                  {fun(&SScriptEntityMaritimeSurfaceConditionsControl::SeaSurfaceHeight), "SeaSurfaceHeight"},
                                                                                  {fun(&SScriptEntityMaritimeSurfaceConditionsControl::SurfaceWaterTemperature), "SurfaceWaterTemperature"},
                                                                                  {fun(&SScriptEntityMaritimeSurfaceConditionsControl::SurfaceClarity), "SurfaceClarity"},
                                                                                });

  chai.add(m);
}

void RegisterMaritimeSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendMaritimeSurfaceConditionsControl), "sendMaritimeSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptMaritimeSurfaceConditionsControl>(*m, "MaritimeSurfaceConditionsControl",
                                                                          {constructor<SScriptMaritimeSurfaceConditionsControl()>(), constructor<SScriptMaritimeSurfaceConditionsControl(const SScriptMaritimeSurfaceConditionsControl&)>()},
                                                                          {
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::WhitecapEnable), "WhitecapEnable"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::Scope), "Scope"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::RegionID), "RegionID"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::EntityID), "EntityID"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::SeaSurfaceHeight), "SeaSurfaceHeight"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::SurfaceWaterTemperature), "SurfaceWaterTemperature"},
                                                                            {fun(&SScriptMaritimeSurfaceConditionsControl::SurfaceClarity), "SurfaceClarity"},
                                                                          });

  chai.add(m);
}

struct ScriptEntityTerrestrialSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  uint16_t SurfaceConditionID = 0;
  uint8_t Severity = 0;// 0 - 31 (least to most severe)
  uint8_t Coverage = 0;// 0 - 100
  uint16_t EntityID = 0;// environmental entity ID
};

struct ScriptRegionTerrestrialSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  uint16_t SurfaceConditionID = 0;
  uint8_t Severity = 0;// 0 - 31 (least to most severe)
  uint8_t Coverage = 0;// 0 - 100
  string Scope;
  uint16_t RegionID = 0;
};

struct ScriptGlobalTerrestrialSurfaceConditionsControl
{
  bool SurfaceConditionsEnable = false;
  uint16_t SurfaceConditionID = 0;
  uint8_t Severity = 0;// 0 - 31 (least to most severe)
  uint8_t Coverage = 0;// 0 - 100
};

void sendEntityTerrestrialSurfaceConditionsControl(const ScriptEntityTerrestrialSurfaceConditionsControl& scriptTerrestrialSurfaceConditionsControl)
{
  SCigiTerrestrialSurfaceCondition cigiTerrestrialSurfaceCondition;
  cigiTerrestrialSurfaceCondition.bEnabled = scriptTerrestrialSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiTerrestrialSurfaceCondition.surfaceConditionID = SurfaceConditionID(scriptTerrestrialSurfaceConditionsControl.SurfaceConditionID);
  CigiTerrestrialSurfaceSeverity severity = CigiTerrestrialSurfaceSeverity(scriptTerrestrialSurfaceConditionsControl.Severity);
  cigiTerrestrialSurfaceCondition.severity = ConvertToPercentage(severity);
  CigiIntPercentage coverage = CigiIntPercentage(scriptTerrestrialSurfaceConditionsControl.Coverage);
  cigiTerrestrialSurfaceCondition.coverage = ConvertToPercentage(coverage);
  EntityID entityID = EntityID(scriptTerrestrialSurfaceConditionsControl.EntityID);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendEntityTerrestrialSurfaceConditionsControl(entityID, cigiTerrestrialSurfaceCondition);
}

void sendRegionTerrestrialSurfaceConditionsControl(const ScriptRegionTerrestrialSurfaceConditionsControl& scriptTerrestrialSurfaceConditionsControl)
{
  SCigiTerrestrialSurfaceCondition cigiTerrestrialSurfaceCondition;
  cigiTerrestrialSurfaceCondition.bEnabled = scriptTerrestrialSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiTerrestrialSurfaceCondition.surfaceConditionID = SurfaceConditionID(scriptTerrestrialSurfaceConditionsControl.SurfaceConditionID);
  CigiTerrestrialSurfaceSeverity severity = CigiTerrestrialSurfaceSeverity(scriptTerrestrialSurfaceConditionsControl.Severity);
  cigiTerrestrialSurfaceCondition.severity = ConvertToPercentage(severity);
  CigiIntPercentage coverage = CigiIntPercentage(scriptTerrestrialSurfaceConditionsControl.Coverage);
  cigiTerrestrialSurfaceCondition.coverage = ConvertToPercentage(coverage);
  RegionID regionID = RegionID(scriptTerrestrialSurfaceConditionsControl.RegionID);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendRegionTerrestrialSurfaceConditionsControl(regionID, cigiTerrestrialSurfaceCondition);
}

void sendGlobalTerrestrialSurfaceConditionsControl(const ScriptGlobalTerrestrialSurfaceConditionsControl& scriptTerrestrialSurfaceConditionsControl)
{
  SCigiTerrestrialSurfaceCondition cigiTerrestrialSurfaceCondition;
  cigiTerrestrialSurfaceCondition.bEnabled = scriptTerrestrialSurfaceConditionsControl.SurfaceConditionsEnable;
  cigiTerrestrialSurfaceCondition.surfaceConditionID = SurfaceConditionID(scriptTerrestrialSurfaceConditionsControl.SurfaceConditionID);
  CigiTerrestrialSurfaceSeverity severity = CigiTerrestrialSurfaceSeverity(scriptTerrestrialSurfaceConditionsControl.Severity);
  cigiTerrestrialSurfaceCondition.severity = ConvertToPercentage(severity);
  CigiIntPercentage coverage = CigiIntPercentage(scriptTerrestrialSurfaceConditionsControl.Coverage);
  cigiTerrestrialSurfaceCondition.coverage = ConvertToPercentage(coverage);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendGlobalTerrestrialSurfaceConditionsControl(cigiTerrestrialSurfaceCondition);
}

void RegisterGlobalTerrestrialSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(static_cast<void (*)(const ScriptGlobalTerrestrialSurfaceConditionsControl&)>(&sendGlobalTerrestrialSurfaceConditionsControl)), "sendGlobalTerrestrialSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<ScriptGlobalTerrestrialSurfaceConditionsControl>(*m, "GlobalTerrestrialSurfaceConditionsControl",
                                                                                  {constructor<ScriptGlobalTerrestrialSurfaceConditionsControl()>(), constructor<ScriptGlobalTerrestrialSurfaceConditionsControl(const ScriptGlobalTerrestrialSurfaceConditionsControl&)>()},
                                                                                  {
                                                                                    {fun(&ScriptGlobalTerrestrialSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                                    {fun(&ScriptGlobalTerrestrialSurfaceConditionsControl::SurfaceConditionID), "SurfaceConditionID"},
                                                                                    {fun(&ScriptGlobalTerrestrialSurfaceConditionsControl::Severity), "Severity"},
                                                                                    {fun(&ScriptGlobalTerrestrialSurfaceConditionsControl::Coverage), "Coverage"},
                                                                                  });

  chai.add(m);
}

void RegisterRegionTerrestrialSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(static_cast<void (*)(const ScriptRegionTerrestrialSurfaceConditionsControl&)>(&sendRegionTerrestrialSurfaceConditionsControl)), "sendRegionTerrestrialSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<ScriptRegionTerrestrialSurfaceConditionsControl>(*m, "RegionTerrestrialSurfaceConditionsControl",
                                                                                  {constructor<ScriptRegionTerrestrialSurfaceConditionsControl()>(), constructor<ScriptRegionTerrestrialSurfaceConditionsControl(const ScriptRegionTerrestrialSurfaceConditionsControl&)>()},
                                                                                  {
                                                                                    {fun(&ScriptRegionTerrestrialSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                                    {fun(&ScriptRegionTerrestrialSurfaceConditionsControl::SurfaceConditionID), "SurfaceConditionID"},
                                                                                    {fun(&ScriptRegionTerrestrialSurfaceConditionsControl::Severity), "Severity"},
                                                                                    {fun(&ScriptRegionTerrestrialSurfaceConditionsControl::Coverage), "Coverage"},
                                                                                    {fun(&ScriptRegionTerrestrialSurfaceConditionsControl::RegionID), "RegionID"},
                                                                                  });

  chai.add(m);
}

void RegisterEntityTerrestrialSurfaceConditionsControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(static_cast<void (*)(const ScriptEntityTerrestrialSurfaceConditionsControl&)>(&sendEntityTerrestrialSurfaceConditionsControl)), "sendEntityTerrestrialSurfaceConditionsControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<ScriptEntityTerrestrialSurfaceConditionsControl>(*m, "EntityTerrestrialSurfaceConditionsControl",
                                                                                  {constructor<ScriptEntityTerrestrialSurfaceConditionsControl()>(), constructor<ScriptEntityTerrestrialSurfaceConditionsControl(const ScriptEntityTerrestrialSurfaceConditionsControl&)>()},
                                                                                  {
                                                                                    {fun(&ScriptEntityTerrestrialSurfaceConditionsControl::SurfaceConditionsEnable), "SurfaceConditionsEnable"},
                                                                                    {fun(&ScriptEntityTerrestrialSurfaceConditionsControl::SurfaceConditionID), "SurfaceConditionID"},
                                                                                    {fun(&ScriptEntityTerrestrialSurfaceConditionsControl::Severity), "Severity"},
                                                                                    {fun(&ScriptEntityTerrestrialSurfaceConditionsControl::Coverage), "Coverage"},
                                                                                    {fun(&ScriptEntityTerrestrialSurfaceConditionsControl::EntityID), "EntityID"},
                                                                                  });

  chai.add(m);
}

struct SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  double SourceLatitude = 0;
  double SourceLongitude = 0;
  double SourceAltitude = 0;
  double DestinationLatitude = 0;
  double DestinationLongitude = 0;
  double DestinationAltitude = 0;
};

struct SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  double SourceLatitude = 0;
  double SourceLongitude = 0;
  double SourceAltitude = 0;
  double DestinationLatitude = 0;
  double DestinationLongitude = 0;
  double DestinationAltitude = 0;
  string ResponseCoordinateSystem;
};

struct SScriptLineOfSightSegmentRequestGeodeticToEntityBasic
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  double SourceLatitude = 0;
  double SourceLongitude = 0;
  double SourceAltitude = 0;
  uint16_t DestinationEntityID = 0;
  double DestinationXOffset = 0;
  double DestinationYOffset = 0;
  double DestinationZOffset = 0;
};

struct SScriptLineOfSightSegmentRequestGeodeticToEntityExtended
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  double SourceLatitude = 0;
  double SourceLongitude = 0;
  double SourceAltitude = 0;
  uint16_t DestinationEntityID = 0;
  double DestinationXOffset = 0;
  double DestinationYOffset = 0;
  double DestinationZOffset = 0;
  string ResponseCoordinateSystem;
};

struct SScriptLineOfSightSegmentRequestEntityToGeodeticBasic
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  uint16_t SourceEntityID = 0;
  double SourceXOffset = 0;
  double SourceYOffset = 0;
  double SourceZOffset = 0;
  double DestinationLatitude = 0;
  double DestinationLongitude = 0;
  double DestinationAltitude = 0;
};

struct SScriptLineOfSightSegmentRequestEntityToGeodeticExtended
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  uint16_t SourceEntityID = 0;
  double SourceXOffset = 0;
  double SourceYOffset = 0;
  double SourceZOffset = 0;
  double DestinationLatitude = 0;
  double DestinationLongitude = 0;
  double DestinationAltitude = 0;
  string ResponseCoordinateSystem;
};

struct SScriptLineOfSightSegmentRequestEntityToEntityBasic
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  uint16_t SourceEntityID = 0;
  double SourceXOffset = 0;
  double SourceYOffset = 0;
  double SourceZOffset = 0;
  uint16_t DestinationEntityID = 0;
  double DestinationXOffset = 0;
  double DestinationYOffset = 0;
  double DestinationZOffset = 0;
};

struct SScriptLineOfSightSegmentRequestEntityToEntityExtended
{
  uint16_t LOSID = 0;
  uint8_t UpdatePeriod = 0;
  uint8_t AlphaThreshold = 0;
  int MaterialMask = 0;
  uint16_t SourceEntityID = 0;
  double SourceXOffset = 0;
  double SourceYOffset = 0;
  double SourceZOffset = 0;
  uint16_t DestinationEntityID = 0;
  double DestinationXOffset = 0;
  double DestinationYOffset = 0;
  double DestinationZOffset = 0;
  string ResponseCoordinateSystem;
};

void sendLineOfSightSegmentRequestGeodeticToGeodeticBasic(SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestGeodeticToGeodeticBasic lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.SourceLatitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.SourceLongitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.SourceAltitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.DestinationLatitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.DestinationLongitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.DestinationAltitude);
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestGeodeticToGeodeticBasic(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestGeodeticToGeodeticExtended(SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestGeodeticToGeodeticExtended lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.SourceLatitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.SourceLongitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.SourceAltitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.DestinationLatitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.DestinationLongitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.DestinationAltitude);
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;
  lineOfSightSegmentRequest.eResponseCoordinateSystem = ConvertCigiStringToCoordinateSystem(scriptLineOfSightSegmentRequest.ResponseCoordinateSystem);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestGeodeticToGeodeticExtended(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestGeodeticToEntityBasic(SScriptLineOfSightSegmentRequestGeodeticToEntityBasic scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestGeodeticToEntityBasic lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.destinationEntityID = EntityID(scriptLineOfSightSegmentRequest.DestinationEntityID);
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.SourceLatitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.SourceLongitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.SourceAltitude);
  lineOfSightSegmentRequest.destinationOffset[0] = scriptLineOfSightSegmentRequest.DestinationXOffset;
  lineOfSightSegmentRequest.destinationOffset[1] = scriptLineOfSightSegmentRequest.DestinationYOffset;
  lineOfSightSegmentRequest.destinationOffset[2] = scriptLineOfSightSegmentRequest.DestinationZOffset;
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestGeodeticToEntityBasic(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestGeodeticToEntityExtended(SScriptLineOfSightSegmentRequestGeodeticToEntityExtended scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestGeodeticToEntityExtended lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.destinationEntityID = EntityID(scriptLineOfSightSegmentRequest.DestinationEntityID);
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.SourceLatitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.SourceLongitude);
  lineOfSightSegmentRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.SourceAltitude);
  lineOfSightSegmentRequest.destinationOffset[0] = scriptLineOfSightSegmentRequest.DestinationXOffset;
  lineOfSightSegmentRequest.destinationOffset[1] = scriptLineOfSightSegmentRequest.DestinationYOffset;
  lineOfSightSegmentRequest.destinationOffset[2] = scriptLineOfSightSegmentRequest.DestinationZOffset;
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;
  lineOfSightSegmentRequest.eResponseCoordinateSystem = ConvertCigiStringToCoordinateSystem(scriptLineOfSightSegmentRequest.ResponseCoordinateSystem);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestGeodeticToEntityExtended(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestEntityToGeodeticBasic(SScriptLineOfSightSegmentRequestEntityToGeodeticBasic scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestEntityToGeodeticBasic lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.sourceEntityID = EntityID(scriptLineOfSightSegmentRequest.SourceEntityID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceOffset[0] = scriptLineOfSightSegmentRequest.SourceXOffset;
  lineOfSightSegmentRequest.sourceOffset[1] = scriptLineOfSightSegmentRequest.SourceYOffset;
  lineOfSightSegmentRequest.sourceOffset[2] = scriptLineOfSightSegmentRequest.SourceZOffset;
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.DestinationLatitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.DestinationLongitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.DestinationAltitude);
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestEntityToGeodeticBasic(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestEntityToGeodeticExtended(SScriptLineOfSightSegmentRequestEntityToGeodeticExtended scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestEntityToGeodeticExtended lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.sourceEntityID = EntityID(scriptLineOfSightSegmentRequest.SourceEntityID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceOffset[0] = scriptLineOfSightSegmentRequest.SourceXOffset;
  lineOfSightSegmentRequest.sourceOffset[1] = scriptLineOfSightSegmentRequest.SourceYOffset;
  lineOfSightSegmentRequest.sourceOffset[2] = scriptLineOfSightSegmentRequest.SourceZOffset;
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.latitude = Latitude(scriptLineOfSightSegmentRequest.DestinationLatitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.longitude = Longitude(scriptLineOfSightSegmentRequest.DestinationLongitude);
  lineOfSightSegmentRequest.destinationGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightSegmentRequest.DestinationAltitude);
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;
  lineOfSightSegmentRequest.eResponseCoordinateSystem = ConvertCigiStringToCoordinateSystem(scriptLineOfSightSegmentRequest.ResponseCoordinateSystem);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestEntityToGeodeticExtended(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestEntityToEntityBasic(SScriptLineOfSightSegmentRequestEntityToEntityBasic scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestEntityToEntityBasic lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.sourceEntityID = EntityID(scriptLineOfSightSegmentRequest.SourceEntityID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.destinationEntityID = EntityID(scriptLineOfSightSegmentRequest.DestinationEntityID);
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceOffset[0] = scriptLineOfSightSegmentRequest.SourceXOffset;
  lineOfSightSegmentRequest.sourceOffset[1] = scriptLineOfSightSegmentRequest.SourceYOffset;
  lineOfSightSegmentRequest.sourceOffset[2] = scriptLineOfSightSegmentRequest.SourceZOffset;
  lineOfSightSegmentRequest.destinationOffset[0] = scriptLineOfSightSegmentRequest.DestinationXOffset;
  lineOfSightSegmentRequest.destinationOffset[1] = scriptLineOfSightSegmentRequest.DestinationYOffset;
  lineOfSightSegmentRequest.destinationOffset[2] = scriptLineOfSightSegmentRequest.DestinationZOffset;
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestEntityToEntityBasic(lineOfSightSegmentRequest);
}

void sendLineOfSightSegmentRequestEntityToEntityExtended(SScriptLineOfSightSegmentRequestEntityToEntityExtended scriptLineOfSightSegmentRequest)
{
  SLineOfSightSegmentRequestEntityToEntityExtended lineOfSightSegmentRequest;
  lineOfSightSegmentRequest.requestID = LineOfSightRequestID(scriptLineOfSightSegmentRequest.LOSID);
  lineOfSightSegmentRequest.sourceEntityID = EntityID(scriptLineOfSightSegmentRequest.SourceEntityID);
  lineOfSightSegmentRequest.nAlphaThreshold = scriptLineOfSightSegmentRequest.AlphaThreshold;
  lineOfSightSegmentRequest.destinationEntityID = EntityID(scriptLineOfSightSegmentRequest.DestinationEntityID);
  lineOfSightSegmentRequest.updatePeriod = UpdatePeriod(scriptLineOfSightSegmentRequest.UpdatePeriod);
  lineOfSightSegmentRequest.sourceOffset[0] = scriptLineOfSightSegmentRequest.SourceXOffset;
  lineOfSightSegmentRequest.sourceOffset[1] = scriptLineOfSightSegmentRequest.SourceYOffset;
  lineOfSightSegmentRequest.sourceOffset[2] = scriptLineOfSightSegmentRequest.SourceZOffset;
  lineOfSightSegmentRequest.destinationOffset[0] = scriptLineOfSightSegmentRequest.DestinationXOffset;
  lineOfSightSegmentRequest.destinationOffset[1] = scriptLineOfSightSegmentRequest.DestinationYOffset;
  lineOfSightSegmentRequest.destinationOffset[2] = scriptLineOfSightSegmentRequest.DestinationZOffset;
  lineOfSightSegmentRequest.nMaterialMask = scriptLineOfSightSegmentRequest.MaterialMask;
  lineOfSightSegmentRequest.eResponseCoordinateSystem = ConvertCigiStringToCoordinateSystem(scriptLineOfSightSegmentRequest.ResponseCoordinateSystem);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightSegmentRequestEntityToEntityExtended(lineOfSightSegmentRequest);
}

void RegisterLineOfSightSegmentRequestGeodeticToGeodeticBasic(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestGeodeticToGeodeticBasic), "sendLineOfSightSegmentRequestGeodeticToGeodeticBasic");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic>(
    *m, "LineOfSightSegmentRequestGeodeticToGeodeticBasic", {constructor<SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic()>(), constructor<SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic(const SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic&)>()},
    {
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::LOSID), "LOSID"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::UpdatePeriod), "UpdatePeriod"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::AlphaThreshold), "AlphaThreshold"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::MaterialMask), "MaterialMask"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::SourceLatitude), "SourceLatitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::SourceLongitude), "SourceLongitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::SourceAltitude), "SourceAltitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::DestinationLatitude), "DestinationLatitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::DestinationLongitude), "DestinationLongitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticBasic::DestinationAltitude), "DestinationAltitude"},
    });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestGeodeticToGeodeticExtended(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestGeodeticToGeodeticExtended), "sendLineOfSightSegmentRequestGeodeticToGeodeticExtended");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended>(
    *m, "LineOfSightSegmentRequestGeodeticToGeodeticExtended", {constructor<SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended()>(), constructor<SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended(const SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended&)>()},
    {
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::LOSID), "LOSID"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::UpdatePeriod), "UpdatePeriod"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::AlphaThreshold), "AlphaThreshold"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::MaterialMask), "MaterialMask"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::SourceLatitude), "SourceLatitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::SourceLongitude), "SourceLongitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::SourceAltitude), "SourceAltitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::DestinationLatitude), "DestinationLatitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::DestinationLongitude), "DestinationLongitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::DestinationAltitude), "DestinationAltitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToGeodeticExtended::ResponseCoordinateSystem), "ResponseCoordinateSystem"},
    });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestGeodeticToEntityBasic(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestGeodeticToEntityBasic), "sendLineOfSightSegmentRequestGeodeticToEntityBasic");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestGeodeticToEntityBasic>(*m, "LineOfSightSegmentRequestGeodeticToEntityBasic",
                                                                                        {constructor<SScriptLineOfSightSegmentRequestGeodeticToEntityBasic()>(), constructor<SScriptLineOfSightSegmentRequestGeodeticToEntityBasic(const SScriptLineOfSightSegmentRequestGeodeticToEntityBasic&)>()},
                                                                                        {
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::LOSID), "LOSID"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::UpdatePeriod), "UpdatePeriod"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::AlphaThreshold), "AlphaThreshold"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::MaterialMask), "MaterialMask"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::DestinationEntityID), "DestinationEntityID"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::SourceLatitude), "SourceLatitude"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::SourceLongitude), "SourceLongitude"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::SourceAltitude), "SourceAltitude"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::DestinationXOffset), "DestinationXOffset"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::DestinationYOffset), "DestinationYOffset"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityBasic::DestinationZOffset), "DestinationZOffset"},
                                                                                        });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestGeodeticToEntityExtended(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestGeodeticToEntityExtended), "sendLineOfSightSegmentRequestGeodeticToEntityExtended");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestGeodeticToEntityExtended>(
    *m, "LineOfSightSegmentRequestGeodeticToEntityExtended", {constructor<SScriptLineOfSightSegmentRequestGeodeticToEntityExtended()>(), constructor<SScriptLineOfSightSegmentRequestGeodeticToEntityExtended(const SScriptLineOfSightSegmentRequestGeodeticToEntityExtended&)>()},
    {
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::LOSID), "LOSID"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::UpdatePeriod), "UpdatePeriod"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::AlphaThreshold), "AlphaThreshold"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::MaterialMask), "MaterialMask"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::DestinationEntityID), "DestinationEntityID"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::SourceLatitude), "SourceLatitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::SourceLongitude), "SourceLongitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::SourceAltitude), "SourceAltitude"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::DestinationXOffset), "DestinationXOffset"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::DestinationYOffset), "DestinationYOffset"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::DestinationZOffset), "DestinationZOffset"},
      {fun(&SScriptLineOfSightSegmentRequestGeodeticToEntityExtended::ResponseCoordinateSystem), "ResponseCoordinateSystem"},
    });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestEntityToGeodeticBasic(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestEntityToGeodeticBasic), "sendLineOfSightSegmentRequestEntityToGeodeticBasic");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestEntityToGeodeticBasic>(*m, "LineOfSightSegmentRequestEntityToGeodeticBasic",
                                                                                        {constructor<SScriptLineOfSightSegmentRequestEntityToGeodeticBasic()>(), constructor<SScriptLineOfSightSegmentRequestEntityToGeodeticBasic(const SScriptLineOfSightSegmentRequestEntityToGeodeticBasic&)>()},
                                                                                        {
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::LOSID), "LOSID"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::UpdatePeriod), "UpdatePeriod"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::AlphaThreshold), "AlphaThreshold"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::MaterialMask), "MaterialMask"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::SourceEntityID), "SourceEntityID"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::SourceXOffset), "SourceXOffset"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::SourceYOffset), "SourceYOffset"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::SourceZOffset), "SourceZOffset"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::DestinationLatitude), "DestinationLatitude"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::DestinationLongitude), "DestinationLongitude"},
                                                                                          {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticBasic::DestinationAltitude), "DestinationAltitude"},
                                                                                        });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestEntityToGeodeticExtended(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestEntityToGeodeticExtended), "sendLineOfSightSegmentRequestEntityToGeodeticExtended");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestEntityToGeodeticExtended>(
    *m, "LineOfSightSegmentRequestEntityToGeodeticExtended", {constructor<SScriptLineOfSightSegmentRequestEntityToGeodeticExtended()>(), constructor<SScriptLineOfSightSegmentRequestEntityToGeodeticExtended(const SScriptLineOfSightSegmentRequestEntityToGeodeticExtended&)>()},
    {
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::LOSID), "LOSID"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::UpdatePeriod), "UpdatePeriod"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::AlphaThreshold), "AlphaThreshold"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::MaterialMask), "MaterialMask"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::SourceEntityID), "SourceEntityID"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::SourceXOffset), "SourceXOffset"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::SourceYOffset), "SourceYOffset"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::SourceZOffset), "SourceZOffset"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::DestinationLatitude), "DestinationLatitude"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::DestinationLongitude), "DestinationLongitude"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::DestinationAltitude), "DestinationAltitude"},
      {fun(&SScriptLineOfSightSegmentRequestEntityToGeodeticExtended::ResponseCoordinateSystem), "ResponseCoordinateSystem"},
    });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestEntityToEntityBasic(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestEntityToEntityBasic), "sendLineOfSightSegmentRequestEntityToEntityBasic");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestEntityToEntityBasic>(*m, "LineOfSightSegmentRequestEntityToEntityBasic",
                                                                                      {constructor<SScriptLineOfSightSegmentRequestEntityToEntityBasic()>(), constructor<SScriptLineOfSightSegmentRequestEntityToEntityBasic(const SScriptLineOfSightSegmentRequestEntityToEntityBasic&)>()},
                                                                                      {
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::LOSID), "LOSID"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::UpdatePeriod), "UpdatePeriod"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::AlphaThreshold), "AlphaThreshold"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::MaterialMask), "MaterialMask"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::SourceEntityID), "SourceEntityID"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::DestinationEntityID), "DestinationEntityID"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::SourceXOffset), "SourceXOffset"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::SourceYOffset), "SourceYOffset"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::SourceZOffset), "SourceZOffset"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::DestinationXOffset), "DestinationXOffset"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::DestinationYOffset), "DestinationYOffset"},
                                                                                        {fun(&SScriptLineOfSightSegmentRequestEntityToEntityBasic::DestinationZOffset), "DestinationZOffset"},
                                                                                      });

  chai.add(m);
}

void RegisterLineOfSightSegmentRequestEntityToEntityExtended(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightSegmentRequestEntityToEntityExtended), "sendLineOfSightSegmentRequestEntityToEntityExtended");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightSegmentRequestEntityToEntityExtended>(*m, "LineOfSightSegmentRequestEntityToEntityExtended",
                                                                                         {constructor<SScriptLineOfSightSegmentRequestEntityToEntityExtended()>(), constructor<SScriptLineOfSightSegmentRequestEntityToEntityExtended(const SScriptLineOfSightSegmentRequestEntityToEntityExtended&)>()},
                                                                                         {
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::LOSID), "LOSID"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::UpdatePeriod), "UpdatePeriod"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::AlphaThreshold), "AlphaThreshold"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::MaterialMask), "MaterialMask"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::SourceEntityID), "SourceEntityID"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::DestinationEntityID), "DestinationEntityID"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::SourceXOffset), "SourceXOffset"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::SourceYOffset), "SourceYOffset"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::SourceZOffset), "SourceZOffset"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::DestinationXOffset), "DestinationXOffset"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::DestinationYOffset), "DestinationYOffset"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::DestinationZOffset), "DestinationZOffset"},
                                                                                           {fun(&SScriptLineOfSightSegmentRequestEntityToEntityExtended::ResponseCoordinateSystem), "ResponseCoordinateSystem"},
                                                                                         });

  chai.add(m);
}

struct SScriptLineOfSightVectorRequestGeodeticBasic
{
  uint16_t LOSID = 0;
  uint8_t AlphaThreshold = 0;
  uint8_t UpdatePeriod = 0;
  float Azimuth = 0;
  float Elevation = 0;
  float MinimumRange = 0;
  float MaximumRange = 0;
  int MaterialMask = 0;
  double SourceLatitude = 0;
  double SourceLongitude = 0;
  double SourceAltitude = 0;
};

struct SScriptLineOfSightVectorRequestGeodeticExtended
{
  uint16_t LOSID = 0;
  uint8_t AlphaThreshold = 0;
  uint8_t UpdatePeriod = 0;
  float Azimuth = 0;
  float Elevation = 0;
  float MinimumRange = 0;
  float MaximumRange = 0;
  int MaterialMask = 0;
  double SourceLatitude = 0;
  double SourceLongitude = 0;
  double SourceAltitude = 0;
  string ResponseCoordinateSystem;
};

struct SScriptLineOfSightVectorRequestEntityBasic
{
  uint16_t LOSID = 0;
  uint8_t AlphaThreshold = 0;
  uint8_t UpdatePeriod = 0;
  float Azimuth = 0;
  float Elevation = 0;
  float MinimumRange = 0;
  float MaximumRange = 0;
  int MaterialMask = 0;
  int EntityID = 0;
  double SourceXOffset = 0;
  double SourceYOffset = 0;
  double SourceZOffset = 0;
};

struct SScriptLineOfSightVectorRequestEntityExtended
{
  uint16_t LOSID = 0;
  uint8_t AlphaThreshold = 0;
  uint8_t UpdatePeriod = 0;
  float Azimuth = 0;
  float Elevation = 0;
  float MinimumRange = 0;
  float MaximumRange = 0;
  int MaterialMask = 0;
  int EntityID = 0;
  double SourceXOffset = 0;
  double SourceYOffset = 0;
  double SourceZOffset = 0;
  string ResponseCoordinateSystem;
};

void sendLineOfSightVectorRequestGeodeticBasic(SScriptLineOfSightVectorRequestGeodeticBasic scriptLineOfSightVectorRequest)
{
  SLineOfSightVectorRequestGeodeticBasic lineOfSightVectorRequest;
  lineOfSightVectorRequest.requestID = LineOfSightRequestID(scriptLineOfSightVectorRequest.LOSID);
  lineOfSightVectorRequest.nAlphaThreshold = scriptLineOfSightVectorRequest.AlphaThreshold;
  lineOfSightVectorRequest.updatePeriod = UpdatePeriod(scriptLineOfSightVectorRequest.UpdatePeriod);
  lineOfSightVectorRequest.azimuth = Degrees180(scriptLineOfSightVectorRequest.Azimuth);
  lineOfSightVectorRequest.elevation = Degrees90(scriptLineOfSightVectorRequest.Elevation);
  lineOfSightVectorRequest.fMinimumRange = scriptLineOfSightVectorRequest.MinimumRange;
  lineOfSightVectorRequest.fMaximumRange = scriptLineOfSightVectorRequest.MaximumRange;
  lineOfSightVectorRequest.sourceGeodeticCoordinates.latitude = Latitude(scriptLineOfSightVectorRequest.SourceLatitude);
  lineOfSightVectorRequest.sourceGeodeticCoordinates.longitude = Longitude(scriptLineOfSightVectorRequest.SourceLongitude);
  lineOfSightVectorRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightVectorRequest.SourceAltitude);
  lineOfSightVectorRequest.nMaterialMask = scriptLineOfSightVectorRequest.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightVectorRequestGeodeticBasic(lineOfSightVectorRequest);
}

void sendLineOfSightVectorRequestGeodeticExtended(SScriptLineOfSightVectorRequestGeodeticExtended scriptLineOfSightVectorRequest)
{
  SLineOfSightVectorRequestGeodeticExtended lineOfSightVectorRequest;
  lineOfSightVectorRequest.requestID = LineOfSightRequestID(scriptLineOfSightVectorRequest.LOSID);
  lineOfSightVectorRequest.nAlphaThreshold = scriptLineOfSightVectorRequest.AlphaThreshold;
  lineOfSightVectorRequest.updatePeriod = UpdatePeriod(scriptLineOfSightVectorRequest.UpdatePeriod);
  lineOfSightVectorRequest.azimuth = Degrees180(scriptLineOfSightVectorRequest.Azimuth);
  lineOfSightVectorRequest.elevation = Degrees90(scriptLineOfSightVectorRequest.Elevation);
  lineOfSightVectorRequest.fMinimumRange = scriptLineOfSightVectorRequest.MinimumRange;
  lineOfSightVectorRequest.fMaximumRange = scriptLineOfSightVectorRequest.MaximumRange;
  lineOfSightVectorRequest.sourceGeodeticCoordinates.latitude = Latitude(scriptLineOfSightVectorRequest.SourceLatitude);
  lineOfSightVectorRequest.sourceGeodeticCoordinates.longitude = Longitude(scriptLineOfSightVectorRequest.SourceLongitude);
  lineOfSightVectorRequest.sourceGeodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptLineOfSightVectorRequest.SourceAltitude);
  lineOfSightVectorRequest.nMaterialMask = scriptLineOfSightVectorRequest.MaterialMask;
  lineOfSightVectorRequest.eResponseCoordinateSystem = ConvertCigiStringToCoordinateSystem(scriptLineOfSightVectorRequest.ResponseCoordinateSystem);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightVectorRequestGeodeticExtended(lineOfSightVectorRequest);
}

void sendLineOfSightVectorRequestEntityBasic(SScriptLineOfSightVectorRequestEntityBasic scriptLineOfSightVectorRequest)
{
  SLineOfSightVectorRequestEntityBasic lineOfSightVectorRequest;
  lineOfSightVectorRequest.requestID = LineOfSightRequestID(scriptLineOfSightVectorRequest.LOSID);
  lineOfSightVectorRequest.sourceEntityID = EntityID(static_cast<uint16_t>(scriptLineOfSightVectorRequest.EntityID));
  lineOfSightVectorRequest.nAlphaThreshold = scriptLineOfSightVectorRequest.AlphaThreshold;
  lineOfSightVectorRequest.updatePeriod = UpdatePeriod(scriptLineOfSightVectorRequest.UpdatePeriod);
  lineOfSightVectorRequest.azimuth = Degrees180(scriptLineOfSightVectorRequest.Azimuth);
  lineOfSightVectorRequest.elevation = Degrees90(scriptLineOfSightVectorRequest.Elevation);
  lineOfSightVectorRequest.fMinimumRange = scriptLineOfSightVectorRequest.MinimumRange;
  lineOfSightVectorRequest.fMaximumRange = scriptLineOfSightVectorRequest.MaximumRange;
  lineOfSightVectorRequest.sourceOffset[0] = scriptLineOfSightVectorRequest.SourceXOffset;
  lineOfSightVectorRequest.sourceOffset[1] = scriptLineOfSightVectorRequest.SourceYOffset;
  lineOfSightVectorRequest.sourceOffset[2] = scriptLineOfSightVectorRequest.SourceZOffset;
  lineOfSightVectorRequest.nMaterialMask = scriptLineOfSightVectorRequest.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightVectorRequestEntityBasic(lineOfSightVectorRequest);
}

void sendLineOfSightVectorRequestEntityExtended(SScriptLineOfSightVectorRequestEntityExtended scriptLineOfSightVectorRequest)
{
  SLineOfSightVectorRequestEntityExtended lineOfSightVectorRequest;
  lineOfSightVectorRequest.requestID = LineOfSightRequestID(scriptLineOfSightVectorRequest.LOSID);
  lineOfSightVectorRequest.sourceEntityID = EntityID(static_cast<uint16_t>(scriptLineOfSightVectorRequest.EntityID));
  lineOfSightVectorRequest.nAlphaThreshold = scriptLineOfSightVectorRequest.AlphaThreshold;
  lineOfSightVectorRequest.updatePeriod = UpdatePeriod(scriptLineOfSightVectorRequest.UpdatePeriod);
  lineOfSightVectorRequest.azimuth = Degrees180(scriptLineOfSightVectorRequest.Azimuth);
  lineOfSightVectorRequest.elevation = Degrees90(scriptLineOfSightVectorRequest.Elevation);
  lineOfSightVectorRequest.fMinimumRange = scriptLineOfSightVectorRequest.MinimumRange;
  lineOfSightVectorRequest.fMaximumRange = scriptLineOfSightVectorRequest.MaximumRange;
  lineOfSightVectorRequest.sourceOffset[0] = scriptLineOfSightVectorRequest.SourceXOffset;
  lineOfSightVectorRequest.sourceOffset[1] = scriptLineOfSightVectorRequest.SourceYOffset;
  lineOfSightVectorRequest.sourceOffset[2] = scriptLineOfSightVectorRequest.SourceZOffset;
  lineOfSightVectorRequest.nMaterialMask = scriptLineOfSightVectorRequest.MaterialMask;
  lineOfSightVectorRequest.eResponseCoordinateSystem = ConvertCigiStringToCoordinateSystem(scriptLineOfSightVectorRequest.ResponseCoordinateSystem);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendLineOfSightVectorRequestEntityExtended(lineOfSightVectorRequest);
}

void RegisterLineOfSightVectorRequestGeodeticBasic(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightVectorRequestGeodeticBasic), "sendLineOfSightVectorRequestGeodeticBasic");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightVectorRequestGeodeticBasic>(*m, "LineOfSightVectorRequestGeodeticBasic",
                                                                               {constructor<SScriptLineOfSightVectorRequestGeodeticBasic()>(), constructor<SScriptLineOfSightVectorRequestGeodeticBasic(const SScriptLineOfSightVectorRequestGeodeticBasic&)>()},
                                                                               {
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::LOSID), "LOSID"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::AlphaThreshold), "AlphaThreshold"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::UpdatePeriod), "UpdatePeriod"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::Azimuth), "Azimuth"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::Elevation), "Elevation"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::MinimumRange), "MinimumRange"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::MaximumRange), "MaximumRange"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::SourceLatitude), "SourceLatitude"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::SourceLongitude), "SourceLongitude"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::SourceAltitude), "SourceAltitude"},
                                                                                 {fun(&SScriptLineOfSightVectorRequestGeodeticBasic::MaterialMask), "MaterialMask"},
                                                                               });

  chai.add(m);
}

void RegisterLineOfSightVectorRequestGeodeticExtended(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightVectorRequestGeodeticExtended), "sendLineOfSightVectorRequestGeodeticExtended");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightVectorRequestGeodeticExtended>(*m, "LineOfSightVectorRequestGeodeticExtended",
                                                                                  {constructor<SScriptLineOfSightVectorRequestGeodeticExtended()>(), constructor<SScriptLineOfSightVectorRequestGeodeticExtended(const SScriptLineOfSightVectorRequestGeodeticExtended&)>()},
                                                                                  {
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::LOSID), "LOSID"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::AlphaThreshold), "AlphaThreshold"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::UpdatePeriod), "UpdatePeriod"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::Azimuth), "Azimuth"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::Elevation), "Elevation"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::MinimumRange), "MinimumRange"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::MaximumRange), "MaximumRange"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::SourceLatitude), "SourceLatitude"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::SourceLongitude), "SourceLongitude"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::SourceAltitude), "SourceAltitude"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::MaterialMask), "MaterialMask"},
                                                                                    {fun(&SScriptLineOfSightVectorRequestGeodeticExtended::ResponseCoordinateSystem), "ResponseCoordinateSystem"},
                                                                                  });

  chai.add(m);
}

void RegisterLineOfSightVectorRequestEntityBasic(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightVectorRequestEntityBasic), "sendLineOfSightVectorRequestEntityBasic");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightVectorRequestEntityBasic>(*m, "LineOfSightVectorRequestEntityBasic", {constructor<SScriptLineOfSightVectorRequestEntityBasic()>(), constructor<SScriptLineOfSightVectorRequestEntityBasic(const SScriptLineOfSightVectorRequestEntityBasic&)>()},
                                                                             {
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::LOSID), "LOSID"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::EntityID), "EntityID"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::AlphaThreshold), "AlphaThreshold"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::UpdatePeriod), "UpdatePeriod"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::Azimuth), "Azimuth"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::Elevation), "Elevation"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::MinimumRange), "MinimumRange"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::MaximumRange), "MaximumRange"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::SourceXOffset), "SourceXOffset"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::SourceYOffset), "SourceYOffset"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::SourceZOffset), "SourceZOffset"},
                                                                               {fun(&SScriptLineOfSightVectorRequestEntityBasic::MaterialMask), "MaterialMask"},
                                                                             });

  chai.add(m);
}

void RegisterLineOfSightVectorRequestEntityExtended(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendLineOfSightVectorRequestEntityExtended), "sendLineOfSightVectorRequestEntityExtended");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptLineOfSightVectorRequestEntityExtended>(*m, "LineOfSightVectorRequestEntityExtended",
                                                                                {constructor<SScriptLineOfSightVectorRequestEntityExtended()>(), constructor<SScriptLineOfSightVectorRequestEntityExtended(const SScriptLineOfSightVectorRequestEntityExtended&)>()},
                                                                                {
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::LOSID), "LOSID"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::EntityID), "EntityID"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::AlphaThreshold), "AlphaThreshold"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::UpdatePeriod), "UpdatePeriod"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::Azimuth), "Azimuth"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::Elevation), "Elevation"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::MinimumRange), "MinimumRange"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::MaximumRange), "MaximumRange"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::SourceXOffset), "SourceXOffset"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::SourceYOffset), "SourceYOffset"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::SourceZOffset), "SourceZOffset"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::MaterialMask), "MaterialMask"},
                                                                                  {fun(&SScriptLineOfSightVectorRequestEntityExtended::ResponseCoordinateSystem), "ResponseCoordinateSystem"},
                                                                                });

  chai.add(m);
}

struct SScriptEnvironmentalConditionsRequest
{
  uint8_t RequestID = 0;
  bool MaritimeSurfaceConditionsRequest = false;
  bool TerrestrialSurfaceConditionsRequest = false;
  bool WeatherConditionsRequest = false;
  bool AerosolConcentrationsRequest = false;
  double Latitude = 0;
  double Longitude = 0;
  double Altitude = 0;
};

void sendEnvironmentalConditionsRequest(SScriptEnvironmentalConditionsRequest scriptEnvironmentalConditionsRequest)
{
  SEnvironmentalConditionsRequest environmentalConditionsRequest;
  environmentalConditionsRequest.nRequestID = scriptEnvironmentalConditionsRequest.RequestID;
  environmentalConditionsRequest.geodeticCoordinates.latitude = Latitude(scriptEnvironmentalConditionsRequest.Latitude);
  environmentalConditionsRequest.geodeticCoordinates.longitude = Longitude(scriptEnvironmentalConditionsRequest.Longitude);
  environmentalConditionsRequest.geodeticCoordinates.altitude = HeightRelativeToWGS84Ellipsoid(scriptEnvironmentalConditionsRequest.Altitude);

  environmentalConditionsRequest.bMaritimeSurfaceConditionsRequest = scriptEnvironmentalConditionsRequest.MaritimeSurfaceConditionsRequest;
  environmentalConditionsRequest.bTerrestrialSurfaceConditionsRequest = scriptEnvironmentalConditionsRequest.TerrestrialSurfaceConditionsRequest;
  environmentalConditionsRequest.bWeatherConditionsRequest = scriptEnvironmentalConditionsRequest.WeatherConditionsRequest;
  environmentalConditionsRequest.bAerosolConcentrationsRequest = scriptEnvironmentalConditionsRequest.AerosolConcentrationsRequest;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendEnvironmentalConditionsRequest(environmentalConditionsRequest);
}

void RegisterEnvironmentalConditionsRequest(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendEnvironmentalConditionsRequest), "sendEnvironmentalConditionsRequest");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptEnvironmentalConditionsRequest>(*m, "EnvironmentalConditionsRequest", {constructor<SScriptEnvironmentalConditionsRequest()>(), constructor<SScriptEnvironmentalConditionsRequest(const SScriptEnvironmentalConditionsRequest&)>()},
                                                                        {
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::RequestID), "RequestID"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::Latitude), "Latitude"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::Longitude), "Longitude"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::Altitude), "Altitude"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::MaritimeSurfaceConditionsRequest), "MaritimeSurfaceConditionsRequest"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::TerrestrialSurfaceConditionsRequest), "TerrestrialSurfaceConditionsRequest"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::WeatherConditionsRequest), "WeatherConditionsRequest"},
                                                                          {fun(&SScriptEnvironmentalConditionsRequest::AerosolConcentrationsRequest), "AerosolConcentrationsRequest"},
                                                                        });

  chai.add(m);
}

struct SScriptWeatherConditionsResponse
{
  int RequestID = 0;
  float Humidity = 0;
  float AirTemperature = 0;
  float VisibilityRange = 0;
  float HorizontalWindSpeed = 0;
  float VerticalWindSpeed = 0;
  float WindDirection = 0;
  float BarometricPressure = 0;
};

void CScriptRuntime::OnHostCigiWeatherConditionsResponseEvent(const HostCigiWeatherConditionsResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnWeatherConditionsResponse"))
  {
    auto pFuncOnWeatherConditionsResponse = m_pChaiScript->eval<std::function<void(SScriptWeatherConditionsResponse&)>>("OnWeatherConditionsResponse");
    try
    {
      SScriptWeatherConditionsResponse weatherConditionsResponse;
      weatherConditionsResponse.RequestID = args.weatherConditionsResponse.requestID;
      weatherConditionsResponse.Humidity = ConvertToCigiFloatPercentage(args.weatherConditionsResponse.humidity).Value();
      weatherConditionsResponse.AirTemperature = args.weatherConditionsResponse.fAirTemperature;
      weatherConditionsResponse.VisibilityRange = args.weatherConditionsResponse.fVisibilityRange;
      weatherConditionsResponse.HorizontalWindSpeed = args.weatherConditionsResponse.windSpeedHorVer.horizontalWindSpeed;
      weatherConditionsResponse.VerticalWindSpeed = args.weatherConditionsResponse.windSpeedHorVer.verticalWindSpeed;
      weatherConditionsResponse.WindDirection = args.weatherConditionsResponse.fWindDirection;
      weatherConditionsResponse.BarometricPressure = args.weatherConditionsResponse.fBarometricPressure;

      pFuncOnWeatherConditionsResponse(weatherConditionsResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnWeatherConditionsResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void RegisterWeatherConditionsResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptWeatherConditionsResponse>(*m, "WeatherConditionsResponse", {constructor<SScriptWeatherConditionsResponse()>(), constructor<SScriptWeatherConditionsResponse(const SScriptWeatherConditionsResponse&)>()},
                                                                   {
                                                                     {fun(&SScriptWeatherConditionsResponse::RequestID), "RequestID"},
                                                                     {fun(&SScriptWeatherConditionsResponse::Humidity), "Humidity"},
                                                                     {fun(&SScriptWeatherConditionsResponse::AirTemperature), "AirTemperature"},
                                                                     {fun(&SScriptWeatherConditionsResponse::VisibilityRange), "VisibilityRange"},
                                                                     {fun(&SScriptWeatherConditionsResponse::HorizontalWindSpeed), "HorizontalWindSpeed"},
                                                                     {fun(&SScriptWeatherConditionsResponse::VerticalWindSpeed), "VerticalWindSpeed"},
                                                                     {fun(&SScriptWeatherConditionsResponse::WindDirection), "WindDirection"},
                                                                     {fun(&SScriptWeatherConditionsResponse::BarometricPressure), "BarometricPressure"},
                                                                   });
  chai.add(m);
}

struct SScriptAerosolConcentrationResponse
{
  int RequestID = 0;
  int LayerID = 0;
  float AerosolConcentration = 0;
};

void CScriptRuntime::OnHostCigiAerosolConcentrationResponseEvent(const HostCigiAerosolConcentrationResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnAerosolConcentrationResponse"))
  {
    auto pFuncOnAerosolConcentrationResponse = m_pChaiScript->eval<std::function<void(SScriptAerosolConcentrationResponse&)>>("OnAerosolConcentrationResponse");
    try
    {
      SScriptAerosolConcentrationResponse aerosolConcentrationResponse;
      aerosolConcentrationResponse.RequestID = args.aerosolConcentrationResponse.requestID;
      aerosolConcentrationResponse.LayerID = args.aerosolConcentrationResponse.layerID;
      aerosolConcentrationResponse.AerosolConcentration = args.aerosolConcentrationResponse.fAerosolConcentration;

      pFuncOnAerosolConcentrationResponse(aerosolConcentrationResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnAerosolConcentrationResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void RegisterAerosolConcentrationResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptAerosolConcentrationResponse>(*m, "AerosolConcentrationResponse", {constructor<SScriptAerosolConcentrationResponse()>(), constructor<SScriptAerosolConcentrationResponse(const SScriptAerosolConcentrationResponse&)>()},
                                                                      {
                                                                        {fun(&SScriptAerosolConcentrationResponse::RequestID), "RequestID"},
                                                                        {fun(&SScriptAerosolConcentrationResponse::LayerID), "LayerID"},
                                                                        {fun(&SScriptAerosolConcentrationResponse::AerosolConcentration), "AerosolConcentration"},
                                                                      });
  chai.add(m);
}

struct SScriptMaritimeSurfaceConditionsResponse
{
  int RequestID = 0;
  float SeaSurfaceHeight = 0;
  float SurfaceWaterTemperature = 0;
  float SurfaceClarity = 0;
};

void CScriptRuntime::OnHostCigiMaritimeSurfaceConditionsResponseEvent(const HostCigiMaritimeSurfaceConditionsResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnMaritimeSurfaceConditionsResponse"))
  {
    auto pFuncOnMaritimeSurfaceConditionsResponse = m_pChaiScript->eval<std::function<void(SScriptMaritimeSurfaceConditionsResponse&)>>("OnMaritimeSurfaceConditionsResponse");
    try
    {
      SScriptMaritimeSurfaceConditionsResponse maritimeSurfaceConditionsResponse;
      maritimeSurfaceConditionsResponse.RequestID = args.maritimeSurfaceConditionsResponse.requestID;
      maritimeSurfaceConditionsResponse.SeaSurfaceHeight = static_cast<float>(args.maritimeSurfaceConditionsResponse.fSeaSurfaceHeight.Value());
      maritimeSurfaceConditionsResponse.SurfaceWaterTemperature = args.maritimeSurfaceConditionsResponse.fSurfaceWaterTemperature.Value();
      maritimeSurfaceConditionsResponse.SurfaceClarity = ConvertToCigiFloatPercentage(args.maritimeSurfaceConditionsResponse.surfaceClarity).Value();

      pFuncOnMaritimeSurfaceConditionsResponse(maritimeSurfaceConditionsResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnMaritimeSurfaceConditionsResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void RegisterMaritimeSurfaceConditionsResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptMaritimeSurfaceConditionsResponse>(*m, "MaritimeSurfaceConditionsResponse", {constructor<SScriptMaritimeSurfaceConditionsResponse()>(), constructor<SScriptMaritimeSurfaceConditionsResponse(const SScriptMaritimeSurfaceConditionsResponse&)>()},
                                                                           {
                                                                             {fun(&SScriptMaritimeSurfaceConditionsResponse::RequestID), "RequestID"},
                                                                             {fun(&SScriptMaritimeSurfaceConditionsResponse::SeaSurfaceHeight), "SeaSurfaceHeight"},
                                                                             {fun(&SScriptMaritimeSurfaceConditionsResponse::SurfaceWaterTemperature), "SurfaceWaterTemperature"},
                                                                             {fun(&SScriptMaritimeSurfaceConditionsResponse::SurfaceClarity), "SurfaceClarity"},
                                                                           });
  chai.add(m);
}

struct SScriptTerrestrialSurfaceConditionsResponse
{
  int RequestID = 0;
  int SurfaceConditionID = 0;
};

void CScriptRuntime::OnHostCigiTerrestrialSurfaceConditionsResponseEvent(const HostCigiTerrestrialSurfaceConditionsResponseEventArgs& args)
{
  if (m_pChaiScript == nullptr || !m_bIsExecutingScript)
  {
    return;
  }

  auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
  if (pFunctionExists("OnTerrestrialSurfaceConditionsResponse"))
  {
    auto pFuncOnTerrestrialSurfaceConditionsResponse = m_pChaiScript->eval<std::function<void(SScriptTerrestrialSurfaceConditionsResponse&)>>("OnTerrestrialSurfaceConditionsResponse");
    try
    {
      SScriptTerrestrialSurfaceConditionsResponse terrestrialSurfaceConditionsResponse;
      terrestrialSurfaceConditionsResponse.RequestID = args.terrestrialSurfaceConditionsResponse.requestID;
      terrestrialSurfaceConditionsResponse.SurfaceConditionID = static_cast<int>(args.terrestrialSurfaceConditionsResponse.surfaceConditionID);

      pFuncOnTerrestrialSurfaceConditionsResponse(terrestrialSurfaceConditionsResponse);
    }
    catch (const chaiscript::exception::eval_error& e)
    {
      HostCigiErrorEventArgs errorArgs;
      stringstream ss;
      ss << "Script OnTerrestrialSurfaceConditionsResponse Error\n" << e.pretty_print() << '\n';
      errorArgs.sError = ss.str();
      Event::Raise<HostCigiEvent>(errorArgs);

      m_bIsExecutingScript = false;
    }
  }
}

void RegisterTerrestrialSurfaceConditionsResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptTerrestrialSurfaceConditionsResponse>(*m, "TerrestrialSurfaceConditionsResponse", {constructor<SScriptTerrestrialSurfaceConditionsResponse()>(), constructor<SScriptTerrestrialSurfaceConditionsResponse(const SScriptTerrestrialSurfaceConditionsResponse&)>()},
                                                                              {
                                                                                {fun(&SScriptTerrestrialSurfaceConditionsResponse::RequestID), "RequestID"},
                                                                                {fun(&SScriptTerrestrialSurfaceConditionsResponse::SurfaceConditionID), "SurfaceConditionID"},
                                                                              });
  chai.add(m);
}

struct SScriptSymbolClone
{
  uint16_t SymbolID = 0;
  uint16_t SourceID = 0;
  string SourceType;
};

void sendSymbolClone(SScriptSymbolClone scriptSymbolClone)
{
  SSymbolClone symbolClone;
  symbolClone.eSymbolSourceType = ConvertCigiStringToSourceType(scriptSymbolClone.SourceType);
  symbolClone.sourceID = SymbolID(scriptSymbolClone.SourceID);
  symbolClone.symbolID = SymbolID(scriptSymbolClone.SymbolID);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendSymbolClone(symbolClone);
}

void RegisterSymbolClone(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolClone), "sendSymbolClone");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptSymbolClone>(*m, "SymbolClone", {constructor<SScriptSymbolClone()>(), constructor<SScriptSymbolClone(const SScriptSymbolClone&)>()},
                                                     {
                                                       {fun(&SScriptSymbolClone::SourceID), "SourceID"},
                                                       {fun(&SScriptSymbolClone::SourceType), "SourceType"},
                                                       {fun(&SScriptSymbolClone::SymbolID), "SymbolID"},
                                                     });

  chai.add(m);
}

struct SScriptSymbolControl
{
  uint16_t SymbolID = 0;
  uint16_t ParentSymbolID = 0;
  string SymbolState;
  string AttachState;
  string FlashControl;
  bool InheritColor = false;
  uint8_t Layer = 0;
  uint8_t FlashDutyCyclePercentage = 0;
  uint16_t SurfaceID = 0;
  float FlashPeriod = 0;
  float PositionU = 0;
  float PositionV = 0;
  float Rotation = 0;
  uint8_t Red = 0;
  uint8_t Green = 0;
  uint8_t Blue = 0;
  uint8_t Alpha = 0;
  float ScaleU = {1};
  float ScaleV = {1};
};

void sendSymbolControl(SScriptSymbolControl scriptSymbolControl)
{
  SSymbolControl symbolControl;

  symbolControl.eAttachState = ConvertCigiStringToAttachState(scriptSymbolControl.AttachState);
  symbolControl.eFlashControl = ConvertCigiStringToFlashControl(scriptSymbolControl.FlashControl);
  symbolControl.bInheritColor = scriptSymbolControl.InheritColor;
  symbolControl.eSymbolState = ConvertCigiStringToSymbolState(scriptSymbolControl.SymbolState);
  symbolControl.fFlashPeriod = scriptSymbolControl.FlashPeriod;
  symbolControl.fPositionU = scriptSymbolControl.PositionU;
  symbolControl.fPositionV = scriptSymbolControl.PositionV;
  symbolControl.fRotation = Degrees(scriptSymbolControl.Rotation);
  symbolControl.fScaleU = scriptSymbolControl.ScaleU;
  symbolControl.fScaleV = scriptSymbolControl.ScaleV;
  symbolControl.color.r = scriptSymbolControl.Red;
  symbolControl.color.g = scriptSymbolControl.Green;
  symbolControl.color.b = scriptSymbolControl.Blue;
  symbolControl.color.a = scriptSymbolControl.Alpha;
  symbolControl.parentSymbolID = SymbolID(scriptSymbolControl.ParentSymbolID);
  CigiIntPercentage flashDutyCyclePercentage = CigiIntPercentage(scriptSymbolControl.FlashDutyCyclePercentage);
  symbolControl.flashDutyCyclePercentage = ConvertToPercentage(flashDutyCyclePercentage);
  symbolControl.nLayerID = scriptSymbolControl.Layer;
  symbolControl.surfaceID = SymbolSurfaceID(scriptSymbolControl.SurfaceID);
  symbolControl.symbolID = SymbolID(scriptSymbolControl.SymbolID);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendSymbolControl(symbolControl);
}

void RegisterSymbolControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolControl), "sendSymbolControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptSymbolControl>(*m, "SymbolControl", {constructor<SScriptSymbolControl()>(), constructor<SScriptSymbolControl(const SScriptSymbolControl&)>()},
                                                       {
                                                         {fun(&SScriptSymbolControl::SymbolID), "SymbolID"},
                                                         {fun(&SScriptSymbolControl::ParentSymbolID), "ParentSymbolID"},
                                                         {fun(&SScriptSymbolControl::SymbolState), "SymbolState"},
                                                         {fun(&SScriptSymbolControl::AttachState), "AttachState"},
                                                         {fun(&SScriptSymbolControl::FlashControl), "FlashControl"},
                                                         {fun(&SScriptSymbolControl::InheritColor), "InheritColor"},
                                                         {fun(&SScriptSymbolControl::Layer), "Layer"},
                                                         {fun(&SScriptSymbolControl::FlashDutyCyclePercentage), "FlashDutyCyclePercentage"},
                                                         {fun(&SScriptSymbolControl::SurfaceID), "SurfaceID"},
                                                         {fun(&SScriptSymbolControl::FlashPeriod), "FlashPeriod"},
                                                         {fun(&SScriptSymbolControl::PositionU), "PositionU"},
                                                         {fun(&SScriptSymbolControl::PositionV), "PositionV"},
                                                         {fun(&SScriptSymbolControl::Rotation), "Rotation"},
                                                         {fun(&SScriptSymbolControl::Red), "Red"},
                                                         {fun(&SScriptSymbolControl::Green), "Green"},
                                                         {fun(&SScriptSymbolControl::Blue), "Blue"},
                                                         {fun(&SScriptSymbolControl::Alpha), "Alpha"},
                                                         {fun(&SScriptSymbolControl::ScaleU), "ScaleU"},
                                                         {fun(&SScriptSymbolControl::ScaleV), "ScaleV"},
                                                       });

  chai.add(m);
}

struct SScriptWaveControl
{
  uint8_t WaveID = 0;
  bool WaveEnable = false;
  string Scope;
  string BreakerType;
  uint16_t EntityRegionID = 0;
  float WaveHeight = 0;
  float Wavelength = 0;
  float Period = 0;
  float Direction = 0;
  float PhaseOffset = 0;
  float Leading = 0;
};

void sendWaveControl(SScriptWaveControl scriptWaveControl)
{
  SCigiWaveCondition waveCondition;

  waveCondition.bWaveEnabled = scriptWaveControl.WaveEnable;
  waveCondition.waveID = scriptWaveControl.WaveID;
  waveCondition.fWaveHeight = scriptWaveControl.WaveHeight;
  waveCondition.fWavelength = scriptWaveControl.Wavelength;
  waveCondition.eBreakerType = ConvertCigiStringToWaveBreakerType(scriptWaveControl.BreakerType);
  waveCondition.fPeriod = scriptWaveControl.Period;
  waveCondition.direction = Degrees360(scriptWaveControl.Direction);
  waveCondition.phaseOffset = Degrees360(scriptWaveControl.PhaseOffset);
  waveCondition.leading = Degrees180(scriptWaveControl.Leading);

  ECigiScope scope = ConvertCigiStringToScope(scriptWaveControl.Scope);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();

  if (scope == ECigiScope::ENTITY)
  {
    EntityID entityID(scriptWaveControl.EntityRegionID);
    pSession->SendEntityWaveControl(entityID, waveCondition);
  }
  else if (scope == ECigiScope::REGIONAL)
  {
    RegionID regionID(scriptWaveControl.EntityRegionID);
    pSession->SendRegionalWaveControl(regionID, waveCondition);
  }
  else if (scope == ECigiScope::GLOBAL)
  {
    pSession->SendGlobalWaveControl(waveCondition);
  }
}

void RegisterWaveControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendWaveControl), "sendWaveControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptWaveControl>(*m, "WaveControl", {constructor<SScriptWaveControl()>(), constructor<SScriptWaveControl(const SScriptWaveControl&)>()},
                                                     {
                                                       {fun(&SScriptWaveControl::WaveID), "WaveID"},
                                                       {fun(&SScriptWaveControl::WaveEnable), "WaveEnable"},
                                                       {fun(&SScriptWaveControl::Scope), "Scope"},
                                                       {fun(&SScriptWaveControl::BreakerType), "BreakerType"},
                                                       {fun(&SScriptWaveControl::EntityRegionID), "EntityRegionID"},
                                                       {fun(&SScriptWaveControl::WaveHeight), "WaveHeight"},
                                                       {fun(&SScriptWaveControl::Wavelength), "Wavelength"},
                                                       {fun(&SScriptWaveControl::Period), "Period"},
                                                       {fun(&SScriptWaveControl::Direction), "Direction"},
                                                       {fun(&SScriptWaveControl::PhaseOffset), "PhaseOffset"},
                                                       {fun(&SScriptWaveControl::Leading), "Leading"},
                                                     });

  chai.add(m);
}

struct SScriptSensorControl
{
  uint8_t SensorID = 0;
  string TrackMode;
  string SensorOnOff;
  string Polarity;
  bool LineByLineDropoutEnable = 0;
  string AutomaticGain;
  string TrackWhiteBlack;
  string ResponseType;
  uint16_t ViewID = 0;
  float Gain = 0;
  float Level = 0;
  float ACCoupling = 0;
  float Noise = 0;
};

void sendSensorControl(SScriptSensorControl scriptSensorControl)
{
  SCigiSensorControl sensorControl;

  sensorControl.sensorID = SensorID(scriptSensorControl.SensorID);
  sensorControl.eTrackMode = ConvertCigiStringToTrackMode(scriptSensorControl.TrackMode);
  sensorControl.ePolarity = ConvertCigiStringToPolarity(scriptSensorControl.Polarity);
  sensorControl.bSensorOn = scriptSensorControl.SensorOnOff == "On";
  sensorControl.bLineByLineDropoutEnabled = scriptSensorControl.LineByLineDropoutEnable;
  sensorControl.bAutomaticGain = scriptSensorControl.AutomaticGain == "Enable";
  sensorControl.eSensorTrack = ConvertCigiStringToSensorTrack(scriptSensorControl.TrackWhiteBlack);
  sensorControl.bExtendedResponse = scriptSensorControl.ResponseType == "Extended";
  sensorControl.viewID = ViewID(scriptSensorControl.ViewID);
  sensorControl.gain = SensorGain(scriptSensorControl.Gain);
  sensorControl.level = SensorLevel(scriptSensorControl.Level);
  sensorControl.fACCoupling = scriptSensorControl.ACCoupling;
  sensorControl.noise = SensorNoise(scriptSensorControl.Noise);

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendSensorControl(sensorControl);
}

void RegisterSensorControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSensorControl), "sendSensorControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptSensorControl&>(*m, "SensorControl", {constructor<SScriptSensorControl()>(), constructor<SScriptSensorControl(const SScriptSensorControl&)>()},
                                                        {
                                                          {fun(&SScriptSensorControl::SensorID), "SensorID"},
                                                          {fun(&SScriptSensorControl::TrackMode), "TrackMode"},
                                                          {fun(&SScriptSensorControl::SensorOnOff), "SensorOnOff"},
                                                          {fun(&SScriptSensorControl::Polarity), "Polarity"},
                                                          {fun(&SScriptSensorControl::LineByLineDropoutEnable), "LineByLineDropoutEnable"},
                                                          {fun(&SScriptSensorControl::AutomaticGain), "AutomaticGain"},
                                                          {fun(&SScriptSensorControl::TrackWhiteBlack), "TrackWhiteBlack"},
                                                          {fun(&SScriptSensorControl::ResponseType), "ResponseType"},
                                                          {fun(&SScriptSensorControl::ViewID), "ViewID"},
                                                          {fun(&SScriptSensorControl::Gain), "Gain"},
                                                          {fun(&SScriptSensorControl::Level), "Level"},
                                                          {fun(&SScriptSensorControl::ACCoupling), "ACCoupling"},
                                                          {fun(&SScriptSensorControl::Noise), "Noise"},
                                                        });

  chai.add(m);
}

struct SScriptMotionTrackerControl
{
  int TrackerID = 0;
  bool TrackerEnable = false;
  bool BoresightEnable = false;
  bool XEnable = false;
  bool YEnable = false;
  bool ZEnable = false;
  bool RollEnable = false;
  bool PitchEnable = false;
  bool YawEnable = false;
  string ViewViewGroupSelect;
  uint8_t ViewViewGroupID = 0;
};

void sendMotionTrackerControl(SScriptMotionTrackerControl scriptMotionTrackerControl)
{

  EViewType viewType = ConvertCigiStringToViewType(scriptMotionTrackerControl.ViewViewGroupSelect);

  if (viewType == EViewType::VIEW)
  {
    SMotionTrackerViewControl motionTrackerControl;
    motionTrackerControl.motionTrackerID = MotionTrackerID(static_cast<uint8_t>(scriptMotionTrackerControl.TrackerID));
    motionTrackerControl.bBoresightEnable = scriptMotionTrackerControl.BoresightEnable;
    motionTrackerControl.bEnable = scriptMotionTrackerControl.TrackerEnable;
    motionTrackerControl.bXEnable = scriptMotionTrackerControl.XEnable;
    motionTrackerControl.bYEnable = scriptMotionTrackerControl.YEnable;
    motionTrackerControl.bZEnable = scriptMotionTrackerControl.ZEnable;
    motionTrackerControl.bRollEnable = scriptMotionTrackerControl.RollEnable;
    motionTrackerControl.bPitchEnable = scriptMotionTrackerControl.PitchEnable;
    motionTrackerControl.bYawEnable = scriptMotionTrackerControl.YawEnable;

    motionTrackerControl.viewID = ViewID(scriptMotionTrackerControl.ViewViewGroupID);

    CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
    pSession->SendMotionTrackerViewControl(motionTrackerControl);
  }
  else if (viewType == EViewType::VIEW_GROUP)
  {
    SMotionTrackerViewGroupControl motionTrackerControl;
    motionTrackerControl.motionTrackerID = MotionTrackerID(static_cast<uint8_t>(scriptMotionTrackerControl.TrackerID));
    motionTrackerControl.bBoresightEnable = scriptMotionTrackerControl.BoresightEnable;
    motionTrackerControl.bEnable = scriptMotionTrackerControl.TrackerEnable;
    motionTrackerControl.bXEnable = scriptMotionTrackerControl.XEnable;
    motionTrackerControl.bYEnable = scriptMotionTrackerControl.YEnable;
    motionTrackerControl.bZEnable = scriptMotionTrackerControl.ZEnable;
    motionTrackerControl.bRollEnable = scriptMotionTrackerControl.RollEnable;
    motionTrackerControl.bPitchEnable = scriptMotionTrackerControl.PitchEnable;
    motionTrackerControl.bYawEnable = scriptMotionTrackerControl.YawEnable;

    motionTrackerControl.viewGroupID = ViewGroupID(scriptMotionTrackerControl.ViewViewGroupID);

    CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
    pSession->SendMotionTrackerViewGroupControl(motionTrackerControl);
  }
}

void RegisterMotionTrackerControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendMotionTrackerControl), "sendMotionTrackerControl");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptMotionTrackerControl>(*m, "MotionTrackerControl", {constructor<SScriptMotionTrackerControl()>(), constructor<SScriptMotionTrackerControl(const SScriptMotionTrackerControl&)>()},
                                                              {
                                                                {fun(&SScriptMotionTrackerControl::TrackerID), "TrackerID"},
                                                                {fun(&SScriptMotionTrackerControl::TrackerEnable), "TrackerEnable"},
                                                                {fun(&SScriptMotionTrackerControl::BoresightEnable), "BoresightEnable"},
                                                                {fun(&SScriptMotionTrackerControl::XEnable), "XEnable"},
                                                                {fun(&SScriptMotionTrackerControl::YEnable), "YEnable"},
                                                                {fun(&SScriptMotionTrackerControl::ZEnable), "ZEnable"},
                                                                {fun(&SScriptMotionTrackerControl::RollEnable), "RollEnable"},
                                                                {fun(&SScriptMotionTrackerControl::PitchEnable), "PitchEnable"},
                                                                {fun(&SScriptMotionTrackerControl::YawEnable), "YawEnable"},
                                                                {fun(&SScriptMotionTrackerControl::ViewViewGroupSelect), "ViewViewGroupSelect"},
                                                                {fun(&SScriptMotionTrackerControl::ViewViewGroupID), "ViewViewGroupID"},
                                                              });
  chai.add(m);
}

struct SScriptCollisionDetectionSegmentDefinition
{
  uint8_t SegmentID = 0;
  bool SegmentEnable = false;
  uint16_t EntityID = 0;
  float X1 = 0;
  float Y1 = 0;
  float Z1 = 0;
  float X2 = 0;
  float Y2 = 0;
  float Z2 = 0;
  int MaterialMask = 0;
};

void sendCollisionDetectionSegmentDefinition(SScriptCollisionDetectionSegmentDefinition scriptCollisionDetectionSegmentDefinition)
{
  SCollisionDetectionSegmentDefinition collisionDetectionSegmentDefinition;

  collisionDetectionSegmentDefinition.segmentID = SegmentID(scriptCollisionDetectionSegmentDefinition.SegmentID);
  collisionDetectionSegmentDefinition.bSegmentEnabled = scriptCollisionDetectionSegmentDefinition.SegmentEnable;
  collisionDetectionSegmentDefinition.entityID = EntityID(scriptCollisionDetectionSegmentDefinition.EntityID);
  collisionDetectionSegmentDefinition.beg[0] = scriptCollisionDetectionSegmentDefinition.X1;
  collisionDetectionSegmentDefinition.beg[1] = scriptCollisionDetectionSegmentDefinition.Y1;
  collisionDetectionSegmentDefinition.beg[2] = scriptCollisionDetectionSegmentDefinition.Z1;
  collisionDetectionSegmentDefinition.end[0] = scriptCollisionDetectionSegmentDefinition.X2;
  collisionDetectionSegmentDefinition.end[1] = scriptCollisionDetectionSegmentDefinition.Y2;
  collisionDetectionSegmentDefinition.end[2] = scriptCollisionDetectionSegmentDefinition.Z2;
  collisionDetectionSegmentDefinition.nMaterialMask = scriptCollisionDetectionSegmentDefinition.MaterialMask;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendCollisionDetectionSegment(collisionDetectionSegmentDefinition);
}

void RegisterCollisionDetectionSegmentDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendCollisionDetectionSegmentDefinition), "sendCollisionDetectionSegmentDefinition");

  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptCollisionDetectionSegmentDefinition>(*m, "CollisionDetectionSegmentDefinition", {constructor<SScriptCollisionDetectionSegmentDefinition()>(), constructor<SScriptCollisionDetectionSegmentDefinition(const SScriptCollisionDetectionSegmentDefinition&)>()},
                                                                             {
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::SegmentID), "SegmentID"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::SegmentEnable), "SegmentEnable"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::EntityID), "EntityID"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::X1), "X1"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::Y1), "Y1"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::Z1), "Z1"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::X2), "X2"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::Y2), "Y2"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::Z2), "Z2"},
                                                                               {fun(&SScriptCollisionDetectionSegmentDefinition::MaterialMask), "MaterialMask"},
                                                                             });

  chai.add(m);
}

struct SScriptCollisionDetectionVolumeDefinition
{
  uint8_t VolumeID = 0;
  bool VolumeEnable = false;
  string VolumeType;
  uint16_t EntityID = 0;
  float X = 0;
  float Y = 0;
  float Z = 0;
  float RadiusHeight = 0;
  float Width = 0;
  float Depth = 0;
  float Roll = 0;
  float Pitch = 0;
  float Yaw = 0;
};

void sendCollisionDetectionVolumeDefinition(SScriptCollisionDetectionVolumeDefinition scriptCollisionDetectionVolumeDefinition)
{

  EVolumeType eVolumeType = ConvertCigiStringToVolumeType(scriptCollisionDetectionVolumeDefinition.VolumeType);

  if (eVolumeType == EVolumeType::CUBOID)
  {
    SCollisionDetectionCuboidDefinition collisionDetectionCuboidDefinition;
    collisionDetectionCuboidDefinition.volumeID = VolumeID(scriptCollisionDetectionVolumeDefinition.VolumeID);
    collisionDetectionCuboidDefinition.bVolumeEnabled = scriptCollisionDetectionVolumeDefinition.VolumeEnable;
    collisionDetectionCuboidDefinition.entityID = EntityID(scriptCollisionDetectionVolumeDefinition.EntityID);
    collisionDetectionCuboidDefinition.offset[0] = scriptCollisionDetectionVolumeDefinition.X;
    collisionDetectionCuboidDefinition.offset[1] = scriptCollisionDetectionVolumeDefinition.Y;
    collisionDetectionCuboidDefinition.offset[2] = scriptCollisionDetectionVolumeDefinition.Z;

    collisionDetectionCuboidDefinition.fHeight = scriptCollisionDetectionVolumeDefinition.RadiusHeight;
    collisionDetectionCuboidDefinition.fWidth = scriptCollisionDetectionVolumeDefinition.Width;
    collisionDetectionCuboidDefinition.fDepth = scriptCollisionDetectionVolumeDefinition.Depth;
    collisionDetectionCuboidDefinition.rotation.roll = Degrees(scriptCollisionDetectionVolumeDefinition.Roll);
    collisionDetectionCuboidDefinition.rotation.pitch = Degrees(scriptCollisionDetectionVolumeDefinition.Pitch);
    collisionDetectionCuboidDefinition.rotation.yaw = Degrees(scriptCollisionDetectionVolumeDefinition.Yaw);

    CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
    pSession->SendCollisionDetectionCuboidVolume(collisionDetectionCuboidDefinition);
  }
  else if (eVolumeType == EVolumeType::SPHERE)
  {
    SCollisionDetectionSphereDefinition collisionDetectionSphereDefinition;
    collisionDetectionSphereDefinition.volumeID = VolumeID(scriptCollisionDetectionVolumeDefinition.VolumeID);
    collisionDetectionSphereDefinition.bVolumeEnabled = scriptCollisionDetectionVolumeDefinition.VolumeEnable;
    collisionDetectionSphereDefinition.entityID = EntityID(scriptCollisionDetectionVolumeDefinition.EntityID);
    collisionDetectionSphereDefinition.offset[0] = scriptCollisionDetectionVolumeDefinition.X;
    collisionDetectionSphereDefinition.offset[1] = scriptCollisionDetectionVolumeDefinition.Y;
    collisionDetectionSphereDefinition.offset[2] = scriptCollisionDetectionVolumeDefinition.Z;

    collisionDetectionSphereDefinition.fRadius = scriptCollisionDetectionVolumeDefinition.RadiusHeight;

    CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
    pSession->SendCollisionDetectionSphereVolume(collisionDetectionSphereDefinition);
  }
}

void RegisterCollisionDetectionVolumeDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendCollisionDetectionVolumeDefinition), "sendCollisionDetectionVolumeDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptCollisionDetectionVolumeDefinition>(*m, "CollisionDetectionVolumeDefinition", {constructor<SScriptCollisionDetectionVolumeDefinition()>(), constructor<SScriptCollisionDetectionVolumeDefinition(const SScriptCollisionDetectionVolumeDefinition&)>()},
                                                                            {
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::VolumeID), "VolumeID"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::VolumeEnable), "VolumeEnable"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::VolumeType), "VolumeType"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::EntityID), "EntityID"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::X), "X"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Y), "Y"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Z), "Z"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::RadiusHeight), "RadiusHeight"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Width), "Width"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Depth), "Depth"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Roll), "Roll"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Pitch), "Pitch"},
                                                                              {fun(&SScriptCollisionDetectionVolumeDefinition::Yaw), "Yaw"},
                                                                            });

  chai.add(m);
}

struct SScriptPositionRequest
{
  uint8_t ArticulatedPartID = 0;
  string UpdateMode;
  string ObjectClass;
  string CoordinateSystem;
  uint16_t ObjectID = 0;
};

void sendPositionRequest(SScriptPositionRequest scriptPositionRequest)
{
  SPositionRequest positionRequest;

  positionRequest.articulatedPartID = ArticulatedPartID(scriptPositionRequest.ArticulatedPartID);
  positionRequest.eCoordinateSystem = ConvertCigiStringToObjectCoordinateSystem(scriptPositionRequest.CoordinateSystem);
  positionRequest.eObjectClass = ConvertCigiStringToObjectClass(scriptPositionRequest.ObjectClass);
  positionRequest.bContinuous = ConvertCigiStringToUpdateMode(scriptPositionRequest.UpdateMode) == EUpdateMode::CONTINUOUS;
  positionRequest.nObjectID = scriptPositionRequest.ObjectID;

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendPositionRequest(positionRequest);
}

void RegisterPositionRequest(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendPositionRequest), "sendPositionRequest");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptPositionRequest>(*m, "PositionRequest", {constructor<SScriptPositionRequest()>(), constructor<SScriptPositionRequest(const SScriptPositionRequest&)>()},
                                                         {
                                                           {fun(&SScriptPositionRequest::ArticulatedPartID), "ArticulatedPartID"},
                                                           {fun(&SScriptPositionRequest::UpdateMode), "UpdateMode"},
                                                           {fun(&SScriptPositionRequest::ObjectClass), "ObjectClass"},
                                                           {fun(&SScriptPositionRequest::CoordinateSystem), "CoordinateSystem"},
                                                           {fun(&SScriptPositionRequest::ObjectID), "ObjectID"},
                                                         });
  chai.add(m);
}

struct SScriptSymbolSurfaceDefinition
{
  uint16_t SurfaceID = 0;
  uint16_t EntityViewID = 0;
  string SurfaceState;
  string AttachType;
  string Billboard;
  bool PerspectiveGrowthEnable = false;
  float XOffsetLeft = 0;
  float YOffsetRight = 0;
  float ZOffsetTop = 0;
  float YawBottom = 0;
  float Pitch = 0;
  float Roll = 0;
  float Width = 0;
  float Height = 0;
  float MinU = 0;
  float MaxU = 0;
  float MinV = 0;
  float MaxV = 0;
};

void sendSymbolSurfaceDefinition(SScriptSymbolSurfaceDefinition scriptSymbolSurfaceDefinition)
{
  EAttachType eAttachType = ConvertCigiStringToAttachType(scriptSymbolSurfaceDefinition.AttachType);

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();

  if (eAttachType == EAttachType::ENTITY)
  {
    EBillboard eBillboard = ConvertCigiStringToBillboard(scriptSymbolSurfaceDefinition.Billboard);

    if (eBillboard == EBillboard::NON_BILLBOARD)
    {
      SEntitySymbolSurfaceDefinition symbolSurfaceDefinition;

      symbolSurfaceDefinition.surfaceID = SymbolSurfaceID(scriptSymbolSurfaceDefinition.SurfaceID);
      symbolSurfaceDefinition.entityID = EntityID(scriptSymbolSurfaceDefinition.EntityViewID);
      symbolSurfaceDefinition.eSurfaceState = ConvertCigiStringToSurfaceState(scriptSymbolSurfaceDefinition.SurfaceState);
      symbolSurfaceDefinition.offsetToSurface[0] = scriptSymbolSurfaceDefinition.XOffsetLeft;
      symbolSurfaceDefinition.offsetToSurface[1] = scriptSymbolSurfaceDefinition.YOffsetRight;
      symbolSurfaceDefinition.offsetToSurface[2] = scriptSymbolSurfaceDefinition.ZOffsetTop;
      symbolSurfaceDefinition.yaw = Degrees(scriptSymbolSurfaceDefinition.YawBottom);
      symbolSurfaceDefinition.pitch = Degrees90(scriptSymbolSurfaceDefinition.Pitch);
      symbolSurfaceDefinition.roll = Degrees180(scriptSymbolSurfaceDefinition.Roll);
      symbolSurfaceDefinition.width = scriptSymbolSurfaceDefinition.Width;
      symbolSurfaceDefinition.height = scriptSymbolSurfaceDefinition.Height;
      symbolSurfaceDefinition.uvMin[0] = scriptSymbolSurfaceDefinition.MinU;
      symbolSurfaceDefinition.uvMin[1] = scriptSymbolSurfaceDefinition.MinV;
      symbolSurfaceDefinition.uvMax[0] = scriptSymbolSurfaceDefinition.MaxU;
      symbolSurfaceDefinition.uvMax[1] = scriptSymbolSurfaceDefinition.MaxV;

      pSession->SendEntitySymbolSurfaceDefinition(symbolSurfaceDefinition);
    }
    else if (eBillboard == EBillboard::BILLBOARD)
    {
      SEntityBillboardSymbolSurfaceDefinition symbolSurfaceDefinition;

      symbolSurfaceDefinition.surfaceID = SymbolSurfaceID(scriptSymbolSurfaceDefinition.SurfaceID);
      symbolSurfaceDefinition.entityID = EntityID(scriptSymbolSurfaceDefinition.EntityViewID);
      symbolSurfaceDefinition.eSurfaceState = ConvertCigiStringToSurfaceState(scriptSymbolSurfaceDefinition.SurfaceState);
      symbolSurfaceDefinition.bPerspectiveGrowthEnabled = scriptSymbolSurfaceDefinition.PerspectiveGrowthEnable;
      symbolSurfaceDefinition.offsetToEntity[0] = scriptSymbolSurfaceDefinition.XOffsetLeft;
      symbolSurfaceDefinition.offsetToEntity[1] = scriptSymbolSurfaceDefinition.YOffsetRight;
      symbolSurfaceDefinition.offsetToEntity[2] = scriptSymbolSurfaceDefinition.ZOffsetTop;
      symbolSurfaceDefinition.width = scriptSymbolSurfaceDefinition.Width;
      symbolSurfaceDefinition.height = scriptSymbolSurfaceDefinition.Height;
      symbolSurfaceDefinition.uvMin[0] = scriptSymbolSurfaceDefinition.MinU;
      symbolSurfaceDefinition.uvMin[1] = scriptSymbolSurfaceDefinition.MinV;
      symbolSurfaceDefinition.uvMax[0] = scriptSymbolSurfaceDefinition.MaxU;
      symbolSurfaceDefinition.uvMax[1] = scriptSymbolSurfaceDefinition.MaxV;

      pSession->SendEntityBillboardSymbolSurfaceDefinition(symbolSurfaceDefinition);
    }
  }
  else if (eAttachType == EAttachType::VIEW)
  {
    SViewSymbolSurfaceDefinition symbolSurfaceDefinition;

    symbolSurfaceDefinition.surfaceID = SymbolSurfaceID(scriptSymbolSurfaceDefinition.SurfaceID);
    symbolSurfaceDefinition.viewID = ViewID(scriptSymbolSurfaceDefinition.EntityViewID);
    symbolSurfaceDefinition.eSurfaceState = ConvertCigiStringToSurfaceState(scriptSymbolSurfaceDefinition.SurfaceState);
    symbolSurfaceDefinition.fLeft = scriptSymbolSurfaceDefinition.XOffsetLeft;
    symbolSurfaceDefinition.fRight = scriptSymbolSurfaceDefinition.YOffsetRight;
    symbolSurfaceDefinition.fTop = scriptSymbolSurfaceDefinition.ZOffsetTop;
    symbolSurfaceDefinition.fBottom = scriptSymbolSurfaceDefinition.YawBottom;
    symbolSurfaceDefinition.width = scriptSymbolSurfaceDefinition.Width;
    symbolSurfaceDefinition.height = scriptSymbolSurfaceDefinition.Height;
    symbolSurfaceDefinition.uvMin[0] = scriptSymbolSurfaceDefinition.MinU;
    symbolSurfaceDefinition.uvMin[1] = scriptSymbolSurfaceDefinition.MinV;
    symbolSurfaceDefinition.uvMax[0] = scriptSymbolSurfaceDefinition.MaxU;
    symbolSurfaceDefinition.uvMax[1] = scriptSymbolSurfaceDefinition.MaxV;

    pSession->SendViewSymbolSurfaceDefinition(symbolSurfaceDefinition);
  }
}

void RegisterSymbolSurfaceDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolSurfaceDefinition), "sendSymbolSurfaceDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolSurfaceDefinition>(*m, "SymbolSurfaceDefinition", {constructor<SScriptSymbolSurfaceDefinition()>(), constructor<SScriptSymbolSurfaceDefinition(const SScriptSymbolSurfaceDefinition&)>()},
                                                                 {
                                                                   {fun(&SScriptSymbolSurfaceDefinition::SurfaceID), "SurfaceID"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::EntityViewID), "EntityViewID"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::SurfaceState), "SurfaceState"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::AttachType), "AttachType"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::Billboard), "Billboard"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::PerspectiveGrowthEnable), "PerspectiveGrowthEnable"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::XOffsetLeft), "XOffsetLeft"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::YOffsetRight), "YOffsetRight"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::ZOffsetTop), "ZOffsetTop"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::YawBottom), "YawBottom"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::Pitch), "Pitch"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::Roll), "Roll"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::Width), "Width"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::Height), "Height"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::MinU), "MinU"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::MaxU), "MaxU"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::MinV), "MinV"},
                                                                   {fun(&SScriptSymbolSurfaceDefinition::MaxV), "MaxV"},
                                                                 });
  chai.add(m);
}

struct SScriptSymbolTextDefinition
{
  string Alignment;
  string Orientation;
  string FontCategory;
  string FontName;
  uint16_t SymbolID = 0;
  float FontSize = 0;
  string Octet;
};

void sendSymbolTextDefinition(SScriptSymbolTextDefinition scriptSymbolTextDefinition)
{
  SSymbolTextDefinition symbolTextDefinition;

  symbolTextDefinition.eTextAlignment = ConvertCigiStringToAlignment(scriptSymbolTextDefinition.Alignment);
  symbolTextDefinition.eTextOrientation = ConvertCigiStringToOrientation(scriptSymbolTextDefinition.Orientation);
  symbolTextDefinition.fFontSize = scriptSymbolTextDefinition.FontSize;
  symbolTextDefinition.fontID = g_pScriptRuntime->GetFontID(scriptSymbolTextDefinition.FontCategory, scriptSymbolTextDefinition.FontName);
  symbolTextDefinition.symbolID = SymbolID(scriptSymbolTextDefinition.SymbolID);
  symbolTextDefinition.sText = scriptSymbolTextDefinition.Octet;

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pHostSession->SendSymbolTextDefinition(symbolTextDefinition);
}

void RegisterSymbolTextDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolTextDefinition), "sendSymbolTextDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolTextDefinition>(*m, "SymbolTextDefinition", {constructor<SScriptSymbolTextDefinition()>(), constructor<SScriptSymbolTextDefinition(const SScriptSymbolTextDefinition&)>()},
                                                              {
                                                                {fun(&SScriptSymbolTextDefinition::Alignment), "Alignment"},
                                                                {fun(&SScriptSymbolTextDefinition::Orientation), "Orientation"},
                                                                {fun(&SScriptSymbolTextDefinition::FontCategory), "FontCategory"},
                                                                {fun(&SScriptSymbolTextDefinition::FontName), "FontName"},
                                                                {fun(&SScriptSymbolTextDefinition::SymbolID), "SymbolID"},
                                                                {fun(&SScriptSymbolTextDefinition::FontSize), "FontSize"},
                                                                {fun(&SScriptSymbolTextDefinition::Octet), "Octet"},
                                                              });
  chai.add(m);
}

struct SScriptSymbolCircle
{
  float CenterU = 0;
  float CenterV = 0;
  float Radius = 0;
  float InnerRadius = 0;
  float StartAngle = 0;
  float EndAngle = 0;
};

struct SScriptSymbolCircleDefinition
{
  uint16_t SymbolID = 0;
  uint16_t StipplePattern = 0;
  string DrawingStyle;
  float LineWidth = 0;
  float StipplePatternLength = 0;
  std::vector<SScriptSymbolCircle> Circles;
};

void sendSymbolCircleDefinition(SScriptSymbolCircleDefinition scriptSymbolCircleDefinition)
{
  SSymbolCircle symbolCircleDefinition;

  symbolCircleDefinition.symbolID = SymbolID(scriptSymbolCircleDefinition.SymbolID);
  symbolCircleDefinition.stipplePattern = scriptSymbolCircleDefinition.StipplePattern;
  symbolCircleDefinition.eDrawingStyle = ConvertCigiStringToDrawingStyle(scriptSymbolCircleDefinition.DrawingStyle);
  symbolCircleDefinition.fLineWidth = scriptSymbolCircleDefinition.LineWidth;
  symbolCircleDefinition.fStipplePatternLength = scriptSymbolCircleDefinition.StipplePatternLength;

  for (auto c : scriptSymbolCircleDefinition.Circles)
  {
    SCircleProperties circle;
    circle.centerUV.U = c.CenterU;
    circle.centerUV.V = c.CenterV;
    circle.fRadius = c.Radius;
    circle.fInnerRadius = c.InnerRadius;
    circle.startAngle = Degrees(c.StartAngle);
    circle.endAngle = Degrees(c.EndAngle);

    symbolCircleDefinition.circles.push_back(circle);
  }

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendSymbolCircleDefinition(symbolCircleDefinition);
}

void RegisterSymbolCircle(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());
  chaiscript::utility::add_class<SScriptSymbolCircle>(*m, "SymbolCircle", {constructor<SScriptSymbolCircle()>(), constructor<SScriptSymbolCircle(const SScriptSymbolCircle&)>()},
                                                      {{fun(&SScriptSymbolCircle::CenterU), "CenterU"},
                                                       {fun(&SScriptSymbolCircle::CenterV), "CenterV"},
                                                       {fun(&SScriptSymbolCircle::Radius), "Radius"},
                                                       {fun(&SScriptSymbolCircle::InnerRadius), "InnerRadius"},
                                                       {fun(&SScriptSymbolCircle::StartAngle), "StartAngle"},
                                                       {fun(&SScriptSymbolCircle::EndAngle), "EndAngle"}});
  chai.add(m);
}

void RegisterSymbolCircleDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolCircleDefinition), "sendSymbolCircleDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolCircleDefinition>(*m, "SymbolCircleDefinition", {constructor<SScriptSymbolCircleDefinition()>(), constructor<SScriptSymbolCircleDefinition(const SScriptSymbolCircleDefinition&)>()},
                                                                {{fun(&SScriptSymbolCircleDefinition::SymbolID), "SymbolID"},
                                                                 {fun(&SScriptSymbolCircleDefinition::StipplePattern), "StipplePattern"},
                                                                 {fun(&SScriptSymbolCircleDefinition::DrawingStyle), "DrawingStyle"},
                                                                 {fun(&SScriptSymbolCircleDefinition::LineWidth), "LineWidth"},
                                                                 {fun(&SScriptSymbolCircleDefinition::StipplePatternLength), "StipplePatternLength"},
                                                                 {fun(&SScriptSymbolCircleDefinition::Circles), "Circles"}});

  chai.add(m);
  RegisterSymbolCircle(chai);
  chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<SScriptSymbolCircle>>("CircleList"));
}

struct ScriptVertexUV
{
  float U = 0;
  float V = 0;
};

struct SScriptSymbolPolygonDefinition
{
  uint16_t SymbolID = 0;
  uint16_t StipplePattern = 0;
  string PrimitiveType;
  float LineWidth = 0;
  float StipplePatternLength = 0;
  vector<ScriptVertexUV> Vertices;
};

void sendSymbolPolygonDefinition(SScriptSymbolPolygonDefinition scriptSymbolPolygonDefinition)
{
  SSymbolPolygon symbolPolygonDefinition;

  symbolPolygonDefinition.symbolID = SymbolID(scriptSymbolPolygonDefinition.SymbolID);
  symbolPolygonDefinition.nStipplePattern = scriptSymbolPolygonDefinition.StipplePattern;
  symbolPolygonDefinition.ePrimitiveType = ConvertCigiStringToPrimitivePolygon(scriptSymbolPolygonDefinition.PrimitiveType);
  symbolPolygonDefinition.fLineWidth = scriptSymbolPolygonDefinition.LineWidth;
  symbolPolygonDefinition.fStipplePatternLength = scriptSymbolPolygonDefinition.StipplePatternLength;

  for (auto v : scriptSymbolPolygonDefinition.Vertices)
  {
    Vec2f vec2;
    vec2[0] = v.U;
    vec2[1] = v.V;
    symbolPolygonDefinition.vertices.push_back(vec2);
  }

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendSymbolPolygonDefinition(symbolPolygonDefinition);
}

void RegisterVertexUV(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<ScriptVertexUV>(*m, "VertexUV", {constructor<ScriptVertexUV()>(), constructor<ScriptVertexUV(const ScriptVertexUV&)>()},
                                                 {
                                                   {fun(&ScriptVertexUV::U), "U"},
                                                   {fun(&ScriptVertexUV::V), "V"},
                                                 });

  chaiscript::bootstrap::operators::assign<ScriptVertexUV>(*m);

  chai.add(m);
}

void RegisterSymbolPolygonDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolPolygonDefinition), "sendSymbolPolygonDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolPolygonDefinition>(*m, "SymbolPolygonDefinition", {constructor<SScriptSymbolPolygonDefinition()>(), constructor<SScriptSymbolPolygonDefinition(const SScriptSymbolPolygonDefinition&)>()},
                                                                 {
                                                                   {fun(&SScriptSymbolPolygonDefinition::SymbolID), "SymbolID"},
                                                                   {fun(&SScriptSymbolPolygonDefinition::StipplePattern), "StipplePattern"},
                                                                   {fun(&SScriptSymbolPolygonDefinition::PrimitiveType), "PrimitiveType"},
                                                                   {fun(&SScriptSymbolPolygonDefinition::LineWidth), "LineWidth"},
                                                                   {fun(&SScriptSymbolPolygonDefinition::StipplePatternLength), "StipplePatternLength"},
                                                                   {fun(&SScriptSymbolPolygonDefinition::Vertices), "Vertices"},
                                                                 });

  chai.add(m);
  chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<ScriptVertexUV>>("VertexList"));
}

struct SScriptShortSymbolControl
{
  uint16_t SymbolID = 0;
  string AttributeSelect1;
  string AttributeSelect2;
  string SymbolState;
  string AttachState;
  string FlashControl;
  bool InheritColor = false;
  float AttributeValue1_Float = 0;
  int AttributeValue1_Int = 0;
  float AttributeValue2_Float = 0;
  int AttributeValue2_Int = 0;
  uint8_t Color1Red = 0;
  uint8_t Color1Green = 0;
  uint8_t Color1Blue = 0;
  uint8_t Color1Alpha = 0;
  uint8_t Color2Red = 0;
  uint8_t Color2Green = 0;
  uint8_t Color2Blue = 0;
  uint8_t Color2Alpha = 0;
};

void sendShortSymbolControl(SScriptShortSymbolControl scriptShortSymbolControl)
{
  SShortSymbolControl shortSymbolControl;

  shortSymbolControl.symbolID = SymbolID(scriptShortSymbolControl.SymbolID);
  shortSymbolControl.eAttributeSelect1 = ConvertCigiStringToAttributeSelect(scriptShortSymbolControl.AttributeSelect1);
  shortSymbolControl.eAttributeSelect2 = ConvertCigiStringToAttributeSelect(scriptShortSymbolControl.AttributeSelect2);
  shortSymbolControl.eSymbolState = ConvertCigiStringToESymbolState(scriptShortSymbolControl.SymbolState);
  shortSymbolControl.eAttachState = ConvertCigiStringToAttachState(scriptShortSymbolControl.AttachState);
  shortSymbolControl.eFlashControl = ConvertCigiStringToFlashControl(scriptShortSymbolControl.FlashControl);
  shortSymbolControl.bInheritColor = scriptShortSymbolControl.InheritColor;

  // Value for AttributeSelect1
  if (scriptShortSymbolControl.AttributeSelect1 == "Color")
  {
    shortSymbolControl.attributeValue1.rgba.r = scriptShortSymbolControl.Color1Red;
    shortSymbolControl.attributeValue1.rgba.g = scriptShortSymbolControl.Color1Green;
    shortSymbolControl.attributeValue1.rgba.b = scriptShortSymbolControl.Color1Blue;
    shortSymbolControl.attributeValue1.rgba.a = scriptShortSymbolControl.Color1Alpha;
  }
  else if (scriptShortSymbolControl.AttributeSelect1 == "Flash Period" || scriptShortSymbolControl.AttributeSelect1 == "Position U" || scriptShortSymbolControl.AttributeSelect1 == "Position V" || scriptShortSymbolControl.AttributeSelect1 == "Rotation" ||
           scriptShortSymbolControl.AttributeSelect1 == "Scale U" || scriptShortSymbolControl.AttributeSelect1 == "Scale V")
  {
    shortSymbolControl.attributeValue1.f = scriptShortSymbolControl.AttributeValue1_Float;
  }
  else if (scriptShortSymbolControl.AttributeSelect1 == "Surface ID" || scriptShortSymbolControl.AttributeSelect1 == "Parent Symbol ID" || scriptShortSymbolControl.AttributeSelect1 == "Layer" || scriptShortSymbolControl.AttributeSelect1 == "Flash Duty Cycle Percentage")
  {
    shortSymbolControl.attributeValue1.n = scriptShortSymbolControl.AttributeValue1_Int;
  }
  else
  {
    shortSymbolControl.attributeValue1.n = scriptShortSymbolControl.AttributeValue1_Int;
  }

  // Value for AttributeSelect2
  if (scriptShortSymbolControl.AttributeSelect2 == "Color")
  {
    shortSymbolControl.attributeValue2.rgba.r = scriptShortSymbolControl.Color2Red;
    shortSymbolControl.attributeValue2.rgba.g = scriptShortSymbolControl.Color2Green;
    shortSymbolControl.attributeValue2.rgba.b = scriptShortSymbolControl.Color2Blue;
    shortSymbolControl.attributeValue2.rgba.a = scriptShortSymbolControl.Color2Alpha;
  }
  else if (scriptShortSymbolControl.AttributeSelect2 == "Flash Period" || scriptShortSymbolControl.AttributeSelect2 == "Position U" || scriptShortSymbolControl.AttributeSelect2 == "Position V" || scriptShortSymbolControl.AttributeSelect2 == "Rotation" ||
           scriptShortSymbolControl.AttributeSelect2 == "Scale U" || scriptShortSymbolControl.AttributeSelect2 == "Scale V")
  {
    shortSymbolControl.attributeValue2.f = scriptShortSymbolControl.AttributeValue2_Float;
  }
  else if (scriptShortSymbolControl.AttributeSelect2 == "Surface ID" || scriptShortSymbolControl.AttributeSelect2 == "Parent Symbol ID" || scriptShortSymbolControl.AttributeSelect2 == "Layer" || scriptShortSymbolControl.AttributeSelect2 == "Flash Duty Cycle Percentage")
  {
    shortSymbolControl.attributeValue2.n = scriptShortSymbolControl.AttributeValue2_Int;
  }
  else
  {
    shortSymbolControl.attributeValue2.n = scriptShortSymbolControl.AttributeValue2_Int;
  }

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendShortSymbolControl(shortSymbolControl);
}

void RegisterShortSymbolControl(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendShortSymbolControl), "sendShortSymbolControl");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptShortSymbolControl>(*m, "ShortSymbolControl", {constructor<SScriptShortSymbolControl()>(), constructor<SScriptShortSymbolControl(const SScriptShortSymbolControl&)>()},
                                                            {
                                                              {fun(&SScriptShortSymbolControl::SymbolID), "SymbolID"},
                                                              {fun(&SScriptShortSymbolControl::AttributeSelect1), "AttributeSelect1"},
                                                              {fun(&SScriptShortSymbolControl::AttributeSelect2), "AttributeSelect2"},
                                                              {fun(&SScriptShortSymbolControl::SymbolState), "SymbolState"},
                                                              {fun(&SScriptShortSymbolControl::AttachState), "AttachState"},
                                                              {fun(&SScriptShortSymbolControl::FlashControl), "FlashControl"},
                                                              {fun(&SScriptShortSymbolControl::InheritColor), "InheritColor"},
                                                              {fun(&SScriptShortSymbolControl::AttributeValue1_Float), "AttributeValue1_Float"},
                                                              {fun(&SScriptShortSymbolControl::AttributeValue1_Int), "AttributeValue1_Int"},
                                                              {fun(&SScriptShortSymbolControl::AttributeValue2_Float), "AttributeValue2_Float"},
                                                              {fun(&SScriptShortSymbolControl::AttributeValue2_Int), "AttributeValue2_Int"},
                                                              {fun(&SScriptShortSymbolControl::Color1Red), "Color1Red"},
                                                              {fun(&SScriptShortSymbolControl::Color1Green), "Color1Green"},
                                                              {fun(&SScriptShortSymbolControl::Color1Blue), "Color1Blue"},
                                                              {fun(&SScriptShortSymbolControl::Color1Alpha), "Color1Alpha"},
                                                              {fun(&SScriptShortSymbolControl::Color2Red), "Color2Red"},
                                                              {fun(&SScriptShortSymbolControl::Color2Green), "Color2Green"},
                                                              {fun(&SScriptShortSymbolControl::Color2Blue), "Color2Blue"},
                                                              {fun(&SScriptShortSymbolControl::Color2Alpha), "Color2Alpha"},
                                                            });

  chai.add(m);
}

struct SScriptSymbolTexturedCircle
{
  float CenterU = 0;
  float CenterV = 0;
  float Radius = 0;
  float InnerRadius = 0;
  float StartAngle = 0;
  float EndAngle = 0;
  float TextureCoordinateS = 0;
  float TextureCoordinateT = 0;
  float TextureMappingRadius = 0;
  float TextureMappingRotation = 0;
};

struct SScriptSymbolTexturedCircleDefinition
{
  uint16_t SymbolID = 0;
  uint16_t TextureID = 0;
  string TextureFilterMode;
  string TextureRepeatOrClamp;
  std::vector<SScriptSymbolTexturedCircle> Circles;
};

void sendSymbolTexturedCircleDefinition(SScriptSymbolTexturedCircleDefinition scriptSymbolTexturedCircleDefinition)
{
  SSymbolTexturedCircle symbolTexturedCircleDefinition;

  symbolTexturedCircleDefinition.symbolID = SymbolID(scriptSymbolTexturedCircleDefinition.SymbolID);
  symbolTexturedCircleDefinition.textureID = TextureID(scriptSymbolTexturedCircleDefinition.TextureID);
  symbolTexturedCircleDefinition.eTextureFilter = ConvertCigiStringToTextureFilterMode(scriptSymbolTexturedCircleDefinition.TextureFilterMode);
  symbolTexturedCircleDefinition.eTextureWrap = ConvertCigiStringToTextureWrapMode(scriptSymbolTexturedCircleDefinition.TextureRepeatOrClamp);

  for (auto c : scriptSymbolTexturedCircleDefinition.Circles)
  {
    STexturedCircleProperties circle;
    circle.centerUV[0] = c.CenterU;
    circle.centerUV[1] = c.CenterV;
    circle.fRadius = c.Radius;
    circle.fInnerRadius = c.InnerRadius;
    circle.startAngle = Degrees(c.StartAngle);
    circle.endAngle = Degrees(c.EndAngle);
    circle.centerTextureST[0] = c.TextureCoordinateS;
    circle.centerTextureST[1] = c.TextureCoordinateT;
    circle.fTextureMapRadius = c.TextureMappingRadius;
    circle.fTextureMapRotation = c.TextureMappingRotation;

    symbolTexturedCircleDefinition.circles.push_back(circle);
  }

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendSymbolTexturedCircleDefinition(symbolTexturedCircleDefinition);
}

void RegisterSymbolTexturedCircle(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolTexturedCircle>(*m, "SymbolTexturedCircle", {constructor<SScriptSymbolTexturedCircle()>(), constructor<SScriptSymbolTexturedCircle(const SScriptSymbolTexturedCircle&)>()},
                                                              {
                                                                {fun(&SScriptSymbolTexturedCircle::CenterU), "CenterU"},
                                                                {fun(&SScriptSymbolTexturedCircle::CenterV), "CenterV"},
                                                                {fun(&SScriptSymbolTexturedCircle::Radius), "Radius"},
                                                                {fun(&SScriptSymbolTexturedCircle::InnerRadius), "InnerRadius"},
                                                                {fun(&SScriptSymbolTexturedCircle::StartAngle), "StartAngle"},
                                                                {fun(&SScriptSymbolTexturedCircle::EndAngle), "EndAngle"},
                                                                {fun(&SScriptSymbolTexturedCircle::TextureCoordinateS), "TextureCoordinateS"},
                                                                {fun(&SScriptSymbolTexturedCircle::TextureCoordinateT), "TextureCoordinateT"},
                                                                {fun(&SScriptSymbolTexturedCircle::TextureMappingRadius), "TextureMappingRadius"},
                                                                {fun(&SScriptSymbolTexturedCircle::TextureMappingRotation), "TextureMappingRotation"},
                                                              });

  chai.add(m);
}

void RegisterSymbolTexturedCircleDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolTexturedCircleDefinition), "sendSymbolTexturedCircleDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolTexturedCircleDefinition>(*m, "SymbolTexturedCircleDefinition", {constructor<SScriptSymbolTexturedCircleDefinition()>(), constructor<SScriptSymbolTexturedCircleDefinition(const SScriptSymbolTexturedCircleDefinition&)>()},
                                                                        {
                                                                          {fun(&SScriptSymbolTexturedCircleDefinition::SymbolID), "SymbolID"},
                                                                          {fun(&SScriptSymbolTexturedCircleDefinition::TextureID), "TextureID"},
                                                                          {fun(&SScriptSymbolTexturedCircleDefinition::TextureFilterMode), "TextureFilterMode"},
                                                                          {fun(&SScriptSymbolTexturedCircleDefinition::TextureRepeatOrClamp), "TextureRepeatOrClamp"},
                                                                          {fun(&SScriptSymbolTexturedCircleDefinition::Circles), "Circles"},
                                                                        });

  chai.add(m);
  chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<SScriptSymbolTexturedCircle>>("TexturedCircleList"));
}

struct SScriptSymbolTexturedVertex
{
  float VertexU = 0;
  float VertexV = 0;
  float TextureCoordinateS = 0;
  float TextureCoordinateT = 0;
};

struct SScriptSymbolTexturedPolygonDefinition
{
  uint16_t SymbolID = 0;
  uint16_t TextureID = 0;
  string PrimitiveType;
  string TextureFilterMode;
  string TextureRepeatMode;
  std::vector<SScriptSymbolTexturedVertex> Vertices;
};

void sendSymbolTexturedPolygonDefinition(SScriptSymbolTexturedPolygonDefinition scriptSymbolTexturedPolygonDefinition)
{
  SSymbolTexturedPolygon symbolTexturedPolygonDefinition;

  symbolTexturedPolygonDefinition.symbolID = SymbolID(scriptSymbolTexturedPolygonDefinition.SymbolID);
  symbolTexturedPolygonDefinition.textureID = TextureID(scriptSymbolTexturedPolygonDefinition.TextureID);
  symbolTexturedPolygonDefinition.ePrimitiveType = ConvertCigiStringToPrimitivePolygon(scriptSymbolTexturedPolygonDefinition.PrimitiveType);
  symbolTexturedPolygonDefinition.eTextureFilterMode = ConvertCigiStringToTextureFilterMode(scriptSymbolTexturedPolygonDefinition.TextureFilterMode);
  symbolTexturedPolygonDefinition.eTextureWrapMode = ConvertCigiStringToTextureWrapMode(scriptSymbolTexturedPolygonDefinition.TextureRepeatMode);

  for (auto v : scriptSymbolTexturedPolygonDefinition.Vertices)
  {
    SSymbolTexturedPolygonVertex vertex;
    vertex.uv[0] = v.VertexU;
    vertex.uv[1] = v.VertexV;
    vertex.textureCoordinateST[0] = v.TextureCoordinateS;
    vertex.textureCoordinateST[1] = v.TextureCoordinateT;
    symbolTexturedPolygonDefinition.vertices.push_back(vertex);
  }

  CHostSession* pSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  pSession->SendSymbolTexturedPolygonDefinition(symbolTexturedPolygonDefinition);
}

void RegisterSymbolTexturedPolygonVertex(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolTexturedVertex>(*m, "SymbolTexturedPolygonVertex", {constructor<SScriptSymbolTexturedVertex()>(), constructor<SScriptSymbolTexturedVertex(const SScriptSymbolTexturedVertex&)>()},
                                                              {
                                                                {fun(&SScriptSymbolTexturedVertex::VertexU), "VertexU"},
                                                                {fun(&SScriptSymbolTexturedVertex::VertexV), "VertexV"},
                                                                {fun(&SScriptSymbolTexturedVertex::TextureCoordinateS), "TextureCoordinateS"},
                                                                {fun(&SScriptSymbolTexturedVertex::TextureCoordinateT), "TextureCoordinateT"},
                                                              });
  chai.add(m);
}

void RegisterSymbolTexturedPolygonDefinition(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&sendSymbolTexturedPolygonDefinition), "sendSymbolTexturedPolygonDefinition");
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptSymbolTexturedPolygonDefinition>(*m, "SymbolTexturedPolygonDefinition", {constructor<SScriptSymbolTexturedPolygonDefinition()>(), constructor<SScriptSymbolTexturedPolygonDefinition(const SScriptSymbolTexturedPolygonDefinition&)>()},
                                                                         {
                                                                           {fun(&SScriptSymbolTexturedPolygonDefinition::SymbolID), "SymbolID"},
                                                                           {fun(&SScriptSymbolTexturedPolygonDefinition::TextureID), "TextureID"},
                                                                           {fun(&SScriptSymbolTexturedPolygonDefinition::PrimitiveType), "PrimitiveType"},
                                                                           {fun(&SScriptSymbolTexturedPolygonDefinition::TextureFilterMode), "TextureFilterMode"},
                                                                           {fun(&SScriptSymbolTexturedPolygonDefinition::TextureRepeatMode), "TextureRepeatMode"},
                                                                           {fun(&SScriptSymbolTexturedPolygonDefinition::Vertices), "Vertices"},
                                                                         });
  chai.add(m);
}

void RegisterScriptHatHotResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptHatHotResponse>(*m, "HATHOTResponse", {constructor<SScriptHatHotResponse()>(), constructor<SScriptHatHotResponse(const SScriptHatHotResponse&)>()},
                                                        {
                                                          {fun(&SScriptHatHotResponse::HatHotId), "HATHOTId"},
                                                          {fun(&SScriptHatHotResponse::Height), "Height"},
                                                          {fun(&SScriptHatHotResponse::HostFrameLSN), "HostFrameLSN"},
                                                          {fun(&SScriptHatHotResponse::Valid), "Valid"},
                                                        });
  chai.add(m);
}

void RegisterScriptHatHotExtendedResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptHatHotExtendedResponse>(*m, "HATHOTExtendedResponse", {constructor<SScriptHatHotExtendedResponse()>(), constructor<SScriptHatHotExtendedResponse(const SScriptHatHotExtendedResponse&)>()},
                                                                {
                                                                  {fun(&SScriptHatHotExtendedResponse::HatHotId), "HATHOTId"},
                                                                  {fun(&SScriptHatHotExtendedResponse::HostFrameLSN), "HostFrameLSN"},
                                                                  {fun(&SScriptHatHotExtendedResponse::Hat), "Hat"},
                                                                  {fun(&SScriptHatHotExtendedResponse::Hot), "Hot"},
                                                                  {fun(&SScriptHatHotExtendedResponse::MaterialCode), "MaterialCode"},
                                                                  {fun(&SScriptHatHotExtendedResponse::NormalAzimuth), "NormalAzimuth"},
                                                                  {fun(&SScriptHatHotExtendedResponse::NormalElevation), "NormalElevation"},
                                                                  {fun(&SScriptHatHotExtendedResponse::Valid), "Valid"},
                                                                });
  chai.add(m);
}

void RegisterScriptLineOfSightResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptLineOfSightResponse>(*m, "LineOfSightResponse", {constructor<SScriptLineOfSightResponse()>(), constructor<SScriptLineOfSightResponse(const SScriptLineOfSightResponse&)>()},
                                                             {
                                                               {fun(&SScriptLineOfSightResponse::RequestID), "RequestID"},
                                                               {fun(&SScriptLineOfSightResponse::Valid), "Valid"},
                                                               {fun(&SScriptLineOfSightResponse::Range), "Range"},
                                                               {fun(&SScriptLineOfSightResponse::ResponseCount), "ResponseCount"},
                                                               {fun(&SScriptLineOfSightResponse::HostFrameLSN), "HostFrameLSN"},
                                                             });
  chai.add(m);
}

void RegisterScriptLineOfSightEntityResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptLineOfSightEntityResponse>(*m, "LineOfSightEntityResponse", {constructor<SScriptLineOfSightEntityResponse()>(), constructor<SScriptLineOfSightEntityResponse(const SScriptLineOfSightEntityResponse&)>()},
                                                                   {
                                                                     {fun(&SScriptLineOfSightEntityResponse::RequestID), "RequestID"},
                                                                     {fun(&SScriptLineOfSightEntityResponse::EntityID), "EntityID"},
                                                                     {fun(&SScriptLineOfSightEntityResponse::Valid), "Valid"},
                                                                     {fun(&SScriptLineOfSightEntityResponse::Visible), "Visible"},
                                                                     {fun(&SScriptLineOfSightEntityResponse::Range), "Range"},
                                                                     {fun(&SScriptLineOfSightEntityResponse::ResponseCount), "ResponseCount"},
                                                                     {fun(&SScriptLineOfSightEntityResponse::HostFrameLSN), "HostFrameLSN"},
                                                                   });
  chai.add(m);
}

void RegisterScriptLineOfSightExtendedGeodeticCoordinatesResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptLineOfSightExtendedGeodeticCoordinatesResponse>(*m, "LineOfSightExtendedGeodeticCoordinatesResponse",
                                                                                        {constructor<SScriptLineOfSightExtendedGeodeticCoordinatesResponse()>(), constructor<SScriptLineOfSightExtendedGeodeticCoordinatesResponse(const SScriptLineOfSightExtendedGeodeticCoordinatesResponse&)>()},
                                                                                        {
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::RequestID), "RequestID"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::Valid), "Valid"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::Range), "Range"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::ResponseCount), "ResponseCount"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::HostFrameLSN), "HostFrameLSN"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::RangeValid), "RangeValid"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::NormalAzimuth), "NormalAzimuth"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::NormalElevation), "NormalElevation"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::MaterialCode), "MaterialCode"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::Latitude), "Latitude"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::Longitude), "Longitude"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::Altitude), "Altitude"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::SurfaceColorR), "SurfaceColorR"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::SurfaceColorG), "SurfaceColorG"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::SurfaceColorB), "SurfaceColorB"},
                                                                                          {fun(&SScriptLineOfSightExtendedGeodeticCoordinatesResponse::SurfaceColorA), "SurfaceColorA"},
                                                                                        });
  chai.add(m);
}

void RegisterScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse>(
    *m, "LineOfSightExtendedEntityGeodeticCoordinatesResponse", {constructor<SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse()>(), constructor<SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse(const SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse&)>()},
    {
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::RequestID), "RequestID"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::EntityID), "EntityID"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::Valid), "Valid"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::Visible), "Visible"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::Range), "Range"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::ResponseCount), "ResponseCount"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::HostFrameLSN), "HostFrameLSN"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::RangeValid), "RangeValid"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::NormalAzimuth), "NormalAzimuth"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::NormalElevation), "NormalElevation"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::MaterialCode), "MaterialCode"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::Latitude), "Latitude"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::Longitude), "Longitude"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::Altitude), "Altitude"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::SurfaceColorR), "SurfaceColorR"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::SurfaceColorG), "SurfaceColorG"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::SurfaceColorB), "SurfaceColorB"},
      {fun(&SScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse::SurfaceColorA), "SurfaceColorA"},
    });
  chai.add(m);
}

void RegisterScriptLineOfSightExtendedEntityCoordinatesResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptLineOfSightExtendedEntityCoordinatesResponse>(*m, "LineOfSightExtendedEntityCoordinatesResponse",
                                                                                      {constructor<SScriptLineOfSightExtendedEntityCoordinatesResponse()>(), constructor<SScriptLineOfSightExtendedEntityCoordinatesResponse(const SScriptLineOfSightExtendedEntityCoordinatesResponse&)>()},
                                                                                      {
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::RequestID), "RequestID"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::EntityID), "EntityID"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::Valid), "Valid"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::Visible), "Visible"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::Range), "Range"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::ResponseCount), "ResponseCount"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::HostFrameLSN), "HostFrameLSN"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::RangeValid), "RangeValid"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::NormalAzimuth), "NormalAzimuth"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::NormalElevation), "NormalElevation"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::MaterialCode), "MaterialCode"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::X), "X"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::Y), "Y"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::Z), "Z"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::SurfaceColorR), "SurfaceColorR"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::SurfaceColorG), "SurfaceColorG"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::SurfaceColorB), "SurfaceColorB"},
                                                                                        {fun(&SScriptLineOfSightExtendedEntityCoordinatesResponse::SurfaceColorA), "SurfaceColorA"},
                                                                                      });
  chai.add(m);
}

void RegisterScriptCollisionSegmentResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptCollisionSegmentResponse>(*m, "CollisionSegmentResponse", {constructor<SScriptCollisionSegmentResponse()>(), constructor<SScriptCollisionSegmentResponse(const SScriptCollisionSegmentResponse&)>()},
                                                                  {
                                                                    {fun(&SScriptCollisionSegmentResponse::EntityID), "EntityID"},
                                                                    {fun(&SScriptCollisionSegmentResponse::SegmentID), "SegmentID"},
                                                                    {fun(&SScriptCollisionSegmentResponse::IntersectionDistance), "IntersectionDistance"},
                                                                    {fun(&SScriptCollisionSegmentResponse::MaterialCode), "MaterialCode"},
                                                                  });
  chai.add(m);
}

void RegisterScriptCollisionVolumeResponse(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptCollisionVolumeResponse>(*m, "CollisionVolumeResponse", {constructor<SScriptCollisionVolumeResponse()>(), constructor<SScriptCollisionVolumeResponse(const SScriptCollisionVolumeResponse&)>()},
                                                                 {
                                                                   {fun(&SScriptCollisionVolumeResponse::EntityID), "EntityID"},
                                                                   {fun(&SScriptCollisionVolumeResponse::VolumeID), "VolumeID"},
                                                                   {fun(&SScriptCollisionVolumeResponse::ContactedVolumeID), "ContactedVolumeID"},
                                                                 });
  chai.add(m);
}

void RegisterScriptPositionResponseGeodeticCoordinatesDefinition(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptPositionResponseGeodeticCoordinates>(*m, "PositionResponseGeodeticCoordinates", {constructor<SScriptPositionResponseGeodeticCoordinates()>(), constructor<SScriptPositionResponseGeodeticCoordinates(const SScriptPositionResponseGeodeticCoordinates&)>()},
                                                                             {
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::Latitude), "Latitude"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::Longitude), "Longitude"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::Altitude), "Altitude"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::ObjectClass), "ObjectClass"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::objectID), "ObjectID"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::Roll), "Roll"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::Pitch), "Pitch"},
                                                                               {fun(&SScriptPositionResponseGeodeticCoordinates::Yaw), "Yaw"},
                                                                             });
  chai.add(m);
}

void RegisterScriptPositionResponseParentEntityCoordinatesDefinition(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptPositionResponseParentEntityCoordinates>(*m, "PositionResponseParentEntityCoordinates",
                                                                                 {constructor<SScriptPositionResponseParentEntityCoordinates()>(), constructor<SScriptPositionResponseParentEntityCoordinates(const SScriptPositionResponseParentEntityCoordinates&)>()},
                                                                                 {
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::xOffset), "X"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::yOffset), "Y"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::zOffset), "Z"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::ObjectClass), "ObjectClass"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::objectID), "ObjectID"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::Roll), "Roll"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::Pitch), "Pitch"},
                                                                                   {fun(&SScriptPositionResponseParentEntityCoordinates::Yaw), "Yaw"},
                                                                                 });
  chai.add(m);
}

void RegisterScriptPositionResponseArticulatedPartCoordinatesDefinition(ChaiScript& chai)
{
  chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

  chaiscript::utility::add_class<SScriptPositionResponseArticulatedPartCoordinates>(*m, "PositionResponseArticulatedPartCoordinates",
                                                                                    {constructor<SScriptPositionResponseArticulatedPartCoordinates()>(), constructor<SScriptPositionResponseArticulatedPartCoordinates(const SScriptPositionResponseArticulatedPartCoordinates&)>()},
                                                                                    {
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::xOffset), "X"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::yOffset), "Y"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::zOffset), "Z"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::ArticulatedPartID), "ArticulatedPartID"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::ObjectClass), "ObjectClass"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::objectID), "ObjectID"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::Roll), "Roll"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::Pitch), "Pitch"},
                                                                                      {fun(&SScriptPositionResponseArticulatedPartCoordinates::Yaw), "Yaw"},
                                                                                    });
  chai.add(m);
}

void logMessage(string sMessage)
{
  HostCigiMessageEventArgs args;
  args.sMessage = sMessage;
  Event::Raise<HostCigiEvent>(args);
}

void RegisterLogMessage(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&logMessage), "logMessage");
}

void logClear()
{
  HostCigiClearMessageEventArgs args;
  Event::Raise<HostCigiEvent>(args);
}

void RegisterLogClear(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&logClear), "logClear");
}

double getHostSeconds()
{
  if (g_HostCigiLibGlobals.pHost == nullptr)
  {
    return 0;
  }

  CHostSession* pHostSession = g_HostCigiLibGlobals.pHost->GetHostSession();
  if (pHostSession == nullptr)
  {
    return 0;
  }

  return pHostSession->GetSessionTime();
}

void RegisterGetHostSeconds(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&getHostSeconds), "getHostSeconds");
}

float scriptSin(float p)
{
  return std::sin(p);
}

float scriptCos(float p)
{
  return std::cos(p);
}

float scriptModulo(float a, float b)
{
  float c = (a - (int)(a / b) * b);
  if (c < 0)
  {
    c = c + b;
  }
  return c;
}

void RegisterSin(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&scriptSin), "sin");
}

void RegisterCos(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&scriptCos), "cos");
}

void RegisterModulo(ChaiScript& chai)
{
  chai.add(chaiscript::fun(&scriptModulo), "modulo");
}

CScriptRuntime::CScriptRuntime(const std::filesystem::path& scriptsPath) : m_scriptsPath(scriptsPath)
{
  g_pScriptRuntime = this;

  m_pChaiScript = std::make_unique<ChaiScript>();
  ChaiScript& chai = *m_pChaiScript;

  chai.add(chaiscript::fun(&registerCloudType), "registerCloudType");
  chai.add(chaiscript::fun(&registerComponentClass), "registerComponentClass");
  chai.add(chaiscript::fun(&registerLayerID), "registerLayerID");
  chai.add(chaiscript::fun(&registerSurfaceConditionID), "registerSurfaceConditionID");
  chai.add(chaiscript::fun(&registerPixelReplicationMode), "registerPixelReplicationMode");
  chai.add(chaiscript::fun(&registerFontID), "registerFontID");
  chai.add(chaiscript::fun(&registerImageGeneratorMessage), "registerImageGeneratorMessage");
  chai.add(chaiscript::fun(&addEntity), "addEntity");

  // Host-To-IG Packets: Functions and Structs
  RegisterIGControl(chai);
  RegisterViewDefinition(chai);
  RegisterTopLevelEntityPosition(chai);
  RegisterChildEntityPosition(chai);
  RegisterConformalClampedEntityPosition(chai);
  RegisterEntityVelocityControl(chai);
  RegisterArticulatedPartVelocityControl(chai);
  RegisterArticulatedPartControl(chai);
  RegisterShortArticulatedPartControl(chai);
  RegisterViewControl(chai);
  RegisterComponentControl(chai);
  RegisterShortComponentControl(chai);
  RegisterEntityControl(chai);
  RegisterAnimationControl(chai);
  RegisterCelestialSphereControl(chai);
  RegisterAtmosphereControl(chai);
  RegisterWeatherControl(chai);
  RegisterEnvironmentalRegionControl(chai);
  RegisterEarthReferenceDefinition(chai);
  RegisterEntityAccelerationControl(chai);
  RegisterArticulatedPartAccelerationControl(chai);
  RegisterHATHOTGlobalRequest(chai);
  RegisterHATHOTEntityRequest(chai);
  RegisterMaritimeSurfaceConditionsControl(chai);
  RegisterGlobalMaritimeSurfaceConditionsControl(chai);
  RegisterRegionMaritimeSurfaceConditionsControl(chai);
  RegisterEntityMaritimeSurfaceConditionsControl(chai);
  RegisterEntityTerrestrialSurfaceConditionsControl(chai);
  RegisterRegionTerrestrialSurfaceConditionsControl(chai);
  RegisterGlobalTerrestrialSurfaceConditionsControl(chai);
  RegisterLineOfSightSegmentRequestGeodeticToGeodeticBasic(chai);
  RegisterLineOfSightSegmentRequestGeodeticToEntityBasic(chai);
  RegisterLineOfSightSegmentRequestEntityToGeodeticBasic(chai);
  RegisterLineOfSightSegmentRequestEntityToEntityBasic(chai);
  RegisterLineOfSightSegmentRequestGeodeticToGeodeticExtended(chai);
  RegisterLineOfSightSegmentRequestGeodeticToEntityExtended(chai);
  RegisterLineOfSightSegmentRequestEntityToGeodeticExtended(chai);
  RegisterLineOfSightSegmentRequestEntityToEntityExtended(chai);
  RegisterLineOfSightVectorRequestGeodeticBasic(chai);
  RegisterLineOfSightVectorRequestEntityBasic(chai);
  RegisterLineOfSightVectorRequestGeodeticExtended(chai);
  RegisterLineOfSightVectorRequestEntityExtended(chai);
  RegisterEnvironmentalConditionsRequest(chai);
  RegisterSymbolClone(chai);
  RegisterWaveControl(chai);
  RegisterSensorControl(chai);
  RegisterMotionTrackerControl(chai);
  RegisterCollisionDetectionSegmentDefinition(chai);
  RegisterCollisionDetectionVolumeDefinition(chai);
  RegisterSymbolSurfaceDefinition(chai);
  RegisterSymbolTextDefinition(chai);
  RegisterSymbolCircleDefinition(chai);
  RegisterVertexUV(chai);
  RegisterSymbolPolygonDefinition(chai);
  RegisterShortSymbolControl(chai);
  RegisterSymbolTexturedCircle(chai);
  RegisterSymbolTexturedCircleDefinition(chai);
  RegisterSymbolTexturedPolygonVertex(chai);
  RegisterSymbolTexturedPolygonDefinition(chai);
  RegisterPositionRequest(chai);
  RegisterSymbolControl(chai);

  // IG-To-Host Response Packets: Only Structs (Functions are setup at the top)
  RegisterScriptHatHotResponse(chai);
  RegisterScriptHatHotExtendedResponse(chai);
  RegisterScriptLineOfSightResponse(chai);
  RegisterScriptLineOfSightEntityResponse(chai);
  RegisterScriptLineOfSightExtendedEntityCoordinatesResponse(chai);
  RegisterScriptLineOfSightExtendedEntityGeodeticCoordinatesResponse(chai);
  RegisterScriptLineOfSightExtendedGeodeticCoordinatesResponse(chai);
  RegisterScriptCollisionSegmentResponse(chai);
  RegisterScriptCollisionVolumeResponse(chai);
  RegisterScriptPositionResponseGeodeticCoordinatesDefinition(chai);
  RegisterScriptPositionResponseParentEntityCoordinatesDefinition(chai);
  RegisterScriptPositionResponseArticulatedPartCoordinatesDefinition(chai);
  RegisterWeatherConditionsResponse(chai);
  RegisterAerosolConcentrationResponse(chai);
  RegisterMaritimeSurfaceConditionsResponse(chai);
  RegisterTerrestrialSurfaceConditionsResponse(chai);

  // Miscellaneous Functions
  RegisterLogMessage(chai);
  RegisterLogClear(chai);
  RegisterGetHostSeconds(chai);
  RegisterSin(chai);
  RegisterCos(chai);
  RegisterModulo(chai);

  chai.add(chaiscript::fun(&Pause), "Pause");
  chai.add(chaiscript::fun(&Wait), "Wait");

  //grab current state
  base_state = m_pChaiScript->get_state();
  base_locals = m_pChaiScript->get_locals();

  if (g_HostCigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_HostCigiLibGlobals.pEventDispatcher->RegisterListener<HostCigiEvent>(this);
  }
}

CScriptRuntime::~CScriptRuntime()
{
  if (g_HostCigiLibGlobals.pEventDispatcher != nullptr)
  {
    g_HostCigiLibGlobals.pEventDispatcher->UnregisterListener<HostCigiEvent>(this);
  }

  g_pScriptRuntime = nullptr;
}

void CScriptRuntime::AddWaitCallback(const SWaitCallback& waitCallback)
{
  m_WaitCallbacks.push_back(waitCallback);
}

void CScriptRuntime::Execute(const std::string& sCategory, const std::string& sFilename)
{
  try
  {
    m_bIsExecutingScript = true;
    m_WaitCallbacks.clear();

    //restore previous state
    m_pChaiScript->set_state(base_state);
    m_pChaiScript->set_locals(base_locals);

    m_pChaiScript->eval("def function_exists(String name) { return contains(get_function_objects(), fun(elem){ return elem.first == name; }); }");

    //evaluate file
    m_pChaiScript->eval_file((m_scriptsPath / sCategory / sFilename).string().c_str());

    auto pFunctionExists = m_pChaiScript->eval<std::function<bool(const std::string&)>>("function_exists");
    if (pFunctionExists("Start"))
    {
      auto pFuncStart = m_pChaiScript->eval<std::function<void()>>("Start");
      pFuncStart();
    }
  }
  catch (const chaiscript::exception::eval_error& ee)
  {
    m_bIsExecutingScript = false;
    m_WaitCallbacks.clear();

    string s = ee.pretty_print();

    HostCigiErrorEventArgs args;
    args.sError = s;
    Event::Raise<HostCigiEvent>(args);
  }
  catch (const std::exception& ex)
  {
    m_bIsExecutingScript = false;
    m_WaitCallbacks.clear();

    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

void CScriptRuntime::LoadConfiguration()
{
  try
  {
    m_pChaiScript->eval_file((m_scriptsPath / "configure.chai").string());
  }
  catch (const chaiscript::exception::eval_error& ee)
  {
    string s = ee.pretty_print();

    HostCigiErrorEventArgs args;
    args.sError = s;
    Event::Raise<HostCigiEvent>(args);
  }
  catch (const std::exception& ex)
  {
    HostCigiErrorEventArgs args;
    args.sError = ex.what();
    Event::Raise<HostCigiEvent>(args);
  }
}

sbio::CloudType CScriptRuntime::GetCloudType(const std::string& sCloudType) const
{
  auto it = m_CloudTypeValues.find(sCloudType);
  if (it == m_CloudTypeValues.end())
  {
    return CloudType(static_cast<uint8_t>(atoi(sCloudType.c_str())));
  }

  return it->second;
}

std::string CScriptRuntime::GetCloudTypeString(const sbio::CloudType& cloudType) const
{
  for (auto it = m_CloudTypeValues.begin(); it != m_CloudTypeValues.end(); ++it)
  {
    if (it->second == cloudType)
    {
      return it->first;
    }
  }

  return "";
}

FontID CScriptRuntime::GetFontID(const std::string& sCategory, const std::string& sFontName)
{
  for (auto f : m_Fonts)
  {
    if (f.sCategory == sCategory && f.sFontName == sFontName)
    {
      return f.fontID;
    }
  }

  return UnknownFontID;
}

std::list<std::string> CScriptRuntime::GetFontCategories() const
{
  std::list<string> categories;

  for (auto f : m_Fonts)
  {
    if (find(categories.begin(), categories.end(), f.sCategory) == categories.end())
    {
      categories.push_back(f.sCategory);
    }
  }

  return categories;
}

std::list<std::string> CScriptRuntime::GetFontNames(const std::string& sCategory) const
{
  std::list<string> fonts;

  for (auto f : m_Fonts)
  {
    if (f.sCategory == sCategory)
    {
      fonts.push_back(f.sFontName);
    }
  }

  return fonts;
}

const std::list<std::string>& CScriptRuntime::GetCloudTypes() const
{
  return m_CloudTypes;
}

const std::list<std::string>& CScriptRuntime::GetComponentClasses() const
{
  return m_ComponentClasses;
}

const std::list<std::string>& CScriptRuntime::GetPixelReplicationModes() const
{
  return m_PixelReplicationModes;
}

const std::list<std::string>& CScriptRuntime::GetSurfaceConditions() const
{
  return m_SurfaceConditions;
}

std::string CScriptRuntime::GetComponentClassString(sbio::cigi::CigiComponentClassID componentClassID) const
{
  for (auto v : m_ComponentClassValues)
  {
    if (v.second == componentClassID)
    {
      return v.first;
    }
  }

  return "";
}

sbio::cigi::CigiComponentClassID CScriptRuntime::GetComponentClassID(const std::string& sComponentClass) const
{
  auto it = m_ComponentClassValues.find(sComponentClass);
  if (it == m_ComponentClassValues.end())
  {
    return CigiComponentClassID(static_cast<uint8_t>(atoi(sComponentClass.c_str())));
  }

  return it->second;
}

std::string CScriptRuntime::GetImageGeneratorMessage(int nMessageID) const
{
  auto it = m_ImageGeneratorMessages.find(nMessageID);

  if (it == m_ImageGeneratorMessages.end())
  {
    return "";
  }

  return it->second;
}

PixelReplicationMode CScriptRuntime::GetPixelReplicationModeID(const std::string& sPixelReplicationMode) const
{
  auto it = m_PixelReplicationModeValues.find(sPixelReplicationMode);
  if (it == m_PixelReplicationModeValues.end())
  {
    return PixelReplicationMode(static_cast<uint8_t>(atoi(sPixelReplicationMode.c_str())));
  }

  return it->second;
}

std::string CScriptRuntime::GetPixelReplicationModeString(const PixelReplicationMode& pixelReplicationMode) const
{
  for (auto it = m_PixelReplicationModeValues.begin(); it != m_PixelReplicationModeValues.end(); ++it)
  {
    if (it->second == pixelReplicationMode)
    {
      return it->first;
    }
  }

  return "";
}

sbio::SurfaceConditionID CScriptRuntime::GetSurfaceConditionID(const std::string& sSurfaceCondition) const
{
  auto it = m_SurfaceConditionValues.find(sSurfaceCondition);
  if (it == m_SurfaceConditionValues.end())
  {
    return SurfaceConditionID(static_cast<uint16_t>(atoi(sSurfaceCondition.c_str())));
  }

  return it->second;
}

int CScriptRuntime::GetLayerID(const std::string& sLayerID) const
{
  auto it = m_LayerValues.find(sLayerID);
  if (it == m_LayerValues.end())
  {
    return atoi(sLayerID.c_str());
  }

  return it->second;
}

std::string CScriptRuntime::GetLayerIDString(const int& layerID) const
{
  for (auto it = m_LayerValues.begin(); it != m_LayerValues.end(); ++it)
  {
    if (it->second == layerID)
    {
      return it->first;
    }
  }

  return "";
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
