//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_GEODETIC_TO_ENTITY_EXTENDED_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_GEODETIC_TO_ENTITY_EXTENDED_HANDLER_H

#include "LineOfSightRequestHandler.h"

/**
 * @brief Handles extended line-of-sight segment requests from a geodetic point to an entity.
 *
 * The handler stores the request data and resolves the geodetic and entity endpoints used by
 * the common line-of-sight request processing path.
 */
class CLineOfSightSegmentRequestGeodeticToEntityExtendedHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a handler for a geodetic-to-entity extended segment request.
   * @param request Request data to process.
   */
  CLineOfSightSegmentRequestGeodeticToEntityExtendedHandler(const sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended& request);

  /**
   * @brief Processes the stored line-of-sight segment request.
   * @return `true` when handling succeeds; otherwise `false`.
   */
  virtual bool Handle() override;

protected:
  /**
   * @brief Gets the stored request through the base request interface.
   * @return Const reference to the stored request data.
   */
  virtual const sbio::cigi::SLineOfSightRequest& GetRequest() const override;
  /**
   * @brief Gets mutable access to the stored request through the base request interface.
   * @return Mutable reference to the stored request data.
   */
  virtual sbio::cigi::SLineOfSightRequest& GetRequestRef() override;

  /**
   * @brief Resolves the start point for the segment request.
   * @param point Output parameter receiving the resolved geocentric point.
   * @return `true` when the point is resolved; otherwise `false`.
   */
  bool ResolveStartPoint(GeocentricCoordinates& point);

  /**
   * @brief Resolves the end point for the segment request.
   * @param point Output parameter for the end point coordinates.
   * @return True if resolved successfully, false otherwise.
   */
  bool ResolveEndPoint(GeocentricCoordinates& point);

private:
  sbio::cigi::SLineOfSightSegmentRequestGeodeticToEntityExtended m_Request;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
