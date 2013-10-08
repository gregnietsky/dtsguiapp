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

#define __DTS_C_API

#include <wx/toolbar.h>
#include <wx/combobox.h>
#include <wx/stattext.h>

#include "dtsgui.h"

#include "private.h"
#include "DTSXMLComboBox.h"

class DTSAPPToolBar: public wxToolBar {
	public:
		DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data);
		~DTSAPPToolBar();
		static struct curl_post *GetPostInfo(const wxString& val);
	private:
		int itemid;
		void HandleEvent(wxCommandEvent &event);
		DTSXMLComboBox *server;
		wxComboBox *proto;
		struct dtsgui *dtsgui;
};


DTSAPPToolBar::DTSAPPToolBar(struct dtsgui *dtsgui, wxWindow *parent, long style, wxWindowID id, wxString name, void *data) {
	int servid;

	if (dtsgui && objref(dtsgui)) {
		this->dtsgui = (struct dtsgui*)dtsgui;
	} else {
		this->dtsgui = NULL;
	}

	itemid = wxID_AUTO_LOWEST;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, style, name);
	proto = new wxComboBox(this, itemid++, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	servid = itemid++;
	server = new DTSXMLComboBox(this, servid, "https://sip1.speakezi.co.za:666/auth/test.php", "/servers/Server", &DTSAPPToolBar::GetPostInfo, 3);
	server->SetSize(wxSize(300,-1));
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
	AddStretchableSpace();

	Bind(wxEVT_TEXT_ENTER, &DTSAPPToolBar::HandleEvent, this, servid, servid, NULL);
	Bind(wxEVT_COMBOBOX, &DTSAPPToolBar::HandleEvent, this, servid, servid, NULL);
	Bind(wxEVT_COMBOBOX_DROPDOWN, &DTSAPPToolBar::HandleEvent, this, servid, servid, NULL);
}

DTSAPPToolBar::~DTSAPPToolBar() {
	if (dtsgui) {
		objunref(dtsgui);
	}
}

struct curl_post *DTSAPPToolBar::GetPostInfo(const wxString& val) {
	struct curl_post *post;

	post = curl_newpost();
	curl_postitem(post, "function", "getcust");
	curl_postitem(post, "search", val);

	return post;
}

void DTSAPPToolBar::HandleEvent(wxCommandEvent& event) {
	int etype = event.GetEventType(), idx;
	struct xml_node *xn;

	idx = server->GetSelection();

	if (etype == wxEVT_TEXT_ENTER) {
		DTS_C_API::dtsgui_alert(dtsgui, "Got Me Some URL ARRRRGh");
	} else if (etype == wxEVT_COMBOBOX) {
		xn = (struct xml_node*)server->GetClientData(idx);
		DTS_C_API::dtsgui_alert(dtsgui, wxString("Selected IP ").Append(xml_getattr(xn, "ipaddr")));
	} else if ((etype == wxEVT_COMBOBOX_DROPDOWN) && !server->HasXMLList()) {
		DTS_C_API::dtsgui_alert(dtsgui, "Please enter 3 or more characters to search !");
	}
}

void *app_toolbar(struct dtsgui *dtsgui, void *pw, long style, int id, const char *name, void *data) {
	DTSAPPToolBar *tb;

	tb = new DTSAPPToolBar(dtsgui, (wxWindow*)pw, style, id, name, data);

	return tb;
}
