#include	"compiler.h"
#include	<ddraw.h>
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"np2.h"
#include	"winloc.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"np2class.h"
#include	"pccore.h"
#include	"scrndraw.h"
#include	"palettes.h"

#if defined(SUPPORT_DCLOCK)
#include	"dclock.h"
#endif

extern WINLOCEX np2_winlocexallwin(HWND base);


typedef struct {
	LPDIRECTDRAW		ddraw1;
	LPDIRECTDRAW2		ddraw2;
	LPDIRECTDRAWSURFACE	primsurf;
	LPDIRECTDRAWSURFACE	backsurf;
#if defined(SUPPORT_DCLOCK)
	LPDIRECTDRAWSURFACE	clocksurf;
#endif
	LPDIRECTDRAWCLIPPER	clipper;
	LPDIRECTDRAWPALETTE	palette;
	UINT				scrnmode;
	int					width;
	int					height;
	int					extend;
	int					cliping;
	RGB32				pal16mask;
	UINT8				r16b;
	UINT8				l16r;
	UINT8				l16g;
	UINT8				menudisp;
	int					menusize;
	RECT				scrn;
	RECT				rect;
	RECT				scrnclip;
	RECT				rectclip;
	PALETTEENTRY		pal[256];
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


static void setwindowsize(HWND hWnd, int width, int height) {

	RECT	workrc;
	int		scx;
	int		scy;
	UINT	cnt;
	RECT	rectwindow;
	RECT	rectclient;
	int		cx;
	int		cy;
	UINT	update;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);

	cnt = 2;
	do {
		GetWindowRect(hWnd, &rectwindow);
		GetClientRect(hWnd, &rectclient);
		cx = width;
		cx += np2oscfg.paddingx * 2;
		cx += rectwindow.right - rectwindow.left;
		cx -= rectclient.right - rectclient.left;
		cy = height;
		cy += np2oscfg.paddingy * 2;
		cy += rectwindow.bottom - rectwindow.top;
		cy -= rectclient.bottom - rectclient.top;

		update = 0;
		if (scx < cx) {
			np2oscfg.winx = (scx - cx) / 2;
			update |= SYS_UPDATEOSCFG;
		}
		else {
			if ((np2oscfg.winx + cx) > workrc.right) {
				np2oscfg.winx = workrc.right - cx;
				update |= SYS_UPDATEOSCFG;
			}
			if (np2oscfg.winx < workrc.left) {
				np2oscfg.winx = workrc.left;
				update |= SYS_UPDATEOSCFG;
			}
		}
		if (scy < cy) {
			np2oscfg.winy = (scy - cy) / 2;
			update |= SYS_UPDATEOSCFG;
		}
		else {
			if ((np2oscfg.winy + cy) > workrc.bottom) {
				np2oscfg.winy = workrc.bottom - cy;
				update |= SYS_UPDATEOSCFG;
			}
			if (np2oscfg.winy < workrc.top) {
				np2oscfg.winy = workrc.top;
				update |= SYS_UPDATEOSCFG;
			}
		}
		sysmng_update(update);
		MoveWindow(hWnd, np2oscfg.winx, np2oscfg.winy, cx, cy, TRUE);
	} while(--cnt);
}

