#include	"compiler.h"
#include	"strres.h"
#include	"profile.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cmndraw.h"
#include	"memdbg32.h"
#include	"softkbd.h"


#pragma pack(push, 1)
typedef struct {
	BITMAPINFOHEADER	bmiHeader;
	UINT32				bmiColors[256];
} _BMPINFO, *BMPINFO;
#pragma pack(pop)


static const char inifile[] = "np2sub.cfg";

static void setclientsize(HWND hwnd, int width, int height) {

	RECT	rectDisktop;
	int		scx;
	int		scy;
	UINT	cnt;
	RECT	rectWindow;
	RECT	rectClient;
	int		x, y, w, h;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDisktop, 0);
	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);

	cnt = 2;
	do {
		GetWindowRect(hwnd, &rectWindow);
		GetClientRect(hwnd, &rectClient);
		w = width + (rectWindow.right - rectWindow.left)
					- (rectClient.right - rectClient.left);
		h = height + (rectWindow.bottom - rectWindow.top)
					- (rectClient.bottom - rectClient.top);

		x = rectWindow.left;
		y = rectWindow.top;
		if (scx < w) {
			x = (scx - w) / 2;
		}
		else {
			if ((x + w) > rectDisktop.right) {
				x = rectDisktop.right - w;
			}
			if (x < rectDisktop.left) {
				x = rectDisktop.left;
			}
		}
		if (scy < h) {
			y = (scy - h) / 2;
		}
		else {
			if ((y + h) > rectDisktop.bottom) {
				y = rectDisktop.bottom - h;
			}
			if (y < rectDisktop.top) {
				y = rectDisktop.top;
			}
		}
		MoveWindow(hwnd, x, y, w, h, TRUE);
	} while(--cnt);
}

static HBITMAP allocbmp(int width, int height, CMNVRAM *vram) {

	_BMPINFO	bi;
	HDC			hdc;
	HBITMAP		ret;
	void		*image;

	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = 0 - height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_BITFIELDS;
	bi.bmiColors[0] = 0xff0000;
	bi.bmiColors[1] = 0x00ff00;
	bi.bmiColors[2] = 0x0000ff;
	hdc = GetDC(NULL);
	ret = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
														&image, NULL, 0);
	ReleaseDC(NULL, hdc);
	if (ret != NULL) {
		ZeroMemory(image, width * height * 4);
		if (vram) {
			vram->ptr = (UINT8 *)image;
			vram->width = width;
			vram->height = height;
			vram->xalign = 4;
			vram->yalign = width * 4;
			vram->bpp = 32;
		}
	}
	return(ret);
}


// ---- memdbg

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

typedef struct {
	HWND	hwnd;
	HBITMAP	hbmp;
	CMNVRAM	vram;
} MDBGWIN;

typedef struct {
	int		posx;
	int		posy;
} MDBGCFG;

static	MDBGWIN		mdbgwin;
static	MDBGCFG		mdbgcfg;

static const char mdbgapp[] = "Memory Map";
static const char mdbgclass[] = "np2-memdbg";
static const PFTBL mdbgini[] = {
	{"WindposX", PFTYPE_SINT32,		&mdbgcfg.posx,			0},
	{"WindposY", PFTYPE_SINT32,		&mdbgcfg.posy,			0}};


static void mdpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
		case 32:
			for (i=0; i<pals; i++) {
				dst[i].pal32.d = src[i].d;
			}
			break;
	}
}

static void mdwincreate(HWND hWnd) {

	int			width;
	int			height;

	memdbg32_initialize();
	memdbg32_getsize(&width, &height);
	setclientsize(hWnd, width, height);
	mdbgwin.hbmp = allocbmp(width, height, &mdbgwin.vram);
	if (mdbgwin.hbmp) {
		memdbg32_paint(&mdbgwin.vram, mdpalcnv, TRUE);
	}
}

static void mdwinpaint(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	BITMAP		bmp;
	HDC			hmdc;

	hdc = BeginPaint(hWnd, &ps);
	if (mdbgwin.hbmp) {
		GetObject(mdbgwin.hbmp, sizeof(BITMAP), &bmp);
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, mdbgwin.hbmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
		DeleteDC(hmdc);
	}
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK mdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			mdwincreate(hWnd);
			break;

		case WM_PAINT:
			mdwinpaint(hWnd);
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
			DeleteObject(mdbgwin.hbmp);
			mdbgwin.hbmp = NULL;
			mdbgwin.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}

BOOL memdbg_initialize(HINSTANCE hInstance) {

	WNDCLASS	wc;

	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = mdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mdbgclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

void memdbg_create(void) {

	HWND	hWnd;

	if (mdbgwin.hwnd) {
		return;
	}
	hWnd = CreateWindow(mdbgclass, mdbgapp,
							WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
							WS_MINIMIZEBOX,
							mdbgcfg.posx, mdbgcfg.posy, 128, 128,
							NULL, NULL, hInst, NULL);
	mdbgwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto mdope_err;
	}
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	SetForegroundWindow(hWndMain);
	return;

mdope_err:
	return;
}

