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

#include <stdint.h>
#include <stdlib.h>

#include <wx/scrolwin.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/button.h>

#include "dtsgui.hpp"

#include "DTSPanel.h"

static const int def_buttons[6] = {wxID_FIRST, wxID_BACKWARD, wxID_FORWARD, wxID_LAST, wxID_APPLY, wxID_REFRESH};

DTSPanelEvent::DTSPanelEvent(void *userdata, event_callback ev_cb, DTSPanel *win) {
	data = userdata;
	evcb = ev_cb;
	parent = win;
}

void DTSPanelEvent::OnButton(wxCommandEvent &event) {
	int eid, i;

	if (!parent) {
		event.Skip(true);
		return;
	}

	eid=event.GetId();
	for(i = 0; i < 6; i++) {
		if (parent->buttons[i] == eid) {
			eid = 1 << i;
			break;
		}
	}

	if (evcb) {
		evcb((void *)parent, eid, data);
	}

	parent->EventHandler(eid, &event);
	event.Skip();

}

void DTSPanelEvent::BindButton(wxPanel *win, int button) {
	win->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DTSPanelEvent::OnButton, this, button, button, NULL);
}

DTSPanel::DTSPanel(wxFrame *mainwin, wxString statusmsg, int butmask) {
	button_mask = butmask;
	status = statusmsg;
	frame = mainwin;
	SetName(status);
	panel = NULL;
	dtsevthandler = NULL;
	memcpy(buttons, def_buttons, sizeof(def_buttons));
}

DTSPanel::~DTSPanel() {
	if (dtsevthandler) {
		delete dtsevthandler;
	}
}

void DTSPanel::EventHandler(int eid, wxCommandEvent *event) {
}

void DTSPanel::SetupWin(void) {
	wxWindow *w = static_cast<wxWindow *>(this);

	if (panel) {
		fgs = new wxGridBagSizer(9, 25);
		panel->SetSizer(fgs);
	}

	beenshown = false;
	g_row = 0;
	w->SetName(status);
	w->Show(false);
}

void DTSPanel::SetEventCallback(event_callback evcb, void *userdata) {
	dtsevthandler = new DTSPanelEvent(userdata, evcb, this);
}

wxString DTSPanel::GetName() {
	return status;
}

bool DTSPanel::ShowPanel(bool show) {
	if (show) {
		if (frame) {
			frame->SetStatusText(status);
		}
		if (!beenshown && panel) {
			Buttons();
			fgs->AddGrowableCol(3, 1);
			beenshown = true;
		}
	}
	if (dtsevthandler) {
		printf("Do Something\n");
	}
	return show;
}

void DTSPanel::AddItem(wxWindow *item, const wxGBPosition pos, const wxGBSpan span, int flags, int growrow) {
	fgs->Add(item, pos, span, flags);
	if (growrow >= 0) {
		fgs->AddGrowableRow(g_row, growrow);
	}
}

wxTextCtrl *DTSPanel::TextBox(const char *title, wxString defval, int flags, int rows) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxTextCtrl *tbox = new wxTextCtrl(panel, -1, defval, wxPoint(-1, -1), wxSize(-1, -1), flags);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(rows, 3));
	AddItem(tbox, wxGBPosition(g_row, 3), wxGBSpan(rows,3), wxEXPAND|wxGROW, (rows > 1) ? 1 : -1);
	g_row += rows;

	return tbox;
}

wxTextCtrl *DTSPanel::PasswdBox(const char *title, wxString defval, int flags) {
	return TextBox(title, defval, flags | wxTE_PASSWORD);
}

wxCheckBox *DTSPanel::CheckBox(const char *title, int ischecked) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxCheckBox *cbox = new wxCheckBox(panel, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3));
	AddItem(cbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3));
	g_row++;

	return cbox;
}

wxChoice *DTSPanel::ListBox(const char *title) {
	wxStaticText *text = new wxStaticText(panel, -1, title);
	wxChoice *lbox = new wxComboBox(panel, -1);

	AddItem(text, wxGBPosition(g_row, 0), wxGBSpan(1, 3));
	AddItem(lbox, wxGBPosition(g_row, 3), wxGBSpan(1, 3), wxEXPAND|wxGROW);
	g_row++;

	return lbox;
}

void DTSPanel::Buttons(void) {
	int i, b;

	if (!button_mask || !dtsevthandler) {
		return;
	}

	for(i = 0; i < 6; i++) {
		b = buttons[i];
		if (button_mask & (1 << i)) {
			wxButton *button = new wxButton(panel, b);
			dtsevthandler->BindButton(panel, b);
			AddItem(button, wxGBPosition(g_row, i), wxDefaultSpan, wxALIGN_BOTTOM);
		}
	}
	fgs->AddGrowableRow(g_row, 0);
	g_row++;
}

wxPanel *DTSPanel::GetPanel() {
	return panel;
}

DTSScrollPanel::DTSScrollPanel(wxWindow *parent,wxFrame *frame, wxString status, int butmask)
	:wxScrolledWindow(parent, -1),
	 DTSPanel(frame, status, butmask) {
	type = wx_DTSPANEL_SCROLLPANEL;
	SetScrollRate(10, 10);
	panel = dynamic_cast<wxPanel *>(this);
	SetupWin();
}

bool DTSScrollPanel::Show(bool show) {
	ShowPanel(show);
	return wxScrolledWindow::Show(show);
}

DTSStaticPanel::DTSStaticPanel(wxWindow *parent,wxFrame *frame, wxString status, int butmask)
	:wxPanel(parent, -1),
	 DTSPanel(frame, status, butmask) {
	type = wx_DTSPANEL_PANEL;
	panel = dynamic_cast<wxPanel *>(this);
	SetupWin();
}

DTSStaticPanel::DTSStaticPanel() {
	type = wx_DTSPANEL_PANEL;
}

bool DTSStaticPanel::Show(bool show) {
	ShowPanel(show);
	return wxPanel::Show(show);
}

DTSWindow::DTSWindow(wxWindow *parent, wxFrame *frame, wxString status)
	:wxWindow(parent, -1),
	 DTSPanel(frame, status) {
	type = wx_DTSPANEL_WINDOW;
	SetupWin();
}

bool DTSWindow::Show(bool show) {
	ShowPanel(show);
	return wxWindow::Show(show);
}

DTSDialog::DTSDialog(wxFrame *frame, wxString name, int butmask) {
	type = wx_DTSPANEL_DIALOG;

	button_mask = butmask;
	buttons[4] = wxID_OK;
	buttons[5] = wxID_CANCEL;

	dialog = new wxDialog(frame, -1, name);
	sizer = new wxBoxSizer(wxHORIZONTAL);
	dialog->SetSizer(sizer);

	panel = dynamic_cast<wxPanel *>(this);
	panel->Create(dialog, -1);
	panel->SetName(name);

	sizer->Add(panel, 1, wxALL | wxEXPAND, 15);
	frame->Raise();

	SetupWin();
}

bool DTSDialog::Show(bool show) {
	ShowPanel(show);
	if (show) {
		dialog->Fit();
		dialog->Center();
		dialog->Layout();
		dialog->SetClientSize(sizer->GetSize());

		dialog->ShowModal();
	}
	return show;
}