static void renewalclientsize(BOOL winloc) {

	int			width;
	int			height;
	int			extend;
	UINT		fscrnmod;
	int			multiple;
	int			scrnwidth;
	int			scrnheight;
	int			tmpcy;
	WINLOCEX	wlex;

	width = min(scrnstat.width, ddraw.width);
	height = min(scrnstat.height, ddraw.height);
	extend = 0;

	// ï`âÊîÕàÕÅ`
	if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		scrnwidth = width;
		scrnheight = height;
		fscrnmod = np2oscfg.fscrnmod & FSCRNMOD_ASPECTMASK;
		switch(fscrnmod) {
			default:
			case FSCRNMOD_NORESIZE:
				break;

			case FSCRNMOD_ASPECTFIX8:
				scrnwidth = (ddraw.width << 3) / width;
				scrnheight = (ddraw.height << 3) / height;
				multiple = min(scrnwidth, scrnheight);
				scrnwidth = (width * multiple) >> 3;
				scrnheight = (height * multiple) >> 3;
				break;

			case FSCRNMOD_ASPECTFIX:
				scrnwidth = ddraw.width;
				scrnheight = (scrnwidth * height) / width;
				if (scrnheight >= ddraw.height) {
					scrnheight = ddraw.height;
					scrnwidth = (scrnheight * width) / height;
				}
				break;

			case FSCRNMOD_LARGE:
				scrnwidth = ddraw.width;
				scrnheight = ddraw.height;
				break;
		}
		ddraw.scrn.left = (ddraw.width - scrnwidth) / 2;
		ddraw.scrn.top = (ddraw.height - scrnheight) / 2;
		ddraw.scrn.right = ddraw.scrn.left + scrnwidth;
		ddraw.scrn.bottom = ddraw.scrn.top + scrnheight;

		// ÉÅÉjÉÖÅ[ï\é¶éûÇÃï`âÊóÃàÊ
		ddraw.rectclip = ddraw.rect;
		ddraw.scrnclip = ddraw.scrn;
		if (ddraw.scrnclip.top < ddraw.menusize) {
			ddraw.scrnclip.top = ddraw.menusize;
			tmpcy = ddraw.height - ddraw.menusize;
			if (scrnheight > tmpcy) {
				switch(fscrnmod) {
					default:
					case FSCRNMOD_NORESIZE:
						tmpcy = min(tmpcy, height);
						ddraw.rectclip.bottom = tmpcy;
						break;

					case FSCRNMOD_ASPECTFIX8:
					case FSCRNMOD_ASPECTFIX:
						ddraw.rectclip.bottom = (tmpcy * height) / scrnheight;
						break;

					case FSCRNMOD_LARGE:
						break;
				}
			}
			ddraw.scrnclip.bottom = ddraw.menusize + tmpcy;
		}
	}
	else {
		multiple = scrnstat.multiple;
		if (!(ddraw.scrnmode & SCRNMODE_ROTATE)) {
			if ((np2oscfg.paddingx) && (multiple == 8)) {
				extend = min(scrnstat.extend, ddraw.extend);
			}
			scrnwidth = (width * multiple) >> 3;
			scrnheight = (height * multiple) >> 3;
			ddraw.rect.right = width + extend;
			ddraw.rect.bottom = height;
			ddraw.scrn.left = np2oscfg.paddingx - extend;
			ddraw.scrn.top = np2oscfg.paddingy;
		}
		else {
			if ((np2oscfg.paddingy) && (multiple == 8)) {
				extend = min(scrnstat.extend, ddraw.extend);
			}
			scrnwidth = (height * multiple) >> 3;
			scrnheight = (width * multiple) >> 3;
			ddraw.rect.right = height;
			ddraw.rect.bottom = width + extend;
			ddraw.scrn.left = np2oscfg.paddingx;
			ddraw.scrn.top = np2oscfg.paddingy - extend;
		}
		ddraw.scrn.right = np2oscfg.paddingx + scrnwidth;
		ddraw.scrn.bottom = np2oscfg.paddingy + scrnheight;

		wlex = NULL;
		if (winloc) {
			wlex = np2_winlocexallwin(g_hWndMain);
		}
		winlocex_setholdwnd(wlex, g_hWndMain);
		setwindowsize(g_hWndMain, scrnwidth, scrnheight);
		winlocex_move(wlex);
		winlocex_destroy(wlex);
	}
	scrnsurf.width = width;
	scrnsurf.height = height;
	scrnsurf.extend = extend;
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
	POINT	clipt;
	RECT	target;

	GetClientRect(g_hWndMain, &base);
	clipt.x = 0;
	clipt.y = 0;
	ClientToScreen(g_hWndMain, &clipt);
	base.left += clipt.x;
	base.top += clipt.y;
	base.right += clipt.x;
	base.bottom += clipt.y;
	target.left = base.left + ddraw.scrn.left;
	target.top = base.top + ddraw.scrn.top;
	target.right = base.left + ddraw.scrn.right;
	target.bottom = base.top + ddraw.scrn.bottom;
	clearoutofrect(&target, &base);
}

static void clearoutfullscreen(void) {

	RECT	base;
const RECT	*scrn;

	base.left = 0;
	base.top = 0;
	base.right = ddraw.width;
	base.bottom = ddraw.height;
	if (GetWindowLongPtr(g_hWndMain, NP2GWLP_HMENU)) {
		scrn = &ddraw.scrn;
		base.top = 0;
	}
	else {
		scrn = &ddraw.scrnclip;
		base.top = ddraw.menusize;
	}
	clearoutofrect(scrn, &base);
#if defined(SUPPORT_DCLOCK)
	dclock_redraw();
#endif
}

