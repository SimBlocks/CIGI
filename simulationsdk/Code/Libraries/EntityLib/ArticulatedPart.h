//Copyright SimBlocks LLC 2016-2026
/**
 * @file ArticulatedPart.h
 * @brief Declares the CArticulatedPart class for managing articulated parts of an entity.
 *
 * The CArticulatedPart class represents an articulated part of a simulation entity. It stores the part's
 * local body-space position and orientation together with linear and angular rates used by `Update()` to
 * integrate motion over time.
 *
 * @see sbio::EntityID
 * @see sbio::ArticulatedPartID
 * @see sbio::math::TBodyEulerRotation
 * @see sbio::math::TBodyTransform
 * @see sbio::entity::STransformationRate
 * @see SAccelerationRate
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_ARTICULATED_PART_H
#define SIMBLOCKS_ENTITY_ARTICULATED_PART_H

#include "EntityLib/EntityDeclarations.h"
#include "EntityTypes.h"
#include "GlobalHeaders/CommonTypes.h"
#include "MathLib/MathTypes.h"
#include <list>

namespace sbio
{
  namespace entity
  {
    /**
     * @brief Represents one articulated part belonging to an entity.
     *
     * Instances are identified by the `(EntityID, ArticulatedPartID)` pair supplied at construction time.
     * The object maintains a local body-space transform and optional linear and angular rates. `Update()`
     * applies simple forward integration using the stored rates and accelerations.
     *
     * Invariants:
     * - `GetEntityID()` and `GetArticulatedPartID()` always report the identifiers supplied to the constructor.
     * - `GetLocalTransformation()` is derived from the current stored position and body Euler rotation.
     *
     */
    class CArticulatedPart
    {
    public:
      /**
       * @brief Constructs an articulated part for a specific entity and articulated-part identifier.
       *
       * @param entityID Identifier of the entity that owns this articulated part.
       * @param articulatedPartID Identifier of this articulated part within the owning entity.
       */
      CArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID);

      /**
       * @brief Virtual destructor.
       */
      virtual ~CArticulatedPart();

      /**
       * @brief Marks the articulated part as disabled.
       *
       * Notes:
       * - The base implementation does not suppress `Update()` and does not otherwise alter motion state.
       */
      virtual void Disable();

      /**
       * @brief Marks the articulated part as enabled.
       */
      virtual void Enable();

      /**
       * @brief Gets the current body Euler rotation.
       * @return A copy of the stored body Euler rotation.
       */
      sbio::math::TBodyEulerRotation GetBodyEulerRotation() const;

      /**
       * @brief Gets the identifier of the entity that owns this articulated part.
       *
       * @return The owning entity identifier supplied at construction.
       */
      sbio::EntityID GetEntityID() const;

      /**
       * @brief Gets the articulated-part identifier.
       *
       * @return The articulated-part identifier supplied at construction.
       */
      sbio::ArticulatedPartID GetArticulatedPartID() const;

      /**
       * @brief Builds the current local body-space transform for the articulated part.
       *
       * @return A `TBodyTransform` whose position is the stored local position and whose rotation is derived
       *         from the stored body Euler rotation.
       */
      sbio::math::TBodyTransform GetLocalTransformation() const;

      /**
       * @brief Replaces the stored acceleration state.
       * @param acceleration Linear and angular acceleration to store for subsequent `Update()` calls.
       */
      void SetAcceleration(const SAccelerationRate<sbio::math::BodyCoordinates>& acceleration);

      /**
       * @brief Replaces the stored body Euler rotation.
       * @param rotation New body Euler rotation.
       */
      virtual void SetBodyEulerRotation(const sbio::math::TBodyEulerRotation& rotation);

      /**
       * @brief Replaces the stored local body-space position.
       * @param position New local position.
       */
      virtual void SetPosition(const sbio::math::BodyCoordinates& position);

      /**
       * @brief Replaces the stored transformation-rate state.
       * @param transformationRate Linear and angular velocity to store for subsequent `Update()` calls.
       */
      void SetTransformationRate(const sbio::entity::STransformationRate<sbio::math::BodyCoordinates>& transformationRate);

      /**
       * @brief Advances the articulated part state by integrating stored rates over elapsed time.
       *
       * @param fDeltaTime Elapsed simulation time, in seconds.
       *
       * Notes:
       * - Returns immediately when `fDeltaTime` converts to `0.0f`.
       * - The base implementation does not consult the enabled flag.
       */
      virtual void Update(double fDeltaTime);

    protected:
      sbio::EntityID m_EntityID = UnknownEntityID;///< Parent entity ID
      sbio::ArticulatedPartID m_ArticulatedPartID = UnknownArticulatedPartID;///< Articulated part ID
      sbio::math::BodyCoordinates m_Position;///< Current position in body coordinates
      sbio::math::TBodyEulerRotation m_Rotation;///< Current body Euler rotation
      sbio::entity::STransformationRate<sbio::math::BodyCoordinates> m_TransformationRate;///< Transformation rate
      SAccelerationRate<sbio::math::BodyCoordinates> m_AccelerationRate;///< Acceleration rate
      bool m_bEnabled = true;///< True if the part is enabled (visible/active)
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
