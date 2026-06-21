//Copyright SimBlocks LLC 2016-2026
#pragma once
#ifndef SIMBLOCKS_BASE_PACKET_PANEL_H
#define SIMBLOCKS_BASE_PACKET_PANEL_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <set>
#include <unordered_map>
#include <filesystem>

/**
 * @brief Provides common wxWidgets controls and typed accessors for packet-editing panels.
 */
class CBasePacketPanel : public wxPanel
{
public:
  /**
   * @brief Constructs a `CBasePacketPanel` instance.
   * @param pParent Parent window or sizer that owns the created control.
   */
  CBasePacketPanel(wxWindow* pParent);

  /**
   * @brief Changes text.
   * @param sCtrlName Control name or lookup key.
   * @param sText Text value to assign.
   */
  void ChangeText(const std::string& sCtrlName, const std::string& sText);
  /**
   * @brief Creates bool control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param bChecked Initial checked state.
   */
  void CreateBoolControl(wxSizer* pParent, const std::string& sName, bool bChecked = false);
  /**
   * @brief Creates button.
   * @param pWindow Window used as the control parent.
   * @param pSizer Sizer that receives the created control.
   * @param id Identifier for the control or event source.
   * @param sName Control name or lookup key.
   */
  void CreateButton(wxWindow* pWindow, wxSizer* pSizer, int id, const std::string& sName);
  /**
   * @brief Creates choice control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param choices Choice labels used to populate the control.
   * @param windowID wxWidgets identifier assigned to the control.
   * @param nWidth Requested control width in pixels.
   */
  void CreateChoiceControl(wxSizer* pParent, const std::string& sName, wxArrayString choices, wxWindowID windowID = wxID_ANY, int nWidth = 200);
  /**
   * @brief Creates choice control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param eWidgetID wxWidgets identifier assigned to the control.
   * @param sName Control name or lookup key.
   * @param choices Choice labels used to populate the control.
   * @param bShowNumbers Whether show numbers.
   * @param bShowNumbers Whether choice labels include numeric prefixes.
   */
  void CreateChoiceControl(wxSizer* pParent, wxWindowID eWidgetID, const std::string& sName, wxArrayString choices, bool bShowNumbers = false);
  /**
   * @brief Creates directory control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   */
  void CreateDirectoryControl(wxSizer* pParent, const std::string& sName, std::string = "");
  /**
   * @brief Creates float control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param bEditable Whether the control allows editing.
   * @param sDefaultPath Default text displayed by the control.
   */
  void CreateFloatControl(wxSizer* pParent, const std::string& sName, bool bEditable = true, const std::string& sDefaultPath = "0");
  /**
   * @brief Creates int control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param sDefaultValue Default text displayed by the control.
   */
  void CreateIntControl(wxSizer* pParent, const std::string& sName, const std::string& sDefaultValue = "0");
  /**
   * @brief Creates multi selection control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param choices Choice labels used to populate the control.
   */
  void CreateMultiSelectionControl(wxSizer* pParent, const std::string& sName, wxArrayString choices);
  /**
   * @brief Creates radio control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param choices Choice labels used to populate the control.
   * @param windowID wxWidgets identifier assigned to the control.
   */
  void CreateRadioControl(wxSizer* pParent, const std::string& sName, wxArrayString choices, wxWindowID windowID);
  /**
   * @brief Creates range control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   * @param nMin Minimum accepted value.
   * @param nMax Maximum accepted value.
   */
  void CreateRangeControl(wxSizer* pParent, const std::string& sName, int nMin, int nMax);
  /**
   * @brief Creates static box control.
   * @param pWindow Window used as the control parent.
   * @param sName Control name or lookup key.
   * @return Created static box control.
   */
  wxStaticBoxSizer* CreateStaticBoxControl(wxWindow* pWindow, const std::string& sName);
  /**
   * @brief Creates text control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   */
  void CreateTextControl(wxSizer* pParent, const std::string& sName);
  /**
   * @brief Creates text area control.
   * @param pParent Parent window or sizer that owns the created control.
   * @param sName Control name or lookup key.
   */
  void CreateTextAreaControl(wxSizer* pParent, const std::string& sName);

