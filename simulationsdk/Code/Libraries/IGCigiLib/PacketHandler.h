//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketHandler.h
 * @brief Declares the CCigiPacketHandler class for SimBlocks CIGI IG packet processing and handler management.
 *
 * Provides the CCigiPacketHandler class for managing packet processing, handler objects, and simulation state in the SimBlocks CIGI IG library.
 * Integrates with SimBlocks CIGI, entity, terrain, animation, event, and utility types for simulation and packet management.
 * Supports packet logging, handler initialization, and simulation update routines.
 *
 * @see sbio::cigi::ig::CCigiPacketHandler
 * @see sbio::cigi::ig::CCigiImageGenerator
 * @see sbio::cigi::ig::CCigiTerrainHandler
 * @see sbio::cigi::ig::CCigiEntityControlHandler
 * @see sbio::cigi::ig::CCigiAccelerationControlHandler
 * @see sbio::cigi::ig::CCigiAnimationControlHandler
 * @see sbio::cigi::ig::CCigiArticulatedPartHandler
 * @see sbio::cigi::ig::CCigiAtmosphereControlHandler
 * @see sbio::cigi::ig::CCigiCelestialSphereHandler
 * @see sbio::cigi::ig::CCigiEarthReferenceModelHandler
 * @see sbio::cigi::ig::CCigiEnvironmentalRegionHandler
 * @see sbio::cigi::ig::CCigiMaritimeSurfaceConditionPacketHandler
 * @see sbio::cigi::ig::CCigiComponentControlHandler
 * @see sbio::cigi::ig::CCigiSymbolHandler
 * @see sbio::cigi::ig::CCigiVelocityControlHandler
 * @see sbio::cigi::ig::CCigiViewHandler
 * @see sbio::cigi::ig::CCigiWaveControlHandler
 * @see sbio::cigi::ig::CCigiCollisionControlHandler
 * @see sbio::cigi::ig::CCigiSensorControlHandler
 * @see sbio::cigi::ig::CCigiMotionTrackerControlHandler
 * @see sbio::utils::CPacketLoggerWriter
 * @see sbio::utils::CPacketLoggerReader
 * @see sbio::utils::CUDPReceiveSocket
 */
#pragma once
#ifndef SIMBLOCKS_CIGI_LIB_PACKET_HANDLER_H
#define SIMBLOCKS_CIGI_LIB_PACKET_HANDLER_H

