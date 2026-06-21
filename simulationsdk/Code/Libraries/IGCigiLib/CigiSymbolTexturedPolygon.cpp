//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbolTexturedPolygon.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::symbol;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CIGCigiSymbolTexturedPolygon::CIGCigiSymbolTexturedPolygon(SymbolID symbolID) : CSymbolTexturedPolygon(symbolID)
{
  SendCreate();
}

void CIGCigiSymbolTexturedPolygon::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CIGCigiSymbolTexturedPolygon* pSymbol = dynamic_cast<CIGCigiSymbolTexturedPolygon*>(pSymbolGeometry);

  Set(pSymbol->GetProperties(), m_SymbolTexturedPolygon.symbolID);
  SendUpdate();
}

void CIGCigiSymbolTexturedPolygon::SendCreate()
{
  SCreateSymbolTexturedPolygonMessage data;
  data.SymbolID = m_SymbolTexturedPolygon.symbolID;
  g_CigiLibGlobals.pEventMessenger->SendCreateSymbolTexturedPolygonMessage(data);
}

void CIGCigiSymbolTexturedPolygon::SendUpdate()
{
  SUpdateSymbolTexturedPolygonMessage data;
  data.SymbolID = m_SymbolTexturedPolygon.symbolID;
  data.Primitive = m_SymbolTexturedPolygon.ePrimitiveType;
  data.TextureID = m_SymbolTexturedPolygon.textureID;
  data.TextureFilterMode = m_SymbolTexturedPolygon.eTextureFilterMode;
  data.TextureWrapMode = m_SymbolTexturedPolygon.eTextureWrapMode;
  data.NumVertices = static_cast<uint32_t>(m_SymbolTexturedPolygon.vertices.size());

  g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolTexturedPolygonMessage(data);

  for (int n = 0; n < m_SymbolTexturedPolygon.vertices.size(); ++n)
  {
    SSetSymbolTexturedPolygonVertexMessage polygonElementData;
    polygonElementData.SymbolID = m_SymbolTexturedPolygon.symbolID;
    polygonElementData.VertexIndex = n;
    polygonElementData.VertexUV.U = m_SymbolTexturedPolygon.vertices[n].uv[0];
    polygonElementData.VertexUV.V = m_SymbolTexturedPolygon.vertices[n].uv[1];
    polygonElementData.TextureCoordinateST.S = m_SymbolTexturedPolygon.vertices[n].textureCoordinateST[0];
    polygonElementData.TextureCoordinateST.T = m_SymbolTexturedPolygon.vertices[n].textureCoordinateST[1];

    g_CigiLibGlobals.pEventMessenger->SendSetSymbolTexturedPolygonVertexMessage(polygonElementData);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
