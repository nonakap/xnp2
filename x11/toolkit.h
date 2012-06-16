/*	$Id: toolkit.h,v 1.11 2007/01/10 18:02:21 monaka Exp $	*/

/*
 * Copyright (c) 2003 NONAKA Kimihiro
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

#ifndef	NP2_X11_TOOLKIT_H__
#define	NP2_X11_TOOLKIT_H__

typedef struct {
	const char*	(*get_toolkit)(void);
	BOOL		(*arginit)(int* argc, char*** argv);
	void		(*terminate)(void);
	void		(*widget_create)(void);
	void		(*widget_show)(void);
	void		(*widget_mainloop)(void);
	void		(*widget_quit)(void);
	void		(*event_process)(void);
	void		(*set_window_title)(const char* str);
	void		(*messagebox)(const char *title, const char *msg);
} gui_toolkit_t;

void toolkit_msgbox(const char *title, const char *msg);

#if (USE_GTK2 + USE_SDL) > 1

extern gui_toolkit_t* toolkitp;

void toolkit_initialize(void);
#define	toolkit_terminate()		(*toolkitp->terminate)()
#define	toolkit_arginit(argcp, argvp)	(*toolkitp->arginit)(argcp, argvp)
#define	toolkit_widget_create()		(*toolkitp->widget_create)()
#define	toolkit_widget_show()		(*toolkitp->widget_show)()
#define	toolkit_widget_mainloop()	(*toolkitp->widget_mainloop)()
#define	toolkit_widget_quit()		(*toolkitp->widget_quit)()
#define	toolkit_event_process()		(*toolkitp->event_process)()
#define	toolkit_set_window_title(s)	(*toolkitp->set_window_title)(s)
#define	toolkit_messagebox(t,m)		(*toolkitp->messagebox)(t,m)

#elif USE_GTK2 > 0

#include "gtk2/gtk_toolkit.h"

#define	toolkit_initialize()
#define	toolkit_terminate()
#define	toolkit_arginit(argcp, argvp)	gui_gtk_arginit(argcp, argvp)
#define	toolkit_widget_create()		gui_gtk_widget_create()
#define	toolkit_widget_show()		gui_gtk_widget_show()
#define	toolkit_widget_mainloop()	gui_gtk_widget_mainloop()
#define	toolkit_widget_quit()		gui_gtk_widget_quit()
#define	toolkit_event_process()		gui_gtk_event_process()
#define	toolkit_set_window_title(s)	gui_gtk_set_window_title(s)
#define	toolkit_messagebox(t,m)		gui_gtk_messagebox(t,m)

#elif USE_SDL > 0

#include "sdl/sdl_toolkit.h"

#define	toolkit_initialize()
#define	toolkit_terminate()
#define	toolkit_arginit(argcp, argvp)	gui_sdl_arginit(argcp, argvp)
#define	toolkit_widget_create()		gui_sdl_widget_create()
#define	toolkit_widget_show()		gui_sdl_widget_show()
#define	toolkit_widget_mainloop()	gui_sdl_widget_mainloop()
#define	toolkit_widget_quit()		gui_sdl_widget_quit()
#define	toolkit_event_process()		gui_sdl_event_process()
#define	toolkit_set_window_title(s)	gui_sdl_set_window_title(s)
#define	toolkit_messagebox(t,m)		gui_sdl_messagebox(t,m)

#else

#error need to defined USE_GTK2 or USE_SDL !!!

#endif

#endif	/* NP2_X11_TOOLKIT_H__ */