#include "CigiLib/CigiTypeDeclarations.h"
#include "EntityLib/EntityType.h"
#include "GlobalHeaders/CommonTypes.h"
#include "IGCigiLib/IGCigiTypeDeclarations.h"
#include "UtilitiesLib/UtilitiesDeclarations.h"
#include "CigiLib/CigiTypesIGToHost.h"
#include "UtilitiesLib/PacketLoggerReader.h"
#include <memory>
#include <unordered_map>

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      /**
       * @brief Tracks the connection state of one host endpoint observed by a packet handler.
       */
      struct SHostConnectionInfo
      {
        std::string sHostAddress;
        double fLastMessageReceivedTime = 0;
        bool bConnected = false;
      };

      /**
       * @brief Base class for version-specific host-to-IG packet handlers.
       */
      class CCigiPacketHandler
      {
      public:
        /**
         * @brief Constructs a packet handler.
         * @param imageGenerator Reference to the image generator.
         * @param nHostToIgPort Host-to-IG port number.
         */
        CCigiPacketHandler(CCigiImageGenerator& imageGenerator, int nHostToIgPort);
        /**
         * @brief Destroys the packet handler.
         */
        virtual ~CCigiPacketHandler();

        /**
         * @brief Gets the terrain handler.
         * @return Non-owning pointer to the owned terrain handler.
         */
        CCigiTerrainHandler* GetTerrainHandler() const;
        /**
         * @brief Gets the environmental region handler.
         * @return Non-owning pointer to the owned environmental-region handler.
         */
        CCigiEnvironmentalRegionHandler* GetEnvironmentalRegionHandler() const;
        /**
         * @brief Handles IG control packet.
         * @param igControl IG control packet data.
         */
        void Handle(const SCigiIgControl& igControl);
        /**
         * @brief Processes incoming packets (pure virtual).
         *
         * Derived classes implement protocol-version-specific packet framing and parsing.
         */
        virtual void ProcessPackets() = 0;

        /**
         * @brief Emits the host-connected diagnostic message for one endpoint.
         */
        void SendConnectedToHostMessage(std::string sHostAddress);

        /**
         * @brief Emits the host-disconnected diagnostic message for one endpoint.
         */
        void SendDisconnectedFromHostMessage(std::string sHostAddress);

        /**
         * @brief Updates the packet handler state.
         */
        void Update();

      protected:
        /**
         * @brief Updates connectivity state for the host that produced the latest packet.
         */
        void CheckHostConnection(std::string sHostAddress, double fCurrentTime);

        /**
         * @brief Detects hosts that have timed out and emits disconnect diagnostics.
         */
        void CheckHostDisconnection(double fCurrentTime);

      protected:
        CCigiImageGenerator& m_ImageGenerator;///< Non-owning image generator associated with this handler.
        std::unique_ptr<sbio::utils::CPacketLoggerWriter> m_pPacketLoggerWriter;///< Owned packet-log writer when packet logging is enabled for write mode.
        std::unique_ptr<sbio::utils::CPacketLoggerReader> m_pPacketLoggerReader;///< Owned packet-log reader when packet logging is enabled for read mode.
        std::unique_ptr<CCigiEntityControlHandler> m_pEntityControlHandler;///< Owned entity-control handler.
        std::unique_ptr<CCigiAccelerationControlHandler> m_pAccelerationControlHandler;///< Owned acceleration-control handler.
        std::unique_ptr<CCigiAnimationControlHandler> m_pAnimationControlHandler;///< Owned animation-control handler.
        std::unique_ptr<CCigiArticulatedPartHandler> m_pArticulatedPartHandler;///< Owned articulated-part handler.
        std::unique_ptr<CCigiAtmosphereControlHandler> m_pAtmosphereControlHandler;///< Owned atmosphere-control handler.
        std::unique_ptr<CCigiCelestialSphereHandler> m_pCelestialSphereHandler;///< Owned celestial-sphere handler.
        std::unique_ptr<CCigiEarthReferenceModelHandler> m_pEarthReferenceModelHandler;///< Owned earth-reference-model handler.
        std::unique_ptr<CCigiEnvironmentalRegionHandler> m_pEnvironmentalRegionHandler;///< Owned environmental-region handler.
        std::unique_ptr<CCigiMaritimeSurfaceConditionPacketHandler> m_pMaritimeSurfaceConditionPacketHandler;///< Owned maritime-surface packet handler.
        std::unique_ptr<CCigiComponentControlHandler> m_pComponentControlHandler;///< Owned component-control handler.
        std::unique_ptr<CCigiSymbolHandler> m_pSymbolHandler;///< Owned symbol handler.
        std::unique_ptr<CCigiVelocityControlHandler> m_pVelocityControlHandler;///< Owned velocity-control handler.
        std::unique_ptr<CCigiViewHandler> m_pViewHandler;///< Owned view handler.
        std::unique_ptr<CCigiWaveControlHandler> m_pWaveControlHandler;///< Owned wave-control handler.
        std::unique_ptr<CCigiCollisionControlHandler> m_pCollisionControlHandler;///< Owned collision-control handler.
        std::unique_ptr<CCigiSensorControlHandler> m_pSensorControlHandler;///< Owned sensor-control handler.
        std::unique_ptr<CCigiMotionTrackerControlHandler> m_pMotionTrackerControlHandler;///< Owned motion-tracker-control handler.
        std::unique_ptr<CCigiTerrainHandler> m_pTerrainHandler;///< Owned terrain handler.
        std::unique_ptr<sbio::utils::CUDPReceiveSocket> m_pSocketHostToIG;///< Owned UDP socket used to receive host-to-IG traffic.
        bool m_bByteSwap = false;///< Indicates whether inbound packets require byte swapping.

        std::list<SHostConnectionInfo> m_HostConnections;///< Per-host connectivity records maintained by timeout logic.
      };
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
