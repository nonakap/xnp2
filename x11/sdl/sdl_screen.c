#include "compiler.h"

#include "np2.h"

#include "scrndraw.h"
#include "vramhdl.h"
#include "menubase.h"

#include "scrnmng.h"

#include "sdl/xnp2.h"

typedef struct {
	SCRNMNG		sm;

	BOOL		enable;

	int		width;
	int		height;

	SDL_Surface	*surface;
	VRAMHDL		vram;
} SDLSCRNMNG;

typedef struct {
	int		width;
	int		height;
} SCRNSTAT;

typedef struct {
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	int		srcpos;
	int		dstpos;
} DRAWRECT;

static SDLSCRNMNG scrnmng;
static SCRNSTAT scrnstat;
static SCRNSURF scrnsurf;

SCRNMNG *scrnmngp = &scrnmng.sm;


static BOOL
calcdrawrect(SDL_Surface *surface, DRAWRECT *dr, VRAMHDL s, const RECT_T *rt)
{
	int pos;

	dr->xalign = surface->format->BytesPerPixel;
	dr->yalign = surface->pitch;
	dr->srcpos = 0;
	dr->dstpos = 0;
	dr->width = min(scrnmng.width, s->width);
	dr->height = min(scrnmng.height, s->height);
	if (rt) {
		pos = max(rt->left, 0);
		dr->srcpos += pos;
		dr->dstpos += pos * dr->xalign;
		dr->width = min(rt->right, dr->width) - pos;

		pos = max(rt->top, 0);
		dr->srcpos += pos * s->width;
		dr->dstpos += pos * dr->yalign;
		dr->height = min(rt->bottom, dr->height) - pos;
	}
	if ((dr->width <= 0) || (dr->height <= 0))
		return(FAILURE);
	return(SUCCESS);
}

void
scrnmng_initialize(void)
{

	scrnstat.width = 640;
	scrnstat.height = 400;
}

BOOL
scrnmng_create(BYTE mode)
{
	char s[256];
	const SDL_VideoInfo *vinfo;
	SDL_Surface *surface;
	SDL_PixelFormat	*fmt;
	int width = FULLSCREEN_WIDTH;
	int height = FULLSCREEN_HEIGHT;
	BOOL r;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error: SDL_Init: %s\n", SDL_GetError());
		return(FAILURE);
	}
	SDL_WM_SetCaption(np2oscfg.titles, np2oscfg.titles);

	vinfo = SDL_GetVideoInfo();
	if (vinfo == NULL) {
		fprintf(stderr,"Error: SDL_GetVideoInfo: %s\n", SDL_GetError());
		return(FAILURE);
	}
	SDL_VideoDriverName(s, sizeof(s));

	surface = SDL_SetVideoMode(width, height, vinfo->vfmt->BitsPerPixel,
	    (mode & SCRNMODE_FULLSCREEN)
	      ?  (SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF|SDL_FULLSCREEN)
	      :  (SDL_SWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF));
	if (surface == NULL) {
		fprintf(stderr,"Error: SDL_SetVideoMode: %s\n", SDL_GetError());
		return(FAILURE);
	}

	r = FALSE;
	fmt = surface->format;
#if defined(SUPPORT_8BPP)
	if (fmt->BitsPerPixel == 8) {
		r = TRUE;
	}
#endif
#if defined(SUPPORT_16BPP)
	if ((fmt->BitsPerPixel == 16) && (fmt->Rmask == 0xf800) &&
		(fmt->Gmask == 0x07e0) && (fmt->Bmask == 0x001f)) {
		r = TRUE;
	}
#endif
#if defined(SUPPORT_24BPP)
	if (fmt->BitsPerPixel == 24) {
		r = TRUE;
	}
#endif
#if defined(SUPPORT_32BPP)
	if (fmt->BitsPerPixel == 32) {
		r = TRUE;
	}
#endif
#if defined(SCREEN_BPP)
	if (fmt->BitsPerPixel != SCREEN_BPP) {
		r = FALSE;
	}
#endif
	if (r) {
		scrnmng.enable = TRUE;
		scrnmng.width = width;
		scrnmng.height = height;
		scrnmng.sm.bpp = fmt->BitsPerPixel;
		return SUCCESS;
	}

	fprintf(stderr, "Error: Bad screen mode");
	return FAILURE;
}

void
scrnmng_destroy(void)
{

	scrnmng.enable = FALSE;
}

RGB16
scrnmng_makepal16(RGB32 pal32)
{
	RGB16 ret;

	ret = (pal32.p.r & 0xf8) << 8;
	ret += (pal32.p.g & 0xfc) << 3;
	ret += pal32.p.b >> 3;

	return ret;
}

void
scrnmng_setwidth(int posx, int width)
{

	UNUSED(posx);

	scrnstat.width = width;
}

void
scrnmng_setheight(int posy, int height)
{

	UNUSED(posy);

	scrnstat.height = height;
}

void
scrnmng_setextend(int extend)
{

	UNUSED(extend);

	/* nothing to do */
}

