/**
 * @file	np2.cpp
 * @brief	main window
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/02/17 10:36:05 $
 */

#include "compiler.h"
#include <time.h>
#include <winsock.h>
#ifndef __GNUC__
#include <winnls32.h>
#endif
#include "resource.h"
#include "strres.h"
#include "parts.h"
#include "np2.h"
#include "np2arg.h"
#include "dosio.h"
#include "extromio.h"
#include "commng.h"
#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "winkbd.h"
#include "ini.h"
#include "menu.h"
#include "winloc.h"
#include "sstp.h"
#include "sstpmsg.h"
#include "toolwin.h"
#include "juliet.h"
#include "np2class.h"
#include "dialog.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "pc9861k.h"
#include "mpu98ii.h"
#include "bios.h"
#include "scrndraw.h"
#include "sound.h"
#include "beep.h"
#include "s98.h"
#include "diskdrv.h"
#include "fddfile.h"
#include "timing.h"
#include "keystat.h"
#include "debugsub.h"
#include "subwind.h"
#include "viewer.h"
#if !defined(_WIN64)
#include "cputype.h"
#endif
#if defined(SUPPORT_DCLOCK)
#include "dclock.h"
#endif

#ifdef BETA_RELEASE
#define		OPENING_WAIT		1500
#endif

static	TCHAR		szClassName[] = _T("NP2-MainWindow");
		HWND		g_hWndMain;
		HINSTANCE	g_hInstance;
		HINSTANCE	g_hPrevInst;
#if !defined(_WIN64)
		int			mmxflag;
