//Copyright SimBlocks LLC 2016-2026
#include "PacketsPage.h"
#include "AccelerationControlPanel.h"
#include "AnimationControlPanel.h"
#include "ArticulatedPartControlPanel.h"
#include "AtmosphereControlPanel.h"
#include "CelestialSphereControlPanel.h"
#include "CollisionDetectionSegmentDefinitionPanel.h"
#include "CollisionDetectionVolumeDefinitionPanel.h"
#include "ComponentControlPanel.h"
#include "ConformalClampedEntityPositionPanel.h"
#include "EarthReferenceModelDefinitionPanel.h"
#include "EntityControlPanel.h"
#include "EntityPositionPanel.h"
#include "EnvironmentalConditionsRequestPanel.h"
#include "EnvironmentalRegionControlPanel.h"
#include "HATHOTRequestPanel.h"
#include "HostEmulatorDefinitions.h"
#include "IGControlPacketPanel.h"
#include "LineOfSightSegmentRequestPanel.h"
#include "LineOfSightVectorRequestPanel.h"
#include "MaritimeSurfaceControlPanel.h"
#include "MotionTrackerControlPanel.h"
#include "PositionRequestPanel.h"
#include "SensorControlPanel.h"
#include "ShortArticulatedPartControlPanel.h"
#include "ShortComponentControlPanel.h"
#include "ShortSymbolControlPanel.h"
#include "SymbolCircleDefinitionPanel.h"
#include "SymbolClonePanel.h"
#include "SymbolControlPanel.h"
#include "SymbolPolygonDefinitionPanel.h"
#include "SymbolSurfaceDefinitionPanel.h"
#include "SymbolTextDefinitionPanel.h"
#include "SymbolTexturedCircleDefinitionPanel.h"
#include "SymbolTexturedPolygonDefinition.h"
#include "TerrestrialSurfaceConditionsPanel.h"
#include "VelocityControlPanel.h"
#include "ViewControlPanel.h"
#include "ViewDefinitionPanel.h"
#include "WaveControlPanel.h"
#include "WeatherControlPanel.h"

// clang-format off
wxBEGIN_EVENT_TABLE(CPacketsPage, wxPanel)
  EVT_LISTBOX(ID_PACKETS_PAGE_PACKET_TYPE_LIST_BOX, CPacketsPage::OnListBox)
wxEND_EVENT_TABLE();
// clang-format on

