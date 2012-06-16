#include	"compiler.h"
#include	<time.h>
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"resource.h"
#include	"strres.h"
#include	"parts.h"
#include	"np2.h"
#include	"np2arg.h"
#include	"dosio.h"
#include	"commng.h"
#include	"joymng.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"winkbd.h"
#include	"ini.h"
#include	"menu.h"
#include	"subwind.h"
#include	"dialog.h"
#include	"cpucore.h"
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
#include	"timing.h"
#include	"keystat.h"
#include	"debugsub.h"


#ifdef BETA_RELEASE
#define		OPENING_WAIT		1500
#endif

#if defined(SUPPORT_PC9821)
		const TCHAR szAppCaption[] = _T("Neko Project-21");
#elif defined(CPUCORE_IA32)
		const TCHAR szAppCaption[] = _T("Neko Project II (IA-32)");
#else
		const TCHAR szAppCaption[] = _T("Neko Project II (C Version)");
#endif
static	const TCHAR	szClassName[] = _T("NP2-MainWindow");
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPrev;

		NP2OSCFG	np2oscfg = {
						CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
						KEY_UNKNOWN, 0,
						0, 0, 0, {1, 2, 2, 1},
						0, 0, 0};

		TCHAR		modulefile[MAX_PATH];
		TCHAR		fddfolder[MAX_PATH];
		TCHAR		hddfolder[MAX_PATH];
		TCHAR		bmpfilefolder[MAX_PATH];

static	UINT		framecnt = 0;
static	UINT		waitcnt = 0;
static	UINT		framemax = 1;
static	int			np2opening = 1;
static	int			np2quitmsg = 0;
static	UINT8	scrnmode;


static const TCHAR np2help[] = _T("np2.chm");
static const TCHAR np2flagext[] = _T("S%02d");


static void winuienter(void) {

	soundmng_disable(SNDPROC_MAIN);
	scrnmng_topwinui();
}

static void winuileave(void) {

	scrnmng_clearwinui();
	soundmng_enable(SNDPROC_MAIN);
}

static void changescreen(UINT8 newmode) {

	UINT8	change;
	UINT8	renewal;

	change = scrnmode ^ newmode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	if (newmode & SCRNMODE_FULLSCREEN) {
		renewal |= (change & SCRNMODE_HIGHCOLOR);
	}
	else {
		renewal |= (change & SCRNMODE_ROTATEMASK);
	}
	if (renewal) {
		soundmng_stop();
		mousemng_disable(MOUSEPROC_WINUI);
		scrnmng_destroy();
		if (scrnmng_create(newmode) == SUCCESS) {
			scrnmode = newmode;
		}
		else {
			if (scrnmng_create(scrnmode) != SUCCESS) {
				PostQuitMessage(0);
				return;
			}
		}
		scrndraw_redraw();
		mousemng_enable(MOUSEPROC_WINUI);
		soundmng_play();
	}
	else {
		scrnmode = newmode;
	}
}


// ---- resume and statsave

#define	SUPPORT_RESUME

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
static void getstatfilename(TCHAR *path, const TCHAR *ext, int size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, str_dot, size);
	file_catname(path, ext, size);
}

static int flagsave(const TCHAR *ext) {

	int		ret;
	TCHAR	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	soundmng_stop();
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	soundmng_play();
	return(ret);
}

static void flagdelete(const TCHAR *ext) {

	TCHAR	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	file_delete(path);
}

static int flagload(const TCHAR *ext, const TCHAR *title, BOOL force) {

	int		ret;
	int		id;
	TCHAR	path[MAX_PATH];
	TCHAR	buf[1024];

	getstatfilename(path, ext, NELEMENTS(path));
	winuienter();
	id = IDYES;
	ret = statsave_check(path, buf, NELEMENTS(buf));
	if (ret & (~STATFLAG_DISKCHG)) {
		MessageBox(hWndMain, _T("Couldn't restart"), title,
										MB_OK | MB_ICONSTOP);
		id = IDNO;
	}
	else if ((!force) && (ret & STATFLAG_DISKCHG)) {
		TCHAR buf2[1024 + 256];
		wsprintf(buf2, _T("Conflict!\n\n%s\nContinue?"), buf);
		id = MessageBox(hWndMain, buf2, title,
										MB_YESNOCANCEL | MB_ICONQUESTION);
	}
	if (id == IDYES) {
		statsave_load(path);
	}
	sysmng_workclockreset();
	sysmng_updatecaption();
	winuileave();
	return(id);
}
#endif


// ---- proc

