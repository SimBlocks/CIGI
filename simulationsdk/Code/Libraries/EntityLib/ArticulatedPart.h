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
     * The object maintains a local body-space transform and linear and angular motion state. `Update()`
     * advances motion only while enabled: first velocities using acceleration, then position and Euler
     * angles using those updated velocities. The entity ID identifies the associated entity.
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
       * @brief Destroys the articulated part; does not remove or modify its owning entity.
       */
      virtual ~CArticulatedPart();

      /**
       * @brief Disables motion integration for the articulated part.
       *
       * Subsequent base `Update()` calls leave position, rotation, and velocities unchanged. Stored motion
       * state is preserved for re-enabling; explicit setters can still change it while disabled.
       */
      virtual void Disable();

      /**
       * @brief Enables motion integration from the stored state.
       *
       * Subsequent base `Update()` calls resume integration without catching up on time skipped while
       * disabled. The base implementation does not change motion state or rendering in this call.
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
       * - Returns without changing motion state when disabled or when `fDeltaTime` converts to `0.0f`.
       * - Time passed while disabled is not accumulated for later integration.
       * - Advances velocities before position and rotation, using the supplied double-precision time step.
       * - Applies position and rotation changes through `SetPosition()` and `SetBodyEulerRotation()`.
       * - The base implementation does not consult coordinate-system selectors.
       */
      virtual void Update(double fDeltaTime);

    protected:
      sbio::EntityID m_EntityID = UnknownEntityID;///< Parent entity ID
      sbio::ArticulatedPartID m_ArticulatedPartID = UnknownArticulatedPartID;///< Articulated part ID
      sbio::math::BodyCoordinates m_Position;///< Current position in body coordinates
      sbio::math::TBodyEulerRotation m_Rotation;///< Current body Euler rotation
      sbio::entity::STransformationRate<sbio::math::BodyCoordinates> m_TransformationRate;///< Transformation rate
      SAccelerationRate<sbio::math::BodyCoordinates> m_AccelerationRate;///< Acceleration rate
      bool m_bEnabled = true;///< Enables motion integration in the base `Update()`; false freezes integration.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
