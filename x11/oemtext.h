
#ifdef __cplusplus
extern "C" {
#endif

#include "codecnv.h"

#if 1
#define	oemtext_sjistooem	codecnv_sjistoeuc
#define	oemtext_oemtosjis	codecnv_euctosjis
#else
UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);
#endif

#ifdef __cplusplus
}
#endif

