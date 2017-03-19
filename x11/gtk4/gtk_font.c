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
#include "codecnv/codecnv.h"

#include "fontmng.h"

#include "gtk4/xnp2.h"

BRESULT
fontmng_init(void)
{

	fontmng_setdeffontname("fixed");

	return SUCCESS;
}

void
fontmng_terminate(void)
{

	/* Nothing to do */
}

void
fontmng_setdeffontname(const OEMCHAR *fontface)
{

	milstr_ncpy(fontname, fontface, sizeof(fontname));
}

void *
fontmng_create(int size, UINT type, const OEMCHAR *fontface)
{
	return NULL;
}

void
fontmng_destroy(void *hdl)
{
}

BRESULT
fontmng_getsize(void *hdl, const char *str, POINT_T *pt)
{
	return SUCCESS;
}

BRESULT
fontmng_getdrawsize(void *hdl, const char *str, POINT_T *pt)
{
	return SUCCESS;
}

FNTDAT
fontmng_get(void *hdl, const char *str)
{
	return NULL;
}