#endif
		UINT8		np2break = 0;									// ver0.30
		BOOL		winui_en;

		NP2OSCFG	np2oscfg = {
						OEMTEXT(PROJECTNAME) OEMTEXT(PROJECTSUBNAME),
						OEMTEXT("NP2"),
						CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, 0, 0, 0, 1, 0, 0,
						0, 0, KEY_UNKNOWN, 0,
						0, 0, 0, {1, 2, 2, 1},
						{5, 0, 0x3e, 19200,
						 OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
						{0, 0, 0x3e, 19200,
						 OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
						{0, 0, 0x3e, 19200,
						 OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
						{0, 0, 0x3e, 19200,
						 OEMTEXT(""), OEMTEXT(""), OEMTEXT(""), OEMTEXT("")},
						0xffffff, 0xffbf6a, 0, 0,
						0, 1, 0, 9801, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		OEMCHAR		fddfolder[MAX_PATH];
		OEMCHAR		hddfolder[MAX_PATH];
		OEMCHAR		bmpfilefolder[MAX_PATH];
		OEMCHAR		modulefile[MAX_PATH];

static	UINT		framecnt = 0;
static	UINT		waitcnt = 0;
static	UINT		framemax = 1;
static	UINT8		np2stopemulate = 0;
static	int			np2opening = 1;
static	int			np2quitmsg = 0;
static	UINT8		scrnmode;
static	WINLOCEX	smwlex;
static	HMODULE		s_hModResource;

static const OEMCHAR np2help[] = OEMTEXT("np2.chm");
static const OEMCHAR np2flagext[] = OEMTEXT("S%02d");
#if defined(_WIN64)
static const OEMCHAR szNp2ResDll[] = OEMTEXT("np2x64_%u.dll");
#else	// defined(_WIN64)
static const OEMCHAR szNp2ResDll[] = OEMTEXT("np2_%u.dll");
#endif	// defined(_WIN64)


// ----

static int messagebox(HWND hWnd, LPCTSTR lpcszText, UINT uType)
{
	int		nRet;
	LPTSTR	lpszText;

#if defined(OSLANG_UTF8)
	TCHAR szCation[128];
	oemtotchar(szCaption, NELEMENTS(szCaption), np2oscfg.titles, -1);
#else	// defined(OSLANG_UTF8)
	LPCTSTR szCaption = np2oscfg.titles;
#endif	// defined(OSLANG_UTF8)

	nRet = 0;
	if (HIWORD(lpcszText))
	{
		nRet = MessageBox(hWnd, lpcszText, szCaption, uType);
	}
	else
	{
		lpszText = lockstringresource(lpcszText);
		nRet = MessageBox(hWnd, lpszText, szCaption, uType);
		unlockstringresource(lpszText);
	}
	return nRet;
}

// ----

static HINSTANCE loadextinst(HINSTANCE hInstance)
{
	OEMCHAR	szPath[MAX_PATH];
	OEMCHAR	szDllName[32];
	HMODULE hMod;

	file_cpyname(szPath, modulefile, NELEMENTS(szPath));
	file_cutname(szPath);
	OEMSPRINTF(szDllName, szNp2ResDll, GetOEMCP());
	file_catname(szPath, szDllName, NELEMENTS(szPath));
	hMod = LoadLibrary(szPath);
	s_hModResource = hMod;
	if (hMod != NULL)
	{
		hInstance = (HINSTANCE)hMod;
	}
	return(hInstance);
}

static void unloadextinst(void)
{
	HMODULE hMod;

	hMod = s_hModResource;
	s_hModResource = 0;
	if (hMod)
	{
		FreeLibrary(hMod);
	}
}


// ----

static void winuienter(void) {

	winui_en = TRUE;
	soundmng_disable(SNDPROC_MAIN);
	scrnmng_topwinui();
}

static void winuileave(void) {

	scrnmng_clearwinui();
	soundmng_enable(SNDPROC_MAIN);
	winui_en = FALSE;
}

WINLOCEX np2_winlocexallwin(HWND base) {

	UINT	i;
	UINT	cnt;
	HWND	list[5];

	cnt = 0;
	list[cnt++] = g_hWndMain;
	list[cnt++] = toolwin_gethwnd();
	list[cnt++] = kdispwin_gethwnd();
	list[cnt++] = skbdwin_gethwnd();
	list[cnt++] = mdbgwin_gethwnd();
	for (i=0; i<cnt; i++) {
		if (list[i] == base) {
			list[i] = NULL;
		}
	}
	if (base != g_hWndMain) {		// hWndMainのみ全体移動
		base = NULL;
	}
	return(winlocex_create(base, list, cnt));
}

static void changescreen(UINT8 newmode) {

	UINT8		change;
	UINT8		renewal;
	WINLOCEX	wlex;

	change = scrnmode ^ newmode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	wlex = NULL;
	if (newmode & SCRNMODE_FULLSCREEN) {
		renewal |= (change & SCRNMODE_HIGHCOLOR);
	}
	else {
		renewal |= (change & SCRNMODE_ROTATEMASK);
	}
	if (renewal) {
		if (renewal & SCRNMODE_FULLSCREEN) {
			toolwin_destroy();
			kdispwin_destroy();
			skbdwin_destroy();
			mdbgwin_destroy();
		}
		else if (renewal & SCRNMODE_ROTATEMASK) {
			wlex = np2_winlocexallwin(g_hWndMain);
			winlocex_setholdwnd(wlex, g_hWndMain);
		}
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
		if (renewal & SCRNMODE_FULLSCREEN) {
			if (!scrnmng_isfullscreen()) {
				if (np2oscfg.toolwin) {
					toolwin_create(g_hInstance);
				}
				if (np2oscfg.keydisp) {
					kdispwin_create(g_hInstance);
				}
			}
		}
		else if (renewal & SCRNMODE_ROTATEMASK) {
			winlocex_move(wlex);
			winlocex_destroy(wlex);
		}
		mousemng_enable(MOUSEPROC_WINUI);
		soundmng_play();
	}
	else {
		scrnmode = newmode;
	}
}

static void wincentering(HWND hWnd) {

	RECT	rc;
	int		width;
	int		height;

	GetWindowRect(hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	np2oscfg.winx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	np2oscfg.winy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if (np2oscfg.winx < 0) {
		np2oscfg.winx = 0;
	}
	if (np2oscfg.winy < 0) {
		np2oscfg.winy = 0;
	}
	sysmng_update(SYS_UPDATEOSCFG);
	MoveWindow(g_hWndMain, np2oscfg.winx, np2oscfg.winy, width, height, TRUE);
}

void np2active_renewal(void) {										// ver0.30

	if (np2break & (~NP2BREAK_MAIN)) {
		np2stopemulate = 2;
		soundmng_disable(SNDPROC_MASTER);
	}
	else if (np2break & NP2BREAK_MAIN) {
		if (np2oscfg.background & 1) {
			np2stopemulate = 1;
		}
		else {
			np2stopemulate = 0;
		}
		if (np2oscfg.background) {
			soundmng_disable(SNDPROC_MASTER);
		}
		else {
			soundmng_enable(SNDPROC_MASTER);
		}
	}
	else {
		np2stopemulate = 0;
		soundmng_enable(SNDPROC_MASTER);
	}
}


// ---- resume and statsave

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
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
	soundmng_stop();
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	soundmng_play();
	return(ret);
}

static void flagdelete(const OEMCHAR *ext) {

	OEMCHAR	path[MAX_PATH];

	getstatfilename(path, ext, NELEMENTS(path));
	file_delete(path);
}

static int flagload(HWND hWnd, const OEMCHAR *ext, LPCTSTR title, BOOL force)
{
	int		nRet;
	int		nID;
	OEMCHAR	szPath[MAX_PATH];
	OEMCHAR	szStat[1024];
	TCHAR	szFormat[256];
	TCHAR	szMessage[1024 + 256];

	getstatfilename(szPath, ext, NELEMENTS(szPath));
	winuienter();
	nID = IDYES;
	nRet = statsave_check(szPath, szStat, NELEMENTS(szStat));
	if (nRet & (~STATFLAG_DISKCHG))
	{
		messagebox(hWnd, MAKEINTRESOURCE(IDS_ERROR_RESUME),
													MB_OK | MB_ICONSTOP);
		nID = IDNO;
	}
	else if ((!force) && (nRet & STATFLAG_DISKCHG))
	{
#if defined(OSLANG_UTF8)
		TCHAR szStat2[128];
		oemtotchar(szStat2, NELEMENTS(szStat2), szStat, -1);
#else	// defined(OSLANG_UTF8)
		LPCTSTR szStat2 = szStat;
#endif	// defined(OSLANG_UTF8)
		loadstringresource(IDS_CONFIRM_RESUME, szFormat, NELEMENTS(szFormat));
		wsprintf(szMessage, szFormat, szStat2);
		nID = messagebox(hWnd, szMessage, MB_YESNOCANCEL | MB_ICONQUESTION);
	}
	if (nID == IDYES)
	{
		statsave_load(szPath);
		toolwin_setfdd(0, fdd_diskname(0));
		toolwin_setfdd(1, fdd_diskname(1));
	}
	sysmng_workclockreset();
	sysmng_updatecaption(1);
	winuileave();
	return nID;
}
#endif


// ---- proc

static void np2popup(HWND hWnd, LPARAM lp) {

	HMENU	mainmenu;
	HMENU	hMenu;
	POINT	pt;

	mainmenu = (HMENU)GetWindowLongPtr(hWnd, NP2GWLP_HMENU);
	if (mainmenu == NULL) {
		return;
	}
	hMenu = CreatePopupMenu();
	menu_addmenubar(hMenu, mainmenu);
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenu);
}

static void OnCommand(HWND hWnd, WPARAM wParam)
{
	HINSTANCE	hInstance;
	UINT		update;
	UINT		uID;
	BOOL		b;

	hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);;
	update = 0;
	uID = LOWORD(wParam);
	switch(uID)
	{
		case IDM_RESET:
			b = FALSE;
			if (!np2oscfg.comfirm)
			{
				b = TRUE;
			}
			else if (sstpconfirm_reset())
			{
				winuienter();
				if (messagebox(hWnd, MAKEINTRESOURCE(IDS_CONFIRM_RESET),
									MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					b = TRUE;
				}
				winuileave();
			}
			if (b)
			{
				sstpmsg_reset();
				juliet_YMF288Reset();
				pccore_cfgupdate();
				pccore_reset();
			}
			break;

		case IDM_CONFIG:
			winuienter();
			sstpmsg_config();
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_CONFIG),
									hWnd, (DLGPROC)CfgDialogProc);
			if (!scrnmng_isfullscreen()) {
				UINT8 thick;
				thick = (GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME)?1:0;
				if (thick != np2oscfg.thickframe) {
					WINLOCEX wlex;
					wlex = np2_winlocexallwin(hWnd);
					winlocex_setholdwnd(wlex, hWnd);
					np2class_frametype(hWnd, np2oscfg.thickframe);
					winlocex_move(wlex);
					winlocex_destroy(wlex);
				}
			}
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
			toolwin_setfdd(0, NULL);
			break;

		case IDM_FDD2OPEN:
			winuienter();
			dialog_changefdd(hWnd, 1);
			winuileave();
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(1, NULL, 0);
			toolwin_setfdd(1, NULL);
			break;

		case IDM_FDD3OPEN:
			winuienter();
			dialog_changefdd(hWnd, 2);
			winuileave();
			break;

		case IDM_FDD3EJECT:
			diskdrv_setfdd(2, NULL, 0);
			toolwin_setfdd(2, NULL);
			break;

		case IDM_FDD4OPEN:
			winuienter();
			dialog_changefdd(hWnd, 3);
			winuileave();
			break;

		case IDM_FDD4EJECT:
			diskdrv_setfdd(3, NULL, 0);
			toolwin_setfdd(3, NULL);
			break;

		case IDM_IDE0OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x00);
			winuileave();
			break;

		case IDM_IDE0EJECT:
			diskdrv_sethdd(0x00, NULL);
			break;

		case IDM_IDE1OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x01);
			winuileave();
			break;

		case IDM_IDE1EJECT:
			diskdrv_sethdd(0x01, NULL);
			break;

#if defined(SUPPORT_IDEIO)
		case IDM_IDE2OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x02);
			winuileave();
			break;

		case IDM_IDE2EJECT:
			diskdrv_sethdd(0x02, NULL);
			break;
