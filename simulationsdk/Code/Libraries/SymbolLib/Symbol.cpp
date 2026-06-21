//Copyright SimBlocks LLC 2016-2026
#include "Symbol.h"
#include "SymbolLib/SymbolGeometry.h"
#include "SymbolCircle.h"
#include "SymbolPolygon.h"
#include "SymbolText.h"
#include "SymbolTexturedCircle.h"
#include "SymbolTexturedPolygon.h"
#include "SymbolGeometryFactory.h"
#include "SymbolLib.h"

using namespace std;
using namespace sbio;
using namespace sbio::symbol;

extern sbio::symbol::SSymbolLibSettings g_SymbolLibSettings;

CSymbol::CSymbol(SymbolID symbolID, sbio::symbol::ESymbolType eSymbolType) : m_SymbolID(symbolID), m_eSymbolType(eSymbolType)
{
  if (g_SymbolLibSettings.pSymbolGeometryFactory != nullptr)
  {
    m_pGeometry = g_SymbolLibSettings.pSymbolGeometryFactory->Create(symbolID, eSymbolType);
  }
}

CSymbol::~CSymbol()
{
}

void CSymbol::CopyFrom(CSymbol* pSymbol, SymbolID symbolID)
{
  this->m_SymbolID = symbolID;
  this->m_Color = pSymbol->GetColor();
  this->m_Position = pSymbol->GetPosition();
  this->m_Scale = pSymbol->GetScale();
  this->m_bVisible = pSymbol->IsVisible();
  this->m_bHiddenByAncestor = false;
  this->m_Rotation = pSymbol->m_Rotation;
  this->m_SymbolSurfaceID = pSymbol->GetSymbolSurfaceID();
  this->m_ParentSymbolID = UnknownSymbolID;
  this->m_Children.clear();
}

bool CSymbol::GetEffectiveVisibility() const
{
  if (!m_bVisible)
  {
    return false;
  }

  if (m_bHiddenByAncestor)
  {
    return false;
  }

  return true;
}

const SColor32& CSymbol::GetColor() const
{
  return m_Color;
}

ESymbolType CSymbol::GetSymbolType() const
{
  return m_eSymbolType;
}

void CSymbol::AddChild(SymbolID symbolID)
{
  m_Children.insert(symbolID);
}

void CSymbol::RemoveChild(SymbolID symbolID)
{
  m_Children.erase(symbolID);
}

std::vector<SymbolID> CSymbol::GetChildren() const
{
  vector<SymbolID> v;
  v.assign(m_Children.begin(), m_Children.end());

  return v;
}

SymbolID CSymbol::GetParentSymbolID() const
{
  return m_ParentSymbolID;
}

void CSymbol::SetParentSymbolID(SymbolID parentSymbolID)
{
  m_ParentSymbolID = parentSymbolID;
}

SymbolID CSymbol::GetSymbolID() const
{
  return m_SymbolID;
}

sbio::symbol::CSymbolGeometry* CSymbol::GetSymbolGeometry() const
{
  return m_pGeometry.get();
}

SymbolSurfaceID CSymbol::GetSymbolSurfaceID() const
{
  return m_SymbolSurfaceID;
}

bool CSymbol::IsTopLevel() const
{
  return m_ParentSymbolID == UnknownSymbolID;
}

bool CSymbol::IsVisible() const
{
  return m_bVisible;
}

bool CSymbol::IsHiddenByAncestor() const
{
  return m_bHiddenByAncestor;
}

void CSymbol::SetHiddenByAncestor(bool bHiddenByAncestor)
{
  m_bHiddenByAncestor = bHiddenByAncestor;
}

sbio::math::Degrees CSymbol::GetRotation() const
{
  return m_Rotation;
}

void CSymbol::SetColor(const SColor32& color)
{
  m_Color = color;
}

void sbio::symbol::CSymbol::SetPosition(const sbio::math::Vec2f& position)
{
  m_Position = position;
}

void sbio::symbol::CSymbol::SetScale(const sbio::math::Vec2f& scale)
{
  m_Scale = scale;
}

sbio::math::Vec2f sbio::symbol::CSymbol::GetScale()
{
  return m_Scale;
}

sbio::math::Vec2f sbio::symbol::CSymbol::GetPosition()
{
  return m_Position;
}

void CSymbol::SetRotation(Degrees rotation)
{
  m_Rotation = rotation;
}

void CSymbol::SetSymbolSurfaceID(SymbolSurfaceID symbolSurfaceID)
{
  m_SymbolSurfaceID = symbolSurfaceID;
}

void CSymbol::SetVisible(bool bVisible, bool bForceChange)
{
  m_bVisible = bVisible;
}

void CSymbol::Update()
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