  /**
   * @brief Gets bool.
   * @param sName Control name or lookup key.
   * @return Bool value.
   */
  bool GetBool(const std::string& sName) const;
  /**
   * @brief Gets choice.
   * @param sName Control name or lookup key.
   * @return Choice value.
   */
  std::string GetChoice(const std::string& sName) const;
  /**
   * @brief Gets choice index.
   * @param sName Control name or lookup key.
   * @return Choice index value.
   */
  int GetChoiceIndex(const std::string& sName) const;
  /**
   * @brief Gets directory.
   * @param sName Control name or lookup key.
   * @return Directory value.
   */
  std::string GetDirectory(const std::string& sName) const;
  /**
   * @brief Gets double.
   * @param sName Control name or lookup key.
   * @return Double value.
   */
  double GetDouble(const std::string& sName) const;
  /**
   * @brief Gets float.
   * @param sName Control name or lookup key.
   * @return Float value.
   */
  float GetFloat(const std::string& sName) const;
  /**
   * @brief Gets int.
   * @param sName Control name or lookup key.
   * @return Int value.
   */
  int GetInt(const std::string& sName) const;
  /**
   * @brief Gets radio.
   * @param sName Control name or lookup key.
   * @return Radio value.
   */
  std::string GetRadio(const std::string& sName) const;
  /**
   * @brief Gets range.
   * @param sName Control name or lookup key.
   * @return Range value.
   */
  int GetRange(const std::string& sName) const;
  /**
   * @brief Gets multiple selections.
   * @param s S value.
   * @return Multiple selections value.
   */
  std::set<std::string> GetMultipleSelections(const std::string& s);
  /**
   * @brief Gets string.
   * @param sName Control name or lookup key.
   * @return String value.
   */
  std::string GetString(const std::string& sName) const;

  /**
   * @brief Notifies radio.
   * @param sName Control name or lookup key.
   */
  void NotifyRadio(const std::string& sName);
  /**
   * @brief Resets double.
   * @param sName Control name or lookup key.
   * @return Reset double value read from the control.
   */
  double ResetDouble(const std::string& sName);
  /**
   * @brief Resets float.
   * @param sName Control name or lookup key.
   * @return Reset float value read from the control.
   */
  float ResetFloat(const std::string& sName);
  /**
   * @brief Resets int.
   * @param sName Control name or lookup key.
   * @return Reset integer value read from the control.
   */
  int ResetInt(const std::string& sName);

  /**
   * @brief Sets string.
   * @param sName Control name or lookup key.
   * @param sText Text value to assign.
   */
  void SetString(const std::string& sName, const std::string& sText);
  /**
   * @brief Sets directory.
   * @param sName Control name or lookup key.
   * @param sPath Path text value.
   */
  void SetDirectory(const std::string& sName, const std::filesystem::path& sPath);
  /**
   * @brief Sets int.
   * @param sName Control name or lookup key.
   * @param n N value.
   */
  void SetInt(const std::string& sName, int n);
  /**
   * @brief Sets float.
   * @param sName Control name or lookup key.
   * @param f F value.
   */
  void SetFloat(const std::string& sName, float f);

  /**
   * @brief Selects choice.
   * @param sName Control name or lookup key.
   * @param nChoicePos Choice pos numeric value.
   */
  void SelectChoice(const std::string& sName, int nChoicePos);
  /**
   * @brief Selects choice.
   * @param sName Control name or lookup key.
   * @param sChoice Choice text value.
   */
  void SelectChoice(const std::string& sName, const wxString& sChoice);
  /**
   * @brief Updates choice control.
   * @param sName Control name or lookup key.
   * @param choices Choice labels used to populate the control.
   */
  void UpdateChoiceControl(const std::string& sName, wxArrayString choices);

protected:
  virtual void OnDirectorySelected(const std::string& sName) {};
  virtual void OnTextChanged(const std::string& sName) {};

private:
  std::unordered_map<std::string, wxCheckBox*> m_CheckBoxCtrls;
  std::unordered_map<std::string, wxComboBox*> m_ComboBoxCtrls;
  std::unordered_map<std::string, wxListBox*> m_ListBoxCtrls;
  std::unordered_map<std::string, wxTextCtrl*> m_TextCtrls;
  std::unordered_map<std::string, wxTextCtrl*> m_DirectoryCtrls;
  std::unordered_map<std::string, wxRadioBox*> m_RadioCtrls;
  std::unordered_map<std::string, wxSpinCtrl*> m_SpinCtrls;
  std::unordered_map<std::string, wxStaticText*> m_StaticText;
  std::unordered_map<std::string, wxStaticBox*> m_StaticBoxCtrls;
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
