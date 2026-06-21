//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbolGeometryFactory.h"
#include "CigiSymbolCircle.h"
#include "CigiSymbolPolygon.h"
#include "CigiSymbolText.h"
#include "CigiSymbolTexturedCircle.h"
#include "CigiSymbolTexturedPolygon.h"

using namespace std;
using namespace sbio;
using namespace sbio::symbol;
using namespace sbio::cigi::ig;

std::unique_ptr<CSymbolGeometry> CCigiSymbolGeometryFactory::Create(SymbolID symbolID, ESymbolType eSymbolType)
{
  switch (eSymbolType)
  {
  case ESymbolType::CIRCLE:
  {
    return std::make_unique<CIGCigiSymbolCircle>(symbolID);
  }
  case ESymbolType::POLYGON:
  {
    return std::make_unique<CIGCigiSymbolPolygon>(symbolID);
  }
  case ESymbolType::TEXT:
  {
    return std::make_unique<CIGCigiSymbolText>(symbolID);
  }
  case ESymbolType::TEXTURED_CIRCLE:
  {
    return std::make_unique<CIGCigiSymbolTexturedCircle>(symbolID);
  }
  case ESymbolType::TEXTURED_POLYGON:
  {
    return std::make_unique<CIGCigiSymbolTexturedPolygon>(symbolID);
  }
  default:
    return nullptr;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
