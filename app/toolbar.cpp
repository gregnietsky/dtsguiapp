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

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

enum server_state {
	DTSAPPTB_SERVER_POP = 1 << 0,
	DTSAPPTB_SERVER_SET = 1 << 1
};


class DTSAPPToolBar: public wxToolBar {
	public:
		DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data);
	private:
		void OnServer(wxCommandEvent& event);
		wxComboBox *server;
		wxComboBox *proto;
		struct dtsgui *dtsgui;
		int sflags;
};


DTSAPPToolBar::DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data) {
	if (dtsgui && objref(dtsgui)) {
		this->dtsgui = (struct dtsgui*)dtsgui;
	}
	sflags = 0;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, style, name);
	proto = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	server = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(300,-1), 0, NULL, wxTE_PROCESS_ENTER | wxCB_DROPDOWN);
	wxStaticText *text2 = new wxStaticText(this, wxID_ANY, "://");
	wxStaticText *text = new wxStaticText(this, wxID_ANY, "Server ");

	AddControl(text);
	proto->Append("http");
	proto->Append("https");
	proto->Append("https [:666]");
	proto->SetSelection(2);

	AddControl(proto);
	AddControl(text2);
	AddControl(server);
	server->Append(wxEmptyString);
	AddStretchableSpace();

	server->Bind(wxEVT_TEXT_ENTER, &DTSAPPToolBar::OnServer, this);
	server->Bind(wxEVT_COMBOBOX, &DTSAPPToolBar::OnServer, this);
	server->Bind(wxEVT_COMBOBOX_DROPDOWN, &DTSAPPToolBar::OnServer, this);
	server->Bind(wxEVT_COMMAND_TEXT_UPDATED, &DTSAPPToolBar::OnServer, this);
}

void DTSAPPToolBar::OnServer(wxCommandEvent& event) {
	int etype = event.GetEventType();
	wxString val;

	val = server->GetValue();

	if ((sflags & DTSAPPTB_SERVER_POP) && !(sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMMAND_TEXT_UPDATED)) {
		sflags &= ~DTSAPPTB_SERVER_POP;
	} else if ((sflags & DTSAPPTB_SERVER_SET) && (etype == wxEVT_COMMAND_TEXT_UPDATED)) {
		sflags &= ~DTSAPPTB_SERVER_SET;
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && (etype == wxEVT_COMBOBOX_DROPDOWN) && (server->GetValue().Len() < 5)) {
		dtsgui_alert(dtsgui, "Please enter 5 or more characters to search !");
	} else if (!(sflags & DTSAPPTB_SERVER_POP) && (etype == wxEVT_COMBOBOX_DROPDOWN)) {
		struct curl_post *post = curl_newpost();
		struct curlbuf *cbuf;

		sflags |= (DTSAPPTB_SERVER_POP | DTSAPPTB_SERVER_SET);

		val = server->GetValue();
		curl_postitem(post, "function", "getcust");
		curl_postitem(post, "search", val);
		cbuf = test_posturl(dtsgui, NULL, NULL, "https://sip1.speakezi.co.za:666/auth/test.php" , post);
		server->Clear();
/*		if (cbuf && cbuf->body) {
			dtsgui_alert(dtsgui, (char*)cbuf->body);
		}*/
		if (cbuf) {
			objunref(cbuf);
		}
		server->Append(val);
		server->Popup();
	} else if (etype == wxEVT_TEXT_ENTER) {
		dtsgui_alert(dtsgui, "Got Me Some URL ARRRRGh");
	} else if (etype == wxEVT_COMBOBOX) {
		/*Setting the value fires events reenable popup when done*/
		val.Append(" SELECTED INFO HERE");
		sflags |= DTSAPPTB_SERVER_SET;
		server->SetValue(val);
		sflags |= DTSAPPTB_SERVER_POP;
	}
}

void *app_toolbar(struct dtsgui *dtsgui, void *pw, long style, int id, const char *name, void *data) {
	DTSAPPToolBar *tb;

	tb = new DTSAPPToolBar(dtsgui, (wxWindow*)pw, style, id, name, data);

	return tb;
}