static void paletteinit(void) {

	HDC 	hdc;
	UINT	i;

	hdc = GetDC(g_hWndMain);
	GetSystemPaletteEntries(hdc, 0, 256, ddraw.pal);
	ReleaseDC(g_hWndMain, hdc);
#if defined(SUPPORT_DCLOCK)
	for (i=0; i<4; i++) {
		ddraw.pal[i+START_PALORG].peBlue = dclockpal.pal32[i].p.b;
		ddraw.pal[i+START_PALORG].peRed = dclockpal.pal32[i].p.r;
		ddraw.pal[i+START_PALORG].peGreen = dclockpal.pal32[i].p.g;
		ddraw.pal[i+START_PALORG].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
#endif
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


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
	scrnstat.extend = 1;
	scrnstat.multiple = 8;
	setwindowsize(g_hWndMain, 640, 400);
}

BRESULT scrnmng_create(UINT8 scrnmode) {

	DWORD			winstyle;
	DWORD			winstyleex;
	HMENU			hmenu;
	LPDIRECTDRAW2	ddraw2;
	DDSURFACEDESC	ddsd;
	DDPIXELFORMAT	ddpf;
	int				width;
	int				height;
	UINT			bitcolor;
	UINT			fscrnmod;
	DEVMODE			devmode;

	ZeroMemory(&scrnmng, sizeof(scrnmng));
	winstyle = GetWindowLong(g_hWndMain, GWL_STYLE);
	winstyleex = GetWindowLong(g_hWndMain, GWL_EXSTYLE);
	hmenu = GetMenu(g_hWndMain);
	if (scrnmode & SCRNMODE_FULLSCREEN) {
		scrnmode &= ~SCRNMODE_ROTATEMASK;
		scrnmng.flag = SCRNFLAG_FULLSCREEN;
		winstyle &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
		winstyle |= WS_POPUP;
		winstyleex |= WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_UNCHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_CHECKED);
		ddraw.menudisp = 0;
		ddraw.menusize = GetSystemMetrics(SM_CYMENU);
		np2class_enablemenu(g_hWndMain, FALSE);
	}
	else {
		scrnmng.flag = SCRNFLAG_HAVEEXTEND;
		winstyle |= WS_SYSMENU;
		if (np2oscfg.thickframe) {
			winstyle |= WS_THICKFRAME;
		}
		if (np2oscfg.wintype < 2) {
			winstyle |= WS_CAPTION;
		}
		winstyle &= ~WS_POPUP;
		winstyleex &= ~WS_EX_TOPMOST;
		CheckMenuItem(hmenu, IDM_WINDOW, MF_CHECKED);
		CheckMenuItem(hmenu, IDM_FULLSCREEN, MF_UNCHECKED);
	}
	SetWindowLong(g_hWndMain, GWL_STYLE, winstyle);
	SetWindowLong(g_hWndMain, GWL_EXSTYLE, winstyleex);

	if (DirectDrawCreate(NULL, &ddraw.ddraw1, NULL) != DD_OK) {
		goto scre_err;
	}
	ddraw.ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&ddraw2);
	ddraw.ddraw2 = ddraw2;

	if (scrnmode & SCRNMODE_FULLSCREEN) {
#if defined(SUPPORT_DCLOCK)
		dclock_init();
#endif
		ddraw2->SetCooperativeLevel(g_hWndMain,
										DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		width = np2oscfg.fscrn_cx;
		height = np2oscfg.fscrn_cy;
		bitcolor = np2oscfg.fscrnbpp;
		fscrnmod = np2oscfg.fscrnmod;
		if ((fscrnmod & (FSCRNMOD_SAMERES | FSCRNMOD_SAMEBPP)) &&
			(EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devmode))) {
			if (fscrnmod & FSCRNMOD_SAMERES) {
				width = devmode.dmPelsWidth;
				height = devmode.dmPelsHeight;
			}
			if (fscrnmod & FSCRNMOD_SAMEBPP) {
				bitcolor = devmode.dmBitsPerPel;
			}
		}
		if ((width == 0) || (height == 0)) {
			width = 640;
			height = (np2oscfg.force400)?400:480;
		}
		if (bitcolor == 0) {
#if !defined(SUPPORT_PC9821)
			bitcolor = (scrnmode & SCRNMODE_HIGHCOLOR)?16:8;
#else
			bitcolor = 16;
#endif
		}
		if (ddraw2->SetDisplayMode(width, height, bitcolor, 0, 0) != DD_OK) {
			goto scre_err;
		}
		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, g_hWndMain);

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
		if (bitcolor == 8) {
			paletteinit();
		}
		else if (bitcolor == 16) {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}
		else if (bitcolor == 24) {
		}
		else if (bitcolor == 32) {
		}
		else {
			goto scre_err;
		}
