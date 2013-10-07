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

void dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point wsize, struct point wpos, const char *title, const char *status) {
	new DTSApp(confcallback_cb, userdata, wsize, wpos, title, status);
}

int dtsgui_run(int argc, char **argv) {
	return wxEntry(argc, argv);
}

void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	DTSFrame *frame = dtsgui->GetFrame();
	frame->CloseMenu((wxMenu *)dtsmenu, wxID_EXIT);
}

void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui) {
	DTSFrame *frame = dtsgui->GetFrame();
	frame->CloseMenu((wxMenu *)dtsmenu, wxID_SAVE);
}

dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name) {
	DTSFrame *frame = dtsgui->GetFrame();
	return frame->NewMenu(name);
}

void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text) {
	DTSFrame *frame = dtsgui->GetFrame();
	frame->SetAbout((wxMenu *)dtsmenu, text);
}

void dtsgui_menusep(dtsgui_menu dtsmenu) {
	wxMenu *m = (wxMenu *)dtsmenu;
	m->AppendSeparator();
}

int dtsgui_confirm(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->Confirm(text);
}

void dtsgui_alert(struct dtsgui *dtsgui, const char *text) {
	DTSFrame *f = dtsgui->GetFrame();
	f->Alert(text);
}

int dtsgui_progress_start(struct dtsgui *dtsgui, const char *text, int maxval, int quit) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->StartProgress(text, maxval, quit);
}

int dtsgui_progress_update(struct dtsgui *dtsgui, int newval, const char* newtext) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->UpdateProgress(newval, newtext);
}

int dtsgui_progress_increment(struct dtsgui *dtsgui, int ival, const char* newtext) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->IncProgress(ival, newtext);
}

void dtsgui_progress_end(struct dtsgui *dtsgui) {
	DTSFrame *f = dtsgui->GetFrame();
	f->EndProgress();
}

dtsgui_pane dtsgui_panel(struct dtsgui *dtsgui, const char *name, const char *title, int butmask, enum panel_type type, void *userdata) {
	DTSFrame *frame = dtsgui->GetFrame();
	return frame->CreatePane(name, title, butmask, type, userdata);
}

extern void dtsgui_panel_setxml(dtsgui_pane pane, struct xml_doc *xmldoc) {
	DTSPanel *p = (DTSPanel *)pane;
	p->SetXMLDoc(xmldoc);
}

struct xml_doc *dtsgui_panelxml(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->GetXMLDoc();
}

void dtsgui_delpane(dtsgui_pane pane) {
	delete (DTSPanel *)pane;
}

extern dtsgui_treeview dtsgui_treewindow(struct dtsgui *dtsgui, const char *title, dtsgui_tree_cb tree_cb, void *userdata, struct xml_doc *xmldoc) {
	DTSFrame *frame = dtsgui->GetFrame();
	DTSTreeWindow *tw  = new DTSTreeWindow(frame, frame, tree_cb, title, 25);
	tw->SetXMLDoc(xmldoc);
	return tw;
}

extern dtsgui_tabview dtsgui_tabwindow(struct dtsgui *dtsgui, const char *title, void *data) {
	DTSFrame *frame = dtsgui->GetFrame();
	DTSTabWindow *tw = new DTSTabWindow(frame, title, data);
	return tw;
}

extern void dtsgui_textbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->TextBox(title, name, value, wxTE_LEFT | wxTE_PROCESS_ENTER, 1, data, DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_textbox_multi(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->TextBox(title, name, value, wxTE_MULTILINE, 5, data,  DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_passwdbox(dtsgui_pane pane, const char *title, const char *name, const char *value, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->TextBox(title, name, value, wxTE_PASSWORD | wxTE_PROCESS_ENTER, 1, data,  DTSGUI_FORM_DATA_PTR);
}

extern void dtsgui_checkbox(dtsgui_pane pane, const char *title, const char *name, const char *checkval, const char *uncheck, int ischecked, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->CheckBox(title, name, ischecked, checkval, uncheck, data,  DTSGUI_FORM_DATA_PTR);
}

extern struct form_item *dtsgui_listbox(dtsgui_pane pane, const char *title, const char *name, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->ListBox(title, name, NULL, data,  DTSGUI_FORM_DATA_PTR);
}

extern struct form_item *dtsgui_combobox(dtsgui_pane pane, const char *title, const char *name, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->ComboBox(title, name, NULL, data, DTSGUI_FORM_DATA_PTR);
}

void dtsgui_listbox_add(struct form_item *listbox, const char *text, const char *value) {
	listbox->Append(text, value);
}

void dtsgui_listbox_set(struct form_item *listbox, int idx) {
	listbox->SetSelection(idx);
}

void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	return p->SetEventCallback(evcb, data);
}

void dtsgui_configcallback(dtsgui_pane pane,dtsgui_configcb cb, void *data) {
	DTSPanel *p = (DTSPanel *)pane;
	p->SetConfigCallback(cb, data);
}

void *dtsgui_userdata(struct dtsgui *dtsgui) {
	return dtsgui->GetUserData();
}

struct bucket_list *dtsgui_panel_items(dtsgui_pane pane) {
	DTSDialog *p = (DTSDialog *)pane;
	return p->GetItems();
}

extern void *dtsgui_item_data(struct form_item *fi) {
	return (fi) ? fi->GetData() : NULL;
}

extern const char *dtsgui_item_name(struct form_item *fi) {
	return fi->GetName();
}

extern 	const char *dtsgui_item_value(struct form_item *fi) {
	return (fi) ? fi->GetValue() : NULL;
}

extern struct dtsgui_wizard* dtsgui_newwizard(struct dtsgui *dtsgui, const char *title) {
	return new dtsgui_wizard(dtsgui, dtsgui->GetFrame(), title);
}

extern dtsgui_pane dtsgui_wizard_addpage(struct dtsgui_wizard *dtswiz, const char *title, void *userdata, struct xml_doc *xmldoc) {
	return dtswiz->AddPage(title, xmldoc, userdata);
}

extern int dtsgui_runwizard(struct dtsgui_wizard *dtswiz) {
	return dtswiz->RunWizard();
}

extern const char *dtsgui_filesave(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->FileDialog(title, path, name, filter, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
}

extern const char *dtsgui_fileopen(struct dtsgui *dtsgui, const char *title, const char *path, const char *name, const char *filter) {
	DTSFrame *f = dtsgui->GetFrame();
	return f->FileDialog(title, path, name, filter, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
}

extern void dtsgui_xmlpanel_update(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel*)pane;
	p->Update_XML();
}

extern void *dtsgui_paneldata(dtsgui_pane pane) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->GetUserData();
}

void dtsgui_titleappend(struct dtsgui *dtsgui, const char *text) {
	dtsgui->AppendTitle(text);
}

void dtsgui_menuitemenable(dtsgui_menuitem dmi, int enable) {
	wxMenuItem *mi = (wxMenuItem*)dmi;
	mi->Enable((enable) ? true : false);
}

struct xml_node *dtsgui_panetoxml(dtsgui_pane p, const char *xpath, const char *node, const char *nodeval, const char *attrkey) {
	DTSPanel *dp = (DTSPanel*)p;
	return dp->Panel2XML(xpath, node, nodeval, attrkey);
}

void dtsgui_setwindow(struct dtsgui *dtsgui, dtsgui_pane p) {
	DTSFrame *f = dtsgui->GetFrame();
	f->SetWindow(NULL);
}

void dtsgui_sendevent(struct dtsgui *dtsgui, int eid) {
	DTSFrame *f =  dtsgui->GetFrame();
	f->SendDTSEvent(eid, NULL);
}

void dtsgui_settitle(dtsgui_pane pane, const char *title) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->SetTitle(title);
}

void dtsgui_setstatus(dtsgui_pane pane, const char *status) {
	DTSPanel *p = (DTSPanel*)pane;
	return p->SetStatus(status);
}

int dtsgui_treenodeid(dtsgui_treenode tn) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->GetNodeID();
}

