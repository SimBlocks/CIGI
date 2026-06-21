//Copyright SimBlocks LLC 2016-2026
#include "HostEmulatorGuiApp.h"
#include "CigiLib/CigiLib.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostCigiLib/HostCigiLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorOptions.h"
#include "SetupDialog.h"
#include "ViewLib/ViewLib.h"
#include "UtilitiesLib/EventDispatcher.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "ViewLib/ViewManager.h"
#include "HostCigiLib/HostViewManager.h"
#include "ViewLib/View.h"
#include "ViewLib/IViewCreator.h"
#include "ViewLib/ViewGroup.h"
#include "HostEmulatorDefinitions.h"
#include <memory>
#include <string>

#include <sstream>
#include <wx/wx.h>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;
using namespace sbio::view;

wxIMPLEMENT_APP(CHostEmulatorGuiApp);
BEGIN_EVENT_TABLE(CHostEmulatorGuiApp, wxApp)
EVT_TIMER(ID_HOST_UPDATE_TIMER, CHostEmulatorGuiApp::OnHostUpdateTimer)
EVT_TIMER(ID_GUI_UPDATE_TIMER, CHostEmulatorGuiApp::OnGuiUpdateTimer)
END_EVENT_TABLE()

CHostEmulatorGuiApp* g_pHostEmulatorGuiApp = nullptr;

SHostEmulatorGlobals g_globals;

CHostEmulatorGuiApp::CHostEmulatorGuiApp() : wxApp()
{
  g_pHostEmulatorGuiApp = this;
  previousFrameNumber = 0;

#ifdef _WIN32
  ZeroMemory(&m_IGProcessInformation, sizeof(m_IGProcessInformation));
#endif
}

CHostEmulatorGuiApp::~CHostEmulatorGuiApp()
{
}

void CHostEmulatorGuiApp::Uninitialize()
{
  m_pHostUpdateTimer->Stop();
  m_pGuiUpdateTimer->Stop();

#ifdef _WIN32
  if (m_IGProcessInformation.hProcess != NULL)
  {
    TerminateProcess(m_IGProcessInformation.hProcess, 0);
    CloseHandle(m_IGProcessInformation.hProcess);
  }
#endif

#ifdef _WIN32
  if (m_IGProcessInformation.hThread != NULL)
  {
    CloseHandle(m_IGProcessInformation.hThread);
  }
#endif
}

CHost* CHostEmulatorGuiApp::GetHost() const
{
  return m_pHost.get();
}

sbio::SessionID CHostEmulatorGuiApp::GetSelectedSessionID() const
{
  return m_SelectedSessionID;
}

void CHostEmulatorGuiApp::LaunchImageGenerator()
{
  std::filesystem::path bin = GetSdkPath() / "Code" / "Bin" / "vc143" / "x64" / "vs2022_Release";

#ifdef _WIN32
  STARTUPINFOA startupInfo;
  ZeroMemory(&startupInfo, sizeof(startupInfo));
  startupInfo.cb = sizeof(startupInfo);

  LPSTR szCmdLine = _strdup("sbioCIGITestImageGenerator.exe");

#else

  int ret = system("./sbioCIGITestImageGenerator");
  if (ret == -1)
  {
  }

#endif

#ifdef _WIN32
  if (!CreateProcessA(NULL, szCmdLine, NULL, NULL, 0, NULL, NULL, bin.string().c_str(), &startupInfo, &m_IGProcessInformation))
  {
  }
#endif
}

class HostViewCreator : public sbio::view::IViewCreator
{
  virtual std::unique_ptr<CView> CreateView(ViewID viewID) override
  {
    return std::make_unique<CView>(viewID);
  }
};

bool CHostEmulatorGuiApp::OnInit()
{
  try
  {
    SGlobals globals;
    globals.applicationsDataPath = GetSdkPath() / "Data" / "Applications";
    globals.librariesDataPath = GetSdkPath() / "Data" / "Libraries";
    globals.thirdPartyPath = sbio::utils::GetEnvVariablePath("SBIO_THIRD_PARTY");
    globals.pEventDispatcher = make_shared<CEventDispatcher>();
    globals.pLogger = make_shared<CLogger>("Host");

    g_globals.pEventDispatcher = globals.pEventDispatcher;
    g_globals.pLogger = globals.pLogger;
    g_globals.applicationsDataPath = globals.applicationsDataPath;
    g_globals.librariesDataPath = globals.librariesDataPath;
    std::shared_ptr<CHostViewManager> pHostViewManager = std::make_shared<CHostViewManager>();
    g_globals.pViewManager = pHostViewManager;

    InitCigiLib();
    InitUtilitiesLib(globals);

    std::unique_ptr<HostViewCreator> pHostViewCreator = std::make_unique<HostViewCreator>();

    std::filesystem::path viewsConfigFile = globals.applicationsDataPath / "HostEmulator" / "Views.config.json";
    pHostViewManager->Init(viewsConfigFile, std::move(pHostViewCreator));

    SViewLibParams viewLibParams;
    viewLibParams.pViewManager = g_globals.pViewManager;
    InitViewLib(globals, viewLibParams);

    CHostEmulatorOptions options;
    options.LoadOptions(globals.applicationsDataPath / "HostEmulator" / "HostEmulator.ini");

    m_pHost = make_unique<CHost>();
    InitHostCigiLib(globals, m_pHost.get());
    m_pHost->Initialize(options.hostSetupOptions);
    m_SelectedSessionID = m_pHost->GetActiveSessionID();

    pFrame = new CHostEmulatorGuiFrame();
    pFrame->m_bFrameActive = true;
    pFrame->Show(true);

    m_pHostUpdateTimer = new wxTimer(this, ID_HOST_UPDATE_TIMER);
    m_pHostUpdateTimer->Start(30);

    m_pGuiUpdateTimer = new wxTimer(this, ID_GUI_UPDATE_TIMER);
    m_pGuiUpdateTimer->Start(250);

    return true;
  }
  catch (const std::exception& e)
  {
    wxMessageBox(wxString::Format("Startup failed: %s", e.what()), "Exception");
    return false;
  }
  catch (...)
  {
    wxMessageBox("Startup failed with unknown exception.", "Unknown Exception");
    return false;
  }
}