#endif

#if defined(SUPPORT_SCSI)
		case IDM_SCSI0OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x20);
			winuileave();
			break;

		case IDM_SCSI0EJECT:
			diskdrv_sethdd(0x20, NULL);
			break;

		case IDM_SCSI1OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x21);
			winuileave();
			break;

		case IDM_SCSI1EJECT:
			diskdrv_sethdd(0x21, NULL);
			break;

		case IDM_SCSI2OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x22);
			winuileave();
			break;

		case IDM_SCSI2EJECT:
			diskdrv_sethdd(0x22, NULL);
			break;

		case IDM_SCSI3OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x23);
			winuileave();
			break;

		case IDM_SCSI3EJECT:
			diskdrv_sethdd(0x23, NULL);
			break;
#endif

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
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY1:
			xmenu_setkey(1);
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY2:
			xmenu_setkey(2);
			keystat_resetjoykey();
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
			winkbd_setf12(0);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COPY:
			xmenu_setf12copy(1);
			winkbd_resetf12();
			winkbd_setf12(1);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12STOP:
			xmenu_setf12copy(2);
			winkbd_resetf12();
			winkbd_setf12(2);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12EQU:
			xmenu_setf12copy(3);
			winkbd_resetf12();
			winkbd_setf12(3);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COMMA:
			xmenu_setf12copy(4);
			winkbd_resetf12();
			winkbd_setf12(4);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_USERKEY1:
			xmenu_setf12copy(5);
			winkbd_resetf12();
			winkbd_setf12(5);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_USERKEY2:
			xmenu_setf12copy(6);
			winkbd_resetf12();
			winkbd_setf12(6);
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

