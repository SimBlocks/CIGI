//Copyright SimBlocks LLC 2016-2026
#include "SymbolTexturedPolygonDefinition.h"
#include "CigiLib/CigiLib.h"
#include "CigiLib/CigiTypes.h"
#include "EntityLib/EntityLib.h"
#include "HostCigiLib/HostSession.h"
#include "HostEmulatorDefinitions.h"
#include "HostEmulatorGuiApp.h"
#include "HostEmulatorUtilities.h"
#include "MathLib/MathTypes.h"
#include "UtilitiesLib/UtilitiesLib.h"

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::entity;
using namespace sbio::symbol;
using namespace sbio::utils;
using namespace sbio::cigi;
using namespace sbio::cigi::host;

// clang-format off
wxBEGIN_EVENT_TABLE(CSymbolTexturedPolygonDefinitionPanel, wxPanel)
  EVT_BUTTON(ID_SYMBOL_TEXTURED_POLYGON_DEFINITION_SEND_PACKET, CSymbolTexturedPolygonDefinitionPanel::OnSend)
wxEND_EVENT_TABLE();
// clang-format on

CSymbolTexturedPolygonDefinitionPanel::CSymbolTexturedPolygonDefinitionPanel(wxWindow* pParentWindow) : CBasePacketPanel(pParentWindow)
{
  wxStaticBoxSizer* vbox = new wxStaticBoxSizer(wxVERTICAL, this, "Symbol Textured Polygon Definition Packet");

  CreateIntControl(vbox, "Symbol ID");
  CreateIntControl(vbox, "Texture ID");

  wxArrayString primitiveTypeOptions;
  primitiveTypeOptions.Add("Triangle");
  primitiveTypeOptions.Add("Triangle Strip");
  primitiveTypeOptions.Add("Triangle Fan");
  CreateChoiceControl(vbox, "Primitive Type", primitiveTypeOptions);

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
  m_pGrid->CreateGrid(m_nNumRows, 4);

  m_pGrid->SetColLabelValue(0, "Vertex U");
  m_pGrid->SetColLabelValue(1, "Vertex V");
  m_pGrid->SetColLabelValue(2, "S");
  m_pGrid->SetColLabelValue(3, "T");

  for (int n = 0; n < 4; ++n)
  {
    m_pGrid->SetColFormatFloat(n, 0, 4);
  }

  wxStaticBoxSizer* textureCoordinatesSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Texture Coordinates");

  wxButton* pAddRowButton = new wxButton(this, ID_SYMBOL_TEXTURED_CIRCLE_DEFINITION_ADD_ROW, "Add Row");
  pAddRowButton->SetBackgroundColour(*(wxWHITE));
  pAddRowButton->Bind(wxEVT_BUTTON, &CSymbolTexturedPolygonDefinitionPanel::OnAddRow, this);
  textureCoordinatesSizer->Add(pAddRowButton, wxSizerFlags().Center());

  wxButton* pRemoveRowButton = new wxButton(this, ID_SYMBOL_TEXTURED_CIRCLE_CLEAR_ROWS, "Cear Rows");
  pRemoveRowButton->SetBackgroundColour(*(wxWHITE));
  pRemoveRowButton->Bind(wxEVT_BUTTON, &CSymbolTexturedPolygonDefinitionPanel::OnRemoveRow, this);
  textureCoordinatesSizer->Add(pRemoveRowButton, wxSizerFlags().Center());

  textureCoordinatesSizer->Add(m_pGrid);

  vbox->Add(textureCoordinatesSizer);

  wxButton* pSendButton = new wxButton(this, ID_SYMBOL_TEXTURED_POLYGON_DEFINITION_SEND_PACKET, "Send");
  pSendButton->SetBackgroundColour(*(wxWHITE));
  vbox->Add(pSendButton, wxSizerFlags().Center());

  SetSizerAndFit(vbox);
}

void CSymbolTexturedPolygonDefinitionPanel::OnAddRow(wxCommandEvent& event)
{
  m_pGrid->InsertRows(m_nNumRows, 1);
  ++m_nNumRows;
}

void CSymbolTexturedPolygonDefinitionPanel::OnRemoveRow(wxCommandEvent& event)
{
  m_pGrid->ClearGrid();
  m_nNumRows = 0;
}

void CSymbolTexturedPolygonDefinitionPanel::OnSend(wxCommandEvent& WXUNUSED(event))
{
  SSymbolTexturedPolygon symTexturedPolygon;
  symTexturedPolygon.symbolID = SymbolID(GetInt("Symbol ID"));
  symTexturedPolygon.textureID = TextureID(GetInt("Texture ID"));

  symTexturedPolygon.ePrimitiveType = ConvertCigiStringToPrimitivePolygon(GetChoice("Primitive Type"));
  symTexturedPolygon.eTextureFilterMode = ConvertCigiStringToTextureFilterMode(GetChoice("Texture Filter Mode"));
  symTexturedPolygon.eTextureWrapMode = ConvertCigiStringToTextureWrapMode(GetChoice("Texture Repeat or Clamp"));

  for (int n = 0; n < m_pGrid->GetNumberRows(); ++n)
  {
    if (m_pGrid->GetCellValue(wxGridCellCoords(n, 0)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 1)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 2)).IsEmpty() || m_pGrid->GetCellValue(wxGridCellCoords(n, 3)).IsEmpty())
    {
      continue;
    }

    SSymbolTexturedPolygonVertex vertex;

    vertex.uv[0] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 0)));
    vertex.uv[1] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 1)));
    vertex.textureCoordinateST[0] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 2)));
    vertex.textureCoordinateST[1] = ToFloat(m_pGrid->GetCellValue(wxGridCellCoords(n, 3)));

    symTexturedPolygon.vertices.push_back(vertex);
  }

  CHostSession* pSession = GetHostSession();
  pSession->SendSymbolTexturedPolygonDefinition(symTexturedPolygon);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
