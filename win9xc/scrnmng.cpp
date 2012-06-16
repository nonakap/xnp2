#include	"compiler.h"
#include	<ddraw.h>
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"np2.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"menu.h"


typedef struct {
	LPDIRECTDRAW		ddraw1;
	LPDIRECTDRAW2		ddraw2;
	LPDIRECTDRAWSURFACE	primsurf;
	LPDIRECTDRAWSURFACE	backsurf;
	LPDIRECTDRAWCLIPPER	clipper;
	LPDIRECTDRAWPALETTE	palette;
	UINT8				enable;
	UINT8				scrnmode;
	int					width;
	int					height;
	int					extend;
	int					cliping;
	RECT				scrn;
	RECT				rect;
#if defined(SUPPORT_16BPP)
	RGB32				pal16mask;
	UINT8				r16b;
	UINT8				l16r;
	UINT8				l16g;
	UINT8				padding16;
#endif
#if defined(SUPPORT_8BPP)
	PALETTEENTRY		pal[256];
#endif
} DDRAW;

typedef struct {
	int		width;
	int		height;
	int		extend;
	int		multiple;
} SCRNSTAT;

static	DDRAW		ddraw;
		SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;


static void setwindowsize(int width, int height) {

	RECT	rectwindow;
	RECT	rectclient;
	RECT	workrc;
	int		scx, scy;
	UINT	update;

	GetWindowRect(hWndMain, &rectwindow);
	GetClientRect(hWndMain, &rectclient);
	width += rectwindow.right - rectwindow.left;
	width -= rectclient.right - rectclient.left;
	height += rectwindow.bottom - rectwindow.top;
	height -= rectclient.bottom - rectclient.top;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);

	update = 0;
	if (scx < width) {
		np2oscfg.winx = (scx - width) / 2;
		update |= SYS_UPDATEOSCFG;
	}
	else {
		if ((np2oscfg.winx + width) > workrc.right) {
			np2oscfg.winx = workrc.right - width;
			update |= SYS_UPDATEOSCFG;
		}
		if (np2oscfg.winx < workrc.left) {
			np2oscfg.winx = workrc.left;
			update |= SYS_UPDATEOSCFG;
		}
	}
	if (scy < height) {
		np2oscfg.winy = (scy - height) / 2;
		update |= SYS_UPDATEOSCFG;
	}
	else {
		if ((np2oscfg.winy + height) > workrc.bottom) {
			np2oscfg.winy = workrc.bottom - height;
			update |= SYS_UPDATEOSCFG;
		}
		if (np2oscfg.winy < workrc.top) {
			np2oscfg.winy = workrc.top;
			update |= SYS_UPDATEOSCFG;
		}
	}
	sysmng_update(update);
	MoveWindow(hWndMain, np2oscfg.winx, np2oscfg.winy, width, height, TRUE);
}

static void renewalclientsize(void) {

	int		width;
	int		height;
	int		multiple;
	int		scrnwidth;
	int		scrnheight;

	width = min(scrnstat.width, ddraw.width);
	height = min(scrnstat.height, ddraw.height);

	// •`‰æ”ÍˆÍ`
	if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		ddraw.scrn.left = (ddraw.width - width) / 2;
		ddraw.scrn.top = (ddraw.height - height) / 2;
		ddraw.scrn.right = ddraw.scrn.left + width;
		ddraw.scrn.bottom = ddraw.scrn.top + height;
	}
	else {
		multiple = scrnstat.multiple;
		if (!(ddraw.scrnmode & SCRNMODE_ROTATE)) {
			scrnwidth = (width * multiple) >> 3;
			scrnheight = (height * multiple) >> 3;
			ddraw.rect.right = width;
			ddraw.rect.bottom = height;
			ddraw.scrn.left = 0;
			ddraw.scrn.top = 0;
		}
		else {
			scrnwidth = (height * multiple) >> 3;
			scrnheight = (width * multiple) >> 3;
			ddraw.rect.right = height;
			ddraw.rect.bottom = width;
			ddraw.scrn.left = 0;
			ddraw.scrn.top = 0;
		}
		ddraw.scrn.right = scrnwidth;
		ddraw.scrn.bottom = scrnheight;
		setwindowsize(scrnwidth, scrnheight);
		setwindowsize(scrnwidth, scrnheight);
	}
	scrnsurf.width = width;
	scrnsurf.height = height;
}

