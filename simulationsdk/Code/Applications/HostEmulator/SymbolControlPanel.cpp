//Copyright SimBlocks LLC 2016-2026
#include "SymbolControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "SymbolLib/SymbolTypes.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostCigiEvent.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::symbol;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolControlPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_CONTROL_SEND_PACKET, CSymbolControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolControlPanel::CSymbolControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Control Packet");

  CreateIntControl(vbox, "Symbol ID");
  CreateIntControl(vbox, "Parent Symbol ID");

  wxArrayString symbolStateOptions;
  symbolStateOptions.Add("Hidden");
  symbolStateOptions.Add("Visible");
  symbolStateOptions.Add("Destroyed");
  CreateChoiceControl(vbox, "Symbol State", symbolStateOptions);

  wxArrayString attachStateOptions;
  attachStateOptions.Add("Detach");
  attachStateOptions.Add("Attach");
  CreateChoiceControl(vbox, "Attach State", attachStateOptions);

  wxArrayString flashControlOptions;
  flashControlOptions.Add("Continue");
  flashControlOptions.Add("Reset");
  CreateChoiceControl(vbox, "Flash Control", flashControlOptions);

  CreateBoolControl(vbox, "Inherit Color");

  wxArrayString layerOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto layers = pHost->GetScriptRuntime()->GetLayers();
      for (auto itLayer : layers)
      {
        layerOptions.Add(itLayer);
      }
    }
  }
  CreateChoiceControl(vbox, "Layer", layerOptions);

  CreateIntControl(vbox, "Flash Duty Cycle Percentage");
  CreateIntControl(vbox, "Surface ID");

  CreateFloatControl(vbox, "Flash Period");
  CreateFloatControl(vbox, "Position U");
  CreateFloatControl(vbox, "Position V");
  CreateFloatControl(vbox, "Rotation");

  CreateIntControl(vbox, "Red");
  CreateIntControl(vbox, "Green");
  CreateIntControl(vbox, "Blue");
  CreateIntControl(vbox, "Alpha");

  CreateFloatControl(vbox, "Scale U");
  CreateFloatControl(vbox, "Scale V");

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CSymbolControlPanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SSymbolControl symbolControl;
  symbolControl.symbolID = SymbolID(GetInt("Symbol ID"));
  symbolControl.parentSymbolID = SymbolID(GetInt("Parent Symbol ID"));
  symbolControl.eSymbolState = ConvertCigiStringToSymbolState(GetChoice("Symbol State"));

  symbolControl.eAttachState = ConvertCigiStringToAttachState(GetChoice("Attach State"));
  symbolControl.eFlashControl = ConvertCigiStringToFlashControl(GetChoice("Flash Control"));
  symbolControl.bInheritColor = GetBool("Inherit Color");

  symbolControl.nLayerID = GetChoiceIndex("Layer");

  int flashDutyCyclePercentage = GetInt("Flash Duty Cycle Percentage");
  if (!CigiIntPercentage::CheckValid(flashDutyCyclePercentage))
  {
    HostCigiErrorEventArgs args;
    args.sError = "Flash Duty Cycle Percentage: Enter number between 0 and 100";
    Event::Raise<HostCigiEvent>(args);

    wxLogError(args.sError.c_str());
    flashDutyCyclePercentage = ResetInt("Flash Duty Cycle Percentage");
  }
  CigiIntPercentage cigiFlashDutyCyclePercentage(flashDutyCyclePercentage);
  symbolControl.flashDutyCyclePercentage = ConvertToPercentage(cigiFlashDutyCyclePercentage);

  symbolControl.surfaceID = SymbolSurfaceID(GetInt("Surface ID"));

  symbolControl.fFlashPeriod = GetFloat("Flash Period");
  symbolControl.fPositionU = GetFloat("Position U");
  symbolControl.fPositionV = GetFloat("Position V");
  symbolControl.fRotation = Degrees(GetFloat("Rotation"));

  symbolControl.color.r = GetInt("Red");
  symbolControl.color.g = GetInt("Green");
  symbolControl.color.b = GetInt("Blue");
  symbolControl.color.a = GetInt("Alpha");

  symbolControl.fScaleU = GetFloat("Scale U");
  symbolControl.fScaleV = GetFloat("Scale V");

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolControl(symbolControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
