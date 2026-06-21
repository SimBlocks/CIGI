//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightSegmentRequestGeodeticToGeodeticHandler.h
 * @brief Declares the CLineOfSightSegmentRequestGeodeticToGeodeticHandler class for SimBlocks CIGI IG line of sight segment request handling from geodetic coordinates to geodetic coordinates.
 *
 * Provides the CLineOfSightSegmentRequestGeodeticToGeodeticHandler class for managing and processing line of sight segment requests from geodetic coordinates to geodetic coordinates in the SimBlocks CIGI IG library.
 * Inherits from CLineOfSightSegmentRequestGeodeticHandler and integrates with SimBlocks CIGI, math, and geodetic types for simulation and line of sight calculations.
 * Supports geodetic-to-geodetic request management and end point resolution for line of sight segment requests.
 *
 * @see CLineOfSightSegmentRequestGeodeticToGeodeticHandler
 * @see CLineOfSightSegmentRequestGeodeticHandler
 * @see CLineOfSightSegmentRequestHandler
 * @see sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodetic
 * @see sbio::math::GeocentricCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_GEODETIC_TO_GEODETIC_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_GEODETIC_TO_GEODETIC_HANDLER_H

#include "LineOfSightRequestHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"

/**
 * @brief Handles line of sight segment requests from geodetic coordinates to geodetic coordinates in SimBlocks CIGI IG integration.
 */
class CLineOfSightSegmentRequestGeodeticToGeodeticBasicHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a line of sight segment request handler for geodetic-to-geodetic requests.
   * @param request Geodetic-to-geodetic segment request data.
   */
  CLineOfSightSegmentRequestGeodeticToGeodeticBasicHandler(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic& request);

  virtual bool Handle() override;

  bool ResolveStartPoint(GeocentricCoordinates& point);

  /**
   * @brief Resolves the end point for the segment request.
   * @param point Output parameter for the end point coordinates.
   * @return True if resolved successfully, false otherwise.
   */
  bool ResolveEndPoint(GeocentricCoordinates& point);

protected:
  virtual const sbio::cigi::SLineOfSightRequest& GetRequest() const override;
  virtual sbio::cigi::SLineOfSightRequest& GetRequestRef() override;

private:
  sbio::cigi::SLineOfSightSegmentRequestGeodeticToGeodeticBasic m_Request;///< Geodetic-to-geodetic segment request data
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
