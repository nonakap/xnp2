
enum {
	SUCCESS		= 0,
	FAILURE		= 1
};

#ifndef PI
#define PI 3.14159265357989
#endif

#ifndef BRESULT
#define	BRESULT		UINT
#endif

#ifndef INTPTR
#define	INTPTR		long
#endif

#ifndef LOADINTELDWORD
#define	LOADINTELDWORD(a)		(((UINT32)(a)[0]) |				\
								((UINT32)(a)[1] << 8) |			\
								((UINT32)(a)[2] << 16) |		\
								((UINT32)(a)[3] << 24))
#endif

#ifndef LOADINTELWORD
#define	LOADINTELWORD(a)		(((UINT16)(a)[0]) | ((UINT16)(a)[1] << 8))
#endif

#ifndef STOREINTELDWORD
#define	STOREINTELDWORD(a, b)	*((a)+0) = (UINT8)((b));		\
								*((a)+1) = (UINT8)((b)>>8);		\
								*((a)+2) = (UINT8)((b)>>16);	\
								*((a)+3) = (UINT8)((b)>>24)
#endif

#ifndef STOREINTELWORD
#define	STOREINTELWORD(a, b)	*((a)+0) = (UINT8)((b));			\
								*((a)+1) = (UINT8)((b)>>8)
#endif

#ifndef	NELEMENTS
#define	NELEMENTS(a)	((int)(sizeof(a) / sizeof(a[0])))
#endif


// ---- Optimize Macros

#ifndef REG8
#define	REG8		UINT8
#endif
#ifndef REG16
#define	REG16		UINT16
#endif

#ifndef LOW8
#define	LOW8(a)					((UINT8)(a))
#endif
#ifndef LOW10
#define	LOW10(a)				((a) & 0x03ff)
#endif
#ifndef LOW11
#define	LOW11(a)				((a) & 0x07ff)
#endif
#ifndef LOW12
#define	LOW12(a)				((a) & 0x0fff)
#endif
#ifndef LOW14
#define	LOW14(a)				((a) & 0x3fff)
#endif
#ifndef LOW15
#define	LOW15(a)				((a) & 0x7fff)
#endif
#ifndef LOW16
#define	LOW16(a)				((UINT16)(a))
#endif
#ifndef HIGH16
#define	HIGH16(a)				(((UINT32)(a)) >> 16)
#endif


#ifndef OEMCHAR
#define	OEMCHAR					char
#endif
#ifndef OEMTEXT
#define	OEMTEXT(string)			string
#endif
#ifndef OEMNULLSTR
#define	OEMNULLSTR				OEMTEXT("")
#endif


#if 0
#ifndef STRLITERAL
#ifdef UNICODE
#define	STRLITERAL(string)		_T(string)
#else
#define	STRLITERAL(string)		(string)
#endif
#endif
#endif

#if !defined(RGB16)
#define	RGB16		UINT16
#endif

#if !defined(RGB32)
#if defined(BYTESEX_LITTLE)
typedef union {
	UINT32	d;
	struct {
		UINT8	b;
		UINT8	g;
		UINT8	r;
		UINT8	e;
	} p;
} RGB32;
#define	RGB32D(r, g, b)		(((r) << 16) + ((g) << 8) + ((b) << 0))
#elif defined(BYTESEX_BIG)
typedef union {
	UINT32	d;
	struct {
		UINT8	e;
		UINT8	r;
		UINT8	g;
		UINT8	b;
	} p;
} RGB32;
#define	RGB32D(r, g, b)		(((r) << 16) + ((g) << 8) + ((b) << 0))
#endif
#endif


#define	FTYPEID(a, b, c, d)	(((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

enum {
	FTYPE_NONE		= 0,
	FTYPE_SMIL		= FTYPEID('S','M','I','L'),
	FTYPE_TEXT		= FTYPEID('T','E','X','T'),
	FTYPE_BMP		= FTYPEID('B','M','P',' '),
	FTYPE_GIF		= FTYPEID('G','I','F',' '),
	FTYPE_WAVE		= FTYPEID('W','A','V','E'),
	FTYPE_OGG		= FTYPEID('O','G','G',' '),
	FTYPE_MP3		= FTYPEID('M','P','3',' '),
	FTYPE_D88		= FTYPEID('.','D','8','8'),
	FTYPE_FDI		= FTYPEID('.','F','D','I'),
	FTYPE_BETA		= FTYPEID('B','E','T','A'),
	FTYPE_THD		= FTYPEID('.','T','H','D'),
	FTYPE_NHD		= FTYPEID('.','N','H','D'),
	FTYPE_HDI		= FTYPEID('.','H','D','I'),
	FTYPE_HDD		= FTYPEID('.','H','D','D'),
	FTYPE_S98		= FTYPEID('.','S','9','8'),
	FTYPE_MIMPI		= FTYPEID('M','I','M','P')
};


#if !defined(INLINE)
#define	INLINE
#endif
#if !defined(FASTCALL)
#define	FASTCALL
#endif

#if !defined(GCC_ATTR_REGPARM)
#define GCC_ATTR_REGPARM
#endif

