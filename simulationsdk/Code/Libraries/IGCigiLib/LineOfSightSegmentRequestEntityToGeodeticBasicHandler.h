//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightSegmentRequestEntityToGeodeticHandler.h
 * @brief Declares the CLineOfSightSegmentRequestEntityToGeodeticHandler class for SimBlocks CIGI IG line of sight segment request handling from entity to geodetic coordinates.
 *
 * Provides the CLineOfSightSegmentRequestEntityToGeodeticHandler class for managing and processing line of sight segment requests from an entity to geodetic coordinates in the SimBlocks CIGI IG library.
 * Inherits from CLineOfSightSegmentRequestEntityHandler and integrates with SimBlocks CIGI, math, and entity types for simulation and line of sight calculations.
 * Supports entity-to-geodetic request management and end point resolution for line of sight segment requests.
 *
 * @see CLineOfSightSegmentRequestEntityToGeodeticHandler
 * @see CLineOfSightSegmentRequestEntityHandler
 * @see CLineOfSightSegmentRequestHandler
 * @see sbio::cigi::SLineOfSightSegmentRequestEntityToGeodetic
 * @see sbio::math::GeocentricCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_ENTITY_TO_GEODETIC_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_ENTITY_TO_GEODETIC_HANDLER_H

#include "LineOfSightRequestHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"

/**
 * @brief Handles line of sight segment requests from an entity to geodetic coordinates in SimBlocks CIGI IG integration.
 */
class CLineOfSightSegmentRequestEntityToGeodeticBasicHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a line of sight segment request handler for entity-to-geodetic requests.
   * @param request Entity-to-geodetic segment request data.
   */
  CLineOfSightSegmentRequestEntityToGeodeticBasicHandler(const sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic& request);

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
  sbio::cigi::SLineOfSightSegmentRequestEntityToGeodeticBasic m_Request;///< Entity-to-geodetic segment request data
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
