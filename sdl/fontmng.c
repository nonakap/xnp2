#include	"compiler.h"
#include	<SDL_ttf.h>
#include	"fontmng.h"
#include	"codecnv.h"


#if !defined(RESOURCE_US)		// use TTF

#define	FONTMNG_CACHE		64


#ifndef	FONTNAME_DEFAULT
#define	FONTNAME_DEFAULT	"./default.ttf"
#endif

static	char	fontname[MAX_PATH] = FONTNAME_DEFAULT;

static	const SDL_Color white = {0xff, 0xff, 0xff, 0};


#if defined(FONTMNG_CACHE)
typedef struct {
	UINT16		str;
	UINT16		next;
} FNTCTBL;
#endif

typedef struct {
	int			fontsize;
	UINT		fonttype;
	int			ptsize;
	int			fontalign;
	TTF_Font	*ttf_font;
#if defined(FONTMNG_CACHE)
	UINT		caches;
	UINT		cachehead;
	FNTCTBL		cache[FONTMNG_CACHE];
#endif
} _FNTMNG, *FNTMNG;


BOOL fontmng_init(void) {

	if (TTF_Init() < 0) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n", SDL_GetError());
		return(FAILURE);
	}
#ifndef WIN32
	atexit(TTF_Quit);
#endif
	return(SUCCESS);
}

void fontmng_setdeffontname(const char *name) {

	milstr_ncpy(fontname, name, sizeof(fontname));
}

void *fontmng_create(int size, UINT type, const char *fontface) {

	int			ptsize;
	int			fontalign;
	int			fontwork;
	int			allocsize;
	FNTMNG		ret;

	if (size < 0) {
		size = -size;
	}
	if (size < 6) {
		size = 6;
	}
	else if (size > 128) {
		size = 128;
	}

	if (size < 10) {
		type |= FDAT_ALIAS;
	}
	else if (size < 16) {
		type &= ~FDAT_BOLD;
	}

	ptsize = size;
	if (type & FDAT_ALIAS) {
		ptsize *= 2;
	}

	fontalign = sizeof(_FNTDAT) + (size * size);
	fontalign = (fontalign + 3) & (~3);
#if defined(FONTMNG_CACHE)
	fontwork = fontalign * FONTMNG_CACHE;
#else
	fontwork = fontalign;
#endif

	allocsize = sizeof(_FNTMNG) + fontwork;
	ret = (FNTMNG)_MALLOC(allocsize, "font mng");
	if (ret == NULL) {
		goto fmc_err1;
	}
	ZeroMemory(ret, allocsize);
	ret->fontsize = size;
	ret->fonttype = type;
	ret->ptsize = ptsize;
	ret->fontalign = fontalign;
	ret->ttf_font = TTF_OpenFont(fontname, ptsize);
	if (ret->ttf_font == NULL) {
		fprintf(stderr, "Couldn't load %d points font from %s: %s\n",
										ptsize, fontname, SDL_GetError());
		goto fmc_err2;
	}
	return(ret);

fmc_err2:
	_MFREE(ret);

fmc_err1:
	(void)fontface;
	return(NULL);
}

void fontmng_destroy(void *hdl) {

	FNTMNG	fnt;

	if (hdl) {
		fnt = (FNTMNG)hdl;
		TTF_CloseFont(fnt->ttf_font);
		_MFREE(hdl);
	}
}

#if defined(FONTMNG_CACHE)
static BOOL fdatgetcache(FNTMNG fhdl, const char *string, FNTDAT *pfdat) {

	BOOL	r;
	UINT	str;
	FNTCTBL	*fct;
	UINT	pos;
	UINT	prev;
	UINT	cnt;

	r = FALSE;
	str = string[0] & 0xff;
	str |= (string[1] & 0xff) << 8;
	fct = fhdl->cache;
	cnt = fhdl->caches;
	pos = fhdl->cachehead;
	prev = FONTMNG_CACHE;
	while(cnt--) {
		if (fct[pos].str != str) {
			prev = pos;
			pos = fct[pos].next;
			continue;
		}
		if (prev < FONTMNG_CACHE) {
			fct[prev].next = fct[pos].next;
			fct[pos].next = (UINT16)fhdl->cachehead;
			fhdl->cachehead = pos;
		}
		r = TRUE;
		break;
	}
	if (r == FALSE) {
		if (fhdl->caches < FONTMNG_CACHE) {
			pos = fhdl->caches;
			fhdl->caches++;
		}
		else {
			pos = prev;
		}
		fct[pos].str = (UINT16)str;
		fct[pos].next = (UINT16)fhdl->cachehead;
		fhdl->cachehead = pos;
	}
	if (pfdat) {
		*pfdat = (FNTDAT)(((UINT8 *)(fhdl + 1)) + (pos * fhdl->fontalign));
	}
	return(r);
}
#endif

