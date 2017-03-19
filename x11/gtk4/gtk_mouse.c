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

#include "mousemng.h"

#include "gtk4/xnp2.h"

typedef struct {
	int mouserunning;	/* showing */
	int lastmouse;		/* working */
	short mousex;
	short mousey;
	UINT8 mouseb;
} mouse_stat_t;

static mouse_stat_t ms_default = {
	0, 0, 0, 0, 0xa0
};
static mouse_stat_t ms;

BRESULT
mousemng_initialize(void)
{

	ms = ms_default;

	return SUCCESS;
}

void
mousemng_term(void)
{

	/* Nothing to do */
}

UINT8
mouse_flag(void)
{

	return ms.mouserunning;
}

void
mouse_running(UINT8 flg)
{
	UINT8 mf = ms.mouserunning;

	switch (flg & 0xc0) {
	case 0x00:
		mf &= ~(1 << (flg & 7));
		break;
	case 0x40:
		mf ^= (1 << (flg & 7));
		break;
	default:
		mf |= (1 << (flg & 7));
		break;
	}

	if ((mf ^ ms.mouserunning) & MOUSE_MASK) {
		ms.mouserunning = (mf & MOUSE_MASK);
#if 0	/* FIXME mouse */
		mouseonoff((ms.mouserunning == 1) ? 1 : 0);
#endif
	}
}

void
mousemng_callback(void)
{
#if 0	/* FIXME mouse */
	int wx, wy;
	int cx, cy;
#endif

	if (ms.lastmouse & 1) {
#if 0	/* FIXME mouse */
		gdk_window_get_pointer(main_window->window, &wx, &wy, NULL);
		getmaincenter(main_window, &cx, &cy);
		ms.mousex += (short)((wx - cx) / 2);
		ms.mousey += (short)((wy - cy) / 2);
		gdk_window_set_pointer(main_window->window, cx, cy);
#endif
	}
}

UINT8
mouse_btn(UINT8 button)
{

	if ((ms.lastmouse & 1) == 0)
		return 0;

	switch (button) {
	case MOUSE_LEFTDOWN:
		ms.mouseb &= 0x7f;
		break;
	case MOUSE_LEFTUP:
		ms.mouseb |= 0x80;
		break;
	case MOUSE_RIGHTDOWN:
		ms.mouseb &= 0xdf;
		break;
	case MOUSE_RIGHTUP:
		ms.mouseb |= 0x20;
		break;
	}
	return 1;
}

UINT8
mousemng_getstat(short *x, short *y, int clear)
{

	*x = ms.mousex;
	*y = ms.mousey;
	if (clear) {
		ms.mousex = 0;
		ms.mousey = 0;
	}
	return ms.mouseb;
}
