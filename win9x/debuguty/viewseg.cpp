#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"viewseg.h"
#include	"cpucore.h"


static void set_viewseg(HWND hwnd, NP2VIEW_T *view, UINT16 seg) {

	if (view->seg != seg) {
		view->seg = seg;
		InvalidateRect(hwnd, NULL, TRUE);
	}
}


static void viewseg_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	int		x;
	LONG	y;
	UINT32	mad;
	UINT32	off;
	UINT8	*p;
	UINT8	buf[16];
	TCHAR	str[16];
	HFONT	hfont;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, np2viewfont);
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	off = (view->pos) << 4;
	mad = (((UINT32)view->seg) << 4) + off;

	if (view->lock) {
		if ((view->buf1.type != ALLOCTYPE_SEG) ||
			(view->buf1.arg != view->seg)) {
			if (viewcmn_alloc(&view->buf1, 0x10000)) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_SEG;
				view->buf1.arg = view->seg;
				viewmem_read(&view->dmem, view->buf1.arg << 4,
											(UINT8 *)view->buf1.ptr, 0x10000);
			}
			viewcmn_putcaption(view);
		}
	}

	for (y=0; y<rc->bottom && off<0x10000; y+=16, off+=16) {
		wsprintf(str, _T("%04x:%04x"), view->seg, off);
		TextOut(hdc, 0, y, str, 9);
		if (view->lock) {
			p = (UINT8 *)view->buf1.ptr;
			p += off;
		}
		else {
			p = buf;
			viewmem_read(&view->dmem, mad, buf, 16);
			mad += 16;
		}
		for (x=0; x<16; x++) {
			str[0] = viewcmn_hex[*p >> 4];
			str[1] = viewcmn_hex[*p & 15];
			str[2] = 0;
			p++;
			TextOut(hdc, (10 + x*3)*8, y, str, 2);
		}
	}
	DeleteObject(SelectObject(hdc, hfont));
}


LRESULT CALLBACK viewseg_proc(NP2VIEW_T *view,
								HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_SEGCS:
					set_viewseg(hwnd, view, CPU_CS);
					break;

				case IDM_SEGDS:
					set_viewseg(hwnd, view, CPU_DS);
					break;

				case IDM_SEGES:
					set_viewseg(hwnd, view, CPU_ES);
					break;

				case IDM_SEGSS:
					set_viewseg(hwnd, view, CPU_SS);
					break;

				case IDM_SEGTEXT:
					set_viewseg(hwnd, view, 0xa000);
					break;

				case IDM_VIEWMODELOCK:
					view->lock ^= 1;
					viewmenu_lock(view);
					viewcmn_putcaption(view);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			break;

		case WM_PAINT:
			viewcmn_paint(view, 0x400000, viewseg_paint);
			break;

	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewseg_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	if (src) {
		switch(src->type) {
			case VIEWMODE_SEG:
				dst->seg = dst->seg;
				break;

			case VIEWMODE_1MB:
				if (dst->pos < 0x10000) {
					dst->seg = (UINT16)dst->pos;
				}
				else {
					dst->seg = 0xffff;
				}
				break;

			case VIEWMODE_ASM:
				dst->seg = src->seg;
				break;

			default:
				src = NULL;
				break;
		}
	}
	if (!src) {
		dst->seg = 0;
	}
	dst->type = VIEWMODE_SEG;
	dst->maxline = 0x1000;
	dst->mul = 1;
	dst->pos = 0;
}

