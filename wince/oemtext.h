
#include	"codecnv.h"

#ifdef __cplusplus
extern "C" {
#endif

UINT oemtext_sjistoucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_ucs2tosjis(char *dst, UINT dcnt, const UINT16 *src, UINT scnt);
UINT oemtext_sjistoutf8(char *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_utf8tosjis(char *dst, UINT dcnt, const char *src, UINT scnt);

#ifdef __cplusplus
}
#endif


#if defined(OSLANG_UTF8)
#define oemtext_sjistooem		oemtext_sjistoutf8
#define	oemtext_oemtosjis		oemtext_utf8tosjis
#elif defined(OSLANG_UCS2)
#define oemtext_sjistooem		oemtext_sjistoucs2
#define	oemtext_oemtosjis		oemtext_ucs2tosjis
#endif


// ---- Windows—p TCHAR-OEMCHAR•ÏŠ·

#undef OEMCHAR_SAME_TCHAR

#if !defined(_UNICODE) && defined(OSLANG_UCS2)
#define	tchartooem		oemtext_sjistoucs2
#define	oemtotchar		oemtext_ucs2tosjis
#elif !defined(_UNICODE) && defined(OSLANG_UTF8)
#define	tchartooem		oemtext_sjistoutf8
#define	oemtotchar		oemtext_utf8tosjis
#elif defined(_UNICODE) && (defined(OSLANG_ANK) || defined(OSLANG_SJIS))
#define tchartooem		oemtext_ucs2tosjis
#define oemtotchar		oemtext_sjistoucs2
#elif defined(_UNICODE) && defined(OSLANG_UTF8)
#define	tchartooem		codecnv_ucs2toutf8
#define	oemtotchar		codecnv_utf8toucs2
#else
#define OEMCHAR_SAME_TCHAR
#endif

