#include	"compiler.h"
#include	"oemtext.h"


#if defined(_UNICODE)
UINT oemtext_sjistooem(TCHAR *dst, UINT dcnt, const char *src, UINT scnt) {

	return(MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, dst, dcnt));
}

UINT oemtext_oemtosjis(char *dst, UINT dcnt, const TCHAR *src, UINT scnt) {

	return(WideCharToMultiByte(CP_ACP, 0, src, -1, dst, dcnt, NULL, NULL));
}
#endif

