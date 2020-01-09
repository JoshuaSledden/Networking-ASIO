#include "frame.h"

wxBEGIN_EVENT_TABLE(frame, wxFrame)
	//EVT_BUTTON(10001, OnButtonClicked)
wxEND_EVENT_TABLE()

frame::frame()
	: wxFrame(nullptr, wxID_ANY, "Server", wxPoint(30, 30), wxSize(425, 300))
{
	wxBoxSizer* sizerV = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* sizerH = new wxBoxSizer(wxHORIZONTAL);

	sizerH->Add(log_list_ = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(300, 350)), 1, wxALL | wxEXPAND, 8);
	sizerV->Add(command_button_ = new wxButton(this, 10001, "Exit"), 0, wxALL | wxEXPAND | wxALIGN_TOP, 8);
	sizerH->Add(sizerV);

	SetSizer(sizerH);
}

frame::~frame()
{
}

void frame::log(wxString message)
{
	log_list_->Append(message);
}
