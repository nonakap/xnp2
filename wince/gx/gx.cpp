#include	"compiler.h"
#include	"gx.h"

#if !defined(GX_DLL)

#if defined(SIZE_QVGA)
#if defined(SUPPORT_SOFTKBD)
enum {
	WINDOW_WIDTH	= 320,
	WINDOW_HEIGHT	= 240
};
#else
enum {
	WINDOW_WIDTH	= 320,
	WINDOW_HEIGHT	= 200
};
#endif
#else
#if defined(SUPPORT_SOFTKBD)
enum {
	WINDOW_WIDTH	= 640,
	WINDOW_HEIGHT	= 480
};
#else
enum {
	WINDOW_WIDTH	= 640,
	WINDOW_HEIGHT	= 400
};
#endif
#endif

#pragma pack(push, 1)
typedef struct {
	BITMAPINFOHEADER	bmiHeader;
	DWORD				bmiColors[256];
} _BMPINFO, *BMPINFO;
#pragma pack(pop)

#ifdef STRICT
#define	SUBCLASSPROC	WNDPROC
#else
#define	SUBCLASSPROC	FARPROC
#endif

enum {
	GX_INIT		= 0x01,
	GX_WINDOW	= 0x02
};

#if defined(_WIN32_WCE)
static const TCHAR taskbarclass[] = _T("HHTaskBar");
#else
static const TCHAR taskbarclass[] = _T("Shell_TrayWnd");
#endif


static GXDisplayProperties gxdp;
#if 0
static GXKeyList gxkl = {
			VK_UP,		{0, 0},		VK_DOWN, 	{0, 0},
			VK_LEFT,	{0, 0},		VK_RIGHT,	{0, 0},
			VK_RETURN,	{0, 0},		VK_ESCAPE,	{0, 0},
			'Z',		{0, 0},		'X',		{0, 0}};
#else
static GXKeyList gxkl = {
			VK_NUMPAD8,	{0, 0},		VK_NUMPAD2,	{0, 0},
			VK_NUMPAD4,	{0, 0},		VK_NUMPAD6,	{0, 0},
			'Z',		{0, 0},		'X',		{0, 0},
			'C',		{0, 0},		'V',		{0, 0}};
#endif

static	DWORD			GXFlag = 0;
static	int				GXWidth;
static	int				GXHeight;
static	int				GXStartX;
static	int				GXStartY;
static	HWND			gxhwnd = NULL;
static	DWORD			gxhwndstyle;
static	DWORD			gxhwndstyleex;
static	HBITMAP			hgxbmp;
static	void			*gximage = NULL;
static	SUBCLASSPROC	gxold;


static void gx_init(void) {

	DWORD	x;
	DWORD	y;
	GXDisplayProperties dp;
	int		sx;
	int		sy;

	if (GXFlag) {
		return;
	}
	GXFlag = GX_INIT;
	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);

	dp.cxWidth = WINDOW_WIDTH;
	dp.cyHeight = WINDOW_HEIGHT;
	if ((x > WINDOW_WIDTH) && (y > WINDOW_HEIGHT)) {
		GXFlag |= GX_WINDOW;
		x = WINDOW_WIDTH;
		y = WINDOW_HEIGHT;
		sx = 0;
		sy = 0;
	}
	else if ((x >= WINDOW_WIDTH) && (y >= WINDOW_HEIGHT)) {
		sx = (x - WINDOW_WIDTH) / 2;
		sy = (y - WINDOW_HEIGHT) / 2;
	}
	else {
		dp.cxWidth = x;
		dp.cyHeight = y;
		sx = 0;
		sy = 0;
	}
	dp.cbxPitch = 2;
	dp.cbyPitch = ((dp.cxWidth * 2) + 3) & (~3);
	dp.cBPP = 16;
	dp.ffFormat = kfDirect | kfDirect565;
	gxdp = dp;
	GXWidth = x;
	GXHeight = y;
	GXStartX = sx;
	GXStartY = sy;
}

static LRESULT CALLBACK gxdraw(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	int		x;
	int		y;

	switch(msg) {
		case WM_MOUSEMOVE:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			x = LOWORD(lp) - GXStartX;
			y = HIWORD(lp) - GXStartY;
			return(CallWindowProc(gxold, hWnd, msg, wp,
										(LPARAM)(y << 16) | (x & 0xffff)));
	}
	return(CallWindowProc(gxold, hWnd, msg, wp, lp));
}

static void setclientsize(HWND hwnd, LONG width, LONG height) {

	int		scx, scy;
	int		x, y, w, h;
	RECT	rectWindow, rectClient, rectDisktop;

	GetWindowRect(hwnd, &rectWindow);
	GetClientRect(hwnd, &rectClient);
	w = width + (rectWindow.right - rectWindow.left)
				- (rectClient.right - rectClient.left);
	h = height + (rectWindow.bottom - rectWindow.top)
				- (rectClient.bottom - rectClient.top);

	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDisktop, 0);

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
}


