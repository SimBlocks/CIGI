//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiView.h
 * @brief Declares the CCigiView class for SimBlocks CIGI IG view management and entity attachment.
 *
 * Provides the CCigiView class for representing and managing CIGI views attached to entities in the SimBlocks CIGI IG library.
 * Inherits from sbio::view::CView and implements sbio::cigi::ig::IIGCIGIEventListener for event-driven view management.
 * Supports entity attachment, transformation, rotation, offset, mirror mode, view group management, and event handling for simulation views.
 *
 * @see sbio::cigi::ig::CCigiView
 * @see sbio::view::CView
 * @see sbio::cigi::ig::IIGCIGIEventListener
 * @see sbio::math::TCigiBodyEulerRotation
 * @see sbio::math::TGeocentricTransform
 * @see sbio::math::TCigiBodyTransform
 * @see sbio::math::CigiBodyCoordinates
 * @see sbio::ViewID
 * @see sbio::ViewGroupID
 * @see sbio::EntityID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_VIEW_H
#define SIMBLOCKS_CIGI_VIEW_H

#include "EngineLib/EngineTypeDeclarations.h"
#include "ViewLib/View.h"
#include "MathLib/MathTypes.h"
#include "CigiLib/CigiTypes.h"
#include "CigiEvent.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Represents one CIGI-controlled view and its optional entity attachment.
       */
      class CCigiView : public sbio::view::CView, sbio::cigi::ig::IIGCIGIEventListener
      {
      public:
        /**
         * @brief Constructs a CIGI view wrapper.
         * @param viewID Identifier of the view represented by this instance.
         */
        CCigiView(const sbio::ViewID& viewID);
        /**
         * @brief Destroys the view wrapper.
         */
        ~CCigiView();

        // IIGCIGIEventListener overrides
        /**
         * @brief Handles database-loaded notifications.
         */
        virtual void OnDatabaseLoadedEvent() override {};
        /**
         * @brief Handles database-load-failed notifications.
         */
        virtual void OnDatabaseLoadingFailedEvent() override {};
        /**
         * @brief Clears stale attachment state when the attached entity is removed.
         * @param entityID Identifier of the removed entity.
         */
        virtual void OnEntityRemoved(sbio::EntityID entityID) override;

        /**
         * @brief Handles image-generator error notifications.
         * @param args Error data.
         */
        virtual void OnImageGeneratorErrorEvent(const SImageGeneratorErrorEventArgs args) override {};

        /**
         * @brief Returns the identifier of the currently attached entity.
         * @return Attached entity identifier, or `UnknownEntityID` when unattached.
         */
        EntityID GetAttachedEntityID() const;
        /**
         * @brief Returns the current world transform for the view.
         * @return Geocentric world transform derived from the attached object and cached child transform.
         */
        sbio::math::TGeocentricTransform GetWorldTransform() const;
        /**
         * @brief Returns the child transform relative to the attachment target.
         * @return Body-frame transform built from the cached offset and rotation.
         */
        sbio::cigi::TCigiBodyTransform GetChildTransform() const;
        /**
         * @brief Resets view-specific attachment and transform state.
         */
        virtual void Reset();
        /**
         * @brief Sets the attached entity identifier.
         * @param entityID Entity to which this view should be attached.
         */
        void SetAttachedEntityID(EntityID entityID);
        /**
         * @brief Replaces the body-frame offset.
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
         * @brief Marks whether the derived transformation needs recomputation.
         * @param bDirty `true` when derived transforms must be refreshed.
         */
        void SetTransformationDirty(bool bDirty);
        /**
         * @brief Sets the yaw component of the cached rotation.
         * @param fYaw Yaw angle in degrees.
         */
        void SetYaw(Degrees fYaw);
        /**
         * @brief Sets the pitch component of the cached rotation.
         * @param fPitch Pitch angle in degrees.
         */
        void SetPitch(Degrees fPitch);
        /**
         * @brief Sets the roll component of the cached rotation.
         * @param fRoll Roll angle in degrees.
         */
        void SetRoll(Degrees fRoll);
        /**
         * @brief Sets the mirror mode used by this view.
         * @param mirrorMode Mirror mode to store.
         */
        void SetMirrorMode(sbio::EMirrorMode mirrorMode);
        /**
         * @brief Associates the view with a view group.
         * @param viewGroupID View-group identifier to store.
         */
        void SetViewGroupID(sbio::ViewGroupID viewGroupID);
        /**
         * @brief Returns the associated view-group identifier.
         * @return Stored view-group identifier.
         */
        sbio::ViewGroupID GetViewGroupID();
        /**
         * @brief Updates derived state for the view.
         */
        virtual void Update() override;
        /**
         * @brief Recomputes the derived transform from the cached attachment state.
         */
        void UpdateTransformation();
        /**
         * @brief Recomputes derived projection state for the view.
         */
        void UpdateProjection();

        /**
         * @brief Returns the cached body-frame rotation.
         * @return Reference to the cached rotation value.
         */
        const sbio::cigi::TCigiBodyEulerRotation& GetRotation() const;

      private:
        sbio::ViewGroupID m_ViewGroupID = UnknownViewGroupID;///< View group currently associated with this view.
        sbio::cigi::CigiBodyCoordinates m_vOffset;///< Cached body-frame offset relative to the attachment target.
        sbio::cigi::TCigiBodyEulerRotation m_Rotation;///< Cached body-frame rotation relative to the attachment target.
        sbio::EMirrorMode m_eMirrorMode = sbio::EMirrorMode::UNKNOWN;///< Cached mirror mode.
        EntityID m_AttachedEntityID = UnknownEntityID;///< Attached entity identifier, or `UnknownEntityID` when unattached.
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
