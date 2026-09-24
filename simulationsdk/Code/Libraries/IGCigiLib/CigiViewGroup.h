//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiViewGroup.h
 * @brief Declares the CCigiViewGroup class for SimBlocks CIGI IG view group management and entity attachment.
 *
 * Provides the CCigiViewGroup class for representing and managing CIGI view groups attached to entities in the SimBlocks IGCigiLib library.
 * Inherits from sbio::view::CViewGroup and integrates with SimBlocks view, math, and CIGI types for group-based view management.
 * Supports entity attachment, transformation, rotation, offset, view ID management, and group update for simulation views.
 *
 * @see sbio::cigi::ig::CCigiViewGroup
 * @see sbio::view::CViewGroup
 * @see sbio::math::TCigiBodyEulerRotation
 * @see sbio::math::TGeocentricTransform
 * @see sbio::math::TCigiBodyTransform
 * @see sbio::math::CigiBodyCoordinates
 * @see sbio::ViewGroupID
 * @see sbio::ViewID
 * @see sbio::EntityID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_VIEW_GROUP_H
#define SIMBLOCKS_CIGI_VIEW_GROUP_H

#include "EngineLib/EngineTypeDeclarations.h"
#include "ViewLib/View.h"
#include "MathLib/MathTypes.h"
#include "CigiLib/CigiTypes.h"
#include "CigiEvent.h"
#include "ViewLib/ViewGroup.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Represents one CIGI-controlled view group and its shared attachment transform.
       */
      class CCigiViewGroup : public sbio::view::CViewGroup, public IIGCIGIEventListener
      {
      public:
        /**
         * @brief Constructs a CIGI view-group wrapper.
         * @param viewGroupID Identifier of the represented view group.
         */
        CCigiViewGroup(sbio::ViewGroupID viewGroupID);
        /**
         * @brief Destroys the view-group wrapper.
         */
        ~CCigiViewGroup();

        /** @brief Ignores database-loaded notifications. */
        virtual void OnDatabaseLoadedEvent() override {};
        /** @brief Ignores database-load failures. */
        virtual void OnDatabaseLoadingFailedEvent() override {};
        /**
         * @brief Clears the attachment if this group is attached to the removed entity.
         * @param entityID Identifier of the removed entity.
         */
        virtual void OnEntityRemoved(sbio::EntityID entityID) override;
        /**
         * @brief Ignores image-generator error notifications.
         * @param args Unused error details.
         */
        virtual void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) override {};

        /** @brief Clears the group's attachment at an IG reset boundary. */
        virtual void Reset() override;

        /**
         * @brief Adds a view to the group.
         * @param viewID Identifier of the view to associate with this group.
         */
        virtual void AddViewID(sbio::ViewID viewID) override;
        /**
         * @brief Removes a view from the group.
         * @param viewID Identifier of the view to remove.
         */
        virtual void RemoveViewID(sbio::ViewID viewID) override;
        /**
         * @brief Sets the entity to which the group is attached.
         * @param entityID Identifier of the attachment target.
         */
        void SetAttachedEntityID(sbio::EntityID entityID);
        /**
         * @brief Returns the attached entity identifier.
         * @return Attached entity identifier.
         */
        sbio::EntityID GetAttachedEntityID();
        /**
         * @brief Returns the current world transform of the group.
         * @return Geocentric world transform derived from the attachment target and cached child transform.
         */
        sbio::math::TGeocentricTransform GetWorldTransform() const;
        /**
         * @brief Returns the child transform relative to the attachment target.
         * @return Body-frame transform built from the cached offset and rotation.
         */
        sbio::cigi::TCigiBodyTransform GetChildTransform() const;
        /**
         * @brief Replaces the shared body-frame offset.
         * @param vOffset Body-frame offset to store.
         */
        void SetOffset(const sbio::cigi::CigiBodyCoordinates& vOffset);

        /**
         * @brief Returns the cached body-frame offset.
         * @return Copy of the cached offset.
         */
        sbio::cigi::CigiBodyCoordinates GetOffset()
        {
          return m_vOffset;
        }

        /**
         * @brief Sets the cached yaw angle.
         * @param fYaw Yaw angle in degrees.
         */
        void SetYaw(sbio::math::Degrees fYaw);
        /**
         * @brief Sets the cached pitch angle.
         * @param fPitch Pitch angle in degrees.
         */
        void SetPitch(sbio::math::Degrees90 fPitch);
        /**
         * @brief Sets the cached roll angle.
         * @param fRoll Roll angle in degrees.
         */
        void SetRoll(sbio::math::Degrees180 fRoll);
        /**
         * @brief Updates derived state for the group.
         */
        virtual void Update() override;
        /**
         * @brief Recomputes the derived group transform.
         */
        void UpdateTransformation();

        /**
         * @brief Returns the cached body-frame rotation.
         * @return Reference to the cached rotation value.
         */
        const sbio::cigi::TCigiBodyEulerRotation& GetRotation() const;

      private:
        sbio::EntityID m_AttachedEntityID = sbio::UnknownEntityID;
        bool m_bAttachmentInitialized = false;
        bool m_bTransformationDirty = false;///< Indicates whether the derived group transform needs recomputation.
        sbio::cigi::CigiBodyCoordinates m_vOffset;///< Cached body-frame offset relative to the attachment target.
        sbio::cigi::TCigiBodyEulerRotation m_Rotation;///< Cached body-frame rotation relative to the attachment target.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
