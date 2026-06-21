//Copyright SimBlocks LLC 2016-2026
#include "SymbolText.h"

using namespace sbio::symbol;

CSymbolText::CSymbolText(SymbolID symbolID) : CSymbolGeometry(ESymbolType::TEXT)
{
  m_SymbolText.symbolID = symbolID;
}

void CSymbolText::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CSymbolText* pSymbol = dynamic_cast<CSymbolText*>(pSymbolGeometry);
  if (pSymbol)
  {
    Set(pSymbol->GetProperties(), m_SymbolText.symbolID);
  }
}

const sbio::symbol::SSymbolTextDefinition& CSymbolText::GetProperties() const
{
  return m_SymbolText;
}

void CSymbolText::Set(SSymbolTextDefinition properties, SymbolID symbolID)
{
  m_SymbolText = properties;
  m_SymbolText.symbolID = symbolID;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
