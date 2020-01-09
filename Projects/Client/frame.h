#pragma once
#include "wx/wx.h"

class frame : public wxFrame
{
public:
	frame();
	~frame();

	void OnButtonClicked();

public:
	wxButton* command_button_; 
	wxTextCtrl* command_input_;
	wxListBox* log_list_;

	void log(wxString message);

	wxDECLARE_EVENT_TABLE();
};

