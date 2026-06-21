//Copyright SimBlocks LLC 2016-2026
#include "wxWidgetsUtilities/BasePacketPanel.h"
#include "wx/radiobox.h"
#include "wx/spinctrl.h"
#include <sstream>

using namespace std;

#ifdef HI_DPI
const int BORDER_SIZE = 4;
const int CONTROL_HEIGHT = 50;
#else
const int BORDER_SIZE = 2;
const int CONTROL_HEIGHT = 20;
#endif

CBasePacketPanel::CBasePacketPanel(wxWindow* pParent) : wxPanel(pParent)
{
}

void CBasePacketPanel::ChangeText(const std::string& sCtrlName, const std::string& sText)
{
  auto it = m_StaticText.find(sCtrlName);

  if (it == m_StaticText.end())
  {
    return;
  }

  it->second->SetLabel(sText + ":");
}

void CBasePacketPanel::CreateBoolControl(wxSizer* pParent, const std::string& sName, bool bChecked)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxCheckBox* pCheckBoxCtrl = new wxCheckBox(this, wxID_ANY, sName, wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), CONTROL_HEIGHT));
  hbox->Add(pCheckBoxCtrl, 0, wxEXPAND | wxALL, BORDER_SIZE);

  pCheckBoxCtrl->SetValue(bChecked);

  pParent->Add(hbox);

  m_CheckBoxCtrls[sName] = pCheckBoxCtrl;
}

void CBasePacketPanel::CreateButton(wxWindow* pWindow, wxSizer* pSizer, int id, const std::string& sName)
{
  wxButton* pButton = new wxButton(pWindow, id, sName);
  pSizer->Add(pButton, wxSizerFlags().Center());
}

void CBasePacketPanel::CreateChoiceControl(wxSizer* pParent, const std::string& sName, wxArrayString choices, wxWindowID windowID, int nWidth)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  wxString sFirstChoice = choices.empty() ? wxString("") : choices[0];
  wxComboBox* pComboBox = new wxComboBox(this, windowID, sFirstChoice, wxDefaultPosition, wxSize(nWidth - 20, 20), choices);
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  hbox->Add(pComboBox, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_ComboBoxCtrls[sName] = pComboBox;
  m_StaticText[sName] = pStaticText;

  pComboBox->SetSelection(0);
}

void CBasePacketPanel::SelectChoice(const std::string& sName, int nChoicePos)
{
  m_ComboBoxCtrls[sName]->Select(nChoicePos);
}

void CBasePacketPanel::SelectChoice(const std::string& sName, const wxString& sChoice)
{
  m_ComboBoxCtrls[sName]->SetStringSelection(sChoice);
}

void CBasePacketPanel::UpdateChoiceControl(const std::string& sName, wxArrayString choices)
{
  auto it = m_ComboBoxCtrls.find(sName);

  it->second->Clear();

  for (auto s : choices)
  {
    it->second->Append(s);
  }

  it->second->SetSelection(0);
}

void CBasePacketPanel::CreateChoiceControl(wxSizer* pParent, wxWindowID eWidgetID, const std::string& sName, wxArrayString choices, bool bShowNumbers)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxString s;
  if (bShowNumbers)
  {
    int n = 0;
    for (auto& c : choices)
    {
      c.Prepend(wxString::Format("%d - ", n));
      ++n;
    }
  }

  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  wxString sFirstChoice = choices.empty() ? wxString("") : choices[0];
  wxComboBox* pComboBox = new wxComboBox(this, eWidgetID, sFirstChoice, wxDefaultPosition, wxSize(180, 20), choices);
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  hbox->AddSpacer(200 - pStaticText->GetSize().GetX());
  hbox->Add(pComboBox, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_ComboBoxCtrls[sName] = pComboBox;
  m_StaticText[sName] = pStaticText;

  pComboBox->SetSelection(0);
}

void CBasePacketPanel::CreateDirectoryControl(wxSizer* pParent, const std::string& sName, std::string sDefaultPath)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxButton* selectCDBRootDirectoryButton = new wxButton(this, wxID_ANY, sName);

  m_DirectoryCtrls[sName] = new wxTextCtrl(this, wxID_ANY, sDefaultPath, wxDefaultPosition, wxSize(500, CONTROL_HEIGHT));
  hbox->Add(selectCDBRootDirectoryButton, 0, wxEXPAND | wxALL, BORDER_SIZE);
  hbox->Add(m_DirectoryCtrls[sName], 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_DirectoryCtrls[sName]->Bind(wxEVT_TEXT,
                                [this, sName](wxCommandEvent& event)
                                {
                                  OnTextChanged(sName);
                                  event.Skip();
                                });

  auto pOnSelectDirectoryButton = [this](wxCommandEvent& e)
  {
    wxObject* pObject = e.GetEventObject();
    if (pObject->IsKindOf(CLASSINFO(wxButton)))
    {
      wxButton* pButton = (wxButton*)pObject;
      string sLabel = pButton->GetLabel().ToStdString();
      wxDirDialog dirDialog(pButton->GetParent(), sLabel, wxEmptyString, wxDD_DEFAULT_STYLE);

      if (dirDialog.ShowModal() == wxID_OK)
      {
        wxString sPath = dirDialog.GetPath() + "\\";
        m_DirectoryCtrls[sLabel]->SetValue(sPath);
        OnDirectorySelected(sLabel);
      }
    }
  };

  selectCDBRootDirectoryButton->Bind(wxEVT_BUTTON, pOnSelectDirectoryButton, wxID_ANY);
}

