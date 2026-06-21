//Copyright SimBlocks LLC 2016-2026
/**
 * @file ScriptsPage.h
 * @brief Declares the CScriptsPage class for script management in the HostEmulator application.
 *
 * Provides the CScriptsPage class for managing and running scripts from the HostEmulator GUI.
 * Integrates with wxWidgets for GUI management and supports category selection, script listing, and script execution.
 * Inherits from CNotebookPage for notebook integration and provides event handlers for script actions.
 *
 * @see CScriptsPage
 * @see CNotebookPage
 */
#pragma once
#ifndef SIMBLOCKS_HOST_EMULATOR_SCRIPTS_PAGE_H
#define SIMBLOCKS_HOST_EMULATOR_SCRIPTS_PAGE_H

#include "wxWidgetsUtilities/NotebookPage.h"
#include <unordered_map>
#include <wx/wx.h>

/**
 * @brief Scripts page for managing and running scripts in the HostEmulator GUI.
 *
 * Supports category selection, script listing, and script execution.
 */
class CScriptsPage : public CNotebookPage
{
public:
  /**
   * @brief Constructs the scripts page.
   * @param pParent Parent window pointer.
   */
  CScriptsPage(wxWindow* pParent);

  /**
   * @brief Called when the page is selected in the notebook.
   */
  virtual void OnSelected() override;

  /**
   * @brief Updates gui.
   */
  void UpdateGUI();

  DECLARE_EVENT_TABLE()
  /**
   * @brief Handles the categories combo box event.
   * @param event The command event.
   */
  void OnCategoriesComboBox(wxCommandEvent& event);
  /**
   * @brief Handles the script list box event.
   * @param event The command event.
   */
  void OnListBox(wxCommandEvent& event);
  /**
   * @brief Handles the reset script button event.
   * @param event The command event.
   */
  void OnResetScript(wxCommandEvent& event);
  /**
   * @brief Handles the run script button event.
   * @param event The command event.
   */
  void OnRunScript(wxCommandEvent& event);
  /**
   * @brief Handles the stop script button event.
   * @param event The command event.
   */
  void OnStopScript(wxCommandEvent& event);

  /**
   * @brief Handles the edit script event.
   * @param event Event object associated with the callback.
   */
  void OnEditScript(wxCommandEvent& event);

  /**
   * @brief Gets the currently selected script category.
   * @return The selected category string.
   */
  std::string GetCategory();
  /**
   * @brief Gets the script names for a given category.
   * @param sCategory The category string.
   * @return Array of script names.
   */
  wxArrayString GetScriptNames(std::string sCategory);

  /**
   * @brief Updates script state label.
   */
  void UpdateScriptStateLabel();

private:
  /**
   * @brief Gets script categories.
   * @return Script categories value.
   */
  wxArrayString GetScriptCategories();
  /**
   * @brief Constructs a `RefreshCategories` instance.
   */
  void RefreshCategories();
  /**
   * @brief Constructs a `RefreshScriptList` instance.
   * @param sCategory Category text value.
   */
  void RefreshScriptList(std::string sCategory);
  /**
   * @brief Selects first script.
   * @param pListBox List box pointer used by the operation.
   * @param sSelectedScriptFile Selected script file text value.
   */
  void SelectFirstScript(wxListBox* pListBox, std::string& sSelectedScriptFile);

private:
  std::string m_sSelectedScriptFile;///< Currently selected script file
  wxComboBox* m_pCategoriesComboBox;///< Combo box for script categories
  wxListBox* m_pListBox;///< List box for script names
  wxStaticText* m_pScriptStateLabel = nullptr;// Label to display script running state
};

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
