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
     * @brief Generic view class for view management and configuration.
     *
     * Supports view identification, projection configuration, update/reset operations, and extensibility for custom view types.
     */
    class CView
    {
    public:
      /**
       * @brief Constructs a view with the specified view ID.
       * @param viewID The unique identifier for the view.
       */
      CView(const sbio::ViewID& viewID);

      /**
       * @brief Virtual destructor for CView.
       */
      virtual ~CView();

      /**
       * @brief Brings the view to the top of the view stack.
       */
      virtual void BringToTop();

      /**
       * @brief Gets the view's unique ID.
       * @return The view ID.
       */
      virtual sbio::ViewID GetViewID() const;

      /**
       * @brief Resets the view to its default state.
       */
      virtual void Reset();

      /**
       * @brief Updates the view state.
       */
      virtual void Update();

      /**
       * @brief Sets the near clipping plane distance.
       * @param fNear The near plane distance.
       */
      virtual void SetNearPlane(float fNear);

      /**
       * @brief Sets the far clipping plane distance.
       * @param fFar The far plane distance.
       */
      virtual void SetFarPlane(float fFar);

      /**
       * @brief Sets the left half angle for the view frustum.
       * @param fLeftHalfAngle The left half angle in degrees.
       */
      virtual void SetLeftHalfAngle(float fLeftHalfAngle);

      /**
       * @brief Sets the right half angle for the view frustum.
       * @param fRightHalfAngle The right half angle in degrees.
       */
      virtual void SetRightHalfAngle(float fRightHalfAngle);

      /**
       * @brief Sets the top half angle for the view frustum.
       * @param fTopHalfAngle The top half angle in degrees.
       */
      virtual void SetTopHalfAngle(float fTopHalfAngle);

      /**
       * @brief Sets the bottom half angle for the view frustum.
       * @param fBottomHalfAngle The bottom half angle in degrees.
       */
      virtual void SetBottomHalfAngle(float fBottomHalfAngle);

      /**
       * @brief Sets the pixel replication mode for the view.
       * @param nWidth The width in pixels.
       * @param nHeight The height in pixels.
       */
      virtual void SetPixelReplicationMode(int nWidth, int nHeight);

      /**
       * @brief Sets the projection mode for the view.
       * @param eProjectionMode The projection mode.
       */
      virtual void SetProjectionMode(sbio::EProjectionMode eProjectionMode);

    protected:
      sbio::ViewID m_ViewID = UnknownViewID;///< Unique view ID
      bool m_bTransformationDirty = false;///< Transformation dirty flag
      bool m_bProjectionDirty = false;///< Projection dirty flag

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
