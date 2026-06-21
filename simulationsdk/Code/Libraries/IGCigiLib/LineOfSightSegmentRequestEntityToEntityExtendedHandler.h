//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_ENTITY_TO_ENTITY_EXTENDED_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_ENTITY_TO_ENTITY_EXTENDED_HANDLER_H

#include "LineOfSightRequestHandler.h"

/**
 * @brief Handles extended line-of-sight segment requests from one entity to another entity.
 *
 * The handler stores the request data and resolves both segment endpoints before delegating
 * common line-of-sight request handling behavior to the base type.
 */
class CLineOfSightSegmentRequestEntityToEntityExtendedHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a handler for an entity-to-entity extended segment request.
   * @param request Request data to process.
   */
  CLineOfSightSegmentRequestEntityToEntityExtendedHandler(const sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended& request);

  /**
   * @brief Processes the stored line-of-sight segment request.
   * @return `true` when handling succeeds; otherwise `false`.
   */
  virtual bool Handle() override;

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

private:
  sbio::cigi::SLineOfSightSegmentRequestEntityToEntityExtended m_Request;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
