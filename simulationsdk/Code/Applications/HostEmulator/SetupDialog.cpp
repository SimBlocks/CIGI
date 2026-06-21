//Copyright SimBlocks LLC 2016-2026
#include "SetupDialog.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostCigiEvent.h"

using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(SetupDialog, wxDialog)
  EVT_BUTTON(wxID_OK, SetupDialog::OnOk)
wxEND_EVENT_TABLE();
// clang-format on

SetupDialog::SetupDialog(const wxString& title) : wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(300, 350))
{
  SHostSetupOptions options;
  SHostSessionSetupOptions sessionOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr)
    {
      options = pHost->GetHostSetupOptions();
      sessionOptions = GetSessionSetupOptions(options, g_pHostEmulatorGuiApp->GetSelectedSessionID());
    }
  }
  else
  {
    sessionOptions = GetSessionSetupOptions(options, sbio::SessionID(0));
  }

  panel = new wxPanel(this, -1);

  hbox = new wxBoxSizer(wxHORIZONTAL);
  vbox = new wxBoxSizer(wxVERTICAL);

  text1 = new wxStaticText(panel, wxID_ANY, wxT("Image Generator"), wxPoint(15, 5), wxDefaultSize);

  text2 = new wxStaticText(panel, wxID_ANY, wxT("IP Address"), wxPoint(15, 30));
  wxArrayString arraystrIPAddress(11, ipAddressFilter);
  txtvldIPAddress->SetIncludes(arraystrIPAddress);
  ipAddressControl = new wxTextCtrl(panel, wxID_ANY, sessionOptions.igIPAddress, wxPoint(120, 30), wxDefaultSize, 0, *txtvldIPAddress);
  ipAddressControl->SetToolTip("Four [0-255] decimal numbers separated by periods\nFormat:A.B.C.D\nEx.: 127.0.0.1");

  receivePortControl = new wxTextCtrl(panel, wxID_ANY, wxString::Format("%d", sessionOptions.hostToIGPort), wxPoint(120, 55));
  text3 = new wxStaticText(panel, wxID_ANY, wxT("Receive Port"), wxPoint(15, 55));

  localReceivePortControl = new wxTextCtrl(panel, wxID_ANY, wxString::Format("%d", sessionOptions.igToHostPort), wxPoint(120, 80));
  text4 = new wxStaticText(panel, wxID_ANY, wxT("Local Receive Port"), wxPoint(15, 80));

  wxString sVersion;
  if (options.eCigiVersion == ECigiVersion::VERSION_3_3)
  {
    sVersion = "3.3";
  }
  else if (options.eCigiVersion == ECigiVersion::VERSION_4_0)
  {
    sVersion = "4.0";
  }
  versionIDControl = new wxTextCtrl(panel, wxID_ANY, sVersion, wxPoint(120, 105), wxDefaultSize, wxTE_READONLY);
  text5 = new wxStaticText(panel, wxID_ANY, wxT("CIGI Version"), wxPoint(15, 105));

  defaultDatabaseControl = new wxTextCtrl(panel, wxID_ANY, wxString::Format("%d", sessionOptions.defaultDatabaseID.Value()), wxPoint(120, 130));
  text6 = new wxStaticText(panel, wxID_ANY, wxT("Default Database"), wxPoint(15, 130));

  checkbox = new wxCheckBox(panel, wxID_ANY, wxT("Big-Endian Byte Order"), wxPoint(15, 155));
  if (options.bigEndianByteOrder)
  {
    checkbox->Set3StateValue(wxCheckBoxState::wxCHK_CHECKED);
  }
  else
  {
    checkbox->Set3StateValue(wxCheckBoxState::wxCHK_UNCHECKED);
  }

  okButton = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxSize(70, 30));
  cancelButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize(70, 30));

  Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SetupDialog::OnCancel));

  hbox->Add(okButton, 1);
  hbox->Add(cancelButton, 1, wxLEFT, 5);

  vbox->Add(panel, 1);
  vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

  SetSizer(vbox);

  Centre();
}

SHostSessionSetupOptions SetupDialog::GetSessionSetupOptions(const SHostSetupOptions& options, sbio::SessionID sessionID) const
{
  for (const auto& sessionOptions : options.sessions)
  {
    if (sessionOptions.sessionID.Value() == sessionID.Value())
    {
      return sessionOptions;
    }
  }

  SHostSessionSetupOptions sessionOptions;
  sessionOptions.sessionID = sessionID;
  sessionOptions.hostToIGPort = options.hostToIGPort;
  sessionOptions.igToHostPort = options.igToHostPort;
  sessionOptions.hostIPAddress = options.hostIPAddress;
  sessionOptions.igIPAddress = options.igIPAddress;
  sessionOptions.defaultDatabaseID = options.defaultDatabaseID;
  return sessionOptions;
}

long SetupDialog::GetReceivePort() const
{
  long receivePort = 0;
  receivePortControl->GetValue().ToLong(&receivePort);
  return receivePort;
}

