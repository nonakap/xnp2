#include	"compiler.h"
#include	<gx.h>
#if !defined(_WIN32_WCE)
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"dosio.h"
#include	"commng.h"
#include	"inputmng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"winkbd.h"
#include	"ini.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"bios.h"
#include	"scrndraw.h"
#include	"sound.h"
#include	"beep.h"
#include	"s98.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"font.h"
#include	"timing.h"
#include	"keystat.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"sysmenu.h"
#include	"softkbd.h"


static const TCHAR szAppCaption[] = _T("Neko Project II");
static const TCHAR szClassName[] = _T("NP2-MainWindow");


		NP2OSCFG	np2oscfg = {0, 0, 0, 0,
#if !defined(GX_DLL)
								CW_USEDEFAULT, CW_USEDEFAULT,
#endif
#if defined(WIN32_PLATFORM_PSPC)
								0, 0,
#endif
								0};
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPrev;
		OEMCHAR		modulefile[MAX_PATH];
		GXKeyList	gx_keylist;

enum {
	SYSRUNNING_MAIN		= 1,
	SYSRUNNING_FORE		= 2
};

static	UINT		sysrunning;
static	UINT		framecnt;
static	UINT		waitcnt;
static	UINT		framemax = 1;


// ---- resume

static void getstatfilename(OEMCHAR *path, const OEMCHAR *ext, int size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, str_dot, size);
	file_catname(path, ext, size);
}

static int flagsave(const OEMCHAR *ext) {

	int		ret;
	OEMCHAR	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	return(ret);
}

static void flagdelete(const OEMCHAR *ext) {

	OEMCHAR	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	file_delete(path);
}

static int flagload(const OEMCHAR *ext, const OEMCHAR *title, BOOL force) {

	int		ret;
	int		id;
	OEMCHAR	path[MAX_PATH];
	OEMCHAR	buf[1024];
	OEMCHAR	buf2[1024 + 256];

	getstatfilename(path, ext, NELEMENTS(path));
	id = DID_YES;
	ret = statsave_check(path, buf, NELEMENTS(buf));
	if (ret & (~STATFLAG_DISKCHG)) {
		menumbox(OEMTEXT("Couldn't restart"), title, MBOX_OK | MBOX_ICONSTOP);
		id = DID_NO;
	}
	else if ((!force) && (ret & STATFLAG_DISKCHG)) {
		OEMSPRINTF(buf2, OEMTEXT("Conflict!\n\n%s\nContinue?"), buf);
		id = menumbox(buf2, title, MBOX_YESNOCAN | MBOX_ICONQUESTION);
	}
	if (id == DID_YES) {
		statsave_load(path);
	}
	return(id);
}


// ---- proc

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	HDC			hdc;

	switch (msg) {
		case WM_CREATE:
#if !defined(_WIN32_WCE)
			WINNLSEnableIME(hWnd, FALSE);
#else
			ImmAssociateContext(hWnd, NULL);
#endif
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (!sysrunning) {
				scrnmng_clear(TRUE);
			}
			else {
				scrndraw_redraw();
			}
			EndPaint(hWnd, &ps);
			break;

#if defined(WIN32_PLATFORM_PSPC)
		case WM_ERASEBKGND:
			if (sysrunning) {
				scrndraw_redraw();
			}
			break;
#endif

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				if (menuvram == NULL) {
					sysmenu_menuopen(0, 0, 0);
				}
				else {
					menubase_close();
				}
			}
			else {
				winkbd_keydown(wParam, lParam);
			}
			break;

		case WM_KEYUP:
			winkbd_keyup(wParam, lParam);
			break;

#if 1 // !defined(_WIN32_WCE)
		case WM_SYSKEYDOWN:
			winkbd_keydown(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkbd_keyup(wParam, lParam);
			break;
#endif

		case WM_MOUSEMOVE:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram) {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 0);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram) {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 1);
				}
				else if (scrnmng_kbdpos(&lParam) == SUCCESS) {
#if defined(SUPPORT_SOFTKBD)
					softkbd_down(LOWORD(lParam), HIWORD(lParam));
#endif
				}
			}
			break;

		case WM_LBUTTONUP:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
#if defined(SUPPORT_SOFTKBD)
				softkbd_up();
