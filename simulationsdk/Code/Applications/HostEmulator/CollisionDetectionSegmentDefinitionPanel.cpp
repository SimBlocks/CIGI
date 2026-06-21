//Copyright SimBlocks LLC 2016-2026
#include "CollisionDetectionSegmentDefinitionPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CCollisionDetectionSegmentDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_COLLISION_DETECTION_SEGMENT_DEFINITION_PACKET, CCollisionDetectionSegmentDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CCollisionDetectionSegmentDefinitionPanel::CCollisionDetectionSegmentDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Collision Detection Segment Definition Packet");

  CreateIntControl(vbox, "Segment ID");
  CreateBoolControl(vbox, "Segment Enable");
  CreateIntControl(vbox, "Entity ID");

  CreateFloatControl(vbox, "X1");
  CreateFloatControl(vbox, "Y1");
  CreateFloatControl(vbox, "Z1");
  CreateFloatControl(vbox, "X2");
  CreateFloatControl(vbox, "Y2");
  CreateFloatControl(vbox, "Z2");

  CreateIntControl(vbox, "Material Mask");

  wxButton* pSendButton = new wxButton(this, ID_COLLISION_DETECTION_SEGMENT_DEFINITION_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CCollisionDetectionSegmentDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCollisionDetectionSegmentDefinition collisionDetectionSegmentDefinition;
  collisionDetectionSegmentDefinition.segmentID = SegmentID(GetInt("Segment ID"));
  collisionDetectionSegmentDefinition.bSegmentEnabled = GetBool("Segment Enable");
  collisionDetectionSegmentDefinition.entityID = EntityID(GetInt("Entity ID"));
  collisionDetectionSegmentDefinition.beg[0] = GetFloat("X1");
  collisionDetectionSegmentDefinition.beg[1] = GetFloat("Y1");
  collisionDetectionSegmentDefinition.beg[2] = GetFloat("Z1");
  collisionDetectionSegmentDefinition.end[0] = GetFloat("X2");
  collisionDetectionSegmentDefinition.end[1] = GetFloat("Y2");
  collisionDetectionSegmentDefinition.end[2] = GetFloat("Z2");
  collisionDetectionSegmentDefinition.nMaterialMask = GetInt("Material Mask");

  CHostSession* pSession = GetHostSession();
  pSession->SendCollisionDetectionSegment(collisionDetectionSegmentDefinition);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
