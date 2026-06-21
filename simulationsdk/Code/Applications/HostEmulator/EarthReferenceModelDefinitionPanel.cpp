//Copyright SimBlocks LLC 2016-2026
#include "EarthReferenceModelDefinitionPanel.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"

using namespace sbio::math;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CEarthReferenceModelDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_EARTH_REFERENCE_DEFINITION_SEND_PACKET, CEarthReferenceModelDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CEarthReferenceModelDefinitionPanel::CEarthReferenceModelDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Earth Reference Mode Definition Packet");

  CreateBoolControl(vbox, "Custom ERM Enable");
  CreateFloatControl(vbox, "Equatorial Radius");
  CreateFloatControl(vbox, "Flattening");

  wxButton* pSendButton = new wxButton(this, ID_EARTH_REFERENCE_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CEarthReferenceModelDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  sbio::cigi::SCigiEarthReferenceModel earthReferenceModel;
  bool bCustomERM = GetBool("Custom ERM Enable");

  if (bCustomERM)
  {
    earthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::HOST_DEFINED;
  }
  else
  {
    earthReferenceModel.eEarthReferenceModel = EEarthReferenceModel::WGS84;
  }

  earthReferenceModel.fEquatorialRadius = GetFloat("Equatorial Radius");
  earthReferenceModel.fFlattening = GetFloat("Flattening");

  CHostSession* pSession = GetHostSession();
  pSession->SendEarthReferenceModelDefinition(earthReferenceModel);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
