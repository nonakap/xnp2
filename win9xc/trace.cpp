#include	"compiler.h"
#include	<stdarg.h>
#include	"strres.h"
#include	"dosio.h"
#include	"ini.h"


#ifdef TRACE

// #define FILEBUFSIZE			(1 << 20)
// #define FILELASTBUFONLY

#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

#define	IDC_VIEW		(WM_USER + 100)
#define	VIEW_BUFFERSIZE	4096
#define	VIEW_FGCOLOR	0x000000
#define	VIEW_BGCOLOR	0xffffff
#define	VIEW_TEXT		"‚l‚r ƒSƒVƒbƒN"
#define	VIEW_SIZE		12

typedef struct {
	UINT8	en;
	FILEH	fh;
	HWND	hwnd;
} TRACEWIN;

typedef struct {
	int		posx;
	int		posy;
	int		width;
	int		height;
} TRACECFG;

extern	HINSTANCE	hInst;
extern	HINSTANCE	hPrev;

enum {
	IDM_TRACE1		= 3300,
	IDM_TRACE2,
	IDM_TRACEEN,
	IDM_TRACEFH,
	IDM_TRACECL
};

static const TCHAR ProgTitle[] = _T("console");
static const TCHAR ClassName[] = _T("TRACE-console");
static const TCHAR ClassEdit[] = _T("EDIT");
static const TCHAR trace1[] = _T("TRACE");
static const TCHAR trace2[] = _T("VERBOSE");
static const TCHAR traceen[] = _T("Enable");
static const TCHAR tracefh[] = _T("File out");
static const TCHAR tracecl[] = _T("Clear");
static const TCHAR fontface[] = _T(VIEW_TEXT);
static const TCHAR crlf[] = _T("\r\n");

static	TRACEWIN	tracewin;
static	HWND		hView = NULL;
static	HFONT		hfView = NULL;
static	HBRUSH		hBrush = NULL;
static	int			viewpos;
static	int			viewleng;
static	TCHAR		viewbuf[VIEW_BUFFERSIZE * 2];
static	TRACECFG	tracecfg;
static	int			devpos;
static	char		devstr[256];

static const TCHAR np2trace[] = _T("np2trace.ini");
static const TCHAR inititle[] = _T("TRACE");
static const PFTBL initbl[4] = {
			PFVAL("posx",	PFTYPE_SINT32,	&tracecfg.posx),
			PFVAL("posy",	PFTYPE_SINT32,	&tracecfg.posy),
			PFVAL("width",	PFTYPE_SINT32,	&tracecfg.width),
			PFVAL("height",	PFTYPE_SINT32,	&tracecfg.height)};

static void View_ScrollToBottom(HWND hWnd) {

	int		MinPos;
	int		MaxPos;

	GetScrollRange(hWnd, SB_VERT, &MinPos, &MaxPos);
	PostMessage(hWnd, EM_LINESCROLL, 0, MaxPos);
}

static void View_ClrString(void) {

	viewpos = 0;
	viewleng = 0;
	viewbuf[0] = '\0';
	SetWindowText(hView, viewbuf);
}

static void View_AddString(const TCHAR *string) {

	int		slen;
	int		vpos;
	int		vlen;
	TCHAR	c;

	slen = lstrlen(string);
	if ((slen == 0) || ((slen + 3) > VIEW_BUFFERSIZE)) {
		return;
	}
	vpos = viewpos;
	vlen = viewleng;
	if ((vpos + vlen + slen + 3) > (VIEW_BUFFERSIZE * 2)) {
		while(vlen > 0) {
			vlen--;
			c = viewbuf[vpos++];
			if ((c == 0x0a) && ((vlen + slen + 3) <= VIEW_BUFFERSIZE)) {
				break;
			}
		}
		if (vpos >= VIEW_BUFFERSIZE) {
			if (vlen) {
				CopyMemory(viewbuf, viewbuf + vpos, vlen * sizeof(TCHAR));
			}
			vpos = 0;
			viewpos = 0;
		}
	}
	CopyMemory(viewbuf + vpos + vlen, string, slen * sizeof(TCHAR));
	vlen += slen;
	viewbuf[vpos + vlen + 0] = '\r';
	viewbuf[vpos + vlen + 1] = '\n';
	viewbuf[vpos + vlen + 2] = '\0';
	viewleng = vlen + 2;
	SetWindowText(hView, viewbuf + vpos);
	View_ScrollToBottom(hView);
}


// ----

#if defined(FILEBUFSIZE)
static	TCHAR	filebuf[FILEBUFSIZE];
static	UINT32	filebufpos;
#endif

