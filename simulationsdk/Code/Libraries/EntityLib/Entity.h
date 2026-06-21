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
       * @param entityID Unique identifier for the entity.
       */
      CEntity(sbio::EntityID entityID);

      /**
       * @brief Virtual destructor.
       */
      virtual ~CEntity();

      /**
       * @brief Attaches this entity to a parent entity.
       *
       * @param pParent Non-owning pointer to the parent entity. Passing `nullptr` detaches the entity.
       *
       * Notes:
       * - Passing the current parent is a no-op.
       * - Passing `nullptr` is equivalent to calling `Unattach()`.
       */
      virtual void AttachToEntity(CEntity* pParent);

      /**
       * @brief Detaches this entity from its current parent.
       *
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
       * - Derived classes can override this method to apply interpolation or other time-based behavior.
       */
      virtual void Update(double fDeltaTime);

      /**
       * @brief Removes the entity from any external representation owned by a derived class.
       */
      virtual void Remove() {};

      /**
       * @brief Updates the entity alpha in a derived representation.
       * @param fAlpha Alpha value requested by the caller.
       */
      virtual void SetAlpha(float fAlpha) {};

      /**
       * @brief Enables or disables rendering in a derived representation.
       * @param bRenderEnabled `true` to request rendering; `false` to suppress it.
       */
      virtual void SetRenderEnabled(bool bRenderEnabled) {};

      /**
       * @brief Enables or disables collision detection in a derived representation.
       * @param bCollisionDetectionEnabled `true` to enable collision detection; `false` to disable it.
       */
      virtual void SetCollisionDetectionEnabled(bool bCollisionDetectionEnabled) {};

      /**
       * @brief Replaces the stored geocentric world transform.
       *
       * @param geocentricTransform New geocentric transform to store.
       *
       * Notes:
       * - Child entities still report a composed world transform from their parent and local transform.
       */
      void SetWorldTransform(sbio::math::TGeocentricTransform geocentricTransform);

      /**
       * @brief Replaces the stored world transform from geodetic input.
       * @param geodeticTransform New geodetic transform to convert and store.
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
       * @return Const reference to the current body Euler rotation.
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
       */
      void SetClamp(EClamp eClamp);

      /**
       * @brief Replaces the stored geodetic position.
       *
       * @param geodeticCoordinates New geodetic position.
       *
       * Notes:
       * - For child entities, the local transform is unchanged.
       */
      void SetGeodeticCoordinates(const sbio::math::SGeodeticCoordinates& geodeticCoordinates);

      /**
       * @brief Replaces the stored body Euler rotation.
       * @param rotation New body Euler rotation.
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
      CEntity* m_pParent = nullptr;///< Pointer to parent entity
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
