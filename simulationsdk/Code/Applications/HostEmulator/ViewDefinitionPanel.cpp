//Copyright SimBlocks LLC 2016-2026
#include "ViewDefinitionPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "EntityLib/EntityLib.h"
#include "HostCigiLib/Host.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "MathLib/MathTypes.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "ViewLib/ViewLib.h"
#include <boost/algorithm/string.hpp>

using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

extern CHostEmulatorGuiApp* g_pHostEmulatorGuiApp;

// clang-format off
wxBEGIN_EVENT_TABLE(CViewDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_VIEW_DEFINITION_PACKET, CViewDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CViewDefinitionPanel::CViewDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "View Definition Packet");

  CreateIntControl(vbox, "View ID");
  CreateIntControl(vbox, "Group ID");
  CreateBoolControl(vbox, "Near Enable");
  CreateBoolControl(vbox, "Far Enable");
  CreateBoolControl(vbox, "Left Enable");
  CreateBoolControl(vbox, "Right Enable");
  CreateBoolControl(vbox, "Top Enable");
  CreateBoolControl(vbox, "Bottom Enable");

  wxArrayString mirrorOptions;
  mirrorOptions.Add("None");
  mirrorOptions.Add("Horizontal");
  mirrorOptions.Add("Vertical");
  mirrorOptions.Add("Horizontal and Vertical");
  CreateChoiceControl(vbox, "Mirror Mode", mirrorOptions);

  wxArrayString pixelReplicationOptions;
  if (g_pHostEmulatorGuiApp != nullptr)
  {
    CHost* pHost = g_pHostEmulatorGuiApp->GetHost();
    if (pHost != nullptr && pHost->GetScriptRuntime() != nullptr)
    {
      auto pixelReplicationModes = pHost->GetScriptRuntime()->GetPixelReplicationModes();
      for (auto itPixelReplicationMode : pixelReplicationModes)
      {
        pixelReplicationOptions.Add(itPixelReplicationMode);
      }
    }
  }

  CreateChoiceControl(vbox, "Pixel Replication Mode", pixelReplicationOptions);

  wxArrayString projectionTypeOptions;
  projectionTypeOptions.Add("Perspective");
  projectionTypeOptions.Add("Orthographic");
  CreateChoiceControl(vbox, "Projection Type", projectionTypeOptions);

  wxArrayString reorderOptions;
  reorderOptions.Add("No Reorder");
  reorderOptions.Add("Bring to Top");
  CreateChoiceControl(vbox, "Reorder", reorderOptions);

  CreateRangeControl(vbox, "View Type", 0, 7);

  CreateFloatControl(vbox, "Near");
  CreateFloatControl(vbox, "Far");
  CreateFloatControl(vbox, "Left");
  CreateFloatControl(vbox, "Right");
  CreateFloatControl(vbox, "Top");
  CreateFloatControl(vbox, "Bottom");

  wxButton* pSendButton = new wxButton(this, ID_VIEW_DEFINITION_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CViewDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  CHost* pHost = g_pHostEmulatorGuiApp != nullptr ? g_pHostEmulatorGuiApp->GetHost() : nullptr;
  if (pHost == nullptr || pHost->GetScriptRuntime() == nullptr)
  {
    return;
  }

  SCigiViewDefinition viewDefinition;

  viewDefinition.viewID = ViewID(GetInt("View ID"));
  viewDefinition.viewGroupID = ViewGroupID(GetInt("Group ID"));
  viewDefinition.bNearEnabled = GetBool("Near Enable");
  viewDefinition.bFarEnabled = GetBool("Far Enable");
  viewDefinition.bLeftEnabled = GetBool("Left Enable");
  viewDefinition.bRightEnabled = GetBool("Right Enable");
  viewDefinition.bTopEnabled = GetBool("Top Enable");
  viewDefinition.bBottomEnabled = GetBool("Bottom Enable");

  viewDefinition.eMirrorMode = ConvertCigiStringToMirrorMode(GetChoice("Mirror Mode"));
  viewDefinition.pixelReplicationMode = pHost->GetScriptRuntime()->GetPixelReplicationModeID(GetChoice("Pixel Replication Mode"));
  viewDefinition.eProjectionMode = ToProjectionMode(boost::to_upper_copy(GetChoice("Projection Type")));

  viewDefinition.bReorder = GetChoice("Reorder") == "Bring to Top";
  viewDefinition.viewType = ViewType(GetRange("View Type"));
  viewDefinition.fNear = GetFloat("Near");
  viewDefinition.fFar = GetFloat("Far");
  viewDefinition.fLeft = GetFloat("Left");
  viewDefinition.fRight = GetFloat("Right");
  viewDefinition.fTop = GetFloat("Top");
  viewDefinition.fBottom = GetFloat("Bottom");

  CHostSession* pSession = GetHostSession();
  pSession->SendViewDefinition(viewDefinition);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
