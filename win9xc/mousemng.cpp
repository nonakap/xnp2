#include	"compiler.h"
#include	"np2.h"
#include	"mousemng.h"


#define	MOUSEMNG_RANGE		128


typedef struct {
	SINT16	x;
	SINT16	y;
	UINT8	btn;
	UINT	flag;
} MOUSEMNG;

static	MOUSEMNG	mousemng;


UINT8 mousemng_getstat(SINT16 *x, SINT16 *y, int clear) {

	*x = mousemng.x;
	*y = mousemng.y;
	if (clear) {
		mousemng.x = 0;
		mousemng.y = 0;
	}
	return(mousemng.btn);
}


// ----

static void getmaincenter(POINT *cp) {

	RECT	rct;

	GetWindowRect(hWndMain, &rct);
	cp->x = (rct.right + rct.left) / 2;
	cp->y = (rct.bottom + rct.top) / 2;
}

static void mousecapture(BOOL capture) {

	LONG	style;
	POINT	cp;
	RECT	rct;

	style = GetClassLong(hWndMain, GCL_STYLE);
	if (capture) {
		ShowCursor(FALSE);
		getmaincenter(&cp);
		rct.left = cp.x - MOUSEMNG_RANGE;
		rct.right = cp.x + MOUSEMNG_RANGE;
		rct.top = cp.y - MOUSEMNG_RANGE;
		rct.bottom = cp.y + MOUSEMNG_RANGE;
		SetCursorPos(cp.x, cp.y);
		ClipCursor(&rct);
		style &= ~(CS_DBLCLKS);
	}
	else {
		ShowCursor(TRUE);
		ClipCursor(NULL);
		style |= CS_DBLCLKS;
	}
	SetClassLong(hWndMain, GCL_STYLE, style);
}

void mousemng_initialize(void) {

	ZeroMemory(&mousemng, sizeof(mousemng));
	mousemng.btn = uPD8255A_LEFTBIT | uPD8255A_RIGHTBIT;
	mousemng.flag = (1 << MOUSEPROC_SYSTEM);
}

void mousemng_sync(void) {

	POINT	p;
	POINT	cp;

	if ((!mousemng.flag) && (GetCursorPos(&p))) {
		getmaincenter(&cp);
		mousemng.x += (SINT16)((p.x - cp.x) / 2);
		mousemng.y += (SINT16)((p.y - cp.y) / 2);
		SetCursorPos(cp.x, cp.y);
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

