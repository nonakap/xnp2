#include	"compiler.h"
#include	<time.h>
#ifndef NP2GCC
#ifdef TARGET_API_MAC_CARBON
#include	<CFString.h>
#endif
#endif


void macossub_init(void) {
}


void macossub_term(void) {
}


UINT32 macos_gettick(void) {

	UnsignedWide current;

	Microseconds(&current);
	return((UINT32)((current.hi * 4294967) + (current.lo / 1000)));
}


// ---- code

void mkstr255(Str255 dst, const char *src) {

	int		len;

	len = strlen(src);
	if (len >= 255) {
		len = 255;
	}
	dst[0] = (BYTE)len;
	if (len) {
		CopyMemory((char *)dst+1, src, len);
	}
}

void mkcstr(char *dst, int size, const Str255 src) {

	if ((src != NULL) && (dst != NULL) && (size > 0)) {
		size--;
		size = min(size, src[0]);
		if (size) {
			CopyMemory(dst, src + 1, size);
		}
		dst[size] = '\0';
	}
}

