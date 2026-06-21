//Copyright SimBlocks LLC 2016-2026
#include "SymbolTexturedCircleDefinitionPanel.h"
#include "CigiLib/CigiLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "HostEmulatorUtilities.h"
#include "MathLib/MathTypes.h"

using namespace sbio;
using namespace sbio::math;
using namespace sbio::symbol;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolTexturedCircleDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_TEXTURED_CIRCLE_DEFINITION_SEND_PACKET, CSymbolTexturedCircleDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolTexturedCircleDefinitionPanel::CSymbolTexturedCircleDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Textured Circle Definition Packet");

  CreateIntControl(vbox, "Symbol ID");
  CreateIntControl(vbox, "Texture ID");

  wxArrayString textureFilterModeOptions;
  textureFilterModeOptions.Add("Nearest");
  textureFilterModeOptions.Add("Linear");
  CreateChoiceControl(vbox, "Texture Filter Mode", textureFilterModeOptions);

  wxArrayString textureRepeatClampOptions;
  textureRepeatClampOptions.Add("Repeat");
  textureRepeatClampOptions.Add("Clamp");
  CreateChoiceControl(vbox, "Texture Repeat or Clamp", textureRepeatClampOptions);

  m_pGrid = new wxGrid(this, -1, wxPoint(0, 0));
  m_pGrid->SetMinSize(wxSize(400, 200));
  m_pGrid->CreateGrid(m_nNumRows, 10);

  m_pGrid->SetColLabelValue(0, "Center U");
  m_pGrid->SetColLabelValue(1, "Center V");
  m_pGrid->SetColLabelValue(2, "Radius");
  m_pGrid->SetColLabelValue(3, "Inner Radius");
  m_pGrid->SetColLabelValue(4, "Start Angle");
  m_pGrid->SetColLabelValue(5, "End Angle");
  m_pGrid->SetColLabelValue(6, "S");
  m_pGrid->SetColLabelValue(7, "T");
  m_pGrid->SetColLabelValue(8, "TM Radius");
  m_pGrid->SetColLabelValue(9, "TM Rotation");

  for (int n = 0; n < 10; ++n)
  {
    m_pGrid->SetColFormatFloat(n, 0, 4);
  }

  wxStaticBoxSizer* circlesSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Circles");

  wxButton* pAddRowButton = new wxButton(this, ID_SYMBOL_TEXTURED_CIRCLE_DEFINITION_ADD_ROW, "Add Row");
  pAddRowButton->SetBackgroundColour(*(wxWHITE));
  pAddRowButton->Bind(wxEVT_BUTTON, &CSymbolTexturedCircleDefinitionPanel::OnAddRow, this);
  circlesSizer->Add(pAddRowButton, wxSizerFlags().Center());

  wxButton* pRemoveRowButton = new wxButton(this, ID_SYMBOL_TEXTURED_CIRCLE_CLEAR_ROWS, "Cear Rows");
  pRemoveRowButton->SetBackgroundColour(*(wxWHITE));
  pRemoveRowButton->Bind(wxEVT_BUTTON, &CSymbolTexturedCircleDefinitionPanel::OnRemoveRow, this);
  circlesSizer->Add(pRemoveRowButton, wxSizerFlags().Center());

  circlesSizer->Add(m_pGrid);

  vbox->Add(circlesSizer);

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_TEXTURED_CIRCLE_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CSymbolTexturedCircleDefinitionPanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolTexturedCircleDefinitionPanel::OnAddRow(wxCommandEvent& event)
{
  m_pGrid->InsertRows(m_nNumRows, 1);
  ++m_nNumRows;
}

void CSymbolTexturedCircleDefinitionPanel::OnRemoveRow(wxCommandEvent& event)
{
  m_pGrid->ClearGrid();
  m_nNumRows = 0;
}

void CSymbolTexturedCircleDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SSymbolTexturedCircle symbolTexturedCircle;

  symbolTexturedCircle.symbolID = SymbolID(GetInt("Symbol ID"));
  symbolTexturedCircle.textureID = TextureID(GetInt("Texture ID"));
  symbolTexturedCircle.eTextureFilter = ConvertCigiStringToTextureFilterMode(GetChoice("Texture Filter Mode"));
  symbolTexturedCircle.eTextureWrap = ConvertCigiStringToTextureWrapMode(GetChoice("Texture Repeat or Clamp"));

  for (int n = 0; n < m_pGrid->GetNumberRows(); ++n)
  {
    if (m_pGrid->GetCellValue(wxGridCellCoords(n, 0)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 1)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 2)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 3)).IsEmpty() ||
        m_pGrid->GetCellValue(wxGridCellCoords(n, 4)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 5)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 6)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 7)).IsEmpty() ||
        m_pGrid->GetCellValue(wxGridCellCoords(n, 8)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 9)).IsEmpty())
    {
      continue;
    }

    STexturedCircleProperties circle;
    circle.centerUV[0] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 0)));
    circle.centerUV[1] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 1)));
    circle.fRadius = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 2)));
    circle.fInnerRadius = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 3)));
    circle.startAngle = Degrees(ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 4))));
    circle.endAngle = Degrees(ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 5))));
    circle.centerTextureST[0] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 6)));
    circle.centerTextureST[1] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 7)));
    circle.fTextureMapRadius = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 8)));
    circle.fTextureMapRotation = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 9)));

    symbolTexturedCircle.circles.push_back(circle);
  }

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolTexturedCircleDefinition(symbolTexturedCircle);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
