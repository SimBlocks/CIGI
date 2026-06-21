//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightRequestHandler.h
 * @brief Declares the CLineOfSightRequestHandler class for SimBlocks CIGI IG line of sight request handling.
 *
 * Provides the CLineOfSightRequestHandler class for managing and processing line of sight requests and responses in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, math, and coordinate system types for simulation and line of sight calculations.
 * Supports request management, response coordinate system selection, update period handling, and response processing for entity and geodetic coordinate systems.
 *
 * @see CLineOfSightRequestHandler
 * @see sbio::cigi::SLineOfSightRequest
 * @see sbio::cigi::SBaseLineOfSightExtendedResponse
 * @see sbio::math::GeocentricCoordinates
 * @see sbio::FrameNumber
 * @see sbio::ETopLevelCoordinateSystem
 * @see sbio::cigi::UpdatePeriod
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_REQUEST_HANDLER_H

#include "CigiLib/CigiTypesHostToIG.h"
#include "CigiLib/CigiTypesIGToHost.h"

/**
 * @brief Handles line of sight requests and responses for SimBlocks CIGI IG integration.
 */
class CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a line of sight request handler.
   * @param lineOfSightRequest Reference to the line of sight request.
   */
  CLineOfSightRequestHandler();
  /**
   * @brief Destroys the line of sight request handler.
   */
  virtual ~CLineOfSightRequestHandler();

  /**
   * @brief Gets the last host frame number.
   * @return Last host frame number.
   */
  sbio::FrameNumber GetLastHostFrameNumber() const;

  /**
   * @brief Gets the update period for the request.
   * @return Update period value.
   */
  sbio::cigi::UpdatePeriod GetUpdatePeriod() const;
  /**
   * @brief Sets the last host frame number.
   * @param lastHostFrameNumber Frame number value.
   */
  void SetLastHostFrameNumber(sbio::FrameNumber lastHostFrameNumber);

  /**
   * @brief Handles the line of sight request (pure virtual).
   * @return True if handled successfully, false otherwise.
   */
  virtual bool Handle() = 0;
  /**
   * @brief Handles geodetic coordinate system response.
   * @param lineOfSightExtendedResponse Extended response data.
   * @param IntersectionPoint Intersection point coordinates.
   */
  void HandleGeodeticCoordinateSystemResponse(sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& lineOfSightExtendedResponse, sbio::math::GeocentricCoordinates IntersectionPoint);

protected:
  /**
   * @brief Gets the line of sight request.
   * @return Reference to the line of sight request.
   */
  virtual const sbio::cigi::SLineOfSightRequest& GetRequest() const = 0;

  virtual sbio::cigi::SLineOfSightRequest& GetRequestRef() = 0;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
