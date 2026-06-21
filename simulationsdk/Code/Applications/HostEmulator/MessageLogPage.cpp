//Copyright SimBlocks LLC 2016-2026
#include "MessageLogPage.h"
#include "GlobalHeaders/Globals.h"
#include "HostEmulatorDefinitions.h"
#include "UtilitiesLib/EventDispatcher.h"

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CMessageLogPage, wxPanel)
EVT_BUTTON(ID_MESSAGE_LOG_CLEAR, CMessageLogPage::OnClearMessageLog)
wxEND_EVENT_TABLE();
// clang-format on

extern SHostEmulatorGlobals g_globals;

CMessageLogPage::CMessageLogPage(wxWindow* pParent) : CNotebookPage(pParent)
{
  wxBoxSizer* hboxTop = new wxBoxSizer(wxVERTICAL);

  m_pTextCtrl = new wxTextCtrl(this, ID_MESSAGE_PAGE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
  m_pTextCtrl->AppendText("Message Log:\n");
  m_pTextCtrl->SetMinSize(wxSize(200, 200));

  hboxTop->Add(new wxButton(this, ID_MESSAGE_LOG_CLEAR, wxT("Clear"), wxPoint(5, 20)));
  hboxTop->Add(m_pTextCtrl, wxSizerFlags(1).Expand().Border(wxALL, 10));
  SetSizerAndFit(hboxTop);

  if (g_globals.pEventDispatcher)
  {
    g_globals.pEventDispatcher->RegisterListener<HostCigiEvent>(this);
  }
}

CMessageLogPage::~CMessageLogPage()
{
  if (g_globals.pEventDispatcher)
  {
    g_globals.pEventDispatcher->UnregisterListener<HostCigiEvent>(this);
  }
}

// Clears the message log when the Clear button is clicked
void CMessageLogPage::OnClearMessageLog(wxCommandEvent& event)
{
  m_pTextCtrl->Clear();
  m_sTextToAppend.clear();
}

// Displays a generic message to the message log
void CMessageLogPage::OnHostCigiMessageEvent(const HostCigiMessageEventArgs& args)
{
  // Append the message to the text to append buffer, which will be flushed to the text control in UpdateGUI
  m_sTextToAppend += "\n";
  m_sTextToAppend += args.sMessage;
}

// Displays a CIGI Packet message to the message log
void CMessageLogPage::OnHostCigiDataMessageEvent(const HostCigiDataMessageEventArgs& args)
{
  m_sTextToAppend += "\n";
  m_sTextToAppend += args.sDataMessage.str();
  m_sTextToAppend += "\n";
}

void CMessageLogPage::UpdateGUI()
{
  // If there is text to append, append it to the text control and clear the buffer
  if (!m_sTextToAppend.empty())
  {
    m_pTextCtrl->AppendText("\n");
    m_pTextCtrl->AppendText(m_sTextToAppend);
    m_pTextCtrl->AppendText("\n");

    m_sTextToAppend.clear();
  }
}

// Clears the message log (allows any code to clear the log, not just the Clear button)
void CMessageLogPage::OnHostCigiClearMessageEvent(const HostCigiClearMessageEventArgs& args)
{
  m_pTextCtrl->Clear();
  m_sTextToAppend.clear();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
