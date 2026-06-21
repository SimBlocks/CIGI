//Copyright SimBlocks LLC 2016-2026
/**
 * @file OverviewPage.h
 * @brief Declares the COverviewPage class for the overview page in the HostEmulator application.
 *
 * Provides the COverviewPage class for displaying an overview panel in the HostEmulator GUI.
 * Inherits from CNotebookPage for notebook integration and IHostCigiEventListener for event handling.
 * Integrates with wxWidgets for GUI management and SimBlocks SDK for simulation interoperability.
 *
 * @see COverviewPage
 * @see CNotebookPage
 * @see sbio::cigi::host::IHostCigiEventListener
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_OVERVIEW_PAGE_H
#define SIMBLOCKS_HOST_EMULATOR_OVERVIEW_PAGE_H

#include "wxWidgetsUtilities/NotebookPage.h"
#include "HostCigiLib/HostCigiEvent.h"
#include <wx/wx.h>

/**
 * @brief Overview page for displaying summary information in the HostEmulator GUI.
 *
 * Intended for future extension to display host emulator status and summary data.
 */
class COverviewPage : public CNotebookPage
{
public:
  /**
   * @brief Constructs the overview page.
   * @param pParent Parent window pointer.
   */
  COverviewPage(wxWindow* pParent);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