static void trfh_close(void) {

	FILEH	fh;

	fh = tracewin.fh;
	tracewin.fh = FILEH_INVALID;
	if (fh != FILEH_INVALID) {
#if defined(FILEBUFSIZE)
		UINT size = filebufpos & (FILEBUFSIZE - 1);
#if defined(FILELASTBUFONLY)
		if (filebufpos >= FILEBUFSIZE) {
			file_write(fh, filebuf + size, FILEBUFSIZE - size);
		}
#endif
		if (size) {
			file_write(fh, filebuf, size);
		}
#endif
		file_close(fh);
	}
}

static void trfh_open(const TCHAR *fname) {

	trfh_close();
	tracewin.fh = file_create(fname);
#if defined(FILEBUFSIZE)
	filebufpos = 0;
#endif
}

static void trfh_add(const TCHAR *buf) {

	UINT	size;

	size = lstrlen(buf);
#if defined(FILEBUFSIZE)
	while(size) {
		UINT pos = filebufpos & (FILEBUFSIZE - 1);
		UINT rem = FILEBUFSIZE - pos;
		if (size >= rem) {
			CopyMemory(filebuf + pos, buf, rem);
			filebufpos += rem;
			buf += rem;
			size -= rem;
#if !defined(FILELASTBUFONLY)
			file_write(tracewin.fh, filebuf, FILEBUFSIZE);
#endif
		}
		else {
			CopyMemory(filebuf + pos, buf, size);
			filebufpos += size;
			break;
		}
	}
#else
	file_write(tracewin.fh, buf, size);
#endif
}


// ----

static LRESULT CALLBACK traceproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	RECT	rc;
	HMENU	hmenu;

	switch (msg) {
		case WM_CREATE:
			hmenu = GetSystemMenu(hWnd, FALSE);
			InsertMenu(hmenu, 0, MF_BYPOSITION | MF_STRING,
														IDM_TRACE1, trace1);
			InsertMenu(hmenu, 1, MF_BYPOSITION | MF_STRING,
														IDM_TRACE2, trace2);
			InsertMenu(hmenu, 2, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
			InsertMenu(hmenu, 3, MF_BYPOSITION | MF_STRING,
														IDM_TRACEEN, traceen);
			InsertMenu(hmenu, 4, MF_BYPOSITION | MF_STRING,
														IDM_TRACEFH, tracefh);
			InsertMenu(hmenu, 5, MF_BYPOSITION | MF_STRING,
														IDM_TRACECL, tracecl);
			InsertMenu(hmenu, 6, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);

			CheckMenuItem(hmenu, IDM_TRACE1,
								(tracewin.en & 1)?MF_CHECKED:MF_UNCHECKED);
			CheckMenuItem(hmenu, IDM_TRACE2,
								(tracewin.en & 2)?MF_CHECKED:MF_UNCHECKED);
			CheckMenuItem(hmenu, IDM_TRACEEN,
								(tracewin.en & 4)?MF_CHECKED:MF_UNCHECKED);

			GetClientRect(hWnd, &rc);
			hView = CreateWindowEx(WS_EX_CLIENTEDGE,
							ClassEdit, NULL,
							WS_CHILD | WS_VISIBLE | ES_READONLY | ES_LEFT |
							ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL,
							0, 0, rc.right, rc.bottom,
							hWnd, (HMENU)IDC_VIEW, hInst, NULL);
			if (!hView) {
				break;
			}
			SendMessage(hView, EM_SETLIMITTEXT, (WPARAM)VIEW_BUFFERSIZE, 0);

			hfView = CreateFont(VIEW_SIZE, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, fontface);
			if (!hfView) {
				break;
			}
			SendMessage(hView, WM_SETFONT, (WPARAM)hfView,
												MAKELPARAM(TRUE, 0));
			hBrush = CreateSolidBrush(VIEW_BGCOLOR);
			SetFocus(hView);
			return(TRUE);

		case WM_SYSCOMMAND:
			switch(wp) {
				case IDM_TRACE1:
					tracewin.en ^= 1;
					hmenu = GetSystemMenu(hWnd, FALSE);
					CheckMenuItem(hmenu, IDM_TRACE1,
								(tracewin.en & 1)?MF_CHECKED:MF_UNCHECKED);
					break;

				case IDM_TRACE2:
					tracewin.en ^= 2;
					hmenu = GetSystemMenu(hWnd, FALSE);
					CheckMenuItem(hmenu, IDM_TRACE2,
								(tracewin.en & 2)?MF_CHECKED:MF_UNCHECKED);
					break;

				case IDM_TRACEEN:
					tracewin.en ^= 4;
					hmenu = GetSystemMenu(hWnd, FALSE);
					CheckMenuItem(hmenu, IDM_TRACEEN,
								(tracewin.en & 4)?MF_CHECKED:MF_UNCHECKED);
					break;

				case IDM_TRACEFH:
					if (tracewin.fh != FILEH_INVALID) {
						trfh_close();
					}
					else {
						trfh_open(_T("traceout.txt"));
					}
					hmenu = GetSystemMenu(hWnd, FALSE);
					CheckMenuItem(hmenu, IDM_TRACEFH,
									(tracewin.fh != FILEH_INVALID)?
													MF_CHECKED:MF_UNCHECKED);
					break;

				case IDM_TRACECL:
					View_ClrString();
					break;

				default:
					return(DefWindowProc(hWnd, msg, wp, lp));
			}
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE))) {
				GetWindowRect(hWnd, &rc);
				tracecfg.posx = rc.left;
				tracecfg.posy = rc.top;
			}
			break;

		case WM_SIZE:							// window resize
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
										(WS_MAXIMIZE | WS_MINIMIZE))) {
				GetWindowRect(hWnd, &rc);
				tracecfg.width = rc.right - rc.left;
				tracecfg.height = rc.bottom - rc.top;
			}
			MoveWindow(hView, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
			View_ScrollToBottom(hView);
			break;

		case WM_SETFOCUS:
			SetFocus(hView);
			return(0L);

		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			SetTextColor((HDC)wp, VIEW_FGCOLOR);
			SetBkColor((HDC)wp, VIEW_BGCOLOR);
			return((LRESULT)hBrush);

		case WM_CLOSE:
			break;

		case WM_DESTROY:
			if (hBrush) {
				DeleteObject(hBrush);
			}
			if (hfView) {
				DeleteObject(hfView);
			}
			break;
#if 0
		case WM_ENTERSIZEMOVE:
			winloc_movingstart();
			break;

		case WM_MOVING:
			winloc_movingproc((RECT *)lp);
			break;

		case WM_ERASEBKGND:
			return(FALSE);
#endif
		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}