void dtsgui_treenodesetxml(dtsgui_treenode tn,struct xml_node *xn, const char *tattr) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->SetXMLData(xn, tattr);
}

struct xml_node *dtsgui_treenodegetxml(dtsgui_treenode tn, char **buf) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->GetXMLData(buf);
}

void *dtsgui_treenodegetdata(dtsgui_treenode tn) {
	DTSDVMListStore *ls = (DTSDVMListStore*)tn;
	return ls->GetUserData();
}

void dtsgui_setuptoolbar(struct dtsgui *dtsgui, dtsgui_toolbar_create cb, void *data) {
	DTSFrame *f = dtsgui->GetFrame();
	f->SetupToolbar(cb, data);
}

struct curl_post *dtsgui_pane2post(dtsgui_pane p) {
	DTSPanel *dp = (DTSPanel*)p;
	return dp->Panel2Post();
}

dtsgui_treenode dtsgui_treecont(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid, dtsgui_treeviewpanel_cb p_cb, void *data) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	DTSDVMCtrl *tc = tw->GetTreeCtrl();
	wxDataViewItem root = wxDataViewItem(node);

	return tc->AppendContainer(root, title, can_edit, can_sort, can_del, nodeid, p_cb, data).GetID();
}

dtsgui_treenode dtsgui_treeitem(dtsgui_treeview tree, dtsgui_treenode node, const char *title, int can_edit, int can_sort, int can_del, int nodeid, dtsgui_treeviewpanel_cb p_cb, void *data) {
	DTSTreeWindow *tw = (DTSTreeWindow*)tree;
	DTSDVMCtrl *tc = tw->GetTreeCtrl();
	wxDataViewItem root = wxDataViewItem(node);

	return tc->AppendItem(root, title, can_edit, can_sort, can_del, nodeid, p_cb, data).GetID();
}

void dtsgui_newxmltabpane(dtsgui_tabview tabv, dtsgui_pane p, const char *xpath, const char *node, const char *vitem, const char *tattr,  dtsgui_tabpane_newdata_cb data_cb, dtsgui_tabpanel_cb cb, void *cdata, struct xml_doc *xmldoc, void *data) {
	DTSPanel *dp = (DTSPanel*)p;
	class tab_newpane *tn = new tab_newpane((DTSTabWindow*)tabv, xpath, node, vitem, tattr, data_cb, cb, cdata, xmldoc, data);

	dp->SetEventCallback(&tab_newpane::handle_newtabpane_cb, tn);
	objunref(tn);
}

void dtsgui_newxmltreenode(dtsgui_treeview tree, dtsgui_pane p, dtsgui_treenode tn, const char *xpath, const char *node, const char *vitem, const char *tattr,
							int nid, int flags, dtsgui_xmltreenode_cb node_cb, void *data, dtsgui_treeviewpanel_cb p_cb) {
	DTSPanel *dp = (DTSPanel*)p;
	class tree_newnode *nn = new tree_newnode(tree, tn, xpath, node, vitem, tattr, nid, flags, node_cb, data, p_cb);

	dp->SetEventCallback(&tree_newnode::handle_newtreenode_cb, nn);
	objunref(nn);
}

void dtsgui_set_toolbar(struct dtsgui *dtsgui, int show) {
	DTSFrame *f = dtsgui->GetFrame();
	wxToolBar *tb = f->GetToolBar();
	tb->Show((show) ? true : false);
	f->Layout();
}
