//Copyright SimBlocks LLC 2016-2026
#include "SensorControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;
using namespace sbio::utils;

// clang-format off
wxBEGIN_EVENT_TABLE(CSensorControlPanel, wxPanel)
  EVT_BUTTON(ID_SENSOR_CONTROL_SEND_PACKET, CSensorControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSensorControlPanel::CSensorControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Sensor Control Packet");

  CreateIntControl(vbox, "Sensor ID");

  wxArrayString trackModeOptions;
  trackModeOptions.Add("Off");
  trackModeOptions.Add("Force Correlate");
  trackModeOptions.Add("Scene");
  trackModeOptions.Add("Target");
  trackModeOptions.Add("Ship");
  CreateChoiceControl(vbox, "Track Mode", trackModeOptions);

  CreateBoolControl(vbox, "Sensor On");

  wxArrayString polarityOptions;
  polarityOptions.Add("White Hot");
  polarityOptions.Add("Black Hot");
  CreateChoiceControl(vbox, "Polarity", polarityOptions);

  CreateBoolControl(vbox, "Line-By-Line Dropout Enable");
  CreateBoolControl(vbox, "Automatic Gain");

  wxArrayString trackColorOptions;
  trackColorOptions.Add("White");
  trackColorOptions.Add("Black");
  CreateChoiceControl(vbox, "Track White/Black", trackColorOptions);

  CreateBoolControl(vbox, "Extended Response");
  CreateIntControl(vbox, "View ID");

  CreateFloatControl(vbox, "Gain");
  CreateFloatControl(vbox, "Level");
  CreateFloatControl(vbox, "AC Coupling");
  CreateFloatControl(vbox, "Noise");

  wxButton* pSendButton = new wxButton(this, ID_SENSOR_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSensorControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCigiSensorControl sensorControl;
  uint32_t sensorID = GetInt("Sensor ID");
  uint32_t viewID = GetInt("View ID");

  sensorControl.sensorID = SensorID(sensorID);
  sensorControl.bSensorOn = GetBool("Sensor On");
  sensorControl.eTrackMode = ConvertCigiStringToTrackMode(GetChoice("Track Mode"));
  sensorControl.ePolarity = ConvertCigiStringToPolarity(GetChoice("Polarity"));
  sensorControl.bLineByLineDropoutEnabled = GetBool("Line-By-Line Dropout Enable");
  sensorControl.bAutomaticGain = GetBool("Automatic Gain");
  sensorControl.eSensorTrack = ConvertCigiStringToSensorTrack(GetChoice("Track White/Black"));
  sensorControl.bExtendedResponse = GetBool("Extended Response");
  sensorControl.viewID = ViewID(viewID);

  sensorControl.gain = SensorGain(GetFloat("Gain"));
  sensorControl.level = SensorLevel(GetFloat("Level"));
  sensorControl.fACCoupling = GetFloat("AC Coupling");
  sensorControl.noise = SensorNoise(GetFloat("Noise"));

  HostCigiErrorEventArgs args;

  if (!sensorControl.gain.CheckValid())
  {
    args.sError = "Gain: Enter a number between 0 and 1 exclusively";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Gain: Enter a number between 0 and 1 exclusively");
    sensorControl.gain = SensorGain(ResetFloat("Gain"));
    return;
  }
  if (!sensorControl.level.CheckValid())
  {
    args.sError = "Level: Enter a number between 0 and 1 exclusively";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Level: Enter a number between 0 and 1 exclusively");
    sensorControl.level = SensorLevel(ResetFloat("Level"));
    return;
  }
  if (sensorControl.fACCoupling < 0)
  {
    args.sError = "AC Coupling: Enter a number greater than 0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("AC Coupling: Enter a number greater than 0");
    sensorControl.fACCoupling = ResetFloat("AC Coupling");
    return;
  }

  if (!sensorControl.noise.CheckValid())
  {
    args.sError = "Noise: Enter a number between 0 and 1 exclusively";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Noise: Enter a number between 0 and 1 exclusively");
    sensorControl.noise = SensorNoise(ResetFloat("Noise"));
    return;
  }

  CHostSession* pSession = GetHostSession();
  pSession->SendSensorControl(sensorControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
