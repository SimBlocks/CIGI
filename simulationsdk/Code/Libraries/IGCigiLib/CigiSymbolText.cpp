//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbolText.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

#ifndef _WIN32
#include "UtilitiesLib/LinuxUtilities.h"
#endif

using namespace std;
using namespace sbio::symbol;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CIGCigiSymbolText::CIGCigiSymbolText(SymbolID symbolID) : CSymbolText(symbolID)
{
  SendCreate();
}

void CIGCigiSymbolText::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CIGCigiSymbolText* pSymbol = dynamic_cast<CIGCigiSymbolText*>(pSymbolGeometry);
  if (pSymbol == nullptr)
  {
    return;
  }

  Set(pSymbol->GetProperties(), m_SymbolText.symbolID);
  SendUpdate();
}

void CIGCigiSymbolText::SendCreate()
{
  SCreateSymbolTextMessage data;
  data.SymbolID = m_SymbolText.symbolID;
  g_CigiLibGlobals.pEventMessenger->SendCreateSymbolTextMessage(data);
}

void CIGCigiSymbolText::SendUpdate()
{
  string sText = m_SymbolText.sText;
  if (m_SymbolText.eTextOrientation == ETextOrientation::RIGHT_TO_LEFT || m_SymbolText.eTextOrientation == ETextOrientation::BOTTOM_TO_TOP)
  {
    std::reverse(sText.begin(), sText.end());
  }

  SUpdateSymbolTextMessage data;
  data.SymbolID = m_SymbolText.symbolID;
  data.FontID = m_SymbolText.fontID;
  data.FontSize = m_SymbolText.fFontSize;
  strcpy_s(data.Text, sizeof(data.Text), sText.c_str());

  data.Orientation = m_SymbolText.eTextOrientation;
  data.Alignment = m_SymbolText.eTextAlignment;

  g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolTextMessage(data);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
