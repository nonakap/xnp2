#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"winloc.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"ini.h"
#include	"dd2.h"
#include	"subwind.h"
#include	"np2class.h"
#include	"keydisp.h"
#include	"memdbg32.h"
#include	"softkbd.h"


extern WINLOCEX np2_winlocexallwin(HWND base);

static void wintypechange(HWND hWnd, UINT8 type) {

	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, hWnd);
	np2class_windowtype(hWnd, type);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}


// ---- key display

#if defined(SUPPORT_KEYDISP)
enum {
	KDISPCFG_FM		= 0x00,
	KDISPCFG_MIDI	= 0x80
};

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
} KDISPWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	mode;
	UINT8	type;
} KDISPCFG;

static	KDISPWIN	kdispwin;
static	KDISPCFG	kdispcfg;

static const TCHAR kdispclass[] = _T("NP2-KeyDispWin");

static const UINT32 kdisppal[KEYDISP_PALS] =
									{0x00000000, 0xffffffff, 0xf9ff0000};

static const OEMCHAR kdispapp[] = OEMTEXT("Key Display");
static const PFTBL kdispini[] = {
				PFVAL("WindposX", PFTYPE_SINT32,	&kdispcfg.posx),
				PFVAL("WindposY", PFTYPE_SINT32,	&kdispcfg.posy),
				PFVAL("keydmode", PFTYPE_UINT8,		&kdispcfg.mode),
				PFVAL("windtype", PFTYPE_BOOL,		&kdispcfg.type)};


static UINT8 kdgetpal8(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] >> 24);
	}
	return(0);
}

static UINT32 kdgetpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] & 0xffffff);
	}
	return(0);
}

static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32) {

	return(dd2_get16pal((DD2HDL)self->userdata, pal32));
}

static void kddrawkeys(HWND hWnd, BOOL redraw) {

	RECT	rect;
	RECT	draw;
	CMNVRAM	*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(KEYDISP_WIDTH, rect.right - rect.left);
	draw.bottom = min(KEYDISP_HEIGHT, rect.bottom - rect.top);
	if ((draw.right <= 0) || (draw.bottom <= 0)) {
		return;
	}
	vram = dd2_bsurflock(kdispwin.dd2hdl);
	if (vram) {
		keydisp_paint(vram, redraw);
		dd2_bsurfunlock(kdispwin.dd2hdl);
		dd2_blt(kdispwin.dd2hdl, NULL, &draw);
	}
}

static void kdsetwinsize(void) {

	int			width;
	int			height;
	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, kdispwin.hwnd);
	keydisp_getsize(&width, &height);
	winloc_setclientsize(kdispwin.hwnd, width, height);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

static void kdsetdispmode(UINT8 mode) {

	HMENU	hmenu;

	keydisp_setmode(mode);
	hmenu = np2class_gethmenu(kdispwin.hwnd);
	CheckMenuItem(hmenu, IDM_KDISPFM,
					((mode == KEYDISP_MODEFM)?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(hmenu, IDM_KDISPMIDI,
					((mode == KEYDISP_MODEMIDI)?MF_CHECKED:MF_UNCHECKED));
}

static void kdpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	kddrawkeys(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static void kdopenpopup(HWND hWnd, LPARAM lp) {

	HMENU	hMenu;
	POINT	pt;

	hMenu = CreatePopupMenu();
	menu_addmenu(hMenu, 0, np2class_gethmenu(hWnd), FALSE);
	menu_addmenures(hMenu, -1, IDR_CLOSE, TRUE);
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenu);
}

static LRESULT CALLBACK kdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			np2class_windowtype(hWnd, (kdispcfg.type & 1) << 1);
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_KDISPFM:
					kdispcfg.mode = KDISPCFG_FM;
					sysmng_update(SYS_UPDATEOSCFG);
					kdsetdispmode(KEYDISP_MODEFM);
					break;

				case IDM_KDISPMIDI:
					kdispcfg.mode = KDISPCFG_MIDI;
					sysmng_update(SYS_UPDATEOSCFG);
					kdsetdispmode(KEYDISP_MODEMIDI);
					break;

				case IDM_CLOSE:
					return(SendMessage(hWnd, WM_CLOSE, 0, 0));
			}
			break;

		case WM_PAINT:
			kdpaintmsg(hWnd);
			break;
#if 0
		case WM_ACTIVATE:
			if (LOWORD(wp) != WA_INACTIVE) {
				keydisps_reload();
				kddrawkeys(hWnd, TRUE);
			}
			break;
#endif
		case WM_LBUTTONDOWN:
			if (kdispcfg.type & 1) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_RBUTTONDOWN:
			kdopenpopup(hWnd, lp);
			break;

		case WM_LBUTTONDBLCLK:
			kdispcfg.type ^= 1;
			wintypechange(hWnd, (kdispcfg.type & 1) << 1);
			sysmng_update(SYS_UPDATEOSCFG);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(g_hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			winlocex_destroy(kdispwin.wlex);
			kdispwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(kdispwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(kdispwin.wlex);
			kdispwin.wlex = NULL;
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				kdispcfg.posx = rc.left;
				kdispcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			sysmenu_setkeydisp(0);
			sysmng_update(SYS_UPDATEOSCFG);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			dd2_release(kdispwin.dd2hdl);
			kdispwin.hwnd = NULL;
			kdsetdispmode(KEYDISP_MODENONE);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}

BOOL kdispwin_initialize(HINSTANCE hInstance) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = kdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = NP2GWLP_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_KEYDISP);
	wc.lpszClassName = kdispclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	keydisp_initialize();
	return(SUCCESS);
}