void memdbg_destroy(void) {

	if (mdbgwin.hwnd) {
		DestroyWindow(mdbgwin.hwnd);
	}
}

void memdbg_process(void) {

	if ((mdbgwin.hwnd) && (mdbgwin.hbmp)) {
		if (memdbg32_paint(&mdbgwin.vram, mdpalcnv, FALSE)) {
			InvalidateRect(mdbgwin.hwnd, NULL, TRUE);
		}
	}
}

void memdbg_readini(void) {

	char	path[MAX_PATH];

	mdbgcfg.posx = CW_USEDEFAULT;
	mdbgcfg.posy = CW_USEDEFAULT;
	file_cpyname(path, file_getcd(inifile), sizeof(path));
	profile_iniread(path, mdbgapp, mdbgini, NELEMENTS(mdbgini), NULL);
}

void memdbg_writeini(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	profile_iniwrite(path, mdbgapp, mdbgini, NELEMENTS(mdbgini), NULL);
}
#endif


// ---- soft keyboard

#if defined(SUPPORT_SOFTKBD)
typedef struct {
	HWND	hwnd;
	HBITMAP	hbmp;
	CMNVRAM	vram;
} SKBDWIN;

typedef struct {
	int		posx;
	int		posy;
} SKBDCFG;

static	SKBDWIN		skbdwin;
static	SKBDCFG		skbdcfg;

static const char skbdapp[] = "Soft Keyboard";
static const char skbdclass[] = "NP2-SoftKBDWin";
static const PFTBL skbdini[] = {
	{"WindposX", PFTYPE_SINT32,		&skbdcfg.posx,			0},
	{"WindposY", PFTYPE_SINT32,		&skbdcfg.posy,			0}};

static void skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
		case 32:
			for (i=0; i<pals; i++) {
				dst[i].pal32.d = src[i].d;
			}
			break;
	}
}

static void skcreate(HWND hWnd) {

	int			width;
	int			height;

	softkbd_getsize(&width, &height);
	setclientsize(hWnd, width, height);
	skbdwin.hbmp = allocbmp(width, height, &skbdwin.vram);
	if (skbdwin.hbmp) {
		softkbd_paint(&skbdwin.vram, skpalcnv, TRUE);
	}
}

static void skpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	BITMAP		bmp;
	HDC			hmdc;

	hdc = BeginPaint(hWnd, &ps);
	if (skbdwin.hbmp) {
		GetObject(skbdwin.hbmp, sizeof(BITMAP), &bmp);
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, skbdwin.hbmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
		DeleteDC(hmdc);
	}
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK skproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			skcreate(hWnd);
			break;

		case WM_PAINT:
			skpaintmsg(hWnd);
			break;

		case WM_LBUTTONDOWN:
			softkbd_down(LOWORD(lp), HIWORD(lp));
			break;

		case WM_LBUTTONUP:
			softkbd_up();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(hWndMain, msg, wp, lp);
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
			DeleteObject(skbdwin.hbmp);
			skbdwin.hbmp = NULL;
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
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = skproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
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

void skbdwin_create(void) {

	HWND	hwnd;

	if (skbdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&skbdwin, sizeof(skbdwin));
	hwnd = CreateWindow(skbdclass, skbdapp,
							WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
							WS_MINIMIZEBOX,
							skbdcfg.posx, skbdcfg.posy, 128, 128,
							NULL, NULL, hInst, NULL);
	skbdwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto skcre_err1;
	}
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	SetForegroundWindow(hWndMain);

skcre_err1:
	return;
}

void skbdwin_destroy(void) {

	if (skbdwin.hwnd != NULL) {
		DestroyWindow(skbdwin.hwnd);
	}
}

void skbdwin_process(void) {

	if ((skbdwin.hwnd) && (skbdwin.hbmp)) {
		if (softkbd_paint(&skbdwin.vram, skpalcnv, FALSE)) {
			InvalidateRect(skbdwin.hwnd, NULL, TRUE);
		}
	}
}

void skbdwin_readini(void) {

	char	path[MAX_PATH];

	skbdcfg.posx = CW_USEDEFAULT;
	skbdcfg.posy = CW_USEDEFAULT;
	file_cpyname(path, file_getcd(inifile), sizeof(path));
	profile_iniread(path, skbdapp, skbdini, NELEMENTS(skbdini), NULL);
}

void skbdwin_writeini(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	profile_iniwrite(path, skbdapp, skbdini, NELEMENTS(skbdini), NULL);
}
#endif

