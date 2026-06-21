//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTRequestHandler.h
 * @brief Declares the CHATHOTRequestHandler base class for SimBlocks CIGI IG HAT/HOT request handling.
 *
 * Provides the CHATHOTRequestHandler base class for managing and processing HAT/HOT requests in the SimBlocks CIGI IG library.
 * Supports access to common request metadata, request dispatch, and resolution of the request data into engine terrain messages.
 *
 * @see CHATHOTRequestHandler
 * @see sbio::cigi::SBaseHATHOTRequest
 * @see sbio::cigi::SHATHOTGlobalRequest
 * @see sbio::cigi::SHATHOTEntityRequest
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HAT_HOT_REQUEST_HANDLER_H
#define SIMBLOCKS_CIGI_HAT_HOT_REQUEST_HANDLER_H

#include "CigiLib/CigiTypesHostToIG.h"

/**
 * @brief Handles HAT/HOT requests for SimBlocks CIGI IG integration.
 */
class CHATHOTRequestHandler
{
public:
  CHATHOTRequestHandler();
  virtual ~CHATHOTRequestHandler();

  sbio::FrameNumber GetLastHostFrameNumber() const;
  sbio::cigi::UpdatePeriod GetUpdatePeriod() const;
  sbio::cigi::ERequestType GetRequestType() const;
  sbio::HATHOTID GetRequestID() const;
  void SetLastHostFrameNumber(sbio::FrameNumber lastHostFrameNumber);

  virtual sbio::ETopLevelCoordinateSystem GetCoordinateSystem() const = 0;
  virtual sbio::EntityID GetEntityID() const = 0;
  virtual sbio::math::SGeodeticCoordinates GetGeodeticCoordinates() const = 0;
  virtual sbio::math::Vec3 GetOffset() const = 0;

  virtual bool Handle() = 0;

protected:
  bool SendRequest(const sbio::math::SGeodeticCoordinates& resolvedPoint);

  virtual const sbio::cigi::SBaseHATHOTRequest& GetRequest() const = 0;
  virtual sbio::cigi::SBaseHATHOTRequest& GetRequestRef() = 0;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