void kdispwin_create(HINSTANCE hInstance) {

	TCHAR		szCaption[128];
	HWND		hwnd;
	UINT8		mode;
	CMNPALFN	palfn;

	if (kdispwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&kdispwin, sizeof(kdispwin));

	loadstringresource(LOWORD(IDS_CAPTION_KEYDISP),
										szCaption, NELEMENTS(szCaption));
	hwnd = CreateWindow(kdispclass, szCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						kdispcfg.posx, kdispcfg.posy,
						KEYDISP_WIDTH, KEYDISP_HEIGHT,
						NULL, NULL, hInstance, NULL);
	kdispwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto kdcre_err1;
	}
	switch(kdispcfg.mode) {
		case KDISPCFG_FM:
		default:
			mode = KEYDISP_MODEFM;
			break;

		case KDISPCFG_MIDI:
			mode = KEYDISP_MODEMIDI;
			break;
	}
	kdsetdispmode(mode);
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	kdispwin.dd2hdl = dd2_create(hwnd, KEYDISP_WIDTH, KEYDISP_HEIGHT);
	if (kdispwin.dd2hdl == NULL) {
		goto kdcre_err2;
	}
	palfn.get8 = kdgetpal8;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = kdcnvpal16;
	palfn.userdata = (long)kdispwin.dd2hdl;
	keydisp_setpal(&palfn);
	kdispwin_draw(0);
	SetForegroundWindow(g_hWndMain);
	return;

kdcre_err2:
	DestroyWindow(hwnd);

kdcre_err1:
	sysmenu_setkeydisp(0);
	sysmng_update(SYS_UPDATEOSCFG);
}

void kdispwin_destroy(void) {

	if (kdispwin.hwnd != NULL) {
		DestroyWindow(kdispwin.hwnd);
	}
}

HWND kdispwin_gethwnd(void) {

	return(kdispwin.hwnd);
}

void kdispwin_draw(UINT8 cnt) {

	UINT8	flag;

	if (kdispwin.hwnd) {
		if (!cnt) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			kdsetwinsize();
		}
		kddrawkeys(kdispwin.hwnd, FALSE);
	}
}

void kdispwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = CW_USEDEFAULT;
	kdispcfg.posy = CW_USEDEFAULT;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, kdispapp, kdispini, NELEMENTS(kdispini));
}

void kdispwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, kdispapp, kdispini, NELEMENTS(kdispini));
}
#endif


// ---- memdbg

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
	int			width;
	int			height;
} MDBGWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	type;
} MDBGCFG;

static	MDBGWIN		mdbgwin;
static	MDBGCFG		mdbgcfg;

static const TCHAR mdbgtitle[] = _T("Memory Map");
static const TCHAR mdbgclass[] = _T("NP2-MemDbgWin");

#if defined(OSLANG_UTF8)
static const OEMCHAR mdbgapp[] = OEMTEXT("Memory Map");
#else
#define	mdbgapp		mdbgtitle
#endif
static const PFTBL mdbgini[] = {
				PFVAL("WindposX", PFTYPE_SINT32,	&mdbgcfg.posx),
				PFVAL("WindposY", PFTYPE_SINT32,	&mdbgcfg.posy),
				PFVAL("windtype", PFTYPE_BOOL,		&mdbgcfg.type)};


