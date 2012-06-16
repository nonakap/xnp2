#include	"compiler.h"
#if TARGET_API_MAC_CARBON
#include	<CGRemoteOperation.h>
#else
#include	<CursorDevices.h>
#endif
#include	"np2.h"
#include	"mousemng.h"


#if !TARGET_API_MAC_CARBON

typedef struct {
	SINT16	x;
	SINT16	y;
	BYTE	btn;
	UINT	flag;
} MOUSEMNG;

static	MOUSEMNG	mousemng;


BYTE mousemng_getstat(SINT16 *x, SINT16 *y, int clear) {

	*x = mousemng.x;
	*y = mousemng.y;
	if (clear) {
		mousemng.x = 0;
		mousemng.y = 0;
	}
	return(mousemng.btn);
}


// ----

static void SetMouse(const Point *cp) {

#if TARGET_API_MAC_CARBON

	CGPoint	pt;

	pt.x = (float)cp->h;
	pt.y = (float)cp->v;
	CGWarpMouseCursorPosition(pt);

#else
	CursorDevice	*curdev;
	Point			pt;
	WindowPtr		saveport;

	curdev = NULL;
	CursorDeviceNextDevice(&curdev);
	if (curdev != NULL) {
		pt.h = cp->h;
		pt.v = cp->v;
		GetPort(&saveport);
		SetPort(hWndMain);
		LocalToGlobal(&pt);
		SetPort(saveport);
		CursorDeviceMoveTo(curdev, pt.h, pt.v);
	}
#endif
}

static void getmaincenter(Point *cp) {

#if TARGET_API_MAC_CARBON
	Rect	rct;

	GetPortBounds(GetWindowPort(hWndMain), &rct);
	cp->h = (rct.right + rct.left) / 2;
	cp->v = (rct.bottom + rct.top) / 2;
#else
	cp->h = 320;
	cp->v = 200;
#endif
}

static void mousecapture(BOOL capture) {

	Point	cp;

	if (capture) {
		HideCursor();
		getmaincenter(&cp);
		SetMouse(&cp);
	}
	else {
		ShowCursor();
	}
}

void mousemng_initialize(void) {

	ZeroMemory(&mousemng, sizeof(mousemng));
	mousemng.btn = uPD8255A_LEFTBIT | uPD8255A_RIGHTBIT;
	mousemng.flag = (1 << MOUSEPROC_SYSTEM);
}

void mousemng_callback(void) {

	Point	p;
	Point	cp;

	if (!mousemng.flag) {
#if TARGET_API_MAC_CARBON
		SetPortWindowPort(hWndMain);
#else
		SetPort(hWndMain);
#endif
		GetMouse(&p);
		getmaincenter(&cp);
		mousemng.x += (SINT16)((p.h - cp.h) / 2);
		mousemng.y += (SINT16)((p.v - cp.v) / 2);
		SetMouse(&cp);
	}
}

BOOL mousemng_buttonevent(UINT event) {

	if (!mousemng.flag) {
		switch(event) {
			case MOUSEMNG_LEFTDOWN:
				mousemng.btn &= ~(uPD8255A_LEFTBIT);
				break;

			case MOUSEMNG_LEFTUP:
				mousemng.btn |= uPD8255A_LEFTBIT;
				break;

			case MOUSEMNG_RIGHTDOWN:
				mousemng.btn &= ~(uPD8255A_RIGHTBIT);
				break;

			case MOUSEMNG_RIGHTUP:
				mousemng.btn |= uPD8255A_RIGHTBIT;
				break;
		}
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}

void mousemng_enable(UINT proc) {

	UINT	bit;

	bit = 1 << proc;
	if (mousemng.flag & bit) {
		mousemng.flag &= ~bit;
		if (!mousemng.flag) {
			mousecapture(TRUE);
		}
	}
}

void mousemng_disable(UINT proc) {

	if (!mousemng.flag) {
		mousecapture(FALSE);
	}
	mousemng.flag |= (1 << proc);
}

void mousemng_toggle(UINT proc) {

	if (!mousemng.flag) {
		mousecapture(FALSE);
	}
	mousemng.flag ^= (1 << proc);
	if (!mousemng.flag) {
		mousecapture(TRUE);
	}
}

#else

BYTE mousemng_getstat(SINT16 *x, SINT16 *y, int clear) {

	*x = 0;
	*y = 0;
	(void)clear;
	return(0xa0);
}

#endif