// ----

void trace_init(void) {

	HWND	hwnd;

	ZeroMemory(&tracewin, sizeof(tracewin));
	if (!hPrev) {
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = traceproc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = (LPCTSTR)ClassName;
		if (!RegisterClass(&wc)) {
			return;
		}
	}

#if 1
	tracewin.en = 4;
#else
	tracewin.en = 0;
#endif
	tracewin.fh = FILEH_INVALID;

	tracecfg.posx = CW_USEDEFAULT;
	tracecfg.posy = CW_USEDEFAULT;
	tracecfg.width = CW_USEDEFAULT;
	tracecfg.height = CW_USEDEFAULT;
	ini_read(file_getcd(np2trace), inititle, initbl, 4);

	hwnd = CreateWindowEx(WS_EX_CONTROLPARENT,
							ClassName, ProgTitle,
							WS_OVERLAPPEDWINDOW,
							tracecfg.posx, tracecfg.posy,
							tracecfg.width, tracecfg.height,
							NULL, NULL, hInst, NULL);
	tracewin.hwnd = hwnd;
	if (hwnd == NULL) {
		return;
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void trace_term(void) {

	if (tracewin.fh != FILEH_INVALID) {
		trfh_close();
	}
	if (tracewin.hwnd) {
		DestroyWindow(tracewin.hwnd);
		tracewin.hwnd = NULL;
		ini_write(file_getcd(np2trace), inititle, initbl, 4);
	}
}

void trace_fmt(const char *fmt, ...) {

	BOOL	en;
	va_list	ap;
	TCHAR	buf[0x1000];

	en = (tracewin.en & 1) &&
		((tracewin.en & 4) || (tracewin.fh != FILEH_INVALID));
	if (en) {
		va_start(ap, fmt);
#if defined(_UNICODE)
		TCHAR cnvfmt[0x800];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fmt, -1,
												cnvfmt, NELEMENTS(cnvfmt));
		vswprintf(buf, cnvfmt, ap);
#else
		vsprintf(buf, fmt, ap);
#endif
		va_end(ap);
		if ((tracewin.en & 4) && (hView)) {
			View_AddString(buf);
		}
		if (tracewin.fh != FILEH_INVALID) {
			trfh_add(buf);
			trfh_add(crlf);
		}
	}
}

void trace_fmt2(const char *fmt, ...) {

	BOOL	en;
	va_list	ap;
	TCHAR	buf[0x1000];

	en = (tracewin.en & 2) &&
		((tracewin.en & 4) || (tracewin.fh != FILEH_INVALID));
	if (en) {
		va_start(ap, fmt);
#if defined(_UNICODE)
		TCHAR cnvfmt[0x800];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fmt, -1,
												cnvfmt, NELEMENTS(cnvfmt));
		vswprintf(buf, cnvfmt, ap);
#else
		vsprintf(buf, fmt, ap);
#endif
		va_end(ap);
		if ((tracewin.en & 4) && (hView)) {
			View_AddString(buf);
		}
		if (tracewin.fh != FILEH_INVALID) {
			trfh_add(buf);
			trfh_add(crlf);
		}
	}
}
#endif

