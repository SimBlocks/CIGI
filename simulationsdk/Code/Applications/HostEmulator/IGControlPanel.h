//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_IG_CONTROL_PANEL_H
#define SIMBLOCKS_IG_CONTROL_PANEL_H

#include <wx/wx.h>

/**
 * @brief Provides the cigcontrol panel API.
 */
class CIGControlPanel : public wxPanel
{
public:
  /**
   * @brief Constructs a `CIGControlPanel` instance.
   * @param pParent Parent window or sizer that owns the created control.
   * @param point Point value.
   * @param size Count or size value.
   */
  CIGControlPanel(wxPanel* pParent, const wxPoint& point, const wxSize& size);
};

#endif

//Usage of this software is prohibited except through a Software Licensing Agreement with SimBlocks LLC.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
