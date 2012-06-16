#include	"compiler.h"
#include	"codecnv.h"


UINT codecnv_utf8toucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt) {

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
		if ((src[0] == '\0') && (stringmode)) {
			break;
		}
		else if (!(src[0] & 0x80)) {
			c = src[0];
			src += 1;
			scnt -= 1;
		}
		else if ((src[0] & 0xe0) == 0xc0) {
			if ((scnt < 2) ||
				((src[1] & 0xc0) != 0x80)) {
				break;
			}
			c = ((src[0] & 0x1f) << 6) + (src[1] & 0x3f);
			src += 2;
			scnt -= 2;
		}
		else if ((src[0] & 0xf0) == 0xe0) {
			if ((scnt < 3) ||
				((src[1] & 0xc0) != 0x80) ||
				((src[2] & 0xc0) != 0x80)) {
				break;
			}
			c = ((src[0] & 0x0f) << 12) +
								((src[1] & 0x3f) << 6) + (src[2] & 0x3f);
			src += 3;
			scnt -= 3;
		}
		else {
			break;
		}
		if (dcnt == 0) {
			break;
		}
		dcnt--;
		if (dst) {
			dst[0] = (UINT16)c;
			dst += 1;
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

