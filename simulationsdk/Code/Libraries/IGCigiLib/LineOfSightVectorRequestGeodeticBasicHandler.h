//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightVectorRequestGeodeticHandler.h
 * @brief Declares the CLineOfSightVectorRequestGeodeticHandler class for SimBlocks CIGI IG line of sight vector request handling for geodetic coordinates.
 *
 * Provides the CLineOfSightVectorRequestGeodeticHandler class for managing and processing line of sight vector requests for geodetic coordinates in the SimBlocks CIGI IG library.
 * Inherits from CLineOfSightRequestHandler and integrates with SimBlocks CIGI, math, and geodetic types for simulation and line of sight calculations.
 * Supports geodetic-specific request management, start point resolution, rotation retrieval, and entity coordinate system response handling for line of sight vector requests.
 *
 * @see CLineOfSightVectorRequestGeodeticHandler
 * @see CLineOfSightRequestHandler
 * @see sbio::cigi::SLineOfSightVectorRequestGeodetic
 * @see sbio::cigi::SBaseLineOfSightExtendedResponse
 * @see sbio::math::GeocentricCoordinates
 * @see sbio::math::Quaternion4d
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LINE_OF_SIGHT_VECTOR_REQUEST_GEODETIC_BASIC_HANDLER_H
#define SIMBLOCKS_CIGI_LINE_OF_SIGHT_VECTOR_REQUEST_GEODETIC_BASIC_HANDLER_H

#include "LineOfSightRequestHandler.h"

/**
 * @brief Handles line of sight vector requests for geodetic coordinates in SimBlocks CIGI IG integration.
 */
class CLineOfSightVectorRequestGeodeticBasicHandler : public CLineOfSightRequestHandler
{
public:
  /**
   * @brief Constructs a line of sight vector request handler for geodetic coordinates.
   * @param lineOfSightRequest Geodetic vector request data.
   */
  CLineOfSightVectorRequestGeodeticBasicHandler(const sbio::cigi::SLineOfSightVectorRequestGeodeticBasic& lineOfSightRequest);

  virtual bool Handle() override;

  /**
   * @brief Gets the rotation for the vector request.
   * @return Quaternion representing the rotation.
   */
  sbio::math::Quaternion4d GetRotation() const;
  /**
   * @brief Resolves the start point for the vector request.
   * @param point Output parameter for the start point coordinates.
   * @return True if resolved successfully, false otherwise.
   */
  bool ResolveStartPoint(sbio::math::GeocentricCoordinates& point);

protected:
  virtual const sbio::cigi::SLineOfSightRequest& GetRequest() const override;
  virtual sbio::cigi::SLineOfSightRequest& GetRequestRef() override;

private:
  sbio::cigi::SLineOfSightVectorRequestGeodeticBasic m_Request;///< Geodetic vector request data
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
