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
  m_OwnColor = GetColor();
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
  m_OwnColor = GetColor();
  m_bInheritColor = false;

  CCigiSymbol* pCigiSymbol = dynamic_cast<CCigiSymbol*>(pSymbol);
  if (pCigiSymbol == nullptr)
  {
    return;
  }

  m_fFlashPeriod = pCigiSymbol->GetFlashPeriod();
  m_fFlashDutyCycle = pCigiSymbol->GetFlashDutyCycle();
  RestartFlash();

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

uint8_t CCigiSymbol::GetLayerID() const
{
  return m_LayerID;
}

void CCigiSymbol::SetLayerID(uint8_t layerID)
{
  m_LayerID = layerID;
}

void CCigiSymbol::RestartFlash()
{
  m_pFlashStopWatch->Reset();
  m_pFlashStopWatch->Start();
  RefreshFlashVisibility();
}

void CCigiSymbol::SetColor(const SColor32& color)
{
  m_OwnColor = color;
  UpdateEffectiveColor();
}

void CCigiSymbol::SetInheritColor(bool inheritColor)
{
  m_bInheritColor = inheritColor;
  UpdateEffectiveColor();
}

void CCigiSymbol::UpdateEffectiveColor()
{
  std::vector<CCigiSymbol*> symbolsToVisit = {this};
  std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;

  // Perform a depth-first traversal of the symbol hierarchy to update colors for this symbol and all inheriting descendants.
  while (!symbolsToVisit.empty())
  {
    // Pop the next symbol to visit from the stack.
    CCigiSymbol* pSymbol = symbolsToVisit.back();
    symbolsToVisit.pop_back();

    // Skip symbols that have already been visited to avoid infinite loops in case of circular references.
    if (!visitedSymbols.insert(pSymbol->GetSymbolID()).second)
    {
      continue;
    }

    // Determine the effective color for this symbol, considering inheritance.
    SColor32 color = pSymbol->m_OwnColor;
    if (pSymbol->m_bInheritColor && g_CigiLibGlobals.pSymbolSurfaceManager != nullptr)
    {
      const auto parentSymbolID = pSymbol->GetParentSymbolID();
      CSymbol* pParent = nullptr;

      if (parentSymbolID)
      {
        pParent = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(*parentSymbolID);
      }

      if (pParent != nullptr)
      {
        color = pParent->GetColor();
      }
    }

    pSymbol->CSymbol::SetColor(color);

    // Notify the image generator of the color change.
    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      SSetSymbolColorMessage data;
      data.SymbolID = pSymbol->GetSymbolID();
      data.Color.r = static_cast<float>(color.r) / 255.0f;
      data.Color.g = static_cast<float>(color.g) / 255.0f;
      data.Color.b = static_cast<float>(color.b) / 255.0f;
      data.Color.a = static_cast<float>(color.a) / 255.0f;
      g_CigiLibGlobals.pEventMessenger->SendSetSymbolColorMessage(data);
    }

    // Add children that inherit color to the visit list.
    if (g_CigiLibGlobals.pSymbolSurfaceManager != nullptr)
    {
      for (SymbolID childID : pSymbol->GetChildren())
      {
        auto* pChild = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(childID));
        if (pChild != nullptr && pChild->m_bInheritColor && pChild->GetParentSymbolID() == pSymbol->GetSymbolID())
        {
          symbolsToVisit.push_back(pChild);
        }
      }
    }
  }
}

void CCigiSymbol::SetFlash(float fFlashDutyCycle, float fFlashPeriod)
{
  // Preserve the phase when settings are unchanged, but refresh inherited visibility.
  if (fequals(m_fFlashDutyCycle, fFlashDutyCycle) && fequals(m_fFlashPeriod, fFlashPeriod))
  {
    RefreshFlashVisibility();
    return;
  }

  m_fFlashDutyCycle = fFlashDutyCycle;
  m_fFlashPeriod = fFlashPeriod;

  // If a symbol�s flash period or duty cycle is changed, then that symbol�s flash cycle will be restarted.
  RestartFlash();
}

