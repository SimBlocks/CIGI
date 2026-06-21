//Copyright SimBlocks LLC 2016-2026
#include "CigiSymbol.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "SymbolLib/SymbolSurfaceManager.h"
#include "SymbolLib/SymbolGeometry.h"
#include "IGCigiLib/IGCigiLib.h"
#include "MathLib/Math.h"
#include "UtilitiesLib/StopWatch.h"
#include "CigiLib/CigiConversions.h"
#include "EngineLib/EngineLib.h"
#include <unordered_set>
#include <vector>

using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::symbol;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiSymbol::CCigiSymbol(SymbolID symbolID, ESymbolType eSymbolType) : CSymbol(symbolID, eSymbolType)
{
  m_pFlashStopWatch = std::make_unique<CStopWatch>();
}

CCigiSymbol::~CCigiSymbol()
{
}

std::unique_ptr<CSymbol> CCigiSymbol::Clone(SymbolID symbolID)
{
  std::unique_ptr<CCigiSymbol> pSymbol = std::make_unique<CCigiSymbol>(symbolID, GetSymbolType());
  pSymbol->CopyFrom(this, symbolID);

  return pSymbol;
}

void CCigiSymbol::CopyFrom(CSymbol* pSymbol, SymbolID symbolID)
{
  CSymbol::CopyFrom(pSymbol, symbolID);

  CCigiSymbol* pCigiSymbol = dynamic_cast<CCigiSymbol*>(pSymbol);
  if (pCigiSymbol == nullptr)
  {
    return;
  }

  m_fFlashPeriod = pCigiSymbol->GetFlashPeriod();
  m_fFlashDutyCycle = pCigiSymbol->GetFlashDutyCycle();

  if (m_pGeometry != nullptr && pSymbol->GetSymbolGeometry() != nullptr)
  {
    m_pGeometry->CopyFrom(pSymbol->GetSymbolGeometry());
  }
}

float CCigiSymbol::GetFlashDutyCycle() const
{
  return m_fFlashDutyCycle;
}

float CCigiSymbol::GetFlashPeriod() const
{
  return m_fFlashPeriod;
}

void CCigiSymbol::RestartFlash()
{
  m_pFlashStopWatch->Stop();
  m_pFlashStopWatch->Start();
}

void CCigiSymbol::SetColor(const SColor32& color)
{
  CSymbol::SetColor(color);

  SSetSymbolColorMessage data;
  data.SymbolID = m_SymbolID;
  data.Color.r = static_cast<float>(color.r) / 255.0f;
  data.Color.g = static_cast<float>(color.g) / 255.0f;
  data.Color.b = static_cast<float>(color.b) / 255.0f;
  data.Color.a = static_cast<float>(color.a) / 255.0f;

  g_CigiLibGlobals.pEventMessenger->SendSetSymbolColorMessage(data);
}