void CHostEmulatorGuiApp::OnHostUpdateTimer(wxTimerEvent& timerEvent)
{
  double fDeltaTime = stopWatch.GetElapsedSeconds();

  stopWatch.Reset();
  stopWatch.Start();

  m_pHost->Update(fDeltaTime);
}

void CHostEmulatorGuiApp::OnGuiUpdateTimer(wxTimerEvent& timerEvent)
{
  wxString sIGMode;

  CHostSession* pHostSession = m_pHost ? m_pHost->GetHostSession() : nullptr;
  if (pHostSession == nullptr)
  {
    pFrame->GetStatusBar()->SetStatusText("Database: No Database", 0);
    pFrame->GetStatusBar()->SetStatusText("Not Connected", 1);
    pFrame->GetStatusBar()->SetStatusText("CIGI v4.0", 2);
    pFrame->GetStatusBar()->SetStatusText("", 3);
    pFrame->UpdateGUI();
    return;
  }

  if (pHostSession->IsConnected())
  {
    int framesSinceLast = pHostSession->m_LastReceivedIGFrame.Value() - previousFrameNumber;
    float fps = framesSinceLast / (.001f * m_pGuiUpdateTimer->GetInterval());
    previousFrameNumber = pHostSession->m_LastReceivedIGFrame.Value();

    wxString sFPS = wxString::Format("Connected %.2f FPS", fps);
    pFrame->GetStatusBar()->SetStatusText(sFPS, 3);

    sIGMode += "IG Mode: " + ConvertToString(pHostSession->GetActualIGMode());
  }
  else
  {
    sIGMode += "Not Connected";

    pFrame->GetStatusBar()->SetStatusText("", 3);
  }

  if (m_pHost != nullptr)
  {
    if (m_pHost->GetHostSetupOptions().eCigiVersion == ECigiVersion::VERSION_3_3)
    {
      pFrame->GetStatusBar()->SetStatusText("CIGI v3.3", 2);
    }
    else if (m_pHost->GetHostSetupOptions().eCigiVersion == ECigiVersion::VERSION_4_0)
    {
      pFrame->GetStatusBar()->SetStatusText("CIGI v4.0", 2);
    }
  }

  wxString sDatabaseState = "Database: ";
  sDatabaseState += ConvertToString(pHostSession->GetDatabaseState());
  pFrame->GetStatusBar()->SetStatusText(sDatabaseState, 0);

  pFrame->GetStatusBar()->SetStatusText(sIGMode, 1);

  pFrame->UpdateGUI();
}

CHostSession* GetHostSession()
{
  if (g_pHostEmulatorGuiApp == nullptr || g_pHostEmulatorGuiApp->GetHost() == nullptr)
  {
    return nullptr;
  }

  return g_pHostEmulatorGuiApp->GetHost()->GetHostSession();
}

CHostSession* GetHostSession(sbio::SessionID sessionID)
{
  if (g_pHostEmulatorGuiApp == nullptr || g_pHostEmulatorGuiApp->GetHost() == nullptr)
  {
    return nullptr;
  }

  return g_pHostEmulatorGuiApp->GetHost()->GetHostSession(sessionID);
}

void CHostEmulatorGuiApp::OnOk(wxCommandEvent& event)
{
  SHostSetupOptions options = m_pHost->GetHostSetupOptions();

  wxButton* okButton;
  okButton = static_cast<wxButton*>(event.GetEventObject());
  SetupDialog* dialog = static_cast<SetupDialog*>(okButton->GetParent());

  options.igToHostPort = dialog->GetReceivePort();
  options.hostToIGPort = dialog->GetLocalReceivePort();
  options.defaultDatabaseID = DatabaseID(dialog->GetDefaultDatabase());

  options.eCigiVersion = ECigiVersion::VERSION_4_0;

  options.bigEndianByteOrder = dialog->GetBigEndianByteOrder();

  wxString ipAddress = dialog->GetIPAddress();
  if (ipAddress.ToStdString() == "")
  {
    HostCigiErrorEventArgs args;
    args.sError = "String conversion failed";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox(wxT("String conversion failed"));
    return;
  }
  //Validate the IP Address
  if (!dialog->IsIPAddressValid())
  {
    HostCigiErrorEventArgs args;
    args.sError = "IP Address invalid.\nMust be in the form A.B.C.D where A,B,C and D are integers [0, 255]";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox("IP Address invalid.\nMust be in the form A.B.C.D where A,B,C and D are integers [0, 255]");
    return;
  }

  //Validate default database
  if (options.defaultDatabaseID.Value() < 0 || options.defaultDatabaseID.Value() > 127)
  {
    HostCigiErrorEventArgs args;
    args.sError = "Default database invalid.\n Default database must be between 0 and 127.";
    Event::Raise<HostCigiEvent>(args);

    wxMessageBox("Default database invalid.\n Default database must be between 0 and 127.");
    return;
  }
  options.igIPAddress = ipAddress.ToStdString();

  m_pHost->Initialize(options);

  dialog->Destroy();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
