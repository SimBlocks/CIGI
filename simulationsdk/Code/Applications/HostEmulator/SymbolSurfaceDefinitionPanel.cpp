//Copyright SimBlocks LLC 2016-2026
#include "SymbolSurfaceDefinitionPanel.h"
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
using namespace sbio::entity;
using namespace sbio::symbol;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolSurfaceDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_SURFACE_DEFINITION_SEND_PACKET, CSymbolSurfaceDefinitionPanel::OnSend)
  EVT_RADIOBOX(ID_SYMBOL_SURFACE_DEFINITION_ENTITY_VIEW_ID_RADIO_BUTTON_EVENT, CSymbolSurfaceDefinitionPanel::OnEntityViewIDRadio)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolSurfaceDefinitionPanel::CSymbolSurfaceDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Surface Definition Packet");

  CreateIntControl(vbox, "Surface ID");

  wxArrayString surfaceStateOptions;
  surfaceStateOptions.Add("Active");
  surfaceStateOptions.Add("Destroyed");
  CreateChoiceControl(vbox, "Surface State", surfaceStateOptions);

  wxArrayString attachTypeOptions;
  attachTypeOptions.Add("Entity");
  attachTypeOptions.Add("View");
  CreateRadioControl(vbox, "Attach Type", attachTypeOptions, ID_SYMBOL_SURFACE_DEFINITION_ENTITY_VIEW_ID_RADIO_BUTTON_EVENT);
  CreateIntControl(vbox, "Entity ID");

  wxArrayString billboardOptions;
  billboardOptions.Add("Non-Billboard");
  billboardOptions.Add("Billboard");
  CreateChoiceControl(vbox, "Billboard", billboardOptions);

  CreateBoolControl(vbox, "Perspective Growth Enable");

  CreateFloatControl(vbox, "X Offset");
  CreateFloatControl(vbox, "Y Offset");
  CreateFloatControl(vbox, "Z Offset");
  CreateFloatControl(vbox, "Yaw");

  CreateFloatControl(vbox, "Pitch");
  CreateFloatControl(vbox, "Roll");
  CreateFloatControl(vbox, "Width");
  CreateFloatControl(vbox, "Height");

  CreateFloatControl(vbox, "Min U");
  CreateFloatControl(vbox, "Max U");
  CreateFloatControl(vbox, "Min V");
  CreateFloatControl(vbox, "Max V");

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_SURFACE_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CSymbolSurfaceDefinitionPanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);

  NotifyRadio("Attach Type");
}

void CSymbolSurfaceDefinitionPanel::OnEntityViewIDRadio(wxCommandEvent& event)
{
  if (event.GetString() == "Entity")
  {
    ChangeText("Entity ID", "Entity ID");
    ChangeText("X Offset", "X Offset");
    ChangeText("Y Offset", "Y Offset");
    ChangeText("Z Offset", "Z Offset");
    ChangeText("Yaw", "Yaw");
  }
  else
  {
    ChangeText("Entity ID", "View ID");
    ChangeText("X Offset", "Left");
    ChangeText("Y Offset", "Right");
    ChangeText("Z Offset", "Top");
    ChangeText("Yaw", "Bottom");
  }
}

void CSymbolSurfaceDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  EAttachType eAttachType = ConvertCigiStringToAttachType(GetRadio("Attach Type"));

  CHostSession* pSession = GetHostSession();

  if (eAttachType == EAttachType::ENTITY)
  {
    EBillboard eBillboard = ConvertCigiStringToBillboard(GetChoice("Billboard"));

    if (eBillboard == EBillboard::NON_BILLBOARD)
    {
      SEntitySymbolSurfaceDefinition symbolSurfaceDefinition;
      symbolSurfaceDefinition.surfaceID = SymbolSurfaceID(GetInt("Surface ID"));
      symbolSurfaceDefinition.entityID = EntityID(GetInt("Entity ID"));
      symbolSurfaceDefinition.eSurfaceState = ConvertCigiStringToSurfaceState(GetChoice("Surface State"));
      symbolSurfaceDefinition.offsetToSurface[0] = GetFloat("X Offset");
      symbolSurfaceDefinition.offsetToSurface[1] = GetFloat("Y Offset");
      symbolSurfaceDefinition.offsetToSurface[2] = GetFloat("Z Offset");
      symbolSurfaceDefinition.yaw = Degrees(GetFloat("Yaw"));
      symbolSurfaceDefinition.pitch = Degrees90(GetFloat("Pitch"));
      symbolSurfaceDefinition.roll = Degrees180(GetFloat("Roll"));
      symbolSurfaceDefinition.width = GetFloat("Width");
      symbolSurfaceDefinition.height = GetFloat("Height");
      symbolSurfaceDefinition.uvMin[0] = GetFloat("Min U");
      symbolSurfaceDefinition.uvMax[0] = GetFloat("Max U");
      symbolSurfaceDefinition.uvMin[1] = GetFloat("Min V");
      symbolSurfaceDefinition.uvMax[1] = GetFloat("Max V");

      pSession->SendEntitySymbolSurfaceDefinition(symbolSurfaceDefinition);
    }
    else if (eBillboard == EBillboard::BILLBOARD)
    {
      SEntityBillboardSymbolSurfaceDefinition symbolSurfaceDefinition;
      symbolSurfaceDefinition.surfaceID = SymbolSurfaceID(GetInt("Surface ID"));
      symbolSurfaceDefinition.entityID = EntityID(GetInt("Entity ID"));
      symbolSurfaceDefinition.eSurfaceState = ConvertCigiStringToSurfaceState(GetChoice("Surface State"));
      symbolSurfaceDefinition.bPerspectiveGrowthEnabled = GetBool("Perspective Growth Enable");
      symbolSurfaceDefinition.offsetToEntity[0] = GetFloat("X Offset");
      symbolSurfaceDefinition.offsetToEntity[1] = GetFloat("Y Offset");
      symbolSurfaceDefinition.offsetToEntity[2] = GetFloat("Z Offset");
      symbolSurfaceDefinition.width = GetFloat("Width");
      symbolSurfaceDefinition.height = GetFloat("Height");
      symbolSurfaceDefinition.uvMin[0] = GetFloat("Min U");
      symbolSurfaceDefinition.uvMax[0] = GetFloat("Max U");
      symbolSurfaceDefinition.uvMin[1] = GetFloat("Min V");
      symbolSurfaceDefinition.uvMax[1] = GetFloat("Max V");

      pSession->SendEntityBillboardSymbolSurfaceDefinition(symbolSurfaceDefinition);
    }
  }
  else if (eAttachType == EAttachType::VIEW)
  {
    SViewSymbolSurfaceDefinition symbolSurfaceDefinition;
    symbolSurfaceDefinition.surfaceID = SymbolSurfaceID(GetInt("Surface ID"));
    symbolSurfaceDefinition.viewID = ViewID(GetInt("Entity ID"));
    symbolSurfaceDefinition.eSurfaceState = ConvertCigiStringToSurfaceState(GetChoice("Surface State"));
    symbolSurfaceDefinition.fLeft = GetFloat("X Offset");
    symbolSurfaceDefinition.fRight = GetFloat("Y Offset");
    symbolSurfaceDefinition.fTop = GetFloat("Z Offset");
    symbolSurfaceDefinition.fBottom = GetFloat("Yaw");
    symbolSurfaceDefinition.width = GetFloat("Width");
    symbolSurfaceDefinition.height = GetFloat("Height");
    symbolSurfaceDefinition.uvMin[0] = GetFloat("Min U");
    symbolSurfaceDefinition.uvMax[0] = GetFloat("Max U");
    symbolSurfaceDefinition.uvMin[1] = GetFloat("Min V");
    symbolSurfaceDefinition.uvMax[1] = GetFloat("Max V");

    pSession->SendViewSymbolSurfaceDefinition(symbolSurfaceDefinition);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
