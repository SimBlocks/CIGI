//Copyright SimBlocks LLC 2016-2026
#include "SymbolSurfaceManager.h"
#include "Symbol.h"
#include "SymbolLib.h"
#include "UtilitiesLib/Logger.h"
#include <sstream>
#include <vector>

using namespace sbio;
using namespace sbio::symbol;

extern sbio::symbol::SSymbolLibSettings g_SymbolLibSettings;

void CSymbolSurfaceManager::AddSymbol(SymbolID symbolID, std::unique_ptr<CSymbol> pSymbol)
{
  // If the symbol pointer is null, log an error and return without adding it to the manager.
  if (pSymbol == nullptr)
  {
    if (g_SymbolLibSettings.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "Ignoring null symbol for ID: " << symbolID.Value() << std::endl;
      g_SymbolLibSettings.pLogger->LogError(ss);
    }

    return;
  }

  // If the symbol's stored ID does not match the provided ID, log an error and return without adding it to the manager.
  if (pSymbol->GetSymbolID() != symbolID)
  {
    if (g_SymbolLibSettings.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "Ignoring symbol with mismatched ID. Map ID: " << symbolID.Value() << ", Symbol ID: " << pSymbol->GetSymbolID().Value() << std::endl;
      g_SymbolLibSettings.pLogger->LogError(ss);
    }

    return;
  }

  // If the symbol ID already exists in the manager, log an error and return without adding it to the manager.
  if (HasSymbol(symbolID))
  {
    if (g_SymbolLibSettings.pLogger != nullptr)
    {
      std::stringstream ss;
      ss << "Ignoring adding duplicate symbol ID: " << symbolID.Value() << std::endl;
      g_SymbolLibSettings.pLogger->LogError(ss);
    }

    return;
  }

  // Add the symbol to the manager, transferring ownership of the symbol pointer to the manager.
  m_Symbols[symbolID] = std::move(pSymbol);
}

void CSymbolSurfaceManager::AddSymbolSurface(SymbolSurfaceID symbolSurfaceID)
{
  m_SymbolSurfaces.insert(symbolSurfaceID);
}

void CSymbolSurfaceManager::ClearSymbols()
{
  m_Symbols.clear();
  m_SymbolSurfaces.clear();
}

void CSymbolSurfaceManager::ClearSymbolSurfaces()
{
  // Clear the symbol surfaces from the manager, which will not affect the symbols since they are owned by the manager and not the symbol surfaces.
  m_SymbolSurfaces.clear();
}

CSymbol* CSymbolSurfaceManager::GetSymbol(SymbolID symbolID) const
{
  auto it = m_Symbols.find(symbolID);

  if (it == m_Symbols.end())
  {
    return nullptr;
  }

  return it->second.get();
}

const CSymbolSurfaceManager::TSymbols& CSymbolSurfaceManager::GetSymbols() const
{
  return m_Symbols;
}

const CSymbolSurfaceManager::TSymbolSurfaces& CSymbolSurfaceManager::GetSymbolSurfaces() const
{
  return m_SymbolSurfaces;
}

bool CSymbolSurfaceManager::HasSymbol(SymbolID symbolID) const
{
  if (m_Symbols.find(symbolID) == m_Symbols.end())
  {
    return false;
  }

  return true;
}

bool CSymbolSurfaceManager::HasSymbolSurface(SymbolSurfaceID symbolSurfaceID) const
{
  if (m_SymbolSurfaces.find(symbolSurfaceID) == m_SymbolSurfaces.end())
  {
    return false;
  }

  return true;
}

void CSymbolSurfaceManager::RemoveSymbolSurface(SymbolSurfaceID symbolSurfaceID)
{
  m_SymbolSurfaces.erase(symbolSurfaceID);
}

void CSymbolSurfaceManager::RemoveSymbol(SymbolID symbolID)
{
  // Get the symbol object for the symbol being removed
  CSymbol* pSymbol = GetSymbol(symbolID);
  if (pSymbol == nullptr)
  {
    return;
  }

  // If the symbol being removed has a parent symbol, remove the symbol from its parent's list of children
  const auto parentSymbolID = pSymbol->GetParentSymbolID();
  CSymbol* pParentSymbol = nullptr;
  if (parentSymbolID)
  {
    pParentSymbol = GetSymbol(*parentSymbolID);
  }

  if (pParentSymbol != nullptr)
  {
    pParentSymbol->RemoveChild(symbolID);
  }

  // Create a vector to store the IDs of any children that are detached from the removed symbol
  std::vector<SymbolID> detachedChildSymbolIDs;

  // Clear the parent symbol ID of any children to detach them from the removed symbol before removing the symbol
  for (auto childSymbolID : pSymbol->GetChildren())
  {
    CSymbol* pChildSymbol = GetSymbol(childSymbolID);
    if (pChildSymbol != nullptr && pChildSymbol->GetParentSymbolID() == symbolID)
    {
      pChildSymbol->SetParentSymbolID(std::nullopt);
      detachedChildSymbolIDs.push_back(childSymbolID);
    }
  }

  // Remove the symbol from the manager
  m_Symbols.erase(symbolID);

  // Update the hidden by ancestor state for any detached children to ensure that their effective visibility is correctly computed
  for (auto childSymbolID : detachedChildSymbolIDs)
  {
    UpdateSymbolTreeHiddenByAncestor(childSymbolID);
  }
}

