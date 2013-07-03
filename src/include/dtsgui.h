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

#ifndef DTSGUI_H_INCLUDED
#define DTSGUI_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#else
/*application struct*/
typedef struct dtsgui dtsgui;
#endif

/*menu controls*/
typedef void *dtsgui_menu;
typedef void *dtsgui_menuitem;

/*viewable windows*/
typedef void *dtsgui_pane;
typedef void *dtsgui_notebook;
typedef void *dtsgui_treeview;

/*form controls*/
typedef void *dtsgui_textbox;
typedef void *dtsgui_checkbox;
typedef void *dtsgui_listbox;

/*callbacks*/
typedef int (*dtsgui_configcb)(struct dtsgui *, void *userdata);
typedef void (*event_callback)(dtsgui_pane, int, void *);

struct point {
	int x;
	int y;
};

/*buttons YES / No may be YES/NO APPLY/REFRESH OK/CANCEL*/
enum panel_buttons {
	wx_PANEL_BUTTON_FIRST	= 1 << 0,
	wx_PANEL_BUTTON_BACK	= 1 << 1,
	wx_PANEL_BUTTON_FWD		= 1 << 2,
	wx_PANEL_BUTTON_LAST	= 1 << 3,
	wx_PANEL_BUTTON_YES		= 1 << 4,
	wx_PANEL_BUTTON_NO		= 1 << 5
};

enum panel_type {
	wx_DTSPANEL_WINDOW,
	wx_DTSPANEL_PANEL,
	wx_DTSPANEL_SCROLLPANEL,
	wx_DTSPANEL_DIALOG
};

#define wx_PANEL_BUTTON_NAV		wx_PANEL_BUTTON_FIRST | wx_PANEL_BUTTON_BACK | wx_PANEL_BUTTON_FWD | wx_PANEL_BUTTON_LAST
#define wx_PANEL_BUTTON_DIR		wx_PANEL_BUTTON_BACK | wx_PANEL_BUTTON_FWD
#define wx_PANEL_BUTTON_ACTION	wx_PANEL_BUTTON_YES | wx_PANEL_BUTTON_NO
#define wx_PANEL_BUTTON_ALL		wx_PANEL_BUTTON_ACTION | wx_PANEL_BUTTON_NAV

/*application config and control*/
dtsgui *dtsgui_config(dtsgui_configcb confcallback_cb, void *userdata, struct point wsize, struct point wpos, const char *title, const char *status);
int dtsgui_run(int argc, char **argv);
void *dtsgui_userdata(struct dtsgui *dtsgui);

/*menu configuration*/
dtsgui_menu dtsgui_newmenu(struct dtsgui *dtsgui, const char *name);
dtsgui_menuitem dtsgui_newmenuitem(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *hint, dtsgui_pane pane);
void dtsgui_menusep(dtsgui_menu dtsmenu);
void dtsgui_about(dtsgui_menu dtsmenu, struct dtsgui *dtsgui, const char *text);
void dtsgui_close(dtsgui_menu dtsmenu, struct dtsgui *dtsgui);
void dtsgui_exit(dtsgui_menu dtsmenu, struct dtsgui *dtsgui);

/*view config*/
dtsgui_pane dtsgui_newpanel(struct dtsgui *dtsgui, const char *name, int butmask, enum panel_type type);
dtsgui_pane dtsgui_textpane(struct dtsgui *dtsgui, const char *title, const char *buf);
void dtsgui_rundialog(dtsgui_pane pane);

/*form items*/
dtsgui_textbox dtsgui_newtextbox(dtsgui_pane pane, const char *title, const char *value);
dtsgui_textbox dtsgui_newtextbox_multi(dtsgui_pane pane, const char *title, const char *value);
dtsgui_listbox dtsgui_newpasswdbox(dtsgui_pane pane, const char *title, const char *value);
dtsgui_checkbox dtsgui_newcheckbox(dtsgui_pane pane, const char *title, int ischecked);
dtsgui_checkbox dtsgui_newlistbox(dtsgui_pane pane, const char *title);

/*add item to list*/
void dtsgui_listbox_add(dtsgui_listbox, const char *text, void *data);

/* returns auth struct needs to be un-ref'd*/
struct basic_auth *dtsgui_pwdialog(const char *user, const char *passwd,void *data);

/*confirm box*/
int dtsgui_confirm(struct dtsgui *dtsgui, const char *text);

/*set callback for a pane*/
void dtsgui_setevcallback(dtsgui_pane pane,event_callback evcb, void *data);

#ifdef __cplusplus
}
#endif
#endif