const SCRNSURF *
scrnmng_surflock(void)
{
	SDL_Surface *surface;

	if (scrnmng.vram == NULL) {
		surface = SDL_GetVideoSurface();
		if (surface == NULL) {
			return NULL;
		}
		SDL_LockSurface(surface);
		scrnmng.surface = surface;
		scrnsurf.ptr = (BYTE *)surface->pixels;
		scrnsurf.xalign = surface->format->BytesPerPixel;
		scrnsurf.yalign = surface->pitch;
		scrnsurf.bpp = surface->format->BitsPerPixel;
	} else {
		scrnsurf.ptr = scrnmng.vram->ptr;
		scrnsurf.xalign = scrnmng.vram->xalign;
		scrnsurf.yalign = scrnmng.vram->yalign;
		scrnsurf.bpp = scrnmng.vram->bpp;
	}
	scrnsurf.width = min(scrnstat.width, 640);
	scrnsurf.height = min(scrnstat.height, 400);
	scrnsurf.extend = 0;
	return &scrnsurf;
}

static void
draw_onmenu(void)
{
	DRAWRECT dr;
	RECT_T rt;
	SDL_Surface *surface;
	const BYTE *p;
	BYTE *q;
	const BYTE *a;
	int salign;
	int dalign;
	int x;

	rt.left = 0;
	rt.top = 0;
	rt.right = min(scrnstat.width, 640);
	rt.bottom = min(scrnstat.height, 400);

	surface = SDL_GetVideoSurface();
	if (surface == NULL) {
		return;
	}
	SDL_LockSurface(surface);
	if (calcdrawrect(surface, &dr, menuvram, &rt) == SUCCESS) {
		if (scrnmng.sm.bpp == 16) {
			p = scrnmng.vram->ptr + (dr.srcpos * 2);
			q = (BYTE *)surface->pixels + dr.dstpos;
			a = menuvram->alpha + dr.srcpos;
			salign = menuvram->width;
			dalign = dr.yalign - (dr.width * dr.xalign);
			do {
				x = 0;
				do {
					if (a[x] == 0) {
						*(UINT16 *)q = *(UINT16 *)(p + (x * 2));
					}
					q += dr.xalign;
				} while(++x < dr.width);
				p += salign * 2;
				q += dalign;
				a += salign;
			} while(--dr.height);
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Flip(surface);
}

void
scrnmng_surfunlock(const SCRNSURF *surf)
{
	SDL_Surface *surface;

	if (surf) {
		if (scrnmng.vram == NULL) {
			if (scrnmng.surface != NULL) {
				surface = scrnmng.surface;
				scrnmng.surface = NULL;
				SDL_UnlockSurface(surface);
				SDL_Flip(surface);
			}
		} else {
			if (menuvram) {
				draw_onmenu();
			}
		}
	}
}


/*
 * menu
 */
BOOL
scrnmng_entermenu(SCRNMENU *smenu)
{
	int bpp;

	if (smenu == NULL) {
		goto smem_err;
	}
	bpp = scrnmng.sm.bpp;
	if (bpp == 32) {
		bpp = 24;
	}
	vram_destroy(scrnmng.vram);
	scrnmng.vram = vram_create(scrnmng.width, scrnmng.height, FALSE, bpp);
	if (scrnmng.vram == NULL) {
		goto smem_err;
	}
	scrndraw_redraw();

	smenu->width = scrnmng.width;
	smenu->height = scrnmng.height;
	smenu->bpp = bpp;

	return SUCCESS;

smem_err:
	return FAILURE;
}

void
scrnmng_leavemenu(void)
{

	VRAM_RELEASE(scrnmng.vram);
}

void
scrnmng_menudraw(const RECT_T *rct)
{
	DRAWRECT dr;
	SDL_Surface *surface;
	const BYTE *p;
	const BYTE *q;
	BYTE *r;
	BYTE *a;
	int salign;
	int dalign;
	int x;

	if ((!scrnmng.enable) && (menuvram == NULL))
		return;

	surface = SDL_GetVideoSurface();
	if (surface == NULL)
		return;

	SDL_LockSurface(surface);
	if (calcdrawrect(surface, &dr, menuvram, rct) == SUCCESS) {
		if (scrnmng.sm.bpp == 16) {
			p = scrnmng.vram->ptr + (dr.srcpos * 2);
			q = menuvram->ptr + (dr.srcpos * 2);
			r = (BYTE *)surface->pixels + dr.dstpos;
			a = menuvram->alpha + dr.srcpos;
			salign = menuvram->width;
			dalign = dr.yalign - (dr.width * dr.xalign);
			do {
				x = 0;
				do {
					if (a[x]) {
						if (a[x] & 2) {
							*(UINT16 *)r = *(UINT16 *)(q + (x * 2));
						}
						else {
							a[x] = 0;
							*(UINT16 *)r = *(UINT16 *)(p + (x * 2));
						}
					}
					r += dr.xalign;
				} while (++x < dr.width);
				p += salign * 2;
				q += salign * 2;
				r += dalign;
				a += salign;
			} while (--dr.height);
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Flip(surface);
}
