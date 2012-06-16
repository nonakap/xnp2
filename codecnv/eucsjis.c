#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_euctosjis(char *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	orgdcnt;
	BOOL	stringmode;
	UINT	h;
	UINT	l;

	if (src == NULL) {
		return(0);
	}
	if (dcnt == 0) {
		dst = NULL;
		dcnt = (UINT)-1;
	}
	orgdcnt = dcnt;
	stringmode = (((SINT)scnt) < 0);
	if (stringmode) {
		dcnt--;
	}
	while(scnt > 0) {
		scnt--;
		h = (UINT8)*src++;
		if ((h == 0) && (stringmode)) {
			break;
		}
		else if (h < 0x80) {			// ascii
			if (dcnt == 0) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)h;
				dst++;
			}
		}
		else if (h == 0x8e) {
			if (scnt == 0) {
				break;
			}
			scnt--;
			l = (UINT8)*src++;
			if (!l) {
				break;
			}
			if (dcnt == 0) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)h;
				dst++;
			}
		}
		else {
			if (scnt == 0) {
				break;
			}
			scnt--;
			l = (UINT8)*src++;
			if (!l) {
				break;
			}
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				h &= 0x7f;
				l &= 0x7f;
				l += ((h & 1) - 1) & 0x5e;
				if (l >= 0x60) {
					l++;
				}
				h += 0x121;
				l += 0x1f;
				h >>= 1;
				h ^= 0x20;
				dst[0] = (char)h;
				dst[1] = (char)l;
				dst += 2;
			}
		}
	}
	if (dst != NULL) {
		if (stringmode) {
			*dst = '\0';
		}
#if 1	// ˆê‰žŒÝŠ·‚Ìˆ×‚É NULL‚Â‚¯‚é
		else if (dcnt) {
			*dst = '\0';
		}
#endif
	}
	return((UINT)(orgdcnt - dcnt));
}

