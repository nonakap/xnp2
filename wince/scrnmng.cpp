#include	"compiler.h"
#include	<gx.h>
#include	"bmpdata.h"
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"nekop2.res"
#include	"cmndraw.h"
#include	"softkbd.h"


#if defined(GX_DLL)
#pragma comment(lib, "gx.lib")
#endif


typedef struct {
	BOOL	rotate;
	VRAMHDL	vram;
	UINT	posx;
	UINT	posy;
	int		width;
	int		height;
	long	start;
	long	xalign;
	long	yalign;
#if defined(SUPPORT_SOFTKBD)
	int		kbdposy;
	int		kbdheight;
	int		allflash;
#endif
} SCRNMNG;

typedef struct {
	int		width;
	int		height;
} SCRNSTAT;

	GXDisplayProperties		gx_dp;

static	SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;

static	UINT8		gx_disable = 1;
extern	GXKeyList	gx_keylist;

static const TCHAR	errmsg[] = _T("Error");


typedef struct {
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	int		srcpos;
	int		dstpos;
} DRAWRECT;


// ---- GX DLL‚Æ‚©

#if !defined(WIN32_PLATFORM_PSPC) || !defined(SIZE_VGA)

#define	GXGETDISPLAYPROPERTIES()	GXGetDisplayProperties()
#define	GXBEGINDRAW()				GXBeginDraw()
#define	GXENDDRAW()					GXEndDraw()

#else // PocketPC2003SE VGA

typedef struct {
	WORD	wFormat;
	WORD	wBPP;
	VOID	*pFramePointer;
	int		cxStride;
	int		cyStride;
	int		cxPixels;
	int		cyPixels;
} RAWFRAMEBUFFERINFO;

#define	GETRAWFRAMEBUFFER	0x00020001

enum {
	RFBIFMT_565		= 1,
	RFBIFMT_555		= 2,
	RFBIFMT_OTHER	= 3
};

static	RAWFRAMEBUFFERINFO	rfbi;

static GXDisplayProperties GXGETDISPLAYPROPERTIES(void) {

	HDC					hdc;
	GXDisplayProperties	ret;

	hdc = GetDC(NULL);
	ExtEscape(hdc, GETRAWFRAMEBUFFER, 0, NULL, sizeof(rfbi), (char *)&rfbi);
	ReleaseDC(NULL, hdc);
	ret.cxWidth = rfbi.cxPixels;
	ret.cyHeight = rfbi.cyPixels;
	ret.cbxPitch = rfbi.cxStride;
	ret.cbyPitch = rfbi.cyStride;
	ret.cBPP = rfbi.wBPP;
	ret.ffFormat = 0;
	switch(rfbi.wFormat) {
		case RFBIFMT_565:
			ret.ffFormat |= kfDirect565;
			break;

		case RFBIFMT_555:
			ret.ffFormat |= kfDirect555;
			break;
	}
	return(ret);
}
#define	GXBEGINDRAW()		(rfbi.pFramePointer)
#define	GXENDDRAW()
#endif


// ----

static BOOL calcdrawrect(DRAWRECT *dr, VRAMHDL s, const RECT_T *rt) {

	int		pos;

	dr->xalign = scrnmng.xalign;
	dr->yalign = scrnmng.yalign;
	dr->srcpos = 0;
	dr->dstpos = scrnmng.start;
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
	if ((dr->width <= 0) || (dr->height <= 0)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

static void palcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	if (bpp == 16) {
		for (i=0; i<pals; i++) {
			dst[i].pal16 = ((src[i].p.r & 0xf8) << 8) |
							((src[i].p.g & 0xfc) << 3) |
							(src[i].p.b >> 3);
		}
	}
}

static void bmp16draw(void *bmp, UINT8 *dst, int width, int height,
												int xalign, int yalign) {

	CMNVRAM	vram;

	vram.ptr = dst;
	vram.width = width;
	vram.height = height;
	vram.xalign = xalign;
	vram.yalign = yalign;
	vram.bpp = 16;
	cmndraw_bmp16(&vram, bmp, palcnv, CMNBMP_CENTER | CMNBMP_MIDDLE);
}


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
}

