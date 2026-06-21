//Copyright SimBlocks LLC 2016-2026
/**
 * @file IGCigiTypeDeclarations.h
 * @brief Forward declarations for SimBlocks CIGI IG library types and handlers.
 *
 * Provides forward declarations for core classes and structs used in SimBlocks CIGI IG integration, including handlers for control, entity, view, symbol, terrain, and more.
 * Integrates with SimBlocks CIGI library for simulation, messaging, and resource management.
 *
 * @see CCigiAccelerationControlHandler
 * @see CCigiAnimationControlHandler
 * @see CCigiAtmosphereControlHandler
 * @see CCigiArticulatedPartHandler
 * @see CCigiCelestialSphereHandler
 * @see CCigiCollisionControlHandler
 * @see CCigiEarthReferenceModelHandler
 * @see CCigiEntity
 * @see CCigiView
 * @see CCigiComponentControlHandler
 * @see CCigiEntityControlHandler
 * @see CCigiEnvironmentalRegionHandler
 * @see CCigiMaritimeSurfaceConditionPacketHandler
 * @see CCigiSymbolHandler
 * @see CCigiEnvironmentalRegion
 * @see CCigiImageGenerator
 * @see CCigiMotionTrackerControlHandler
 * @see CCigiPacketHandler
 * @see CCigiPacketHandlerV3
 * @see CCigiPacketHandlerV4
 * @see CCigiPacketSender
 * @see CCigiPacketSenderV3
 * @see CCigiPacketSenderV4
 * @see CCigiRegionTree
 * @see CCigiVelocityControlHandler
 * @see CCigiViewHandler
 * @see CCigiWaveControlHandler
 * @see CCigiCollisionControlHandler
 * @see CCigiSensorControlHandler
 * @see CCigiTerrainHandler
 * @see CIGResponseEventDispatcher
 * @see SCigiImageGeneratorProperties
 */
#pragma once

#ifndef SIMBLOCKS_IG_CIGI_LIB_TYPE_DECLARATIONS_H
#define SIMBLOCKS_IG_CIGI_LIB_TYPE_DECLARATIONS_H

#include "CigiLib/CigiTypeDeclarations.h"

namespace sbio
{
  namespace cigi
  {
    namespace ig
    {
      // Forward declarations for CIGI IG library types and handlers
      class CCigiAccelerationControlHandler;
      class CCigiAnimationControlHandler;
      class CCigiAtmosphereControlHandler;
      class CCigiArticulatedPartHandler;
      class CCigiCelestialSphereHandler;
      class CCigiCollisionSegment;
      class CCigiEarthReferenceModelHandler;
      class CCigiEntity;
      class CCigiView;
      class CCigiComponentControlHandler;
      class CCigiEntityControlHandler;
      class CCigiEnvironmentalRegionHandler;
      class CCigiMaritimeSurfaceConditionPacketHandler;
      class CCigiSymbolHandler;

      class CCigiEnvironmentalRegion;
      class CCigiImageGenerator;
      class CCigiMessageLogger;
      class CCigiMotionTrackerControlHandler;
      class CCigiPacketHandler;
      class CCigiPacketHandlerV3;
      class CCigiPacketHandlerV4;
      class CCigiPacketSender;
      class CCigiPacketSenderV3;
      class CCigiPacketSenderV4;
      class CCigiPacketSenders;
      class CCigiRegionTree;
      class CCigiVelocityControlHandler;
      class CCigiViewHandler;
      class CCigiWaveControlHandler;
      class CCigiCollisionControlHandler;
      class CCigiSensorControlHandler;
      class CCigiTerrainHandler;

      class CIGResponseEventDispatcher;

      struct SIGSetupOptions;
      struct SCigiImageGeneratorProperties;
    }
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
