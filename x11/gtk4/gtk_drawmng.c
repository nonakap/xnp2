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
#include "palettes.h"
#include "scrndraw.h"

#include "scrnmng.h"

#include "gtk4/xnp2.h"
#include "gtk4/gtk_drawmng.h"


DRAWMNG_HDL
drawmng_create(void *parent, int width, int height)
{
	return NULL;
}

void
drawmng_release(DRAWMNG_HDL dhdl)
{
}

CMNVRAM *
drawmng_surflock(DRAWMNG_HDL dhdl)
{
	return NULL;
}

void
drawmng_surfunlock(DRAWMNG_HDL dhdl)
{
}

void
drawmng_blt(DRAWMNG_HDL dhdl, RECT_T *sr, POINT_T *dp)
{
}

void
drawmng_set_size(DRAWMNG_HDL dhdl, int width, int height)
{
}

void
drawmng_invalidate(DRAWMNG_HDL dhdl, RECT_T *r)
{
}

void *
drawmng_get_widget_handle(DRAWMNG_HDL dhdl)
{
	return NULL;
}
