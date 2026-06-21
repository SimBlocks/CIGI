//Copyright SimBlocks LLC 2016-2026
#include "TerrestrialSurfaceConditionsPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"
#include "HostEmulatorGuiApp.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostSession.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CTerrestrialSurfaceConditionsPanel, wxPanel)
  EVT_BUTTON(ID_TERRESTRIAL_SURFACE_CONDITION_SEND_PACKET, CTerrestrialSurfaceConditionsPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CTerrestrialSurfaceConditionsPanel::CTerrestrialSurfaceConditionsPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Terrestrial Surface Conditions Packet");

  CreateIntControl(vbox, "ID");
  CreateBoolControl(vbox, "Surface Conditions Enable");

  wxArrayString scopeOptions;
  scopeOptions.Add("Global");
  scopeOptions.Add("Regional");
  scopeOptions.Add("Entity");
  CreateChoiceControl(vbox, "Scope", scopeOptions);

  CreateRangeControl(vbox, "Severity", 0, 31);
  CreateRangeControl(vbox, "Coverage", 0, 100);

  wxArrayString surfaceConditionOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto surfaceConditions = pHost->GetScriptRuntime()->GetSurfaceConditions();
      for (auto itSurfaceCondition : surfaceConditions)
      {
        surfaceConditionOptions.Add(itSurfaceCondition);
      }
    }
  }

  CreateChoiceControl(vbox, "Surface Condition ID", surfaceConditionOptions);

  wxButton* pSendButton = new wxButton(this, ID_TERRESTRIAL_SURFACE_CONDITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CTerrestrialSurfaceConditionsPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  CHost* pHost = g_pHostEmulatorGuiApp != nullptr ? g_pHostEmulatorGuiApp->GetHost() : nullptr;
  if (pHost == nullptr || pHost->GetScriptRuntime() == nullptr)
  {
    return;
  }

  const bool bEnabled = GetBool("Surface Conditions Enable");
  CigiTerrestrialSurfaceSeverity severity = CigiTerrestrialSurfaceSeverity(GetRange("Severity"));
  const Percentage surfaceSeverity = ConvertToPercentage(severity);

  CigiIntPercentage coverage = CigiIntPercentage(GetRange("Coverage"));
  const Percentage surfaceCoverage = ConvertToPercentage(coverage);

  const SurfaceConditionID surfaceConditionID = pHost->GetScriptRuntime()->GetSurfaceConditionID(GetChoice("Surface Condition ID"));
  const ECigiScope eScope = ConvertCigiStringToScope(GetChoice("Scope"));

  CHostSession* pSession = GetHostSession();

  if (eScope == ECigiScope::ENTITY)
  {
    SCigiTerrestrialSurfaceCondition terrestrialSurfaceConditions;
    terrestrialSurfaceConditions.bEnabled = bEnabled;
    terrestrialSurfaceConditions.severity = surfaceSeverity;
    terrestrialSurfaceConditions.coverage = surfaceCoverage;
    terrestrialSurfaceConditions.surfaceConditionID = surfaceConditionID;
    EntityID entityID = EntityID(GetInt("ID"));
    pSession->SendEntityTerrestrialSurfaceConditionsControl(entityID, terrestrialSurfaceConditions);
  }
  else if (eScope == ECigiScope::REGIONAL)
  {
    SCigiTerrestrialSurfaceCondition terrestrialSurfaceConditions;
    terrestrialSurfaceConditions.bEnabled = bEnabled;
    terrestrialSurfaceConditions.severity = surfaceSeverity;
    terrestrialSurfaceConditions.coverage = surfaceCoverage;
    terrestrialSurfaceConditions.surfaceConditionID = surfaceConditionID;
    RegionID regionID = RegionID(GetInt("ID"));
    pSession->SendRegionTerrestrialSurfaceConditionsControl(regionID, terrestrialSurfaceConditions);
  }
  else if (eScope == ECigiScope::GLOBAL)
  {
    SCigiTerrestrialSurfaceCondition terrestrialSurfaceConditions;
    terrestrialSurfaceConditions.bEnabled = bEnabled;
    terrestrialSurfaceConditions.severity = surfaceSeverity;
    terrestrialSurfaceConditions.coverage = surfaceCoverage;
    terrestrialSurfaceConditions.surfaceConditionID = surfaceConditionID;
    pSession->SendGlobalTerrestrialSurfaceConditionsControl(terrestrialSurfaceConditions);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
