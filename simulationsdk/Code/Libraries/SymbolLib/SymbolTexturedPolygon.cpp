//Copyright SimBlocks LLC 2016-2026
#include "SymbolTexturedPolygon.h"

using namespace sbio::symbol;

CSymbolTexturedPolygon::CSymbolTexturedPolygon(SymbolID symbolID) : CSymbolGeometry(ESymbolType::TEXTURED_POLYGON)
{
  m_SymbolTexturedPolygon.symbolID = symbolID;
}

void CSymbolTexturedPolygon::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CSymbolTexturedPolygon* pSymbol = dynamic_cast<CSymbolTexturedPolygon*>(pSymbolGeometry);
  if (pSymbol)
  {
    Set(pSymbol->GetProperties(), m_SymbolTexturedPolygon.symbolID);
  }
}

const sbio::symbol::SSymbolTexturedPolygon& CSymbolTexturedPolygon::GetProperties() const
{
  return m_SymbolTexturedPolygon;
}

void CSymbolTexturedPolygon::Set(const SSymbolTexturedPolygon& properties, SymbolID symbolID)
{
  m_SymbolTexturedPolygon = properties;
  m_SymbolTexturedPolygon.symbolID = symbolID;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
