//Copyright SimBlocks LLC 2016-2026
#include "ScriptsPage.h"
#include "GlobalHeaders/Globals.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include <windows.h>
#include <shellapi.h>

using namespace std;
using namespace sbio;
using namespace sbio::utils;
using namespace sbio::cigi::host;

extern SHostEmulatorGlobals g_globals;
extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CScriptsPage, wxPanel)
  EVT_COMBOBOX(ID_SCRIPTS_PAGE_CATEGORY_COMBO_BOX, CScriptsPage::OnCategoriesComboBox)
  EVT_LISTBOX(ID_SCRIPTS_PAGE_LIST_BOX, CScriptsPage::OnListBox)
  EVT_BUTTON(ID_RUN_SCRIPT_BUTTON, CScriptsPage::OnRunScript)
  EVT_BUTTON(ID_EDIT_SCRIPT_BUTTON, CScriptsPage::OnEditScript)
  EVT_BUTTON(ID_STOP_SCRIPT_BUTTON, CScriptsPage::OnStopScript)
wxEND_EVENT_TABLE();
// clang-format on

std::filesystem::path GetScriptsPath()
{
  return g_globals.applicationsDataPath / "HostEmulator" / "Scripts";
}

CHost* GetHostFromApp()
{
  if (g_pHostEmulatorGuiApp == nullptr)
  {
    return nullptr;
  }

  return g_pHostEmulatorGuiApp->GetHost();
}

CScriptsPage::CScriptsPage(wxWindow* pParent) : CNotebookPage(pParent)
{
  wxFlexGridSizer* hboxTop = new wxFlexGridSizer(wxHORIZONTAL);
  wxBoxSizer* vboxRight = new wxBoxSizer(wxVERTICAL);

  wxPanel* pPanelCenter = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(150, 105));

  wxPanel* pPanelLeft = new wxPanel(this, wxID_ANY, wxDefaultPosition);
  pPanelLeft->SetMinSize(wxSize(500, 700));

  wxArrayString scriptCategoryOptions = GetScriptCategories();
  wxString initialCategory = scriptCategoryOptions.empty() ? "" : scriptCategoryOptions[0];

  m_pCategoriesComboBox = new wxComboBox(pPanelLeft, ID_SCRIPTS_PAGE_CATEGORY_COMBO_BOX, initialCategory, wxPoint(20, 20), wxSize(450, 20), scriptCategoryOptions);
  if (!scriptCategoryOptions.empty())
  {
    m_pCategoriesComboBox->SetSelection(0);
  }

  wxArrayString scriptNameChoices = GetScriptNames(initialCategory.ToStdString());
  m_pListBox = new wxListBox(pPanelLeft, ID_SCRIPTS_PAGE_LIST_BOX, wxPoint(20, 45), wxSize(450, 600), scriptNameChoices);
  SelectFirstScript(m_pListBox, m_sSelectedScriptFile);

  // Script Commands panel
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, pPanelCenter, "Script Commands");
  wxButton* pButton = new wxButton(pPanelCenter, ID_RUN_SCRIPT_BUTTON, wxT("Run Script"));
  vbox->Add(pButton, 0, wxALL, 2);
  pButton = new wxButton(pPanelCenter, ID_STOP_SCRIPT_BUTTON, wxT("Stop Script"));
  vbox->Add(pButton, 0, wxALL, 2);
  pButton = new wxButton(pPanelCenter, ID_EDIT_SCRIPT_BUTTON, wxT("Edit Script"));
  vbox->Add(pButton, 0, wxALL, 2);
  pPanelCenter->SetSizer(vbox);

  // Script Status panel
  wxPanel* pPanelStatus = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(160, 60));
  wxStaticBoxSizer* statusBox = new wxStaticBoxSizer(wxVERTICAL, pPanelStatus, "Script Status");
  m_pScriptStateLabel = new wxStaticText(pPanelStatus, wxID_ANY, "Script State: Stopped", wxDefaultPosition, wxSize(130, 20));
  statusBox->Add(m_pScriptStateLabel, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 8);
  pPanelStatus->SetSizer(statusBox);

  // Add panels to the top-level sizer
  hboxTop->Add(pPanelLeft, 1, wxEXPAND | wxALL, 5);
  vboxRight->Add(pPanelCenter, 0, wxSHRINK | wxBOTTOM, 5);
  vboxRight->Add(pPanelStatus, 0, wxSHRINK, 0);
  hboxTop->Add(vboxRight, 0, wxALL, 5);
  SetSizerAndFit(hboxTop);
}

