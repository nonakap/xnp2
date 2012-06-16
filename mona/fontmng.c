#include	"compiler.h"
#include	"fontmng.h"


#include	"ank12.res"

typedef struct {
	int		fontsize;
	UINT	fonttype;
} _FNTMNG, *FNTMNG;

BOOL fontmng_init(void) {

	return(SUCCESS);
}

void fontmng_setdeffontname(const char *name) {

	(void)name;
}

void *fontmng_create(int size, UINT type, const char *fontface) {

	int		fontalign;
	int		allocsize;
	FNTMNG	ret;

	if (size < ANKFONTSIZE) {
		goto fmc_err1;
	}
	fontalign = sizeof(_FNTDAT) + (size * size);
	fontalign = (fontalign + 3) & (~3);
	allocsize = sizeof(_FNTMNG) + fontalign;
	ret = (FNTMNG)_MALLOC(allocsize, "font mng");
	if (ret == NULL) {
		goto fmc_err1;
	}
	ZeroMemory(ret, allocsize);
	ret->fontsize = size;
	ret->fonttype = type;
	return(ret);

fmc_err1:
	(void)fontface;
	return(NULL);
}

void fontmng_destroy(void *hdl) {

	if (hdl) {
		_MFREE(hdl);
	}
}

static void setfdathead(FNTMNG fhdl, FNTDAT fdat, int width) {

	if (fhdl->fonttype & FDAT_PROPORTIONAL) {
		fdat->width = width;
		fdat->pitch = width + 1;
		fdat->height = fhdl->fontsize;
	}
	else {
		fdat->width = max(width, fhdl->fontsize >> 1);
		fdat->pitch = (fhdl->fontsize >> 1) + 1;
		fdat->height = fhdl->fontsize;
	}
}

static void getlength1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	int		c;

	c = string[0] - 0x20;
	if ((c < 0) || (c >= 0x60)) {
		c = 0x1f;							// ?
	}
	setfdathead(fhdl, fdat, ankfont[c * ANKFONTSIZE]);
}

static void getfont1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	int		c;
const UINT8	*src;
	int		width;
	UINT8	*dst;
	int		x;
	int		y;

	c = string[0] - 0x20;
	if ((c < 0) || (c >= 0x60)) {
		c = 0x1f;							// ?
	}
	src = ankfont + (c * ANKFONTSIZE);
	width = *src++;
	setfdathead(fhdl, fdat, width);
	dst = (UINT8 *)(fdat + 1);
	ZeroMemory(dst, fdat->width * fdat->height);
	dst += ((fdat->height - ANKFONTSIZE) / 2) * fdat->width;
	dst += (fdat->width - width) / 2;
	for (y=0; y<(ANKFONTSIZE - 1); y++) {
		dst += fdat->width;
		for (x=0; x<width; x++) {
			dst[x] = (src[0] & (0x80 >> x))?0xff:0x00;
		}
		src++;
	}
}

BOOL fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	FNTMNG	fhdl;
	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}
	fhdl = (FNTMNG)hdl;

	width = 0;
	buf[2] = '\0';
	while(1) {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		getlength1(fhdl, &fdat, buf, leng);
		width += fdat.pitch;
	}
	if (pt) {
		pt->x = width;
		pt->y = fhdl->fontsize;
	}
	return(SUCCESS);

fmgs_exit:
	return(FAILURE);
}

BOOL fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	FNTMNG	fhdl;
	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		leng;
	int		posx;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}
	fhdl = (FNTMNG)hdl;

	width = 0;
	posx = 0;
	buf[2] = '\0';
	while(1) {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		getlength1(fhdl, &fdat, buf, leng);
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	}
	if (pt) {
		pt->x = width;
		pt->y = fhdl->fontsize;
	}
	return(SUCCESS);

fmgds_exit:
	return(FAILURE);
}

FNTDAT fontmng_get(void *hdl, const char *string) {

	FNTMNG	fhdl;
	FNTDAT	fdat;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmg_err;
	}
	fhdl = (FNTMNG)hdl;
	fdat = (FNTDAT)(fhdl + 1);

	leng = 1;
	if (((((string[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) &&
		(string[1] != '\0')) {
		leng = 2;
	}
	getfont1(fhdl, fdat, string, leng);
	return(fdat);

fmg_err:
	return(NULL);
}

