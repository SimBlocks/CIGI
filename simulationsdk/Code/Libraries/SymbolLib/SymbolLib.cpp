//Copyright SimBlocks LLC 2016-2026
#include "SymbolLib.h"
#include "SymbolGeometryFactory.h"
#include "GlobalHeaders/Globals.h"

sbio::symbol::SSymbolLibSettings g_SymbolLibSettings;

void InitSymbolLib(const sbio::SGlobals& globals, std::unique_ptr<sbio::symbol::CSymbolGeometryFactory> pSymbolGeometryFactory)
{
  g_SymbolLibSettings.dataPath = globals.librariesDataPath;
  g_SymbolLibSettings.pLogger = globals.pLogger;
  g_SymbolLibSettings.pSymbolGeometryFactory = std::move(pSymbolGeometryFactory);
}

void UninitSymbolLib()
{
  g_SymbolLibSettings.dataPath.clear();
  g_SymbolLibSettings.pLogger.reset();
  g_SymbolLibSettings.pSymbolGeometryFactory.reset();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