static void clearoutofrect(const RECT *target, const RECT *base) {

	LPDIRECTDRAWSURFACE	primsurf;
	DDBLTFX				ddbf;
	RECT				rect;

	primsurf = ddraw.primsurf;
	if (primsurf == NULL) {
		return;
	}
	ZeroMemory(&ddbf, sizeof(ddbf));
	ddbf.dwSize = sizeof(ddbf);
	ddbf.dwFillColor = 0;

	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom) {
		primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom) {
		primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom) {
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right) {
			primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right) {
			primsurf->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
	}
}

static void clearoutscreen(void) {

	RECT	base;
	RECT	target;

	GetClientRect(hWndMain, &base);
	target.left = base.left + ddraw.scrn.left;
	target.top = base.top + ddraw.scrn.top;
	target.right = base.left + ddraw.scrn.right;
	target.bottom = base.top + ddraw.scrn.bottom;
	clearoutofrect(&target, &base);
}

static void clearoutfullscreen(void) {

	clearoutofrect(&ddraw.scrn, &ddraw.rect);
}

#if defined(SUPPORT_8BPP)
static void paletteinit(void) {

	HDC 	hdc;
	UINT	i;

	hdc = GetDC(hWndMain);
	GetSystemPaletteEntries(hdc, 0, 256, ddraw.pal);
	ReleaseDC(hWndMain, hdc);
	for (i=0; i<NP2PAL_TOTAL; i++) {
		ddraw.pal[i+START_PAL].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
	ddraw.ddraw2->CreatePalette(DDPCAPS_8BIT, ddraw.pal, &ddraw.palette, 0);
	ddraw.primsurf->SetPalette(ddraw.palette);
}

static void paletteset(void) {

	UINT	i;

	if (ddraw.palette != NULL) {
		for (i=0; i<NP2PAL_TOTAL; i++) {
			ddraw.pal[i+START_PAL].peRed = np2_pal32[i].p.r;
			ddraw.pal[i+START_PAL].peBlue = np2_pal32[i].p.b;
			ddraw.pal[i+START_PAL].peGreen = np2_pal32[i].p.g;
		}
		ddraw.palette->SetEntries(0, START_PAL, NP2PAL_TOTAL,
													&ddraw.pal[START_PAL]);
	}
}
#endif

#if defined(SUPPORT_16BPP)
static void make16mask(DWORD bmask, DWORD rmask, DWORD gmask) {

	UINT8	sft;

	sft = 0;
	while((!(bmask & 0x80)) && (sft < 32)) {
		bmask <<= 1;
		sft++;
	}
	ddraw.pal16mask.p.b = (UINT8)bmask;
	ddraw.r16b = sft;

	sft = 0;
	while((rmask & 0xffffff00) && (sft < 32)) {
		rmask >>= 1;
		sft++;
	}
	ddraw.pal16mask.p.r = (UINT8)rmask;
	ddraw.l16r = sft;

	sft = 0;
	while((gmask & 0xffffff00) && (sft < 32)) {
		gmask >>= 1;
		sft++;
	}
	ddraw.pal16mask.p.g = (UINT8)gmask;
	ddraw.l16g = sft;
}
#endif


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
	scrnstat.extend = 0;
	scrnstat.multiple = 8;
	setwindowsize(640, 400);
}

BOOL scrnmng_create(UINT8 scrnmode) {

	DWORD			winstyle;
	DWORD			winstyleex;
	HMENU			hmenu;
	LPDIRECTDRAW2	ddraw2;
	DDSURFACEDESC	ddsd;
	DDPIXELFORMAT	ddpf;
	UINT			bitcolor;

	ZeroMemory(&scrnmng, sizeof(scrnmng));
	winstyle = GetWindowLong(hWndMain, GWL_STYLE);
	winstyleex = GetWindowLong(hWndMain, GWL_EXSTYLE);
	hmenu = GetMenu(hWndMain);
	if (scrnmode & SCRNMODE_FULLSCREEN) {
		scrnmode &= ~SCRNMODE_ROTATEMASK;
		scrnmng.flag = SCRNFLAG_FULLSCREEN;
		winstyle &= ~(WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU);
		winstyle |= WS_POPUP;
		winstyleex |= WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_UNCHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_CHECKED);
	}
	else {
		scrnmng.flag = SCRNFLAG_HAVEEXTEND;
		winstyle |= WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU;
		winstyle &= ~WS_POPUP;
		winstyleex &= ~WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_CHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_UNCHECKED);
	}
	SetWindowLong(hWndMain, GWL_STYLE, winstyle);
	SetWindowLong(hWndMain, GWL_EXSTYLE, winstyleex);

	if (DirectDrawCreate(NULL, &ddraw.ddraw1, NULL) != DD_OK) {
		goto scre_err;
	}
	ddraw.ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&ddraw2);
	ddraw.ddraw2 = ddraw2;

