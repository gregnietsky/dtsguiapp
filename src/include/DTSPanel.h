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

#ifndef DTSPANEL_H
#define DTSPANEL_H

typedef class DTSPanel DTSPanel;

class DTSPanelEvent: public wxEvtHandler {
	public:
		DTSPanelEvent(void *userdata = NULL, event_callback evcb = NULL, DTSPanel *win = NULL);
		void BindButton(wxPanel *win, int button);
		void OnButton(wxCommandEvent &event);
	private:
		void *data;
		event_callback evcb;
		DTSPanel *parent;
};

class DTSPanel: public virtual wxWindow {
	public:
		DTSPanel(wxFrame* = NULL, wxString = wxEmptyString, int = 0);
		~DTSPanel();
		wxTextCtrl *TextBox(const char *, wxString = wxEmptyString, int flags = wxTE_LEFT, int rows = 1);
		wxTextCtrl *PasswdBox(const char *, wxString = wxEmptyString, int flags = wxTE_LEFT);
		wxCheckBox *CheckBox(const char *title, int ischecked);
		wxChoice *ListBox(const char *title);
		void AddItem(wxWindow *item, const wxGBPosition pos, const wxGBSpan span = wxDefaultSpan, int flags = 0,int growrow = -1);
		void SetEventCallback(event_callback evcb, void *userdata = NULL);
		void EventHandler(int eid, wxCommandEvent *event);
		wxString GetName();
		wxPanel *GetPanel();
		panel_type type;
		int buttons[6];
	protected:
		bool ShowPanel(bool = true);
		void SetupWin();
		DTSPanelEvent *dtsevthandler;
		int button_mask;
		wxString status;
		wxFrame *frame;
		void Buttons(void);
		wxPanel *panel;
	private:
		wxGridBagSizer *fgs;
		bool beenshown;
		int g_row;
};

class DTSStaticPanel: public DTSPanel, public virtual wxPanel  {
	public:
		DTSStaticPanel(wxWindow *,wxFrame* = NULL, wxString = wxEmptyString, int = 0);
		DTSStaticPanel();
		virtual bool Show(bool = true);
};

class DTSScrollPanel: public DTSPanel, public virtual wxScrolledWindow {
	public:
		DTSScrollPanel(wxWindow *, wxFrame* = NULL, wxString = wxEmptyString, int = 0);
		virtual bool Show(bool);
};

class DTSWindow: public DTSPanel, public virtual wxWindow {
	public:
		DTSWindow(wxWindow *, wxFrame* = NULL, wxString = wxEmptyString);
		bool Show(bool = true);
};

class DTSDialog: public DTSStaticPanel {
	public:
		DTSDialog(wxFrame* = NULL, wxString = wxEmptyString, int = wx_PANEL_BUTTON_ACTION);
		virtual bool Show(bool = true);
	private:
		wxBoxSizer *sizer;
		wxDialog *dialog;
};

#endif // DTSPANEL_H
