/*
 * Copyright (c) 2022 Kimihiro Nonaka
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "np2.h"
#include "pccore.h"
#include "scrndraw.h"
#include "timing.h"

#include "toolkit.h"

#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "taskmng.h"

#include "gtk4/xnp2.h"
#include "gtk4/gtk_keyboard.h"
#include "gtk4/dialog_about.h"

static int *xnp2_argcp;
static char ***xnp2_argvp;
static GtkApplication *xnp2_app;
static GtkWidget *main_window;
GtkWidget *drawarea;

/*
 * Menu
 */

static void
reset_activated(GSimpleAction *action G_GNUC_UNUSED,
    GVariant *parameter G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED)
{

	pccore_cfgupdate();
	pccore_reset();
}

static void
exit_activated(GSimpleAction *action G_GNUC_UNUSED,
    GVariant *parameter G_GNUC_UNUSED, gpointer user_data)
{
	GtkWindow *window = user_data;

	gtk_window_close(GTK_WINDOW(window));
}

static void
dialog_about_activated(GSimpleAction *action G_GNUC_UNUSED,
    GVariant *parameter G_GNUC_UNUSED, gpointer user_data)
{
	GtkWindow *window = user_data;
	Xnp2AboutDialog *dialog;

	dialog = xnp2_about_dialog_new(GTK_WINDOW(window));
	gtk_window_present(GTK_WINDOW(dialog));
}

/*
 * FIXME fullscreen doesn't work properly on Ubuntu Desktop 22.04.
 */
static void
change_fullscreen_state(GSimpleAction *action, GVariant *state,
    gpointer user_data)
{
	GtkWindow *window = user_data;

	if (g_variant_get_boolean(state)) {
		gtk_window_fullscreen(window);
	} else {
		gtk_window_unfullscreen(window);
	}
	g_simple_action_set_state(action, state);
}

static void
activate_toggle(GSimpleAction *action G_GNUC_UNUSED,
    GVariant *parameter G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED)
{
	GVariant *state;

	state = g_action_get_state(G_ACTION(action));
	g_action_change_state(G_ACTION(action),
	    g_variant_new_boolean(!g_variant_get_boolean(state)));
	g_variant_unref(state);
}

static void
draw_func(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height,
    gpointer user_data G_GNUC_UNUSED)
{

	scrnmng_draw(cr, width, height);
}

