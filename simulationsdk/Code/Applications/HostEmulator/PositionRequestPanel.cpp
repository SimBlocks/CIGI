//Copyright SimBlocks LLC 2016-2026
#include "PositionRequestPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostEmulatorDefinitions.h"
#include "MathLib/MathTypes.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorGuiApp.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CPositionRequestPanel, wxPanel)
  EVT_BUTTON(ID_POSITION_REQUEST_SEND_PACKET, CPositionRequestPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CPositionRequestPanel::CPositionRequestPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Position Request Packet");

  CreateIntControl(vbox, "Articulated Part ID");

  wxArrayString updateModeOptions;
  updateModeOptions.Add("One-Shot");
  updateModeOptions.Add("Continuous");
  CreateChoiceControl(vbox, "Update Mode", updateModeOptions);

  wxArrayString objectClassOptions;
  objectClassOptions.Add("Entity");
  objectClassOptions.Add("Articulated Part");
  objectClassOptions.Add("View");
  objectClassOptions.Add("View Group");
  objectClassOptions.Add("Motion Tracker");
  CreateChoiceControl(vbox, "Object Class", objectClassOptions);

  wxArrayString coordinateSystemOptions;
  coordinateSystemOptions.Add("Geodetic");
  coordinateSystemOptions.Add("Parent Entity");
  coordinateSystemOptions.Add("Submodel");
  CreateChoiceControl(vbox, "Coordinate System", coordinateSystemOptions);

  CreateIntControl(vbox, "Object ID");

  wxButton* pSendButton = new wxButton(this, ID_POSITION_REQUEST_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CPositionRequestPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SPositionRequest positionRequest;

  positionRequest.articulatedPartID = ArticulatedPartID(GetInt("Articulated Part ID"));
  positionRequest.bContinuous = ConvertCigiStringToUpdateMode(GetChoice("Update Mode")) == EUpdateMode::CONTINUOUS;
  positionRequest.eObjectClass = ConvertCigiStringToObjectClass(GetChoice("Object Class"));
  positionRequest.eCoordinateSystem = ConvertCigiStringToObjectCoordinateSystem(GetChoice("Coordinate System"));
  positionRequest.nObjectID = GetInt("Object ID");

  CHostSession* pSession = GetHostSession();
  pSession->SendPositionRequest(positionRequest);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
