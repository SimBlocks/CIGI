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
 * Seeds controls from the selected session when available and reinitializes the host on acceptance. Child windows
 * belong to the dialog. Numeric accessors expose parsed control values, not independently validated configuration.
 */
class SetupDialog : public wxDialog
{
public:
  /**
   * @brief Creates an unparented setup dialog and populates controls from the application's available host settings.
   * @param title Window caption.
   */
  SetupDialog(const wxString& title);
  /**
   * @brief Validates IPv4 text, port ranges, and database range, then reinitializes the host and ends with wxID_OK.
   * @param event Unused command event; values are read from the dialog's controls.
   *
   * Updates top-level defaults and the selected session entry, appending a missing selection only when the session
   * list was already nonempty. Invalid input raises a host error and displays a message without closing. Does nothing
   * when no host is available. Host initialization exceptions propagate.
   */
  void OnOk(wxCommandEvent& event);
  /**
   * @brief Asks for cancellation confirmation and ends with wxID_CANCEL only when the user answers Yes.
   * @param e Unused command event. Does not apply new host settings.
   */
  void OnCancel(wxCommandEvent& e);

  /**
   * @brief Reads the IG receive-port field, corresponding to hostToIGPort.
   * @return ToLong conversion output, initialized to zero; conversion status and port range are not checked here.
   */
  long GetReceivePort() const;
  /**
   * @brief Reads the host receive-port field, corresponding to igToHostPort.
   * @return ToLong conversion output, initialized to zero; conversion status and port range are not checked here.
   */
  long GetLocalReceivePort() const;
  /**
   * @brief Reads the default-database text field.
   * @return ToLong conversion output, initialized to zero; conversion status and database range are not checked here.
   */
  long GetDefaultDatabase() const;
  /**
   * @brief Reads the IG address field without validation.
   * @return Copy of the current address text.
   */
  wxString GetIPAddress() const;
  /**
   * @brief Reads the read-only protocol version field.
   * @return Version text, normally "3.3" or "4.0", or empty when initialized with an unknown version.
   */
  wxString GetVersionID() const;
  /**
   * @brief Reads the byte-order checkbox.
   * @return True when big-endian byte order is selected; false otherwise.
   */
  bool GetBigEndianByteOrder() const;
  /**
   * @brief Tests the address text against the dialog's anchored dotted-decimal IPv4 expression.
   * @return True for four decimal octets in [0, 255]; does not resolve names or test connectivity.
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

  wxRegEx* regexIpAddress =
    new wxRegEx("^(([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]{1}|[0-9]{2}|[0-1][0-9]{2}|2[0-4][0-9]|25[0-5])$");///< Regex for IP address validation
  wxString ipAddressFilter[11] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "."};///< Allowed characters for IP address
  wxTextValidator* txtvldIPAddress = new wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);///< Validator for IP address input

  wxButton* okButton;///< OK button
  wxButton* cancelButton;///< Cancel button

  wxDECLARE_EVENT_TABLE();
};

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
