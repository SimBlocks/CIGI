//Copyright SimBlocks LLC 2016-2026
#include "AnimationControlPanel.h"
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
wxBEGIN_EVENT_TABLE(CAnimationControlPanel, wxPanel)
  EVT_BUTTON(ID_ANIMATION_CONTROL_SEND_PACKET, CAnimationControlPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CAnimationControlPanel::CAnimationControlPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Animation Control Packet");

  wxArrayString animationStateOptions;
  animationStateOptions.Add("Stop");
  animationStateOptions.Add("Play");
  CreateChoiceControl(vbox, "Animation State", animationStateOptions);

  wxArrayString animationFramePositionResetOptions;
  animationFramePositionResetOptions.Add("Continue");
  animationFramePositionResetOptions.Add("Reset");
  CreateChoiceControl(vbox, "Animation Frame Position Reset", animationFramePositionResetOptions);

  wxArrayString animationLoopModeOptions;
  animationLoopModeOptions.Add("One-Shot");
  animationLoopModeOptions.Add("Continuous");
  CreateChoiceControl(vbox, "Animation Loop Mode", animationLoopModeOptions);

  CreateBoolControl(vbox, "Inherit Alpha");
  CreateIntControl(vbox, "Alpha");
  CreateIntControl(vbox, "Entity ID");
  CreateIntControl(vbox, "Animation ID");
  CreateFloatControl(vbox, "Animation Speed");

  wxButton* pSendButton = new wxButton(this, ID_ANIMATION_CONTROL_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CAnimationControlPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SCigiAnimationControl animationControl;
  animationControl.eAnimationState = ConvertCigiStringToAnimationState(GetChoice("Animation State"));
  animationControl.eAnimationFramePositionReset = ConvertCigiStringToFramePositionReset(GetChoice("Animation Frame Position Reset"));
  animationControl.eAnimationLoopMode = ConvertCigiStringToAnimationLoopMode(GetChoice("Animation Loop Mode"));
  animationControl.bInheritAlpha = GetBool("Inherit Alpha");
  animationControl.alpha = ConvertAlphaToPercentage(GetInt("Alpha"));
  animationControl.entityID = EntityID(GetInt("Entity ID"));
  animationControl.animationID = AnimationID(GetInt("Animation ID"));
  animationControl.fAnimationSpeed = GetFloat("Animation Speed");

  CHostSession* pHostSession = GetHostSession();
  pHostSession->SendAnimationControl(animationControl);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
