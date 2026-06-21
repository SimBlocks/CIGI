//Copyright SimBlocks LLC 2016-2026
#include "View.h"
#include "CigiLib/CigiTypes.h"
#include "MathLib/Math.h"

using namespace sbio;
using namespace sbio::view;

CView::CView(const sbio::ViewID& viewID) : m_ViewID(viewID)
{
  m_fNear = 1;
  m_fFar = 1000000;
  m_fLeftHalfAngle = -40;
  m_fRightHalfAngle = 40;
  m_fTopHalfAngle = 30;
  m_fBottomHalfAngle = -30;
  m_eProjectionMode = EProjectionMode::PERSPECTIVE;
}

CView::~CView()
{
}

void CView::BringToTop()
{
}

sbio::ViewID CView::GetViewID() const
{
  return m_ViewID;
}

void CView::Reset()
{
  m_bTransformationDirty = false;
  m_bProjectionDirty = false;
}

void CView::SetNearPlane(float fNear)
{
  if (!fequals(m_fNear, fNear))
  {
    m_fNear = fNear;
    m_bProjectionDirty = true;
  }
}

void CView::SetFarPlane(float fFar)
{
  if (!fequals(m_fFar, fFar))
  {
    m_fFar = fFar;
    m_bProjectionDirty = true;
  }
}

void CView::SetLeftHalfAngle(float fLeftHalfAngle)
{
  if (!fequals(m_fLeftHalfAngle, fLeftHalfAngle))
  {
    m_fLeftHalfAngle = fLeftHalfAngle;
    m_bProjectionDirty = true;
  }
}

void CView::SetRightHalfAngle(float fRightHalfAngle)
{
  if (!fequals(m_fRightHalfAngle, fRightHalfAngle))
  {
    m_fRightHalfAngle = fRightHalfAngle;
    m_bProjectionDirty = true;
  }
}

void CView::SetTopHalfAngle(float fTopHalfAngle)
{
  if (!fequals(fTopHalfAngle, fTopHalfAngle))
  {
    m_fTopHalfAngle = fTopHalfAngle;
    m_bProjectionDirty = true;
  }
}

void CView::SetBottomHalfAngle(float fBottomHalfAngle)
{
  if (!fequals(m_fBottomHalfAngle, fBottomHalfAngle))
  {
    m_fBottomHalfAngle = fBottomHalfAngle;
    m_bProjectionDirty = true;
  }
}

void CView::SetPixelReplicationMode(int nWidth, int nHeight)
{
}

void CView::SetProjectionMode(EProjectionMode eProjectionMode)
{
  if (m_eProjectionMode != eProjectionMode)
  {
    m_eProjectionMode = eProjectionMode;
    m_bProjectionDirty = true;
  }
}

void CView::Update()
{
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