#if defined(SUPPORT_DCLOCK)
		dclock_palset(bitcolor);
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = DCLOCK_WIDTH;
		ddsd.dwHeight = DCLOCK_HEIGHT;
		ddraw2->CreateSurface(&ddsd, &ddraw.clocksurf, NULL);
		dclock_reset();
#endif
	}
	else {
		ddraw2->SetCooperativeLevel(g_hWndMain, DDSCL_NORMAL);

		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (ddraw2->CreateSurface(&ddsd, &ddraw.primsurf, NULL) != DD_OK) {
			goto scre_err;
		}

		ddraw2->CreateClipper(0, &ddraw.clipper, NULL);
		ddraw.clipper->SetHWnd(0, g_hWndMain);
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
			ddsd.dwWidth = 640 + 1;
			ddsd.dwHeight = 480;
		}
		else {
			ddsd.dwWidth = 480;
			ddsd.dwHeight = 640 + 1;
		}
		width = 640;
		height = 480;

		if (ddraw2->CreateSurface(&ddsd, &ddraw.backsurf, NULL) != DD_OK) {
			goto scre_err;
		}
		bitcolor = ddpf.dwRGBBitCount;
		if (bitcolor == 8) {
			paletteinit();
		}
		else if (bitcolor == 16) {
			make16mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}
		else if (bitcolor == 24) {
		}
		else if (bitcolor == 32) {
		}
		else {
			goto scre_err;
		}
		ddraw.extend = 1;
	}
	scrnmng.bpp = (UINT8)bitcolor;
	scrnsurf.bpp = bitcolor;
	ddraw.scrnmode = scrnmode;
	ddraw.width = width;
	ddraw.height = height;
	ddraw.cliping = 0;
	renewalclientsize(FALSE);
//	screenupdate = 3;					// update!
	return(SUCCESS);

scre_err:
	scrnmng_destroy();
	return(FAILURE);
}

void scrnmng_destroy(void) {

	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		np2class_enablemenu(g_hWndMain, (!np2oscfg.wintype));
	}
#if defined(SUPPORT_DCLOCK)
	if (ddraw.clocksurf) {
		ddraw.clocksurf->Release();
		ddraw.clocksurf = NULL;
	}
#endif
	if (ddraw.backsurf) {
		ddraw.backsurf->Release();
		ddraw.backsurf = NULL;
	}
	if (ddraw.palette) {
		ddraw.palette->Release();
		ddraw.palette = NULL;
	}
	if (ddraw.clipper) {
		ddraw.clipper->Release();
		ddraw.clipper = NULL;
	}
	if (ddraw.primsurf) {
		ddraw.primsurf->Release();
		ddraw.primsurf = NULL;
	}
	if (ddraw.ddraw2) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			ddraw.ddraw2->SetCooperativeLevel(g_hWndMain, DDSCL_NORMAL);
		}
		ddraw.ddraw2->Release();
		ddraw.ddraw2 = NULL;
	}
	if (ddraw.ddraw1) {
		ddraw.ddraw1->Release();
		ddraw.ddraw1 = NULL;
	}
	ZeroMemory(&ddraw, sizeof(ddraw));
}

void scrnmng_querypalette(void) {

	if (ddraw.palette) {
		ddraw.primsurf->SetPalette(ddraw.palette);
	}
}

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB32	pal;

	pal.d = pal32.d & ddraw.pal16mask.d;
	return((RGB16)((pal.p.g << ddraw.l16g) +
						(pal.p.r << ddraw.l16r) + (pal.p.b >> ddraw.r16b)));
}

void scrnmng_fullscrnmenu(int y) {

	UINT8	menudisp;

	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		menudisp = ((y >= 0) && (y < ddraw.menusize))?1:0;
		if (ddraw.menudisp != menudisp) {
			ddraw.menudisp = menudisp;
			if (menudisp == 1) {
				np2class_enablemenu(g_hWndMain, TRUE);
			}
			else {
				np2class_enablemenu(g_hWndMain, FALSE);
				clearoutfullscreen();
			}
		}
	}
}

void scrnmng_topwinui(void) {

	mousemng_disable(MOUSEPROC_WINUI);
	if (!ddraw.cliping++) {											// ver0.28
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(ddraw.clipper);
		}
#ifndef __GNUC__
		WINNLSEnableIME(g_hWndMain, TRUE);
#endif
	}
}

