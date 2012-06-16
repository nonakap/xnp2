#include	"compiler.h"
#include	"np2.h"
#include	"scrnmng.h"
#include	"toolwin.h"


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

#define	WINDOWTITLEOFFSET 22

static void changeclientsize(int width, int height, BYTE mode) {

	QDRAW		qd;
    BYTE		opentoolwin = np2oscfg.toolwin;

	qd = &qdraw;
	if (!qd->exist) {
		return;
	}
    if (!(mode & SCRNMODE_FULLSCREEN)) {
        if (opentoolwin) {
            toolwin_close();
        }
        if (!(mode & SCRNMODE_ROTATE)) {
            qd->width = width;
            qd->height = height;
        }
        else {
            qd->width = height;
            qd->height = width;
        }
        SizeWindow(qd->hWnd, qd->width, qd->height, TRUE);
        SetRect(&qd->rect, 0, 0, qd->width, qd->height);
        if (opentoolwin) {
            toolwin_open();
        }
    }
    else {
        GetWindowBounds(qd->hWnd, kWindowContentRgn, &qd->rect);
    }
}

#if defined(SUPPORT_16BPP)
UINT16 scrnmng_makepal16(RGB32 pal32) {
//win9xのをちょこっと改造(tk800)
	RGB32	pal;

	pal.d = pal32.d & 0xF8F8F8;
	return((UINT16)((pal.p.g << 2) +
						(pal.p.r << 7) + (pal.p.b >> 3)));
}
#else
UINT16 scrnmng_makepal16(RGB32 pal32) {
    return(0);
}
#endif

//ディスプレイの色深度を返す(tk800)
int	scrnmng_getbpp(void) {
    return(CGDisplayBitsPerPixel(kCGDirectMainDisplay));
}

void scrnmng_initialize(void) {

	QDRAW	qd;

	qd = &qdraw;
	qd->width = 640;
	qd->height = 400;
}

BOOL scrnmng_create(BYTE mode) {

	QDRAW	qd;

	qd = &qdraw;
//GWorldの代わりに直接ウィンドウバッファを設定(tk800)
    GrafPtr		dstport;
    dstport = GetWindowPort(hWndMain);
    if (dstport) {
        qd->pm = GetPortPixMap(dstport);
        qd->exist = TRUE;
        qd->hWnd = hWndMain;
        changeclientsize(qd->width, qd->height, mode);
        return(SUCCESS);
    }
	else {
		(void)mode;
		return(FAILURE);
	}
}

void scrnmng_destroy(void) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->exist) {
		qd->exist = FALSE;
	}
}

void scrnmng_setwidth(int posx, int width) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->width != width) {
		qd->width = width;
		changeclientsize(width, qd->height, scrnmode);
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
		changeclientsize(qd->width, height, scrnmode);
	}
	(void)posy;
}

const SCRNSURF *scrnmng_surflock(void) {

	QDRAW		qd;

	qd = &qdraw;
	if (!qd->exist) {
		return(NULL);
	}

	scrnsurf.width = qd->width;
	scrnsurf.height = qd->height;
//描画位置をウィンドウバーの下に設定(tk800) 
    LockPortBits(GetWindowPort(hWndMain));//こうしないと描画位置がおかしくなる(tk800)
	LockPixels(qd->pm);
    long	rowbyte = GetPixRowBytes(qd->pm);
	scrnsurf.bpp = scrnmng_getbpp();
	if (!(scrnmode & SCRNMODE_ROTATE)) {
		scrnsurf.ptr = (BYTE *)GetPixBaseAddr(qd->pm) + rowbyte*WINDOWTITLEOFFSET;
		scrnsurf.xalign = scrnsurf.bpp >> 3;
		scrnsurf.yalign = rowbyte;
	}
	else if (!(scrnmode & SCRNMODE_ROTATEDIR)) {
		scrnsurf.ptr = (BYTE *)GetPixBaseAddr(qd->pm) + rowbyte*WINDOWTITLEOFFSET;
		scrnsurf.ptr += (scrnsurf.width - 1) * rowbyte;
		scrnsurf.xalign = 0 - rowbyte;
		scrnsurf.yalign = scrnsurf.bpp >> 3;
	}
	else {
		scrnsurf.ptr = (BYTE *)GetPixBaseAddr(qd->pm) + rowbyte*WINDOWTITLEOFFSET;
		scrnsurf.ptr += (scrnsurf.height - 1) * (scrnsurf.bpp >> 3);
		scrnsurf.xalign = rowbyte;
		scrnsurf.yalign = 0 - (scrnsurf.bpp >> 3);
    }
	if (scrnmode & SCRNMODE_FULLSCREEN) {
		scrnsurf.ptr += ((CGDisplayPixelsWide(kCGDirectMainDisplay)-qd->width)/2+1)*(scrnsurf.bpp >> 3);
	}
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	QDRAW	qd;

	if (surf) {
		qd = &qdraw;

//画面を更新するようWindow Serverに指示(tk800)
        GrafPtr		dstport;
        dstport = GetWindowPort(qd->hWnd);
        QDAddRectToDirtyRegion(dstport, &qd->rect);
		UnlockPixels(qd->pm);
        UnlockPortBits(dstport);
	}
}
