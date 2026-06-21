//Copyright SimBlocks LLC 2016-2026
#include "ViewGroup.h"

using namespace sbio;
using namespace sbio::view;

CViewGroup::CViewGroup(sbio::ViewGroupID viewGroupID) : m_ViewGroupID(viewGroupID)
{
}

CViewGroup::~CViewGroup()
{
}

void CViewGroup::AddViewID(sbio::ViewID viewID)
{
  //Do not accept UnknownViewID as a valid viewID. (Things will definitely break)
  if (viewID == UnknownViewID)
  {
    return;
  }

  m_ViewIDs.insert(viewID);

  // If there is no center view, then make this the new center view. A View Group should always have a center, and a different center can always be assigned later
  if (m_CenterViewID == UnknownViewID)
  {
    m_CenterViewID = viewID;
  }
}

void CViewGroup::RemoveViewID(sbio::ViewID viewID)
{
  //Do not accept UnknownViewID as a valid viewID. (Things will definitely break)
  if (viewID == UnknownViewID)
  {
    return;
  }

  m_ViewIDs.erase(viewID);

  //If the center view was removed, one of the remaining views in this group becomes the new center
  if (viewID == m_CenterViewID)
  {
    auto it = m_ViewIDs.begin();
    // If there are no Views left in this ViewGroup, then this ViewGroup no longer has a center
    if (it == m_ViewIDs.end())
    {
      m_CenterViewID = UnknownViewID;
    }
    // Otherwise, the first View in the set becomes the new center
    else
    {
      m_CenterViewID = ViewID(it->Value());
    }
  }
}

sbio::ViewID CViewGroup::GetCenterViewID() const
{
  return m_CenterViewID;
}

sbio::ViewGroupID CViewGroup::GetViewGroupID() const
{
  return m_ViewGroupID;
}

const std::set<sbio::ViewID>& CViewGroup::GetViewIDs() const
{
  return m_ViewIDs;
}

void CViewGroup::Reset()
{
}

void CViewGroup::SetCenterViewID(sbio::ViewID centerViewID)
{
  // Do not accept UnknownViewID as a valid centerViewID. (Things will definitely break)
  if (centerViewID == UnknownViewID)
  {
    return;
  }

  m_CenterViewID = centerViewID;
}

void CViewGroup::Update()
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
