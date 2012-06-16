/*	$Id: toolkit.c,v 1.7 2007/01/10 18:02:21 monaka Exp $	*/

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

#include "compiler.h"

#include "np2.h"

#include "toolkit.h"

#include "sysmng.h"

#if (USE_GTK2 + USE_SDL) > 1

gui_toolkit_t* toolkitp;

static struct {
	gui_toolkit_t*	toolkit;
} toolkit[] = {
#if USE_GTK2 > 0
	{ &gtk_toolkit, },
#endif
#if USE_SDL > 0
	{ &sdl_toolkit, },
#endif
};

void
toolkit_initialize(void)
{
	int i;

	if (NELEMENTS(toolkit) > 0) {
		for (i = 0; i < NELEMENTS(toolkit); i++) {
			gui_toolkit_t* p = toolkit[i].toolkit;
			if (strcasecmp(p->get_toolkit(), np2oscfg.toolkit) == 0)
				break;
		}
		if (i < NELEMENTS(toolkit)) {
			toolkitp = toolkit[i].toolkit;
			return;
		}
		sysmng_update(SYS_UPDATEOSCFG);
		milstr_ncpy(np2oscfg.toolkit, "gtk", sizeof(np2oscfg.toolkit));
	}
	toolkitp = &gtk_toolkit;
}

#endif	/* USE_GTK2 + USE_SDL > 1 */

void
toolkit_msgbox(const char *title, const char *msg)
{

	toolkit_messagebox(title, msg);
}
