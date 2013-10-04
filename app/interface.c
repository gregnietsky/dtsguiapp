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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <dtsapp.h>

#include "dtsgui.h"
#include "private.h"

struct new_iface_data {
	dtsgui_tabview tv;
	struct xml_doc *xmldoc;
};

struct iface_cdata {
	const char *name;
};

void free_iface_cdata(void *data) {
	struct iface_cdata *cdata = data;
	if (cdata->name) {
		free((void*)cdata->name);
	}
}

struct iface_cdata *get_iface_cdata(const char *val) {
	struct iface_cdata *cd;

	if (!(cd = objalloc(sizeof(*cd), free_iface_cdata))) {
		return NULL;
	}

	ALLOC_CONST(cd->name, val);
	return cd;
}

void free_newiface_data(void *data) {
	struct new_iface_data *nd = data;

	if (nd->xmldoc) {
		objunref(nd->xmldoc);
	}

}

struct new_iface_data *get_newiface_data(dtsgui_tabview tv, struct xml_doc *xmldoc) {
	struct new_iface_data *nd;

	if (!(nd = objalloc(sizeof(*nd), free_newiface_data))) {
		return NULL;
	}

	if (xmldoc && objref(xmldoc)) {
		nd->xmldoc = xmldoc;
	}
	nd->tv = tv;
	return nd;
}


extern int handle_newxmltabpane(dtsgui_pane p, int type, int event, void *data) {
	struct tab_newpane *tn = (struct tab_newpane*)data;
	struct xml_node *xn;
	const char *name;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		default:
			return 1;
	}

	if (!tn || !(xn = dtsgui_panetoxml(p, tn->xpath, tn->node, tn->vitem, tn->tattr))) {
		return 1;
	}

	if (tn->tattr) {
		name = xml_getattr(xn, tn->tattr);
	} else {
		name = xn->value;
	}

	if (tn->cdata) {
		objunref(tn->cdata);
		tn->cdata = NULL;
	}

	if (xn->value) {
		tn->cdata = get_iface_cdata(xn->value);
		if ((p = dtsgui_tabpage_insert(tn->tabv, name, wx_PANEL_BUTTON_ACTION, tn->data, tn->xmldoc, tn->cb, tn->cdata, tn->last, -1))) {
			tn->last++;
		}
	} else {
		xml_delete(xn);
	}


	objunref(xn);
	return 0;
}

extern int handle_updatetabpane(dtsgui_pane p, int type, int event, void *data) {
	struct iface_cdata *cdata = data;
	const char *name;

	switch(event) {
		case wx_PANEL_BUTTON_YES:
			break;
		case wx_PANEL_BUTTON_NO:
			return 1;
		default:
			return 1;
	}

	if (!cdata || !(name = dtsgui_findvalue(p, "iface"))) {
		return 0;
	}

	if (cdata->name) {
		free((void*)cdata->name);
	}
	ALLOC_CONST(cdata->name, name);

	if ((name = dtsgui_findvalue(p, "name"))) {
		dtsgui_setstatus(p, name);
		free((void*)name);
	}

	return 1;
}

void network_iface_pane_cb(dtsgui_pane p, void *data) {
	struct iface_cdata *cdata = data;
	const char *iface = cdata->name;
	const char *xpre = "/config/IP/Interfaces";

	dtsgui_xmltextbox(p, "Name", "name", xpre, "Interface", NULL, iface, "name");
	network_iface_pane(p, xpre, iface);
	dtsgui_setevcallback(p, handle_updatetabpane, cdata);
}

void network_iface_new_pane_cb(dtsgui_pane p, void *data) {
	struct new_iface_data *nd = data;

	network_iface_new_pane(p, data);
	dtsgui_newxmltabpane(nd->tv, p, "/config/IP/Interfaces", "Interface", "iface", "name", handle_newxmltabpane, network_iface_pane_cb, NULL, nd->xmldoc, nd);
}


dtsgui_pane iface_config(struct dtsgui *dtsgui, const char *title, void *data) {
	dtsgui_tabview tabv;
	struct new_iface_data *nd;
	struct xml_doc *xmldoc;
	struct xml_search *xp;
	struct xml_node *xn;
	const char *name;
	struct iface_cdata *cdata;
	void *iter = NULL;

	if (!(xmldoc = app_getxmldoc(dtsgui))) {
		return NULL;
	}

	if (!(xp = xml_xpath(xmldoc, "/config/IP/Interfaces/Interface", "name"))) {
		objunref(xmldoc);
		return NULL;
	}

	tabv = dtsgui_tabwindow(dtsgui, title, NULL);

	for(xn = xml_getfirstnode(xp, &iter); xn; xn = xml_getnextnode(iter)) {
		name = xml_getattr(xn, "name");
		cdata = get_iface_cdata(xn->value);
		dtsgui_newtabpage(tabv, name, wx_PANEL_BUTTON_ACTION, NULL, xmldoc, network_iface_pane_cb, cdata);
		objunref(cdata);
		objunref(xn);
	}

	nd = get_newiface_data(tabv, xmldoc);
	dtsgui_newtabpage(tabv, "Add", wx_PANEL_BUTTON_ACTION, NULL, xmldoc, network_iface_new_pane_cb, nd);
	objunref(nd);

	if (iter) {
		objunref(iter);
	}
	objunref(xp);
	objunref(xmldoc);
	return tabv;
}
