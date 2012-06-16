/*	$Id: compiler.h,v 1.34 2007/01/23 15:48:20 monaka Exp $	*/

/*-
 * Copyright (c) 2003, 2004 NONAKA Kimihiro
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef	NP2_X11_COMPILER_H__
#define	NP2_X11_COMPILER_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WORDS_BIGENDIAN
#define	BYTESEX_BIG
#else	/* !WORDS_BIGENDIAN */
#define	BYTESEX_LITTLE
#endif	/* WORDS_BIGENDIAN */

#if !defined(USE_SDLAUDIO) && !defined(USE_SDLMIXER)
#ifndef	NOSOUND
#define	NOSOUND
#undef	VERMOUTH_LIB
#endif	/* !NOSOUND */
#else	/* USE_SDLAUDIO || USE_SDLMIXER */
#undef	NOSOUND
#define	VERMOUTH_LIB
#endif	/* !USE_SDLAUDIO && !USE_SDLMIXER */

#include <sys/param.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#define	X11
#define	OSLANG_EUC
#define	OSLINEBREAK_LF

#if (USE_GTK2 > 0)

#include <glib.h>

typedef	gint32		SINT;
typedef	guint32		UINT;

typedef	gint8		SINT8;
typedef	gint16		SINT16;
typedef	gint32		SINT32;
typedef	gint64		SINT64;

typedef	guint8		UINT8;
typedef	guint16		UINT16;
typedef	guint32		UINT32;
typedef	guint64		UINT64;

typedef	guint8		BYTE;
typedef	gchar		TCHAR;

typedef	gboolean	BOOL;

#define PTR_TO_UINT32(p)	((UINT32)GPOINTER_TO_UINT(p))
#define UINT32_TO_PTR(v)	GUINT_TO_POINTER((UINT32)(v))

#else	/* USE_GTK2 <= 0 */

typedef	signed int		SINT;
typedef	unsigned int		UINT;

typedef	signed char		SINT8;
typedef	signed short		SINT16;
typedef	signed int		SINT32;
typedef	signed long long	SINT64;

typedef	unsigned char		UINT8;
typedef	unsigned short		UINT16;
typedef	unsigned int		UINT32;
typedef	unsigned long long	UINT64;

typedef	unsigned char		BYTE;
typedef	char			TCHAR;

typedef	int			BOOL;

#endif	/* USE_GTK2 > 0 */

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	MAX_PATH
#define	MAX_PATH	MAXPATHLEN
#endif

#ifndef	max
#define	max(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#ifndef	min
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef	ZeroMemory
#define	ZeroMemory(d,n)		memset((d), 0, (n))
#endif
#ifndef	CopyMemory
#define	CopyMemory(d,s,n)	memcpy((d), (s), (n))
#endif
#ifndef	FillMemory
#define	FillMemory(a, b, c)	memset((a), (c), (b))
#endif

#ifndef	roundup
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#endif

#ifndef	UNUSED
#define	UNUSED(v)	((void)(v))
#endif

#define	FASTCALL
#define	SOUNDCALL
#define	MEMCALL
#define	CPUCALL

#ifdef	DEBUG
#define	INLINE
#define	__ASSERT(s)	assert(s)
#else
#ifndef	__ASSERT
#define	__ASSERT(s)
#endif
#ifndef	INLINE
#define	INLINE		inline
#endif
#endif

/* archtecture */
/* amd64 */
#if defined(amd64) || defined(__AMD64__) || defined(__amd64__) || \
    defined(x86_64) || defined(__x86_64__) || defined(__X86_64__)
#define	NP2_CPU_ARCH_AMD64
#endif /* amd64 */
/* i386 */
#if defined(i386) || defined(__i386__) || defined(NP2_CPU_ARCH_AMD64)
#define	NP2_CPU_ARCH_IA32
#endif /* i386 */

#if defined(__GNUC__)
#define	GCC_ATTR_PACKED	__attribute__((packed))
#if defined(NP2_CPU_ARCH_IA32)
#define	GCC_CPU_ARCH_IA32
#endif
#if defined(NP2_CPU_ARCH_AMD64)
#define	GCC_CPU_ARCH_AMD64
#endif
#endif /* __GNUC__ */

#ifndef	NELEMENTS
#define	NELEMENTS(a)	((int)(sizeof(a) / sizeof(a[0])))
#endif

UINT32 gettick(void);
#define	GETTICK()	gettick()
#define	GETRAND()	random()
#define	SPRINTF		sprintf
#define	STRLEN		strlen

#define	OEMCHAR		char
#define OEMTEXT(s)	s
#define OEMNULLSTR	OEMTEXT("")
#define	OEMSPRINTF	sprintf
#define	OEMSTRLEN	strlen

#if defined(CPUCORE_IA32)
void toolkit_msgbox(const char *title, const char *msg);
#define	msgbox(title, msg)	toolkit_msgbox(title, msg);

#define	SUPPORT_PC9821
#define	SUPPORT_CRT31KHZ
#define	SUPPORT_IDEIO
#else
#define	SUPPORT_CRT15KHZ
#endif

#if defined(NP2_CPU_ARCH_IA32)
#undef	MEMOPTIMIZE
#define LOADINTELDWORD(a)	(*((UINT32 *)(a)))
#define LOADINTELWORD(a)	(*((UINT16 *)(a)))
#define STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)
#if defined(__GNUC__) && defined(IA32_USE_GCC_ATTR_REGPARM)
#define	GCC_ATTR_REGPARM	__attribute__((regparm(2)))
#endif
#elif defined(arm) || defined (__arm__)
#define	MEMOPTIMIZE	2
#define	REG8		UINT
#define	REG16		UINT
#define	OPNGENARM
#else
#define	MEMOPTIMIZE	1
#endif

#define	SUPPORT_EUC

#undef	SUPPORT_8BPP
#define	SUPPORT_16BPP
#define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define	SUPPORT_NORMALDISP

#undef	SOUND_CRITICAL
#undef	SOUNDRESERVE

#define	SUPPORT_PC9861K
#define	SUPPORT_HOSTDRV

#undef	SUPPORT_SASI
#undef	SUPPORT_SCSI

#if (USE_GTK2 > 0)
#define	SUPPORT_S98
#define	SUPPORT_KEYDISP
#define	SUPPORT_SOFTKBD	0
#endif

#if (USE_SDL > 0)
#define	USE_SYSMENU
#define	SCREEN_BPP	16
#undef	SUPPORT_8BPP
#undef	SUPPORT_24BPP
#undef	SUPPORT_32BPP
#endif

#if (USE_SDL > 0) || defined(USE_SDLAUDIO) || defined(USE_SDLMIXER)
#define	SUPPORT_JOYSTICK
#define	USE_SDL_JOYSTICK
#endif	/* USE_SDL > 0 || USE_SDLAUDIO || USE_SDLMIXER */

/*
 * You could specify a complete path, e.g. "/etc/timidity.cfg", and
 * then specify the library directory in the configuration file.
 */
extern char timidity_cfgfile_path[MAX_PATH];
#define	TIMIDITY_CFGFILE	timidity_cfgfile_path

#include "common.h"
#include "milstr.h"
#include "_memory.h"
#include "rect.h"
#include "lstarray.h"
#include "trace.h"

#endif	/* NP2_X11_COMPILER_H__ */