static void setfdathead(FNTMNG fhdl, FNTDAT fdat, int length,
														SDL_Surface *text) {

	int		width;
	int		pitch;
	int		height;

	if ((fhdl->fonttype & FDAT_PROPORTIONAL) && (text)) {
		width = min(text->w, fhdl->ptsize);
		pitch = width;
		height = min(text->h, fhdl->ptsize);
		if (fhdl->fonttype & FDAT_ALIAS) {
			width = (width + 1) >> 1;
			pitch = width >> 1;
			height = (height + 1) >> 1;
		}
		fdat->width = width;
		fdat->pitch = pitch;
		fdat->height = height;
	}
	else {
		if (length < 2) {
			fdat->pitch = fhdl->fontsize >> 1;
		}
		else {
			fdat->pitch = fhdl->fontsize;
		}
		fdat->width = fhdl->fontsize;
		fdat->height = fhdl->fontsize;
	}
}

static void getlength1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	UINT16		utext[2];
	SDL_Surface	*text;

	if (fhdl->fonttype & FDAT_PROPORTIONAL) {
		codecnv_euctoucs2(utext, NELEMENTS(utext), string, length);
		text = TTF_RenderUNICODE_Solid(fhdl->ttf_font, utext, white);
		setfdathead(fhdl, fdat, length, text);
		if (text) {
			SDL_FreeSurface(text);
		}
	}
	else {
		setfdathead(fhdl, fdat, length, NULL);
	}
}

static UINT8 getpixeldepth(SDL_Surface *s, int x, int y) {

	int		bpp;
const UINT8	*ptr;

	if ((x >= 0) && (x < s->w) && (y >= 0) && (y < s->h)) {
		bpp = s->format->BytesPerPixel;
		ptr = (UINT8 *)s->pixels + (y * s->pitch) + (x * bpp);
		switch(bpp) {
			case 1:
				return((ptr[0] != 0)?FDAT_DEPTH:0);

			case 3:
			case 4:
				return(ptr[0] * FDAT_DEPTH / 255);
		}
	}
	return(0);
}

static void getfont1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	UINT16		utext[2];
	SDL_Surface	*text;
	UINT8		*dst;
	int			x;
	int			y;
	int			depth;

	codecnv_euctoucs2(utext, NELEMENTS(utext), string, length);
	text = TTF_RenderUNICODE_Solid(fhdl->ttf_font, utext, white);
	setfdathead(fhdl, fdat, length, text);
	dst = (UINT8 *)(fdat + 1);
	if (text) {
		if (fhdl->fonttype & FDAT_ALIAS) {
			for (y=0; y<fdat->height; y++) {
				for (x=0; x<fdat->width; x++) {
					depth = getpixeldepth(text, x*2+0, y*2+0);
					depth += getpixeldepth(text, x*2+1, y*2+0);
					depth += getpixeldepth(text, x*2+0, y*2+1);
					depth += getpixeldepth(text, x*2+1, y*2+1);
					*dst++ = (UINT8)((depth + 2) / 4);
				}
			}
		}
		else {
			for (y=0; y<fdat->height; y++) {
				for (x=0; x<fdat->width; x++) {
					*dst++ = getpixeldepth(text, x, y);
				}
			}
		}
		SDL_FreeSurface(text);
	}
	else {
		ZeroMemory(dst, fdat->width * fdat->height);
	}
}

BRESULT fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	int		width;
	int		leng;
	_FNTDAT	fdat;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}

	width = 0;
	while(1) {
		leng = milstr_charsize(string);
		if (!leng) {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, string, leng);
		string += leng;
		width += fdat.pitch;
	}

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgs_exit:
	return(FAILURE);
}

BRESULT fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	int		width;
	int		posx;
	int		leng;
	_FNTDAT	fdat;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}

	width = 0;
	posx = 0;
	while(1) {
		leng = milstr_charsize(string);
		if (!leng) {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, string, leng);
		string += leng;
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	}
	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
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

#if defined(FONTMNG_CACHE)
	if (fdatgetcache(fhdl, string, &fdat)) {
		return(fdat);
	}
#else
	fdat = (FNTDAT)(fhdl + 1);
#endif

	leng = milstr_charsize(string);
	getfont1(fhdl, fdat, string, leng);
	return(fdat);

fmg_err:
	return(NULL);
}

#else

#if defined(SIZE_QVGA)
#include	"ank10.res"
#else
#include	"ank12.res"
#endif

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

BRESULT fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	FNTMNG	fhdl;
	int		width;
	_FNTDAT	fdat;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}
	fhdl = (FNTMNG)hdl;

	width = 0;
	while(1) {
		leng = milstr_charsize(string);
		if (!leng) {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, string, leng);
		string += leng;
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

BRESULT fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	FNTMNG	fhdl;
	_FNTDAT	fdat;
	int		width;
	int		posx;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}
	fhdl = (FNTMNG)hdl;

	width = 0;
	posx = 0;
	while(1) {
		leng = milstr_charsize(string);
		if (!leng) {
			break;
		}
		getlength1(fhdl, &fdat, string, leng);
		string += leng;
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

	if ((hdl == NULL) || (string == NULL)) {
		goto fmg_err;
	}
	fhdl = (FNTMNG)hdl;
	fdat = (FNTDAT)(fhdl + 1);

	getfont1(fhdl, fdat, string, milstr_charsize(string));
	return(fdat);

fmg_err:
	return(NULL);
}
#endif

