
#include	"codecnv.h"

#ifdef __cplusplus
extern "C" {
#endif

UINT oemtext_mbtoucs2(UINT cp, wchar_t *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_ucs2tomb(UINT cp, char *dst, UINT dcnt, const wchar_t *src, UINT scnt);
UINT oemtext_mbtoutf8(UINT cp, char *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_utf8tomb(UINT cp, char *dst, UINT dcnt, const char *src, UINT scnt);

UINT oemtext_chartoucs2(wchar_t *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_ucs2tochar(char *dst, UINT dcnt, const wchar_t *src, UINT scnt);
UINT oemtext_chartoutf8(char *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_utf8tochar(char *dst, UINT dcnt, const char *src, UINT scnt);

#ifdef __cplusplus
}
#endif

#if defined(OSLANG_UTF8)
#define oemtext_sjistooem(a, b, c, d)	oemtext_mbtoutf8(932, a, b, c, d)
#define	oemtext_oemtosjis(a, b, c, d)	oemtext_utf8tomb(932, a, b, c, d)
#elif defined(OSLANG_UCS2)
#define oemtext_sjistooem(a, b, c, d)	oemtext_mbtoucs2(932, a, b, c, d)
#define	oemtext_oemtosjis(a, b, c, d)	oemtext_ucs2tomb(932, a, b, c, d)
#endif


// ---- Windows—p TCHAR-OEMCHAR•ÏŠ·

#undef OEMCHAR_SAME_TCHAR

#if !defined(_UNICODE) && defined(OSLANG_UCS2)
#define	tchartooem		oemtext_chartoucs2
#define	oemtotchar		oemtext_ucs2tochar
#elif !defined(_UNICODE) && defined(OSLANG_UTF8)
#define	tchartooem		oemtext_chartoutf8
#define	oemtotchar		oemtext_utf8tochar
#elif defined(_UNICODE) && (defined(OSLANG_ANK) || defined(OSLANG_SJIS))
#define tchartooem		oemtext_ucs2tochar
#define oemtotchar		oemtext_chartoucs2
#elif defined(_UNICODE) && defined(OSLANG_UTF8)
#define	tchartooem		codecnv_ucs2toutf8
#define	oemtotchar		codecnv_utf8toucs2
#else
#define OEMCHAR_SAME_TCHAR
#endif


// ---- strres

#if defined(OEMCHAR_SAME_TCHAR)

#define	tchar_null		str_null
#define	tchar_d			str_d
#define	tchar_u			str_u
#define	tchar_2x		str_2x
#define	tchar_2d		str_2d
#define	tchar_4X		str_4X
#define	tchar_bmp		str_bmp
#define	tchar_d88		str_d88
#define	tchar_thd		str_thd
#define	tchar_hdd		str_hdd

#else

extern const TCHAR tchar_null[];
extern const TCHAR tchar_d[];
extern const TCHAR tchar_u[];
extern const TCHAR tchar_2x[];
extern const TCHAR tchar_2d[];
extern const TCHAR tchar_4X[];
extern const TCHAR tchar_bmp[];
extern const TCHAR tchar_d88[];
extern const TCHAR tchar_thd[];
extern const TCHAR tchar_hdd[];

#endif


// ---- milstr

#if defined(OEMCHAR_SAME_TCHAR)

#define	miltchar_solveHEX(s)	milstr_solveHEX(s)
#define	miltchar_solveINT(s)	milstr_solveINT(s)

#else

long STRCALL miltchar_solveHEX(const TCHAR *str);
long STRCALL miltchar_solveINT(const TCHAR *str);

#endif