CPacketsPage::CPacketsPage(wxWindow* pParent) : CNotebookPage(pParent)
{
  SetBackgroundColour(wxColor(*wxWHITE));

  wxFlexGridSizer* hboxTop = new wxFlexGridSizer(wxHORIZONTAL);

  wxPanel* pPanelCenter = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500, 700));

  m_Panels["IG Control"] = new CIGControlPacketPanel(pPanelCenter);
  m_Panels["Entity Position"] = new CEntityPositionPanel(pPanelCenter);
  m_Panels["Conformal Clamped Entity Position"] = new CConformalClampedEntityPositionPanel(pPanelCenter);
  m_Panels["Component Control"] = new CComponentControlPanel(pPanelCenter);
  m_Panels["Short Component Control"] = new CShortComponentControlPanel(pPanelCenter);
  m_Panels["Articulated Part Control"] = new CArticulatedPartControlPanel(pPanelCenter);
  m_Panels["Short Articulated Part Control"] = new CShortArticulatedPartControlPanel(pPanelCenter);
  m_Panels["Velocity Control"] = new CVelocityControlPanel(pPanelCenter);
  m_Panels["Celestial Sphere Control"] = new CCelestialSphereControlPanel(pPanelCenter);
  m_Panels["Atmosphere Control"] = new CAtmosphereControlPanel(pPanelCenter);
  m_Panels["Environmental Region Control"] = new CEnvironmentalRegionControlPanel(pPanelCenter);
  m_Panels["Weather Control"] = new CWeatherControlPanel(pPanelCenter);
  m_Panels["Maritime Surface Conditions Control"] = new CMaritimeSurfaceControlPanel(pPanelCenter);
  m_Panels["Wave Control"] = new CWaveControlPanel(pPanelCenter);
  m_Panels["Terrestrial Surface Conditions Control"] = new CTerrestrialSurfaceConditionsPanel(pPanelCenter);
  m_Panels["View Control"] = new CViewControlPacketPanel(pPanelCenter);
  m_Panels["Sensor Control"] = new CSensorControlPanel(pPanelCenter);
  m_Panels["Motion Tracker Control"] = new CMotionTrackerControlPanel(pPanelCenter);
  m_Panels["Earth Reference Model Definition"] = new CEarthReferenceModelDefinitionPanel(pPanelCenter);
  m_Panels["Acceleration Control"] = new CAccelerationControlPanel(pPanelCenter);
  m_Panels["View Definition"] = new CViewDefinitionPanel(pPanelCenter);
  m_Panels["Collision Detection Segment Definition"] = new CCollisionDetectionSegmentDefinitionPanel(pPanelCenter);
  m_Panels["Collision Detection Volume Definition"] = new CCollisionDetectionVolumeDefinitionPanel(pPanelCenter);
  m_Panels["HAT/HOT Request"] = new CHATHOTRequestPanel(pPanelCenter);
  m_Panels["Line of Sight Segment Request"] = new CLineOfSightSegmentRequestPanel(pPanelCenter);
  m_Panels["Line of Sight Vector Request"] = new CLineOfSightVectorRequestPanel(pPanelCenter);
  m_Panels["Position Request"] = new CPositionRequestPanel(pPanelCenter);
  m_Panels["Environmental Conditions Request"] = new CEnvironmentalConditionsRequestPanel(pPanelCenter);
  m_Panels["Symbol Surface Definition"] = new CSymbolSurfaceDefinitionPanel(pPanelCenter);
  m_Panels["Symbol Text Definition"] = new CSymbolTextDefinitionPanel(pPanelCenter);
  m_Panels["Symbol Circle Definition"] = new CSymbolCircleDefinitionPanel(pPanelCenter);
  m_Panels["Symbol Polygon Definition"] = new CSymbolPolygonDefinitionPanel(pPanelCenter);
  m_Panels["Symbol Clone"] = new CSymbolClonePanel(pPanelCenter);
  m_Panels["Symbol Control"] = new CSymbolControlPanel(pPanelCenter);
  m_Panels["Short Symbol Control"] = new CShortSymbolControlPanel(pPanelCenter);
  m_Panels["Symbol Textured Circle Definition"] = new CSymbolTexturedCircleDefinitionPanel(pPanelCenter);
  m_Panels["Symbol Textured Polygon Definition"] = new CSymbolTexturedPolygonDefinitionPanel(pPanelCenter);
  m_Panels["Entity Control"] = new CEntityControlPanel(pPanelCenter);
  m_Panels["Animation Control"] = new CAnimationControlPanel(pPanelCenter);

  wxArrayString choices;
  for (auto it : m_Panels)
  {
    choices.push_back(it.first);

    if (it.second)
    {
      it.second->Hide();
    }
  }

  choices.Sort();

  wxPanel* pPanelTop = new wxPanel(this, wxID_ANY, wxDefaultPosition);
  pPanelTop->SetMinSize(wxSize(300, 600));

  new wxListBox(pPanelTop, ID_PACKETS_PAGE_PACKET_TYPE_LIST_BOX, wxPoint(20, 20), wxSize(250, 600), choices);
  SelectPacket("IG Control");

  hboxTop->Add(pPanelTop, 1, wxEXPAND | wxALL, 5);
  hboxTop->Add(pPanelCenter, 1, wxEXPAND | wxALL, 5);

  SetSizerAndFit(hboxTop);
}

void CPacketsPage::OnListBox(wxCommandEvent& event)
{
  wxString sPacket = event.GetString();

  SelectPacket(sPacket.ToStdString());
}

void CPacketsPage::SelectPacket(const std::string& sPacket)
{
  auto it = m_Panels.find(m_sCurrentPacketSelection);
  if (it != m_Panels.end())
  {
    wxPanel* pPanel = it->second;

    if (pPanel)
    {
      pPanel->Hide();
    }
  }

  m_sCurrentPacketSelection = sPacket;
  wxPanel* pPanel = m_Panels[m_sCurrentPacketSelection];
  if (pPanel)
  {
    pPanel->Show();
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
