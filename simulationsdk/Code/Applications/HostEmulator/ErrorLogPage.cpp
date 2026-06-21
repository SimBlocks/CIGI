//Copyright SimBlocks LLC 2016-2026
#include "ErrorLogPage.h"
#include "GlobalHeaders/Globals.h"
#include "HostEmulatorDefinitions.h"
#include "UtilitiesLib/EventDispatcher.h"

using namespace std;
using namespace sbio;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CErrorLogPage, wxPanel)
EVT_BUTTON(ID_ERROR_LOG_CLEAR, CErrorLogPage::OnClearErrorLog)
wxEND_EVENT_TABLE();
// clang-format on

extern SHostEmulatorGlobals g_globals;

CErrorLogPage::CErrorLogPage(wxWindow* pParent) : CNotebookPage(pParent)
{
  wxBoxSizer* hboxTop = new wxBoxSizer(wxVERTICAL);

  m_pTextCtrl = new wxTextCtrl(this, ID_ERROR_PAGE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
  m_pTextCtrl->AppendText("Error Log:\n");
  m_pTextCtrl->SetMinSize(wxSize(200, 200));

  hboxTop->Add(new wxButton(this, ID_ERROR_LOG_CLEAR, wxT("Clear"), wxPoint(5, 20)));
  hboxTop->Add(m_pTextCtrl, wxSizerFlags(1).Expand().Border(wxALL, 10));
  SetSizerAndFit(hboxTop);

  if (g_globals.pEventDispatcher)
  {
    g_globals.pEventDispatcher->RegisterListener<HostCigiEvent>(this);
  }
}

CErrorLogPage::~CErrorLogPage()
{
  if (g_globals.pEventDispatcher)
  {
    g_globals.pEventDispatcher->UnregisterListener<HostCigiEvent>(this);
  }
}

void CErrorLogPage::OnClearErrorLog(wxCommandEvent& event)
{
  m_pTextCtrl->Clear();
  m_sErrorLog.clear();
  counter = 0;
}

void CErrorLogPage::OnHostCigiErrorEvent(const HostCigiErrorEventArgs& args)
{
  counter++;

  if (counter > 30)
  {
    m_pTextCtrl->Clear();
    m_sErrorLog.clear();
    counter = 1;
  }

  m_sErrorLog += args.sError;
  m_pTextCtrl->AppendText("\n");
  m_pTextCtrl->AppendText(args.sError);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
