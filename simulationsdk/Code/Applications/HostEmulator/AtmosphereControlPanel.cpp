//Copyright SimBlocks LLC 2016-2026
#include "AtmosphereControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "HostEmulatorGuiFrame.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CAtmosphereControlPanel, wxPanel)
  EVT_BUTTON(ID_ATMOSPHERE_CONTROL_SEND_PACKET, CAtmosphereControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CAtmosphereControlPanel::CAtmosphereControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Atmosphere Control");

  CreateBoolControl(vbox, "Atmospheric Model Enable");
  CreateIntControl(vbox, "Global Humidity");

  CreateFloatControl(vbox, "Global Air Temperature");
  CreateFloatControl(vbox, "Global Visibility Range");
  CreateFloatControl(vbox, "Global Horizontal Wind Speed");
  CreateFloatControl(vbox, "Global Vertical Wind Speed");
  CreateFloatControl(vbox, "Global Wind Direction");
  CreateFloatControl(vbox, "Global Barometric Pressure");

  wxButton* pSendButton = new wxButton(this, ID_ATMOSPHERE_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CAtmosphereControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  float min = 0.0;

  SCigiAtmosphereControl atmosphereControl;
  atmosphereControl.bAtmosphereModelEnable = GetBool("Atmospheric Model Enable");
  atmosphereControl.fGlobalAirTemp = GetFloat("Global Air Temperature");
  atmosphereControl.fGlobalBarometricPressure = GetFloat("Global Barometric Pressure");
  atmosphereControl.fGlobalHorizontalWindSpeed = GetFloat("Global Horizontal Wind Speed");
  atmosphereControl.fGlobalVerticalWindSpeed = GetFloat("Global Vertical Wind Speed");
  atmosphereControl.fGlobalVisibility = GetFloat("Global Visibility Range");

  HostCigiErrorEventArgs args;

  float globalWindDirection = GetFloat("Global Wind Direction");
  if (!Degrees::CheckValid(globalWindDirection))
  {
    args.sError = "Global Wind Direction: Enter number between 0.0 and 360.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Global Wind Direction: Enter number between 0.0 and 360.0");
    globalWindDirection = ResetFloat("Global Wind Direction");
  }
  atmosphereControl.globalWindDirection = Degrees(globalWindDirection);

  int globalHumidity = GetInt("Global Humidity");
  if (!CigiIntPercentage::CheckValid(globalHumidity))
  {
    args.sError = "Global Humidity: Enter number between 0 and 100";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Global Humidity: Enter number between 0 and 100");
    globalHumidity = ResetInt("Global Humidity");
  }
  CigiIntPercentage cigiGlobalHumidity(globalHumidity);
  atmosphereControl.globalHumidity = ConvertToPercentage(cigiGlobalHumidity);

  if (atmosphereControl.fGlobalVisibility < min)
  {
    args.sError = "Global Visibility Range: Enter number greater than or equal to 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Global Visibility Range: Enter number greater than or equal to 0");
    atmosphereControl.fGlobalVisibility = ResetInt("Global Visibility Range");
  }
  if (atmosphereControl.fGlobalHorizontalWindSpeed < min)
  {
    args.sError = "Global Horizontal Wind Speed: Enter number greater than or equal to 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Global Horizontal Wind Speed: Enter number greater than or equal to 0");
    atmosphereControl.fGlobalHorizontalWindSpeed = ResetInt("Global Horizontal Wind Speed");
  }

  if (atmosphereControl.fGlobalBarometricPressure < min)
  {
    args.sError = "Global Barometric Pressure: Enter number greater than or equal to 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Global Barometric Pressure: Enter number greater than or equal to 0");
    atmosphereControl.fGlobalBarometricPressure = ResetInt("Global Barometric Pressure");
  }

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendAtmosphereControl(atmosphereControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
