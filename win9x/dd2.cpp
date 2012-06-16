#include	"compiler.h"
#include	<ddraw.h>
#include	"dd2.h"


typedef struct {
	HWND					hwnd;
	LPDIRECTDRAW			ddraw1;
	LPDIRECTDRAW2			ddraw;
	LPDIRECTDRAWSURFACE		primsurf;
	LPDIRECTDRAWSURFACE		backsurf;
	DDPIXELFORMAT			ddpf;
	LPDIRECTDRAWCLIPPER		clipper;
	LPDIRECTDRAWPALETTE		palette;
	int						cliping;
	RGB32					pal16;
	UINT8					r16b;
	UINT8					l16r;
	UINT8					l16g;
	CMNVRAM					vram;
	PALETTEENTRY			pal[256];
} _DD2SURF, *DD2SURF;


DD2HDL dd2_create(HWND hwnd, int width, int height) {

	DD2SURF			dd2;
	DDSURFACEDESC	ddsd;
	HDC 			hdc;

	dd2 = (DD2SURF)_MALLOC(sizeof(_DD2SURF), "dd2surf");
	if (dd2 == NULL) {
		goto dd2cre_err1;
	}
	ZeroMemory(dd2, sizeof(_DD2SURF));
	dd2->hwnd = hwnd;
	if (DirectDrawCreate(NULL, &dd2->ddraw1, NULL) != DD_OK) {
		goto dd2cre_err2;
	}
	dd2->ddraw1->QueryInterface(IID_IDirectDraw2, (void **)&dd2->ddraw);
	dd2->ddraw->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (dd2->ddraw->CreateSurface(&ddsd, &dd2->primsurf, NULL) != DD_OK) {
		goto dd2cre_err2;
	}
	dd2->ddraw->CreateClipper(0, &dd2->clipper, NULL);
	dd2->clipper->SetHWnd(0, hwnd);
	dd2->primsurf->SetClipper(dd2->clipper);
	ZeroMemory(&dd2->ddpf, sizeof(DDPIXELFORMAT));
	dd2->ddpf.dwSize = sizeof(DDPIXELFORMAT);
	if (dd2->primsurf->GetPixelFormat(&dd2->ddpf) != DD_OK) {
		goto dd2cre_err2;
	}
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	if (dd2->ddraw->CreateSurface(&ddsd, &dd2->backsurf, NULL) != DD_OK) {
		goto dd2cre_err2;
	}
	if (dd2->ddpf.dwRGBBitCount == 8) {
		hdc = GetDC(hwnd);
		GetSystemPaletteEntries(hdc, 0, 256, dd2->pal);
		ReleaseDC(hwnd, hdc);
		dd2->ddraw->CreatePalette(DDPCAPS_8BIT, dd2->pal, &dd2->palette, 0);
		dd2->primsurf->SetPalette(dd2->palette);
	}
	else if (dd2->ddpf.dwRGBBitCount == 16) {
		WORD	bit;
		UINT8	cnt;

		dd2->pal16.d = 0;
		for (bit=1; (bit) && (!(dd2->ddpf.dwBBitMask & bit)); bit<<=1);
		for (dd2->r16b=8; (dd2->r16b) && (dd2->ddpf.dwBBitMask & bit);
													dd2->r16b--, bit<<=1) {
			dd2->pal16.p.b >>= 1;
			dd2->pal16.p.b |= 0x80;
		}
		for (dd2->l16r=0, bit=1; (bit) && (!(dd2->ddpf.dwRBitMask & bit));
													dd2->l16r++, bit<<=1);
		for (cnt=0x80; (cnt) && (dd2->ddpf.dwRBitMask & bit);
													cnt>>=1, bit<<=1) {
			dd2->pal16.p.r |= cnt;
		}
		for (; cnt; cnt>>=1) dd2->l16r--;
		for (dd2->l16g=0, bit=1; (bit) && (!(dd2->ddpf.dwGBitMask & bit));
													dd2->l16g++, bit<<=1);
		for (cnt=0x80; (cnt) && (dd2->ddpf.dwGBitMask & bit);
													cnt>>=1, bit<<=1) {
			dd2->pal16.p.g |= cnt;
		}
		for (; cnt; cnt>>=1) dd2->l16g--;
	}
	else if (dd2->ddpf.dwRGBBitCount == 24) {
	}
	else if (dd2->ddpf.dwRGBBitCount == 32) {
	}
	else {
		goto dd2cre_err2;
	}
	dd2->cliping = 0;
	dd2->vram.width = width;
	dd2->vram.height = height;
	dd2->vram.xalign = dd2->ddpf.dwRGBBitCount / 8;
	dd2->vram.bpp = dd2->ddpf.dwRGBBitCount;
	return((DD2HDL)dd2);

dd2cre_err2:
	dd2_release(dd2);
	_MFREE(dd2);

dd2cre_err1:
	return(NULL);
}

