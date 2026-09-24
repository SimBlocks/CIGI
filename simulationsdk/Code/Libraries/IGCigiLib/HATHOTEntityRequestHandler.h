//Copyright SimBlocks LLC 2016-2026
/**
 * @file HATHOTEntityRequestHandler.h
 * @brief Declares the CHATHOTEntityRequestHandler class for entity-relative HAT/HOT request handling.
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_HAT_HOT_ENTITY_REQUEST_HANDLER_H
#define SIMBLOCKS_CIGI_HAT_HOT_ENTITY_REQUEST_HANDLER_H

#include "HATHOTRequestHandler.h"

/**
 * @brief Handles entity-relative HAT/HOT requests for SimBlocks CIGI IG integration.
 */
class CHATHOTEntityRequestHandler : public CHATHOTRequestHandler
{
public:
  /** @brief Stores an entity-relative terrain-height request.
   * @param request Payload copied into this handler; no entity pointer is retained.
   */
  CHATHOTEntityRequestHandler(const sbio::cigi::SHATHOTEntityRequest& request);

  /** @brief Identifies entity-relative positioning.
   * @return `ETopLevelCoordinateSystem::ENTITY`.
   */
  virtual sbio::ETopLevelCoordinateSystem GetCoordinateSystem() const override;
  /** @brief Gets the entity used to resolve the request position.
   * @return Stored entity identifier.
   */
  virtual sbio::EntityID GetEntityID() const override;
  /** @brief Supplies the unused geodetic-position interface.
   * @return Default-constructed coordinates, not the resolved entity position.
   */
  virtual sbio::math::SGeodeticCoordinates GetGeodeticCoordinates() const override;
  /** @brief Gets the stored relative offset.
   * @return Copy of the request's CIGI body-coordinate offset.
   */
  virtual sbio::math::Vec3 GetOffset() const override;
  /** @brief Resolves the entity-relative point to geodetic coordinates and submits the query.
   * @return `false` if the entity manager/entity is absent; otherwise the result of `SendRequest()`.
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
  sbio::cigi::SHATHOTEntityRequest m_Request;
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
