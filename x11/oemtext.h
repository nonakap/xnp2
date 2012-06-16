#include "codecnv.h"

#if 1
#define	oemtext_sjistooem	codecnv_sjistoeuc
#define	oemtext_oemtosjis	codecnv_euctosjis
#else

G_BEGIN_DECLS

UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);

G_END_DECLS

#endif
