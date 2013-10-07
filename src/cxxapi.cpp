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

#ifdef __WIN32__
#define UNICODE 1
#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#endif

#include <stdint.h>
#include <vector>

#include <wx/app.h>
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/wizard.h>

#include <wx/dataview.h>
#include <wx/splitter.h>
#include <wx/notebook.h>

#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/toolbar.h>

#include <dtsapp.h>
#include "dtsgui.h"
#include "dtsgui.hpp"

#include "pitems.h"
#include "evdata.h"
#include "DTSApp.h"
#include "DTSFrame.h"
#include "DTSPanel.h"
#include "DTSListView.h"
#include "DTSTreeWindow.h"
#include "DTSWizard.h"

static int menuid = wxID_AUTO_LOWEST;

dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane p) {
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = dtsgui->GetFrame();
	DTSObject *dp = (DTSObject *)p;
	wxMenuItem *mi;

	wxWindow *w = (dp) ? dp->GetPanel() : NULL;

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(w);

	mi = m->Append(menuid, hint, (dp) ? dp->GetName() : "");
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::SwitchWindow, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
	return mi;
}

extern dtsgui_menuitem dtsgui_newmenucb(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, const char *title, int blank, dtsgui_dynpanel cb,void *data) {
	struct dynamic_panel *p_dyn;
	wxMenu *m = (wxMenu *)dtsmenu;
	DTSFrame *frame = dtsgui->GetFrame();
	wxMenuItem *mi;

	if (!(p_dyn = new dynamic_panel(title, blank, cb, data))) {
		return NULL;
	}

	/*handed over to wx no need to delete*/
	evdata *ev_data = new evdata(p_dyn, 1);
	objunref(p_dyn);

	mi = m->Append(menuid, hint, (title) ? title : "");
	frame->Bind(wxEVT_COMMAND_MENU_SELECTED, &DTSFrame::DynamicPanelEvent, frame, menuid, menuid, (wxObject *)ev_data);
	menuid++;
	return mi;
}

extern dtsgui_pane dtsgui_newtabpage(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata) {
	DTSTabPage *dp = NULL;
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	wxBookCtrlBase *nb = static_cast<wxBookCtrlBase*>(tw);
	DTSFrame *f = tw->GetFrame();

	dp = new DTSTabPage(nb, f, name, true, butmask, cb, cdata, xmldoc);

	if (userdata) {
		dp->SetUserData(userdata);
	}

	return dp;
}

extern dtsgui_pane dtsgui_tabpage_insert(dtsgui_tabview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc, dtsgui_tabpanel_cb cb, void *cdata, int pos, int undo) {
	DTSTabPage *dp = NULL;
	DTSTabWindow *tw = (DTSTabWindow*)tv;
	wxBookCtrlBase *nb = static_cast<wxBookCtrlBase*>(tw);
	DTSFrame *f = tw->GetFrame();

	if (!(dp = new DTSTabPage(nb, f, name, false, butmask, cb, cdata, xmldoc))) {
		return NULL;
	}
	dp->InsertPage(pos);

	if (undo) {
		tw->Undo(undo);
		tw->SetSelection(pos);
	}

	return dp;
}

extern dtsgui_pane dtsgui_treepane(dtsgui_treeview tv, const char *name, int butmask, void *userdata, struct xml_doc *xmldoc) {
	DTSScrollPanel *dp;
	DTSTreeWindow *tw = (DTSTreeWindow*)tv;
	wxWindow *parent;

	parent = tw->GetPanel();
	dp = new DTSScrollPanel(parent, tw->GetFrame(), name, butmask);
	dp->type = wx_DTSPANEL_TREE;

	if (name) {
		dp->SetTitle(name, true);
	}

	if (xmldoc) {
		dp->SetXMLDoc(xmldoc);
	}

	return dp;
}

dtsgui_pane dtsgui_treepane_defalt(dtsgui_treeview self, dtsgui_treenode node) {
	dtsgui_pane p = NULL;
	DTSDVMListStore *ls = (DTSDVMListStore*)node;
	struct xml_doc *xmldoc;
	int nodeid;
	const char *title = strdup(ls->GetTitle().ToUTF8());

	nodeid = ls->GetNodeID();

	if (nodeid == -1) {
		p = dtsgui_treepane(self, NULL, 0, NULL, NULL);
	} else {
		xmldoc = dtsgui_panelxml(self);
		p = dtsgui_treepane(self, title, wx_PANEL_BUTTON_ACTION, NULL, xmldoc);
		if (xmldoc) {
			objunref(xmldoc);
		};
	}
	if (title) {
		free((void*)title);
	}
	return p;
}

extern void dtsgui_xmltextbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, node, fattr, fval,attr))) {
		value = xml->GetValue();
	}

	p->TextBox(title, name, value, wxTE_LEFT | wxTE_PROCESS_ENTER, 1, xml,  DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmltextbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

 	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = xml->GetValue();
	}

	p->TextBox(title, name, value, wxTE_MULTILINE, 5, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmlpasswdbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = xml->GetValue();
	}

	p->TextBox(title, name, value, wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

extern void dtsgui_xmlcheckbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheckval, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	struct xml_element *xml;
	int ischecked = 0;
	const char *value = NULL;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = xml->GetValue();
	}

	if (value && checkval && !strcmp(value, checkval)) {
		ischecked = 1;
	}

	p->CheckBox(title, name, ischecked, checkval, uncheckval, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}
}

