//Copyright SimBlocks LLC 2016-2026
#include "WaveControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CWaveControlPanel, wxPanel)
  EVT_BUTTON(ID_WAVE_CONTROL_SEND_PACKET, CWaveControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CWaveControlPanel::CWaveControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Wave Control Packet");

  CreateIntControl(vbox, "Wave ID");
  CreateBoolControl(vbox, "Wave Enable");

  wxArrayString scopeOptions;
  scopeOptions.Add("Global");
  scopeOptions.Add("Regional");
  scopeOptions.Add("Entity");
  CreateChoiceControl(vbox, "Scope", scopeOptions);

  wxArrayString breakerTypeOptions;
  breakerTypeOptions.Add("Plunging");
  breakerTypeOptions.Add("Spilling");
  breakerTypeOptions.Add("Surging");
  CreateChoiceControl(vbox, "Breaker Type", breakerTypeOptions);

  CreateIntControl(vbox, "ID");

  CreateFloatControl(vbox, "Wave Height");
  CreateFloatControl(vbox, "Wavelength");
  CreateFloatControl(vbox, "Period");
  CreateFloatControl(vbox, "Direction");
  CreateFloatControl(vbox, "Phase Offset");
  CreateFloatControl(vbox, "Leading");

  wxButton* pSendButton = new wxButton(this, ID_WAVE_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CWaveControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCigiWaveCondition waveCondition;

  waveCondition.waveID = GetInt("Wave ID");
  waveCondition.bWaveEnabled = GetBool("Wave Enable");
  waveCondition.eBreakerType = ConvertCigiStringToWaveBreakerType(GetChoice("Breaker Type"));
  waveCondition.fWaveHeight = GetFloat("Wave Height");
  waveCondition.fWavelength = GetFloat("Wavelength");
  waveCondition.fPeriod = GetFloat("Period");
  waveCondition.direction = Degrees360(GetFloat("Direction"));
  waveCondition.phaseOffset = Degrees360(GetFloat("Phase Offset"));
  waveCondition.leading = Degrees180(GetFloat("Leading"));

  int nID = GetInt("ID");
  ECigiScope scope = ConvertCigiStringToScope(GetChoice("Scope"));

  CHostSession* pSession = GetHostSession();

  if (scope == ECigiScope::ENTITY)
  {
    EntityID entityID(nID);
    pSession->SendEntityWaveControl(entityID, waveCondition);
  }
  else if (scope == ECigiScope::REGIONAL)
  {
    RegionID regionID(nID);
    pSession->SendRegionalWaveControl(regionID, waveCondition);
  }
  else if (scope == ECigiScope::GLOBAL)
  {
    pSession->SendGlobalWaveControl(waveCondition);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
