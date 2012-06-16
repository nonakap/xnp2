#include	"compiler.h"
#include	"oemtext.h"


// Use WinAPI version


UINT oemtext_sjistoucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt) {

	int		srccnt;
	int		dstcnt;
	int		r;

	if (((SINT)scnt) > 0) {
		srccnt = scnt;
	}
	else {
		srccnt = -1;
	}
	if (((SINT)dcnt) > 0) {
		dstcnt = dcnt;
		if (srccnt < 0) {
			dstcnt = dstcnt - 1;
			if (dstcnt == 0) {
				if (dst) {
					dst[0] = '\0';
				}
				return(1);
			}
		}
	}
	else {
		dstcnt = 0;
	}
	r = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srccnt, dst, dstcnt);
	if ((r == 0) && (dstcnt != 0)) {
		r = dstcnt;
		if (srccnt < 0) {
			if (dst) {
				dst[r] = '\0';
			}
			r++;
		}
	}
	return(r);
}

UINT oemtext_ucs2tosjis(char *dst, UINT dcnt, const UINT16 *src, UINT scnt) {

	int		srccnt;
	int		dstcnt;
	int		r;

	if (((SINT)scnt) > 0) {
		srccnt = scnt;
	}
	else {
		srccnt = -1;
	}
	if (((SINT)dcnt) > 0) {
		dstcnt = dcnt;
		if (srccnt < 0) {
			dstcnt = dstcnt - 1;
			if (dstcnt == 0) {
				if (dst) {
					dst[0] = '\0';
				}
				return(1);
			}
		}
	}
	else {
		dstcnt = 0;
	}
	r = WideCharToMultiByte(CP_ACP, 0, src, srccnt, dst, dstcnt, NULL, NULL);
	if ((r == 0) && (dstcnt != 0)) {
		r = dstcnt;
		if (srccnt < 0) {
			if (dst) {
				dst[r] = '\0';
			}
			r++;
		}
	}
	return(r);
}

UINT oemtext_sjistoutf8(char *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	leng;
	UINT16	*ucs2;
	UINT	ret;

	(void)scnt;

	leng = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, scnt, NULL, 0);
	if (leng == 0) {
		return(0);
	}
	ucs2 = (UINT16 *)_MALLOC(leng * sizeof(UINT16), "");
	if (ucs2 == NULL) {
		return(0);
	}
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, scnt, ucs2, leng);
	if (((SINT)scnt) < 0) {
		leng = (UINT)-1;
	}
	ret = codecnv_ucs2toutf8(dst, dcnt, ucs2, leng);
	_MFREE(ucs2);
	return(ret);
}

UINT oemtext_utf8tosjis(char *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	leng;
	UINT16	*ucs2;
	UINT	ret;

	(void)scnt;

	leng = codecnv_utf8toucs2(NULL, 0, src, scnt);
	if (leng == 0) {
		return(0);
	}
	ucs2 = (UINT16 *)_MALLOC(leng * sizeof(UINT16), "");
	if (ucs2 == NULL) {
		return(0);
	}
	codecnv_utf8toucs2(ucs2, leng, src, scnt);
	if (((SINT)scnt) < 0) {
		leng = (UINT)-1;
	}
	ret = WideCharToMultiByte(CP_ACP, 0, ucs2, leng, dst, dcnt, NULL, NULL);
	_MFREE(ucs2);
	return(ret);
}

