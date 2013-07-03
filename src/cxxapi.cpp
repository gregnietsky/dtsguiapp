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

#include <wx/app.h>
#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/scrolwin.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include <dtsapp.h>
#include "dtsgui.hpp"

#include "evdata.h"
#include "DTSApp.h"
#include "DTSFrame.h"
#include "DTSPanel.h"

dtsgui *dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point wsize, struct point wpos, const char *title, const char *status) {
	/*deleted on close*/
	DTSApp *guiapp = new DTSApp();

	return guiapp->CreateFrame(confcallback_cb, userdata, wsize, wpos, title, status);
}

int dtsgui_run(int argc, char **argv) {
	return wxEntry(argc, argv);
}

void dtsgui_quit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, int type) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	switch (type) {
		case wxID_EXIT:
			m->Append(type, "&Quit", "Quit And Exit");
			frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::OnClose, frame, wxID_EXIT);
			break;
		case wxID_SAVE:
			m->Append(type, "&Exit", "Save And Exit");
			frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::OnClose, frame, wxID_SAVE);
			break;
	}
}

void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	dtsgui_quit(dtsmenu, dtsgui, wxID_EXIT);
}

void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	dtsgui_quit(dtsmenu, dtsgui, wxID_SAVE);
}

wxWindow *getpanewindow(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->GetPanel();
}

wxString getpanename(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->GetName();
}

wxMenuBar *DTSFrame::GetMenuBar() {
	return menubar;
}

dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name) {
	/*deleted with menubar*/
	wxMenu *new_menu = NULL;
	wxMenuBar *menubar;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	if ((menubar = frame->GetMenuBar())) {
		new_menu = new wxMenu;
		menubar->Append(new_menu, name);
	}
	return new_menu;
}

void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;


	frame->SetAbout(text);
	m->Append(wxID_ABOUT);
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::OnAbout, frame, wxID_ABOUT);
}

void dtsgui_menusep(dtsgui_menu dtsmenu) {
	wxMenu *m = (wxMenu *)dtsmenu;
	m->AppendSeparator();
}

dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane p) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = (DTSFrame *)dtsgui->appframe;

	wxWindow *w = (p) ? getpanewindow(p) : NULL;

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(w);
	static int menuid;

	m->Append(menuid, hint, (p) ? getpanename(p) : "");
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::SwitchWindow, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
	return m;
}

void newappframe(struct dtsgui *dtsgui) {
	new DTSFrame(dtsgui->title, wxPoint(dtsgui->wpos.x, dtsgui->wpos.y),
				 wxSize(dtsgui->wsize.x, dtsgui->wsize.y), dtsgui);
}

int dtsgui_confirm(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = (DTSFrame*)dtsgui->appframe;

	return f->Confirm(text);
}


dtsgui_pane dtsgui_newpanel(struct dtsgui *dtsgui, const char *name, int butmask, enum panel_type type) {
	wxFrame *frame = (wxFrame *)dtsgui->appframe;
	dtsgui_pane p = NULL;

	switch (type) {
		case wx_DTSPANEL_SCROLLPANEL:
			p = new DTSScrollPanel(frame, frame, name, butmask);
			break;
		case wx_DTSPANEL_PANEL:
			p = new DTSStaticPanel(frame, frame, name, butmask);
			break;
		case wx_DTSPANEL_WINDOW:
			p = new DTSWindow(frame);
//			p = new DTSWindow(frame, frame, name);
			break;
		case wx_DTSPANEL_DIALOG:
			p = new DTSDialog(frame, name, butmask);
			break;
	}
	return p;
}

dtsgui_textbox dtsgui_newtextbox(dtsgui_pane pane, const char *title, const char *value) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->TextBox(title, value);
}

dtsgui_textbox dtsgui_newtextbox_multi(dtsgui_pane pane, const char *title, const char *value) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->TextBox(title, value, wxTE_MULTILINE, 5);
}

dtsgui_textbox dtsgui_newpasswdbox(dtsgui_pane pane, const char *title, const char *value) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->TextBox(title, value, wxTE_PASSWORD);
}

dtsgui_checkbox dtsgui_newcheckbox(dtsgui_pane pane, const char *title, int ischecked) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->CheckBox(title, ischecked);
}

dtsgui_checkbox dtsgui_newlistbox(dtsgui_pane pane, const char *title) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->ListBox(title);
}

void dtsgui_listbox_add(dtsgui_listbox listbox, const char *text, void *data) {
	wxComboBox *lbox = (wxComboBox *)listbox;
	lbox->Append(text, data);
	if (lbox->GetSelection() == wxNOT_FOUND) {
		lbox->SetSelection(0);
	}
}

void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;

	return p->SetEventCallback(evcb, data);
}

void dtsgui_rundialog(dtsgui_pane pane) {
	DTSDialog *p = (DTSDialog *)pane;
	p->Show();
	delete p;
}

dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf) {
	wxFrame *f = (wxFrame *)dtsgui->appframe;
	DTSStaticPanel *p = new DTSStaticPanel(f, f, title);
	wxWindow *w = p->GetPanel();

	wxTextCtrl *eb = new wxTextCtrl(w, -1, buf, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY, wxDefaultValidator, wxTextCtrlNameStr);

	p->AddItem(eb, wxGBPosition(0,0), wxGBSpan(10, 6), wxEXPAND|wxGROW, 0);

	return p;
}

void *dtsgui_userdata(struct dtsgui *dtsgui) {
	return dtsgui->userdata;
}
