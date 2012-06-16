#include	"compiler.h"
#include	"strres.h"
#include	"np2.h"
#include	"fontmng.h"
#include	"scrnmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"winkbd.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"sound.h"
#include	"beep.h"
#include	"diskdrv.h"
#include	"keystat.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"menustr.h"
#include	"sysmenu.h"
#include	"sysmenu.res"
#include	"sysmenu.str"
#include	"filesel.h"
#include	"dlgcfg.h"
#include	"dlgscr.h"
#include	"dlgabout.h"


static void sys_cmd(MENUID id) {

	UINT	update;

	update = 0;
	switch(id) {
		case MID_RESET:
			pccore_cfgupdate();
			pccore_reset();
			break;

		case MID_CONFIG:
			menudlg_create(DLGCFG_WIDTH, DLGCFG_HEIGHT, mstr_cfg, dlgcfg_cmd);
			break;

		case MID_FDD1OPEN:
			filesel_fdd(0);
			break;

		case MID_FDD1EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case MID_FDD2OPEN:
			filesel_fdd(1);
			break;

		case MID_FDD2EJECT:
			diskdrv_setfdd(1, NULL, 0);
			break;

		case MID_SASI1OPEN:
			filesel_hdd(0x00);
			break;

		case MID_SASI1EJECT:
			diskdrv_sethdd(0x00, NULL);
			break;

		case MID_SASI2OPEN:
			filesel_hdd(0x01);
			break;

		case MID_SASI2EJECT:
			diskdrv_sethdd(0x01, NULL);
			break;
#if defined(SUPPORT_SCSI)
		case MID_SCSI0OPEN:
			filesel_hdd(0x20);
			break;

		case MID_SCSI0EJECT:
			diskdrv_sethdd(0x20, NULL);
			break;

		case MID_SCSI1OPEN:
			filesel_hdd(0x21);
			break;

		case MID_SCSI1EJECT:
			diskdrv_sethdd(0x21, NULL);
			break;

		case MID_SCSI2OPEN:
			filesel_hdd(0x22);
			break;

		case MID_SCSI2EJECT:
			diskdrv_sethdd(0x22, NULL);
			break;

		case MID_SCSI3OPEN:
			filesel_hdd(0x23);
			break;

		case MID_SCSI3EJECT:
			diskdrv_sethdd(0x23, NULL);
			break;
#endif
		case MID_DISPSYNC:
			np2cfg.DISPSYNC ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_RASTER:
			np2cfg.RASTER ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_NOWAIT:
			np2oscfg.NOWAIT ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_AUTOFPS:
			np2oscfg.DRAW_SKIP = 0;
			update |= SYS_UPDATECFG;
			break;

		case MID_60FPS:
			np2oscfg.DRAW_SKIP = 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_30FPS:
			np2oscfg.DRAW_SKIP = 2;
			update |= SYS_UPDATECFG;
			break;

		case MID_20FPS:
			np2oscfg.DRAW_SKIP = 3;
			update |= SYS_UPDATECFG;
			break;

		case MID_15FPS:
			np2oscfg.DRAW_SKIP = 4;
			update |= SYS_UPDATECFG;
			break;

		case MID_SCREENOPT:
			menudlg_create(DLGSCR_WIDTH, DLGSCR_HEIGHT,
													mstr_scropt, dlgscr_cmd);
			break;

#if defined(WIN32_PLATFORM_PSPC)
		case MID_CURDEF:
			winkbd_bindcur(0);
			np2oscfg.bindcur = 0;
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_CUR1:
			winkbd_bindcur(1);
			np2oscfg.bindcur = 1;
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_CUR2:
			winkbd_bindcur(2);
			np2oscfg.bindcur = 2;
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_BTNDEF:
			winkbd_bindbtn(0);
			np2oscfg.bindbtn = 0;
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_BTN1:
			winkbd_bindbtn(1);
			np2oscfg.bindbtn = 1;
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_BTN2:
			winkbd_bindbtn(2);
			np2oscfg.bindbtn = 2;
			update |= SYS_UPDATEOSCFG;
			break;
#endif

		case MID_KEY:
			np2cfg.KEY_MODE = 0;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case MID_JOY1:
			np2cfg.KEY_MODE = 1;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case MID_JOY2:
			np2cfg.KEY_MODE = 2;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case MID_MOUSEKEY:
			np2cfg.KEY_MODE = 3;
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case MID_XSHIFT:
			np2cfg.XSHIFT ^= 1;
			keystat_forcerelease(0x70);
			update |= SYS_UPDATECFG;
			break;

		case MID_XCTRL:
			np2cfg.XSHIFT ^= 2;
			keystat_forcerelease(0x74);
			update |= SYS_UPDATECFG;
			break;

		case MID_XGRPH:
			np2cfg.XSHIFT ^= 4;
			keystat_forcerelease(0x73);
			update |= SYS_UPDATECFG;
			break;

		case MID_F12MOUSE:
			np2oscfg.F12KEY = 0;
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_F12COPY:
			np2oscfg.F12KEY = 1;
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_F12STOP:
			np2oscfg.F12KEY = 2;
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_F12EQU:
			np2oscfg.F12KEY = 3;
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_F12COMMA:
			np2oscfg.F12KEY = 4;
			winkbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_BEEPOFF:
			np2cfg.BEEP_VOL = 0;
			beep_setvol(0);
			update |= SYS_UPDATECFG;
			break;

		case MID_BEEPLOW:
			np2cfg.BEEP_VOL = 1;
			beep_setvol(1);
			update |= SYS_UPDATECFG;
			break;

		case MID_BEEPMID:
			np2cfg.BEEP_VOL = 2;
			beep_setvol(2);
			update |= SYS_UPDATECFG;
			break;

		case MID_BEEPHIGH:
			np2cfg.BEEP_VOL = 3;
			beep_setvol(3);
			update |= SYS_UPDATECFG;
			break;

		case MID_NOSOUND:
			np2cfg.SOUND_SW = 0x00;
			update |= SYS_UPDATECFG;
			break;

		case MID_PC9801_14:
			np2cfg.SOUND_SW = 0x01;
			update |= SYS_UPDATECFG;
			break;

		case MID_PC9801_26K:
			np2cfg.SOUND_SW = 0x02;
			update |= SYS_UPDATECFG;
			break;

		case MID_PC9801_86:
			np2cfg.SOUND_SW = 0x04;
			update |= SYS_UPDATECFG;
			break;

		case MID_PC9801_26_86:
			np2cfg.SOUND_SW = 0x06;
			update |= SYS_UPDATECFG;
			break;

		case MID_PC9801_86_CB:
			np2cfg.SOUND_SW = 0x14;
			update |= SYS_UPDATECFG;
			break;

		case MID_PC9801_118:
			np2cfg.SOUND_SW = 0x08;
			update |= SYS_UPDATECFG;
			break;

		case MID_SPEAKBOARD:
			np2cfg.SOUND_SW = 0x20;
			update |= SYS_UPDATECFG;
			break;

		case MID_SPARKBOARD:
			np2cfg.SOUND_SW = 0x40;
			update |= SYS_UPDATECFG;
			break;

		case MID_AMD98:
			np2cfg.SOUND_SW = 0x80;
			update |= SYS_UPDATECFG;
			break;

		case MID_JASTSND:
			np2oscfg.jastsnd ^= 1;
			update |= SYS_UPDATEOSCFG;
			break;

		case MID_SEEKSND:
			np2cfg.MOTOR ^= 1;
			update |= SYS_UPDATECFG;
			break;
#if 0
		case IDM_SNDOPT:
			winuienter();
			dialog_sndopt(hWnd);
			winuileave();
			break;
#endif
		case MID_MEM640:
			np2cfg.EXTMEM = 0;
			update |= SYS_UPDATECFG;
			break;

		case MID_MEM16:
			np2cfg.EXTMEM = 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_MEM36:
			np2cfg.EXTMEM = 3;
			update |= SYS_UPDATECFG;
			break;

		case MID_MEM76:
			np2cfg.EXTMEM = 7;
			update |= SYS_UPDATECFG;
			break;
#if 0
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
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MPUPC98),
									hWnd, (DLGPROC)MidiDialogProc);
			winuileave();
			break;
