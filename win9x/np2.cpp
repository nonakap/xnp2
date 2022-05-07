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
#include "misc\WndProc.h"
#include "debuguty\viewer.h"
#include "np2arg.h"
#include "dosio.h"
#include "misc\tstring.h"
#include "commng.h"
#include "commng\cmmidiin32.h"
#if defined(SUPPORT_VSTi)
#include "commng\vsthost\vsteditwnd.h"
#endif	// defined(SUPPORT_VSTi)
#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "winkbd.h"
#include "ini.h"
#include "menu.h"
#include "winloc.h"
#include "dialog\np2class.h"
#include "dialog\dialog.h"
#include "cpucore.h"
#include "pccore.h"
#include "statsave.h"
#include "iocore.h"
#include "pc9861k.h"
#include "mpu98ii.h"
#include "scrndraw.h"
#include "sound.h"
#include "beep.h"
#include "s98.h"
#include "fdd/diskdrv.h"
#include "fdd/fddfile.h"
#if defined(SUPPORT_IDEIO)
#include "fdd/sxsi.h"
#endif	// defined(SUPPORT_IDEIO)
#include "timing.h"
#include "keystat.h"
#include "debugsub.h"
#include "subwnd/kdispwnd.h"
#include "subwnd/mdbgwnd.h"
#include "subwnd/skbdwnd.h"
#include "subwnd/subwnd.h"
#include "subwnd/toolwnd.h"
#if !defined(_WIN64)
#include "cputype.h"
#endif
#if defined(SUPPORT_DCLOCK)
#include "subwnd\dclock.h"
#endif
#include "recvideo.h"

#ifdef BETA_RELEASE
#define		OPENING_WAIT		1500
#endif

static	TCHAR		szClassName[] = _T("NP2-MainWindow");
		HWND		g_hWndMain;
#if !defined(_WIN64)
		int			mmxflag;
#endif
		UINT8		np2break = 0;									// ver0.30
		BOOL		winui_en;
		UINT8		g_scrnmode;

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
						0, 1,
						0, 0,
#if !defined(_WIN64)
						0,
#endif
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						FSCRNMOD_SAMEBPP | FSCRNMOD_SAMERES | FSCRNMOD_ASPECTFIX8,

						CSoundMng::kDSound3, TEXT(""),

#if defined(SUPPORT_VSTi)
						TEXT("%ProgramFiles%\\Roland\\Sound Canvas VA\\SOUND Canvas VA.dll")
#endif	// defined(SUPPORT_VSTi)
					};

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
	LPCTSTR szCaption = np2oscfg.titles;

	std::tstring rText(LoadTString(lpcszText));
	return MessageBox(hWnd, rText.c_str(), szCaption, uType);
}

// ----

/**
 * リソース DLL をロード
 * @param[in] hInstance 元のインスタンス
 * @return インスタンス
 */
static HINSTANCE LoadExternalResource(HINSTANCE hInstance)
{
	OEMCHAR szDllName[32];
	OEMSPRINTF(szDllName, szNp2ResDll, GetOEMCP());

	TCHAR szPath[MAX_PATH];
	file_cpyname(szPath, modulefile, _countof(szPath));
	file_cutname(szPath);
	file_catname(szPath, szDllName, _countof(szPath));

	HMODULE hModule = LoadLibrary(szPath);
	s_hModResource = hModule;
	if (hModule != NULL)
	{
		hInstance = static_cast<HINSTANCE>(hModule);
	}
	return hInstance;
}

/**
 * リソースのアンロード
 */
static void UnloadExternalResource()
{
	HMODULE hModule = s_hModResource;
	s_hModResource = NULL;
	if (hModule)
	{
		FreeLibrary(hModule);
	}
}


// ----

static void winuienter(void) {

	winui_en = TRUE;
	CSoundMng::GetInstance()->Disable(SNDPROC_MAIN);
	scrnmng_topwinui();
}

