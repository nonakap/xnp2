#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_ucs2toutf8(char *dst, UINT dcnt, const UINT16 *src, UINT scnt) {

	UINT	orgdcnt;
	BOOL	stringmode;
	UINT	c;

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
		c = *src++;
		scnt--;
		if ((c == '\0') && (stringmode)) {
			break;
		}
		else if (c < 0x80) {
			if (dcnt == 0) {
				break;
			}
			dcnt--;
			if (dst) {
				dst[0] = (char)c;
				dst += 1;
			}
		}
		else if (c < 0x800) {
			if (dcnt < 2) {
				break;
			}
			dcnt -= 2;
			if (dst) {
				dst[0] = (char)(0xc0 + ((c >> 6) & 0x1f));
				dst[1] = (char)(0x80 + ((c >> 0) & 0x3f));
				dst += 2;
			}
		}
		else {
			if (dcnt < 3) {
				break;
			}
			dcnt -= 3;
			if (dst) {
				dst[0] = (char)(0xe0 + ((c >> 12) & 0x0f));
				dst[1] = (char)(0x80 + ((c >> 6) & 0x3f));
				dst[2] = (char)(0x80 + ((c >> 0) & 0x3f));
				dst += 3;
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

