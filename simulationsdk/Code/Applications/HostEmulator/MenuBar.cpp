//Copyright SimBlocks LLC 2016-2026
#include "MenuBar.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "SetupDialog.h"
#include <wx/aboutdlg.h>
#include <wx/statline.h>

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CMenuBar, wxMenuBar)
wxEND_EVENT_TABLE();
// clang-format on

CMenuBar::CMenuBar() : wxMenuBar()
{
  Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMenuBar::OnQuit));
  Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMenuBar::OnAbout));
  Connect(wxID_SETUP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMenuBar::OnSetup));
  Connect(ID_MENU_LAUNCH_IG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CMenuBar::OnLaunchIG));

  wxMenu* file = new wxMenu;
  wxMenu* test = new wxMenu;
  wxMenu* help = new wxMenu;

  wxMenuItem* setup = new wxMenuItem(file, wxID_SETUP, wxT("Setup"));
  //setup->SetBackgroundColour(wxColor(*wxWHITE));
  file->Append(setup);

  wxMenuItem* exit = new wxMenuItem(file, wxID_EXIT, wxT("Exit"));
  //exit->SetBackgroundColour(wxColor(*wxWHITE));
  file->Append(exit);
  Append(file, wxT("File"));

  wxMenuItem* launchIG = new wxMenuItem(file, ID_MENU_LAUNCH_IG, wxT("Launch IG"));
  //launchIG->SetBackgroundColour(wxColor(*wxWHITE));
  test->Append(launchIG);
  Append(test, wxT("Test"));

  wxMenuItem* about = new wxMenuItem(help, wxID_ABOUT, wxT("About SimBlocks Host Emulator"));
  //about->SetBackgroundColour(wxColor(*wxWHITE));
  help->Append(about);
  Append(help, wxT("Help"));
}

void CMenuBar::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  if (g_pHostEmulatorGuiApp != nullptr && g_pHostEmulatorGuiApp->GetTopWindow() != nullptr)
  {
    g_pHostEmulatorGuiApp->GetTopWindow()->Close(true);
  }
}

void CMenuBar::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxAboutDialogInfo info;
  info.SetName(_("SimBlocks.io Host Emulator"));
  info.SetVersion(_("2026.2"));
  info.SetDescription(_("This program demonstrates Host/IG communication using the Common Image Generator Interface. For more information, please contact sales@simblocks.io"));
  info.SetCopyright(wxT("(C) 2016-2026 SimBlocks LLC"));
  info.SetWebSite("www.simblocks.io");

  wxAboutBox(info);
}

void CMenuBar::OnLaunchIG(wxCommandEvent& event)
{
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    g_pHostEmulatorGuiApp->LaunchImageGenerator();
  }
}

void CMenuBar::OnSetup(wxCommandEvent& event)
{
  SetupDialog dialog(wxT("Setup"));
  dialog.ShowModal();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
