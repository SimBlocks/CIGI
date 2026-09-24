//Copyright SimBlocks LLC 2016-2026
/**
 * @file View.h
 * @brief Declares the CView class for generic view management in the view system.
 *
 * Provides the CView class for representing and managing generic views, supporting view identification,
 * projection configuration, and update/reset operations. Enables extensibility for custom view types and
 * integration with the view management system.
 *
 * @see sbio::view::CView
 * @see sbio::ViewID
 * @see sbio::EProjectionMode
 */
#pragma once
#ifndef SIMBLOCKS_VIEW_LIB_VIEW_H
#define SIMBLOCKS_VIEW_LIB_VIEW_H

#include "GlobalHeaders/CommonTypes.h"
#include "MathLib/MathTypes.h"
#include "ViewLib/ViewTypes.h"

namespace sbio
{
  namespace view
  {
    /**
     * @brief Stores a view identifier and projection parameters for derived view implementations.
     *
     * The base class caches configuration and dirty flags; it does not create a camera or apply rendering changes.
     * Numeric projection setters use `sbio::math::fequals` to detect changes, store changed values, and mark the
     * projection dirty. They do not validate ranges or the ordering of clipping planes. Derived classes can use
     * `Update()` to consume dirty state and implement rendering-specific behavior.
     */
    class CView
    {
    public:
      /**
       * @brief Constructs a view with the specified view ID.
       * @param viewID Identifier to store; the constructor does not validate or register it.
       *
       * Initializes perspective projection with near/far distances of 1 and 1000000, left/right half angles of
       * -40/40 degrees, and top/bottom half angles of 30/-30 degrees. Projection starts dirty; transformation does not.
       */
      CView(const sbio::ViewID& viewID);

      /**
       * @brief Supports destruction through a base view pointer; the base implementation performs no additional work.
       */
      virtual ~CView();

      /**
       * @brief Extension point for bringing a view to the top of a rendering stack.
       *
       * The base implementation does nothing.
       */
      virtual void BringToTop();

      /**
       * @brief Gets the view's unique ID.
       * @return Copy of the stored identifier, including `UnknownViewID` if supplied at construction.
       */
      virtual sbio::ViewID GetViewID() const;

      /**
       * @brief Clears the transformation and projection dirty flags.
       *
       * The base implementation preserves the identifier and all projection parameters; it does not restore
       * constructor defaults or apply pending rendering changes.
       */
      virtual void Reset();

      /**
       * @brief Extension point for updating derived view state.
       *
       * The base implementation does nothing and leaves dirty flags unchanged.
       */
      virtual void Update();

      /**
       * @brief Stores a changed near clipping distance and marks the projection dirty.
       * @param fNear Near plane distance; values considered equal by `sbio::math::fequals` leave the stored state unchanged.
       */
      virtual void SetNearPlane(float fNear);

      /**
       * @brief Stores a changed far clipping distance and marks the projection dirty.
       * @param fFar Far plane distance; values considered equal by `sbio::math::fequals` leave the stored state unchanged.
       */
      virtual void SetFarPlane(float fFar);

      /**
       * @brief Stores a changed left frustum half angle and marks the projection dirty.
       * @param fLeftHalfAngle The left half angle in degrees.
       */
      virtual void SetLeftHalfAngle(float fLeftHalfAngle);

      /**
       * @brief Stores a changed right frustum half angle and marks the projection dirty.
       * @param fRightHalfAngle The right half angle in degrees.
       */
      virtual void SetRightHalfAngle(float fRightHalfAngle);

      /**
       * @brief Stores a changed top frustum half angle and marks the projection dirty.
       * @param fTopHalfAngle The top half angle in degrees.
       */
      virtual void SetTopHalfAngle(float fTopHalfAngle);

      /**
       * @brief Stores a changed bottom frustum half angle and marks the projection dirty.
       * @param fBottomHalfAngle The bottom half angle in degrees.
       */
      virtual void SetBottomHalfAngle(float fBottomHalfAngle);

      /**
       * @brief Extension point for configuring pixel replication.
       * @param nWidth Requested horizontal replication value; ignored by the base implementation.
       * @param nHeight Requested vertical replication value; ignored by the base implementation.
       *
       * The base implementation stores neither value and leaves dirty flags unchanged.
       */
      virtual void SetPixelReplicationMode(int nWidth, int nHeight);

      /**
       * @brief Stores a different projection mode and marks the projection dirty.
       * @param eProjectionMode Mode to store, without validation; the same mode leaves dirty state unchanged.
       */
      virtual void SetProjectionMode(sbio::EProjectionMode eProjectionMode);

    protected:
      sbio::ViewID m_ViewID = UnknownViewID;///< Identifier supplied at construction.
      bool m_bTransformationDirty = false;///< Transformation refresh flag available to derived classes; cleared by Reset().
      bool m_bProjectionDirty = true;///< Set when cached projection parameters change; cleared by Reset().

      float m_fNear = 0;///< Near clipping plane distance
      float m_fFar = 0;///< Far clipping plane distance
      float m_fLeftHalfAngle = 0;///< Left half angle
      float m_fRightHalfAngle = 0;///< Right half angle
      float m_fTopHalfAngle = 0;///< Top half angle
      float m_fBottomHalfAngle = 0;///< Bottom half angle

      sbio::EProjectionMode m_eProjectionMode = sbio::EProjectionMode::UNKNOWN;///< Projection mode
    };
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
