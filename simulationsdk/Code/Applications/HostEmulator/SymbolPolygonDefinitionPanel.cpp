//Copyright SimBlocks LLC 2016-2026
#include "SymbolPolygonDefinitionPanel.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "HostEmulatorDefinitions.h"
#include "MathLib/MathTypes.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorGuiApp.h"
#include "HostEmulatorUtilities.h"
#include "wx/grid.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::symbol;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolPolygonDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_POLYGON_DEFINITION_SEND_PACKET, CSymbolPolygonDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolPolygonDefinitionPanel::CSymbolPolygonDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Polygon Definition Packet");

  CreateIntControl(vbox, "Symbol ID");
  CreateIntControl(vbox, "Stipple Pattern");

  wxArrayString primitiveOptions;
  primitiveOptions.Add("Point");
  primitiveOptions.Add("Line");
  primitiveOptions.Add("Line Strip");
  primitiveOptions.Add("Line Loop");
  primitiveOptions.Add("Triangle");
  primitiveOptions.Add("Triangle Strip");
  primitiveOptions.Add("Triangle Fan");
  CreateChoiceControl(vbox, "Primitive Type", primitiveOptions);

  CreateFloatControl(vbox, "Line Width");
  CreateFloatControl(vbox, "Stipple Pattern Length");

  m_pGrid = new wxGrid(this, -1, wxPoint(0, 0));
  m_pGrid->SetMinSize(wxSize(300, 200));
  m_pGrid->CreateGrid(m_nNumRows, 2);

  m_pGrid->SetColLabelValue(0, "U");
  m_pGrid->SetColLabelValue(1, "V");

  m_pGrid->SetColFormatFloat(0, 0, 4);
  m_pGrid->SetColFormatFloat(1, 0, 4);

  wxStaticBoxSizer* textureCoordinatesSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Vertex UVs");

  wxButton* pAddRowButton = new wxButton(this, ID_SYMBOL_POLYGON_DEFINITION_ADD_ROW, "Add Row");
  pAddRowButton->SetBackgroundColour(*(wxWHITE));
  pAddRowButton->Bind(wxEVT_BUTTON, &CSymbolPolygonDefinitionPanel::OnAddRow, this);
  textureCoordinatesSizer->Add(pAddRowButton, wxSizerFlags().Center());

  wxButton* pRemoveRowButton = new wxButton(this, ID_SYMBOL_POLYGON_DEFINITION_CLEAR_ROWS, "Cear Rows");
  pRemoveRowButton->SetBackgroundColour(*(wxWHITE));
  pRemoveRowButton->Bind(wxEVT_BUTTON, &CSymbolPolygonDefinitionPanel::OnRemoveRow, this);
  textureCoordinatesSizer->Add(pRemoveRowButton, wxSizerFlags().Center());

  textureCoordinatesSizer->Add(m_pGrid);

  vbox->Add(textureCoordinatesSizer);

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_POLYGON_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  pSendButton->Bind(wxEVT_BUTTON, &CSymbolPolygonDefinitionPanel::OnSend, this);
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolPolygonDefinitionPanel::OnAddRow(wxCommandEvent& event)
{
  m_pGrid->InsertRows(m_nNumRows, 1);
  ++m_nNumRows;
}

void CSymbolPolygonDefinitionPanel::OnRemoveRow(wxCommandEvent& event)
{
  m_pGrid->ClearGrid();
  m_nNumRows = 0;
}

void CSymbolPolygonDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SSymbolPolygon symbolPolygon;
  symbolPolygon.symbolID = SymbolID(GetInt("Symbol ID"));
  symbolPolygon.nStipplePattern = GetInt("Stipple Pattern");
  symbolPolygon.ePrimitiveType = ConvertCigiStringToPrimitivePolygon(GetChoice("Primitive Type"));

  symbolPolygon.fLineWidth = GetFloat("Line Width");
  symbolPolygon.fStipplePatternLength = GetFloat("Stipple Pattern Length");

  for (int n = 0; n < m_pGrid->GetNumberRows(); ++n)
  {
    if (m_pGrid->GetCellValue(wxGridCellCoords(n, 0)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 1)).IsEmpty())
    {
      continue;
    }

    Vec2f v;
    v[0] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 0)));
    v[1] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 1)));
    symbolPolygon.vertices.push_back(v);
  }

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolPolygonDefinition(symbolPolygon);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