void CBasePacketPanel::CreateFloatControl(wxSizer* pParent, const std::string& sName, bool bEditable, const std::string& sDefaultValue)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  wxTextCtrl* pFloatCtrl = new wxTextCtrl(this, wxID_ANY, sDefaultValue, wxDefaultPosition, wxSize(180, 20), 0, wxTextValidator(wxFILTER_NUMERIC));
  pFloatCtrl->SetEditable(bEditable);
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  long longSize = pStaticText->GetSize().GetX();
  longSize = 200 - longSize;
  hbox->AddSpacer(longSize);
  hbox->Add(pFloatCtrl, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_TextCtrls[sName] = pFloatCtrl;
  m_StaticText[sName] = pStaticText;

  pFloatCtrl->Bind(wxEVT_TEXT,
                   [this, sName](wxCommandEvent& event)
                   {
                     OnTextChanged(sName);
                     event.Skip();
                   });
}

void CBasePacketPanel::CreateIntControl(wxSizer* pParent, const std::string& sName, const std::string& sDefaultValue)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":", wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), CONTROL_HEIGHT));
  wxTextCtrl* pIntCtrl = new wxTextCtrl(this, wxID_ANY, sDefaultValue, wxDefaultPosition, wxSize(180, CONTROL_HEIGHT), 0, wxTextValidator(wxFILTER_NUMERIC));
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  long longSize = pStaticText->GetSize().GetX();
  hbox->AddSpacer(200 - longSize);
  hbox->Add(pIntCtrl, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_TextCtrls[sName] = pIntCtrl;
  m_StaticText[sName] = pStaticText;
}

void CBasePacketPanel::CreateMultiSelectionControl(wxSizer* pParent, const std::string& sName, wxArrayString choices)
{
  int flags = wxLB_MULTIPLE;

  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  int numEntries = choices.size();
  wxListBox* pListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(180, numEntries * 20), choices, flags);
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  hbox->AddSpacer(200 - pStaticText->GetSize().GetX());
  hbox->Add(pListBox, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_ListBoxCtrls[sName] = pListBox;
}

void CBasePacketPanel::CreateRadioControl(wxSizer* pParent, const std::string& sName, wxArrayString choices, wxWindowID windowID)
{
  wxRadioBox* pRadioBox = new wxRadioBox(this, windowID, sName, wxDefaultPosition, wxSize(380, CONTROL_HEIGHT), choices);
  pParent->Add(pRadioBox, 0, wxALIGN_CENTER);

  m_RadioCtrls[sName] = pRadioBox;
}

void CBasePacketPanel::CreateRangeControl(wxSizer* pParent, const std::string& sName, int nMin, int nMax)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  wxSpinCtrl* pSpinCtrl = new wxSpinCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, 0, nMin, nMax, 0);
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  hbox->AddSpacer(200 - pStaticText->GetSize().GetX());
  hbox->Add(pSpinCtrl, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_SpinCtrls[sName] = pSpinCtrl;
  m_StaticText[sName] = pStaticText;
}

wxStaticBoxSizer* CBasePacketPanel::CreateStaticBoxControl(wxWindow* pWindow, const std::string& sName)
{
  wxStaticBox* pStaticBox = new wxStaticBox(this, wxID_ANY, sName, wxDefaultPosition, wxDefaultSize);
  wxStaticBoxSizer* pStaticBoxSizer = new wxStaticBoxSizer(pStaticBox, wxVERTICAL);

  m_StaticBoxCtrls[sName] = pStaticBox;

  return pStaticBoxSizer;
}

void CBasePacketPanel::CreateTextControl(wxSizer* pParent, const std::string& sName)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  wxTextCtrl* pTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(180, 20));
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  long longSize = pStaticText->GetSize().GetX();
  hbox->AddSpacer(200 - longSize);
  hbox->Add(pTextCtrl, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_TextCtrls[sName] = pTextCtrl;
  m_StaticText[sName] = pStaticText;
}

void CBasePacketPanel::CreateTextAreaControl(wxSizer* pParent, const std::string& sName)
{
  wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sName + ":");
  wxTextCtrl* pTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, 80), wxTE_MULTILINE);
  hbox->Add(pStaticText, 0, wxEXPAND | wxALL, BORDER_SIZE);
  long longSize = pStaticText->GetSize().GetX();
  hbox->AddSpacer(200 - longSize);
  hbox->Add(pTextCtrl, 0, wxEXPAND | wxALL, BORDER_SIZE);
  pParent->Add(hbox);

  m_TextCtrls[sName] = pTextCtrl;
  m_StaticText[sName] = pStaticText;
}

