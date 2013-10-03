/*
	Distrotech Solutions wxWidgets Gui Interface
	Copyright (C) 2013 Gregory Hinton Nietsky <gregory@distrotech.co.za>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <wx/toolbar.h>
#include <wx/combobox.h>
#include <wx/stattext.h>

#include "dtsgui.h"
#include "private.h"

class DTSAPPToolBar: public wxToolBar {
	public:
		DTSAPPToolBar(wxWindow *parent, long style, wxWindowID id, wxString name);
	private:
		wxComboBox *server;
		wxComboBox *proto;
};


DTSAPPToolBar::DTSAPPToolBar(wxWindow *parent, long style, wxWindowID id, wxString name) {
	Create(parent, id, wxDefaultPosition, wxDefaultSize, style, name);
	server = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(300,-1) );
	proto = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	wxStaticText *text = new wxStaticText(this, wxID_ANY, "Server ");
	wxStaticText *text2 = new wxStaticText(this, wxID_ANY, "://");

	AddControl(text);
	proto->Append("http");
	proto->Append("https");
	proto->Append("https [:666]");
	proto->SetSelection(2);

	AddControl(proto);
	AddControl(text2);
	AddControl(server);
	AddStretchableSpace();
}

void *app_toolbar(void *pw, long style, int id, const char *name) {
	DTSAPPToolBar *tb;

	tb = new DTSAPPToolBar((wxWindow*)pw, style, id, name);

	return tb;
}