#endif
				if (menuvram) {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 2);
				}
				else if (scrnmng_ismenu(lParam)) {
					sysmenu_menuopen(0, LOWORD(lParam), HIWORD(lParam));
				}
			}
			break;

#if !defined(GX_DLL)
		case WM_MOVE:
#if !defined(_WIN32_WCE)
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE)))
#endif
			{
				RECT rc;
				GetWindowRect(hWnd, &rc);
				np2oscfg.winx = rc.left;
				np2oscfg.winy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;
#endif

#if !defined(_WIN32_WCE)
		case WM_ENTERSIZEMOVE:
			soundmng_stop();
			break;

		case WM_EXITSIZEMOVE:
			soundmng_play();
			break;
#endif

		case WM_CLOSE:
#if !defined(WIN32_PLATFORM_PSPC)
			taskmng_exit();
#endif
			break;

		case WM_DESTROY:
			break;

		case WM_ACTIVATE:
			if (sysrunning) {
				if (LOWORD(wParam) != WA_INACTIVE) {
					GXResume();
#if defined(GX_DLL)
					scrnmng_enable(TRUE);
#endif
					scrndraw_redraw();
					soundmng_enable(SNDPROC_MAIN);
					sysrunning |= SYSRUNNING_FORE;
				}
				else {
					sysrunning &= ~SYSRUNNING_FORE;
					soundmng_disable(SNDPROC_MAIN);
#if defined(GX_DLL)
					scrnmng_enable(FALSE);
#endif
					GXSuspend();
				}
			}
			break;

#if defined(WAVEMNG_CBMAIN)
		case MM_WOM_DONE:
			soundmng_cb(MM_WOM_DONE, (HWAVEOUT)wParam, (WAVEHDR *)lParam);
			break;
#endif

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


#define	framereset(cnt)		framecnt = 0

static void processwait(UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset(cnt);
	}
#if !defined(_WIN32_WCE)
	else {
		Sleep(1);
	}
#endif
}


// ----