static void winuileave(void) {

	scrnmng_clearwinui();
	CSoundMng::GetInstance()->Enable(SNDPROC_MAIN);
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

	change = g_scrnmode ^ newmode;
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
			g_scrnmode = newmode;
		}
		else {
			if (scrnmng_create(g_scrnmode) != SUCCESS) {
				PostQuitMessage(0);
				return;
			}
		}
		scrndraw_redraw();
		if (renewal & SCRNMODE_FULLSCREEN) {
			if (!scrnmng_isfullscreen()) {
				if (np2oscfg.toolwin) {
					toolwin_create();
				}
				if (np2oscfg.keydisp) {
					kdispwin_create();
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
		g_scrnmode = newmode;
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
		CSoundMng::GetInstance()->Disable(SNDPROC_MASTER);
	}
	else if (np2break & NP2BREAK_MAIN) {
		if (np2oscfg.background & 1) {
			np2stopemulate = 1;
		}
		else {
			np2stopemulate = 0;
		}
		if (np2oscfg.background) {
			CSoundMng::GetInstance()->Disable(SNDPROC_MASTER);
		}
		else {
			CSoundMng::GetInstance()->Enable(SNDPROC_MASTER);
		}
	}
	else {
		np2stopemulate = 0;
		CSoundMng::GetInstance()->Enable(SNDPROC_MASTER);
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
		std::tstring rFormat(LoadTString(IDS_CONFIRM_RESUME));
		wsprintf(szMessage, rFormat.c_str(), szStat);
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

/**
 * サウンドデバイスの再オープン
 * @param[in] hWnd ウィンドウ ハンドル
 */
static void OpenSoundDevice(HWND hWnd)
{
	CSoundMng* pSoundMng = CSoundMng::GetInstance();
	if (pSoundMng->Open(static_cast<CSoundMng::DeviceType>(np2oscfg.cSoundDeviceType), np2oscfg.szSoundDeviceName, hWnd))
	{
		pSoundMng->LoadPCM(SOUND_PCMSEEK, TEXT("SEEKWAV"));
		pSoundMng->LoadPCM(SOUND_PCMSEEK1, TEXT("SEEK1WAV"));
		pSoundMng->SetPCMVolume(SOUND_PCMSEEK, np2cfg.MOTORVOL);
		pSoundMng->SetPCMVolume(SOUND_PCMSEEK1, np2cfg.MOTORVOL);
	}
}

// ---- proc

static void np2popup(HWND hWnd, LPARAM lp)
{
	HMENU hMainMenu = reinterpret_cast<HMENU>(GetWindowLongPtr(hWnd, NP2GWLP_HMENU));
	if (hMainMenu == NULL)
	{
		return;
	}
	HMENU hMenu = CreatePopupMenu();
	InsertMenuPopup(hMenu, 0, TRUE, hMainMenu);
	xmenu_update(hMenu);

	POINT pt;
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenu);
}

static void OnCommand(HWND hWnd, WPARAM wParam)
{
	UINT		update;
	UINT		uID;
	BOOL		b;

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
			else
			{
				winuienter();
				if (messagebox(hWnd, MAKEINTRESOURCE(IDS_CONFIRM_RESET), MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					b = TRUE;
				}
				winuileave();
			}
			if (b)
			{
				if (sys_updates & SYS_UPDATESNDDEV)
				{
					sys_updates &= ~SYS_UPDATESNDDEV;
					OpenSoundDevice(hWnd);
				}
				pccore_cfgupdate();
				pccore_reset();
			}
			break;

		case IDM_CONFIG:
			winuienter();
			dialog_configure(hWnd);
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
			diskdrv_setsxsi(0x00, NULL);
			break;

		case IDM_IDE1OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x01);
			winuileave();
			break;

		case IDM_IDE1EJECT:
			diskdrv_setsxsi(0x01, NULL);
			break;

#if defined(SUPPORT_IDEIO)
		case IDM_IDE2OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x02);
			winuileave();
			break;

		case IDM_IDE2EJECT:
			sxsi_devclose(0x02);
			break;
#endif

#if defined(SUPPORT_SCSI)
		case IDM_SCSI0OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x20);
			winuileave();
			break;

		case IDM_SCSI0EJECT:
			diskdrv_setsxsi(0x20, NULL);
			break;

		case IDM_SCSI1OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x21);
			winuileave();
			break;

		case IDM_SCSI1EJECT:
			diskdrv_setsxsi(0x21, NULL);
			break;

		case IDM_SCSI2OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x22);
			winuileave();
			break;

		case IDM_SCSI2EJECT:
			diskdrv_setsxsi(0x22, NULL);
			break;

		case IDM_SCSI3OPEN:
			winuienter();
			dialog_changehdd(hWnd, 0x23);
			winuileave();
			break;

		case IDM_SCSI3EJECT:
			diskdrv_setsxsi(0x23, NULL);
			break;
