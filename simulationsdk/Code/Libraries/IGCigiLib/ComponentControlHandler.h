//Copyright SimBlocks LLC 2016-2026
/**
 * @file ComponentControlHandler.h
 * @brief Declares component control handler and data parser classes for SimBlocks IGCigiLib library.
 *
 * Provides classes for parsing component control packet payloads and handling component control messages in the SimBlocks IGCigiLib library.
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
        /** @brief Destroys the parser without destroying the packet it references. */
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
       *
       * The packet must outlive this parser. Indices are forwarded without validation. All data getters
       * return `uint8_t`, including getters named for wider or floating-point types; the underlying
       * accessor's result is converted, not reinterpreted, to that return type.
       */
      template <typename T>
      class CComponentDataParser : public CBaseComponentDataParser
      {
      public:
        /**
         * @brief Borrows a full component-control packet.
         * @param componentControl Packet that must remain valid for this parser's lifetime.
         */
        CComponentDataParser(const T& componentControl) : m_ComponentControl(componentControl) {};

        /**
         * @brief Reads through `GetUCharCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Byte selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetUChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUCharCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetCharCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Byte selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetCharCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetUShortCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Subword selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetUShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUShortCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetShortCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Subword selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetShortCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetULongCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetULong(unsigned int nWord)
        {
          return m_ComponentControl.GetULongCompData(nWord);
        }

        /**
         * @brief Reads through `GetLongCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetLong(unsigned int nWord)
        {
          return m_ComponentControl.GetLongCompData(nWord);
        }

        /**
         * @brief Reads through `GetFloatCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result numerically converted to `uint8_t`; not the floating-point bit pattern.
         */
        uint8_t GetFloat(unsigned int nWord)
        {
          return m_ComponentControl.GetFloatCompData(nWord);
        }

        /**
         * @brief Reads through `GetI64CompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetI64(unsigned int nWord)
        {
          return m_ComponentControl.GetI64CompData(nWord);
        }

        /**
         * @brief Reads through `GetDoubleCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result numerically converted to `uint8_t`; not the floating-point bit pattern.
         */
        uint8_t GetDouble(unsigned int nWord)
        {
          return m_ComponentControl.GetDoubleCompData(nWord);
        }

        /**
         * @brief Reports the full-packet component-data word count.
         * @return Six words; this method does not validate individual indices.
         */
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
       *
       * The packet must outlive the parser. As in `CComponentDataParser`, indices are forwarded without
       * checks and every accessor result is converted to `uint8_t`, regardless of the getter's name.
       */
      template <typename T>
      class CShortComponentDataParser : public CBaseComponentDataParser
      {
      public:
        /**
         * @brief Borrows a short component-control packet.
         * @param componentControl Packet that must remain valid for this parser's lifetime.
         */
        CShortComponentDataParser(const T& componentControl) : m_ComponentControl(componentControl) {};

        /**
         * @brief Reads through `GetUCharCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Byte selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetUChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUCharCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetCharCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Byte selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetChar(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetCharCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetUShortCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Subword selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetUShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetUShortCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetShortCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @param nByte Subword selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetShort(unsigned int nWord, unsigned int nByte)
        {
          return m_ComponentControl.GetShortCompData(nWord, nByte);
        }

        /**
         * @brief Reads through `GetULongCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetULong(unsigned int nWord)
        {
          return m_ComponentControl.GetULongCompData(nWord);
        }

        /**
         * @brief Reads through `GetLongCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetLong(unsigned int nWord)
        {
          return m_ComponentControl.GetLongCompData(nWord);
        }

        /**
         * @brief Reads through `GetFloatCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result numerically converted to `uint8_t`.
         */
        uint8_t GetFloat(unsigned int nWord)
        {
          return m_ComponentControl.GetFloatCompData(nWord);
        }

        /**
         * @brief Reads through `GetI64CompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result converted to `uint8_t`.
         */
        uint8_t GetI64(unsigned int nWord)
        {
          return m_ComponentControl.GetI64CompData(nWord);
        }

        /**
         * @brief Reads through `GetDoubleCompData()`.
         * @param nWord Word selector forwarded to the packet.
         * @return Accessor result numerically converted to `uint8_t`.
         */
        uint8_t GetDouble(unsigned int nWord)
        {
          return m_ComponentControl.GetDoubleCompData(nWord);
        }

        /**
         * @brief Reports the short-packet component-data word count.
         * @return Two words; this method does not validate individual indices.
         */
        virtual int GetMaxCount() const override
        {
          return 2;
        }

      private:
        const T& m_ComponentControl;
      };

      /**
       * @brief Dispatches component-control packets to subsystem-specific handlers.
       *
       * `Handle()` suppresses unchanged cached states except for entities, symbols, and symbol surfaces.
       * Direct subsystem methods bypass that cache. Parser pointers are not retained and are currently
       * unused by the subsystem methods; message data comes from `componentControl.state`.
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
        /** @brief Publishes atmosphere component state.
         * @param componentControl Instance identifier, component key, and state to publish.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleAtmosphere(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes celestial-sphere component state.
         * @param componentControl Instance identifier, component key, and state to publish.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleCelestialSphere(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Applies component state to a managed CIGI entity; missing entities are logged and ignored.
         * @param componentControl Entity instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         * @pre The global entity manager is available and any matching entity is a `CCigiEntity`.
         */
        void HandleEntity(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes event component state.
         * @param componentControl Event instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleEvent(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes global layered-weather component state.
         * @param componentControl Layer instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleGlobalLayeredWeather(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes global sea-surface component state.
         * @param componentControl Surface instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleGlobalSeaSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes global terrain-surface component state.
         * @param componentControl Surface instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleGlobalTerrainSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Ignores a component update with no component class.
         * @param componentControl Unused component payload.
         * @param pComponentDataParser Unused parser pointer.
         */
        void HandleNoComponent(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes regional layered-weather component state.
         * @param componentControl Layer instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleRegionalLayeredWeather(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes regional sea-surface component state.
         * @param componentControl Region instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleRegionalSeaSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes regional terrain-surface component state.
         * @param componentControl Surface instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleRegionalTerrainSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes sensor component state.
         * @param componentControl Sensor instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleSensor(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes symbol component state without handler-level duplicate suppression.
         * @param componentControl Symbol instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleSymbol(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes symbol-surface component state without handler-level duplicate suppression.
         * @param componentControl Surface instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleSymbolSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes system component state.
         * @param componentControl System instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleSystem(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes view component state.
         * @param componentControl View instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
        void HandleView(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser);
        /** @brief Publishes view-group component state.
         * @param componentControl View-group instance identifier, component key, and state.
         * @param pComponentDataParser Unused; may be null.
         */
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
