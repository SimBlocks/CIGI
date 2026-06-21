//Copyright SimBlocks LLC 2016-2026
#include "ShortArticulatedPartControlPanel.h"
#include "CigiLib/CigiLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"

using namespace std;
using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CShortArticulatedPartControlPanel, wxPanel)
  EVT_BUTTON(ID_SHORT_ARTICULATED_CONTROL_SEND_PACKET, CShortArticulatedPartControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CShortArticulatedPartControlPanel::CShortArticulatedPartControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Short Articulated Part Control");

  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Articulated Part ID 1");
  CreateIntControl(vbox, "Articulated Part ID 2");

  wxArrayString DOFSelectOptions1;
  DOFSelectOptions1.Add("Not Used");
  DOFSelectOptions1.Add("X Offset");
  DOFSelectOptions1.Add("Y Offset");
  DOFSelectOptions1.Add("Z Offset");
  DOFSelectOptions1.Add("Yaw");
  DOFSelectOptions1.Add("Roll");
  DOFSelectOptions1.Add("Pitch");
  CreateChoiceControl(vbox, "DOF Select 1", DOFSelectOptions1);

  wxArrayString DOFSelectOptions2;
  DOFSelectOptions2.Add("Not Used");
  DOFSelectOptions2.Add("X Offset");
  DOFSelectOptions2.Add("Y Offset");
  DOFSelectOptions2.Add("Z Offset");
  DOFSelectOptions2.Add("Yaw");
  DOFSelectOptions2.Add("Roll");
  DOFSelectOptions2.Add("Pitch");
  CreateChoiceControl(vbox, "DOF Select 2", DOFSelectOptions2);

  CreateBoolControl(vbox, "Articulated Part Enable 1");
  CreateBoolControl(vbox, "Articulated Part Enable 2");

  CreateFloatControl(vbox, "DOF 1");
  CreateFloatControl(vbox, "DOF 2");

  wxButton* pSendButton = new wxButton(this, ID_SHORT_ARTICULATED_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CShortArticulatedPartControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  sbio::cigi::SCigiShortArticulatedPart shortArtPart;

  int nEntityID = GetInt("Entity ID");
  shortArtPart.entityID = sbio::EntityID(nEntityID);
  int nArticulatedPartID1 = GetInt("Articulated Part ID 1");
  shortArtPart.articulatedPartID1 = sbio::ArticulatedPartID(nArticulatedPartID1);
  int nArticulatedPartID2 = GetInt("Articulated Part ID 2");
  shortArtPart.articulatedPartID2 = sbio::ArticulatedPartID(nArticulatedPartID2);

  shortArtPart.eDOF1 = ConvertCigiStringToDegreeOfFreedom(GetChoice("DOF Select 1"));
  shortArtPart.eDOF2 = ConvertCigiStringToDegreeOfFreedom(GetChoice("DOF Select 2"));

  shortArtPart.bArticulatedPart1Enabled = GetBool("Articulated Part Enable 1");
  shortArtPart.bArticulatedPart2Enabled = GetBool("Articulated Part Enable 2");

  shortArtPart.fDOF1 = GetFloat("DOF 1");
  shortArtPart.fDOF2 = GetFloat("DOF 2");

  CHostSession* pSession = GetHostSession();
  pSession->SendShortArticulatedPartControl(shortArtPart);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
