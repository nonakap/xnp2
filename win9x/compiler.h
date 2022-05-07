/**
 * @file	compiler.h
 * @brief	include file for standard system include files,
 *			or project specific include files that are used frequently,
 *			but are changed infrequently
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/09 00:22:18 $
 */

#include "targetver.h"
#define _USE_MATH_DEFINES
#include <windows.h>
#if !defined(__GNUC__)
#include <tchar.h>
#endif	// !defined(__GNUC__)
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#if defined(TRACE)
#include <assert.h>
#endif

#ifndef _T
#define _T(x)	TEXT(x)
#endif	// !_T

#define	BYTESEX_LITTLE
#if !defined(_UNICODE)
#define	OSLANG_SJIS
#else
#define	OSLANG_UCS2
#endif
#define	OSLINEBREAK_CRLF

#if !defined(__GNUC__)
typedef	signed int			SINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
typedef	signed __int64		SINT64;
typedef	unsigned __int64	UINT64;
#define	INLINE				__inline
#define	QWORD_CONST(v)		((UINT64)(v))
#define	SQWORD_CONST(v)		((SINT64)(v))
#define	snprintf			_snprintf
#define	vsnprintf			_vsnprintf
#else
#include <stdlib.h>
typedef	signed int			SINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	signed __int64		SINT64;
#define	INLINE				inline
#endif
#define	FASTCALL			__fastcall

// for x86
#define	LOADINTELDWORD(a)		(*((UINT32 *)(a)))
#define	LOADINTELWORD(a)		(*((UINT16 *)(a)))
#define	STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define	STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)

#if !defined(__GNUC__)
#define	sigjmp_buf				jmp_buf
#define	sigsetjmp(env, mask)	setjmp(env)
#define	siglongjmp(env, val)	longjmp(env, val)
#endif	// !defined(__GNUC__)
#define	msgbox(title, msg)		MessageBoxA(NULL, msg, title, MB_OK)

#define	STRCALL		__stdcall

#define INTPTR				INT_PTR

#define	BRESULT				UINT8
#define	OEMCHAR				TCHAR
#define	OEMTEXT(string)		_T(string)
#define	OEMSPRINTF			wsprintf
#define	OEMSTRLEN			lstrlen

#include "common.h"
#include "milstr.h"
#include "_memory.h"
#include "rect.h"
#include "lstarray.h"
#include "misc\tickcounter.h"
#include "misc\trace.h"
#include "misc\vc6macros.h"

#define	GETTICK()			GetTickCounter()
#if defined(TRACE)
#define	__ASSERT(s)			assert(s)
#else
#define	__ASSERT(s)
#endif
#if defined(_UNICODE)
#define	SPRINTF				sprintf
#define	STRLEN				strlen
#else
#define	SPRINTF				wsprintf
#define	STRLEN				lstrlen
#endif

#define	LABEL				__declspec(naked)
#define	RELEASE(x) 			if (x) {(x)->Release(); (x) = NULL;}

#if !defined(_WIN64)
#define	OPNGENX86
#endif

#define	VERMOUTH_LIB
#define	MT32SOUND_DLL
#define	PARTSCALL	__fastcall
#define	CPUCALL		__fastcall
#define	MEMCALL		__fastcall
#define	DMACCALL	__fastcall
#define	IOOUTCALL	__fastcall
#define	IOINPCALL	__fastcall
#define	SOUNDCALL	__fastcall
#define	VRAMCALL	__fastcall
#define	SCRNCALL	__fastcall
#define	VERMOUTHCL	__fastcall

#if defined(OSLANG_SJIS)
#define	SUPPORT_SJIS
#else
#define	SUPPORT_ANK
#endif

#define	SUPPORT_8BPP
#define	SUPPORT_16BPP
#define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define	SUPPORT_NORMALDISP

#if defined(SUPPORT_PC9821)
#define	CPUCORE_IA32
#define	IA32_PAGING_EACHSIZE
#define	SUPPORT_CRT31KHZ
#define	SUPPORT_PC9801_119
#endif
#define	SUPPORT_CRT15KHZ
#define	SUPPORT_PC9861K
#define	SUPPORT_SOFTKBD		0
#define SUPPORT_S98
#define SUPPORT_WAVEREC
#define SUPPORT_RECVIDEO
#define	SUPPORT_KEYDISP
#define	SUPPORT_MEMDBG32
#define	SUPPORT_HOSTDRV
#define	SUPPORT_SASI
#define	SUPPORT_SCSI
/* #define	SUPPORT_IDEIO */
#define SUPPORT_ARC
#define SUPPORT_ZLIB
#if !defined(_WIN64)
#define	SUPPORT_DCLOCK
#endif

#define	SUPPORT_RESUME
#define	SUPPORT_STATSAVE	10
#define	SUPPORT_ROMEO

#define SOUND_CRITICAL
#define	SOUNDRESERVE	20
#define SUPPORT_VSTi
#define SUPPORT_ASIO
#if (_MSC_VER >= 1500)
#define SUPPORT_WASAPI
#endif	/* (_MSC_VER >= 1500) */

#define	SUPPORT_TEXTCNV

#if defined(CPUCORE_IA32)
#pragma warning(disable: 4819)
#endif


#if (_MSC_VER >= 1400)
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif	/* (_MSC_VER >= 1400) */