void scrnmng_clearwinui(void) {

	if ((ddraw.cliping > 0) && (!(--ddraw.cliping))) {
#ifndef __GNUC__
		WINNLSEnableIME(g_hWndMain, FALSE);
#endif
		if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
			ddraw.primsurf->SetClipper(0);
		}
	}
	if (scrnmng.flag & SCRNFLAG_FULLSCREEN) {
		np2class_enablemenu(g_hWndMain, FALSE);
		clearoutfullscreen();
		ddraw.menudisp = 0;
	}
	else {
		if (np2oscfg.wintype) {
			np2class_enablemenu(g_hWndMain, FALSE);
			InvalidateRect(g_hWndMain, NULL, TRUE);
		}
	}
	mousemng_enable(MOUSEPROC_WINUI);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
	renewalclientsize(TRUE);
}

void scrnmng_setextend(int extend) {

	scrnstat.extend = extend;
	scrnmng.allflash = TRUE;
	renewalclientsize(TRUE);
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
	renewalclientsize(TRUE);
}

const SCRNSURF *scrnmng_surflock(void) {

	DDSURFACEDESC	destscrn;
	HRESULT			r;

	ZeroMemory(&destscrn, sizeof(destscrn));
	destscrn.dwSize = sizeof(destscrn);
	if (ddraw.backsurf == NULL) {
		return(NULL);
	}
	r = ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	if (r == DDERR_SURFACELOST) {
		ddraw.backsurf->Restore();
		r = ddraw.backsurf->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	}
	if (r != DD_OK) {
//		TRACEOUT(("backsurf lock error: %d (%d)", r));
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
	RECT	*rect;
	RECT	*scrn;
	HRESULT	r;

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		paletteset();
	}
	if (ddraw.backsurf != NULL) {
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN) {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutfullscreen();
			}
			if (GetWindowLongPtr(g_hWndMain, NP2GWLP_HMENU)) {
				rect = &ddraw.rect;
				scrn = &ddraw.scrn;
			}
			else {
				rect = &ddraw.rectclip;
				scrn = &ddraw.scrnclip;
			}
			r = ddraw.primsurf->Blt(scrn, ddraw.backsurf, rect,
															DDBLT_WAIT, NULL);
			if (r == DDERR_SURFACELOST) {
				ddraw.backsurf->Restore();
				ddraw.primsurf->Restore();
				ddraw.primsurf->Blt(scrn, ddraw.backsurf, rect,
															DDBLT_WAIT, NULL);
			}
		}
		else {
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				clearoutscreen();
			}
			clip.x = 0;
			clip.y = 0;
			ClientToScreen(g_hWndMain, &clip);
			dst.left = clip.x + ddraw.scrn.left;
			dst.top = clip.y + ddraw.scrn.top;
			dst.right = clip.x + ddraw.scrn.right;
			dst.bottom = clip.y + ddraw.scrn.bottom;
			r = ddraw.primsurf->Blt(&dst, ddraw.backsurf, &ddraw.rect,
									DDBLT_WAIT, NULL);
			if (r == DDERR_SURFACELOST) {
				ddraw.backsurf->Restore();
				ddraw.primsurf->Restore();
				ddraw.primsurf->Blt(&dst, ddraw.backsurf, &ddraw.rect,
														DDBLT_WAIT, NULL);
			}
		}
	}
}


// ----

void scrnmng_setmultiple(int multiple) {

	if (scrnstat.multiple != multiple) {
		scrnstat.multiple = multiple;
		renewalclientsize(TRUE);
	}
}


// ----

#if defined(SUPPORT_DCLOCK)
static const RECT rectclk = {0, 0, DCLOCK_WIDTH, DCLOCK_HEIGHT};

