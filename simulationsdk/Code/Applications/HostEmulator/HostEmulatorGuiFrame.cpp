//Copyright SimBlocks LLC 2016-2026
#include "HostEmulatorGuiFrame.h"
#include "ErrorLogPage.h"
#include "HostEmulatorDefinitions.h"
#include "MenuBar.h"
#include "MessageLogPage.h"
#include "OverviewPage.h"
#include "PacketsPage.h"
#include "ScriptsPage.h"
#include "SetupDialog.h"
#include <wx/artprov.h>

// clang-format off
wxBEGIN_EVENT_TABLE(CHostEmulatorGuiFrame, wxFrame)
EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, CHostEmulatorGuiFrame::OnPageChanged)
wxEND_EVENT_TABLE();
// clang-format on

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

CHostEmulatorGuiFrame::CHostEmulatorGuiFrame() : wxFrame(NULL, wxID_ANY, "SimBlocks.io Host Emulator", wxDefaultPosition, wxSize(800, 800))
{
  Bind(wxEVT_CLOSE_WINDOW, &CHostEmulatorGuiFrame::OnClose, this);

  //creates and sets the menu bar for the window.
  SetMenuBar(new CMenuBar());

  //make the status bar
  wxStatusBar* statusbar = new wxStatusBar(this, wxID_ANY, wxST_SIZEGRIP);
  statusbar->SetFieldsCount(4);
  statusbar->SetStatusText("Database: ", 0);
  statusbar->SetStatusText("IG Mode:", 1);
  statusbar->SetStatusText("CIGI Version:", 2);
  statusbar->SetStatusText("FPS: 0", 3);
  this->SetStatusBar(statusbar);

  m_pNotebook = new wxNotebook(this, ID_NOTEBOOK);
  m_pNotebook->SetBackgroundColour(*(wxWHITE));

  CPacketsPage* pPacketsPage = new CPacketsPage(m_pNotebook);
  m_pNotebook->AddPage(pPacketsPage, "Packets");

  CScriptsPage* pScriptsPage = new CScriptsPage(m_pNotebook);
  m_pNotebook->AddPage(pScriptsPage, "Scripts");

  m_pMessageLogPage = new CMessageLogPage(m_pNotebook);
  m_pNotebook->AddPage(m_pMessageLogPage, "Message Log");

  CErrorLogPage* pErrorLogPage = new CErrorLogPage(m_pNotebook);
  m_pNotebook->AddPage(pErrorLogPage, "Error Log");

  Centre();
}

wxFlexGridSizer* CHostEmulatorGuiFrame::MakeButtonBar(wxFrame* parent)
{
  wxFlexGridSizer* flexGridSizer = new wxFlexGridSizer(1, 6, 0, 0);
  // the buttons are play/pause, stop, reset host, reset IG, reset all, record

  flexGridSizer->AddGrowableCol(0);
  flexGridSizer->AddGrowableCol(1);
  flexGridSizer->AddGrowableCol(2);
  flexGridSizer->AddGrowableCol(3);
  flexGridSizer->AddGrowableCol(4);
  flexGridSizer->AddGrowableCol(5);

  wxButton* playPause = new wxButton(parent, wxID_ANY, wxT("Play/Pause"));
  playPause->SetLabel(wxT("Play/Pause"));
  playPause->SetBackgroundColour(wxColor(*wxWHITE));
  playPause->SetSize(wxSize(16, 16));
  flexGridSizer->Add(playPause, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxButton* stop = new wxButton();
  stop->SetLabel(wxT("Stop"));
  stop->SetSize(wxSize(16, 16));
  flexGridSizer->Add(stop, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  flexGridSizer->Fit(parent);
  flexGridSizer->SetSizeHints(parent);

  return flexGridSizer;
}

void CHostEmulatorGuiFrame::OnClose(wxCloseEvent& event)
{
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    g_pHostEmulatorGuiApp->Uninitialize();
  }

  Destroy();
}

void CHostEmulatorGuiFrame::OnOk(wxCommandEvent& event)
{
  wxButton* okButton;
  okButton = static_cast<wxButton*>(event.GetEventObject());
  SetupDialog* dialog = static_cast<SetupDialog*>(okButton->GetParent());
  wxString message = wxString::Format(wxT("Frame ig to host port: %ld"), dialog->GetReceivePort());
  wxMessageBox(message);

  m_hostToIGPort = dialog->GetReceivePort();
  m_igToHostPort = dialog->GetLocalReceivePort();

  // Set m_ipAddress to the value in ipAddress
  wxString ipAddress = dialog->GetIPAddress();
  wxString cigiVersion = dialog->GetVersionID();

  if (cigiVersion == "3.3")
  {
    m_versionID = sbio::cigi::ECigiVersion::VERSION_3_3;
  }
  else if (cigiVersion == "4.0")
  {
    m_versionID = sbio::cigi::ECigiVersion::VERSION_4_0;
  }

  m_bigEndianByteOrder = dialog->GetBigEndianByteOrder();

  dialog->Destroy();
}

void CHostEmulatorGuiFrame::OnPageChanged(wxNotebookEvent& event)
{
  CNotebookPage* pPage = (CNotebookPage*)m_pNotebook->GetPage(event.GetSelection());
  pPage->OnSelected();
}

void CHostEmulatorGuiFrame::UpdateGUI()
{
  // Update Message Log page
  if (m_pMessageLogPage)
  {
    m_pMessageLogPage->UpdateGUI();
  }

  // Update Scripts page
  CScriptsPage* pScriptsPage = dynamic_cast<CScriptsPage*>(m_pNotebook->GetPage(1));
  if (pScriptsPage)
  {
    pScriptsPage->UpdateGUI();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
