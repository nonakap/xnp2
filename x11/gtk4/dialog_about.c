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
#include "np2ver.h"

#include "gtk4/xnp2.h"
#include "gtk4/dialog_about.h"

#if !defined(NP2VER_X11)
#define NP2VER_X11	""
#endif

struct _Xnp2AboutDialog {
	GtkDialog	parent;
	GtkLabel	*version;
};

G_DEFINE_TYPE(Xnp2AboutDialog, xnp2_about_dialog, GTK_TYPE_DIALOG)

static void
xnp2_about_dialog_init(Xnp2AboutDialog *dialog)
{
	char *xnp2ver;

	uninstall_idle_process();

	gtk_widget_init_template(GTK_WIDGET(dialog));

	xnp2ver = g_strdup_printf("%s\n%s%s",
#if defined(SUPPORT_PC9821)
	    "Neko Project 21",
#else
	    "Neko Project II",
#endif
	    NP2VER_CORE, NP2VER_X11);
	gtk_label_set_text(dialog->version, xnp2ver);
	g_free(xnp2ver);

	g_signal_connect_swapped(G_OBJECT(dialog), "response",
	    G_CALLBACK(gtk_window_destroy), GTK_WIDGET(dialog));
}

static void
xnp2_about_dialog_dispose(GObject *object)
{
	Xnp2AboutDialog *dialog G_GNUC_UNUSED = XNP2_ABOUT_DIALOG(object);

	G_OBJECT_CLASS(xnp2_about_dialog_parent_class)->dispose(object);

	install_idle_process();
}

static void
xnp2_about_dialog_class_init(Xnp2AboutDialogClass *class)
{

	G_OBJECT_CLASS(class)->dispose = xnp2_about_dialog_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
	    XNP2_RESOUCE_PREFIX "dialog_about.ui");
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class),
	    Xnp2AboutDialog, version);
}

Xnp2AboutDialog *
xnp2_about_dialog_new(GtkWindow *window)
{
	gboolean use_header_bar;

	g_object_get(gtk_settings_get_default(), "gtk-dialogs-use-header",
	    &use_header_bar, NULL);

	return g_object_new(XNP2_ABOUT_DIALOG_TYPE, "transient-for", window,
	    "use-header-bar", use_header_bar, NULL);
}
