//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiArticulatedPart.h
 * @brief Declares the CCigiArticulatedPart class for SimBlocks CIGI IG articulated part management.
 *
 * Provides the CCigiArticulatedPart class for managing articulated parts in the SimBlocks CIGI IG library, including degree of freedom application, transformation, and state updates.
 * Inherits from CArticulatedPart and integrates with SimBlocks entity, math, and CIGI types for simulation and articulated part control.
 *
 * @see sbio::cigi::ig::CCigiArticulatedPart
 * @see sbio::entity::CArticulatedPart
 * @see sbio::math::CigiBodyCoordinates
 * @see sbio::math::TCigiBodyEulerRotation
 * @see sbio::math::TBodyEulerRotation
 * @see sbio::math::BodyCoordinates
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_ARTICULATED_PART_H
#define SIMBLOCKS_CIGI_ARTICULATED_PART_H

#include "EngineLib/ImageGeneratorMessages.h"
#include "EntityLib/ArticulatedPart.h"
#include "CigiLib/CigiTypes.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Articulated part class for SimBlocks CIGI IG integration.
       *
       * Manages articulated part state, transformation, and degree of freedom for simulation.
       */
      class CCigiArticulatedPart : public sbio::entity::CArticulatedPart
      {
      public:
        /**
         * @brief Constructs a CIGI articulated part.
         * @param entityID Identifier of the owning entity.
         * @param articulatedPartID Identifier of the articulated part within that entity.
         */
        CCigiArticulatedPart(sbio::EntityID entityID, sbio::ArticulatedPartID articulatedPartID);
        /**
         * @brief Destroys the articulated part wrapper.
         */
        ~CCigiArticulatedPart();

        /**
         * @brief Applies one degree-of-freedom update.
         * @param eDofSelect Degree of freedom to modify.
         * @param fDof Value to apply for the selected degree of freedom.
         */
        void ApplyDegreeOfFreedom(EDegreeOfFreedom eDofSelect, float fDof);
        /**
         * @brief Replaces the articulated-part body transform.
         * @param offset Body-frame offset to apply.
         * @param rotation Body-frame Euler rotation to apply.
         */
        void ApplyTransformation(const sbio::cigi::CigiBodyCoordinates& offset, const sbio::cigi::TCigiBodyEulerRotation& rotation);
        /**
         * @brief Disables the articulated part.
         */
        virtual void Disable() override;
        /**
         * @brief Enables the articulated part.
         */
        virtual void Enable() override;
        /**
         * @brief Replaces the articulated-part rotation.
         * @param rotation Body-frame rotation to store.
         */
        virtual void SetBodyEulerRotation(const sbio::math::TBodyEulerRotation& rotation) override;
        /**
         * @brief Replaces the articulated-part position.
         * @param position Body-frame position to store.
         */
        virtual void SetPosition(const sbio::math::BodyCoordinates& position) override;
        /**
         * @brief Updates deferred state for the articulated part.
         * @param fDeltaTime Simulation time step in seconds.
         */
        virtual void Update(double fDeltaTime) override;

      private:
        bool m_bDirty = false;///< Indicates whether the current state still needs to be published.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
