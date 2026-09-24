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
 * Lists categories beneath g_globals.applicationsDataPath / "HostEmulator" / "Scripts" and delegates execution to
 * the application's script runtime. Files are displayed by stem without filtering extensions; selection constructs
 * a .chai filename. Filesystem enumeration errors propagate. wxWidgets owns the page's child controls.
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
   * @brief Re-enumerates categories and scripts when the page is selected, preserving selections where possible.
   */
  virtual void OnSelected() override;

  /**
   * @brief Refreshes the running/stopped label; does not rescan scripts.
   */
  void UpdateGUI();

  DECLARE_EVENT_TABLE()
  /**
   * @brief Refreshes the script list for the category supplied by the event.
   * @param event Selection event whose string supplies the category.
   */
  void OnCategoriesComboBox(wxCommandEvent& event);
  /**
   * @brief Stores the selected script stem with a .chai suffix, or clears selection for empty text.
   * @param event Selection event supplying the displayed script name.
   */
  void OnListBox(wxCommandEvent& event);
  /**
   * @brief Sends a reset through the script runtime when the host and runtime are available.
   * @param event Unused command event.
   */
  void OnResetScript(wxCommandEvent& event);
  /**
   * @brief Executes the selected category/file unless a script is already running.
   * @param event Unused command event.
   *
   * Missing host/runtime is a no-op. An already-running script produces an error event and message box. Selection
   * validity is delegated to the runtime; this callback does not require a nonempty selection before Execute().
   */
  void OnRunScript(wxCommandEvent& event);
  /**
   * @brief Stops the script runtime if the host and runtime are available.
   * @param event Unused command event.
   */
  void OnStopScript(wxCommandEvent& event);

  /**
   * @brief Opens the selected existing .chai path using Windows ShellExecute's default open action.
   * @param event Unused command event; empty selections and nonexistent paths are ignored.
   */
  void OnEditScript(wxCommandEvent& event);

  /**
   * @brief Gets the currently selected script category.
   * @return Selected category, falling back to editable text and then the first entry; empty if none is available.
   */
  std::string GetCategory();
  /**
   * @brief Gets the script names for a given category.
   * @param sCategory Category path component beneath the scripts root.
   * @return Sorted stems of nondirectory entries, without extension filtering or deduplication; empty for an empty
   * category or a nonexistent path. Other filesystem errors propagate.
   */
  wxArrayString GetScriptNames(std::string sCategory);

  /**
   * @brief Displays runtime running/stopped state, or leaves the label unchanged if host, runtime, or label is absent.
   */
  void UpdateScriptStateLabel();

private:
  /**
   * @brief Enumerates category directories immediately beneath the scripts root.
   * @return Sorted directory names, or an empty array when the root is absent. Other filesystem errors propagate.
   */
  wxArrayString GetScriptCategories();
  /**
   * @brief Rebuilds the category list, retaining the current category if present, otherwise selecting the first.
   * Also refreshes the script list; an empty category list clears both selections.
   */
  void RefreshCategories();
  /**
   * @brief Rebuilds scripts for a category, retaining the previous stem if found, otherwise selecting the first.
   * @param sCategory Category to enumerate; an empty or absent category clears the list and stored filename.
   */
  void RefreshScriptList(std::string sCategory);
  /**
   * @brief Selects the first list entry and forms its .chai filename.
   * @param pListBox Borrowed list control; may be null.
   * @param sSelectedScriptFile Output filename, cleared first and left empty for a null/empty list or empty first entry.
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
