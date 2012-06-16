#include	"compiler.h"
#include	"bmpdata.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"dispsync.h"
#include	"palettes.h"
#include	"scrnbmp.h"

#if defined(SUPPORT_PC9821)
typedef	unsigned int	PALNUM;
#else
typedef	unsigned char	PALNUM;
#endif

typedef union {
	UINT32	d;
	UINT8	rgb[4];
} BMPPAL;


static void screenmix(PALNUM *dest, const UINT8 *src1, const UINT8 *src2) {

	int		i;

	for (i=0; i<(SURFACE_WIDTH * SURFACE_HEIGHT); i++) {
		dest[i] = src1[i] + src2[i] + NP2PAL_GRPH;
	}
}

static void screenmix2(PALNUM *dest, const UINT8 *src1, const UINT8 *src2) {

	int		x, y;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		for (x=0; x<SURFACE_WIDTH; x++) {
			dest[x] = src1[x] + src2[x] + NP2PAL_GRPH;
		}
		dest += SURFACE_WIDTH;
		src1 += SURFACE_WIDTH;
		src2 += SURFACE_WIDTH;
		for (x=0; x<SURFACE_WIDTH; x++) {
			dest[x] = (src1[x] >> 4) + NP2PAL_TEXT;
		}
		dest += SURFACE_WIDTH;
		src1 += SURFACE_WIDTH;
		src2 += SURFACE_WIDTH;
	}
}

static void screenmix3(PALNUM *dest, const UINT8 *src1, const UINT8 *src2) {

	PALNUM	c;
	int		x, y;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		// dest == src1, dest == src2 の時があるので…
		for (x=0; x<SURFACE_WIDTH; x++) {
			c = (src1[x + SURFACE_WIDTH]) >> 4;
			if (!c) {
				c = src2[x] + NP2PAL_SKIP;
			}
			dest[x + SURFACE_WIDTH] = c;
			dest[x] = src1[x] + src2[x] + NP2PAL_GRPH;
		}
		dest += SURFACE_WIDTH * 2;
		src1 += SURFACE_WIDTH * 2;
		src2 += SURFACE_WIDTH * 2;
	}
}

#if defined(SUPPORT_PC9821)
static void screenmix4(PALNUM *dest, const UINT8 *src1, const UINT8 *src2) {

	int		i;

	for (i=0; i<(SURFACE_WIDTH * SURFACE_HEIGHT); i++) {
		if (src1[i]) {
			dest[i] = (src1[i] >> 4) + NP2PAL_TEXTEX;
		}
		else {
			dest[i] = src2[i] + NP2PAL_GRPHEX;
		}
	}
}
#endif


