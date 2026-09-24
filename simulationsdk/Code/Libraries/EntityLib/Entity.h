//Copyright SimBlocks LLC 2016-2026
/**
 * @file Entity.h
 * @brief Declares the CEntity class for managing entities in a simulation.
 *
 * The CEntity class represents one simulation entity. It stores either a top-level world transform or,
 * when attached to another entity, a local transform relative to its parent. The class also stores motion
 * rates, clamp state, and cached geodetic/body-orientation data used by derived implementations.
 *
 * @see sbio::EntityID
 * @see sbio::math::TGeocentricTransform
 * @see sbio::math::TBodyTransform
 * @see sbio::math::SGeodeticCoordinates
 * @see STransformationRate
 * @see SAccelerationRate
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_H
#define SIMBLOCKS_ENTITY_H

#include "GlobalHeaders/CommonTypes.h"
#include "EntityLib/EntityTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include <list>
#include <unordered_set>
#include <map>

namespace sbio
{
  namespace entity
  {
    /**
     * @brief Base class for simulation entities with optional parent-child hierarchy.
     *
     * A `CEntity` is either top-level or attached to another `CEntity`. Top-level entities expose a stored
     * geocentric world transform. Child entities expose a world transform composed from their parent's world
     * transform and their stored local body-space transform.
     * Stored rates, clamp mode, and interpolation state are available to derived implementations;
     * the base `Update()` does not apply them.
     *
     * Invariants:
     * - `GetEntityID()` always reports the identifier supplied to the constructor.
     * - `IsTopLevel()` is equivalent to `!IsChild()`.
     * - When attached, the parent relationship is non-owning and tracked by both `m_pParent` and `m_ParentID`.
     *
     * Extensibility:
     * - `Update()`, `Remove()`, `SetAlpha()`, `SetRenderEnabled()`, and
     *   `SetCollisionDetectionEnabled()` are virtual customization points for derived classes.
     */
    class CEntity
    {
    public:
      /**
       * @brief Constructs an entity with a fixed entity identifier.
       *
       * @param entityID Identifier for the entity; uniqueness is not checked by this constructor.
       */
      CEntity(sbio::EntityID entityID);

      /**
       * @brief Destroys the entity without detaching it or its children and without calling `Remove()`.
       *
       * Callers must detach surviving children before destroying their parent.
       */
      virtual ~CEntity();

      /**
       * @brief Attaches this entity to a parent entity.
       *
       * @param pParent Non-owning parent pointer that must remain valid while attached.
       * Passing `nullptr` detaches the entity.
       *
       * Notes:
       * - Passing the current parent is a no-op.
       * - Passing `nullptr` is equivalent to calling `Unattach()`.
       * - Self-attachment, attachment to a descendant, or a cyclic parent chain is rejected without changes.
       * - Reparenting updates both parents' child-ID sets but does not modify stored transforms;
       *   the resulting world transform may change.
       */
      virtual void AttachToEntity(CEntity* pParent);

      /**
       * @brief Detaches this entity from its current parent.
       *
       * When attached, preserves the composed world transform, refreshes the cached geodetic position
       * and rotation, and removes this entity's ID from the parent's child set. Clears the parent
       * pointer and ID, sets the transformation-rate coordinate selector to `WORLD` without converting
       * rate values, and marks the transform changed. The stored local transform is unchanged.
       * Calling this on a top-level entity is allowed.
       */
      virtual void Unattach();

      /**
       * @brief Gets the entity's world transform.
       *
       * @return For top-level entities, the stored geocentric world transform. For child entities, a transform
       *         composed recursively from the parent's world transform and this entity's local child transform.
       */
      sbio::math::TGeocentricTransform GetWorldTransform() const;

      /**
       * @brief Gets the entity's local transform relative to its parent.
       *
       * @return The stored local body-space transform.
       *
       * Notes:
       * - The value is returned even when the entity is currently top-level.
       */
      sbio::math::TBodyTransform GetChildTransform() const;

      /**
       * @brief Reports whether this entity is attached to a parent entity.
       *
       * @return `true` when a parent pointer is present; otherwise `false`.
       */
      bool IsChild() const;

      /**
       * @brief Reports whether this entity is top-level.
       *
       * @return `true` when no parent pointer is present; otherwise `false`.
       */
      bool IsTopLevel() const;

      /**
       * @brief Updates the entity for the given simulation time step.
       *
       * @param fDeltaTime Elapsed simulation time, in seconds.
       *
       * Notes:
       * - The base implementation does nothing, including no motion integration or child updates.
       * - Derived classes can override this method to apply interpolation or other time-based behavior.
       */
      virtual void Update(double fDeltaTime);

      /**
       * @brief Stores the interpolation enable state for this entity.
       * @param bInterpolationEnabled `true` to enable interpolation in implementations that use this flag;
       *                              `false` to disable it. The base `Update()` does not use the flag.
       */
      void SetInterpolationEnabled(bool bInterpolationEnabled);

      /**
       * @brief Reports the current interpolation enable state.
       * @return Stored per-entity setting, temporarily masked by the global switch during a manager-driven update.
       */
      bool GetInterpolationEnabled() const;

      /**
       * @brief Hook for derived classes to remove an external entity representation.
       *
       * The base implementation does nothing; it does not detach or destroy the entity.
       */
      virtual void Remove();

      /**
       * @brief Updates the entity alpha in a derived representation.
       * @param fAlpha Alpha value requested by the caller.
       *
       * The base implementation does nothing and does not store or validate the value.
       */
      virtual void SetAlpha(float fAlpha);

      /**
       * @brief Enables or disables rendering in a derived representation.
       * @param bRenderEnabled `true` to request rendering; `false` to suppress it.
       *
       * The base implementation does nothing.
       */
      virtual void SetRenderEnabled(bool bRenderEnabled);

      /**
       * @brief Enables or disables collision detection in a derived representation.
       * @param bCollisionDetectionEnabled `true` to enable collision detection; `false` to disable it.
       *
       * The base implementation does nothing.
       */
      virtual void SetCollisionDetectionEnabled(bool bCollisionDetectionEnabled);

      /**
       * @brief Replaces the stored geocentric world transform.
       *
       * @param geocentricTransform New geocentric transform to store.
       *
       * Notes:
       * - For top-level entities, also refreshes the cached geodetic position and body Euler rotation.
       * - Child entities still report a composed world transform from their parent and local transform.
       * - Marks the transform changed.
       */
      void SetWorldTransform(sbio::math::TGeocentricTransform geocentricTransform);

      /**
       * @brief Replaces the stored world transform from geodetic input.
       * @param geodeticTransform New geodetic transform to convert and store.
       *
       * Stores the geodetic position and converted world transform, and marks the transform changed.
       * Also refreshes the cached body Euler rotation for top-level entities. For child entities,
       * the local transform and the world transform returned by `GetWorldTransform()` are unaffected.
       */
      void SetWorldTransform(sbio::math::TGeodeticTransform geodeticTransform);

      /**
       * @brief Gets the entity's geodetic position.
       *
       * @return For top-level entities, the stored cached geodetic position. For child entities, the geodetic
       *         position converted from the current composed world transform.
       */
      sbio::math::SGeodeticCoordinates GetGeodeticCoordinates() const;

      /**
       * @brief Replaces the stored local transform relative to the parent.
       * @param childTransform New local body-space transform.
       *
       * Marks the transform changed; does not update the stored world transform or cached Euler rotation.
       */
      void SetChildTransform(const sbio::math::TBodyTransform& childTransform);

      /**
       * @brief Gets the stored acceleration state.
       * @return Const reference to the current acceleration-rate structure.
       */
      const SAccelerationRate<sbio::math::BodyCoordinates>& GetAccelerationRate() const;

      /**
       * @brief Gets the entity identifier.
       *
       * @return The identifier supplied at construction time.
       */
      sbio::EntityID GetEntityID() const;

      /**
       * @brief Gets the current parent entity identifier.
       *
       * @return The current parent entity identifier, or `UnknownEntityID` when the entity is top-level.
       */
      sbio::EntityID GetParentID() const;

      /**
       * @brief Gets the stored body Euler rotation.
       * @return Const reference to the cached body Euler rotation, not a rotation recomputed from
       *         the parent or the local child transform. The reference is owned by this entity.
       */
      const sbio::math::TBodyEulerRotation& GetRotation() const;

      /**
       * @brief Gets the stored transformation-rate state.
       * @return Const reference to the current transformation-rate structure.
       */
      const STransformationRate<sbio::math::BodyCoordinates>& GetTransformationRate() const;

      /**
       * @brief Replaces the stored acceleration state.
       * @param accelerationRate New acceleration-rate structure.
       */
      void SetAccelerationRate(const SAccelerationRate<sbio::math::BodyCoordinates>& accelerationRate);

      /**
       * @brief Replaces the clamp mode.
       * @param eClamp New clamp mode.
       *
       * Marks the transform changed. The base class does not perform terrain clamping.
       */
      void SetClamp(EClamp eClamp);

      /**
       * @brief Replaces the stored geodetic position.
       *
       * @param geodeticCoordinates New geodetic position.
       *
       * Notes:
       * - For top-level entities, rebuilds the world transform using the stored rotation and marks it changed.
       * - For child entities, the local transform is unchanged.
       */
      void SetGeodeticCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoordinates);

      /**
       * @brief Replaces the stored body Euler rotation.
       * @param rotation New body Euler rotation.
       *
       * For top-level entities, rebuilds the world transform using the cached geodetic position and
       * marks it changed. For child entities, changes only the cached rotation, not the local transform.
       */
      void SetRotation(const sbio::math::TBodyEulerRotation& rotation);

      /**
       * @brief Replaces the stored transformation-rate state.
       * @param transformationRate New transformation-rate structure.
       */
      void SetTransformationRate(const STransformationRate<sbio::math::BodyCoordinates>& transformationRate);

    protected:
      sbio::EntityID m_EntityID = sbio::UnknownEntityID;///< Unique entity ID
      SEntityType m_EntityType;///< Entity type information
      bool m_bActive = false;///< True if the entity is active
      bool m_bInheritAlpha = false;///< True if the entity inherits alpha from parent
      float m_fAlpha = 0;///< Alpha (transparency) value
      sbio::EntityID m_ParentID = sbio::UnknownEntityID;///< Parent entity ID
      bool m_bInterpolationEnabled = true;///< True if interpolation is enabled
      sbio::math::TBodyEulerRotation m_Rotation;///< Current body Euler rotation
      sbio::math::SGeodeticCoordinates m_GeodeticPosition;///< Geodetic position
      EClamp m_eClamp = {EClamp::NONE};///< Clamp mode
      STransformationRate<sbio::math::BodyCoordinates> m_TransformationRate;///< Transformation rate
      SAccelerationRate<sbio::math::BodyCoordinates> m_AccelerationRate;///< Acceleration rate

      typedef std::unordered_set<sbio::EntityID, StrongTypeHash<sbio::EntityID>> TChildrenIDs;///< Set of child entity IDs
      TChildrenIDs m_Children;///< Child entity IDs
      CEntity* m_pParent = nullptr;///< Non-owning parent pointer; must remain valid while attached.
      sbio::math::TGeocentricTransform m_WorldTransform;///< World (geocentric) transform
      sbio::math::TBodyTransform m_LocalTransform;///< Local (body) transform

      bool m_bTransformChanged = false;///< True if the transform has changed
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
