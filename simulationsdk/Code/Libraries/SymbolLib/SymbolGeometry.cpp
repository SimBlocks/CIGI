//Copyright SimBlocks LLC 2016-2026
#include "SymbolLib/SymbolGeometry.h"

using namespace sbio;
using namespace sbio::symbol;

CSymbolGeometry::CSymbolGeometry(ESymbolType eSymbolType) : m_eSymbolType(eSymbolType)
{
}

CSymbolGeometry::~CSymbolGeometry()
{
}

ESymbolType CSymbolGeometry::GetSymbolType() const
{
  return m_eSymbolType;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
