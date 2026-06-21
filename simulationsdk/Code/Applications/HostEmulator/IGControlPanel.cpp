//Copyright SimBlocks LLC 2016-2026
#include "IGControlPanel.h"

CIGControlPanel::CIGControlPanel(wxPanel* pParent, const wxPoint& point, const wxSize& size) : wxPanel(pParent, wxID_ANY, point, size)
{
  int colx[2];
  colx[0] = 20;
  colx[1] = 200;
  int rowy = 60;
  int rowheight = 25;

  new wxStaticText(this, wxID_ANY, "Major Version:", wxPoint(colx[0], rowy));
  wxTextCtrl* pTextCigiVersion = new wxTextCtrl(this, wxID_ANY, "3", wxPoint(colx[1], rowy));
  rowy += rowheight;

  new wxStaticText(this, wxID_ANY, "Database Number:", wxPoint(colx[0], rowy));
  wxTextCtrl* pTextDatabaseNumber = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(colx[1], rowy));
  rowy += rowheight;

  new wxStaticText(this, wxID_ANY, "Entity Type Substitution Enable:", wxPoint(colx[0], rowy));
  wxTextCtrl* pTextEntityTypeSubstitutionEnable = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(colx[1], rowy));
  rowy += rowheight;

  new wxStaticText(this, wxID_ANY, "IG Mode:", wxPoint(colx[0], rowy));
  wxTextCtrl* pTextIGMode = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(colx[1], rowy));
  rowy += rowheight;

  new wxStaticText(this, wxID_ANY, "Timestamp Valid:", wxPoint(colx[0], rowy));
  wxTextCtrl* pTextTimestampValid = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(colx[1], rowy));
  rowy += rowheight;

  new wxStaticText(this, wxID_ANY, "Smoothing Enable:", wxPoint(colx[0], rowy));
  wxTextCtrl* pTextSmoothingEnable = new wxTextCtrl(this, wxID_ANY, "1", wxPoint(colx[1], rowy));
  rowy += rowheight;
}

//Usage of this software is prohibited except through a Software Licensing Agreement with SimBlocks LLC.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