#if defined(SUPPORT_PC9821)
	scrnmode |= SCRNMODE_HIGHCOLOR;
#endif
	if (scrnmode & SCRNMODE_FULLSCREEN) {
		ddraw2->SetCooperativeLevel(hWndMain,
					DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
		if (!(scrnmode & SCRNMODE_HIGHCOLOR)) {
#if defined(SUPPORT_8BPP)
			bitcolor = 8;
#elif defined(SUPPORT_16BPP)
			bitcolor = 16;
#elif defined(SUPPORT_32BPP)
			bitcolor = 32;
#elif defined(SUPPORT_24BPP)
			bitcolor = 24;
#else
			goto scre_err;
#endif
		}
		else {
#if defined(SUPPORT_16BPP)
			bitcolor = 16;
#elif defined(SUPPORT_32BPP)
			bitcolor = 32;
#elif defined(SUPPORT_24BPP)
			bitcolor = 24;
#else
			goto scre_err;
#endif
		}
		if (ddraw2->SetDisplayMode(640, 480, bitcolor, 0, 0) != DD_OK) {
			goto scre_err;
		}
		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, hWndMain);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}
//		fullscrn_clearblank();

		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		if (ddraw.primsurf->GetPixelFormat(&ddpf) != DD_OK) {
			goto scre_err;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = 640;
		ddsd.dwHeight = 480;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
	}
	else {
		ddraw2->SetCooperativeLevel(hWndMain, DDSCL_NORMAL);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}

		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, hWndMain);
		ddraw.primsurf->SetClipper(ddraw.clipper);

		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		if (ddraw.primsurf->GetPixelFormat(&ddpf) != DD_OK) {
			goto scre_err;
		}

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		if (!(scrnmode & SCRNMODE_ROTATE)) {
			ddsd.dwWidth = 640;
			ddsd.dwHeight = 480;
		}
		else {
			ddsd.dwWidth = 480;
			ddsd.dwHeight = 640;
		}

		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		bitcolor = ddpf.dwRGBBitCount;
	}
	if (bitcolor == 8) {
#if defined(SUPPORT_8BPP)
		paletteinit();
#else
		goto scre_err;
#endif
	}
	else if (bitcolor == 16) {
#if defined(SUPPORT_16BPP)
		make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
#else
		goto scre_err;
#endif
	}
	else if (bitcolor == 24) {
#if !defined(SUPPORT_24BPP)
		goto scre_err;
#endif
	}
	else if (bitcolor == 32) {
#if !defined(SUPPORT_32BPP)
		goto scre_err;
#endif
	}
	else {
		goto scre_err;
	}
	scrnmng.bpp = (UINT8)bitcolor;
	scrnsurf.bpp = bitcolor;
	ddraw.enable = TRUE;
	ddraw.scrnmode = scrnmode;
	ddraw.width = 640;
	ddraw.height = 480;
	ddraw.cliping = 0;
	renewalclientsize();
	return(SUCCESS);

scre_err:
	scrnmng_destroy();
	return(FAILURE);
}

void scrnmng_destroy(void) {

	if (ddraw.backsurf) {
		ddraw.backsurf->Release();
	}
	if (ddraw.palette) {
		ddraw.palette->Release();
	}
	if (ddraw.clipper) {
		ddraw.clipper->Release();
	}
	if (ddraw.primsurf) {
		ddraw.primsurf->Release();
	}
	if (ddraw.ddraw2) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			ddraw.ddraw2->SetCooperativeLevel(hWndMain, DDSCL_NORMAL);
		}
		ddraw.ddraw2->Release();
	}
	if (ddraw.ddraw1) {
		ddraw.ddraw1->Release();
	}
	ZeroMemory(&ddraw, sizeof(ddraw));
}

