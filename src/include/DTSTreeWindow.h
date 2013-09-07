#ifndef DTSTREEWINDOW_H
#define DTSTREEWINDOW_H

typedef class DTSTreeWindow DTSTreeWindow;

class DTSTreeWindowEvent: public wxEvtHandler {
	public:
		DTSTreeWindowEvent(void *userdata = NULL, event_callback evcb = NULL, DTSTreeWindow *win = NULL);
		void TreeEvent(wxCommandEvent &event);
	private:
		void *data;
		event_callback evcb;
		DTSTreeWindow *parent;
};

class DTSTreeWindow: public DTSObject, public virtual wxSplitterWindow {
	public:
		DTSTreeWindow(wxWindow *parent = NULL, wxFrame *frame = NULL, wxString stat_msg = wxEmptyString, int pos = 30);
		~DTSTreeWindow();
		void ShowRMenu();
		bool Show(bool = true);
	protected:
		void SetWindow(wxWindow *window);
	private:
		wxPanel *t_pane;
		wxWindow *c_pane;
		wxDataViewTreeCtrl *tree;
		wxBoxSizer *sizer;
		wxWindow *a_window;
		DTSTreeWindowEvent *dtsevthandler;
		wxMenu *rmenu;
};

class DTSTabWindow: public DTSObject, public virtual wxNotebook {
	public:
		DTSTabWindow(wxFrame *frame = NULL, wxString stat_msg = wxEmptyString);
		bool Show(bool = true);
};

#endif // DTSTREEWINDOW_H
