#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"pccore.h"
#include	"soundrecording.h"
#include	"scrnmng.h"

#define	MFCHECK(a)					((a)?1:0)
#ifdef TARGET_API_MAC_CARBON
#define	_GetMenu(a)					GetMenuHandle((a))
#define	_CheckMenuItem(a, b, c)		CheckMenuItem((a), LoWord(b), (c))
#else
#define	_GetMenu(a)					GetMenu((a))
#define	_CheckMenuItem(a, b, c)		CheckItem((a), LoWord(b), (c))
#endif

void menu_setdispmode(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	np2cfg.DISPSYNC = value & 1;
	_CheckMenuItem(hmenu, IDM_DISPSYNC, MFCHECK(np2cfg.DISPSYNC));
}

void menu_setraster(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	np2cfg.RASTER = value & 1;
	_CheckMenuItem(hmenu, IDM_RASTER, MFCHECK(np2cfg.RASTER));
}

void menu_setwaitflg(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	value &= 1;
	np2oscfg.NOWAIT = value;
	_CheckMenuItem(hmenu, IDM_NOWAIT, MFCHECK(value));
}

void menu_setrotate(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	_CheckMenuItem(hmenu, IDM_ROLNORMAL, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_ROLLEFT, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_ROLRIGHT, MFCHECK(value == 2));
}

void menu_setframe(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	np2oscfg.DRAW_SKIP = value;
	_CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void menu_setkey(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_KEYBOARD);
	if (value >= 4) {
		value = 0;
	}
	np2cfg.KEY_MODE = value;
	_CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_MOUSEKEY, MFCHECK(value == 3));
}

void menu_setxshift(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_KEYBOARD);
	np2cfg.XSHIFT = value;
	_CheckMenuItem(hmenu, IDM_XSHIFT, MFCHECK(value & 1));
	_CheckMenuItem(hmenu, IDM_XCTRL, MFCHECK(value & 2));
	_CheckMenuItem(hmenu, IDM_XGRPH, MFCHECK(value & 4));
}

void menu_setf11key(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_KEYBOARD);
	if (value >= 5) {
		value = 0;
	}
	np2oscfg.F11KEY = value;
	_CheckMenuItem(hmenu, IDM_F11KANA, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_F11STOP, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_F11EQU, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_F11NFER, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_F11USER, MFCHECK(value == 4));
}

void menu_setf12key(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_KEYBOARD);
	if (value >= 5) {
		value = 0;
	}
	np2oscfg.F12KEY = value;
	_CheckMenuItem(hmenu, IDM_F12MOUSE, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_F12COPY, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_F12COMMA, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_F12XFER, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_F12USER, MFCHECK(value == 4));
}

void menu_setbeepvol(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SOUND);
	np2cfg.BEEP_VOL = value & 3;
	_CheckMenuItem(hmenu, IDM_BEEPOFF, MFCHECK(np2cfg.BEEP_VOL == 0));
	_CheckMenuItem(hmenu, IDM_BEEPLOW, MFCHECK(np2cfg.BEEP_VOL == 1));
	_CheckMenuItem(hmenu, IDM_BEEPMID, MFCHECK(np2cfg.BEEP_VOL == 2));
	_CheckMenuItem(hmenu, IDM_BEEPHIGH, MFCHECK(np2cfg.BEEP_VOL == 3));
}

void menu_setsound(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SOUND);
	np2cfg.SOUND_SW = value;
	_CheckMenuItem(hmenu, IDM_NOSOUND, MFCHECK(np2cfg.SOUND_SW == 0x00));
	_CheckMenuItem(hmenu, IDM_PC9801_14, MFCHECK(np2cfg.SOUND_SW == 0x01));
	_CheckMenuItem(hmenu, IDM_PC9801_26K, MFCHECK(np2cfg.SOUND_SW == 0x02));
	_CheckMenuItem(hmenu, IDM_PC9801_86, MFCHECK(np2cfg.SOUND_SW == 0x04));
	_CheckMenuItem(hmenu, IDM_PC9801_26_86, MFCHECK(np2cfg.SOUND_SW == 0x06));
	_CheckMenuItem(hmenu, IDM_PC9801_86_CB, MFCHECK(np2cfg.SOUND_SW == 0x14));
	_CheckMenuItem(hmenu, IDM_PC9801_118, MFCHECK(np2cfg.SOUND_SW == 0x08));
	_CheckMenuItem(hmenu, IDM_SPEAKBOARD, MFCHECK(np2cfg.SOUND_SW & 0x20));
	_CheckMenuItem(hmenu, IDM_SPARKBOARD, MFCHECK(np2cfg.SOUND_SW & 0x40));
	_CheckMenuItem(hmenu, IDM_AMD98, MFCHECK(np2cfg.SOUND_SW & 0x80));
}

