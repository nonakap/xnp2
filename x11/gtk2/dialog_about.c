/*	$Id: dialog_about.c,v 1.4 2007/01/10 15:58:43 monaka Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

#include "gtk2/xnp2.h"
#include "gtk2/gtk_menu.h"

#include "resources/np2.xpm"


static void
about_destroy(GtkWidget *w, GtkWidget **wp)
{

	UNUSED(wp);

	install_idle_process();
	gtk_widget_destroy(w);
}

void
create_about_dialog(void)
{
	char work[256];
	GtkWidget *about_dialog;
	GtkWidget *main_widget;
	GtkWidget *ver_label;
	GtkWidget *ok_button;
	GtkWidget *neko_image;
	GdkColormap *colormap;
	GdkPixmap *neko_pixmap;
	GdkBitmap *mask;

	uninstall_idle_process();

	about_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(about_dialog), "About Neko Project II");
	gtk_window_set_position(GTK_WINDOW(about_dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(about_dialog), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(about_dialog), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(about_dialog), 10);
	g_signal_connect(GTK_OBJECT(about_dialog), "destroy",
	    GTK_SIGNAL_FUNC(about_destroy), NULL);

	main_widget = gtk_hbox_new(FALSE, 3);
	gtk_widget_show(main_widget);
	gtk_container_add(GTK_CONTAINER(about_dialog), main_widget);

	colormap = gtk_widget_get_colormap(about_dialog);
	neko_pixmap = gdk_pixmap_colormap_create_from_xpm_d(NULL, colormap,
	    &mask, NULL, (gchar **)np2_icon);
	if (neko_pixmap == NULL)
		g_error("Couldn't create replacement pixmap.");
	neko_image = gtk_image_new_from_pixmap(neko_pixmap, mask);
	g_object_unref(neko_pixmap);
	g_object_unref(mask);
	gtk_widget_show(neko_image);
	gtk_box_pack_start(GTK_BOX(main_widget), neko_image, FALSE, FALSE, 10);

	milstr_ncpy(work, "Neko Project II\n", sizeof(work));
	milstr_ncat(work, NP2VER_CORE, sizeof(work));
#if defined(NP2VER_X11)
	milstr_ncat(work, NP2VER_X11, sizeof(work));
#endif
	ver_label = gtk_label_new(work);
	gtk_widget_show(ver_label);
	gtk_box_pack_start(GTK_BOX(main_widget), ver_label, FALSE, FALSE, 10);

	ok_button = gtk_button_new_from_stock(GTK_STOCK_OK);
	gtk_widget_show(ok_button);
	gtk_box_pack_end(GTK_BOX(main_widget), ok_button, FALSE, TRUE, 0);
	g_signal_connect_swapped(GTK_OBJECT(ok_button), "clicked",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(about_dialog));
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_HAS_DEFAULT);
	gtk_widget_grab_default(ok_button);

	gtk_widget_show_all(about_dialog);
}
