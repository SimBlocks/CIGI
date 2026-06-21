//Copyright SimBlocks LLC 2016-2026
/**
 * @file ComponentControlHandler.h
 * @brief Declares component control handler and data parser classes for SimBlocks CIGI IG library.
 *
 * Provides classes for parsing component control packet payloads and handling component control messages in the SimBlocks CIGI IG library.
 * Supports both short and standard component control packets, with data parsing abstraction and handler function mapping.
 * Integrates with SimBlocks CIGI and common types for simulation and component management.
 *
 * @see sbio::cigi::ig::CBaseComponentDataParser
 * @see sbio::cigi::ig::CComponentDataParser
 * @see sbio::cigi::ig::CShortComponentDataParser
 * @see sbio::cigi::ig::CCigiComponentControlHandler
 * @see SCigiComponentControl
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_COMPONENT_CONTROL_HANDLER_H
#define SIMBLOCKS_CIGI_COMPONENT_CONTROL_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "GlobalHeaders/CommonTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include <unordered_map>
#include <map>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Abstract view of the component-control component data words exposed by parsed packet types.
       *
       */
      class CBaseComponentDataParser
      {
      public:
        virtual ~CBaseComponentDataParser() {};
        /**
         * @brief Reports the number of component-data words exposed by this parser.
         * @return Maximum accessible word count for the underlying packet form.
         */
        virtual int GetMaxCount() const = 0;
      };

      /**
       * @brief Non-owning parser for full component-control packets.
       * @tparam T Parsed packet type that exposes the `Get*CompData()` accessors used here.
       */
      template <typename T>
      class CComponentDataParser : public CBaseComponentDataParser
      {
      public:
        CComponentDataParser(const T& componentControl) : m_ComponentControl(componentControl) {};

        uint8_t GetUChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUCharCompData(nWord, nByte);
        }

        uint8_t GetChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetCharCompData(nWord, nByte);
        }

        uint8_t GetUShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUShortCompData(nWord, nByte);
        }

        uint8_t GetShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetShortCompData(nWord, nByte);
        }

        uint8_t GetULong(unsigned int nWord)
        {
          return m_ComponentControl.GetULongCompData(nWord);
        }

        uint8_t GetLong(unsigned int nWord)
        {
          return m_ComponentControl.GetLongCompData(nWord);
        }

        uint8_t GetFloat(unsigned int nWord)
        {
          return m_ComponentControl.GetFloatCompData(nWord);
        }

        uint8_t GetI64(unsigned int nWord)
        {
          return m_ComponentControl.GetI64CompData(nWord);
        }

        uint8_t GetDouble(unsigned int nWord)
        {
          return m_ComponentControl.GetDoubleCompData(nWord);
        }

        virtual int GetMaxCount() const override
        {
          return 6;
        }

      public:
        const T& m_ComponentControl;
      };

      /**
       * @brief Non-owning parser for short component-control packets.
       * @tparam T Parsed packet type that exposes the `Get*CompData()` accessors used here.
       */
      template <typename T>
      class CShortComponentDataParser : public CBaseComponentDataParser
      {
      public:
        CShortComponentDataParser(const T& componentControl) : m_ComponentControl(componentControl) {};

        uint8_t GetUChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUCharCompData(nWord, nByte);
        }

        uint8_t GetChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetCharCompData(nWord, nByte);
        }

        uint8_t GetUShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUShortCompData(nWord, nByte);
        }

        uint8_t GetShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetShortCompData(nWord, nByte);
        }

        uint8_t GetULong(unsigned int nWord)
        {
          return m_ComponentControl.GetULongCompData(nWord);
        }

        uint8_t GetLong(unsigned int nWord)
        {
          return m_ComponentControl.GetLongCompData(nWord);
        }

        uint8_t GetFloat(unsigned int nWord)
        {
          return m_ComponentControl.GetFloatCompData(nWord);
        }

        uint8_t GetI64(unsigned int nWord)
        {
          return m_ComponentControl.GetI64CompData(nWord);
        }

        uint8_t GetDouble(unsigned int nWord)
        {
          return m_ComponentControl.GetDoubleCompData(nWord);
        }

        virtual int GetMaxCount() const override
        {
          return 2;
        }

      private:
        const T& m_ComponentControl;
      };

      /**
       * @brief Dispatches component-control packets to subsystem-specific handlers.
       */
      class CCigiComponentControlHandler
      {
      public:
        /**
         * @brief Constructs the dispatch table and empty component-state cache.
         */
        CCigiComponentControlHandler();
        /**
         * @brief Destroys the handler.
         */
        ~CCigiComponentControlHandler();

        /**
         * @brief Routes one component-control update.
         * @param componentControl Parsed component-control state.
         * @param pComponentDataParser Non-owning parser that exposes the component data words for the packet form.
         */
        void Handle(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleAtmosphere(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleCelestialSphere(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleEntity(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleEvent(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleGlobalLayeredWeather(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleGlobalSeaSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleGlobalTerrainSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleNoComponent(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleRegionalLayeredWeather(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleRegionalSeaSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleRegionalTerrainSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleSensor(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleSymbol(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleSymbolSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleSystem(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleView(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        void HandleViewGroup(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);

        /**
         * @brief Clears all cached component state.
         */
        void Reset();

      private:
        typedef void (CCigiComponentControlHandler::*TComponentHandlerFunction)(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        typedef std::unordered_map<CigiComponentClassID, TComponentHandlerFunction, StrongTypeHash<CigiComponentClassID>> TComponentHandlerFunctions;
        TComponentHandlerFunctions m_ComponentHandlerFunctions;

        typedef std::map<sbio::cigi::SCigiComponentKey, sbio::cigi::SCigiComponentControlState> TComponentStates;
        TComponentStates m_ComponentStates;
      };
    }
  }
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
