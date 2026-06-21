//Copyright SimBlocks LLC 2016-2026
#pragma once
/**
 * @file EngineTypeDeclarations.h
 * @brief Forward declarations for key engine and image generator types.
 *
 * This header provides forward declarations for core engine types and image generator event messengers.
 * It is intended to reduce compile-time dependencies by allowing other headers to reference these types
 * without including their full definitions.
 */
#ifndef SIMBLOCKS_ENGINE_TYPE_DECLARATIONS_H
#define SIMBLOCKS_ENGINE_TYPE_DECLARATIONS_H

namespace sbio
{
  namespace engine
  {
    class CDatabaseLoader;
  }

  namespace ig
  {
    struct IImageGeneratorEventMessenger;

    class CImageGeneratorEventMessenger;
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
