#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"pccore.h"


#define	MFCHECK(a)					((a)?1:0)
#ifdef TARGET_API_MAC_CARBON
#define	_GetMenu(a)					GetMenuHandle((a))
#define	_CheckMenuItem(a, b, c)		CheckMenuItem((a), LoWord(b), (c))
#else
#define	_GetMenu(a)					GetMenu((a))
#define	_CheckMenuItem(a, b, c)		CheckItem((a), LoWord(b), (c))
#endif

void menu_setrotate(BYTE value) {

	MenuHandle	hmenu;

	hmenu = _GetMenu(IDM_SCREEN);
	_CheckMenuItem(hmenu, IDM_ROLNORMAL, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_ROLLEFT, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_ROLRIGHT, MFCHECK(value == 2));
}

void menu_setdispmode(BYTE value) {

	value &= 1;
	np2cfg.DISPSYNC = value;
	_CheckMenuItem(_GetMenu(IDM_SCREEN), IDM_DISPSYNC, MFCHECK(value));
}

void menu_setraster(BYTE value) {

	value &= 1;
	np2cfg.RASTER = value;
	_CheckMenuItem(_GetMenu(IDM_SCREEN), IDM_RASTER, MFCHECK(value));
}

void menu_setwaitflg(BYTE value) {

	value &= 1;
	np2oscfg.NOWAIT = value;
	_CheckMenuItem(_GetMenu(IDM_SCREEN), IDM_NOWAIT, MFCHECK(value));
}

void menu_setframe(BYTE value) {

	MenuHandle	hmenu;

	np2oscfg.DRAW_SKIP = value;
	hmenu = _GetMenu(IDM_SCREEN);
	_CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void menu_setkey(BYTE value) {

	MenuHandle	hmenu;

	if (value > 3) {
		value = 0;
	}
	np2cfg.KEY_MODE = value;
	hmenu = _GetMenu(IDM_KEYBOARD);
	_CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_MOUSEKEY, MFCHECK(value == 3));
}

void menu_setxshift(BYTE value) {

	MenuHandle	hmenu;

	np2cfg.XSHIFT = value;
	hmenu = _GetMenu(IDM_KEYBOARD);
	_CheckMenuItem(hmenu, IDM_XSHIFT, MFCHECK(value & 1));
	_CheckMenuItem(hmenu, IDM_XCTRL, MFCHECK(value & 2));
	_CheckMenuItem(hmenu, IDM_XGRPH, MFCHECK(value & 4));
}

void menu_setf11key(BYTE value) {

	MenuHandle	hmenu;

	if (value >= 5) {
		value = 0;
	}
	np2oscfg.F11KEY = value;
	hmenu = _GetMenu(IDM_KEYBOARD);
	_CheckMenuItem(hmenu, IDM_F11KANA, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_F11STOP, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_F11NFER, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_F11USER, MFCHECK(value == 4));
}

void menu_setf12key(BYTE value) {

	MenuHandle	hmenu;

	if (value >= 5) {
		value = 0;
	}
	np2oscfg.F12KEY = value;
	hmenu = _GetMenu(IDM_KEYBOARD);
	_CheckMenuItem(hmenu, IDM_F12MOUSE, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_F12COPY, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_F12XFER, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_F12USER, MFCHECK(value == 4));
}

void menu_setbeepvol(BYTE value) {

	MenuHandle	hmenu;

	value &= 3;
	np2cfg.BEEP_VOL = value;
	hmenu = _GetMenu(IDM_SOUND);
	_CheckMenuItem(hmenu, IDM_BEEPOFF, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_BEEPLOW, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_BEEPMID, MFCHECK(value == 2));
	_CheckMenuItem(hmenu, IDM_BEEPHIGH, MFCHECK(value == 3));
}

void menu_setsound(BYTE value) {

	MenuHandle	hmenu;

	np2cfg.SOUND_SW = value;
	hmenu = _GetMenu(IDM_SOUND);
	_CheckMenuItem(hmenu, IDM_NOSOUND, MFCHECK(value == 0x00));
	_CheckMenuItem(hmenu, IDM_PC9801_14, MFCHECK(value == 0x01));
	_CheckMenuItem(hmenu, IDM_PC9801_26K, MFCHECK(value == 0x02));
	_CheckMenuItem(hmenu, IDM_PC9801_86, MFCHECK(value == 0x04));
	_CheckMenuItem(hmenu, IDM_PC9801_26_86, MFCHECK(value == 0x06));
	_CheckMenuItem(hmenu, IDM_PC9801_86_CB, MFCHECK(value == 0x14));
	_CheckMenuItem(hmenu, IDM_PC9801_118, MFCHECK(value == 0x08));
	_CheckMenuItem(hmenu, IDM_SPEAKBOARD, MFCHECK(value == 0x20));
	_CheckMenuItem(hmenu, IDM_SPARKBOARD, MFCHECK(value == 0x40));
	_CheckMenuItem(hmenu, IDM_AMD98, MFCHECK(value == 0x80));
}

void menu_setjastsnd(BYTE value) {

	value &= 1;
	np2oscfg.jastsnd = value;
	_CheckMenuItem(_GetMenu(IDM_SOUND), IDM_JASTSND, MFCHECK(value));
}

void menu_setmotorflg(BYTE value) {

	value &= 1;
	np2cfg.MOTOR = value;
	_CheckMenuItem(_GetMenu(IDM_SOUND), IDM_SEEKSND, MFCHECK(value));
}

void menu_setextmem(BYTE value) {

	MenuHandle	hmenu;

	np2cfg.EXTMEM = value;
	hmenu = _GetMenu(IDM_MEMORY);
	_CheckMenuItem(hmenu, IDM_MEM640, MFCHECK(value == 0));
	_CheckMenuItem(hmenu, IDM_MEM16, MFCHECK(value == 1));
	_CheckMenuItem(hmenu, IDM_MEM36, MFCHECK(value == 3));
	_CheckMenuItem(hmenu, IDM_MEM76, MFCHECK(value == 7));
}

void menu_setmouse(BYTE value) {

	value &= 1;
	np2oscfg.MOUSE_SW = value;
	_CheckMenuItem(GetMenu(IDM_DEVICE), IDM_MOUSE, MFCHECK(value));
}

void menu_setdispclk(BYTE value) {

	MenuHandle	hmenu;

	value &= 3;
	np2oscfg.DISPCLK = value;
	hmenu = _GetMenu(IDM_OTHER);
	_CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	_CheckMenuItem(hmenu, IDM_DISPFLAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void menu_setbtnmode(BYTE value) {

	value &= 1;
	np2cfg.BTN_MODE = value;
	_CheckMenuItem(_GetMenu(IDM_OTHER), IDM_JOYX, MFCHECK(value));
}

void menu_setbtnrapid(BYTE value) {

	value &= 1;
	np2cfg.BTN_RAPID = value;
	_CheckMenuItem(_GetMenu(IDM_OTHER), IDM_RAPID, MFCHECK(value));
}

