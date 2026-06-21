//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_GUI_NOTEBOOK_PAGE_H
#define SIMBLOCKS_HOST_EMULATOR_GUI_NOTEBOOK_PAGE_H

#include "BasePacketPanel.h"

/**
 * @brief Defines the base interface used by HostEmulator notebook pages.
 */
class CNotebookPage : public CBasePacketPanel
{
public:
  /**
   * @brief Constructs a `CNotebookPage` instance.
   * @param pParent Parent window or sizer that owns the created control.
   */
  CNotebookPage(wxWindow* pParent);

  virtual void OnSelected();
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
