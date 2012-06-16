#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_sjistoeuc(char *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	orgdcnt;
	BOOL	stringmode;
	int		s;
	int		c;

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
		s = (UINT8)*src++;
		if ((s == '\0') && (stringmode)) {
			break;
		}
		else if (s < 0x80) {			// ascii
			if (dcnt == 0) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)s;
				dst++;
			}
		}
		else if ((((s ^ 0x20) - 0xa1) & 0xff) < 0x2f) {
			if (scnt == 0) {
				break;
			}
			scnt--;
			c = (UINT8)*src++;
			if (c == '\0') {
				break;
			}
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				c += 0x62 - ((c & 0x80) >> 7);
				if (c < 256) {
					c = (c - 0xa2) & 0x1ff;
				}
				c += 0x9fa1;
				dst[0] = (char)(((s & 0x3f) << 1) + (c >> 8));
				dst[1] = (char)c;
				dst += 2;
			}
		}
		else if (((s - 0xa0) & 0xff) < 0x40) {
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				dst[0] = (char)0x8e;
				dst[1] = (char)s;
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