static void np2cmd(HWND hWnd, UINT16 cmd) {

	UINT	update;

	update = 0;
	switch(cmd) {
		case IDM_RESET:
			pccore_cfgupdate();
			pccore_reset();
			break;

		case IDM_CONFIG:
			winuienter();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIG),
											hWnd, (DLGPROC)CfgDialogProc);
			winuileave();
			break;

		case IDM_NEWDISK:
			winuienter();
			dialog_newdisk(hWnd);
			winuileave();
			break;

		case IDM_CHANGEFONT:
			winuienter();
			dialog_font(hWnd);
			winuileave();
			break;

		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0L);
			break;

		case IDM_FDD1OPEN:
			winuienter();
			dialog_changefdd(hWnd, 0);
			winuileave();
			break;

		case IDM_FDD1EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case IDM_FDD2OPEN:
			winuienter();
			dialog_changefdd(hWnd, 1);
			winuileave();
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(1, NULL, 0);
			break;

		case IDM_SASI1OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0);
			winuileave();
			break;

		case IDM_SASI1EJECT:
			diskdrv_sethdd(0, NULL);
			break;

		case IDM_SASI2OPEN:
			winuienter();
			dialog_changehdd(hWnd, 1);
			winuileave();
			break;

		case IDM_SASI2EJECT:
			diskdrv_sethdd(1, NULL);
			break;

		case IDM_WINDOW:
			changescreen(scrnmode & (~SCRNMODE_FULLSCREEN));
			break;

		case IDM_FULLSCREEN:
			changescreen(scrnmode | SCRNMODE_FULLSCREEN);
			break;

		case IDM_ROLNORMAL:
			xmenu_setroltate(0);
			changescreen(scrnmode & (~SCRNMODE_ROTATEMASK));
			break;

		case IDM_ROLLEFT:
			xmenu_setroltate(1);
			changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATELEFT);
			break;

		case IDM_ROLRIGHT:
			xmenu_setroltate(2);
			changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATERIGHT);
			break;

		case IDM_DISPSYNC:
			xmenu_setdispmode(np2cfg.DISPSYNC ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			xmenu_setraster(np2cfg.RASTER ^ 1);
			if (np2cfg.RASTER) {
				changescreen(scrnmode | SCRNMODE_HIGHCOLOR);
			}
			else {
				changescreen(scrnmode & (~SCRNMODE_HIGHCOLOR));
			}
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOWAIT:
			xmenu_setwaitflg(np2oscfg.NOWAIT ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_AUTOFPS:
			xmenu_setframe(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_60FPS:
			xmenu_setframe(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_30FPS:
			xmenu_setframe(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_20FPS:
			xmenu_setframe(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_15FPS:
			xmenu_setframe(4);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SCREENOPT:
			winuienter();
			dialog_scropt(hWnd);
			winuileave();
			break;

		case IDM_KEY:
			xmenu_setkey(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY1:
			xmenu_setkey(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY2:
			xmenu_setkey(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XSHIFT:
			xmenu_setxshift(np2cfg.XSHIFT ^ 1);
			keystat_forcerelease(0x70);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XCTRL:
			xmenu_setxshift(np2cfg.XSHIFT ^ 2);
			keystat_forcerelease(0x74);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XGRPH:
			xmenu_setxshift(np2cfg.XSHIFT ^ 4);
			keystat_forcerelease(0x73);
			update |= SYS_UPDATECFG;
			break;

		case IDM_F12MOUSE:
			xmenu_setf12copy(0);
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COPY:
			xmenu_setf12copy(1);
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12STOP:
			xmenu_setf12copy(2);
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12EQU:
			xmenu_setf12copy(3);
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COMMA:
			xmenu_setf12copy(4);
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_BEEPOFF:
			xmenu_setbeepvol(0);
			beep_setvol(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPLOW:
			xmenu_setbeepvol(1);
			beep_setvol(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPMID:
			xmenu_setbeepvol(2);
			beep_setvol(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPHIGH:
			xmenu_setbeepvol(3);
			beep_setvol(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOSOUND:
			xmenu_setsound(0x00);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_14:
			xmenu_setsound(0x01);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_26K:
			xmenu_setsound(0x02);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_86:
			xmenu_setsound(0x04);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_26_86:
			xmenu_setsound(0x06);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_86_CB:
			xmenu_setsound(0x14);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_118:
			xmenu_setsound(0x08);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SPEAKBOARD:
			xmenu_setsound(0x20);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SPARKBOARD:
			xmenu_setsound(0x40);
			update |= SYS_UPDATECFG;
			break;

		case IDM_AMD98:
			xmenu_setsound(0x80);
			update |= SYS_UPDATECFG;
			break;

		case IDM_JASTSOUND:
			xmenu_setjastsound(np2oscfg.jastsnd ^ 1);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_SEEKSND:
			xmenu_setmotorflg(np2cfg.MOTOR ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM640:
			xmenu_setextmem(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM16:
			xmenu_setextmem(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM36:
			xmenu_setextmem(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM76:
			xmenu_setextmem(7);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM116:
			xmenu_setextmem(11);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM136:
			xmenu_setextmem(13);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MOUSE:
			mousemng_toggle(MOUSEPROC_SYSTEM);
			xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MPUPC98:
			winuienter();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MPUPC98),
											hWnd, (DLGPROC)MidiDialogProc);
			winuileave();
			break;

		case IDM_MIDIPANIC:									// ver0.29
			rs232c_midipanic();
			mpu98ii_midipanic();
			pc9861k_midipanic();
			break;

		case IDM_SNDOPT:
			winuienter();
			dialog_sndopt(hWnd);
			winuileave();
			break;

		case IDM_BMPSAVE:
			winuienter();
			dialog_writebmp(hWnd);
			winuileave();
			break;
#if defined(SUPPPORT_S98)
		case IDM_S98LOGGING:
			winuienter();
			dialog_s98(hWnd);
			winuileave();
			break;
#endif
#if defined(SUPPORT_WAVEREC)
		case IDM_WAVEREC:
			winuienter();
			dialog_waverec(hWnd);
			winuileave();
			break;
#endif
		case IDM_CALENDAR:
			winuienter();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CALENDAR),
											hWnd, (DLGPROC)ClndDialogProc);
			winuileave();
			break;

		case IDM_JOYX:
			xmenu_setbtnmode(np2cfg.BTN_MODE ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_RAPID:
			xmenu_setbtnrapid(np2cfg.BTN_RAPID ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MSRAPID:
			xmenu_setmsrapid(np2cfg.MOUSERAPID ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_I286SAVE:
			debugsub_status();
			break;

		case IDM_HELP:
			ShellExecute(hWnd, NULL, file_getcd(np2help),
											NULL, NULL, SW_SHOWNORMAL);
			break;

		case IDM_ABOUT:
			winuienter();
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT),
									hWnd, (DLGPROC)AboutDialogProc);
			winuileave();
			break;
	}
	sysmng_update(update);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hdc;
	UINT		update;

	switch (msg) {
		case WM_CREATE:
#ifndef __GNUC__
			WINNLSEnableIME(hWnd, FALSE);
#endif
			break;

		case WM_SYSCOMMAND:
			update = 0;
			switch(wParam) {
				case IDM_MEMORYDUMP:
					debugsub_memorydump();
					break;

				default:
					return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			sysmng_update(update);
			break;

		case WM_COMMAND:
			np2cmd(hWnd, LOWORD(wParam));
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE) {
				scrnmng_update();
				keystat_allrelease();
				mousemng_enable(MOUSEPROC_BG);
			}
			else {
				mousemng_disable(MOUSEPROC_BG);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (np2opening) {
				HINSTANCE	hinst;
				RECT		rect;
				int			width;
				int			height;
				HBITMAP		hbmp;
				BITMAP		bmp;
				HDC			hmdc;
				HBRUSH		hbrush;

#if defined(_WIN64)
				hinst = reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
#else
				hinst = reinterpret_cast<HINSTANCE>(::GetWindowLong(hWnd, GWL_HINSTANCE));
#endif
				GetClientRect(hWnd, &rect);
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				hbmp = LoadBitmap(hinst, _T("NP2BMP"));
				GetObject(hbmp, sizeof(BITMAP), &bmp);
				hbrush = (HBRUSH)SelectObject(hdc,
												GetStockObject(BLACK_BRUSH));
				PatBlt(hdc, 0, 0, width, height, PATCOPY);
				SelectObject(hdc, hbrush);
				hmdc = CreateCompatibleDC(hdc);
				SelectObject(hmdc, hbmp);
				BitBlt(hdc, (width - bmp.bmWidth) / 2,
						(height - bmp.bmHeight) / 2,
							bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
				DeleteDC(hmdc);
				DeleteObject(hbmp);
			}
			else {
//				scrnmng_update();
				scrndraw_redraw();
			}
			EndPaint(hWnd, &ps);
			break;

		case WM_QUERYNEWPALETTE:
			scrnmng_querypalette();
			break;

		case WM_MOVE:
			if ((!scrnmng_isfullscreen()) &&
				(!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE)))) {
				GetWindowRect(hWnd, &rc);
				np2oscfg.winx = rc.left;
				np2oscfg.winy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_SIZE:
			if (wParam == SIZE_RESTORED) {
				scrnmng_restoresize();
			}
			break;

		case WM_ENTERMENULOOP:
			winuienter();
			if (scrnmng_isfullscreen()) {
				DrawMenuBar(hWnd);
			}
			break;

		case WM_EXITMENULOOP:
			winuileave();
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_MAIN);
			mousemng_disable(MOUSEPROC_WINUI);
			break;

		case WM_EXITSIZEMOVE:
			mousemng_enable(MOUSEPROC_WINUI);
			soundmng_enable(SNDPROC_MAIN);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				return(DefWindowProc(hWnd, WM_SYSKEYDOWN, VK_F10, lParam));
			}
			if ((wParam == VK_F12) && (!np2oscfg.F12COPY)) {
				mousemng_toggle(MOUSEPROC_SYSTEM);
				xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
				sysmng_update(SYS_UPDATECFG);
			}
			else {
				winkbd_keydown(wParam, lParam);
			}
			break;

		case WM_KEYUP:
			if (wParam == VK_F11) {
				return(DefWindowProc(hWnd, WM_SYSKEYUP, VK_F10, lParam));
			}
			if ((wParam != VK_F12) || (np2oscfg.F12COPY)) {
				winkbd_keyup(wParam, lParam);
			}
			break;

		case WM_SYSKEYDOWN:
			winkbd_keydown(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkbd_keyup(wParam, lParam);
			break;

		case WM_LBUTTONDOWN:
			if (!mousemng_buttonevent(MOUSEMNG_LEFTDOWN)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_LBUTTONUP:
			if (!mousemng_buttonevent(MOUSEMNG_LEFTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_MBUTTONDOWN:									// ver0.26
			mousemng_toggle(MOUSEPROC_SYSTEM);
			xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
			sysmng_update(SYS_UPDATECFG);
			break;

		case WM_RBUTTONDOWN:
			if (!mousemng_buttonevent(MOUSEMNG_RIGHTDOWN)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_RBUTTONUP:
			if (!mousemng_buttonevent(MOUSEMNG_RIGHTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_NP2CMD:
			switch(LOWORD(lParam)) {
				case NP2CMD_EXIT:
					np2quitmsg = 1;
					PostQuitMessage(0);
					break;

				case NP2CMD_EXIT2:
					np2quitmsg = 2;
					PostQuitMessage(0);
					break;

				case NP2CMD_RESET:
					pccore_cfgupdate();
					pccore_reset();
					break;
			}
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}

static void framereset(void) {

	framecnt = 0;
	sysmng_updatecaption();
	memdbg_process();
	skbdwin_process();
}

static void processwait(UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset();
	}
	else {
		Sleep(1);
	}
	soundmng_sync();
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow) {
	WNDCLASS	wc;
	MSG			msg;
	HWND		hWnd;
	int			i;
#ifdef OPENING_WAIT
	UINT32		tick;
#endif

	_MEM_INIT();

	GetModuleFileName(NULL, modulefile, NELEMENTS(modulefile));
	dosio_init();
	file_setcd(modulefile);
	np2arg_analize();
	initload();
	memdbg_readini();
	skbdwin_readini();

	rand_setseed((unsigned)time(NULL));

	if ((hWnd = FindWindow(szClassName, NULL)) != NULL) {
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		dosio_term();
		return(FALSE);
	}

	hInst = hInstance;
	hPrev = hPreInst;
	TRACEINIT();

	if (np2oscfg.KEYBOARD >= KEY_TYPEMAX) {							// ver0.28
		int keytype = GetKeyboardType(1);
		if ((keytype & 0xff00) == 0x0d00) {
			np2oscfg.KEYBOARD = KEY_PC98;
		}
		else if (!keytype) {
			np2oscfg.KEYBOARD = KEY_KEY101;
		}
		else {
			np2oscfg.KEYBOARD = KEY_KEY106;
		}
	}
	keystat_initialize();

	if (!hPreInst) {
		wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
		wc.lpszClassName = szClassName;
		if (!RegisterClass(&wc)) {
			return(FALSE);
		}
	}
	memdbg_initialize(hInstance);
	skbdwin_initialize(hInstance);

	mousemng_initialize();

	hWnd = CreateWindowEx(0, szClassName, szAppCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						np2oscfg.winx, np2oscfg.winy, 640, 400,
						NULL, NULL, hInstance, NULL);
	hWndMain = hWnd;
	scrnmng_initialize();

	xmenu_setroltate(0);
	xmenu_setdispmode(np2cfg.DISPSYNC);
	xmenu_setraster(np2cfg.RASTER);
	xmenu_setwaitflg(np2oscfg.NOWAIT);
	xmenu_setframe(np2oscfg.DRAW_SKIP);

	xmenu_setkey(0);
	xmenu_setxshift(0);
	xmenu_setf12copy(np2oscfg.F12COPY);
	xmenu_setbeepvol(np2cfg.BEEP_VOL);
	xmenu_setsound(np2cfg.SOUND_SW);
	xmenu_setjastsound(np2oscfg.jastsnd);
	xmenu_setmotorflg(np2cfg.MOTOR);
	xmenu_setextmem(np2cfg.EXTMEM);
	xmenu_setmouse(np2oscfg.MOUSE_SW);

	xmenu_setbtnmode(np2cfg.BTN_MODE);
	xmenu_setbtnrapid(np2cfg.BTN_RAPID);
	xmenu_setmsrapid(np2cfg.MOUSERAPID);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifdef OPENING_WAIT
	tick = GetTickCount();
#endif

	sysmenu_initialize();
	xmenu_initialize();
	DrawMenuBar(hWnd);

	// ver0.30
	if (file_attr_c(np2help) == (short)-1) {
		EnableMenuItem(GetMenu(hWnd), IDM_HELP, MF_GRAYED);
	}

	scrnmode = 0;
	if (np2arg.fullscreen) {
		scrnmode |= SCRNMODE_FULLSCREEN;
	}
	if (np2cfg.RASTER) {
		scrnmode |= SCRNMODE_HIGHCOLOR;
	}
	if (scrnmng_create(scrnmode) != SUCCESS) {
		scrnmode ^= SCRNMODE_FULLSCREEN;
		if (scrnmng_create(scrnmode) != SUCCESS) {
			MessageBox(hWnd, _T("Couldn't create DirectDraw Object"),
										szAppCaption, MB_OK | MB_ICONSTOP);
			return(FALSE);
		}
	}

	soundmng_initialize();
	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mousemng_enable(MOUSEPROC_SYSTEM);
	}

	commng_initialize();
	sysmng_initialize();

	joy_init();
	pccore_init();
	S98_init();

#ifdef OPENING_WAIT
	while((GetTickCount() - tick) < OPENING_WAIT);
#endif

	scrndraw_redraw();

	pccore_reset();

	np2opening = 0;

	// れじうむ
#if defined(SUPPORT_RESUME)
	if (np2oscfg.resume) {
		int		id;

		id = flagload(str_sav, str_resume, FALSE);
		if (id == IDYES) {
			for (i=0; i<4; i++) np2arg.disk[i] = NULL;
		}
		else if (id == IDCANCEL) {
			DestroyWindow(hWnd);
			mousemng_disable(MOUSEPROC_WINUI);
			S98_trash();
			pccore_term();
			soundmng_deinitialize();
			scrnmng_destroy();
			TRACETERM();
			dosio_term();
			return(0);
		}
	}
#endif

//	リセットしてから… コマンドラインのディスク挿入。				// ver0.29
	for (i=0; i<4; i++) {
		if (np2arg.disk[i]) {
			diskdrv_readyfdd((REG8)i, np2arg.disk[i], 0);
		}
	}

	memdbg_create();
	skbdwin_create();

	while(1) {
		if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			if ((msg.message != WM_SYSKEYDOWN) &&
				(msg.message != WM_SYSKEYUP)) {
				TranslateMessage(&msg);
			}
			DispatchMessage(&msg);
		}
		else {
			if (np2oscfg.NOWAIT) {
				joy_flash();
				mousemng_sync();
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
					joy_flash();
					mousemng_sync();
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
					joy_flash();
					mousemng_sync();
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
						framereset();
					}
				}
				else {
					processwait(waitcnt);
					waitcnt = framecnt;
				}
			}
		}
	}

	pccore_cfgupdate();

	mousemng_disable(MOUSEPROC_WINUI);
	S98_trash();

#if defined(SUPPORT_RESUME)
	if (np2oscfg.resume) {
		flagsave(str_sav);
	}
	else {
		flagdelete(str_sav);
	}
#endif

	pccore_term();
	memdbg_destroy();
	skbdwin_destroy();

	soundmng_deinitialize();
	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
		memdbg_writeini();
		skbdwin_writeini();
	}

	skbdwin_deinitialize();

	TRACETERM();
	_MEM_USED("report.txt");
	dosio_term();

	return static_cast<int>(msg.wParam);
}