wxArrayString CScriptsPage::GetScriptCategories()
{
  wxArrayString choices;
  std::filesystem::path scriptsRootPath = GetScriptsPath();

  if (!std::filesystem::exists(scriptsRootPath))
  {
    return choices;
  }

  std::filesystem::directory_iterator it(scriptsRootPath);
  std::filesystem::directory_iterator endit;

  while (it != endit)
  {
    if (std::filesystem::is_directory(*it))
    {
      choices.push_back(it->path().filename().string());
    }

    ++it;
  }

  choices.Sort();
  return choices;
}

wxArrayString CScriptsPage::GetScriptNames(std::string sCategory)
{
  wxArrayString choices;

  if (sCategory.empty())
  {
    return choices;
  }

  std::filesystem::path filePath = GetScriptsPath() / sCategory;

  if (!std::filesystem::exists(filePath))
  {
    return choices;
  }

  std::filesystem::directory_iterator it(filePath);
  std::filesystem::directory_iterator endit;

  while (it != endit)
  {
    if (std::filesystem::is_directory(*it))
    {
    }
    else
    {
      choices.push_back(it->path().stem().string());
    }

    ++it;
  }

  choices.Sort();
  return choices;
}

void CScriptsPage::RefreshScriptList(std::string sCategory)
{
  std::string sPreviouslySelectedScriptFile = m_sSelectedScriptFile;
  m_sSelectedScriptFile.clear();
  m_pListBox->Clear();

  wxArrayString choices = GetScriptNames(sCategory);
  for (int n = 0; n < choices.size(); ++n)
  {
    m_pListBox->Append(choices[n]);
  }

  if (!sPreviouslySelectedScriptFile.empty())
  {
    wxString previouslySelectedScriptName = std::filesystem::path(sPreviouslySelectedScriptFile).stem().string();
    int selectedIndex = m_pListBox->FindString(previouslySelectedScriptName);
    if (selectedIndex != wxNOT_FOUND)
    {
      m_pListBox->SetSelection(selectedIndex);
      m_sSelectedScriptFile = previouslySelectedScriptName.ToStdString() + ".chai";
      return;
    }
  }

  SelectFirstScript(m_pListBox, m_sSelectedScriptFile);
}

void CScriptsPage::RefreshCategories()
{
  std::string sCurrentCategory = GetCategory();
  wxArrayString categories = GetScriptCategories();

  m_pCategoriesComboBox->Clear();

  for (int n = 0; n < categories.size(); ++n)
  {
    m_pCategoriesComboBox->Append(categories[n]);
  }

  if (categories.empty())
  {
    m_pCategoriesComboBox->SetValue("");
    RefreshScriptList("");
    return;
  }

  wxString selectedCategory = categories[0];
  if (!sCurrentCategory.empty())
  {
    for (int n = 0; n < categories.size(); ++n)
    {
      if (categories[n].ToStdString() == sCurrentCategory)
      {
        selectedCategory = categories[n];
        break;
      }
    }
  }

  m_pCategoriesComboBox->SetStringSelection(selectedCategory);
  RefreshScriptList(selectedCategory.ToStdString());
}

void CScriptsPage::OnCategoriesComboBox(wxCommandEvent& event)
{
  RefreshScriptList(event.GetString().ToStdString());
}