#if defined(SUPPORT_PX)
		case IDM_PX1:
			xmenu_setsound(0x30);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PX2:
			xmenu_setsound(0x50);
			update |= SYS_UPDATECFG;
			break;
#endif	// defined(SUPPORT_PX)

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

		case IDM_SERIAL1:
			winuienter();
			dialog_serial(hWnd);
			winuileave();
			break;

		case IDM_MPUPC98:
			winuienter();
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_MPUPC98),
											hWnd, (DLGPROC)MidiDialogProc);
			winuileave();
			break;

		case IDM_MIDIPANIC:
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
#if defined(SUPPORT_S98)
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
		case IDM_DISPCLOCK:
			xmenu_setdispclk(np2oscfg.DISPCLK ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_DISPFRAME:
			xmenu_setdispclk(np2oscfg.DISPCLK ^ 2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_CALENDAR:
			winuienter();
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_CALENDAR),
											hWnd, (DLGPROC)ClndDialogProc);
			winuileave();
			break;

		case IDM_ALTENTER:
			xmenu_setshortcut(np2oscfg.shortcut ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_ALTF4:
			xmenu_setshortcut(np2oscfg.shortcut ^ 2);
			update |= SYS_UPDATECFG;
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

		case IDM_SSTP:
			xmenu_setsstp(np2oscfg.sstp ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_CPUSAVE:
			debugsub_status();
			break;

		case IDM_HELP:
			ShellExecute(hWnd, NULL, file_getcd(np2help),
									NULL, NULL, SW_SHOWNORMAL);
			break;

		case IDM_ABOUT:
			sstpmsg_about();
			if (sstp_result() != SSTP_SENDING) {
				winuienter();
				DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUT),
								hWnd, (DLGPROC)AboutDialogProc);
				winuileave();
			}
			break;

		default:
#if defined(SUPPORT_STATSAVE)
			if ((uID >= IDM_FLAGSAVE) &&
				(uID < (IDM_FLAGSAVE + SUPPORT_STATSAVE))) {
				OEMCHAR ext[4];
				OEMSPRINTF(ext, np2flagext, uID - IDM_FLAGSAVE);
				flagsave(ext);
			}
			else if ((uID >= IDM_FLAGLOAD) &&
				(uID < (IDM_FLAGLOAD + SUPPORT_STATSAVE))) {
				OEMCHAR ext[4];
				OEMSPRINTF(ext, np2flagext, uID - IDM_FLAGLOAD);
				flagload(hWnd, ext, _T("Status Load"), TRUE);
			}