void dd2_release(DD2HDL dd2hdl) {

	DD2SURF	dd2;

	dd2 = (DD2SURF)dd2hdl;
	if (dd2) {
		RELEASE(dd2->palette);
		RELEASE(dd2->clipper);
		RELEASE(dd2->backsurf);
		RELEASE(dd2->primsurf);
		RELEASE(dd2->ddraw);
		RELEASE(dd2->ddraw1);
		_MFREE(dd2);
	}
}

CMNVRAM *dd2_bsurflock(DD2HDL dd2hdl) {

	DD2SURF			dd2;
	DDSURFACEDESC	surface;
	HRESULT			r;

	dd2 = (DD2SURF)dd2hdl;
	if ((dd2 == NULL) || (dd2->backsurf == NULL)) {
		return(NULL);
	}
	ZeroMemory(&surface, sizeof(DDSURFACEDESC));
	surface.dwSize = sizeof(DDSURFACEDESC);
	r = dd2->backsurf->Lock(NULL, &surface, DDLOCK_WAIT, NULL);
	if (r == DDERR_SURFACELOST) {
		dd2->backsurf->Restore();
		r = dd2->backsurf->Lock(NULL, &surface, DDLOCK_WAIT, NULL);
	}
	if (r != DD_OK) {
		return(NULL);
	}
	dd2->vram.ptr = (UINT8 *)surface.lpSurface;
	dd2->vram.yalign = surface.lPitch;
	return(&dd2->vram);
}

void dd2_bsurfunlock(DD2HDL dd2hdl) {

	DD2SURF			dd2;

	dd2 = (DD2SURF)dd2hdl;
	if ((dd2) && (dd2->backsurf)) {
		dd2->backsurf->Unlock(NULL);
	}
}

void dd2_blt(DD2HDL dd2hdl, const POINT *pt, const RECT *rect) {

	DD2SURF	dd2;
	POINT	clipt;
	RECT	scrn;

	dd2 = (DD2SURF)dd2hdl;
	if ((dd2) && (dd2->backsurf)) {
		if (pt) {
			clipt = *pt;
		}
		else {
			clipt.x = 0;
			clipt.y = 0;
		}
		ClientToScreen(dd2->hwnd, &clipt);
		scrn.left = clipt.x;
		scrn.top = clipt.y;
		scrn.right = clipt.x + rect->right - rect->left;
		scrn.bottom = clipt.y + rect->bottom - rect->top;
		if (dd2->primsurf->Blt(&scrn, dd2->backsurf, (RECT *)rect,
									DDBLT_WAIT, NULL) == DDERR_SURFACELOST) {
			dd2->backsurf->Restore();
			dd2->primsurf->Restore();
		}
	}
}

UINT16 dd2_get16pal(DD2HDL dd2hdl, RGB32 pal) {

	DD2SURF	dd2;

	dd2 = (DD2SURF)dd2hdl;
	if (dd2) {
		pal.d &= dd2->pal16.d;
		return((((UINT16)pal.p.g) << dd2->l16g) |
				(((UINT16)pal.p.r) << dd2->l16r) | (pal.p.b >> dd2->r16b));
	}
	else {
		return(0);
	}
}

