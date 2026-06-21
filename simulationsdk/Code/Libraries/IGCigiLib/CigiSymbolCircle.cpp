//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbolCircle.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"

using namespace sbio::symbol;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CIGCigiSymbolCircle::CIGCigiSymbolCircle(SymbolID symbolID) : CSymbolCircle(symbolID)
{
  SendCreate();
}

void CIGCigiSymbolCircle::CopyFrom(CSymbolGeometry* pSymbolGeometry)
{
  CIGCigiSymbolCircle* pSymbolCircle = dynamic_cast<CIGCigiSymbolCircle*>(pSymbolGeometry);

  Set(pSymbolCircle->GetProperties(), m_SymbolCircle.symbolID);
  SendUpdate();
}

void CIGCigiSymbolCircle::SendCreate()
{
  SCreateSymbolCircleMessage data;
  data.SymbolID = m_SymbolCircle.symbolID;
  g_CigiLibGlobals.pEventMessenger->SendCreateSymbolCircleMessage(data);
}

void CIGCigiSymbolCircle::SendUpdate()
{
  if (m_SymbolCircle.eDrawingStyle == EDrawingStyle::FILL)
  {
    SUpdateSymbolCircleFilledMessage data;
    data.SymbolID = m_SymbolCircle.symbolID;
    data.NumElements = static_cast<uint32_t>(m_SymbolCircle.circles.size());

    g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolCircleFilledMessage(data);

    for (auto c : m_SymbolCircle.circles)
    {
      SUpdateSymbolCircleFilledElementMessage circleElementData;
      circleElementData.CenterUV = c.centerUV;
      circleElementData.EndAngle = c.endAngle.Value();
      circleElementData.Radius = c.fRadius;
      circleElementData.InnerRadius = c.fInnerRadius;
      circleElementData.StartAngle = c.startAngle.Value();
      circleElementData.SymbolID = data.SymbolID;
      g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolCircleFilledElementMessage(circleElementData);
    }
  }
  else
  {
    SUpdateSymbolCircleMessage data;
    data.SymbolID = m_SymbolCircle.symbolID;
    data.LineWidth = m_SymbolCircle.fLineWidth;
    data.StipplePattern = m_SymbolCircle.stipplePattern;
    data.StipplePatternLength = m_SymbolCircle.fStipplePatternLength;
    data.NumElements = static_cast<uint32_t>(m_SymbolCircle.circles.size());

    g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolCircleMessage(data);

    for (auto c : m_SymbolCircle.circles)
    {
      SUpdateSymbolCircleElementMessage circleElementData;
      circleElementData.CenterUV = c.centerUV;
      circleElementData.EndAngle = c.endAngle.Value();
      circleElementData.Radius = c.fRadius;
      circleElementData.StartAngle = c.startAngle.Value();
      circleElementData.SymbolID = data.SymbolID;
      g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolCircleElementMessage(circleElementData);
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