SCRNBMP scrnbmp(void) {

	BMPDATA	bd;
	UINT	scrnsize;
	UINT	allocsize;
	PALNUM	*scrn;
	UINT8	*p;
	UINT8	*q;
	PALNUM	*s;
	UINT	pals;
	BMPPAL	pal[NP2PAL_TOTAL];
	PALNUM	remap[NP2PAL_TOTAL];
	UINT8	remapflg[NP2PAL_TOTAL];
	int		x;
	int		y;
	PALNUM	col;
	BMPPAL	curpal;
	UINT	pos;
	BMPINFO	bi;
	UINT	type;
	UINT	palsize;
	UINT	align;
	SCRNBMP	ret;
	BMPFILE	*bf;
	int		r;
	void	(*mix)(PALNUM *dest, const UINT8 *src1, const UINT8 *src2);

	bd.width = dsync.scrnxmax;
	bd.height = dsync.scrnymax;
	if ((bd.width <= 0) || (bd.height <= 0)) {
		goto sb_err1;
	}
	scrnsize = SURFACE_WIDTH * SURFACE_HEIGHT;
	allocsize = scrnsize * sizeof(PALNUM);
	scrn = (PALNUM *)_MALLOC(allocsize, "screen data");
	if (scrn == NULL) {
		goto sb_err1;
	}
	ZeroMemory(scrn, allocsize);

#if defined(SUPPORT_PC9821)
	if (gdc.analog & 2) {
		mix = screenmix4;
	}
	else
#endif
	if (!(gdc.mode1 & 0x10)) {
		mix = screenmix;
	}
	else if (!np2cfg.skipline) {
		mix = screenmix2;
	}
	else {
		mix = screenmix3;
	}
	q = p = ((UINT8 *)scrn) + (scrnsize * (sizeof(PALNUM) - 1));
	if (gdcs.textdisp & 0x80) {
		p = np2_tram;
	}
	if (gdcs.grphdisp & 0x80) {
#if defined(SUPPORT_PC9821)
		if ((gdc.analog & 6) == 6) {
			q = np2_vram[0];
		}
		else
#endif
		q = np2_vram[gdcs.disp];
	}
	(*mix)(scrn, p, q);

	// パレット最適化
	s = scrn;
	pals = 0;
	ZeroMemory(pal, sizeof(pal));
	ZeroMemory(remap, sizeof(remap));
	ZeroMemory(remapflg, sizeof(remapflg));
	for (y=0; y<bd.height; y++) {
		for (x=0; x<bd.width; x++) {
			col = s[x];
			if (!remapflg[col]) {
				remapflg[col] = 1;
				curpal.rgb[0] = np2_pal32[col].p.b;
				curpal.rgb[1] = np2_pal32[col].p.g;
				curpal.rgb[2] = np2_pal32[col].p.r;
				for (pos=0; pos<pals; pos++) {
					if (pal[pos].d == curpal.d) {
						break;
					}
				}
				if (pos >= pals) {
					pal[pos].d = curpal.d;
					pals++;
				}
				remap[col] = (PALNUM)pos;
			}
			s[x] = remap[col];
		}
		s += SURFACE_WIDTH;
	}

	if (pals <= 2) {
		type = SCRNBMP_1BIT;
		bd.bpp = 1;
		palsize = 4 << 1;
	}
	else if (pals <= 16) {
		type = SCRNBMP_4BIT;
		bd.bpp = 4;
		palsize = 4 << 4;
	}
	else if (pals <= 256) {
		type = SCRNBMP_8BIT;
		bd.bpp = 8;
		palsize = 4 << 8;
	}
	else {
		type = SCRNBMP_24BIT;
		bd.bpp = 24;
		palsize = 0;
	}
	allocsize = sizeof(BMPFILE) + sizeof(BMPINFO) + palsize;
	bmpdata_setinfo(&bi, &bd);
	allocsize += bmpdata_getdatasize(&bi);
	align = bmpdata_getalign(&bi);

	ret = (SCRNBMP)_MALLOC(sizeof(_SCRNBMP) + allocsize, "scrnbmp");
	if (ret == NULL) {
		goto sb_err2;
	}

	bf = (BMPFILE *)(ret + 1);
	ZeroMemory(bf, allocsize);
	bf->bfType[0] = 'B';
	bf->bfType[1] = 'M';
	pos = sizeof(BMPFILE) + sizeof(BMPINFO) + palsize;
	STOREINTELDWORD(bf->bfOffBits, pos);
	q = (UINT8 *)(bf + 1);
	STOREINTELDWORD(bi.biClrImportant, pals);
	CopyMemory(q, &bi, sizeof(bi));
	q += sizeof(bi);
	if (palsize) {
		CopyMemory(q, pal, pals * 4);
		q += palsize;
	}
	s = scrn + (SURFACE_WIDTH * bd.height);
	do {
		s -= SURFACE_WIDTH;
		switch(type) {
			case SCRNBMP_1BIT:
				for (x=0; x<bd.width; x++) {
					if (s[x]) {
						q[x >> 3] |= 0x80 >> (x & 7);
					}
				}
				break;

			case SCRNBMP_4BIT:
				r = bd.width / 2;
				for (x=0; x<r; x++) {
					q[x] = (s[x*2+0] << 4) + s[x*2+1];
				}
				if (bd.width & 1) {
					q[x] = s[x*2+0] << 4;
				}
				break;

			case SCRNBMP_8BIT:
				for (x=0; x<bd.width; x++) {
					q[x] = (UINT8)s[x];
				}
				break;

			case SCRNBMP_24BIT:
				for (x=0; x<bd.width; x++) {
					curpal.d = pal[s[x]].d;
					q[x*3+0] = curpal.rgb[0];
					q[x*3+1] = curpal.rgb[1];
					q[x*3+2] = curpal.rgb[2];
				}
				break;
		}
		q += align;
	} while(scrn < s);
	_MFREE(scrn);
	ret->type = type;
	ret->ptr = (UINT8 *)(ret + 1);
	ret->size = allocsize;
	return(ret);

sb_err2:
	_MFREE(scrn);

sb_err1:
	return(NULL);
}

