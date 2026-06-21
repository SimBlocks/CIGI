//Copyright SimBlocks LLC 2016-2026
#include "EntityControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EntityLib/EntityTypes.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CEntityControlPanel, wxPanel)
  EVT_BUTTON(ID_ENTITY_CONTROL_SEND_PACKET, CEntityControlPanel::OnSend)
  EVT_COMBOBOX(ID_ENTITY_CONTROL_ENTITY_TYPE, CEntityControlPanel::OnEntityTypeSelection)
wxEND_EVENT_TABLE();
// clang-format on

CEntityControlPanel::CEntityControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Entity Control Panel");

  wxArrayString entityStateOptions;
  entityStateOptions.Add("Inactive");
  entityStateOptions.Add("Active");
  entityStateOptions.Add("Destroyed");
  CreateChoiceControl(vbox, "Entity State", entityStateOptions);

  CreateBoolControl(vbox, "Collision Reporting Enable");
  CreateBoolControl(vbox, "Inherit Alpha");
  CreateBoolControl(vbox, "Smoothing Enable");

  CreateIntControl(vbox, "Alpha");
  CreateIntControl(vbox, "Entity ID");

  wxArrayString extendedEntityOptions;
  extendedEntityOptions.Add("Short");
  extendedEntityOptions.Add("Extended");
  CreateChoiceControl(vbox, ID_ENTITY_CONTROL_ENTITY_TYPE, "Entity Type", extendedEntityOptions);

  m_ShortEntityTypeSizer = new wxBoxSizer(wxVERTICAL);
  CreateIntControl(m_ShortEntityTypeSizer, "Short Entity Type");
  vbox->Add(m_ShortEntityTypeSizer);

  m_ExtendedEntityTypeSizer = new wxBoxSizer(wxVERTICAL);

  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Kind");
  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Domain");
  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Country");
  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Category");
  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Subcategory");
  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Specific");
  CreateIntControl(m_ExtendedEntityTypeSizer, "Entity Extra");
  vbox->Add(m_ExtendedEntityTypeSizer);

  wxButton* pSendButton = new wxButton(this, ID_ENTITY_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);

  m_ExtendedEntityTypeSizer->Show(false);
}

sbio::cigi::EExtendedEntityType ConvertCigiStringToExtendedEntityType(std::string type)
{
  if (type.compare("Short") == 0)
  {
    return sbio::cigi::EExtendedEntityType::SHORT;
  }
  return sbio::cigi::EExtendedEntityType::EXTENDED;
}

void CEntityControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  sbio::cigi::SEntityControl entityControl;

  string sEntityState = GetChoice("Entity State");
  entityControl.eState = ConvertCigiStringToActiveState(sEntityState);
  entityControl.bCollisionReportingEnabled = GetBool("Collision Reporting Enable");
  entityControl.bInheritAlpha = GetBool("Inherit Alpha");
  entityControl.bSmoothingEnabled = GetBool("Smoothing Enable");
  entityControl.eExtendedEntityType = ConvertCigiStringToExtendedEntityType(GetChoice("Entity Type"));

  entityControl.alpha = GetInt("Alpha");
  entityControl.entityID = EntityID(GetInt("Entity ID"));
  entityControl.entityType.entityKindID = SisoEntityKindID(GetInt("Entity Kind"));
  entityControl.entityType.entityDomainID = SisoEntityDomainID(GetInt("Entity Domain"));
  uint16_t entityID = GetInt("Entity ID");
  entityControl.entityID = sbio::EntityID(entityID);

  entityControl.entityType.entityCountryID = SisoEntityCountryID(GetInt("Entity Country"));
  entityControl.entityType.entityCategoryID = SisoEntityCategoryID(GetInt("Entity Category"));
  entityControl.entityType.entitySubCategoryID = SisoEntitySubCategoryID(GetInt("Entity Subcategory"));
  entityControl.entityType.entitySpecificID = SisoEntitySpecificID(GetInt("Entity Specific"));
  entityControl.entityType.entityExtraID = SisoEntityExtraID(GetInt("Entity Extra"));

  CHostSession* pSession = GetHostSession();
  if (entityControl.eExtendedEntityType == EExtendedEntityType::SHORT)
  {
    entityControl.shortEntityTypeID = ShortEntityTypeID(GetInt("Short Entity Type"));
  }

  pSession->SendEntityControl(entityControl);
}

void CEntityControlPanel::OnEntityTypeSelection(wxCommandEvent& event)
{
  string sSelection = GetChoice("Entity Type");

  if (sSelection == "Short")
  {
    m_ShortEntityTypeSizer->Show(true);
    m_ExtendedEntityTypeSizer->Show(false);
  }
  else if (sSelection == "Extended")
  {
    m_ShortEntityTypeSizer->Show(false);
    m_ExtendedEntityTypeSizer->Show(true);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