static void mdpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
#if defined(SUPPORT_16BPP)
		case 16:
			for (i=0; i<pals; i++) {
				dst[i].pal16 = dd2_get16pal(mdbgwin.dd2hdl, src[i]);
			}
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
#endif
#if defined(SUPPORT_32BPP)
		case 32:
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
			for (i=0; i<pals; i++) {
				dst[i].pal32.d = src[i].d;
			}
			break;
#endif
	}
}

static void mdwincreate(HWND hWnd) {

	int			width;
	int			height;

	memdbg32_getsize(&width, &height);
}

static void mddrawwin(HWND hWnd, BOOL redraw) {

	RECT		rect;
	RECT		draw;
	CMNVRAM		*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(mdbgwin.width, rect.right - rect.left);
	draw.bottom = min(mdbgwin.height, rect.bottom - rect.top);
	vram = dd2_bsurflock(mdbgwin.dd2hdl);
	if (vram) {
		memdbg32_paint(vram, mdpalcnv, redraw);
		dd2_bsurfunlock(mdbgwin.dd2hdl);
		dd2_blt(mdbgwin.dd2hdl, NULL, &draw);
	}
}

static void mdpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	mddrawwin(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK mdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			winloc_setclientsize(hWnd, mdbgwin.width, mdbgwin.height);
			np2class_windowtype(hWnd, (mdbgcfg.type & 1) + 1);
			break;

		case WM_PAINT:
			mdpaintmsg(hWnd);
			break;

		case WM_LBUTTONDOWN:
			if (mdbgcfg.type & 1) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_LBUTTONDBLCLK:
			mdbgcfg.type ^= 1;
			wintypechange(hWnd, (mdbgcfg.type & 1) + 1);
			sysmng_update(SYS_UPDATEOSCFG);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(g_hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			winlocex_destroy(mdbgwin.wlex);
			mdbgwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(mdbgwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(mdbgwin.wlex);
			mdbgwin.wlex = NULL;
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				mdbgcfg.posx = rc.left;
				mdbgcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			dd2_release(mdbgwin.dd2hdl);
			mdbgwin.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}

BOOL mdbgwin_initialize(HINSTANCE hInstance) {

	WNDCLASS	wc;

	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = mdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mdbgclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	memdbg32_initialize();
	return(SUCCESS);
}

void mdbgwin_create(HINSTANCE hInstance) {

	HWND	hWnd;

	if (mdbgwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&mdbgwin, sizeof(mdbgwin));
	memdbg32_getsize(&mdbgwin.width, &mdbgwin.height);
	hWnd = CreateWindow(mdbgclass, mdbgtitle,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						mdbgcfg.posx, mdbgcfg.posy,
						mdbgwin.width, mdbgwin.height,
						NULL, NULL, hInstance, NULL);
	mdbgwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto mdcre_err1;
	}
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hWnd);
	mdbgwin.dd2hdl = dd2_create(hWnd, mdbgwin.width, mdbgwin.height);
	if (mdbgwin.dd2hdl == NULL) {
		goto mdcre_err2;
	}
	InvalidateRect(hWnd, NULL, TRUE);
	SetForegroundWindow(g_hWndMain);
	return;

mdcre_err2:
	DestroyWindow(hWnd);

mdcre_err1:
	return;
}

void mdbgwin_destroy(void) {

	if (mdbgwin.hwnd) {
		DestroyWindow(mdbgwin.hwnd);
	}
}

void mdbgwin_process(void) {

	if ((mdbgwin.hwnd) && (memdbg32_process())) {
		mddrawwin(mdbgwin.hwnd, FALSE);
	}
}

HWND mdbgwin_gethwnd(void) {

	return(mdbgwin.hwnd);
}

void mdbgwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	mdbgcfg.posx = CW_USEDEFAULT;
	mdbgcfg.posy = CW_USEDEFAULT;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, mdbgapp, mdbgini, NELEMENTS(mdbgini));
}

void mdbgwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, mdbgapp, mdbgini, NELEMENTS(mdbgini));
}
#endif


// ---- soft keyboard

#if defined(SUPPORT_SOFTKBD)
typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
	int			width;
	int			height;
} SKBDWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	type;
} SKBDCFG;

static	SKBDWIN		skbdwin;
static	SKBDCFG		skbdcfg;

static const TCHAR skbdclass[] = _T("NP2-SoftKBDWin");