void CCigiSymbol::SetFlash(float fFlashDutyCycle, float fFlashPeriod)
{
  //ignore if values are equal
  if (fequals(m_fFlashDutyCycle, fFlashDutyCycle) && fequals(m_fFlashPeriod, fFlashPeriod))
  {
    return;
  }

  m_fFlashDutyCycle = fFlashDutyCycle;
  m_fFlashPeriod = fFlashPeriod;

  //If a symbol�s flash period or duty cycle is changed, then that symbol�s flash cycle will be restarted.
  RestartFlash();

  if (fequals(m_fFlashDutyCycle, 1))
  {
    //The Flash Duty Cycle Percentage parameter specifies the percentage of each flash cycle that the symbol is visible.
    //If this parameter is set to 100%, then no flashing occurs and the symbol is always visible.
  }
  else
  {
    //If a symbol�s duty cycle is less than 100%, then any descendents (child symbols, grandchildren, etc.) will inherit the symbol�s duty cycle and flash period.
    //The Flash Duty Cycle Percentage and Flash Period attributes of the descendents will be ignored.
    //If a symbol flashes, then any descendents will flash in synchronization with that symbol.

    // Perform a breadth-first traversal of the symbol tree rooted at this symbol, applying flash parameters to each descendant.
    std::vector<CCigiSymbol*> symbolsToVisit = {this};
    std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;

    // The root symbol is included in the traversal to ensure that its flash parameters are applied to itself and that its flash cycle is restarted.
    while (!symbolsToVisit.empty())
    {
      // Get the next symbol to visit from the back of the vector and remove it from the vector.
      CCigiSymbol* pCurrentSymbol = symbolsToVisit.back();
      symbolsToVisit.pop_back();

      if (pCurrentSymbol == nullptr)
      {
        continue;
      }

      // Skip if the symbol has already been visited to prevent infinite loops in the case of circular references.
      if (!visitedSymbols.insert(pCurrentSymbol->GetSymbolID()).second)
      {
        continue;
      }

      // Apply flash parameters to the current symbol if it is not the root symbol.
      // The root symbol has already had its flash parameters updated and its flash cycle restarted.
      if (pCurrentSymbol != this)
      {
        pCurrentSymbol->m_fFlashDutyCycle = m_fFlashDutyCycle;
        pCurrentSymbol->m_fFlashPeriod = m_fFlashPeriod;
        pCurrentSymbol->RestartFlash();
      }

      // Add the children of the current symbol to the vector of symbols to visit.
      for (auto childID : pCurrentSymbol->m_Children)
      {
        // Get the symbol object for the child symbol ID and add it to the vector of symbols to visit if it is a CCigiSymbol.
        CCigiSymbol* pChildSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(childID));
        if (pChildSymbol == nullptr)
        {
          continue;
        }

        symbolsToVisit.push_back(pChildSymbol);
      }
    }
  }
}

void CCigiSymbol::SetRotation(Degrees fRotation)
{
  CSymbol::SetRotation(fRotation);

  if (IsTopLevel())
  {
    SSetTopLevelSymbolTransformMessage data;
    data.SymbolID = m_SymbolID;
    data.Rotation = fRotation;

    g_CigiLibGlobals.pEventMessenger->SendSetTopLevelSymbolTransformMessage(data);
  }
  else
  {
    SSetChildSymbolTransformMessage data;
    data.SymbolID = m_SymbolID;
    data.Rotation = fRotation;

    g_CigiLibGlobals.pEventMessenger->SendSetChildSymbolTransformMessage(data);
  }
}

void CCigiSymbol::SetSymbolSurfaceID(SymbolSurfaceID symbolSurfaceID)
{
  if (m_SymbolSurfaceID == symbolSurfaceID)
  {
    return;
  }

  CSymbol::SetSymbolSurfaceID(symbolSurfaceID);

  SSetSymbolSurfaceMessage data;
  data.SymbolID = m_SymbolID;
  data.SurfaceID = symbolSurfaceID;

  g_CigiLibGlobals.pEventMessenger->SendSetSymbolSurfaceMessage(data);
}

void CCigiSymbol::SetVisible(bool bVisible, bool bForceChange)
{
  const bool bCurrentVisible = GetEffectiveVisibility();

  CSymbol::SetVisible(bVisible);

  const bool bNewVisible = GetEffectiveVisibility();
  if (bCurrentVisible != bNewVisible || bForceChange)
  {
    SSetSymbolVisibleMessage data;
    data.SymbolID = m_SymbolID;
    data.Visible = bNewVisible;
    g_CigiLibGlobals.pEventMessenger->SendSetSymbolVisibleMessage(data);
  }
}

void CCigiSymbol::Update()
{
  CSymbol::Update();

  if (m_fFlashPeriod > 0)
  {
    if (m_pFlashStopWatch->GetElapsedSeconds() > m_fFlashPeriod)
    {
      SetVisible(true);

      m_pFlashStopWatch->Reset();
      m_pFlashStopWatch->Start();
    }
    else if (m_pFlashStopWatch->GetElapsedSeconds() > m_fFlashDutyCycle * m_fFlashPeriod)
    {
      SetVisible(false);
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
