//Copyright SimBlocks LLC 2016-2026
/**
 * @file OverviewPage.h
 * @brief Declares the COverviewPage class for the overview page in the HostEmulator application.
 *
 * Provides the COverviewPage class for displaying an overview panel in the HostEmulator GUI.
 * Inherits from CNotebookPage for notebook integration; does not register an event listener.
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
 * @brief Empty overview-page layout reserved for future summary controls.
 *
 * Creates upper and lower layout regions only; does not currently display or query host status.
 */
class COverviewPage : public CNotebookPage
{
public:
  /**
   * @brief Constructs the overview page.
   * @param pParent Parent window owning this page through the wxWidgets hierarchy.
   */
  COverviewPage(wxWindow* pParent);

private:
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
