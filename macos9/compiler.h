#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<stddef.h>
#if defined(CPUCORE_IA32)
#include	<setjmp.h>
#endif
#ifndef NP2GCC
#include	<MultiProcessing.h>
#endif

#define	MACOS
#define	BYTESEX_BIG
#define	OSLANG_SJIS
#define	OSLINEBREAK_CR


typedef	signed int		SINT;
typedef	unsigned int	UINT;

typedef	signed char		SINT8;
typedef	unsigned char	UINT8;

typedef	signed short	SINT16;
typedef	unsigned short	UINT16;

typedef	signed int		SINT32;
typedef	unsigned int	UINT32;

typedef	SInt64			SINT64;
typedef	UInt64			UINT64;

typedef	Boolean			BOOL;
typedef	signed char		TCHAR;
typedef	signed char		CHAR;
typedef	unsigned char	BYTE;

#define	MAX_PATH	260

#define	ZeroMemory(a, b)		memset((a),  0 , (b))
#define	FillMemory(a, b, c)		memset((a), (c), (b))
#define	CopyMemory(a, b, c)		memcpy((a), (b), (c))

#define	max(a, b)				(((a)>(b))?(a):(b))
#define	min(a, b)				(((a)<(b))?(a):(b))

#define	BRESULT				UINT8
#define	OEMCHAR				char
#define	OEMTEXT(string)		string
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen


#include	"common.h"
#include	"macossub.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()			macos_gettick()
#define	__ASSERT(s)
#define	SPRINTF				sprintf
#define	STRLEN				strlen

#define	VERMOUTH_LIB
// #define SOUND_CRITICAL

#define	SUPPORT_SJIS

// #define	SUPPORT_8BPP
#ifdef NP2GCC
#define	SUPPORT_16BPP
#endif
// #define	SUPPORT_24BPP
#define	SUPPORT_32BPP
// #define SUPPORT_NORMALDISP
#define	MEMOPTIMIZE		1

#if defined(CPUCORE_IA32)
#define	SUPPORT_CRT31KHZ
#define	SUPPORT_PC9821
#define	IA32_PAGING_EACHSIZE
#endif
#define	SUPPORT_CRT15KHZ
#define	SUPPORT_HOSTDRV
#define	SUPPORT_SWSEEKSND
#define	SUPPORT_SASI
#define	SUPPORT_SCSI

#define	SUPPORT_RESUME
#define	SUPPORT_STATSAVE

#define	SOUNDRESERVE	80


#if defined(CPUCORE_IA32)
#define	sigjmp_buf				jmp_buf
#define	sigsetjmp(env, mask)	setjmp(env)
#define	siglongjmp(env, val)	longjmp(env, val)
#define	msgbox(title, msg)		TRACEOUT(("%s", title)); \
								TRACEOUT(("%s", msg))
#endif