void CSymbolSurfaceManager::UpdateSymbolTreeHiddenByAncestor(SymbolID symbolID)
{
  CSymbol* pRootSymbol = GetSymbol(symbolID);
  if (pRootSymbol == nullptr)
  {
    return;
  }

  // Recompute the hidden by ancestor state for the root symbol based on its parent's effective visibility
  const auto parentSymbolID = pRootSymbol->GetParentSymbolID();
  CSymbol* pParentSymbol = nullptr;
  if (parentSymbolID)
  {
    pParentSymbol = GetSymbol(*parentSymbolID);
  }

  const bool bRootHiddenByAncestor = pParentSymbol != nullptr && !pParentSymbol->GetEffectiveVisibility();

  // If the root symbol's hidden by ancestor state has changed, update it and force a visibility update
  // to ensure that any change in effective visibility is propagated to its children.
  if (pRootSymbol->IsHiddenByAncestor() != bRootHiddenByAncestor)
  {
    pRootSymbol->SetHiddenByAncestor(bRootHiddenByAncestor);
    pRootSymbol->SetVisible(pRootSymbol->IsVisible(), true);
  }
  else
  {
    pRootSymbol->SetHiddenByAncestor(bRootHiddenByAncestor);
  }

  // Perform a depth-first traversal of the symbol tree starting from the root symbol to update the hidden by ancestor state for all descendant symbols.
  std::vector<SymbolID> symbolsToVisit = {symbolID};
  std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;

  // The root symbol is included in the traversal to ensure that its hidden by ancestor state is updated based on its parent's effective visibility
  // and that any change in effective visibility is propagated to its children.
  while (!symbolsToVisit.empty())
  {
    // Get the next symbol ID to visit from the back of the vector and remove it from the vector.
    const SymbolID currentSymbolID = symbolsToVisit.back();
    symbolsToVisit.pop_back();

    // Skip if the symbol has already been visited to prevent infinite loops in the case of circular references.
    if (!visitedSymbols.insert(currentSymbolID).second)
    {
      continue;
    }

    // Get the symbol object for the current symbol ID
    CSymbol* pCurrentSymbol = GetSymbol(currentSymbolID);
    if (pCurrentSymbol == nullptr)
    {
      continue;
    }

    // Determine the hidden by ancestor state for the children of the current symbol based on the current symbol's effective visibility.
    const bool bChildrenHiddenByAncestor = !pCurrentSymbol->GetEffectiveVisibility();
    const auto children = pCurrentSymbol->GetChildren();

    // Add the children of the current symbol to the vector of symbols to visit and update their hidden by ancestor state based on the current symbol's effective visibility.
    for (auto childSymbolID : children)
    {
      CSymbol* pChildSymbol = GetSymbol(childSymbolID);
      if (pChildSymbol == nullptr)
      {
        continue;
      }

      // If the child symbol's hidden by ancestor state has changed,
      // update it and force a visibility update to ensure that any change in effective visibility is propagated to its children.
      if (pChildSymbol->IsHiddenByAncestor() != bChildrenHiddenByAncestor)
      {
        pChildSymbol->SetHiddenByAncestor(bChildrenHiddenByAncestor);
        pChildSymbol->SetVisible(pChildSymbol->IsVisible(), true);
      }
      else
      {
        pChildSymbol->SetHiddenByAncestor(bChildrenHiddenByAncestor);
      }

      // Add the child symbol ID to the vector of symbols to visit to ensure that the hidden by ancestor state is updated for all descendants of the root symbol.
      symbolsToVisit.push_back(childSymbolID);
    }
  }
}

void CSymbolSurfaceManager::SetSymbolTreeVisible(SymbolID symbolID, bool bVisible)
{
  CSymbol* pSymbol = GetSymbol(symbolID);
  if (pSymbol == nullptr)
  {
    return;
  }

  // Set the visibility of the symbol
  pSymbol->SetVisible(bVisible);

  // Recompute the hidden by ancestor state for the symbol and its descendants based on the change in visibility of the symbol.
  UpdateSymbolTreeHiddenByAncestor(symbolID);
}

void CSymbolSurfaceManager::Update()
{
  for (auto& it : m_Symbols)
  {
    it.second->Update();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