#if defined(_WIN32_WCE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPWSTR lpszCmdLine, int nCmdShow)
#elif defined(UNICODE)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow)
#endif
{
	HWND		hWnd;
	WNDCLASS	np2;
	int			id;
	MSG			msg;

	hWnd = FindWindow(szClassName, NULL);
	if (hWnd != NULL) {
#if !defined(WIN32_PLATFORM_PSPC)
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
#else
		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow((HWND)((ULONG)hWnd | 1));
#endif
		return(0);
	}

#if defined(OEMCHAR_SAME_TCHAR)
	GetModuleFileName(NULL, modulefile, NELEMENTS(modulefile));
#else
	TCHAR _modulefile[MAX_PATH];
	GetModuleFileName(NULL, _modulefile, NELEMENTS(_modulefile));
	tchartooem(modulefile, NELEMENTS(modulefile), _modulefile, (UINT)-1);
#endif
	dosio_init();
	file_setcd(modulefile);
	initload();

//	srand((unsigned)time(NULL));

	hInst = hInstance;
	hPrev = hPreInst;
	TRACEINIT();

	inputmng_init();
	keystat_initialize();

//	if (!hPreInst) {
		np2.style = CS_HREDRAW | CS_VREDRAW;
		np2.lpfnWndProc = WndProc;
		np2.cbClsExtra = 0;
		np2.cbWndExtra = 0;
		np2.hInstance = hInstance;
		np2.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		np2.hCursor = LoadCursor(NULL, IDC_ARROW);
		np2.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		np2.lpszMenuName = NULL;
		np2.lpszClassName = szClassName;
		if (!RegisterClass(&np2)) {
			return(FALSE);
		}
//	}

#if !defined(GX_DLL)
	hWnd = CreateWindow(szClassName, szAppCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						np2oscfg.winx, np2oscfg.winy,
						FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT,
						NULL, NULL, hInstance, NULL);
#else
	hWnd = CreateWindow(szClassName, szAppCaption,
						WS_VISIBLE,
						0, 0,
		      			GetSystemMetrics(SM_CXSCREEN),
		      			GetSystemMetrics(SM_CYSCREEN),
						NULL, NULL, hInstance, NULL);
#endif
	hWndMain = hWnd;
	if (hWnd == NULL) {
		goto np2main_err1;
	}

	scrnmng_initialize();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (sysmenu_create() != SUCCESS) {
		DestroyWindow(hWnd);
		goto np2main_err1;
	}
	if (scrnmng_create(hWnd, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT)
																!= SUCCESS) {
		MessageBox(hWnd, _T("Couldn't create DirectDraw Object"),
									szAppCaption, MB_OK | MB_ICONSTOP);
		DestroyWindow(hWnd);
		goto np2main_err2;
	}

	if (GXOpenInput() == 0) {
		DestroyWindow(hWnd);
		goto np2main_err3;
	}
	gx_keylist = GXGetDefaultKeys(GX_NORMALKEYS);
	scrnmng_keybinds();
#if defined(WIN32_PLATFORM_PSPC)
	winkbd_bindinit();
#endif

	soundmng_initialize();
	commng_initialize();
	sysmng_initialize();
	taskmng_initialize();
	pccore_init();
	S98_init();

	pccore_reset();
	scrndraw_redraw();

#if defined(WIN32_PLATFORM_PSPC)
	winkbd_bindcur(np2oscfg.bindcur);
	winkbd_bindbtn(np2oscfg.bindbtn);
#endif

	sysrunning |= SYSRUNNING_MAIN | SYSRUNNING_FORE;

	if (np2oscfg.resume) {
		id = flagload(str_sav, str_resume, FALSE);
		if (id == DID_CANCEL) {
			DestroyWindow(hWnd);
			goto np2main_err4;
		}
	}

	while(taskmng_isavail()) {
		if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
#if 1 // !defined(_WIN32_WCE)
			if ((msg.hwnd != hWnd) ||
				((msg.message != WM_SYSKEYDOWN) &&
				(msg.message != WM_SYSKEYUP))) {
				TranslateMessage(&msg);
			}
#else
			TranslateMessage(&msg);
#endif
			DispatchMessage(&msg);
		}
		else if (sysrunning & SYSRUNNING_FORE) {
			if (np2oscfg.NOWAIT) {
				pccore_exec(framecnt == 0);
				if (np2oscfg.DRAW_SKIP) {			// nowait frame skip
					framecnt++;
					if (framecnt >= np2oscfg.DRAW_SKIP) {
						processwait(0);
					}
				}
				else {							// nowait auto skip
					framecnt = 1;
					if (timing_getcount()) {
						processwait(0);
					}
				}
			}
			else if (np2oscfg.DRAW_SKIP) {		// frame skip
				if (framecnt < np2oscfg.DRAW_SKIP) {
					pccore_exec(framecnt == 0);
					framecnt++;
				}
				else {
					processwait(np2oscfg.DRAW_SKIP);
				}
			}
			else {								// auto skip
				if (!waitcnt) {
					UINT cnt;
					pccore_exec(framecnt == 0);
					framecnt++;
					cnt = timing_getcount();
					if (framecnt > cnt) {
						waitcnt = framecnt;
						if (framemax > 1) {
							framemax--;
						}
					}
					else if (framecnt >= framemax) {
						if (framemax < 12) {
							framemax++;
						}
						if (cnt >= 12) {
							timing_reset();
						}
						else {
							timing_setcount(cnt - framecnt);
						}
						framereset(0);
					}
				}
				else {
					processwait(waitcnt);
					waitcnt = framecnt;
				}
			}
		}
		else {
			Sleep(100);
		}
	}

	soundmng_disable(SNDPROC_MAIN);
	sysrunning = FALSE;

	DestroyWindow(hWnd);

	pccore_cfgupdate();
	if (np2oscfg.resume) {
		flagsave(str_sav);
	}
	else {
		flagdelete(str_sav);
	}
	pccore_term();
	S98_trash();
	soundmng_deinitialize();

	GXCloseInput();
	scrnmng_destroy();
	sysmenu_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	dosio_term();

	return(msg.wParam);

np2main_err4:
	pccore_term();
	S98_trash();
	soundmng_deinitialize();
	GXCloseInput();

np2main_err3:
	scrnmng_destroy();

np2main_err2:
	sysmenu_destroy();

np2main_err1:
	TRACETERM();
	dosio_term();
	return(FALSE);
}