static const OEMCHAR skbdapp[] = OEMTEXT("Soft Keyboard");
static const PFTBL skbdini[] = {
				PFVAL("WindposX", PFTYPE_SINT32,	&skbdcfg.posx),
				PFVAL("WindposY", PFTYPE_SINT32,	&skbdcfg.posy),
				PFVAL("windtype", PFTYPE_BOOL,		&skbdcfg.type)};

static void skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
#if defined(SUPPORT_16BPP)
		case 16:
			for (i=0; i<pals; i++) {
				dst[i].pal16 = dd2_get16pal(skbdwin.dd2hdl, src[i]);
			}
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
#endif
#if defined(SUPPORT_32BPP)
		case 32:
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
			for (i=0; i<pals; i++) {
				dst[i].pal32.d = src[i].d;
			}
			break;
#endif
	}
}

static void skdrawkeys(HWND hWnd, BOOL redraw) {

	RECT		rect;
	RECT		draw;
	CMNVRAM		*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(skbdwin.width, rect.right - rect.left);
	draw.bottom = min(skbdwin.height, rect.bottom - rect.top);
	vram = dd2_bsurflock(skbdwin.dd2hdl);
	if (vram) {
		softkbd_paint(vram, skpalcnv, redraw);
		dd2_bsurfunlock(skbdwin.dd2hdl);
		dd2_blt(skbdwin.dd2hdl, NULL, &draw);
	}
}

static void skpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	skdrawkeys(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK skproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			winloc_setclientsize(hWnd, skbdwin.width, skbdwin.height);
			np2class_windowtype(hWnd, (skbdcfg.type & 1) + 1);
			break;

		case WM_PAINT:
			skpaintmsg(hWnd);
			break;

		case WM_LBUTTONDOWN:
			if ((softkbd_down(LOWORD(lp), HIWORD(lp))) &&
				(skbdcfg.type & 1)) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_LBUTTONDBLCLK:
			if (softkbd_down(LOWORD(lp), HIWORD(lp))) {
				skbdcfg.type ^= 1;
				wintypechange(hWnd, (skbdcfg.type & 1) + 1);
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_LBUTTONUP:
			softkbd_up();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(g_hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			winlocex_destroy(skbdwin.wlex);
			skbdwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(skbdwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(skbdwin.wlex);
			skbdwin.wlex = NULL;
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				skbdcfg.posx = rc.left;
				skbdcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			dd2_release(skbdwin.dd2hdl);
			skbdwin.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}

BOOL skbdwin_initialize(HINSTANCE hInstance) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = skproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = NP2GWLP_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = skbdclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	softkbd_initialize();
	return(SUCCESS);
}

void skbdwin_deinitialize(void) {

	softkbd_deinitialize();
}

void skbdwin_create(HINSTANCE hInstance) {

	TCHAR	szCaption[128];
	HWND	hwnd;

	if (skbdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&skbdwin, sizeof(skbdwin));
	if (softkbd_getsize(&skbdwin.width, &skbdwin.height) != SUCCESS) {
		return;
	}

	loadstringresource(LOWORD(IDS_CAPTION_SOFTKEY),
										szCaption, NELEMENTS(szCaption));
	hwnd = CreateWindow(skbdclass, szCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						skbdcfg.posx, skbdcfg.posy,
						skbdwin.width, skbdwin.height,
						NULL, NULL, hInstance, NULL);
	skbdwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto skcre_err1;
	}
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	skbdwin.dd2hdl = dd2_create(hwnd, skbdwin.width, skbdwin.height);
	if (skbdwin.dd2hdl == NULL) {
		goto skcre_err2;
	}
	InvalidateRect(hwnd, NULL, TRUE);
	SetForegroundWindow(g_hWndMain);
	return;

skcre_err2:
	DestroyWindow(hwnd);

skcre_err1:
	return;
}

void skbdwin_destroy(void) {

	if (skbdwin.hwnd != NULL) {
		DestroyWindow(skbdwin.hwnd);
	}
}

HWND skbdwin_gethwnd(void) {

	return(skbdwin.hwnd);
}

void skbdwin_process(void) {

	if ((skbdwin.hwnd) && (softkbd_process())) {
		skdrawkeys(skbdwin.hwnd, FALSE);
	}
}

void skbdwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	skbdcfg.posx = CW_USEDEFAULT;
	skbdcfg.posy = CW_USEDEFAULT;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, skbdapp, skbdini, NELEMENTS(skbdini));
}

void skbdwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, skbdapp, skbdini, NELEMENTS(skbdini));
}
#endif