struct form_item *dtsgui_xmllistbox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	const char *value = NULL;
	struct xml_element *xml;
	struct form_item *fi;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = xml->GetValue();
	}
	fi = p->ListBox(title, name, value, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}

	return fi;
}

struct form_item *dtsgui_xmlcombobox(dtsgui_pane pane, const char *title, const char *name, const char *xpath, const char *node, const char *fattr, const char *fval, const char *attr) {
	DTSPanel *p = (DTSPanel *)pane;
	const char *value = NULL;
	struct xml_element *xml;
	struct form_item *fi;

	if ((xml = p->GetNode(xpath, node, fattr, fval, attr))) {
		value = xml->GetValue();
	}

	fi = p->ComboBox(title, name, value, xml, DTSGUI_FORM_DATA_XML);

	if (value) {
		free((void*)value);
	}

	return fi;
}

void dtsgui_listbox_addxml(struct form_item *lb, struct xml_doc *xmldoc, const char *xpath, const char *nattr, const char *vattr) {
	struct xml_search *xs;
	struct xml_node *xn;
	void *iter;
	const char *name, *value;

	if (!(xs = xml_xpath(xmldoc, xpath, nattr))) {
		return;
	}

	for(xn = xml_getfirstnode(xs, &iter); xn ; xn = xml_getnextnode(iter)) {
		name = (nattr) ? xml_getattr(xn, nattr) : xn->value;
		value = (vattr) ? xml_getattr(xn, vattr) : xn->value;
		dtsgui_listbox_add(lb, name, value);
		objunref(xn);
	}
	objunref(xs);
	if (iter) {
		objunref(iter);
	}
}

void dtsgui_rundialog(dtsgui_pane pane, event_callback evcb, void *data) {
	DTSDialog *p = (DTSDialog *)pane;

	p->SetEventCallback(evcb, data);
	p->RunDialog();
	delete p;
}

dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf) {
	DTSFrame *f = dtsgui->GetFrame();
	DTSStaticPanel *p = new DTSStaticPanel(f, f, title);
	wxPanel *wxp = static_cast<wxPanel*>(p);

	wxTextCtrl *eb = new wxTextCtrl(wxp, -1, buf, wxDefaultPosition, wxDefaultSize,wxTE_MULTILINE | wxHSCROLL | wxTE_READONLY);

	p->AddItem(eb, wxGBPosition(0,0), wxGBSpan(10, 6), wxEXPAND|wxGROW, 0, 0);

	return p;
}

extern struct form_item *dtsgui_finditem(dtsgui_pane p, const char *name) {
	struct form_item *fi;
	struct bucket_list *il;

	il = dtsgui_panel_items(p);
	fi = (struct form_item*)bucket_list_find_key(il, (void*)name);
	objunref(il);

	return fi;
}

extern const char *dtsgui_findvalue(dtsgui_pane p, const char *name) {
	struct form_item *fi;
	const char *val = NULL;

	if ((fi = dtsgui_finditem(p, name))) {
		val = fi->GetValue();
		objunref(fi);
	}
	return val;
}

void dtsgui_menuenable(dtsgui_menu dm, int enable) {
	bool state =  (enable) ? true : false;
	wxMenuItemList items;
	wxMenuItem *mi;
	int cnt,i;

	wxMenu *m = (wxMenu*)dm;

	items = m->GetMenuItems();
	cnt = m->GetMenuItemCount();

	for(i=0; i < cnt;i++) {
		mi = items[i];
		mi->Enable(state);
	}
}

const char *dtsgui_treenodeparent(dtsgui_treenode tn) {
	DTSDVMListStore *entry, *parent;
	const char *val;

	entry = (DTSDVMListStore*)tn;
	if (!entry || !(parent = entry->GetParent())) {
		return NULL;
	}
	val = strdup(parent->GetTitle().ToUTF8());
	return val;
}

void dtsgui_nodesetxml(dtsgui_treeview tree, dtsgui_treenode node, const char *title) {
	DTSDVMListStore *ls = (DTSDVMListStore*)node;
	struct xml_doc *xmldoc;
	struct xml_node *xn;
	char *buff = NULL;

	if (!(xmldoc = dtsgui_panelxml(tree))) {
		return;
	}

	if (!(xn = ls->GetXMLData(&buff))) {
		objunref(xmldoc);
		return;
	}

	if (buff) {
		xml_setattr(xmldoc, xn, buff, title);
		objunref(buff);
	} else {
		xml_modify(xmldoc, xn, title);
	}
	objunref(xn);
	objunref(xmldoc);
}

extern struct xml_doc *dtsgui_buf2xml(struct curlbuf *cbuf) {
	struct xml_doc *xmldoc = NULL;

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		curl_ungzip(cbuf);
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize, 1);
	}
	return xmldoc;
}

void *dtsgui_char2obj(const char *orig) {
	int len = strlen(orig) + 1;
	void *nobj;

	if ((nobj = objalloc(len, NULL))) {
		memcpy(nobj, orig, len);
	}
	return nobj;
}

#ifdef __WIN32
void getwin32folder(int csidl, char *path) {
	SHGetFolderPathA(NULL, csidl, NULL, 0, path);
}
#endif