static void
destroy_activated(GtkWindow *window G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{
	GApplication *app = user_data;

	g_application_quit(G_APPLICATION(app));
}

static gboolean
close_activated(GtkWindow *window G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{

	/* TRUE to stop other handlers from being invoked for the signal. */
	return FALSE;
}

static gboolean
key_pressed(GtkEventControllerKey *self G_GNUC_UNUSED, guint keyval,
    guint keycode G_GNUC_UNUSED, guint modifiers G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{

	if (keyval == GDK_KEY_F11) {
#if 0	/* FIXME menu fullscreen */
		if ((np2oscfg.F11KEY == 1) && (scrnmode & SCRNMODE_FULLSCREEN))
			xmenu_toggle_menu();
		else if (np2oscfg.F11KEY == 2)
			xmenu_select_screen(scrnmode ^ SCRNMODE_FULLSCREEN);
#endif
	} else if ((keyval == GDK_KEY_F12) && (np2oscfg.F12KEY == 0)) {
#if 0	/* FIXME menu mouse */
		xmenu_toggle_item(NULL, "mousemode", !np2oscfg.MOUSE_SW);
#endif
	} else {
		gtkkbd_keydown(keyval);
	}
	return TRUE;
}

static void
key_released(GtkEventControllerKey *self G_GNUC_UNUSED, guint keyval,
    guint keycode G_GNUC_UNUSED, guint modifiers G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{

	if ((keyval != GDK_KEY_F12) || (np2oscfg.F12KEY != 0)) {
		gtkkbd_keyup(keyval);
	}
}

static void
mouse_motion(GtkEventControllerMotion *self G_GNUC_UNUSED, gdouble x, gdouble y,
    gpointer user_data G_GNUC_UNUSED)
{

	(void)x;
	(void)y;
}

static void
mouse_enter(GtkEventControllerMotion *self G_GNUC_UNUSED, gdouble x, gdouble y,
    gpointer user_data G_GNUC_UNUSED)
{

	(void)x;
	(void)y;
}

static void
mouse_leave(GtkEventControllerMotion *self G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{

	/* Nothing to do */
}

static void
mouse_pressed(GtkGestureClick *self, gint n_press G_GNUC_UNUSED,
    gdouble x G_GNUC_UNUSED, gdouble y G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{
	guint btn;

	btn = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
	switch (btn) {
	case GDK_BUTTON_PRIMARY:
		mouse_btn(MOUSE_LEFTDOWN);
		break;
	case GDK_BUTTON_MIDDLE:
#if 0	/* FIXME menu mouse */
		xmenu_toggle_item(NULL, "mousemode", !np2oscfg.MOUSE_SW);
#endif
		break;
	case GDK_BUTTON_SECONDARY:
		mouse_btn(MOUSE_RIGHTDOWN);
		break;
	default:
		break;
	}
}

static void
mouse_released(GtkGestureClick *self, gint n_press G_GNUC_UNUSED,
    gdouble x G_GNUC_UNUSED, gdouble y G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{
	guint btn;

	btn = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(self));
	switch (btn) {
	case GDK_BUTTON_PRIMARY:
		mouse_btn(MOUSE_LEFTUP);
		break;
	case GDK_BUTTON_MIDDLE:
		break;
	case GDK_BUTTON_SECONDARY:
		mouse_btn(MOUSE_RIGHTUP);
		break;
	default:
		break;
	}
}

static void
app_startup(GApplication *app, gpointer user_data G_GNUC_UNUSED)
{
	GtkBuilder *builder;
	GMenuModel *menubar;
	GMenu *menu;
	GError *error = NULL;
	char *name;
	int i;

	builder = gtk_builder_new();
	gtk_builder_set_translation_domain(builder, GETTEXT_PACKAGE);
	if (!gtk_builder_add_from_resource(builder,
	    XNP2_RESOUCE_PREFIX "menu.ui", &error)) {
		g_error("Failed to load menu ui resource: %s\n",
		    error->message);
		g_error_free(error);
	}
	menubar = G_MENU_MODEL(gtk_builder_get_object(builder, "menubar"));

#if defined(SUPPORT_STATSAVE)
	if (!np2oscfg.statsave) {
		/* Remove Stat menu. */
		g_menu_remove(G_MENU(menubar), 1);
	}
#else
	/* Remove Stat menu. */
	g_menu_remove(G_MENU(menubar), 1);
#endif

	/* FDD */
	for (i = 3; i >= 0; i--) {
		if (!(np2cfg.fddequip & (1U << i))) {
			name = g_strdup_printf("fdd%u", i + 1);
			menu = G_MENU(gtk_builder_get_object(builder, name));
			g_free(name);
			g_menu_remove_all(menu);

			menu = G_MENU(gtk_builder_get_object(builder, "fdd"));
			g_menu_remove(menu, i);
		}
	}

#if defined(SUPPORT_IDEIO)
	/* Remove SASI* entry. */
	for (i = 1; i >= 0; i--) {
		name = g_strdup_printf("sasi%u", i + 1);
		menu = G_MENU(gtk_builder_get_object(builder, name));
		g_free(name);
		g_menu_remove_all(menu);

		menu = G_MENU(gtk_builder_get_object(builder, "harddisk"));
		g_menu_remove(menu, 3 + i);
	}
#else
	/* Remove ATAPI* entry. */
	for (i = 0; i >= 0; i--) {
		name = g_strdup_printf("atapi%u", i);
		menu = G_MENU(gtk_builder_get_object(builder, name));
		g_free(name);
		g_menu_remove_all(menu);

		menu = G_MENU(gtk_builder_get_object(builder, "harddisk"));
		g_menu_remove(menu, 2 + i);
	}

	/* Remove ATA* entry. */
	for (i = 1; i >= 0; i--) {
		name = g_strdup_printf("ata0%u", i);
		menu = G_MENU(gtk_builder_get_object(builder, name));
		g_free(name);
		g_menu_remove_all(menu);

		menu = G_MENU(gtk_builder_get_object(builder, "harddisk"));
		g_menu_remove(menu, i);
	}
#endif

	/* XXX FIXME IMPLME menu */

	gtk_application_set_menubar(GTK_APPLICATION(app), menubar);
	g_object_unref(builder);
}

static void
app_activate(GApplication *app, gpointer user_data G_GNUC_UNUSED)
{
	static const char *accels[] = { "F11", NULL };
	static const GActionEntry win_entries[] = {
		/* Emulate */
		{ "reset", reset_activated, NULL, NULL, NULL },
		{ "exit", exit_activated, NULL, NULL, NULL },

		/* Stat */
		/* FDD */
		/* HardDisk */
		/* Screen */
		/* Device */
		/* Other */
		{ "about", dialog_about_activated, NULL, NULL, NULL },

		/* Keyboard shortcuts */
		{ "fullscreen", activate_toggle, NULL, "false",
			change_fullscreen_state },
	};
	GtkBuilder *builder;
	GtkWidget *window;
	GtkIconTheme *icon_theme;
	GtkDrawingArea *drawing_area;
	GError *error = NULL;
	GtkEventController *keyevc, *motionevc;
	GtkGesture *clickges;
	GtkStyleContext *context;
	guchar *pixels, *p;
	int rowstride, x, y;

	builder = gtk_builder_new();
	gtk_builder_set_translation_domain(builder, GETTEXT_PACKAGE);
	if (!gtk_builder_add_from_resource(builder,
	    XNP2_RESOUCE_PREFIX "main.ui", &error)) {
		g_error("Failed to load main ui resource: %s\n",
		    error->message);
		g_error_free(error);
	}

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), np2oscfg.titles);
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	g_signal_connect(G_OBJECT(window), "destroy",
	    G_CALLBACK(destroy_activated), app);
	g_signal_connect(G_OBJECT(window), "close-request",
	    G_CALLBACK(close_activated), NULL);
	g_action_map_add_action_entries(G_ACTION_MAP(window), win_entries,
	    G_N_ELEMENTS(win_entries), window);
	gtk_application_set_accels_for_action(GTK_APPLICATION(app),
	    "win.fullscreen", accels);
	main_window = window;

	icon_theme = gtk_icon_theme_get_for_display(
	    gtk_widget_get_display(window));
	if (gtk_icon_theme_has_icon(icon_theme, "xnp2")) {
		gtk_window_set_default_icon_name("xnp2");
		gtk_window_set_icon_name(GTK_WINDOW(window), "xnp2");
	}

	drawing_area = GTK_DRAWING_AREA(gtk_builder_get_object(builder,
	    "drawing_area"));
	gtk_drawing_area_set_draw_func(drawing_area, draw_func, NULL, NULL);
	gtk_widget_set_focusable(GTK_WIDGET(drawing_area), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(drawing_area),
	    scrnmng_last_width, scrnmng_last_height);
	drawarea = GTK_WIDGET(drawing_area);

	GtkCssProvider *provider = gtk_css_provider_new();
	gchar *css = g_strdup_printf(".xypadding{background-color:black;"
	    /*padding:up right bottom left */
	     "padding:%upx %upx %upx %upx;}",
	    np2oscfg.paddingy, np2oscfg.paddingx,
	    np2oscfg.paddingy, np2oscfg.paddingx);
	gtk_css_provider_load_from_data(provider, css, -1);
	g_free(css);
	context = gtk_widget_get_style_context(GTK_WIDGET(drawing_area));
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	keyevc = gtk_event_controller_key_new();
	g_signal_connect(G_OBJECT(keyevc), "key-pressed",
	    G_CALLBACK(key_pressed), NULL);
	g_signal_connect(G_OBJECT(keyevc), "key-released",
	    G_CALLBACK(key_released), NULL);
	gtk_widget_add_controller(GTK_WIDGET(drawing_area), keyevc);

	motionevc = gtk_event_controller_motion_new();
	g_signal_connect(G_OBJECT(motionevc), "motion",
	    G_CALLBACK(mouse_motion), NULL);
	g_signal_connect(G_OBJECT(motionevc), "enter",
	    G_CALLBACK(mouse_enter), NULL);
	g_signal_connect(G_OBJECT(motionevc), "leave",
	    G_CALLBACK(mouse_leave), NULL);
	gtk_widget_add_controller(GTK_WIDGET(drawing_area), motionevc);

	clickges = gtk_gesture_click_new();
	gtk_gesture_single_set_touch_only(GTK_GESTURE_SINGLE(clickges), FALSE);
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(clickges), 0);
	g_signal_connect(G_OBJECT(clickges), "pressed",
	    G_CALLBACK(mouse_pressed), NULL);
	g_signal_connect(G_OBJECT(clickges), "released",
	    G_CALLBACK(mouse_released), NULL);
	gtk_widget_add_controller(GTK_WIDGET(drawing_area),
	    GTK_EVENT_CONTROLLER(clickges));

	g_object_unref(builder);

	gtk_window_present(GTK_WINDOW(window));
}

/*
 * idle process
 */
static volatile int install_count = 0;
static guint idle_id;

void
install_idle_process(void)
{

	if (install_count++ == 0) {
		idle_id = g_idle_add(mainloop, NULL);
		soundmng_play();
	}
}

void
uninstall_idle_process(void)
{

	if (--install_count == 0) {
		soundmng_stop();
		g_source_remove(idle_id);
	}
}

/*
 * toolkit
 */
BRESULT
gui_gtk_arginit(int *argcp, char ***argvp)
{

	xnp2_argcp = argcp;
	xnp2_argvp = argvp;

	return SUCCESS;
}

void
gui_gtk_widget_create(void)
{
	char *homeenv;
	gchar *buf;

	homeenv = getenv("HOME");
	if (homeenv) {
		buf = g_strdup_printf("%s/.np2/gtkrc", homeenv);
#if 0	/* FIXME Deprecated since 3.0 */
		gtk_rc_add_default_file(buf);
#endif
		g_free(buf);

		buf = g_strdup_printf("%s/.np2/accels", homeenv);
#if 0	/* FIXME use GtkShortcutController? GtkShortcut? */
		gtk_accel_map_load(buf);
#endif
		g_free(buf);
	}

	xnp2_app = gtk_application_new("org.nonakap.xnp2",
	    G_APPLICATION_FLAGS_NONE);
	g_signal_connect(xnp2_app, "startup", G_CALLBACK(app_startup), NULL);
	g_signal_connect(xnp2_app, "activate", G_CALLBACK(app_activate), NULL);
}

void
gui_gtk_widget_show(void)
{

	/* Nothing to do */
}

void
gui_gtk_widget_mainloop(void)
{

	install_idle_process();
	g_application_run(G_APPLICATION(xnp2_app), *xnp2_argcp, *xnp2_argvp);
	uninstall_idle_process();
}

void
gui_gtk_widget_quit(void)
{

	taskmng_exit();
	g_application_quit(G_APPLICATION(xnp2_app));
}

void
gui_gtk_event_process(void)
{

	if (taskmng_isavail() &&
	    g_list_model_get_n_items(gtk_window_get_toplevels()) > 0) {
		g_main_context_iteration(NULL, FALSE);
	}
}

void
gui_gtk_set_window_title(const char *title)
{

	gtk_window_set_title(GTK_WINDOW(main_window), title);
}

int
gui_gtk_msgbox(const char *title, const char *msg, UINT flags)
{

	return 0;
}