bool CBasePacketPanel::GetBool(const std::string& sName) const
{
  auto it = m_CheckBoxCtrls.find(sName);

  return it->second->IsChecked();
}

std::string CBasePacketPanel::GetChoice(const std::string& sName) const
{
  auto it = m_ComboBoxCtrls.find(sName);

  return it->second->GetStringSelection().ToStdString();
}

int CBasePacketPanel::GetChoiceIndex(const std::string& sName) const
{
  auto it = m_ComboBoxCtrls.find(sName);

  return it->second->GetCurrentSelection();
}

std::string CBasePacketPanel::GetDirectory(const std::string& sName) const
{
  auto it = m_DirectoryCtrls.find(sName);

  std::string directory = it->second->GetValue().ToStdString();
  if (directory.empty())
  {
    return directory;
  }

  if (directory.back() != '\\' && directory.back() != '/')
  {
    directory += "\\";
  }

  return directory;
}

double CBasePacketPanel::GetDouble(const std::string& sName) const
{
  auto it = m_TextCtrls.find(sName);

  return wxAtof(it->second->GetValue());
}

float CBasePacketPanel::GetFloat(const std::string& sName) const
{
  auto it = m_TextCtrls.find(sName);

  return static_cast<float>(wxAtof(it->second->GetValue()));
}

int CBasePacketPanel::GetInt(const std::string& sName) const
{
  auto it = m_TextCtrls.find(sName);

  return wxAtoi(it->second->GetValue());
}

std::set<std::string> CBasePacketPanel::GetMultipleSelections(const std::string& sName)
{
  auto it = m_ListBoxCtrls.find(sName);

  wxArrayInt listBoxSelections;
  it->second->GetSelections(listBoxSelections);

  set<string> selections;
  for (auto n : listBoxSelections)
  {
    selections.insert(it->second->GetString(n).ToStdString());
  }

  return selections;
}

std::string CBasePacketPanel::GetRadio(const std::string& sName) const
{
  auto it = m_RadioCtrls.find(sName);
  return it->second->GetStringSelection().ToStdString();
}

int CBasePacketPanel::GetRange(const std::string& sName) const
{
  auto it = m_SpinCtrls.find(sName);

  return it->second->GetValue();
}

void CBasePacketPanel::NotifyRadio(const std::string& sName)
{
  auto it = m_RadioCtrls.find(sName);

#ifdef _WIN32
  it->second->SendNotificationEvent();
#else
  wxRadioBox* pRadioBox = it->second;
  wxCommandEvent event(wxEVT_RADIOBOX, pRadioBox->GetId());
  event.SetInt(pRadioBox->GetSelection());
  wxPostEvent(pRadioBox, event);
#endif
}

std::string CBasePacketPanel::GetString(const std::string& sName) const
{
  auto it = m_TextCtrls.find(sName);

  return it->second->GetValue().ToStdString();
}

double CBasePacketPanel::ResetDouble(const std::string& sName)
{
  auto it = m_TextCtrls.find(sName);
  it->second->Clear();
  it->second->SetValue("0");
  return 0.0;
}

float CBasePacketPanel::ResetFloat(const std::string& sName)
{
  auto it = m_TextCtrls.find(sName);
  it->second->Clear();
  it->second->SetValue("0");
  return 0.0;
}

int CBasePacketPanel::ResetInt(const std::string& sName)
{
  auto it = m_TextCtrls.find(sName);
  it->second->Clear();
  it->second->SetValue("0");
  return 0;
}

void CBasePacketPanel::SetString(const std::string& sName, const std::string& sText)
{
  auto it = m_TextCtrls.find(sName);
  it->second->Clear();
  it->second->SetValue(sText);
}

void CBasePacketPanel::SetDirectory(const std::string& sName, const std::filesystem::path& sPath)
{
  auto it = m_DirectoryCtrls.find(sName);
  if (it == m_DirectoryCtrls.end())
  {
    return;
  }

  std::filesystem::path normalizedPath = sPath;
  std::string normalizedPathString = normalizedPath.string();
  while (normalizedPathString.size() > 3 && (normalizedPathString.back() == '\\' || normalizedPathString.back() == '/'))
  {
    normalizedPathString.pop_back();
  }

  it->second->Clear();
  it->second->SetValue(normalizedPathString);
}

void CBasePacketPanel::SetInt(const std::string& sName, int n)
{
  auto it = m_TextCtrls.find(sName);
  it->second->Clear();

  std::stringstream ss;
  ss << n;
  it->second->SetValue(ss.str());
}

void CBasePacketPanel::SetFloat(const std::string& sName, float f)
{
  auto it = m_TextCtrls.find(sName);
  it->second->Clear();

  std::stringstream ss;
  ss << f;
  it->second->SetValue(ss.str());
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
