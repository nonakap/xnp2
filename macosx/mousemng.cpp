#include	"compiler.h"
#include	"np2.h"
#include	"mousemng.h"


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

	CGPoint	pt;

	pt.x = (float)cp->h;
	pt.y = (float)cp->v;
	CGWarpMouseCursorPosition(pt);
}

static void getmaincenter(Point *cp) {

	Rect	rct;

	GetWindowBounds(hWndMain, kWindowContentRgn, &rct);
	cp->h = (rct.right + rct.left) / 2;
	cp->v = (rct.bottom + rct.top) / 2;
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

void mousemng_callback(HIPoint delta) {

	Point	cp;

	if (!mousemng.flag) {
        getmaincenter(&cp);
        mousemng.x += (SINT16)delta.x;
        mousemng.y += (SINT16)delta.y;
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