BOOL scrnmng_isdispclockclick(const POINT *pt) {

	if (pt->y >= (ddraw.height - DCLOCK_HEIGHT)) {
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}

void scrnmng_dispclock(void) {

	DDSURFACEDESC	dest;
const RECT			*scrn;
																// ver0.26
	if (!ddraw.clocksurf) {
		return;
	}
	if (!dclock_disp()) {
		return;
	}
	if (GetWindowLongPtr(g_hWndMain, NP2GWLP_HMENU)) {
		scrn = &ddraw.scrn;
	}
	else {
		scrn = &ddraw.scrnclip;
	}
	if ((scrn->bottom + DCLOCK_HEIGHT) > ddraw.height) {
		return;
	}
	dclock_make();
	ZeroMemory(&dest, sizeof(dest));
	dest.dwSize = sizeof(dest);
	if (ddraw.clocksurf->Lock(NULL, &dest, DDLOCK_WAIT, NULL) == DD_OK) {
		if (scrnmng.bpp == 8) {
			dclock_out8(dest.lpSurface, dest.lPitch);
		}
		else if (scrnmng.bpp == 16) {
			dclock_out16(dest.lpSurface, dest.lPitch);
		}
		else if (scrnmng.bpp == 24) {
			dclock_out24(dest.lpSurface, dest.lPitch);
		}
		else if (scrnmng.bpp == 32) {
			dclock_out32(dest.lpSurface, dest.lPitch);
		}
		ddraw.clocksurf->Unlock(NULL);
	}
	if (ddraw.primsurf->BltFast(ddraw.width - DCLOCK_WIDTH - 4,
									ddraw.height - DCLOCK_HEIGHT,
									ddraw.clocksurf, (RECT *)&rectclk,
									DDBLTFAST_WAIT) == DDERR_SURFACELOST) {
		ddraw.primsurf->Restore();
		ddraw.clocksurf->Restore();
	}
	dclock_cntdown(np2oscfg.DRAW_SKIP);
}
#endif


// ----

typedef struct {
	int		bx;
	int		by;
	int		cx;
	int		cy;
	int		mul;
} SCRNSIZING;

static	SCRNSIZING	scrnsizing;

enum {
	SIZING_ADJUST	= 12
};

void scrnmng_entersizing(void) {

	RECT	rectwindow;
	RECT	rectclient;
	int		cx;
	int		cy;

	GetWindowRect(g_hWndMain, &rectwindow);
	GetClientRect(g_hWndMain, &rectclient);
	scrnsizing.bx = (np2oscfg.paddingx * 2) +
					(rectwindow.right - rectwindow.left) -
					(rectclient.right - rectclient.left);
	scrnsizing.by = (np2oscfg.paddingy * 2) +
					(rectwindow.bottom - rectwindow.top) -
					(rectclient.bottom - rectclient.top);
	cx = min(scrnstat.width, ddraw.width);
	cx = (cx + 7) >> 3;
	cy = min(scrnstat.height, ddraw.height);
	cy = (cy + 7) >> 3;
	if (!(ddraw.scrnmode & SCRNMODE_ROTATE)) {
		scrnsizing.cx = cx;
		scrnsizing.cy = cy;
	}
	else {
		scrnsizing.cx = cy;
		scrnsizing.cy = cx;
	}
	scrnsizing.mul = scrnstat.multiple;
}

void scrnmng_sizing(UINT side, RECT *rect) {

	int		width;
	int		height;
	int		mul;

	if ((side != WMSZ_TOP) && (side != WMSZ_BOTTOM)) {
		width = rect->right - rect->left - scrnsizing.bx + SIZING_ADJUST;
		width /= scrnsizing.cx;
	}
	else {
		width = 16;
	}
	if ((side != WMSZ_LEFT) && (side != WMSZ_RIGHT)) {
		height = rect->bottom - rect->top - scrnsizing.by + SIZING_ADJUST;
		height /= scrnsizing.cy;
	}
	else {
		height = 16;
	}
	mul = min(width, height);
	if (mul <= 0) {
		mul = 1;
	}
	else if (mul > 16) {
		mul = 16;
	}
	width = scrnsizing.bx + (scrnsizing.cx * mul);
	height = scrnsizing.by + (scrnsizing.cy * mul);
	switch(side) {
		case WMSZ_LEFT:
		case WMSZ_TOPLEFT:
		case WMSZ_BOTTOMLEFT:
			rect->left = rect->right - width;
			break;

		case WMSZ_RIGHT:
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMRIGHT:
		default:
			rect->right = rect->left + width;
			break;
	}

	switch(side) {
		case WMSZ_TOP:
		case WMSZ_TOPLEFT:
		case WMSZ_TOPRIGHT:
			rect->top = rect->bottom - height;
			break;

		case WMSZ_LEFT:
		case WMSZ_RIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMLEFT:
		case WMSZ_BOTTOMRIGHT:
		default:
			rect->bottom = rect->top + height;
			break;
	}
	scrnsizing.mul = mul;
}

void scrnmng_exitsizing(void) {

	sysmenu_setscrnmul(scrnsizing.mul);
	scrnmng_setmultiple(scrnsizing.mul);
	InvalidateRect(g_hWndMain, NULL, TRUE);		// ugh
}