BOOL scrnmng_create(HWND hWnd, LONG width, LONG height) {

	TCHAR	msg[32];
#if defined(SUPPORT_SOFTKBD)
	int		kbdheight;
#endif

	if ((width <= 0) || (height <= 0)) {
		return(FAILURE);
	}
	if (GXOpenDisplay(hWnd, GX_FULLSCREEN) == 0) {
		MessageBox(hWnd, _T("Couldn't GAPI Object"),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}
	gx_dp = GXGETDISPLAYPROPERTIES();
	if (gx_dp.cBPP != 16) {
		MessageBox(hWnd, _T("Only 16bit color support..."),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}
	if (!(gx_dp.ffFormat & kfDirect565)) {
		MessageBox(hWnd, _T("Only 16bit(565) support..."),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}

	if ((gx_dp.cxWidth >= (DWORD)width) &&
		(gx_dp.cyHeight >= (DWORD)height)) {
		scrnmng.rotate = FALSE;
	}
	else if ((gx_dp.cxWidth >= (DWORD)height) &&
			(gx_dp.cyHeight >= (DWORD)width)) {
		scrnmng.rotate = TRUE;
	}
	else {
		wsprintf(msg, _T("Required %dx%d..."), width, height);
		MessageBox(hWnd, msg, errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}

	if (!scrnmng.rotate) {
		scrnmng.posx = ((gx_dp.cxWidth - width) / 2);
		scrnmng.posy = ((gx_dp.cyHeight - height) / 2);
		scrnmng.start = scrnmng.posx * gx_dp.cbxPitch +
						scrnmng.posy * gx_dp.cbyPitch;
		scrnmng.width = width;
		scrnmng.height = height;
		scrnmng.xalign = gx_dp.cbxPitch;
		scrnmng.yalign = gx_dp.cbyPitch;
	}
	else {
		scrnmng.posx = ((gx_dp.cxWidth - height) / 2);
		scrnmng.posy = ((gx_dp.cyHeight - width) / 2);
		scrnmng.start = ((scrnmng.posy + width - 1) * gx_dp.cbyPitch) +
						(scrnmng.posx * gx_dp.cbxPitch);
		scrnmng.width = width;
		scrnmng.height = height;
		scrnmng.xalign = -gx_dp.cbyPitch;
		scrnmng.yalign = gx_dp.cbxPitch;
	}
	gx_disable = 0;
	scrnmng_clear(TRUE);
#if defined(SUPPORT_SOFTKBD)
	softkbd_initialize();
	if (softkbd_getsize(NULL, &kbdheight) != SUCCESS) {
		kbdheight = 0;
	}
	kbdheight = min(kbdheight, height);
	scrnmng.kbdposy = height - kbdheight;
	scrnmng.kbdheight = kbdheight;
#endif
	return(SUCCESS);
}

void scrnmng_destroy(void) {

	gx_disable = 1;
	GXCloseDisplay();
#if defined(SUPPORT_SOFTKBD)
	softkbd_deinitialize();
#endif
}

#if defined(SUPPORT_SOFTKBD)
void scrnmng_allflash(void) {

	scrnmng.allflash = 1;
}
#endif

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB16	ret;

	ret = (pal32.p.r & 0xf8) << 8;
#if defined(SIZE_QVGA) && !defined(SIZE_VGATEST)
	ret += (pal32.p.g & 0xfc) << (3 + 16);
#else
	ret += (pal32.p.g & 0xfc) << 3;
#endif
	ret += pal32.p.b >> 3;
	return(ret);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
}

const SCRNSURF *scrnmng_surflock(void) {

	if (gx_disable) {
		return(NULL);
	}
	if (scrnmng.vram == NULL) {
		scrnsurf.ptr = (UINT8 *)GXBEGINDRAW() + scrnmng.start;
		scrnsurf.xalign = scrnmng.xalign;
		scrnsurf.yalign = scrnmng.yalign;
	}
	else {
		scrnsurf.ptr = scrnmng.vram->ptr;
		scrnsurf.xalign = 2;
		scrnsurf.yalign = scrnmng.vram->yalign;
	}
#if !defined(SIZE_VGATEST)
	scrnsurf.width = min(scrnstat.width, 640);
	scrnsurf.height = min(scrnstat.height, 400);
#else
	scrnsurf.width = min(scrnstat.width, 320);
	scrnsurf.height = min(scrnstat.height, 200);
#endif
	scrnsurf.bpp = 16;
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

static void draw_onmenu(void) {

	RECT_T		rt;
	DRAWRECT	dr;
const UINT8		*p;
	UINT8		*q;
const UINT8		*a;
	int			salign;
	int			dalign;
	int			r;

	rt.left = 0;
	rt.top = 0;
#if defined(SUPPORT_SOFTKBD)
	if (!scrnmng.allflash) {
#endif
		rt.right = min(scrnstat.width, 640);
		rt.bottom = min(scrnstat.height, 400);
#if defined(SIZE_QVGA)
		rt.right >>= 1;
		rt.bottom >>= 1;
#endif
#if defined(SUPPORT_SOFTKBD)
	}
	else {
		scrnmng.allflash = 0;
		rt.right = scrnmng.width;
		rt.bottom = scrnmng.height;
	}
#endif
	if (calcdrawrect(&dr, menuvram, &rt) != SUCCESS) {
		return;
	}
	p = scrnmng.vram->ptr + (dr.srcpos * 2);
	q = (UINT8 *)GXBEGINDRAW() + dr.dstpos;
	a = menuvram->alpha + dr.srcpos;
	salign = menuvram->width - dr.width;
	dalign = dr.yalign - (dr.width * dr.xalign);
	do {
		r = dr.width;
		do {
			if (a[0] == 0) {
				*(UINT16 *)q = *(UINT16 *)p;
			}
			p += 2;
			q += dr.xalign;
			a += 1;
		} while(--r);
		p += salign * 2;
		q += dalign;
		a += salign;
	} while(--dr.height);
	GXENDDRAW();
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

#if defined(SUPPORT_SOFTKBD)
	CMNVRAM	vram;
#endif

	if (surf) {
		if (scrnmng.vram == NULL) {
#if defined(SUPPORT_SOFTKBD)
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				vram.ptr = surf->ptr + (scrnmng.kbdposy * surf->yalign);
				vram.width = scrnmng.width;
				vram.height = scrnmng.kbdheight;
				vram.xalign = surf->xalign;
				vram.yalign = surf->yalign;
				vram.bpp = 16;
				softkbd_paint(&vram, palcnv, TRUE);
			}
#endif
			GXENDDRAW();
		}
		else {
			if (menuvram) {
				draw_onmenu();
			}
		}
	}
}


// ---- for wince

void scrnmng_enable(BOOL enable) {

	if (enable) {
		gx_disable &= ~2;
	}
	else {
		gx_disable |= 2;
	}
}

BOOL scrnmng_mousepos(LPARAM *lp) {

	UINT	x;
	UINT	y;

	if (!scrnmng.rotate) {
		x = LOWORD(*lp) - scrnmng.posx;
		y = HIWORD(*lp) - scrnmng.posy;
	}
	else {
		x = scrnmng.width - HIWORD(*lp) - 1 - scrnmng.posy;
		y = LOWORD(*lp) - scrnmng.posx;
	}
	if ((x >= (UINT)scrnmng.width) || (y >= (UINT)scrnmng.height)) {
		return(FAILURE);
	}
	*lp = (x & 0xffff) | ((y << 16) & 0xffff0000);
	return(SUCCESS);
}

void scrnmng_clear(BOOL logo) {

	void	*bmp;
	UINT8	*p;
	UINT8	*q;
	int		y;
	int		x;
	long	yalign;

	if (gx_disable) {
		return;
	}
	bmp = NULL;
	if (logo) {
		bmp = (void *)bmpdata_solvedata(nekop2_bmp);
	}
	p = (UINT8 *)GXBEGINDRAW();
	q = p;
	y = gx_dp.cyHeight;
	yalign = gx_dp.cbyPitch - (gx_dp.cbxPitch * gx_dp.cxWidth);
	do {
		x = gx_dp.cxWidth;
		do {
			*(UINT16 *)q = 0;
			q += gx_dp.cbxPitch;
		} while(--x);
		q += yalign;
	} while(--y);
	bmp16draw(bmp, p + scrnmng.start, scrnmng.width, scrnmng.height,
												scrnmng.xalign, scrnmng.yalign);	GXENDDRAW();
	if (bmp) {
		_MFREE(bmp);
	}
}

void scrnmng_keybinds(void) {

	short	tmp;

	if (scrnmng.rotate) {
		tmp = gx_keylist.vkLeft;
		gx_keylist.vkLeft = gx_keylist.vkDown;
		gx_keylist.vkDown = gx_keylist.vkRight;
		gx_keylist.vkRight = gx_keylist.vkUp;
		gx_keylist.vkUp = tmp;
	}
}


// ---- for menubase

BOOL scrnmng_entermenu(SCRNMENU *smenu) {

	VRAMHDL	vram;
#if defined(SUPPORT_SOFTKBD)
	CMNVRAM	kbdvram;
#endif

	if (smenu == NULL) {
		goto smem_err;
	}
	vram_destroy(scrnmng.vram);
	vram = vram_create(scrnmng.width, scrnmng.height, FALSE, 16);
	scrnmng.vram = vram;
	if (vram == NULL) {
		goto smem_err;
	}
	scrndraw_redraw();
#if defined(SUPPORT_SOFTKBD)
	kbdvram.ptr = vram->ptr + (vram->yalign * scrnmng.kbdposy);
	kbdvram.width = vram->width;
	kbdvram.height = scrnmng.kbdheight;
	kbdvram.xalign = vram->xalign;
	kbdvram.yalign = vram->yalign;
	kbdvram.bpp = vram->bpp;
	softkbd_paint(&kbdvram, palcnv, TRUE);
#endif
	smenu->width = scrnmng.width;
	smenu->height = scrnmng.height;
	smenu->bpp = 16;
	return(SUCCESS);

smem_err:
	return(FAILURE);
}

void scrnmng_leavemenu(void) {

	VRAM_RELEASE(scrnmng.vram);
}

void scrnmng_menudraw(const RECT_T *rct) {

	DRAWRECT	dr;
const UINT8		*p;
const UINT8		*q;
	UINT8		*r;
	UINT8		*a;
	int			salign;
	int			dalign;
	int			x;

	if ((gx_disable) || (menuvram == NULL)) {
		return;
	}
	if (calcdrawrect(&dr, menuvram, rct) != SUCCESS) {
		return;
	}
	p = scrnmng.vram->ptr + (dr.srcpos * 2);
	q = menuvram->ptr + (dr.srcpos * 2);
	r = (UINT8 *)GXBEGINDRAW() + dr.dstpos;
	a = menuvram->alpha + dr.srcpos;
	salign = menuvram->width;
	dalign = dr.yalign - (dr.width * dr.xalign);
	do {
		x = 0;
		do {
			if (a[x]) {
				if (a[x] & 2) {
					*(UINT16 *)r = *(UINT16 *)(q + x * 2);
				}
				else {
					a[x] = 0;
					*(UINT16 *)r = *(UINT16 *)(p + x * 2);
				}
			}
			r += dr.xalign;
		} while(++x < dr.width);
		p += salign * 2;
		q += salign * 2;
		r += dalign;
		a += salign;
	} while(--dr.height);
	GXENDDRAW();
}


// ----

#if defined(SUPPORT_SOFTKBD)
BOOL scrnmng_kbdpos(LPARAM *lp) {

	UINT	x;
	UINT	y;

	x = LOWORD(*lp);
	y = HIWORD(*lp) - scrnmng.kbdposy;
	*lp = (x & 0xffff) | ((y << 16) & 0xffff0000);
	return(SUCCESS);
}

BOOL scrnmng_ismenu(LPARAM lp) {

	if ((LOWORD(lp) < 32) && (HIWORD(lp) >= (scrnmng.height - 32))) {
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}
#endif

