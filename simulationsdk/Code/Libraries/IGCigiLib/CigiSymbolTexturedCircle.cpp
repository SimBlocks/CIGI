//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbolTexturedCircle.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::symbol;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CIGCigiSymbolTexturedCircle::CIGCigiSymbolTexturedCircle(SymbolID symbolID) : CSymbolTexturedCircle(symbolID)
{
  SendCreate();
}

void CIGCigiSymbolTexturedCircle::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CIGCigiSymbolTexturedCircle* pSymbol = dynamic_cast<CIGCigiSymbolTexturedCircle*>(pSymbolGeometry);

  Set(pSymbol->GetProperties(), m_SymbolTexturedCircle.symbolID);
  SendUpdate();
}

void CIGCigiSymbolTexturedCircle::SendCreate()
{
  SCreateSymbolTexturedCircleMessage data;
  data.SymbolID = m_SymbolTexturedCircle.symbolID;
  g_CigiLibGlobals.pEventMessenger->SendCreateSymbolTexturedCircleMessage(data);
}

void CIGCigiSymbolTexturedCircle::SendUpdate()
{
  SUpdateSymbolTexturedCircleMessage data;
  data.SymbolID = m_SymbolTexturedCircle.symbolID;
  data.TextureID = m_SymbolTexturedCircle.textureID;
  data.TextureFilterMode = m_SymbolTexturedCircle.eTextureFilter;
  data.TextureWrapMode = m_SymbolTexturedCircle.eTextureWrap;
  data.NumCircles = static_cast<uint32_t>(m_SymbolTexturedCircle.circles.size());
  g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolTexturedCircleMessage(data);

  int n = 0;
  for (auto circle : m_SymbolTexturedCircle.circles)
  {
    SUpdateTexturedCircleMessage updateTexturedCircleMessage;
    updateTexturedCircleMessage.CenterTextureUV.U = circle.centerTextureST[0];
    updateTexturedCircleMessage.CenterTextureUV.V = circle.centerTextureST[1];
    updateTexturedCircleMessage.CenterUV.U = circle.centerUV[0];
    updateTexturedCircleMessage.CenterUV.V = circle.centerUV[1];
    updateTexturedCircleMessage.CircleIndex = n;
    updateTexturedCircleMessage.EndAngle = circle.endAngle.Value();
    updateTexturedCircleMessage.Radius = circle.fRadius;
    updateTexturedCircleMessage.StartAngle = circle.startAngle.Value();
    updateTexturedCircleMessage.SymbolID = m_SymbolTexturedCircle.symbolID;
    updateTexturedCircleMessage.TextureMapRadius = circle.fTextureMapRadius;
    updateTexturedCircleMessage.TextureMapRotation = circle.fTextureMapRotation;
    g_CigiLibGlobals.pEventMessenger->SendUpdateTexturedCircleMessage(updateTexturedCircleMessage);

    ++n;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
