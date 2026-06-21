//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTGlobalRequestHandler.h
 * @brief Declares the CHATHOTGlobalRequestHandler class for geodetic HAT/HOT request handling.
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HAT_HOT_GLOBAL_REQUEST_HANDLER_H
#define SIMBLOCKS_CIGI_HAT_HOT_GLOBAL_REQUEST_HANDLER_H

#include "HATHOTRequestHandler.h"

/**
 * @brief Handles geodetic HAT/HOT requests for SimBlocks CIGI IG integration.
 */
class CHATHOTGlobalRequestHandler : public CHATHOTRequestHandler
{
public:
  CHATHOTGlobalRequestHandler(const sbio::cigi::SHATHOTGlobalRequest& request);

  virtual sbio::ETopLevelCoordinateSystem GetCoordinateSystem() const override;
  virtual sbio::EntityID GetEntityID() const override;
  virtual sbio::math::SGeodeticCoordinates GetGeodeticCoordinates() const override;
  virtual sbio::math::Vec3 GetOffset() const override;
  virtual bool Handle() override;

protected:
  virtual const sbio::cigi::SBaseHATHOTRequest& GetRequest() const override;
  virtual sbio::cigi::SBaseHATHOTRequest& GetRequestRef() override;

private:
  sbio::cigi::SHATHOTGlobalRequest m_Request;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
