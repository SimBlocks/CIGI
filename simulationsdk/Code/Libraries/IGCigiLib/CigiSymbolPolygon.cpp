//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbolPolygon.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::symbol;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CIGCigiSymbolPolygon::CIGCigiSymbolPolygon(SymbolID symbolID) : CSymbolPolygon(symbolID)
{
  SendCreate();
}

void CIGCigiSymbolPolygon::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CIGCigiSymbolPolygon* pSymbolPolygon = dynamic_cast<CIGCigiSymbolPolygon*>(pSymbolGeometry);

  Set(pSymbolPolygon->GetProperties(), m_SymbolPolygon.symbolID);
  SendUpdate();
}

void CIGCigiSymbolPolygon::SendCreate()
{
  SCreateSymbolPolygonMessage data;
  data.SymbolID = m_SymbolPolygon.symbolID;
  g_CigiLibGlobals.pEventMessenger->SendCreateSymbolPolygonMessage(data);
}

void CIGCigiSymbolPolygon::SendUpdate()
{
  SUpdateSymbolPolygonMessage data;
  data.SymbolID = m_SymbolPolygon.symbolID;
  data.StipplePattern = m_SymbolPolygon.nStipplePattern;
  data.Primitive = m_SymbolPolygon.ePrimitiveType;
  data.LineWidth = m_SymbolPolygon.fLineWidth;
  data.StipplePatternLength = m_SymbolPolygon.fStipplePatternLength;
  data.NumVertices = static_cast<uint32_t>(m_SymbolPolygon.vertices.size());

  g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolPolygonMessage(data);

  for (int n = 0; n < m_SymbolPolygon.vertices.size(); ++n)
  {
    SSetSymbolPolygonVertexMessage polygonElementData;
    polygonElementData.SymbolID = m_SymbolPolygon.symbolID;
    polygonElementData.VertexIndex = n;
    polygonElementData.X = m_SymbolPolygon.vertices[n][0];
    polygonElementData.Y = m_SymbolPolygon.vertices[n][1];

    g_CigiLibGlobals.pEventMessenger->SendSetSymbolPolygonVertexMessage(polygonElementData);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
