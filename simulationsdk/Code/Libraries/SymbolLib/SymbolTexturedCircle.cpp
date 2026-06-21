//Copyright SimBlocks LLC 2016-2026
#include "SymbolTexturedCircle.h"

using namespace sbio::symbol;

CSymbolTexturedCircle::CSymbolTexturedCircle(SymbolID symbolID) : CSymbolGeometry(ESymbolType::TEXTURED_CIRCLE)
{
  m_SymbolTexturedCircle.symbolID = symbolID;
}

void CSymbolTexturedCircle::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CSymbolTexturedCircle* pSymbol = dynamic_cast<CSymbolTexturedCircle*>(pSymbolGeometry);
  if (pSymbol)
  {
    Set(pSymbol->GetProperties(), m_SymbolTexturedCircle.symbolID);
  }
}

const sbio::symbol::SSymbolTexturedCircle& CSymbolTexturedCircle::GetProperties() const
{
  return m_SymbolTexturedCircle;
}

void CSymbolTexturedCircle::Set(const SSymbolTexturedCircle& properties, SymbolID symbolID)
{
  m_SymbolTexturedCircle = properties;
  m_SymbolTexturedCircle.symbolID = symbolID;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