#endif
		case MID_MIDIPANIC:
			rs232c_midipanic();
			mpu98ii_midipanic();
			pc9861k_midipanic();
			break;
#if 0
		case IDM_BMPSAVE:
			winuienter();
			dialog_writebmp(hWnd);
			winuileave();
			break;

		case IDM_S98LOGGING:
			winuienter();
			dialog_s98(hWnd);
			winuileave();
			break;

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
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CALENDAR),
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
#endif
		case MID_JOYX:
			np2cfg.BTN_MODE ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_RAPID:
			np2cfg.BTN_RAPID ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_MSRAPID:
			np2cfg.MOUSERAPID ^= 1;
			update |= SYS_UPDATECFG;
			break;

		case MID_ABOUT:
			menudlg_create(DLGABOUT_WIDTH, DLGABOUT_HEIGHT,
													mstr_about, dlgabout_cmd);
			break;

#if defined(MENU_TASKMINIMIZE)
		case SID_MINIMIZE:
			taskmng_minimize();
			break;
#endif
		case MID_EXIT:
		case SID_CLOSE:
			taskmng_exit();
			break;
	}
	sysmng_update(update);
}


// ----

BOOL sysmenu_create(void) {

	if (menubase_create() != SUCCESS) {
		goto smcre_err;
	}
	menuicon_regist(MICON_NP2, &np2icon);
	if (menusys_create(s_main, sys_cmd, MICON_NP2, str_np2)) {
		goto smcre_err;
	}
#if defined(SUPPORT_SOFTKBD)
	menusys_setstyle(MENUSTYLE_BOTTOM);
#endif
	return(SUCCESS);

smcre_err:
	return(FAILURE);
}