void scrnmng_querypalette(void) {

	if (ddraw.palette) {
		ddraw.primsurf->SetPalette(ddraw.palette);
	}
}

#if defined(SUPPORT_16BPP)
UINT16 scrnmng_makepal16(RGB32 pal32) {

	RGB32	pal;

	pal.d = pal32.d & ddraw.pal16mask.d;
	return((UINT16)((pal.p.g << ddraw.l16g) +
						(pal.p.r << ddraw.l16r) + (pal.p.b >> ddraw.r16b)));
}
#endif

void scrnmng_topwinui(void) {

	mousemng_disable(MOUSEPROC_WINUI);
	if (!ddraw.cliping++) {											// ver0.28
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(ddraw.clipper);
		}
#ifndef __GNUC__
		WINNLSEnableIME(hWndMain, TRUE);
#endif
	}
}

void scrnmng_clearwinui(void) {

	if ((ddraw.cliping > 0) && (!(--ddraw.cliping))) {
#ifndef __GNUC__
		WINNLSEnableIME(hWndMain, FALSE);
#endif
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(0);
		}
	}
	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		clearoutfullscreen();
	}
	mousemng_enable(MOUSEPROC_WINUI);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
	renewalclientsize();
}

void scrnmng_setextend(int extend) {

	(void)extend;
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
	renewalclientsize();
}

const SCRNSURF *scrnmng_surflock(void) {

	DDSURFACEDESC	destscrn;

	ZeroMemory(&destscrn, sizeof(destscrn));
	destscrn.dwSize = sizeof(destscrn);
	if (ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL) != DD_OK) {
		return(NULL);
	}
	if (!(ddraw.scrnmode & SCRNMODE_ROTATE)) {
		scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		scrnsurf.xalign = scrnsurf.bpp >> 3;
		scrnsurf.yalign = destscrn.lPitch;
	}
	else if (!(ddraw.scrnmode & SCRNMODE_ROTATEDIR)) {
		scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		scrnsurf.ptr += (scrnsurf.width - 1) * destscrn.lPitch;
		scrnsurf.xalign = 0 - destscrn.lPitch;
		scrnsurf.yalign = scrnsurf.bpp >> 3;
	}
	else {
		scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		scrnsurf.ptr += (scrnsurf.height - 1) * (scrnsurf.bpp >> 3);
		scrnsurf.xalign = destscrn.lPitch;
		scrnsurf.yalign = 0 - (scrnsurf.bpp >> 3);
	}
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	ddraw.backsurf->Unlock(NULL);
	scrnmng_update();
}

void scrnmng_update(void) {

	POINT	clip;
	RECT	dst;

#if defined(SUPPORT_8BPP)
	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		paletteset();
	}
#endif
	if (ddraw.backsurf != NULL) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutfullscreen();
			}
			if (ddraw.primsurf->Blt(&ddraw.scrn, ddraw.backsurf, &ddraw.rect,
									DDBLT_WAIT, NULL) == DDERR_SURFACELOST) {
				ddraw.primsurf->Restore();
				ddraw.backsurf->Restore();
			}
		}
		else {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutscreen();
			}
			clip.x = 0;
			clip.y = 0;
			ClientToScreen(hWndMain, &clip);
			dst.left = clip.x + ddraw.scrn.left;
			dst.top = clip.y + ddraw.scrn.top;
			dst.right = clip.x + ddraw.scrn.right;
			dst.bottom = clip.y + ddraw.scrn.bottom;
			if (ddraw.primsurf->Blt(&dst, ddraw.backsurf, &ddraw.rect,
									DDBLT_WAIT, NULL) == DDERR_SURFACELOST) {
				ddraw.primsurf->Restore();
				ddraw.backsurf->Restore();
			}
		}
	}
}

void scrnmng_restoresize(void) {

	if ((ddraw.enable) && (!(ddraw.scrnmode & SCRNMODE_FULLSCREEN))) {
		renewalclientsize();
	}
}

