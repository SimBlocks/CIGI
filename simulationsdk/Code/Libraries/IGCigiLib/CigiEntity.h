//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiEntity.h
 * @brief Declares the CCigiEntity class for SimBlocks CIGI IG entity management and animation.
 *
 * Provides the CCigiEntity class for managing entities in the SimBlocks IGCigiLib library, including rendering, collision, alpha, animation, and hierarchical relationships.
 * Inherits from CEntity and integrates with SimBlocks entity, math, and CIGI types for simulation and entity control.
 * Supports animation, component state, attach state, and message sending for simulation interoperability.
 *
 * @see sbio::cigi::ig::CCigiEntity
 * @see sbio::entity::CEntity
 * @see sbio::math::TCigiBodyTransform
 * @see sbio::cigi::SCigiComponentKey
 * @see sbio::cigi::SCigiComponentControlState
 * @see sbio::cigi::SCigiAnimationControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ENTITY_H
#define SIMBLOCKS_CIGI_ENTITY_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "EngineLib/EngineTypeDeclarations.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityDeclarations.h"
#include "EntityLib/EntityType.h"
#include "GlobalHeaders/CommonTypes.h"
#include "CigiEntityAnimation.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Local body-frame velocity state for a CIGI entity.
       *
       * Stores linear velocity in body coordinates and angular velocity as body Euler rates.
       */
      struct SLocalBodyTransformationRate
      {
        sbio::math::BodyCoordinates linearVelocity;///< Linear velocity in body coordinates.
        sbio::math::TBodyEulerVelocity angularVelocity;///< Angular velocity expressed as body Euler rates.
      };

      /**
       * @brief Local body-frame acceleration state for a CIGI entity.
       *
       * Stores linear acceleration in body coordinates and angular acceleration as body Euler rates.
       */
      struct SLocalBodyAccelerationRate
      {
        sbio::math::BodyCoordinates linearAcceleration;///< Linear acceleration in body coordinates.
        sbio::math::TBodyEulerAcceleration angularAcceleration;///< Angular acceleration expressed as body Euler rates.
      };

      /**
       * @brief World-frame velocity state for a CIGI entity.
       *
       * Stores linear velocity in CIGI NED coordinates and angular velocity as body Euler rates.
       */
      struct SCigiWorldTransformationRate
      {
        sbio::cigi::CigiNEDCoordinates linearVelocity;///< Linear velocity in CIGI NED coordinates.
        sbio::math::TBodyEulerVelocity angularVelocity;///< Angular velocity expressed as body Euler rates.
      };

      /**
       * @brief World-frame acceleration state for a CIGI entity.
       *
       * Stores linear acceleration in CIGI NED coordinates and angular acceleration as body Euler rates.
       */
      struct SCigiWorldAccelerationRate
      {
        sbio::cigi::CigiNEDCoordinates linearAcceleration;///< Linear acceleration in CIGI NED coordinates.
        sbio::math::TBodyEulerAcceleration angularAcceleration;///< Angular acceleration expressed as body Euler rates.
      };

      /**
       * @brief Represents one entity whose state is driven by CIGI host traffic.
       *
       * Responsibilities:
       * - Extends the base entity with CIGI-specific component, animation, attach-state, and message-publication behavior.
       * - Tracks the child transform used when the entity is attached to another entity.
       * - Maintains per-component control state and per-animation state keyed by their CIGI identifiers.
       *
       * Ownership:
       * - Inherits entity lifecycle from `sbio::entity::CEntity`.
       * - Owns the animation objects stored in `m_Animations`.
       * - Does not own parent entities referenced through attach state.
       *
       * Invariants:
       * - `m_ShortEntityTypeID` identifies the entity's current short type, when one is available.
       * - `m_AttachState` and `m_ChildTransform` describe how the entity is positioned relative to a parent when attached.
       * - `m_EntityComponents` stores the latest component state for each component key applied to this entity.
       *
       * Failure cases:
       * - Operations that require parent entities, downstream message delivery, or animation instances depend on the surrounding entity and messaging systems.
       *
       * Side effects:
       * - Mutating operations may change inherited entity state and may emit engine update messages from the implementation.
       */
      class CCigiEntity : public sbio::entity::CEntity
      {
      public:
        /**
         * @brief Constructs a CIGI entity wrapper.
         * @param entityID Identifier of the represented entity.
         * @param entityControl Initial entity-control data used to seed entity state.
         */
        CCigiEntity(EntityID entityID, const SEntityControl& entityControl);

        /**
         * @brief Destroys the entity wrapper and its owned animation objects.
         */
        ~CCigiEntity();

        /**
         * @brief Removes the entity from the simulation.
         */
        virtual void Remove() override;

        /**
         * @brief Sets rendering enabled/disabled for the entity.
         * @param bRenderEnabled True to enable rendering, false to disable.
         */
        virtual void SetRenderEnabled(bool bRenderEnabled) override;

        /**
         * @brief Sets collision detection enabled/disabled for the entity.
         * @param bCollisionDetectionEnabled True to enable collision detection, false to disable.
         */
        virtual void SetCollisionDetectionEnabled(bool bCollisionDetectionEnabled) override;

        /**
         * @brief Sets the alpha (transparency) value for the entity.
         * @param fAlpha The alpha value.
         */
        virtual void SetAlpha(float fAlpha) override;

        /**
         * @brief Sets the entity's own alpha and whether to inherit its parent's effective alpha.
         * @param fAlpha Own alpha used when inheritance does not apply; not clamped here.
         * @param inheritAlpha Whether a CIGI parent's alpha replaces this entity's own alpha.
         */
        void SetAlpha(float fAlpha, bool inheritAlpha);

        /** @brief Reports the requested parent-alpha inheritance flag.
         * @return Stored flag, even when no CIGI parent is attached.
         */
        bool GetInheritAlpha() const;

        /**
         * @brief Gets the child transform for the entity.
         * @return The child transform.
         */
        sbio::cigi::TCigiBodyTransform GetChildTransform() const;

        /**
         * @brief Updates the entity's state based on the elapsed time.
         * @param deltaTime The elapsed time in seconds.
         *
         * Integrates motion only when the inherited interpolation flag is enabled, then sends pending transform updates.
         */
        virtual void Update(double deltaTime) override;

        /**
         * @brief Gets the short entity type ID.
         * @return The short entity type ID.
         */
        ShortEntityTypeID GetShortEntityType() const;

        /**
         * @brief Interpolates the entity's state.
         * @param deltaTime The elapsed time in seconds.
         */
        void Interpolate(double deltaTime);

        /**
         * @brief Sets the entity component state.
         * @param key Component key.
         * @param state Component control state.
         */
        void SetEntityComponent(sbio::cigi::SCigiComponentKey key, sbio::cigi::SCigiComponentControlState state);

        /**
         * @brief Sets the entity state.
         * @param EntityState Active state.
         * @param bForce Force state change flag.
         */
        void SetEntityState(sbio::cigi::EActiveState EntityState, bool bForce = false);

        /**
         * @brief Sets the attach state for the entity.
         * @param AttachState Attach state.
         * @param parentID Parent entity ID.
         * @param inheritAlpha Inherit alpha flag.
         * @return True if attach state was set, false otherwise.
         */
        bool SetAttachState(sbio::EAttachState AttachState, EntityID parentID, bool inheritAlpha);

        /**
         * @brief Updates the entity's animation.
         * @param control Animation control parameters.
         */
        void UpdateAnimation(const SCigiAnimationControl& control);

        /**
         * @brief Sets the child transform for the entity.
         * @param childTransform Child transform value.
         */
        void SetChildTransform(sbio::cigi::TCigiBodyTransform childTransform);

        /** @brief Stores the geodetic position and orientation and converts them into the active world coordinate system.
         * @param geodeticCoordinates Position to cache and convert.
         * @param rotation CIGI body Euler rotation to cache and convert.
         *
         * Marks the transform changed; does not change attachment state.
         */
        void SetTopLevelTransform(const sbio::math::SGeodeticCoordinates& geodeticCoordinates, const sbio::cigi::TCigiBodyEulerRotation& rotation);

        /** @brief Stores a world transform and marks it changed.
         * @param worldTransform Transform in the active world coordinate system.
         *
         * Refreshes cached geodetic position and Euler rotation only for top-level entities.
         */
        void SetTopLevelWorldTransform(const sbio::math::TGeocentricTransform& worldTransform);

        /** @brief Selects the coordinate system used for velocity integration without converting stored rates.
         * @param coordinateSystem Velocity coordinate-system selector.
         */
        void SetTransformationRateCoordinateSystem(sbio::EObjectCoordinateSystem coordinateSystem);

        /** @brief Selects acceleration coordinates, also initializing an unknown velocity selector to match.
         * @param coordinateSystem Acceleration coordinate-system selector; stored rates are not converted here.
         */
        void SetAccelerationRateCoordinateSystem(sbio::EObjectCoordinateSystem coordinateSystem);

        /** @brief Replaces stored local/body velocity data without changing the coordinate selector.
         * @param transformationRate Linear and angular velocities to copy.
         */
        void SetLocalBodyTransformationRate(const SLocalBodyTransformationRate& transformationRate);

        /** @brief Replaces stored CIGI world/NED velocity data without changing the coordinate selector.
         * @param transformationRate Linear and angular velocities to copy.
         */
        void SetCigiWorldTransformationRate(const SCigiWorldTransformationRate& transformationRate);

        /** @brief Replaces stored local/body acceleration data without changing the coordinate selector.
         * @param accelerationRate Linear and angular accelerations to copy.
         */
        void SetLocalBodyAccelerationRate(const SLocalBodyAccelerationRate& accelerationRate);

        /** @brief Replaces stored CIGI world/NED acceleration data without changing the coordinate selector.
         * @param accelerationRate Linear and angular accelerations to copy.
         */
        void SetCigiWorldAccelerationRate(const SCigiWorldAccelerationRate& accelerationRate);

        /**
         * @brief Sends an update message for the entity.
         */
        void SendUpdateMessage();

        /**
         * @brief Attaches this entity to a parent entity.
         * @param pParent Pointer to the parent entity.
         */
        virtual void AttachToEntity(CEntity* pParent) override;

        /**
         * @brief Makes this entity a top-level entity without a parent.
         */
        virtual void Unattach() override;

      private:
        /** @brief Recomputes effective alpha, publishing changes and updating animations and inheriting children. */
        void UpdateEffectiveAlpha();

        /** @brief Refreshes all owned animation channels using this entity's effective alpha. */
        void UpdateAnimationAlpha();

        /** @brief Converts stored acceleration into the coordinate frame selected for velocity integration.
         * @return Copy of linear and angular acceleration, rotated when local/non-local selectors differ.
         */
        SLocalBodyAccelerationRate GetAccelerationInVelocityCoordinates() const;

        /**
         * @brief Sends a top-level entity transform message.
         */
        void SendTopLevelEntityTransformMessage() const;

        /**
         * @brief Sends a child entity transform message.
         */
        void SendChildEntityTransformMessage() const;

        /**
         * @brief Updates the animation direction.
         * @param control Animation control parameters.
         * @param bForce Force update flag.
         */
        void UpdateAnimationDirection(const SCigiAnimationControl& control, bool bForce);

        /**
         * @brief Updates the animation speed.
         * @param control Animation control parameters.
         * @param bForce Force update flag.
         */
        void UpdateAnimationSpeed(const SCigiAnimationControl& control, bool bForce);

        /**
         * @brief Updates the animation state.
         * @param control Animation control parameters.
         * @param bForce Force update flag.
         */
        void UpdateAnimationState(const SCigiAnimationControl& control, bool bForce);

        /**
         * @brief Updates the animation loop mode.
         * @param control Animation control parameters.
         * @param bForce Force update flag.
         */
        void UpdateAnimationLoopMode(const SCigiAnimationControl& control, bool bForce);

      private:
        bool m_bAlphaInitialized = false;
        float m_OwnAlpha = 0;///< Alpha to use when parent inheritance does not apply.

        ShortEntityTypeID m_ShortEntityTypeID = UnknownShortEntityTypeID;///< short entity-type identifier.
        sbio::cigi::EActiveState m_EntityState = sbio::cigi::EActiveState::UNKNOWN;///< Cached CIGI active state.
        sbio::EAttachState m_AttachState = sbio::EAttachState::UNKNOWN;///< Cached attach state relative to a parent entity.
        bool m_bEnableCollision = false;///< Cached collision enable state.

        typedef std::unordered_map<sbio::AnimationID, std::unique_ptr<sbio::cigi::ig::CCigiEntityAnimation>, StrongTypeHash<sbio::AnimationID>> TAnimations;
        TAnimations m_Animations;///< Owned animation state keyed by animation identifier.

        sbio::cigi::TCigiBodyTransform m_ChildTransform;///< Cached child transform relative to the parent entity.
        sbio::EObjectCoordinateSystem m_TransformationRateCoordinateSystem = sbio::EObjectCoordinateSystem::UNKNOWN;///< Coordinate system for active transformation rates.
        sbio::EObjectCoordinateSystem m_AccelerationRateCoordinateSystem = sbio::EObjectCoordinateSystem::UNKNOWN;///< Coordinate system for active acceleration rates.
        SLocalBodyTransformationRate m_LocalBodyTransformationRate;///< Internal local/body transformation rate.
        SLocalBodyAccelerationRate m_LocalBodyAccelerationRate;///< Internal local/body acceleration rate.
        SCigiWorldTransformationRate m_CigiWorldTransformationRate;///< CIGI world/NED transformation rate.
        SCigiWorldAccelerationRate m_CigiWorldAccelerationRate;///< CIGI world/NED acceleration rate.
        std::map<sbio::cigi::SCigiComponentKey, sbio::cigi::SCigiComponentControlState> m_EntityComponents;///< Latest component control state keyed by component identity.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