// ----

int GXOpenDisplay(HWND hWnd, DWORD dwFlags) {

	DWORD		winstyle;
	DWORD		winstyleex;
	_BMPINFO	bi;
	HDC			hdc;
	HWND		hTaskBar;

	if (gxhwnd) {
		goto gxopn_err1;
	}
	gx_init();
	gxhwnd = hWnd;

	// bitmapì¬B
	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = gxdp.cxWidth;
	bi.bmiHeader.biHeight = 0 - gxdp.cyHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 16;
	bi.bmiHeader.biCompression = BI_BITFIELDS;
	bi.bmiColors[0] = 0xf800;
	bi.bmiColors[1] = 0x07e0;
	bi.bmiColors[2] = 0x001f;
	hdc = GetDC(NULL);
	hgxbmp = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
														&gximage, NULL, 0);
	ReleaseDC(NULL, hdc);
	if (hgxbmp == NULL) {
		goto gxopn_err2;
	}
	ZeroMemory(gximage, gxdp.cbyPitch * gxdp.cyHeight);

	gxold = (SUBCLASSPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)gxdraw);

	if (GXFlag & GX_WINDOW) {
		setclientsize(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT);
	}
	else {
		hTaskBar = FindWindow(taskbarclass, NULL);
		if (hTaskBar) {
			ShowWindow(hTaskBar, SW_HIDE);
		}
		winstyle = GetWindowLong(hWnd, GWL_STYLE);
		gxhwndstyle = winstyle;
		winstyleex = GetWindowLong(hWnd, GWL_EXSTYLE);
		gxhwndstyleex = winstyleex;
		winstyle &= ~(WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU);
		winstyle |= WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		winstyleex |= WS_EX_TOPMOST;
		SetWindowLong(hWnd, GWL_STYLE, winstyle);
		SetWindowLong(hWnd, GWL_EXSTYLE, winstyleex);
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
					SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		MoveWindow(hWnd, 0, 0, GXWidth, GXHeight, TRUE);
	}
	return(1);

gxopn_err2:
	gxhwnd = NULL;
	gximage = NULL;

gxopn_err1:
	return(0);
}

int GXCloseDisplay(void) {

	HWND	hTaskBar;

	if (gxhwnd) {
		SetWindowLong(gxhwnd, GWL_WNDPROC, (LONG)gxold);
		gximage = NULL;
		DeleteObject(hgxbmp);
		if (!(GXFlag & GX_WINDOW)) {
			SetWindowLong(gxhwnd, GWL_STYLE, gxhwndstyle);
			SetWindowLong(gxhwnd, GWL_EXSTYLE, gxhwndstyleex);
			SetWindowPos(gxhwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			hTaskBar = FindWindow(taskbarclass, NULL);
			if (hTaskBar) {
				ShowWindow(hTaskBar, SW_SHOW);
			}
		}
		gxhwnd = NULL;
	}
	return(0);
}

void *GXBeginDraw(void) {

	return(gximage);
}

int GXEndDraw(void) {

	HDC		hdc;
	HDC		hmemdc;
	HBITMAP	hbitmap;

	if (gxhwnd) {
		hdc = GetDC(gxhwnd);
		hmemdc = CreateCompatibleDC(hdc);
		hbitmap = (HBITMAP)SelectObject(hmemdc, hgxbmp);
		BitBlt(hdc, GXStartX, GXStartY, gxdp.cxWidth, gxdp.cyHeight,
				hmemdc, 0, 0, SRCCOPY);
		SelectObject(hmemdc, hbitmap);
		DeleteDC(hmemdc);
		ReleaseDC(gxhwnd, hdc);
	}
	return(1);
}

GXDisplayProperties GXGetDisplayProperties(void) {

	gx_init();
	return(gxdp);
}

int GXSuspend(void) {

	HWND	hTaskBar;

	if (gxhwnd) {
		if (!(GXFlag & GX_WINDOW)) {
			hTaskBar = FindWindow(taskbarclass, NULL);
			if (hTaskBar) {
				ShowWindow(hTaskBar, SW_SHOW);
			}
		}
		return(1);
	}
	else {
		return(0);
	}
}

int GXResume(void) {

	HWND	hTaskBar;

	if (gxhwnd) {
		if (!(GXFlag & GX_WINDOW)) {
			hTaskBar = FindWindow(taskbarclass, NULL);
			if (hTaskBar) {
				ShowWindow(hTaskBar, SW_HIDE);
			}
			MoveWindow(gxhwnd, 0, 0, GXWidth, GXHeight, TRUE);
		}
		return(1);
	}
	else {
		return(0);
	}
}

int GXOpenInput(void) {

	return(1);
}

int GXCloseInput(void) {

	return(1);
}

GXKeyList GXGetDefaultKeys(int iOptions) {

	return(gxkl);
}

#endif

