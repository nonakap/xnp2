#include	"compiler.h"
#include	"fontmng.h"


typedef struct {
	int				fontsize;
	UINT			fonttype;
	int				fontwidth;
	int				fontheight;

	GWorldPtr		gw;
	PixMapHandle	pm;
	Rect			rect;
	RGBColor		bg;
	RGBColor		fg;
	short			fontid;
} _FNTMNG, *FNTMNG;


extern	WindowPtr		hWndMain;

static const BYTE jis2dtbl[94*2] = {
		0x85,0x40,0x85,0x41,0x85,0x42,0x85,0x43,0x85,0x44,0x85,0x45,0x85,0x46,
		0x85,0x47,0x85,0x48,0x85,0x49,0x85,0x4a,0x85,0x4b,0x85,0x4c,0x85,0x4d,
		0x85,0x4e,0x85,0x4f,0x85,0x50,0x85,0x51,0x85,0x52,0x85,0x53,0x85,0x9f,
		0x85,0xa0,0x85,0xa1,0x85,0xa2,0x85,0xa3,0x85,0xa4,0x85,0xa5,0x85,0xa6,
		0x85,0xa7,0x85,0xa8,0x81,0x40,0x87,0x9f,0x87,0xa2,0x87,0xa0,0x87,0xa1,
		0x87,0xa9,0x87,0xab,0x87,0xa7,0x87,0xa8,0x87,0xac,0x87,0xaf,0x87,0xb0,
		0x87,0xb3,0x87,0xb2,0x87,0xb5,0x87,0xad,0x87,0xb4,0x86,0x40,0x86,0x42,
		0x86,0x48,0x86,0x4a,0x86,0x4c,0x86,0x4d,0x86,0x46,0x81,0x40,0x81,0x40,
		0x81,0x40,0x81,0x40,0x81,0x40,0x81,0x40,0x81,0x40,0x81,0x40,0x87,0xe8,
		0x88,0x54,0x88,0x55,0x86,0x9b,0x86,0x9c,0x86,0x9d,0x87,0x93,0x87,0x94,
		0x87,0x95,0x87,0x96,0x87,0x97,0x87,0x4d,0x87,0x50,0x87,0x4b,0x87,0xe5,
		0x87,0xe6,0x87,0xe7,0x81,0xe0,0x81,0xdf,0x81,0xe7,0x88,0x40,0x83,0xb0,
		0x81,0xe3,0x81,0xdb,0x81,0xda,0x88,0x41,0x88,0x42,0x81,0xe6,0x81,0xbf,
		0x81,0xbe,0x81,0x40,0x81,0x40};

