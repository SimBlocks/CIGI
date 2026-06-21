//Copyright SimBlocks LLC 2016-2026
#include "Symbol.h"
#include "SymbolGeometryFactory.h"
#include "SymbolCircle.h"
#include "SymbolPolygon.h"
#include "SymbolText.h"
#include "SymbolTexturedCircle.h"
#include "SymbolTexturedPolygon.h"

using namespace std;
using namespace sbio;
using namespace sbio::symbol;

std::unique_ptr<CSymbolGeometry> CSymbolGeometryFactory::Create(SymbolID symbolID, sbio::symbol::ESymbolType eSymbolType)
{
  switch (eSymbolType)
  {
  case ESymbolType::CIRCLE:
  {
    return std::make_unique<CSymbolCircle>(symbolID);
  }
  case ESymbolType::POLYGON:
  {
    return std::make_unique<CSymbolPolygon>(symbolID);
  }
  case ESymbolType::TEXT:
  {
    return std::make_unique<CSymbolText>(symbolID);
  }
  case ESymbolType::TEXTURED_CIRCLE:
  {
    return std::make_unique<CSymbolTexturedCircle>(symbolID);
  }
  case ESymbolType::TEXTURED_POLYGON:
  {
    return std::make_unique<CSymbolTexturedPolygon>(symbolID);
  }
  default:
    return nullptr;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