CCigiSymbol* CCigiSymbol::GetFlashSource()
{
  CCigiSymbol* pSource = this;
  CSymbol* pAncestor = this;
  std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;
  while (pAncestor != nullptr && visitedSymbols.insert(pAncestor->GetSymbolID()).second)
  {
    auto* pCigiAncestor = dynamic_cast<CCigiSymbol*>(pAncestor);
    if (pCigiAncestor != nullptr && pCigiAncestor->m_fFlashPeriod > 0 && pCigiAncestor->m_fFlashDutyCycle < 1)
    {
      // The highest flashing ancestor controls the entire descendant branch.
      pSource = pCigiAncestor;
    }
    if (g_CigiLibGlobals.pSymbolSurfaceManager == nullptr)
    {
      break;
    }

    const auto parentSymbolID = pAncestor->GetParentSymbolID();
    if (parentSymbolID)
    {
      pAncestor = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(*parentSymbolID);
    }
    else
    {
      pAncestor = nullptr;
    }
  }
  return pSource;
}

void CCigiSymbol::UpdateFlashVisibility()
{
  CCigiSymbol* pSource = GetFlashSource();
  if (pSource->m_fFlashPeriod > 0)
  {
    double elapsed = pSource->m_pFlashStopWatch->GetElapsedSeconds();
    if (elapsed >= pSource->m_fFlashPeriod)
    {
      pSource->m_pFlashStopWatch->Reset();
      pSource->m_pFlashStopWatch->Start();
      elapsed = 0;
    }
    SetFlashVisible(elapsed < pSource->m_fFlashDutyCycle * pSource->m_fFlashPeriod);
  }
  else
  {
    SetFlashVisible(true);
  }
}

void CCigiSymbol::RefreshFlashVisibility()
{
  std::vector<CCigiSymbol*> symbolsToVisit = {this};
  std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;
  while (!symbolsToVisit.empty())
  {
    CCigiSymbol* pSymbol = symbolsToVisit.back();
    symbolsToVisit.pop_back();
    if (!visitedSymbols.insert(pSymbol->GetSymbolID()).second)
    {
      continue;
    }
    pSymbol->UpdateFlashVisibility();
    if (g_CigiLibGlobals.pSymbolSurfaceManager != nullptr)
    {
      for (SymbolID childID : pSymbol->GetChildren())
      {
        auto* pChild = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(childID));
        if (pChild != nullptr && pChild->GetParentSymbolID() == pSymbol->GetSymbolID())
        {
          symbolsToVisit.push_back(pChild);
        }
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
  if (HasSymbolSurfaceID() && m_SymbolSurfaceID == symbolSurfaceID)
  {
    return;
  }

  CSymbol::SetSymbolSurfaceID(symbolSurfaceID);

  SSetSymbolSurfaceMessage data;
  data.SymbolID = m_SymbolID;
  data.SurfaceID = symbolSurfaceID;

  g_CigiLibGlobals.pEventMessenger->SendSetSymbolSurfaceMessage(data);
}

void CCigiSymbol::ClearSymbolSurfaceID()
{
  if (!HasSymbolSurfaceID())
  {
    return;
  }

  CSymbol::ClearSymbolSurfaceID();

  SClearSymbolSurfaceMessage data;
  data.SymbolID = m_SymbolID;
  g_CigiLibGlobals.pEventMessenger->SendClearSymbolSurfaceMessage(data);
}

void CCigiSymbol::SetVisible(bool bVisible, bool bForceChange)
{
  const bool bCurrentVisible = GetEffectiveVisibility() && m_bFlashVisible;

  CSymbol::SetVisible(bVisible);

  const bool bNewVisible = GetEffectiveVisibility() && m_bFlashVisible;
  if (bCurrentVisible != bNewVisible || bForceChange)
  {
    SSetSymbolVisibleMessage data;
    data.SymbolID = m_SymbolID;
    data.Visible = bNewVisible;
    g_CigiLibGlobals.pEventMessenger->SendSetSymbolVisibleMessage(data);
  }
}

void CCigiSymbol::SetFlashVisible(bool bVisible)
{
  const bool bCurrentVisible = GetEffectiveVisibility() && m_bFlashVisible;
  m_bFlashVisible = bVisible;

  // If the flash visibility has changed, update the host's visibility state to reflect the new effective visibility.
  if (bCurrentVisible != (GetEffectiveVisibility() && m_bFlashVisible))
  {
    SetVisible(IsVisible(), true);
  }
}

void CCigiSymbol::Update()
{
  CSymbol::Update();
  UpdateFlashVisibility();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
