#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewsnd.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"


typedef struct {
const TCHAR	*str;
	UINT16	reg;
	UINT16	mask;
} FMSNDTBL;

static const FMSNDTBL fmsndtbl[] = {
		{_T("Sound-Board I"), 0, 0},
		{NULL, 0x0000, 0xffff},
		{NULL, 0x0010, 0x3f07},
		{NULL, 0x0020, 0x07e6},
		{NULL, 0x0030, 0x7777},
		{NULL, 0x0040, 0x7777},
		{NULL, 0x0050, 0x7777},
		{NULL, 0x0060, 0x7777},
		{NULL, 0x0070, 0x7777},
		{NULL, 0x0080, 0x7777},
		{NULL, 0x0090, 0x7777},
		{NULL, 0x00a0, 0x7777},
		{NULL, 0x00b0, 0x0077},
		{tchar_null, 0, 0},
		{NULL, 0x0100, 0xffff},
		{NULL, 0x0110, 0x0001},
		{NULL, 0x0130, 0x7777},
		{NULL, 0x0140, 0x7777},
		{NULL, 0x0150, 0x7777},
		{NULL, 0x0160, 0x7777},
		{NULL, 0x0170, 0x7777},
		{NULL, 0x0180, 0x7777},
		{NULL, 0x0190, 0x7777},
		{NULL, 0x01a0, 0x7777},
		{NULL, 0x01b0, 0x0077},
		{tchar_null, 0, 0},
		{_T("Sound-Board II"), 0, 0},
		{NULL, 0x0200, 0xffff},
		{NULL, 0x0220, 0x07e6},
		{NULL, 0x0230, 0x7777},
		{NULL, 0x0240, 0x7777},
		{NULL, 0x0250, 0x7777},
		{NULL, 0x0260, 0x7777},
		{NULL, 0x0270, 0x7777},
		{NULL, 0x0280, 0x7777},
		{NULL, 0x0290, 0x7777},
		{NULL, 0x02a0, 0x7777},
		{NULL, 0x02b0, 0x0077},
		{tchar_null, 0, 0},
		{NULL, 0x0230, 0x7777},
		{NULL, 0x0240, 0x7777},
		{NULL, 0x0250, 0x7777},
		{NULL, 0x0260, 0x7777},
		{NULL, 0x0270, 0x7777},
		{NULL, 0x0280, 0x7777},
		{NULL, 0x0290, 0x7777},
		{NULL, 0x02a0, 0x7777},
		{NULL, 0x02b0, 0x0077}};


static void viewsnd_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	int		x;
	LONG	y;
	UINT	pos;
const UINT8	*p;
	TCHAR	str[16];
	HFONT	hfont;
	UINT	reg;
	UINT16	mask;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, np2viewfont);
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_SND) {
			if (viewcmn_alloc(&view->buf1, 0x400)) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_SND;
				CopyMemory(view->buf1.ptr, opn.reg, 0x400);
				CopyMemory(view->buf1.ptr, &psg1.reg, 0x10);
				CopyMemory(((UINT8 *)view->buf1.ptr) + 0x200, &psg2.reg, 0x10);
			}
			viewcmn_putcaption(view);
		}
	}

	pos = view->pos;
	for (y=0; (y<rc->bottom) && (pos<NELEMENTS(fmsndtbl)); y+=16, pos++) {
		if (fmsndtbl[pos].str) {
			TextOut(hdc, 0, y, fmsndtbl[pos].str,
												lstrlen(fmsndtbl[pos].str));
		}
		else {
			reg = fmsndtbl[pos].reg;
			mask = fmsndtbl[pos].mask;

			wsprintf(str, _T("%04x"), reg & 0x1ff);
			TextOut(hdc, 0, y, str, 4);

			if (view->lock) {
				p = (UINT8 *)view->buf1.ptr;
				p += reg;
			}
			else if (reg & 0x1ff) {
				p = opn.reg + reg;
			}
			else if (reg & 0x200) {
				p = (UINT8 *)&psg2.reg;
			}
			else {
				p = (UINT8 *)&psg1.reg;
			}
			for (x=0; x<16; x++) {
				if (mask & 1) {
					str[0] = viewcmn_hex[*p >> 4];
					str[1] = viewcmn_hex[*p & 15];
					str[2] = 0;
					TextOut(hdc, (6 + x*3) * 8, y, str, 2);
				}
				p++;
				mask >>= 1;
			}
		}
	}
	DeleteObject(SelectObject(hdc, hfont));
}


LRESULT CALLBACK viewsnd_proc(NP2VIEW_T *view,
								HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_VIEWMODELOCK:
					view->lock ^= 1;
					viewmenu_lock(view);
					viewcmn_putcaption(view);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			break;

		case WM_PAINT:
			viewcmn_paint(view, 0x400000, viewsnd_paint);
			break;

	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewsnd_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	dst->type = VIEWMODE_SND;
	dst->maxline = NELEMENTS(fmsndtbl);
	dst->mul = 1;
	dst->pos = 0;
}