void menu_setjastsound(BYTE value) {

	value &= 1;
	np2oscfg.jastsnd = value;
	CheckMenuItem(GetMenuRef(IDM_SOUND), IDM_JASTSOUND, MFCHECK(value));
}

void menu_setmotorflg(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SOUND);
	np2cfg.MOTOR = value & 1;
	_CheckMenuItem(hmenu, IDM_SEEKSND, MFCHECK(np2cfg.MOTOR));
}

void menu_setextmem(BYTE value) {								// ver0.28

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_MEMORY);
	np2cfg.EXTMEM = value;
	_CheckMenuItem(hmenu, IDM_MEM640, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_MEM16, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_MEM36, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_MEM76, MFCHECK(value == 7));
	_CheckMenuItem(hmenu, IDM_MEM116, MFCHECK(value == 11));
	_CheckMenuItem(hmenu, IDM_MEM136, MFCHECK(value == 13));
}

void menu_setdispclk(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_OTHER);
	value &= 3;
	np2oscfg.DISPCLK = value;
	_CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	_CheckMenuItem(hmenu, IDM_DISPFLAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void menu_setbtnrapid(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_OTHER);
	np2cfg.BTN_RAPID = value;
	_CheckMenuItem(hmenu, IDM_RAPID, MFCHECK(np2cfg.BTN_RAPID));
}

void menu_setbtnmode(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_OTHER);
	np2cfg.BTN_MODE = value & 1;
	_CheckMenuItem(hmenu, IDM_JOYX, MFCHECK(np2cfg.BTN_MODE));
}

#if defined(NP2GCC)
void menu_setmouse(BYTE value) {

	value &= 1;
	np2oscfg.MOUSE_SW = value;
	_CheckMenuItem(GetMenu(IDM_DEVICE), IDM_MOUSE, MFCHECK(value));
    if (scrnmode & SCRNMODE_FULLSCREEN) {
        if (!value) {
            ShowMenuBar();
        }
        else {
            HideMenuBar();
        }
    }
}
#endif

void menu_sets98logging(BYTE value) {

	CheckMenuItem(GetMenuRef(IDM_OTHER), IDM_S98LOGGING, MFCHECK(value));
}

void menu_setrecording(bool end) {
    int ret;
    MenuRef	hmenu = GetMenuRef(IDM_OTHER);
    
    ret = soundRec(end);
    if (ret == 1) {
        CheckMenuItem(hmenu, IDM_RECORDING, true);
    }
    else {
        CheckMenuItem(hmenu, IDM_RECORDING, false);
    }
}

void menu_setmsrapid(BYTE value) {

	value &= 1;
	np2cfg.MOUSERAPID = value;
	CheckMenuItem(GetMenuRef(IDM_OTHER), IDM_MSRAPID, MFCHECK(value));
}

void menu_setkeydisp(BYTE value) {

#if defined(SUPPORT_KEYDISP)
	value &= 1;
	np2oscfg.keydisp = value;
	CheckMenuItem(GetMenuRef(IDM_OTHER), IDM_KEYDISP, MFCHECK(value));
#endif
}

void menu_setsoftwarekeyboard(BYTE value) {

#if defined(SUPPORT_SOFTKBD)
	value &= 1;
	np2oscfg.softkey = value;
	CheckMenuItem(GetMenuRef(IDM_OTHER), IDM_SOFTKBD, MFCHECK(value));
#endif
}
