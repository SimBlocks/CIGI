//Copyright SimBlocks LLC 2016-2026
#include "MaritimeSurfaceControlPanel.h"
#include "CigiLib/CigiConversions.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CMaritimeSurfaceControlPanel, wxPanel)
  EVT_BUTTON(ID_MARITIME_SURFACE_CONTROL_SEND_PACKET, CMaritimeSurfaceControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CMaritimeSurfaceControlPanel::CMaritimeSurfaceControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Maritime Surface Control Packet");

  CreateBoolControl(vbox, "Surface Conditions Enable");
  CreateBoolControl(vbox, "Whitecap Enable");

  wxArrayString scopeOptions;
  scopeOptions.Add("Global");
  scopeOptions.Add("Regional");
  scopeOptions.Add("Entity");
  CreateChoiceControl(vbox, "Scope", scopeOptions);

  CreateIntControl(vbox, "ID");

  CreateFloatControl(vbox, "Sea Surface Height");
  CreateFloatControl(vbox, "Surface Water Temperature");
  CreateFloatControl(vbox, "Surface Clarity");

  wxButton* pSendButton = new wxButton(this, ID_MARITIME_SURFACE_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CMaritimeSurfaceControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCigiMaritimeSurfaceCondition maritimeSurfaceCondition;
  maritimeSurfaceCondition.bActive = GetBool("Surface Conditions Enable");
  maritimeSurfaceCondition.bWhitecapEnabled = GetBool("Whitecap Enable");
  maritimeSurfaceCondition.fSeaSurfaceHeight = HeightRelativeToWGS84Ellipsoid(GetFloat("Sea Surface Height"));
  maritimeSurfaceCondition.fSurfaceWaterTemperature = TemperatureCelsius(GetFloat("Surface Water Temperature"));

  CigiFloatPercentage surfaceClarity = CigiFloatPercentage(GetFloat("Surface Clarity"));
  maritimeSurfaceCondition.surfaceClarity = ConvertToPercentage(surfaceClarity);

  ECigiScope eScope = ConvertCigiStringToScope(GetChoice("Scope"));

  CHostSession* pSession = GetHostSession();

  if (eScope == ECigiScope::ENTITY)
  {
    EntityID entityID = EntityID(GetInt("ID"));
    pSession->SendEntityMaritimeSurfaceConditionsControl(entityID, maritimeSurfaceCondition);
  }
  else if (eScope == ECigiScope::REGIONAL)
  {
    RegionID regionID = RegionID(GetInt("ID"));
    pSession->SendRegionMaritimeSurfaceConditionsControl(regionID, maritimeSurfaceCondition);
  }
  else if (eScope == ECigiScope::GLOBAL)
  {
    pSession->SendGlobalMaritimeSurfaceConditionsControl(maritimeSurfaceCondition);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
