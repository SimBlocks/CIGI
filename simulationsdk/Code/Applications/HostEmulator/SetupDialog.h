//Copyright SimBlocks LLC 2016-2026
/**
 * @file SetupDialog.h
 * @brief Declares the SetupDialog class for the HostEmulator application's setup dialog.
 *
 * Provides the SetupDialog class for managing the setup dialog window, user input controls, and validation logic
 * for configuring host and IG connection options. Integrates with wxWidgets for GUI management and SimBlocks SDK
 * for simulation interoperability.
 *
 * @see SetupDialog
 * @see CHostEmulatorGuiApp
 * @see CHostEmulatorGuiFrame
 */
#pragma once
#include "HostEmulatorGuiApp.h"
#include "HostEmulatorGuiFrame.h"
#include "HostCigiLib/HostCigiLibTypes.h"
#include <wx/regex.h>
#include <wx/wx.h>

/**
 * @brief Setup dialog for configuring host and IG connection options.
 *
 * Manages user input controls and validation for host/IG configuration.
 */
class SetupDialog : public wxDialog
{
public:
  /**
   * @brief Constructs a `SetupDialog` instance.
   * @param title Title value.
   */
  SetupDialog(const wxString& title);
  /**
   * @brief Handles the ok event.
   * @param event Event object associated with the callback.
   */
  void OnOk(wxCommandEvent& event);
  /**
   * @brief Handles the cancel event.
   * @param e E value.
   */
  void OnCancel(wxCommandEvent& e);

  /**
   * @brief Gets receive port.
   * @return Receive port value.
   */
  long GetReceivePort() const;
  /**
   * @brief Gets local receive port.
   * @return Local receive port value.
   */
  long GetLocalReceivePort() const;
  /**
   * @brief Gets default database.
   * @return Default database value.
   */
  long GetDefaultDatabase() const;
  /**
   * @brief Gets ipaddress.
   * @return Ipaddress value.
   */
  wxString GetIPAddress() const;
  /**
   * @brief Gets version id.
   * @return Version id value.
   */
  wxString GetVersionID() const;
  /**
   * @brief Gets big endian byte order.
   * @return Big endian byte order value.
   */
  bool GetBigEndianByteOrder() const;
  /**
   * @brief Determines whether ipaddress valid.
   * @return `true` when the condition is met; otherwise `false`.
   */
  bool IsIPAddressValid() const;

private:
  /**
   * @brief Gets session setup options.
   * @param options Options value.
   * @param sessionID Session identifier.
   * @return Session setup options value.
   */
  sbio::cigi::host::SHostSessionSetupOptions GetSessionSetupOptions(const sbio::cigi::host::SHostSetupOptions& options, sbio::SessionID sessionID) const;

  wxPanel* panel;///< Main panel
  wxBoxSizer* hbox;///< Horizontal box sizer
  wxBoxSizer* vbox;///< Vertical box sizer

  wxStaticText* text1;///< Label for IP address
  wxStaticText* text2;///< Label for receive port
  wxStaticText* text3;///< Label for local receive port
  wxStaticText* text4;///< Label for version ID
  wxStaticText* text5;///< Label for default database
  wxStaticText* text6;///< Label for big endian byte order
  wxCheckBox* checkbox;///< Checkbox for big endian byte order

  wxTextCtrl* ipAddressControl;///< Input for IP address
  wxTextCtrl* receivePortControl;///< Input for receive port
  wxTextCtrl* localReceivePortControl;///< Input for local receive port
  wxTextCtrl* versionIDControl;///< Input for version ID
  wxTextCtrl* defaultDatabaseControl;///< Input for default database

  wxRegEx* regexIpAddress = new wxRegEx("^(([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])$");///< Regex for IP address validation
  wxString ipAddressFilter[11] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "."};///< Allowed characters for IP address
  wxTextValidator* txtvldIPAddress = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);///< Validator for IP address input

  wxButton* okButton;///< OK button
  wxButton* cancelButton;///< Cancel button

  /**
   * @brief Constructs a `wxDECLARE_EVENT_TABLE` instance.
   */
  wxDECLARE_EVENT_TABLE();
};

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