#endif
			break;
	}
	sysmng_update(update);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hdc;
	BOOL		b;
	UINT		update;
	HWND		subwin;
	WINLOCEX	wlex;

	switch (msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			np2class_windowtype(hWnd, np2oscfg.wintype);
			sstp_construct(hWnd);
#ifndef __GNUC__
			WINNLSEnableIME(hWnd, FALSE);
#endif
			break;

		case WM_SYSCOMMAND:
			update = 0;
			switch(wParam) {
				case IDM_TOOLWIN:
					sysmenu_settoolwin(np2oscfg.toolwin ^ 1);
					if (np2oscfg.toolwin) {
						toolwin_create(g_hInstance);
					}
					else {
						toolwin_destroy();
					}
					update |= SYS_UPDATEOSCFG;
					break;

#if defined(SUPPORT_KEYDISP)
				case IDM_KEYDISP:
					sysmenu_setkeydisp(np2oscfg.keydisp ^ 1);
					if (np2oscfg.keydisp) {
						kdispwin_create(g_hInstance);
					}
					else {
						kdispwin_destroy();
					}
					break;
#endif
#if defined(SUPPORT_SOFTKBD)
				case IDM_SOFTKBD:
					skbdwin_create(g_hInstance);
					break;
#endif
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
				case IDM_MEMDBG32:
					mdbgwin_create(g_hInstance);
					break;
#endif
				case IDM_SCREENCENTER:
					if ((!scrnmng_isfullscreen()) &&
						(!(GetWindowLong(hWnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE)))) {
						wlex = np2_winlocexallwin(hWnd);
						wincentering(hWnd);
						winlocex_move(wlex);
						winlocex_destroy(wlex);
					}
					break;

				case IDM_SNAPENABLE:
					sysmenu_setwinsnap(np2oscfg.WINSNAP ^ 1);
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_BACKGROUND:
					sysmenu_setbackground(np2oscfg.background ^ 1);
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_BGSOUND:
					sysmenu_setbgsound(np2oscfg.background ^ 2);
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_MEMORYDUMP:
					debugsub_memorydump();
					break;

				case IDM_DEBUGUTY:
					viewer_open(g_hInstance);
					break;

				case IDM_SCRNMUL4:
				case IDM_SCRNMUL6:
				case IDM_SCRNMUL8:
				case IDM_SCRNMUL10:
				case IDM_SCRNMUL12:
				case IDM_SCRNMUL16:
					if ((!scrnmng_isfullscreen()) &&
						!(GetWindowLong(g_hWndMain, GWL_STYLE) & WS_MINIMIZE))
					{
						sysmenu_setscrnmul((UINT8)(wParam - IDM_SCRNMUL));
						scrnmng_setmultiple((int)(wParam - IDM_SCRNMUL));
					}
					break;

				case SC_MINIMIZE:
					wlex = np2_winlocexallwin(hWnd);
					winlocex_close(wlex);
					winlocex_destroy(wlex);
					return(DefWindowProc(hWnd, msg, wParam, lParam));

				case SC_RESTORE:
					subwin = toolwin_gethwnd();
					if (subwin) {
						ShowWindow(subwin, SW_SHOWNOACTIVATE);
					}
					subwin = kdispwin_gethwnd();
					if (subwin) {
						ShowWindow(subwin, SW_SHOWNOACTIVATE);
					}
					subwin = skbdwin_gethwnd();
					if (subwin) {
						ShowWindow(subwin, SW_SHOWNOACTIVATE);
					}
					subwin = mdbgwin_gethwnd();
					if (subwin) {
						ShowWindow(subwin, SW_SHOWNOACTIVATE);
					}
					return(DefWindowProc(hWnd, msg, wParam, lParam));

				default:
					return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			sysmng_update(update);
			break;

		case WM_COMMAND:
			OnCommand(hWnd, wParam);
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE) {
				np2break &= ~NP2BREAK_MAIN;
				scrnmng_update();
				keystat_allrelease();
				mousemng_enable(MOUSEPROC_BG);
			}
			else {
				np2break |= NP2BREAK_MAIN;
				mousemng_disable(MOUSEPROC_BG);
			}
			np2active_renewal();
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (np2opening) {
				HINSTANCE	hInstance;
				RECT		rect;
				int			width;
				int			height;
				HBITMAP		hbmp;
				BITMAP		bmp;
				HDC			hmdc;
				HBRUSH		hbrush;
				hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
				GetClientRect(hWnd, &rect);
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				hbmp = LoadBitmap(hInstance, _T("NP2BMP"));
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
			winlocex_destroy(smwlex);
			smwlex = np2_winlocexallwin(hWnd);
			scrnmng_entersizing();
			break;

		case WM_MOVING:
			if (np2oscfg.WINSNAP) {
				winlocex_moving(smwlex, (RECT *)lParam);
			}
			break;

		case WM_SIZING:
			scrnmng_sizing((UINT)wParam, (RECT *)lParam);
			break;

		case WM_EXITSIZEMOVE:
			scrnmng_exitsizing();
			winlocex_move(smwlex);
			winlocex_destroy(smwlex);
			smwlex = NULL;
			mousemng_enable(MOUSEPROC_WINUI);
			soundmng_enable(SNDPROC_MAIN);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				np2class_enablemenu(g_hWndMain, TRUE);
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
			if (lParam & 0x20000000) {								// ver0.30
				if ((np2oscfg.shortcut & 1) && (wParam == VK_RETURN)) {
					changescreen(scrnmode ^ SCRNMODE_FULLSCREEN);
					break;
				}
				if ((np2oscfg.shortcut & 2) && (wParam == VK_F4)) {
					SendMessage(hWnd, WM_CLOSE, 0, 0L);
					break;
				}
			}
			winkbd_keydown(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkbd_keyup(wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			if (scrnmng_isfullscreen()) {
				POINT p;
				if (GetCursorPos(&p)) {
					scrnmng_fullscrnmenu(p.y);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			if (!mousemng_buttonevent(MOUSEMNG_LEFTDOWN)) {
				if (!scrnmng_isfullscreen()) {
					if (np2oscfg.wintype == 2) {
						return(SendMessage(hWnd, WM_NCLBUTTONDOWN,
															HTCAPTION, 0L));
					}
				}
#if defined(SUPPORT_DCLOCK)
				else {
					POINT p;
					if ((GetCursorPos(&p)) &&
						(scrnmng_isdispclockclick(&p))) {
						np2oscfg.clk_x++;
						sysmng_update(SYS_UPDATEOSCFG);
						dclock_reset();
					}
				}
#endif
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_LBUTTONUP:
			if (!mousemng_buttonevent(MOUSEMNG_LEFTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_MBUTTONDOWN:
			mousemng_toggle(MOUSEPROC_SYSTEM);
			xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
			sysmng_update(SYS_UPDATECFG);
			break;

		case WM_RBUTTONDOWN:
			if (!mousemng_buttonevent(MOUSEMNG_RIGHTDOWN)) {
				if (!scrnmng_isfullscreen()) {
					np2popup(hWnd, lParam);
				}
#if defined(SUPPORT_DCLOCK)
				else {
					POINT p;
					if ((GetCursorPos(&p)) &&
						(scrnmng_isdispclockclick(&p)) &&
						(np2oscfg.clk_x)) {
						np2oscfg.clk_fnt++;
						sysmng_update(SYS_UPDATEOSCFG);
						dclock_reset();
					}
				}
#endif
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_RBUTTONUP:
			if (!mousemng_buttonevent(MOUSEMNG_RIGHTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_LBUTTONDBLCLK:
			if (!scrnmng_isfullscreen()) {
				np2oscfg.wintype++;
				if (np2oscfg.wintype >= 3) {
					np2oscfg.wintype = 0;
				}
				wlex = np2_winlocexallwin(hWnd);
				winlocex_setholdwnd(wlex, hWnd);
				np2class_windowtype(hWnd, np2oscfg.wintype);
				winlocex_move(wlex);
				winlocex_destroy(wlex);
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			b = FALSE;
			if (!np2oscfg.comfirm) {
				b = TRUE;
			}
			else if (sstpconfirm_exit()) {
				winuienter();
				if (messagebox(hWnd, MAKEINTRESOURCE(IDS_CONFIRM_EXIT),
									MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					b = TRUE;
				}
				winuileave();
			}
			if (b) {
				viewer_allclose();
				DestroyWindow(hWnd);
			}
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
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
					juliet_YMF288Reset();
					pccore_cfgupdate();
					pccore_reset();
					break;
			}
			break;

		case WM_SSTP:
			switch(LOWORD(lParam)) {
				case FD_CONNECT:
					if (!HIWORD(lParam)) {
						sstp_connect();
					}
					break;
				case FD_READ:
					if (!HIWORD(lParam)) {
						sstp_readSocket();
					}
					break;
				case FD_WRITE:
					if (!HIWORD(lParam)) {
//						sstp_writeSokect();
					}
					break;
				case FD_CLOSE:
					if (!HIWORD(lParam)) {
						sstp_disconnect();
					}
					break;
			}
			break;

		case MM_MIM_DATA:
			cmmidi_recvdata((HMIDIIN)wParam, (DWORD)lParam);
			break;

		case MM_MIM_LONGDATA:
			cmmidi_recvexcv((HMIDIIN)wParam, (MIDIHDR *)lParam);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


static void framereset(UINT cnt) {

	framecnt = 0;
#if defined(SUPPORT_DCLOCK)
	scrnmng_dispclock();
#endif
	kdispwin_draw((UINT8)cnt);
	skbdwin_process();
	mdbgwin_process();
	toolwin_draw((UINT8)cnt);
	viewer_allreload(FALSE);
	if (np2oscfg.DISPCLK & 3) {
		if (sysmng_workclockrenewal()) {
			sysmng_updatecaption(3);
		}
	}
}

static void processwait(UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset(cnt);
	}
	else {
		Sleep(1);
	}
	soundmng_sync();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst,
										LPSTR lpszCmdLine, int nCmdShow) {
	WNDCLASS	wc;
	MSG			msg;
	HWND		hWnd;
	UINT		i;
	DWORD		style;
#ifdef OPENING_WAIT
	UINT32		tick;
#endif
	BOOL		xrollkey;

	_MEM_INIT();

	GetModuleFileName(NULL, modulefile, NELEMENTS(modulefile));
	dosio_init();
	file_setcd(modulefile);
	np2arg_analize();
	initload();
	toolwin_readini();
	kdispwin_readini();
	skbdwin_readini();
	mdbgwin_readini();

	rand_setseed((unsigned)time(NULL));

	szClassName[0] = (TCHAR)np2oscfg.winid[0];
	szClassName[1] = (TCHAR)np2oscfg.winid[1];
	szClassName[2] = (TCHAR)np2oscfg.winid[2];

	if ((hWnd = FindWindow(szClassName, NULL)) != NULL) {
		sstpmsg_running();
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		dosio_term();
		return(FALSE);
	}

	g_hInstance = loadextinst(hInstance);
	g_hPrevInst = hPrevInst;
#if !defined(_WIN64)
	mmxflag = (havemmx())?0:MMXFLAG_NOTSUPPORT;
	mmxflag += (np2oscfg.disablemmx)?MMXFLAG_DISABLE:0;
#endif
	TRACEINIT();

	xrollkey = (np2oscfg.xrollkey == 0);
	if (np2oscfg.KEYBOARD >= KEY_TYPEMAX) {
		int keytype = GetKeyboardType(1);
		if ((keytype & 0xff00) == 0x0d00) {
			np2oscfg.KEYBOARD = KEY_PC98;
			xrollkey = !xrollkey;
		}
		else if (!keytype) {
			np2oscfg.KEYBOARD = KEY_KEY101;
		}
		else {
			np2oscfg.KEYBOARD = KEY_KEY106;
		}
	}
	winkbd_roll(xrollkey);
	winkbd_setf12(np2oscfg.F12COPY);
	keystat_initialize();

	np2class_initialize(g_hInstance);
	if (!hPrevInst) {
		wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = NP2GWLP_SIZE;
		wc.hInstance = g_hInstance;
		wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
		wc.lpszClassName = szClassName;
		if (!RegisterClass(&wc)) {
			unloadextinst();
			TRACETERM();
			dosio_term();
			return(FALSE);
		}

		toolwin_initapp(g_hInstance);
		kdispwin_initialize(g_hInstance);
		skbdwin_initialize(g_hInstance);
		mdbgwin_initialize(g_hInstance);
		viewer_init(g_hInstance);
	}

	mousemng_initialize();

	style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	if (np2oscfg.thickframe) {
		style |= WS_THICKFRAME;
	}
	hWnd = CreateWindowEx(0, szClassName, np2oscfg.titles, style,
						np2oscfg.winx, np2oscfg.winy, 640, 400,
						NULL, NULL, g_hInstance, NULL);
	g_hWndMain = hWnd;
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

	xmenu_setshortcut(np2oscfg.shortcut);
	xmenu_setdispclk(np2oscfg.DISPCLK);
	xmenu_setbtnmode(np2cfg.BTN_MODE);
	xmenu_setbtnrapid(np2cfg.BTN_RAPID);
	xmenu_setmsrapid(np2cfg.MOUSERAPID);
	xmenu_setsstp(np2oscfg.sstp);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifdef OPENING_WAIT
	tick = GetTickCount();
#endif

	sysmenu_initialize();
	xmenu_initialize();
	DrawMenuBar(hWnd);

	if (file_attr_c(np2help) == (short)-1) {						// ver0.30
		EnableMenuItem(GetMenu(hWnd), IDM_HELP, MF_GRAYED);
	}

	sysmenu_settoolwin(np2oscfg.toolwin);
	sysmenu_setkeydisp(np2oscfg.keydisp);
	sysmenu_setwinsnap(np2oscfg.WINSNAP);
	sysmenu_setbackground(np2oscfg.background);
	sysmenu_setbgsound(np2oscfg.background);
	sysmenu_setscrnmul(8);

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
			if (sstpmsg_dxerror())
			{
				messagebox(hWnd, MAKEINTRESOURCE(IDS_ERROR_DIRECTDRAW),
														MB_OK | MB_ICONSTOP);
			}
			unloadextinst();
			TRACETERM();
			dosio_term();
			return(FALSE);
		}
	}

	if (soundmng_initialize() == SUCCESS) {
		soundmng_pcmload(SOUND_PCMSEEK, OEMTEXT("SEEKWAV"), EXTROMIO_RES);
		soundmng_pcmload(SOUND_PCMSEEK1, OEMTEXT("SEEK1WAV"), EXTROMIO_RES);
		soundmng_pcmvolume(SOUND_PCMSEEK, np2cfg.MOTORVOL);
		soundmng_pcmvolume(SOUND_PCMSEEK1, np2cfg.MOTORVOL);
	}

	if (np2oscfg.useromeo) {
		juliet_initialize();
	}

	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mousemng_enable(MOUSEPROC_SYSTEM);
	}

	commng_initialize();
	sysmng_initialize();

	joymng_initialize();
	pccore_init();
	S98_init();

	sstpmsg_welcome();

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

		id = flagload(hWnd, str_sav, _T("Resume"), FALSE);
		if (id == IDYES) {
			for (i=0; i<4; i++) {
				np2arg.disk[i] = NULL;
			}
		}
		else if (id == IDCANCEL) {
			DestroyWindow(hWnd);
			mousemng_disable(MOUSEPROC_WINUI);
			S98_trash();
			pccore_term();
			sstp_destruct();
			soundmng_deinitialize();
			scrnmng_destroy();
			unloadextinst();
			TRACETERM();
			dosio_term();
			viewer_term();
			return(FALSE);
		}
	}
#endif

//	リセットしてから… コマンドラインのディスク挿入。
	for (i=0; i<4; i++) {
		if (np2arg.disk[i]) {
			diskdrv_readyfdd((REG8)i, np2arg.disk[i], 0);
		}
	}

	if (!(scrnmode & SCRNMODE_FULLSCREEN)) {
		if (np2oscfg.toolwin) {
			toolwin_create(g_hInstance);
		}
		if (np2oscfg.keydisp) {
			kdispwin_create(g_hInstance);
		}
	}

	while(1) {
		if (!np2stopemulate) {
			if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
				if (!GetMessage(&msg, NULL, 0, 0)) {
					break;
				}
				if ((msg.hwnd != hWnd) ||
					((msg.message != WM_SYSKEYDOWN) &&
					(msg.message != WM_SYSKEYUP))) {
					TranslateMessage(&msg);
				}
				DispatchMessage(&msg);
			}
			else {
				if (np2oscfg.NOWAIT) {
					joymng_sync();
					mousemng_sync();
					pccore_exec(framecnt == 0);
#if defined(SUPPORT_DCLOCK)
					dclock_callback();
#endif
					if (np2oscfg.DRAW_SKIP) {		// nowait frame skip
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
						joymng_sync();
						mousemng_sync();
						pccore_exec(framecnt == 0);
#if defined(SUPPORT_DCLOCK)
						dclock_callback();
#endif
						framecnt++;
					}
					else {
						processwait(np2oscfg.DRAW_SKIP);
					}
				}
				else {								// auto skip
					if (!waitcnt) {
						UINT cnt;
						joymng_sync();
						mousemng_sync();
						pccore_exec(framecnt == 0);
#if defined(SUPPORT_DCLOCK)
						dclock_callback();
#endif
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
		}
		else if ((np2stopemulate == 1) ||				// background sleep
				(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	toolwin_destroy();
	kdispwin_destroy();
	skbdwin_destroy();
	mdbgwin_destroy();

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

	juliet_YMF288Reset();
	pccore_term();

	sstp_destruct();

	juliet_deinitialize();

	soundmng_deinitialize();
	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
		toolwin_writeini();
		kdispwin_writeini();
		skbdwin_writeini();
		mdbgwin_writeini();
	}
	skbdwin_deinitialize();

	unloadextinst();

	TRACETERM();
	_MEM_USED("report.txt");
	dosio_term();

	viewer_term();													// ver0.30

	return((int)msg.wParam);
}

