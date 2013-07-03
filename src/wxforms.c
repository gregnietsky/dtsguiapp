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
#include <stdio.h>
#include <string.h>

#include <dtsapp.h>

#include "dtsgui.h"

struct app_data {
	struct dtsgui *dtsgui;
	struct xml_doc *xmldoc;
};

void xml_config(struct xml_doc *xmldoc) {
	void *xmlbuf;

	xmlbuf = xml_doctobuffer(xmldoc);
	printf("%s\n", xml_getbuffer(xmlbuf));
	objunref(xmlbuf);
}

struct xml_doc *loadxmlconf(struct dtsgui *dtsgui) {
	struct basic_auth *auth;
	struct curlbuf *cbuf;
	struct xml_doc *xmldoc = NULL;

	auth = curl_newauth("admin", "");
	if (!(cbuf = curl_geturl("https://ns2.distrotech.co.za:666/ns/config/netsentry.xml", auth, dtsgui_pwdialog, dtsgui))) {
		objunref(auth);
		return NULL;
	}

	curl_ungzip(cbuf);

	if (cbuf && cbuf->c_type && !strcmp("application/xml", cbuf->c_type)) {
		xmldoc = xml_loadbuf(cbuf->body, cbuf->bsize);
	}

	objunref(cbuf);
	objunref(auth);
	return xmldoc;
}

void handle_test(dtsgui_pane p, int event, void *data) {
	switch(event) {
		case wx_PANEL_BUTTON_YES:
			printf("Ok\n");
			break;
		case wx_PANEL_BUTTON_NO:
			printf("Cancel\n");
			break;
		default
				:
			printf("Other %i\n", event);
			break;
	}
}

void testpanel(struct dtsgui *dtsgui, dtsgui_menu menu) {
	dtsgui_pane p;
	dtsgui_listbox l;

	p = dtsgui_newpanel(dtsgui, "Test Panel", wx_PANEL_BUTTON_ACTION, 1);

	dtsgui_newtextbox(p, "TextBox", "Text");
	dtsgui_newtextbox_multi(p, "Multi Line", "L\nL\n");
	dtsgui_newcheckbox(p, "Checkbox", 1);

	l = dtsgui_newlistbox(p, "ListBox");
	dtsgui_listbox_add(l, "Test1", NULL);
	dtsgui_listbox_add(l, "Test2", NULL);
	dtsgui_listbox_add(l, "Test3", NULL);

	dtsgui_newmenuitem(menu, dtsgui, "&Test", p);
	dtsgui_setevcallback(p, handle_test, NULL);
}

void file_menu(struct dtsgui *dtsgui) {
	dtsgui_menu file;

	file = dtsgui_newmenu(dtsgui, "&File");
	testpanel(dtsgui, file);

	dtsgui_menusep(file);
	dtsgui_close(file, dtsgui);
	dtsgui_exit(file, dtsgui);
}

void help_menu(struct dtsgui *dtsgui) {
	dtsgui_menu help;
	dtsgui_pane p;

	help = dtsgui_newmenu(dtsgui, "&Help");

	p = dtsgui_textpane(dtsgui, "TEST", "Hello");
	dtsgui_newmenuitem(help, dtsgui, "&Hello...\tCtrl-H", p);
	dtsgui_menusep(help);
	dtsgui_about(help, dtsgui, "This is a test application!!!!");
}

int guiconfig_cb(struct dtsgui *dtsgui, void *data) {
	struct app_data *appdata = data;

	if (!data || !objref(appdata)) {
		return 0;
	}

	/* menus*/
	file_menu(dtsgui);
	help_menu(dtsgui);

	/*load xml config via http*/
	if (!(appdata->xmldoc = loadxmlconf(dtsgui))) {
		objunref(appdata);
		dtsgui_confirm(dtsgui, "Config DL Failed (as expected after 3 tries)\nHello Dave\n\nWould You Like To Play .... Thermo Nuclear War ?");
		return 1;
	}
	return 1;
}

void free_appdata(void *data) {
	struct app_data *appdata = data;
	if (appdata->dtsgui) {
		objunref(appdata->dtsgui);
	}
	if (appdata->xmldoc) {
		objunref(appdata->xmldoc);
	}
}

int main(int argc, char **argv) {
	struct point wsize = {800, 600};
	struct point wpos = {50, 50};
	struct app_data *appdata;
	int res;

	if (!(appdata = objalloc(sizeof(*appdata), free_appdata))) {
		return -1;
	}

	appdata->dtsgui = dtsgui_config(guiconfig_cb, appdata, wsize, wpos, "Wx Windows Test", "Welcome to wxWidgets!");
	res = dtsgui_run(argc, argv);
	objunref(appdata);

	return res;
}
