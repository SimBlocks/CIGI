//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightSegmentRequestEntityToEntityHandler.h
 * @brief Declares the CLineOfSightSegmentRequestEntityToEntityHandler class for SimBlocks CIGI IG line of sight segment request handling between entities.
 *
 * Provides the CLineOfSightSegmentRequestEntityToEntityHandler class for managing and processing line of sight segment requests between entities in the SimBlocks CIGI IG library.
 * Inherits from CLineOfSightSegmentRequestEntityHandler and integrates with SimBlocks CIGI, math, and entity types for simulation and line of sight calculations.
 * Supports entity-to-entity request management and end point resolution for line of sight segment requests.
 *
 * @see CLineOfSightSegmentRequestEntityToEntityHandler
 * @see CLineOfSightSegmentRequestEntityHandler
 * @see CLineOfSightSegmentRequestHandler
 * @see sbio::cigi::SLineOfSightSegmentRequestEntityToEntity
 * @see sbio::cigi::SBaseLineOfSightExtendedResponse
 * @see sbio::math::GeocentricCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_ENTITY_TO_ENTITY_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_ENTITY_TO_ENTITY_HANDLER_H

#include "LineOfSightRequestHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"

/**
 * @brief Handles line of sight segment requests between entities in SimBlocks CIGI IG integration.
 */
class CLineOfSightSegmentRequestEntityToEntityBasicHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a line of sight segment request handler for entity-to-entity requests.
   * @param request Entity-to-entity segment request data.
   */
  CLineOfSightSegmentRequestEntityToEntityBasicHandler(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic& request);

  virtual bool Handle() override;

  bool ResolveStartPoint(GeocentricCoordinates& point);

  /**
   * @brief Resolves the end point for the segment request.
   * @param point Output parameter for the end point coordinates.
   * @return True if resolved successfully, false otherwise.
   */
  bool ResolveEndPoint(GeocentricCoordinates& point);

protected:
  //virtual const sbio::cigi::SLineOfSightSegmentRequestEntity& GetEntityRequest() const override;
  virtual const sbio::cigi::SLineOfSightRequest& GetRequest() const override;
  virtual sbio::cigi::SLineOfSightRequest& GetRequestRef() override;

private:
  sbio::cigi::SLineOfSightSegmentRequestEntityToEntityBasic m_Request;///< Entity-to-entity segment request data
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
