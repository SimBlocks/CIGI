//Copyright SimBlocks LLC 2016-2026
#include "CelestialSphereControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CCelestialSphereControlPanel, wxPanel)
  EVT_BUTTON(ID_CELESTIAL_SPHERE_CONTROL_SEND_PACKET, CCelestialSphereControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CCelestialSphereControlPanel::CCelestialSphereControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Celestial Sphere Control");

  CreateBoolControl(vbox, "Continuous Time-of-Day Enable");
  CreateBoolControl(vbox, "Sun Enable");
  CreateBoolControl(vbox, "Moon Enable");
  CreateBoolControl(vbox, "Star Field Enable");
  CreateBoolControl(vbox, "Date/Time Valid");

  CreateIntControl(vbox, "Hour");
  CreateIntControl(vbox, "Minute");
  CreateFloatControl(vbox, "Seconds");
  CreateIntControl(vbox, "Date: Year", "2026");
  CreateIntControl(vbox, "Date: Month", "1");
  CreateIntControl(vbox, "Date: Day", "1");

  CreateFloatControl(vbox, "Star Field Intensity");

  wxButton* pSendButton = new wxButton(this, ID_CELESTIAL_SPHERE_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CCelestialSphereControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCigiCelestialSphereControl celestialSphereControl;
  celestialSphereControl.bContinuousTimeOfDayEnable = GetBool("Continuous Time-of-Day Enable");
  celestialSphereControl.bSunEnable = GetBool("Sun Enable");
  celestialSphereControl.bMoonEnable = GetBool("Moon Enable");
  celestialSphereControl.bStarFieldEnable = GetBool("Star Field Enable");
  celestialSphereControl.bDateTimeValid = GetBool("Date/Time Valid");

  HostCigiErrorEventArgs args;

  int hour = GetInt("Hour");
  if (!Hour::CheckValid(hour))
  {
    args.sError = "Hour: Enter number between 0 and 23";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Hour: Enter number between 0 and 23");
    hour = ResetInt("Hour");
  }
  celestialSphereControl.hour = Hour(hour);

  int minute = GetInt("Minute");
  if (!Minute::CheckValid(minute))
  {
    args.sError = "Minute: Enter number between 0 and 59";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Minute: Enter number between 0 and 59");
    minute = ResetInt("Minute");
  }
  celestialSphereControl.minute = Minute(minute);

  float seconds = GetFloat("Seconds");
  if (!Second::CheckValid(seconds))
  {
    args.sError = "Seconds: Enter number between 0.0 and 59.99999";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Seconds: Enter number between 0.0 and 59.99999");
    seconds = ResetFloat("Seconds");
  }
  celestialSphereControl.second = Second(seconds);

  celestialSphereControl.year = Year(GetInt("Date: Year"));

  int month = GetInt("Date: Month");
  if (!CigiMonth::CheckValid(month))
  {
    args.sError = "Month: Enter number between 1 and 12";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Month: Enter number between 1 and 12");
    month = ResetInt("Date: Month");
  }
  CigiMonth cigiMonth = CigiMonth(month);
  celestialSphereControl.month = ConvertToMonth(cigiMonth);

  int day = (GetInt("Date: Day"));
  if (!Day::CheckValid(day))
  {
    args.sError = "Day: Enter number between 1 and 31";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Day: Enter number between 1 and 31");
    day = ResetInt("Date: Day");
  }
  celestialSphereControl.day = Day(day);

  float starfieldIntensity = GetFloat("Star Field Intensity");
  if (!CigiFloatPercentage::CheckValid(starfieldIntensity))
  {
    args.sError = "Star Field Intensity: Enter number between 0.0 and 100.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Star Field Intensity: Enter number between 0.0 and 100.0");
    starfieldIntensity = ResetFloat("Star Field Intensity");
  }
  CigiFloatPercentage cigiStarfieldIntensity = CigiFloatPercentage(starfieldIntensity);
  celestialSphereControl.starFieldIntensity = ConvertToPercentage(cigiStarfieldIntensity);

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendCelestialSphereControl(celestialSphereControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
