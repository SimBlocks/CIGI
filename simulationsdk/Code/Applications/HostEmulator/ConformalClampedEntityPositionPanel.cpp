//Copyright SimBlocks LLC 2016-2026
#include "ConformalClampedEntityPositionPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "ErrorLogPage.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CConformalClampedEntityPositionPanel, wxPanel)
  EVT_BUTTON(ID_CONFORMAL_CLAMPED_ENTITY_SEND_PACKET, CConformalClampedEntityPositionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CConformalClampedEntityPositionPanel::CConformalClampedEntityPositionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Conformal Clamped Entity Position");

  CreateIntControl(vbox, "Entity ID");
  CreateFloatControl(vbox, "Yaw");
  CreateFloatControl(vbox, "Latitude");
  CreateFloatControl(vbox, "Longitude");

  wxButton* pSendButton = new wxButton(this, ID_CONFORMAL_CLAMPED_ENTITY_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CConformalClampedEntityPositionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCigiConformalClampedEntityPosition conformalClampedEntityPos;
  conformalClampedEntityPos.entityID = EntityID(GetInt("Entity ID"));

  HostCigiErrorEventArgs args;

  double latitude = GetDouble("Latitude");
  if (!Latitude::CheckValid(latitude))
  {
    args.sError = "Latitude: Enter number between -90.0 and 90.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Latitude: Enter number between -90.0 and 90.0");
    latitude = ResetDouble("Latitude");
  }
  conformalClampedEntityPos.latitude = Latitude(latitude);

  double longitude = GetDouble("Longitude");
  if (!Longitude::CheckValid(longitude))
  {
    args.sError = "Longtitude: Enter number between -180.0 and 180.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Longtitude: Enter number between -180.0 and 180.0");
    longitude = ResetDouble("Longitude");
  }
  conformalClampedEntityPos.longitude = Longitude(longitude);

  float degree = GetFloat("Yaw");
  if (!Degrees::CheckValid(degree))
  {
    args.sError = "Yaw: Enter number between 0.0 and 360.0";
    Event::Raise<HostCigiEvent>(args);

    wxLogError("Yaw: Enter number between 0.0 and 360.0");
    degree = ResetFloat("Yaw");
  }
  conformalClampedEntityPos.fYaw = Degrees(degree);

  delete wxLog::SetActiveTarget(NULL);

  CHostSession* pSession = GetHostSession();
  pSession->SendConformalClampedEntityPosition(conformalClampedEntityPos);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
