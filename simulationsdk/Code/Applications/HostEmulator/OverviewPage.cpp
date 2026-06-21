//Copyright SimBlocks LLC 2016-2026
#include "OverviewPage.h"
#include "GlobalHeaders/Globals.h"
#include "PacketSelectionPanel.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "HostEmulatorDefinitions.h"

using namespace sbio;

extern SHostEmulatorGlobals g_globals;

COverviewPage::COverviewPage(wxWindow* pParent) : CNotebookPage(pParent)
{
  SetBackgroundColour(wxColor(*wxWHITE));

  //this is the outside box. It has two things inside it, the top and the bottom
  wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

  //this is the top horizontal layout box.
  wxBoxSizer* hboxTop = new wxBoxSizer(wxHORIZONTAL);

  vbox->Add(hboxTop, 3, wxALL | wxEXPAND, 10);

  //this is the bottom horizontal layout box.
  wxBoxSizer* hboxBot = new wxBoxSizer(wxHORIZONTAL);

  vbox->Add(hboxBot, 1, wxALL | wxEXPAND, 0);

  SetSizerAndFit(vbox);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
