//Copyright SimBlocks LLC 2016-2026
#include "SymbolCircleDefinitionPanel.h"
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
wxBEGIN_EVENT_TABLE(CSymbolCircleDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_CIRCLE_DEFINITION_SEND_PACKET, CSymbolCircleDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolCircleDefinitionPanel::CSymbolCircleDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Circle Definition Packet");

  CreateIntControl(vbox, "Symbol ID");
  CreateIntControl(vbox, "Stipple Pattern");

  wxArrayString drawingStyleOptions;
  drawingStyleOptions.Add("Line");
  drawingStyleOptions.Add("Fill");
  CreateChoiceControl(vbox, "Drawing Style", drawingStyleOptions);

  CreateFloatControl(vbox, "Line Width");
  CreateFloatControl(vbox, "Stipple Pattern Length");

  m_pGrid = new wxGrid(this, -1, wxPoint(0, 0));
  m_pGrid->SetMinSize(wxSize(400, 200));
  m_pGrid->CreateGrid(m_nNumRows, 6);

  m_pGrid->SetColLabelValue(0, "Center U");
  m_pGrid->SetColLabelValue(1, "Center V");
  m_pGrid->SetColLabelValue(2, "Radius");
  m_pGrid->SetColLabelValue(3, "Inner Radius");
  m_pGrid->SetColLabelValue(4, "Start Angle");
  m_pGrid->SetColLabelValue(5, "End Angle");

  for (int n = 0; n < 10; ++n)
  {
    m_pGrid->SetColFormatFloat(n, 0, 4);
  }

  wxStaticBoxSizer* circlesSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Circles");

  wxButton* pAddRowButton = new wxButton(this, ID_SYMBOL_CIRCLE_DEFINITION_ADD_ROW, "Add Row");
  pAddRowButton->SetBackgroundColour(*(wxWHITE));
  pAddRowButton->Bind(wxEVT_BUTTON, &CSymbolCircleDefinitionPanel::OnAddRow, this);
  circlesSizer->Add(pAddRowButton, wxSizerFlags().Center());

  wxButton* pRemoveRowButton = new wxButton(this, ID_SYMBOL_CIRCLE_CLEAR_ROWS, "Cear Rows");
  pRemoveRowButton->SetBackgroundColour(*(wxWHITE));
  pRemoveRowButton->Bind(wxEVT_BUTTON, &CSymbolCircleDefinitionPanel::OnRemoveRow, this);
  circlesSizer->Add(pRemoveRowButton, wxSizerFlags().Center());

  circlesSizer->Add(m_pGrid);

  vbox->Add(circlesSizer);

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_CIRCLE_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CSymbolCircleDefinitionPanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolCircleDefinitionPanel::OnAddRow(wxCommandEvent& event)
{
  m_pGrid->InsertRows(m_nNumRows, 1);
  ++m_nNumRows;
}

void CSymbolCircleDefinitionPanel::OnRemoveRow(wxCommandEvent& event)
{
  m_pGrid->ClearGrid();
  m_nNumRows = 0;
}

bool IsMissingRowCell(wxGrid* pGrid, int row)
{
  for (int c = 0; c < pGrid->GetNumberCols(); ++c)
  {
    if (pGrid->GetCellValue(wxGridCellCoords(row, c)).IsEmpty())
    {
      return true;
    }
  }

  return false;
}

void CSymbolCircleDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SSymbolCircle symbolCircleDefinition;

  symbolCircleDefinition.symbolID = SymbolID(GetInt("Symbol ID"));
  symbolCircleDefinition.stipplePattern = GetInt("Stipple Pattern");
  symbolCircleDefinition.eDrawingStyle = ConvertCigiStringToDrawingStyle(GetChoice("Drawing Style"));
  symbolCircleDefinition.fLineWidth = GetFloat("Line Width");
  symbolCircleDefinition.fStipplePatternLength = GetFloat("Stipple Pattern Length");

  for (int n = 0; n < m_pGrid->GetNumberRows(); ++n)
  {
    if (IsMissingRowCell(m_pGrid, n))
    {
      break;
    }

    SCircleProperties circle;
    circle.centerUV.U = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 0)));
    circle.centerUV.V = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 1)));
    circle.fRadius = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 2)));
    circle.fInnerRadius = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 3)));
    circle.startAngle = Degrees(ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 4))));
    circle.endAngle = Degrees(ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 5))));

    symbolCircleDefinition.circles.push_back(circle);
  }

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolCircleDefinition(symbolCircleDefinition);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
