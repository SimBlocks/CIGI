//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTGlobalRequestHandler.h
 * @brief Declares the CHATHOTGlobalRequestHandler class for geodetic HAT/HOT request handling.
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HAT_HOT_GLOBAL_REQUEST_HANDLER_H
#define SIMBLOCKS_CIGI_HAT_HOT_GLOBAL_REQUEST_HANDLER_H

#include "HATHOTRequestHandler.h"

/**
 * @brief Handles geodetic HAT/HOT requests for SimBlocks CIGI IG integration.
 */
class CHATHOTGlobalRequestHandler : public CHATHOTRequestHandler
{
public:
  /** @brief Stores a geodetic terrain-height request.
   * @param request Payload copied into this handler.
   */
  CHATHOTGlobalRequestHandler(const sbio::cigi::SHATHOTGlobalRequest& request);

  /** @brief Identifies geodetic positioning.
   * @return `ETopLevelCoordinateSystem::GEODETIC`.
   */
  virtual sbio::ETopLevelCoordinateSystem GetCoordinateSystem() const override;
  /** @brief Supplies the unused entity-reference interface.
   * @return `UnknownEntityID`.
   */
  virtual sbio::EntityID GetEntityID() const override;
  /** @brief Gets the query position.
   * @return Copy of the stored geodetic coordinates.
   */
  virtual sbio::math::SGeodeticCoordinates GetGeodeticCoordinates() const override;
  /** @brief Supplies the unused relative-offset interface.
   * @return Default-constructed vector.
   */
  virtual sbio::math::Vec3 GetOffset() const override;
  /** @brief Submits the query at its stored geodetic position.
   * @return Result of `SendRequest()`; submission is not a terrain-hit acknowledgment.
   */
  virtual bool Handle() override;

protected:
  /** @brief Gets the stored request metadata.
   * @return Const reference owned by this handler.
   */
  virtual const sbio::cigi::SBaseHATHOTRequest& GetRequest() const override;
  /** @brief Gets mutable stored request metadata.
   * @return Reference owned by this handler.
   */
  virtual sbio::cigi::SBaseHATHOTRequest& GetRequestRef() override;

private:
  sbio::cigi::SHATHOTGlobalRequest m_Request;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
