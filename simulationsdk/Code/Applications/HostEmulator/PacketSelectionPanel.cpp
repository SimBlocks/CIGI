//Copyright SimBlocks LLC 2016-2026
#include "PacketSelectionPanel.h"

PacketSelectionPanel::PacketSelectionPanel(const wxString& title, wxPanel* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE, title)
{
  SetBackgroundColour(wxColor(*wxWHITE));

  wxStaticText* text = new wxStaticText(this, wxID_ANY, wxT("Packet Selection:\n This is where the packet selection panel is"));
  text->Wrap(100);
  wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
  vbox->Add(text);

  wxButton* b1 = new wxButton(this, wxID_ANY, wxT("Button"));
  vbox->Add(b1);

  this->SetSizerAndFit(vbox);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