long SetupDialog::GetLocalReceivePort() const
{
  long localReceivePort = 0;
  localReceivePortControl->GetValue().ToLong(&localReceivePort);
  return localReceivePort;
}

long SetupDialog::GetDefaultDatabase() const
{
  long defaultDatabase = 0;
  defaultDatabaseControl->GetValue().ToLong(&defaultDatabase);
  return defaultDatabase;
}

wxString SetupDialog::GetIPAddress() const
{
  return ipAddressControl->GetValue();
}

wxString SetupDialog::GetVersionID() const
{
  return versionIDControl->GetValue();
}

bool SetupDialog::GetBigEndianByteOrder() const
{
  return checkbox->GetValue();
}

bool SetupDialog::IsIPAddressValid() const
{
  return regexIpAddress->Matches(GetIPAddress());
}

void SetupDialog::OnOk(wxCommandEvent& event)
{
  CHost* pHost = g_pHostEmulatorGuiApp != nullptr ? g_pHostEmulatorGuiApp->GetHost() : nullptr;
  if (pHost == nullptr)
  {
    return;
  }

  SHostSetupOptions options = pHost->GetHostSetupOptions();
  const sbio::SessionID sessionID = g_pHostEmulatorGuiApp != nullptr ? g_pHostEmulatorGuiApp->GetSelectedSessionID() : sbio::SessionID(0);
  SHostSessionSetupOptions sessionOptions = GetSessionSetupOptions(options, sessionID);

  //Validate the IP Address
  options.igIPAddress = ipAddressControl->GetValue();
  if (!regexIpAddress->Matches(options.igIPAddress))
  {
    HostCigiErrorEventArgs args;
    args.sError = "IP Address invalid.\nMust be in the form A.B.C.D where A,B,C and D are integers [0, 255]";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox("IP Address invalid.\nMust be in the form A.B.C.D where A,B,C and D are integers [0, 255]");
    return;
  }

  long receivePort = 0;
  long localReceivePort = 0;
  long majorVersion = 0;
  long minorVersion = 0;
  long defaultDatabaseInput = 0;
  if (!receivePortControl->GetValue().ToCLong(&receivePort) || receivePort < 0 || receivePort > 65535)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Receive Port invalid.\nReceive Port must be between 0 and 65535.";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox("Receive Port invalid.\nReceive Port must be between 0 and 65535.");
    return;
  }

  if (!localReceivePortControl->GetValue().ToCLong(&localReceivePort) || localReceivePort < 0 || localReceivePort > 65535)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Local Receive Port invalid.\nLocal Receive Port must be between 0 and 65535.";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox("Local Receive Port invalid.\nLocal Receive Port must be between 0 and 65535.");
    return;
  }

  defaultDatabaseControl->GetValue().ToCLong(&defaultDatabaseInput);
  wxString cigiVersion = versionIDControl->GetValue();
  wxString majorVersionString = cigiVersion.SubString(0, 1);
  majorVersionString.ToCLong(&majorVersion);
  wxString minorVersionString = cigiVersion.SubString(2, 2);
  minorVersionString.ToCLong(&minorVersion);

  options.bigEndianByteOrder = checkbox->GetValue();

  if (options.igIPAddress.empty())
  {
    HostCigiErrorEventArgs args;
    args.sError = "String conversion failed";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox(wxT("String conversion failed"));
    return;
  }

  if (cigiVersion == "3.3")
  {
    options.eCigiVersion = ECigiVersion::VERSION_3_3;
  }
  else if (cigiVersion == "4.0")
  {
    options.eCigiVersion = ECigiVersion::VERSION_4_0;
  }

  if (defaultDatabaseInput < 0 || defaultDatabaseInput > 127)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Default database invalid.\n Default database must be between 0 and 127.";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox("Default database invalid.\n Default database must be between 0 and 127.");
    return;
  }

  sessionOptions.hostToIGPort = static_cast<uint16_t>(receivePort);
  sessionOptions.igToHostPort = static_cast<uint16_t>(localReceivePort);
  sessionOptions.igIPAddress = options.igIPAddress;
  sessionOptions.defaultDatabaseID = DatabaseID(defaultDatabaseInput);

  options.hostToIGPort = sessionOptions.hostToIGPort;
  options.igToHostPort = sessionOptions.igToHostPort;
  options.defaultDatabaseID = sessionOptions.defaultDatabaseID;

  bool bUpdatedSession = false;
  for (auto& existingSessionOptions : options.sessions)
  {
    if (existingSessionOptions.sessionID.Value() == sessionID.Value())
    {
      existingSessionOptions = sessionOptions;
      bUpdatedSession = true;
      break;
    }
  }

  if (!bUpdatedSession && !options.sessions.empty())
  {
    options.sessions.push_back(sessionOptions);
  }

  pHost->Initialize(options);

  EndModal(wxID_OK);
}

void SetupDialog::OnCancel(wxCommandEvent& WXUNUSED(e))
{
  wxMessageDialog dialog(this, wxT("The default values will be used."), wxT("Cancel"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
  if (dialog.ShowModal() == wxID_YES)
  {
    EndModal(wxID_CANCEL);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
