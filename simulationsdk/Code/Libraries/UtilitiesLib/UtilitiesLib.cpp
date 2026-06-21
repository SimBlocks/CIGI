//Copyright SimBlocks LLC 2016-2026
/**
 * @file UtilitiesLib.cpp
 * @brief Implementation file for Utilities Library globals.
 *
 * This file defines the global variables and storage for the utilities library,
 * including the global SUtilitiesGlobals instance used for event dispatching and logging.
 *
 * @see sbio::utils::SUtilitiesGlobals
 */
#include "UtilitiesLib.h"

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Global instance holding pointers to utility services such as event dispatcher and logger.
     *
     * This instance is used throughout the utilities library to provide access to shared services.
     */
    SUtilitiesGlobals g_UtilitiesGlobals;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