void sysmenu_destroy(void) {

	menubase_close();
	menubase_destroy();
	menusys_destroy();
}

BOOL sysmenu_menuopen(UINT menutype, int x, int y) {

	BYTE	b;

	menusys_setcheck(MID_DISPSYNC, (np2cfg.DISPSYNC & 1));
	menusys_setcheck(MID_RASTER, (np2cfg.RASTER & 1));
	menusys_setcheck(MID_NOWAIT, (np2oscfg.NOWAIT & 1));
	b = np2oscfg.DRAW_SKIP;
	menusys_setcheck(MID_AUTOFPS, (b == 0));
	menusys_setcheck(MID_60FPS, (b == 1));
	menusys_setcheck(MID_30FPS, (b == 2));
	menusys_setcheck(MID_20FPS, (b == 3));
	menusys_setcheck(MID_15FPS, (b == 4));
#if defined(WIN32_PLATFORM_PSPC)
	b = np2oscfg.bindcur;
	menusys_setcheck(MID_CURDEF, (b == 0));
	menusys_setcheck(MID_CUR1, (b == 1));
	menusys_setcheck(MID_CUR2, (b == 2));
	b = np2oscfg.bindbtn;
	menusys_setcheck(MID_BTNDEF, (b == 0));
	menusys_setcheck(MID_BTN1, (b == 1));
	menusys_setcheck(MID_BTN2, (b == 2));
#endif

	b = np2cfg.KEY_MODE;
	menusys_setcheck(MID_KEY, (b == 0));
	menusys_setcheck(MID_JOY1, (b == 1));
	menusys_setcheck(MID_JOY2, (b == 2));
	menusys_setcheck(MID_MOUSEKEY, (b == 3));
	b = np2cfg.XSHIFT;
	menusys_setcheck(MID_XSHIFT, (b & 1));
	menusys_setcheck(MID_XCTRL, (b & 2));
	menusys_setcheck(MID_XGRPH, (b & 4));
	b = np2oscfg.F12KEY;
	menusys_setcheck(MID_F12MOUSE, (b == 0));
	menusys_setcheck(MID_F12COPY, (b == 1));
	menusys_setcheck(MID_F12STOP, (b == 2));
	menusys_setcheck(MID_F12EQU, (b == 3));
	menusys_setcheck(MID_F12COMMA, (b == 4));
	b = np2cfg.BEEP_VOL & 3;
	menusys_setcheck(MID_BEEPOFF, (b == 0));
	menusys_setcheck(MID_BEEPLOW, (b == 1));
	menusys_setcheck(MID_BEEPMID, (b == 2));
	menusys_setcheck(MID_BEEPHIGH, (b == 3));
	b = np2cfg.SOUND_SW;
	menusys_setcheck(MID_NOSOUND, (b == 0x00));
	menusys_setcheck(MID_PC9801_14, (b == 0x01));
	menusys_setcheck(MID_PC9801_26K, (b == 0x02));
	menusys_setcheck(MID_PC9801_86, (b == 0x04));
	menusys_setcheck(MID_PC9801_26_86, (b == 0x06));
	menusys_setcheck(MID_PC9801_86_CB, (b == 0x14));
	menusys_setcheck(MID_PC9801_118, (b == 0x08));
	menusys_setcheck(MID_SPEAKBOARD, (b == 0x20));
	menusys_setcheck(MID_SPARKBOARD, (b == 0x40));
	menusys_setcheck(MID_AMD98, (b == 0x80));
	menusys_setcheck(MID_JASTSND, (np2oscfg.jastsnd & 1));
	menusys_setcheck(MID_SEEKSND, (np2cfg.MOTOR & 1));
	b = np2cfg.EXTMEM;
	menusys_setcheck(MID_MEM640, (b == 0));
	menusys_setcheck(MID_MEM16, (b == 1));
	menusys_setcheck(MID_MEM36, (b == 3));
	menusys_setcheck(MID_MEM76, (b == 7));
	menusys_setcheck(MID_JOYX, (np2cfg.BTN_MODE & 1));
	menusys_setcheck(MID_RAPID, (np2cfg.BTN_RAPID & 1));
	menusys_setcheck(MID_MSRAPID, (np2cfg.MOUSERAPID & 1));
	return(menusys_open(x, y));
}