#endif

		case IDM_WINDOW:
			changescreen(g_scrnmode & (~SCRNMODE_FULLSCREEN));
			break;

		case IDM_FULLSCREEN:
			changescreen(g_scrnmode | SCRNMODE_FULLSCREEN);
			break;

		case IDM_ROLNORMAL:
			changescreen(g_scrnmode & (~SCRNMODE_ROTATEMASK));
			break;

		case IDM_ROLLEFT:
			changescreen((g_scrnmode & (~SCRNMODE_ROTATEMASK)) | SCRNMODE_ROTATELEFT);
			break;

		case IDM_ROLRIGHT:
			changescreen((g_scrnmode & (~SCRNMODE_ROTATEMASK)) | SCRNMODE_ROTATERIGHT);
			break;

		case IDM_DISPSYNC:
			np2cfg.DISPSYNC = !np2cfg.DISPSYNC;
			update |= SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			np2cfg.RASTER = !np2cfg.RASTER;
			if (np2cfg.RASTER)
			{
				changescreen(g_scrnmode | SCRNMODE_HIGHCOLOR);
			}
			else
			{
				changescreen(g_scrnmode & (~SCRNMODE_HIGHCOLOR));
			}
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOWAIT:
			np2oscfg.NOWAIT = !np2oscfg.NOWAIT;
			update |= SYS_UPDATECFG;
			break;

		case IDM_AUTOFPS:
			np2oscfg.DRAW_SKIP = 0;
			update |= SYS_UPDATECFG;
			break;

		case IDM_60FPS:
			np2oscfg.DRAW_SKIP = 1;
			update |= SYS_UPDATECFG;
			break;

		case IDM_30FPS:
			np2oscfg.DRAW_SKIP = 2;
			update |= SYS_UPDATECFG;
			break;

		case IDM_20FPS:
			np2oscfg.DRAW_SKIP = 3;
			update |= SYS_UPDATECFG;
			break;

		case IDM_15FPS:
			np2oscfg.DRAW_SKIP = 4;
			update |= SYS_UPDATECFG;
			break;

		case IDM_SCREENOPT:
			winuienter();
			dialog_scropt(hWnd);
			winuileave();
			break;

		case IDM_KEY:
			np2cfg.KEY_MODE = 0;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY1:
			np2cfg.KEY_MODE = 1;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY2:
			np2cfg.KEY_MODE = 2;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_XSHIFT:
			np2cfg.XSHIFT ^= 1;
			keystat_forcerelease(0x70);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XCTRL:
			np2cfg.XSHIFT ^= 2;
			keystat_forcerelease(0x74);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XGRPH:
			np2cfg.XSHIFT ^= 4;
			keystat_forcerelease(0x73);
			update |= SYS_UPDATECFG;
			break;

		case IDM_F12MOUSE:
			np2oscfg.F12COPY = 0;
			winkbd_resetf12();
			winkbd_setf12(0);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COPY:
			np2oscfg.F12COPY = 1;
			winkbd_resetf12();
			winkbd_setf12(1);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12STOP:
			np2oscfg.F12COPY = 2;
			winkbd_resetf12();
			winkbd_setf12(2);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12EQU:
			np2oscfg.F12COPY = 3;
			winkbd_resetf12();
			winkbd_setf12(3);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COMMA:
			np2oscfg.F12COPY = 4;
			winkbd_resetf12();
			winkbd_setf12(4);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_USERKEY1:
			np2oscfg.F12COPY = 5;
			winkbd_resetf12();
			winkbd_setf12(5);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_USERKEY2:
			np2oscfg.F12COPY = 6;
			winkbd_resetf12();
			winkbd_setf12(6);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_BEEPOFF:
			np2cfg.BEEP_VOL = 0;
			beep_setvol(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPLOW:
			np2cfg.BEEP_VOL = 1;
			beep_setvol(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPMID:
			np2cfg.BEEP_VOL = 2;
			beep_setvol(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPHIGH:
			np2cfg.BEEP_VOL = 3;
			beep_setvol(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOSOUND:
			np2cfg.SOUND_SW = 0x00;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PC9801_14:
			np2cfg.SOUND_SW = 0x01;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PC9801_26K:
			np2cfg.SOUND_SW = 0x02;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PC9801_86:
			np2cfg.SOUND_SW = 0x04;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PC9801_26_86:
			np2cfg.SOUND_SW = 0x06;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PC9801_86_CB:
			np2cfg.SOUND_SW = 0x14;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PC9801_118:
			np2cfg.SOUND_SW = 0x08;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_SPEAKBOARD:
			np2cfg.SOUND_SW = 0x20;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_SPARKBOARD:
			np2cfg.SOUND_SW = 0x40;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

#if defined(SUPPORT_PX)
		case IDM_PX1:
			np2cfg.SOUND_SW = 0x30;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_PX2:
			np2cfg.SOUND_SW = 0x50;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;
#endif	// defined(SUPPORT_PX)

		case IDM_SOUNDORCHESTRA:
			np2cfg.SOUND_SW = 0x32;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_SOUNDORCHESTRAV:
			np2cfg.SOUND_SW = 0x82;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_AMD98:
			np2cfg.SOUND_SW = 0x80;
			update |= SYS_UPDATECFG | SYS_UPDATESBOARD;
			break;

		case IDM_JASTSOUND:
			np2oscfg.jastsnd = !np2oscfg.jastsnd;
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_SEEKSND:
			np2cfg.MOTOR = !np2cfg.MOTOR;
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM640:
			np2cfg.EXTMEM = 0;
			update |= SYS_UPDATECFG | SYS_UPDATEMEMORY;
			break;

		case IDM_MEM16:
			np2cfg.EXTMEM = 1;
			update |= SYS_UPDATECFG | SYS_UPDATEMEMORY;
			break;

		case IDM_MEM36:
			np2cfg.EXTMEM = 3;
			update |= SYS_UPDATECFG | SYS_UPDATEMEMORY;
			break;

		case IDM_MEM76:
			np2cfg.EXTMEM = 7;
			update |= SYS_UPDATECFG | SYS_UPDATEMEMORY;
			break;

		case IDM_MEM116:
			np2cfg.EXTMEM = 11;
			update |= SYS_UPDATECFG | SYS_UPDATEMEMORY;
			break;

		case IDM_MEM136:
			np2cfg.EXTMEM = 13;
			update |= SYS_UPDATECFG | SYS_UPDATEMEMORY;
			break;

		case IDM_MOUSE:
			mousemng_toggle(MOUSEPROC_SYSTEM);
			np2oscfg.MOUSE_SW = !np2oscfg.MOUSE_SW;
			update |= SYS_UPDATECFG;
			break;

		case IDM_SERIAL1:
			winuienter();
			dialog_serial(hWnd);
			winuileave();
			break;

		case IDM_MPUPC98:
			winuienter();
			dialog_mpu98(hWnd);
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

		case IDM_S98LOGGING:
			winuienter();
			dialog_soundlog(hWnd);
			winuileave();
			break;

		case IDM_CALENDAR:
			winuienter();
			dialog_calendar(hWnd);
			winuileave();
			break;

		case IDM_ALTENTER:
			np2oscfg.shortcut ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case IDM_ALTF4:
			np2oscfg.shortcut ^= 2;
			update |= SYS_UPDATECFG;
			break;

		case IDM_DISPCLOCK:
			np2oscfg.DISPCLK ^= 1;
			update |= SYS_UPDATECFG;
			sysmng_workclockrenewal();
			sysmng_updatecaption(3);
			break;

		case IDM_DISPFRAME:
			np2oscfg.DISPCLK ^= 2;
			update |= SYS_UPDATECFG;
			sysmng_workclockrenewal();
			sysmng_updatecaption(3);
			break;

		case IDM_JOYX:
			np2cfg.BTN_MODE = !np2cfg.BTN_MODE;
			update |= SYS_UPDATECFG;
			break;

		case IDM_RAPID:
			np2cfg.BTN_RAPID = !np2cfg.BTN_RAPID;
			update |= SYS_UPDATECFG;
			break;

		case IDM_MSRAPID:
			np2cfg.MOUSERAPID = !np2cfg.MOUSERAPID;
			update |= SYS_UPDATECFG;
			break;

		case IDM_CPUSAVE:
			debugsub_status();
			break;

		case IDM_HELP:
			ShellExecute(hWnd, NULL, file_getcd(np2help), NULL, NULL, SW_SHOWNORMAL);
			break;

		case IDM_ABOUT:
			winuienter();
			dialog_about(hWnd);
			winuileave();
			break;

		default:
#if defined(SUPPORT_STATSAVE)
			if ((uID >= IDM_FLAGSAVE) && (uID < (IDM_FLAGSAVE + SUPPORT_STATSAVE)))
			{
				OEMCHAR ext[4];
				OEMSPRINTF(ext, np2flagext, uID - IDM_FLAGSAVE);
				flagsave(ext);
			}
			else if ((uID >= IDM_FLAGLOAD) && (uID < (IDM_FLAGLOAD + SUPPORT_STATSAVE)))
			{
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
#ifndef __GNUC__
			WINNLSEnableIME(hWnd, FALSE);
#endif
			break;

		case WM_SYSCOMMAND:
			update = 0;
			switch(wParam) {
				case IDM_TOOLWIN:
					np2oscfg.toolwin = !np2oscfg.toolwin;
					if (np2oscfg.toolwin) {
						toolwin_create();
					}
					else {
						toolwin_destroy();
					}
					update |= SYS_UPDATEOSCFG;
					break;

#if defined(SUPPORT_KEYDISP)
				case IDM_KEYDISP:
					np2oscfg.keydisp = !np2oscfg.keydisp;
					if (np2oscfg.keydisp) {
						kdispwin_create();
					}
					else {
						kdispwin_destroy();
					}
					break;
#endif
#if defined(SUPPORT_SOFTKBD)
				case IDM_SOFTKBD:
					skbdwin_create();
					break;
#endif
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
				case IDM_MEMDBG32:
					mdbgwin_create();
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
					np2oscfg.WINSNAP = !np2oscfg.WINSNAP;
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_BACKGROUND:
					np2oscfg.background ^= 1;
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_BGSOUND:
					np2oscfg.background ^= 2;
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_MEMORYDUMP:
					debugsub_memorydump();
					break;

				case IDM_DEBUGUTY:
					CDebugUtyView::New();
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
				RECT		rect;
				int			width;
				int			height;
				HBITMAP		hbmp;
				BITMAP		bmp;
				HDC			hmdc;
				HBRUSH		hbrush;
				GetClientRect(hWnd, &rect);
				width = rect.right - rect.left;
				height = rect.bottom - rect.top;
				HINSTANCE hInstance = CWndProc::FindResourceHandle(TEXT("NP2BMP"), RT_BITMAP);
				hbmp = LoadBitmap(hInstance, TEXT("NP2BMP"));
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
			sysmenu_update(GetSystemMenu(hWnd, FALSE));
			xmenu_update(GetMenu(hWnd));
			if (scrnmng_isfullscreen()) {
				DrawMenuBar(hWnd);
			}
			break;

		case WM_EXITMENULOOP:
			winuileave();
			break;

		case WM_ENTERSIZEMOVE:
			CSoundMng::GetInstance()->Disable(SNDPROC_MAIN);
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
			CSoundMng::GetInstance()->Enable(SNDPROC_MAIN);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				np2class_enablemenu(g_hWndMain, TRUE);
				return(DefWindowProc(hWnd, WM_SYSKEYDOWN, VK_F10, lParam));
			}
			if ((wParam == VK_F12) && (!np2oscfg.F12COPY)) {
				mousemng_toggle(MOUSEPROC_SYSTEM);
				np2oscfg.MOUSE_SW = !np2oscfg.MOUSE_SW;
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
					changescreen(g_scrnmode ^ SCRNMODE_FULLSCREEN);
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
						DispClock::GetInstance()->Reset();
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
			np2oscfg.MOUSE_SW = !np2oscfg.MOUSE_SW;
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
						DispClock::GetInstance()->Reset();
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
			else
			{
				winuienter();
				if (messagebox(hWnd, MAKEINTRESOURCE(IDS_CONFIRM_EXIT),
									MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					b = TRUE;
				}
				winuileave();
			}
			if (b) {
				CDebugUtyView::AllClose();
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
					pccore_cfgupdate();
					pccore_reset();
					break;
			}
			break;

		case MM_MIM_DATA:
			CComMidiIn32::RecvData(reinterpret_cast<HMIDIIN>(wParam), static_cast<UINT>(lParam));
			break;

		case MM_MIM_LONGDATA:
			CComMidiIn32::RecvExcv(reinterpret_cast<HMIDIIN>(wParam), reinterpret_cast<MIDIHDR*>(lParam));
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}

/**
 * 1フレーム実行
 * @param[in] bDraw 描画フラグ
 */
static void ExecuteOneFrame(BOOL bDraw)
{
	if (recvideo_isEnabled())
	{
		bDraw = TRUE;
	}

	joymng_sync();
	mousemng_sync();
	pccore_exec(bDraw);
	recvideo_write();
#if defined(SUPPORT_DCLOCK)
	DispClock::GetInstance()->Update();
#endif
#if defined(SUPPORT_VSTi)
	CVstEditWnd::OnIdle();
#endif	// defined(SUPPORT_VSTi)
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
	CDebugUtyView::AllUpdate(false);
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
	CWndProc::Initialize(hInstance);
	CSubWndBase::Initialize(hInstance);
#if defined(SUPPORT_VSTi)
	CVstEditWnd::Initialize(hInstance);
#endif	// defined(SUPPORT_VSTi)

	GetModuleFileName(NULL, modulefile, NELEMENTS(modulefile));
	dosio_init();
	file_setcd(modulefile);
	Np2Arg::GetInstance()->Parse();
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
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
		dosio_term();
		return(FALSE);
	}

	hInstance = LoadExternalResource(hInstance);
	CWndProc::SetResourceHandle(hInstance);

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

	np2class_initialize(hInstance);
	if (!hPrevInst) {
		wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = NP2GWLP_SIZE;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
		wc.lpszClassName = szClassName;
		if (!RegisterClass(&wc)) {
			UnloadExternalResource();
			TRACETERM();
			dosio_term();
			return(FALSE);
		}

		kdispwin_initialize();
		skbdwin_initialize();
		mdbgwin_initialize();
		CDebugUtyView::Initialize(hInstance);
	}

	mousemng_initialize();

	style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	if (np2oscfg.thickframe) {
		style |= WS_THICKFRAME;
	}
	hWnd = CreateWindowEx(0, szClassName, np2oscfg.titles, style,
						np2oscfg.winx, np2oscfg.winy, 640, 400,
						NULL, NULL, hInstance, NULL);
	g_hWndMain = hWnd;
	scrnmng_initialize();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#ifdef OPENING_WAIT
	tick = GetTickCount();
#endif

	sysmenu_initialize(GetSystemMenu(hWnd, FALSE));

	HMENU hMenu = np2class_gethmenu(hWnd);
	xmenu_initialize(hMenu);
	xmenu_update(hMenu);
	if (file_attr_c(np2help) == -1)								// ver0.30
	{
		EnableMenuItem(hMenu, IDM_HELP, MF_GRAYED);
	}
	DrawMenuBar(hWnd);

	g_scrnmode = 0;
	if (Np2Arg::GetInstance()->fullscreen())
	{
		g_scrnmode |= SCRNMODE_FULLSCREEN;
	}
	if (np2cfg.RASTER) {
		g_scrnmode |= SCRNMODE_HIGHCOLOR;
	}
	if (scrnmng_create(g_scrnmode) != SUCCESS) {
		g_scrnmode ^= SCRNMODE_FULLSCREEN;
		if (scrnmng_create(g_scrnmode) != SUCCESS) {
			messagebox(hWnd, MAKEINTRESOURCE(IDS_ERROR_DIRECTDRAW), MB_OK | MB_ICONSTOP);
			UnloadExternalResource();
			TRACETERM();
			dosio_term();
			return(FALSE);
		}
	}

	CSoundMng::Initialize();
	OpenSoundDevice(hWnd);

	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mousemng_enable(MOUSEPROC_SYSTEM);
	}

	commng_initialize();
	sysmng_initialize();

	joymng_initialize();
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

		id = flagload(hWnd, str_sav, _T("Resume"), FALSE);
		if (id == IDYES)
		{
			Np2Arg::GetInstance()->ClearDisk();
		}
		else if (id == IDCANCEL) {
			DestroyWindow(hWnd);
			mousemng_disable(MOUSEPROC_WINUI);
			S98_trash();
			pccore_term();
			CSoundMng::GetInstance()->Close();
			CSoundMng::Deinitialize();
			scrnmng_destroy();
			UnloadExternalResource();
			TRACETERM();
			dosio_term();
			return(FALSE);
		}
	}
#endif

//	リセットしてから… コマンドラインのディスク挿入。
	for (i = 0; i < 4; i++)
	{
		LPCTSTR lpDisk = Np2Arg::GetInstance()->disk(i);
		if (lpDisk)
		{
			diskdrv_readyfdd((REG8)i, lpDisk, 0);
		}
	}

	if (!(g_scrnmode & SCRNMODE_FULLSCREEN)) {
		if (np2oscfg.toolwin) {
			toolwin_create();
		}
		if (np2oscfg.keydisp) {
			kdispwin_create();
		}
	}

	sysmng_workclockreset();
	sysmng_updatecaption(3);

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
					ExecuteOneFrame(framecnt == 0);
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
						ExecuteOneFrame(framecnt == 0);
						framecnt++;
					}
					else {
						processwait(np2oscfg.DRAW_SKIP);
					}
				}
				else {								// auto skip
					if (!waitcnt) {
						UINT cnt;
						ExecuteOneFrame(framecnt == 0);
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

	pccore_term();

	CSoundMng::GetInstance()->Close();
	CSoundMng::Deinitialize();
	scrnmng_destroy();
	recvideo_close();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
		toolwin_writeini();
		kdispwin_writeini();
		skbdwin_writeini();
		mdbgwin_writeini();
	}
	skbdwin_deinitialize();

	UnloadExternalResource();

	TRACETERM();
	_MEM_USED(TEXT("report.txt"));
	dosio_term();

	return((int)msg.wParam);
}