static int getsjis1(char *dst, const char *src) {

	int		pos;

	if (src) {
		if ((((src[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			if (src[1]) {
				if (((BYTE)src[0] == 0x87) &&
					((BYTE)src[1] >= 0x40) && ((BYTE)src[1] < 0x9f)) {
					pos = (BYTE)src[1] - 0x40;
					if (pos >= 0x40) {
						pos--;
					}
					dst[0] = jis2dtbl[pos*2+0];
					dst[1] = jis2dtbl[pos*2+1];
				}
				else {
					dst[0] = src[0];
					dst[1] = src[1];
				}
				dst[2] = '\0';
				return(2);
			}
		}
		else if (src[0]) {
			if (dst) {
				dst[0] = src[0];
				dst[1] = '\0';
			}
			return(1);
		}
	}
	return(0);
}


void *fontmng_create(int size, UINT type, const TCHAR *fontface) {

	void		*ret;
	_FNTMNG		fnt;
	int			fontalign;
	int			allocsize;

	ZeroMemory(&fnt, sizeof(fnt));

	if (size < 0) {
		size *= -1;
	}
	if (size < 6) {
		size = 6;
	}
	else if (size > 128) {
		size = 128;
	}
	fnt.fontsize = size;
	fnt.fonttype = type;
	fnt.fontwidth = size + 1;
	fnt.fontheight = size + 1;

	fontalign = sizeof(_FNTDAT) + (fnt.fontwidth * fnt.fontheight);

	allocsize = sizeof(fnt);
	allocsize += fontalign;

	SetRect(&fnt.rect, 0, 0, fnt.fontwidth, fnt.fontheight);
	if (NewGWorld(&fnt.gw, 32, &fnt.rect, NULL, NULL, useTempMem) != noErr) {
		return(NULL);
	}
	fnt.pm = GetGWorldPixMap(fnt.gw);
	GetFNum("\pOsaka|“™•", &fnt.fontid);
	fnt.fg.red = 0xffff;
	fnt.fg.green = 0xffff;
	fnt.fg.blue = 0xffff;
	ret = _MALLOC(allocsize, "font mng");
	if (ret) {
		CopyMemory(ret, &fnt, sizeof(fnt));
	}
	else {
		DisposeGWorld(fnt.gw);
	}
	(void)fontface;
	return(ret);
}


void fontmng_destroy(void *hdl) {

	FNTMNG	fhdl;

	fhdl = (FNTMNG)hdl;
	if (fhdl) {
		DisposeGWorld(fhdl->gw);
		free(fhdl);
	}
}


static void getlength1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	if (length < 2) {
		fdat->width = fhdl->fontwidth;
		fdat->pitch = (fhdl->fontsize + 1) >> 1;
	}
	else {
		fdat->width = fhdl->fontwidth;
		fdat->pitch = fhdl->fontsize;
	}
	fdat->height = fhdl->fontheight;
	(void)string;
}


BOOL fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		leng;

	width = 0;
	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}

	buf[2] = '\0';
	do {
		leng = getsjis1(buf, string);
		if (!leng) {
			break;
		}
		string += leng;
		getlength1((FNTMNG)hdl, &fdat, buf, leng);
		width += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgs_exit:
	return(FAILURE);
}


BOOL fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		posx;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}

	width = 0;
	posx = 0;
	do {
		leng = getsjis1(buf, string);
		if (!leng) {
			break;
		}
		string += leng;
		getlength1((FNTMNG)hdl, &fdat, buf, leng);
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgds_exit:
	return(FAILURE);
}


static void fontmng_getchar(FNTMNG fhdl, FNTDAT fdat, const char *string) {

	GDHandle	hgd;
	GWorldPtr	gwp;
	BYTE		*p, *q;
	int			yalign;
	RGBColor	bak;
	int			x, y;
	FontInfo	info;
	int			leng;
	char		buf[4];

	GetGWorld(&gwp, &hgd);
	LockPixels(fhdl->pm);
	SetGWorld(fhdl->gw, NULL);

	p = (BYTE *)GetPixBaseAddr(fhdl->pm);
	yalign = ((*fhdl->pm)->rowBytes) & 0x3fff;
	TextFont(fhdl->fontid);
	TextSize(fhdl->fontsize);
	GetFontInfo(&info);
	GetForeColor(&bak);
#if TARGET_API_MAC_CARBON
	// ‚â‚è•û‰ð‚ç‚È‚¢‚Ì‚Å Ž©‘O‚Åíœ‚µ‚Ü‚·^^;
	{
		BYTE	*q;
		int		r;
		q = p;
		r = fhdl->fontheight;
		while(r--) {
			ZeroMemory(q, fhdl->fontwidth * 4);
			q += ((*fhdl->pm)->rowBytes) & 0x3fff;
		}
	}
#else
	FillRect(&fhdl->rect, &qd.black);
#endif

	leng = getsjis1(buf, string);
	if (leng) {
		RGBForeColor(&fhdl->fg);
#if TARGET_API_MAC_CARBON
		MoveTo(0, info.ascent - 1);
#else
		MoveTo(0, fhdl->fontsize - 1);
#endif
		DrawText(buf, 0, leng);
		getlength1(fhdl, fdat, buf, leng);
	}

	y = fdat->width;
	q = (BYTE *)(fdat + 1);
	while(y--) {
		BYTE *fnt;
		fnt = p;
		x = fdat->height;
		do {
			*q++ = fnt[1];
			fnt += 4;
		} while(--x);
		p += yalign;
	}
	RGBForeColor(&bak);
	UnlockPixels(fhdl->pm);
	SetGWorld(gwp, hgd);
}


FNTDAT fontmng_get(void *hdl, const char *string) {

	FNTMNG	fhdl;
	FNTDAT	fdat;

	if ((hdl == NULL) || (string == NULL)) {
		goto ftmggt_err;
	}
	fhdl = (FNTMNG)hdl;
	fdat = (FNTDAT)(fhdl + 1);
	fontmng_getchar(fhdl, fdat, string);
	return(fdat);

ftmggt_err:
	return(NULL);
}

