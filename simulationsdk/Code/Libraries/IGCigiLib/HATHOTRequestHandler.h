//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTRequestHandler.h
 * @brief Declares the CHATHOTRequestHandler base class for SimBlocks CIGI IG HAT/HOT request handling.
 *
 * Provides the CHATHOTRequestHandler base class for managing and processing HAT/HOT requests in the SimBlocks IGCigiLib library.
 * Supports access to common request metadata, request dispatch, and resolution of the request data into engine terrain messages.
 *
 * @see CHATHOTRequestHandler
 * @see sbio::cigi::SBaseHATHOTRequest
 * @see sbio::cigi::SHATHOTGlobalRequest
 * @see sbio::cigi::SHATHOTEntityRequest
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HAT_HOT_REQUEST_HANDLER_H
#define SIMBLOCKS_CIGI_HAT_HOT_REQUEST_HANDLER_H

#include "CigiLib/CigiTypesHostToIG.h"

/**
 * @brief Handles HAT/HOT requests for SimBlocks CIGI IG integration.
 */
class CHATHOTRequestHandler
{
public:
  /** @brief Allocates a correlation generation for this request-handler instance. */
  CHATHOTRequestHandler();
  /** @brief Destroys the handler; does not cancel engine work already dispatched. */
  virtual ~CHATHOTRequestHandler();

  /** @brief Gets the host frame stored with the request.
   * @return Stored host frame number.
   */
  sbio::FrameNumber GetLastHostFrameNumber() const;
  /** @brief Gets the requested repeat interval.
   * @return Stored update period in frames; zero denotes a one-shot request.
   */
  sbio::cigi::UpdatePeriod GetUpdatePeriod() const;
  /** @brief Gets the requested terrain-height response form.
   * @return Stored HAT, HOT, or extended request selector.
   */
  sbio::cigi::ERequestType GetRequestType() const;
  /** @brief Gets the identifier used to correlate terrain-height responses.
   * @return Identifier from the owned request payload.
   */
  sbio::HATHOTID GetRequestID() const;
  /** @brief Replaces the host frame stored in the request.
   * @param lastHostFrameNumber Frame number to store.
   */
  void SetLastHostFrameNumber(sbio::FrameNumber lastHostFrameNumber);
  /** @brief Gets this handler's response-correlation generation.
   * @return Generation allocated at construction, used to distinguish reused request IDs.
   */
  uint64_t GetRequestGeneration() const;

  /** @brief Gets the host frame nibble for response correlation.
   * @return Low four frame bits for a recurring request, or zero for a one-shot request.
   */
  uint8_t GetHostFrameLSN() const;

  /** @brief Records the frame used as the repeat-interval baseline.
   * @param frame Last dispatch frame.
   */
  void SetLastUpdateFrame(sbio::FrameNumber frame);

  /** @brief Tests whether the request's repeat interval has elapsed.
   * @param frame Current frame number.
   * @return Whether unsigned frame distance from the baseline is at least the stored update period.
   */
  bool IsUpdateDue(sbio::FrameNumber frame) const;

  /** @brief Identifies the request's position representation.
   * @return Coordinate selector supplied by the concrete request handler.
   */
  virtual sbio::ETopLevelCoordinateSystem GetCoordinateSystem() const = 0;

  /** @brief Gets the referenced entity, if any.
   * @return Entity identifier or `UnknownEntityID` for a geodetic request.
   */
  virtual sbio::EntityID GetEntityID() const = 0;

  /** @brief Gets the stored geodetic position, where applicable.
   * @return Geodetic request position; entity-relative handlers return a default-constructed value.
   */
  virtual sbio::math::SGeodeticCoordinates GetGeodeticCoordinates() const = 0;

  /** @brief Gets the entity-relative offset, where applicable.
   * @return CIGI body offset; geodetic handlers return a default-constructed vector.
   */
  virtual sbio::math::Vec3 GetOffset() const = 0;

  /** @brief Resolves the request position and submits a terrain query.
   * @return `true` when submitted; `false` when required services/targets are unavailable or the type is unsupported.
   *         Submission does not guarantee a valid terrain response.
   */
  virtual bool Handle() = 0;

protected:
  /** @brief Dispatches the selected HAT, HOT, or extended query through the global messenger.
   * @param resolvedPoint Geodetic position after any entity-relative offset has been resolved.
   * @return `true` after invoking the messenger; `false` if absent or the request type is unsupported.
   */
  bool SendRequest(const sbio::math::SGeodeticCoordinates& resolvedPoint);

  /** @brief Exposes the concrete handler's stored request metadata.
   * @return Borrowed const reference to request data owned by the derived handler.
   */
  virtual const sbio::cigi::SBaseHATHOTRequest& GetRequest() const = 0;
  /** @brief Exposes mutable stored request metadata.
   * @return Borrowed reference to request data owned by the derived handler.
   */
  virtual sbio::cigi::SBaseHATHOTRequest& GetRequestRef() = 0;

private:
  uint64_t m_RequestGeneration = 0;
  uint32_t m_LastUpdateFrame = 0;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
