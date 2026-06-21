//Copyright SimBlocks LLC 2016-2026
/**
 * @file CigiSymbol.h
 * @brief Declares the CCigiSymbol class for SimBlocks CIGI IG symbol management and CIGI-specific symbol logic.
 *
 * Provides the CCigiSymbol class for representing and managing middleware symbols with CIGI-specific data and logic in the SimBlocks CIGI IG library.
 * Inherits from sbio::symbol::CSymbol and integrates with SimBlocks symbol, utility, and color types for symbol rendering and manipulation.
 * Supports symbol cloning, copying, color and transformation management, flash control, and visibility updates for simulation symbols.
 *
 * @see sbio::cigi::ig::CCigiSymbol
 * @see sbio::symbol::CSymbol
 * @see sbio::SColor32
 * @see sbio::SymbolID
 * @see sbio::symbol::ESymbolType
 * @see sbio::SymbolSurfaceID
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_SYMBOL_H
#define SIMBLOCKS_CIGI_SYMBOL_H

#include "SymbolLib/Symbol.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include <memory>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Base symbol implementation used for CIGI-driven symbols.
       *
       * Responsibilities:
       * - Extends `sbio::symbol::CSymbol` with flash timing behavior and CIGI-specific update publication.
       * - Stores flash configuration independently of concrete symbol geometry.
       *
       * Ownership:
       * - Inherits symbol lifecycle from `sbio::symbol::CSymbol`.
       * - Owns the optional stopwatch used to evaluate flash state.
       *
       * Invariants:
       * - `m_fFlashDutyCycle` and `m_fFlashPeriod` describe the current flash configuration.
       * - When present, `m_pFlashStopWatch` measures the flash phase for this symbol instance only.
       *
       * Failure cases:
       * - Message-delivery failures and symbol-surface resolution are handled by downstream systems.
       *
       * Side effects:
       * - Mutating operations may update inherited symbol state and may emit symbol create/update messages in the implementation.
       */
      class CCigiSymbol : public sbio::symbol::CSymbol
      {
      public:
        /**
         * @brief Constructs a CIGI symbol.
         * @param symbolID Unique symbol identifier.
         * @param eSymbolType Symbol type enumeration.
         */
        CCigiSymbol(sbio::symbol::SymbolID symbolID, sbio::symbol::ESymbolType eSymbolType);
        /**
         * @brief Destroys the CIGI symbol.
         */
        ~CCigiSymbol();

        /**
         * @brief Clones the symbol with a new symbol ID.
         * @param symbolID The new symbol ID for the clone.
         * @return A unique pointer to the cloned symbol.
         */
        virtual std::unique_ptr<CSymbol> Clone(sbio::symbol::SymbolID symbolID) override;
        /**
         * @brief Copies properties from another symbol.
         * @param pSymbol The symbol to copy from.
         * @param symbolID The symbol ID to assign.
         */
        virtual void CopyFrom(CSymbol* pSymbol, sbio::symbol::SymbolID symbolID) override;
        /**
         * @brief Gets the flash duty cycle (0-1).
         * @return Flash duty cycle value.
         */
        float GetFlashDutyCycle() const;
        /**
         * @brief Gets the flash period in seconds.
         * @return Flash period value.
         */
        float GetFlashPeriod() const;
        /**
         * @brief Restarts the flash timer for the symbol.
         */
        void RestartFlash();
        /**
         * @brief Sets the color of the symbol.
         * @param color The new color.
         */
        virtual void SetColor(const SColor32& color) override;
        /**
         * @brief Sets flash parameters for the symbol.
         * @param fFlashDutyCycle Duty cycle (0-1).
         * @param fFlashPeriod Period in seconds.
         */
        void SetFlash(float fFlashDutyCycle, float fFlashPeriod);
        /**
         * @brief Sets the rotation of the symbol.
         * @param fRotation Rotation in degrees.
         */
        virtual void SetRotation(Degrees fRotation) override;
        /**
         * @brief Sets the visibility of the symbol.
         * @param bVisible True to make visible, false to hide.
         * @param bForceChange Force the visibility change.
         */
        virtual void SetVisible(bool bVisible, bool bForceChange = false) override;
        /**
         * @brief Sets the symbol surface ID.
         * @param symbolSurfaceID The new symbol surface ID.
         */
        virtual void SetSymbolSurfaceID(sbio::symbol::SymbolSurfaceID symbolSurfaceID) override;
        /**
         * @brief Updates the symbol state.
         */
        virtual void Update() override;

      private:
        float m_fFlashDutyCycle = 0;///< Cached flash duty cycle in the normalized range expected by the implementation.
        float m_fFlashPeriod = 0;///< Cached flash period in seconds.
        std::unique_ptr<sbio::utils::CStopWatch> m_pFlashStopWatch;///< Owned stopwatch used to evaluate flash state.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
