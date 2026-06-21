//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightSegmentRequestGeodeticToEntityHandler.h
 * @brief Declares the CLineOfSightSegmentRequestGeodeticToEntityHandler class for SimBlocks CIGI IG line of sight segment request handling from geodetic coordinates to entity.
 *
 * Provides the CLineOfSightSegmentRequestGeodeticToEntityHandler class for managing and processing line of sight segment requests from geodetic coordinates to an entity in the SimBlocks CIGI IG library.
 * Inherits from CLineOfSightSegmentRequestGeodeticHandler and integrates with SimBlocks CIGI, math, and entity types for simulation and line of sight calculations.
 * Supports geodetic-to-entity request management and end point resolution for line of sight segment requests.
 *
 * @see CLineOfSightSegmentRequestGeodeticToEntityHandler
 * @see CLineOfSightSegmentRequestGeodeticHandler
 * @see CLineOfSightSegmentRequestHandler
 * @see sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntity
 * @see sbio::math::GeocentricCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_GEODETIC_TO_ENTITY_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_GEODETIC_TO_ENTITY_HANDLER_H

#include "LineOfSightRequestHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"

/**
 * @brief Handles line of sight segment requests from geodetic coordinates to an entity in SimBlocks CIGI IG integration.
 */
class CLineOfSightSegmentRequestGeodeticToEntityBasicHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a line of sight segment request handler for geodetic-to-entity requests.
   * @param request Geodetic-to-entity segment request data.
   */
  CLineOfSightSegmentRequestGeodeticToEntityBasicHandler(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic& request);

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
  sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityBasic m_Request;///< Geodetic-to-entity segment request data
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
