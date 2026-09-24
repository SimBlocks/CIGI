//Copyright SimBlocks LLC 2016-2026
/**
 * @file LineOfSightRequestHandler.h
 * @brief Declares the CLineOfSightRequestHandler class for SimBlocks CIGI IG line of sight request handling.
 *
 * Provides the CLineOfSightRequestHandler class for managing and processing line of sight requests and responses in the SimBlocks IGCigiLib library.
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
   * @param rangeOffset Distance from the original source to the engine query start; zero for segments.
   * @param sourceEntityID Source entity dependency, or UnknownEntityID for a geodetic source.
   * @param destinationEntityID Destination entity dependency, or UnknownEntityID for a geodetic destination.
   */
  explicit CLineOfSightRequestHandler(double rangeOffset = 0.0, sbio::EntityID sourceEntityID = sbio::UnknownEntityID, sbio::EntityID destinationEntityID = sbio::UnknownEntityID);

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
  /** @brief Gets this handler's response-correlation generation.
   * @return Generation allocated at construction to distinguish reused request identifiers.
   */
  uint64_t GetRequestGeneration() const;
  /** @brief Gets the distance excluded before the engine query's start point.
   * @return Range offset supplied at construction; zero for segment handlers.
   */
  double GetRangeOffset() const;

  /** @brief Tests whether the request depends on a particular entity.
   * @param entityID Entity identifier to compare against both endpoint dependencies.
   * @return `true` for a matching non-unknown identifier; otherwise `false`.
   */
  bool ReferencesEntity(sbio::EntityID entityID) const;

  /** @brief Gets the host frame nibble used in responses.
   * @return Low four host-frame bits for recurring requests, or zero for one-shot requests.
   */
  uint8_t GetHostFrameLSN() const;

  /** @brief Records the repeat-interval baseline.
   * @param frame Last dispatch frame.
   */
  void SetLastUpdateFrame(sbio::FrameNumber frame);
  /** @brief Tests whether the repeat interval has elapsed.
   * @param frame Current frame number.
   * @return Whether unsigned frame distance from the baseline is at least the request's update period.
   */
  bool IsUpdateDue(sbio::FrameNumber frame) const;

  /**
   * @brief Handles the line of sight request (pure virtual).
   * @return `true` when the engine request is submitted; `false` when services or endpoint resolution fail.
   *         Does not report whether the ray intersects terrain or an entity.
   */
  virtual bool Handle() = 0;
  /**
   * @brief Handles geodetic coordinate system response.
   * @param lineOfSightExtendedResponse Extended response data.
   * @param IntersectionPoint Intersection point coordinates.
   */
  void HandleGeodeticCoordinateSystemResponse(sbio::cigi::SLineOfSightExtendedGeodeticCoordinatesResponse& lineOfSightExtendedResponse,
                                              sbio::math::GeocentricCoordinates IntersectionPoint);

  /**
   * @brief Records a one-shot response and reports whether the advertised total has been received.
   * @param responseCount Total response packet count. Zero is treated as a single response.
   * @return True when the response sequence is complete.
   */
  bool RecordResponse(uint8_t responseCount);

protected:
  /**
   * @brief Gets the line of sight request.
   * @return Reference to the line of sight request.
   */
  virtual const sbio::cigi::SLineOfSightRequest& GetRequest() const = 0;

  /** @brief Exposes mutable request metadata supplied by the concrete handler.
   * @return Borrowed reference to the concrete handler's stored request.
   */
  virtual sbio::cigi::SLineOfSightRequest& GetRequestRef() = 0;

private:
  const double m_RangeOffset;
  const sbio::EntityID m_SourceEntityID;
  const sbio::EntityID m_DestinationEntityID;
  uint64_t m_RequestGeneration = 0;
  uint16_t m_ResponsesReceived = 0;
  uint32_t m_LastUpdateFrame = 0;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
