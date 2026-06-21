//Copyright SimBlocks LLC 2016-2026
#include <iostream>
#include <fstream>
#include <string>
#include "ViewManager.h"
#include "View.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "ViewLib/View.h"
#include "ViewLib/IViewCreator.h"
#include "ViewLib.h"
#include "ViewGroup.h"

using namespace sbio;
using namespace sbio::view;
using namespace std;

extern SViewLibSettings g_ViewLibSettings;

void CViewManager::AddView(std::unique_ptr<CView> pView)
{
  // Do not accept UnknownViewID as a valid viewID. (Things will definitely break)
  if (pView->GetViewID() == UnknownViewID)
  {
    return;
  }

  m_Views.insert(std::make_pair(pView->GetViewID().Value(), std::move(pView)));
}

void CViewManager::AddViewGroup(std::unique_ptr<CViewGroup> pViewGroup)
{
  // Do not accept UnknownViewGroupID as a valid viewGroupID. (Things will definitely break)
  if (pViewGroup->GetViewGroupID() == UnknownViewGroupID)
  {
    return;
  }

  m_ViewGroups[pViewGroup->GetViewGroupID()] = std::move(pViewGroup);
}

CView* CViewManager::GetView(ViewID viewID) const
{
  auto itView = m_Views.find(viewID);
  if (itView == m_Views.end())
  {
    return nullptr;
  }

  return itView->second.get();
}

CViewGroup* CViewManager::GetViewGroup(sbio::ViewGroupID viewGroupID) const
{
  auto itView = m_ViewGroups.find(viewGroupID);
  if (itView == m_ViewGroups.end())
  {
    return nullptr;
  }

  return itView->second.get();
}

bool CViewManager::HasView(ViewID viewID) const
{
  if (m_Views.find(viewID) == m_Views.end())
  {
    return false;
  }

  return true;
}

bool CViewManager::RemoveView(ViewID viewID)
{
  // Do not accept UnknownViewID as a valid viewID.
  if (viewID == UnknownViewID)
  {
    return false;
  }

  auto itView = m_Views.find(viewID);
  if (itView == m_Views.end())
  {
    return false;
  }

  for (auto& it : m_ViewGroups)
  {
    it.second->RemoveViewID(viewID);
  }

  m_Views.erase(itView);
  return true;
}

void CViewManager::Reset()
{
  //don't clear view for now. just reset each one
  for (auto& v : m_Views)
  {
    v.second->Reset();
  }
}

void CViewManager::Update()
{
  for (auto& it : m_ViewGroups)
  {
    it.second->Update();
  }

  for (auto& it : m_Views)
  {
    it.second->Update();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
