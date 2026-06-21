//Copyright SimBlocks LLC 2016-2026
#include "SymbolCircle.h"

using namespace sbio::symbol;

CSymbolCircle::CSymbolCircle(SymbolID symbolID) : CSymbolGeometry(ESymbolType::CIRCLE)
{
  m_SymbolCircle.symbolID = symbolID;
}

void CSymbolCircle::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CSymbolCircle* pSymbol = dynamic_cast<CSymbolCircle*>(pSymbolGeometry);
  if (pSymbol)
  {
    Set(pSymbol->GetProperties(), m_SymbolCircle.symbolID);
  }
}

const sbio::symbol::SSymbolCircle& CSymbolCircle::GetProperties() const
{
  return m_SymbolCircle;
}

void CSymbolCircle::Set(const sbio::symbol::SSymbolCircle& properties, SymbolID symbolID)
{
  m_SymbolCircle = properties;
  m_SymbolCircle.symbolID = symbolID;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
