//Copyright SimBlocks LLC 2016-2026
#include "SymbolPolygon.h"

using namespace sbio::symbol;

CSymbolPolygon::CSymbolPolygon(SymbolID symbolID) : CSymbolGeometry(ESymbolType::POLYGON)
{
  m_SymbolPolygon.symbolID = symbolID;
}

void CSymbolPolygon::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CSymbolPolygon* pSymbol = dynamic_cast<CSymbolPolygon*>(pSymbolGeometry);
  if (pSymbol)
  {
    Set(pSymbol->GetProperties(), m_SymbolPolygon.symbolID);
  }
}

const sbio::symbol::SSymbolPolygon& CSymbolPolygon::GetProperties() const
{
  return m_SymbolPolygon;
}

void CSymbolPolygon::Set(const SSymbolPolygon& symbolPolygonProperties, SymbolID symbolID)
{
  m_SymbolPolygon = symbolPolygonProperties;
  m_SymbolPolygon.symbolID = symbolID;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