void CScriptsPage::OnListBox(wxCommandEvent& event)
{
  m_sSelectedScriptFile = event.GetString().ToStdString();

  if (!m_sSelectedScriptFile.empty())
  {
    m_sSelectedScriptFile += ".chai";
  }
}

std::string CScriptsPage::GetCategory()
{
  if (m_pCategoriesComboBox == nullptr)
  {
    return "";
  }

  wxString sCategory = m_pCategoriesComboBox->GetStringSelection();
  if (sCategory.empty())
  {
    sCategory = m_pCategoriesComboBox->GetValue();
  }
  if (sCategory.empty() && m_pCategoriesComboBox->GetCount() > 0)
  {
    sCategory = m_pCategoriesComboBox->GetString(0);
  }

  return sCategory.ToStdString();
}

void CScriptsPage::OnResetScript(wxCommandEvent& event)
{
  CHost* pHost = GetHostFromApp();
  if (pHost == nullptr || pHost->GetScriptRuntime() == nullptr)
  {
    return;
  }

  pHost->GetScriptRuntime()->SendReset();
}

void CScriptsPage::OnEditScript(wxCommandEvent& event)
{
  std::string sCategory = GetCategory();
  std::string sScriptFile = m_sSelectedScriptFile;

  if (sCategory.empty() || sScriptFile.empty())
  {
    return;
  }

  std::filesystem::path filePath = GetScriptsPath() / sCategory / sScriptFile;

  if (!std::filesystem::exists(filePath))
  {
    return;
  }

  //open script using default editor
  ShellExecute(NULL, L"open", filePath.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CScriptsPage::OnRunScript(wxCommandEvent& event)
{
  CHost* pHost = GetHostFromApp();
  if (pHost == nullptr || pHost->GetScriptRuntime() == nullptr)
  {
    return;
  }

  if (pHost->GetScriptRuntime()->IsScriptRunning())
  {
    HostCigiErrorEventArgs args;
    args.sError = "Cannot run script while another script is running.";
    Event::Raise<HostCigiEvent>(args);
    wxMessageBox("A script is already running.", "Can't Run Script", wxOK | wxICON_ERROR, this);
    return;
  }

  std::cout << "Running script: Category=" << GetCategory() << ", File=" << m_sSelectedScriptFile << std::endl;

  pHost->GetScriptRuntime()->Execute(GetCategory(), m_sSelectedScriptFile);
}

void CScriptsPage::OnSelected()
{
  RefreshCategories();
}

void CScriptsPage::OnStopScript(wxCommandEvent& event)
{
  CHost* pHost = GetHostFromApp();
  if (pHost == nullptr || pHost->GetScriptRuntime() == nullptr)
  {
    return;
  }

  pHost->GetScriptRuntime()->Stop();
}

void CScriptsPage::SelectFirstScript(wxListBox* pListBox, std::string& sSelectedScriptFile)
{
  sSelectedScriptFile.clear();

  if (pListBox == nullptr || pListBox->GetCount() == 0)
  {
    return;
  }

  pListBox->SetSelection(0);
  sSelectedScriptFile = pListBox->GetString(0).ToStdString();
  if (!sSelectedScriptFile.empty())
  {
    sSelectedScriptFile += ".chai";
  }
}

void CScriptsPage::UpdateScriptStateLabel()
{
  CHost* pHost = GetHostFromApp();
  if (pHost == nullptr)
  {
    return;
  }

  auto scriptRuntime = pHost->GetScriptRuntime();
  if (scriptRuntime == nullptr || m_pScriptStateLabel == nullptr)
  {
    return;
  }

  bool running = scriptRuntime->IsScriptRunning();
  m_pScriptStateLabel->SetLabel(running ? "Script State: Running" : "Script State: Stopped");
}

void CScriptsPage::UpdateGUI()
{
  UpdateScriptStateLabel();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
