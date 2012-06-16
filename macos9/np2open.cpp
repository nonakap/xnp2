#include	"compiler.h"
#include	"bmpdata.h"
#include	"scrnmng.h"
#include	"np2open.h"
#include	"nekop2.res"


static BYTE *lzxsolve(const BYTE *dat) {

	BYTE	*ret;
	BYTE	*ptr;
	int		size;
	int		level;
	BYTE	ctrl;
	BYTE	bit;
	UINT	mask;
	UINT	tmp;
	int		pos;
	int		leng;

	ret = NULL;
	if (dat == NULL) {
		return(NULL);
	}
	size = dat[0] + (dat[1] << 8) + (dat[2] << 16);
	level = dat[3];
	dat += 4;
	ret = (BYTE *)_MALLOC(size, "res");
	if (ret == NULL) {
		goto lxz_err;
	}
	ptr = ret;

	ctrl = 0;
	bit = 0;
	mask = (1 << level) - 1;
	while(size) {
		if (!bit) {
			ctrl = *dat++;
			bit = 0x80;
		}
		if (ctrl & bit) {
			tmp = *dat++;
			tmp <<= 8;
			tmp |= *dat++;
			pos = -1 - (tmp >> level);
			leng = (tmp & mask) + 1;
			leng = min(leng, size);
			size -= leng;
			while(leng--) {
				*ptr = *(ptr + pos);
				ptr++;
			}
		}
		else {
			*ptr++ = *dat++;
			size--;
		}
		bit >>= 1;
	}

lxz_err:
	return(ret);
}

void np2open(void) {

	BYTE		*res;
const BMPFILE	*bf;
const BMPINFO	*bi;
	BYTE		*paltbl;
	BMPDATA		bd;
	int			bmpalign;
const SCRNSURF	*surf;
	BYTE		*dst;
	int			width;
	int			y;
	int			x;
const BYTE		*src;
	int			pal;

	res = lzxsolve(nekop2_bmp);
	if (res == NULL) {
		goto n2o_err1;
	}
	bf = (BMPFILE *)res;
	bi = (BMPINFO *)(bf + 1);
	paltbl = (BYTE *)(bi + 1);
	if (((bf->bfType[0] != 'B') && (bf->bfType[1] != 'M')) ||
		(bmpdata_getinfo(bi, &bd) != SUCCESS) || (bd.bpp != 4)) {
		goto n2o_err2;
	}
	src = res + LOADINTELDWORD(bf->bfOffBits);
	bmpalign = bmpdata_getalign(bi);
	if (bd.height > 0) {
		src += (bd.height - 1) * bmpalign;
		bmpalign *= -1;
	}
	else {
		bd.height *= -1;
	}

	surf = scrnmng_surflock();
	if ((surf == NULL) || (surf->bpp != 32)) {
		goto n2o_err2;
	}
	// screen clear;
	dst = surf->ptr;
	width = surf->width + surf->extend;
	for (y=0; y<surf->height; y++) {
		for (x=0; x<width; x++) {
			*(UINT32 *)dst = 0;
			dst += surf->xalign;
		}
		dst += surf->yalign - (width * surf->xalign);
	}
	// put resource
	if ((width < bd.width) || (surf->height < bd.height)) {
		goto n2o_err3;
	}
	dst = surf->ptr;
	dst += ((width - bd.width) / 2) * surf->xalign +
			((surf->height - bd.height) / 2) * surf->yalign;
	for (y=0; y<bd.height; y++) {
		for (x=0; x<bd.width; x++) {
			if (!(x & 1)) {
				pal = src[x >> 1] >> 4;
			}
			else {
				pal = src[x >> 1] & 15;
			}
			((RGB32 *)dst)->p.r = paltbl[pal*4+2];
			((RGB32 *)dst)->p.g = paltbl[pal*4+1];
			((RGB32 *)dst)->p.b = paltbl[pal*4+0];
			dst += surf->xalign;
		}
		dst += surf->yalign - (bd.width * surf->xalign);
		src += bmpalign;
	}

n2o_err3:
	scrnmng_surfunlock(surf);

n2o_err2:
	_MFREE(res);

n2o_err1:
	return;
}

