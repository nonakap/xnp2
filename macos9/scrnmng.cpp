#include	"compiler.h"
#include	"np2.h"
#include	"scrnmng.h"


typedef struct {
	int				width;
	int				height;
	BOOL			exist;
	WindowPtr		hWnd;
	GWorldPtr		gw;
	PixMapHandle	pm;
	Rect			rect;
	GWorldPtr		gwp;
	GDHandle		hgd;
} _QDRAW, *QDRAW;


static	_QDRAW		qdraw;
static	SCRNSURF	scrnsurf;


static void changeclientsize(int width, int height) {

	QDRAW		qd;

	qd = &qdraw;
	if (!qd->exist) {
		return;
	}
	SizeWindow(qd->hWnd, width, height, TRUE);
}

void scrnmng_initialize(void) {

	QDRAW	qd;

	qd = &qdraw;
	qd->width = 640;
	qd->height = 400;
}

BOOL scrnmng_create(BYTE scrnmode) {

	QDRAW	qd;

	qd = &qdraw;
	SetRect(&qd->rect, 0, 0, 640, 480);
	if (NewGWorld(&qd->gw, 32, &qd->rect, NULL, NULL, useTempMem) == noErr) {
		qd->pm = GetGWorldPixMap(qd->gw);
		qd->exist = TRUE;
		qd->hWnd = hWndMain;
		return(SUCCESS);
	}
	else {
		(void)scrnmode;
		return(FAILURE);
	}
}

void scrnmng_destroy(void) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->exist) {
		qd->exist = FALSE;
		DisposeGWorld(qd->gw);
	}
}

void scrnmng_setwidth(int posx, int width) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->width != width) {
		qd->width = width;
		changeclientsize(width, qd->height);
	}
	(void)posx;
}

void scrnmng_setextend(int extend) {

	(void)extend;
}

void scrnmng_setheight(int posy, int height) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->height != height) {
		qd->height = height;
		changeclientsize(qd->width, height);
	}
	(void)posy;
}

const SCRNSURF *scrnmng_surflock(void) {

	QDRAW		qd;

	qd = &qdraw;
	if (!qd->exist) {
		return(NULL);
	}

	GetGWorld(&qd->gwp, &qd->hgd);
	LockPixels(qd->pm);
	SetGWorld(qd->gw, NULL);

	scrnsurf.ptr = (BYTE *)GetPixBaseAddr(qd->pm);
	scrnsurf.xalign = 4;
	scrnsurf.yalign = ((*qd->pm)->rowBytes) & 0x3fff;

	scrnsurf.width = qd->width;
	scrnsurf.height = qd->height;
	scrnsurf.extend = 0;
	scrnsurf.bpp = 32;
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	QDRAW	qd;
#if TARGET_API_MAC_CARBON
	GrafPtr		thePort;
	RgnHandle	theVisibleRgn;
#endif

	if (surf) {
		qd = &qdraw;
#if TARGET_API_MAC_CARBON
		theVisibleRgn = NewRgn();
		if (!EmptyRgn(GetPortVisibleRegion(GetWindowPort(qd->hWnd),
															theVisibleRgn))) {
			LockPortBits(GetWindowPort(qd->hWnd));
//			LockPixels(qd->pm);
			GetPort(&thePort);
			SetPortWindowPort(qd->hWnd);
			CopyBits((BitMap*)(*qd->pm),
							GetPortBitMapForCopyBits(GetWindowPort(qd->hWnd)),
							&qd->rect, &qd->rect, srcCopy, theVisibleRgn);
			SetPort(thePort);
//			UnlockPixels(qd->pm);
			UnlockPortBits(GetWindowPort(qd->hWnd));
		}
		DisposeRgn(theVisibleRgn);
#else
		CopyBits((BitMap *)(*qd->pm), &(qd->hWnd->portBits), &qd->rect,
										&qd->rect, srcCopy, qd->hWnd->visRgn);
#endif
		UnlockPixels(qd->pm);
		SetGWorld(qd->gwp, qd->hgd);
	}
}

