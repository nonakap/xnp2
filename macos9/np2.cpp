#include	"compiler.h"
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"dosio.h"
#include	"commng.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"mackbd.h"
#include	"ini.h"
#include	"menu.h"
#include	"np2open.h"
#include	"dialog.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"timing.h"
#include	"keystat.h"
#include	"debugsub.h"
#include	"bios.h"
#include	"scrndraw.h"
#include	"sound.h"
#include	"beep.h"
#include	"s98.h"
#include	"diskdrv.h"
#include	"fddfile.h"


		NP2OSCFG	np2oscfg = {100, 100,
								0, 0, 0, 0,
								0, 0,
#if defined(SUPPORT_RESUME)
								0,
#endif
#if defined(SUPPORT_STATSAVE)
								0,
#endif
								0, 0};

		WindowPtr	hWndMain;
		BOOL		np2running;
static	UINT		framecnt = 0;
static	UINT		waitcnt = 0;
static	UINT		framemax = 1;
static	BYTE		scrnmode;


#define DRAG_THRESHOLD		5

#ifndef NP2GCC
#define	DEFAULTPATH		":"
#else
#define	DEFAULTPATH		"::::"
#endif
static	char	target[MAX_PATH] = DEFAULTPATH;

#if !defined(SUPPORT_PC9821)
static const char np2app[] = "np2";
#else
static const char np2app[] = "np21";
#endif


// ----

#if defined(SUPPORT_RESUME)
static const char np2resumeext[] = ".sav";
#endif
#if defined(SUPPORT_STATSAVE)
static const char np2flagext[] = ".sv%u";
#endif

#if defined(SUPPORT_RESUME) || defined(SUPPORT_STATSAVE)
static void getstatfilename(char *path, const char *ext, UINT size) {

	file_cpyname(path, file_getcd(np2app), size);
	file_catname(path, ext, size);
}

static void flagsave(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	statsave_save(path);
}

static void flagdelete(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	file_delete(path);
}

static int flagload(const char *ext, BOOL force) {

	char	path[MAX_PATH];
	char	buf[1024];
	int		r;
	int		ret;

	getstatfilename(path, ext, sizeof(path));
	r = statsave_check(path, buf, sizeof(buf));
	if (r & (~STATFLAG_DISKCHG)) {
		ResumeErrorDialogProc();
		ret = IDCANCEL;
	}
	else if ((!force) && (r & STATFLAG_DISKCHG)) {
		ret = ResumeWarningDialogProc(buf);
	}
	else {
		ret = IDOK;
	}
	if (ret == IDOK) {
		statsave_load(path);
	}
	return(ret);
}
#endif


// ---- ‚¨‚Ü‚¶‚È‚¢

#if TARGET_CARBON
#define	AEProc(fn)	NewAEEventHandlerUPP((AEEventHandlerProcPtr)(fn))
#else
#define	AEProc(fn)	NewAEEventHandlerProc(fn)
#endif

#ifdef TARGET_API_MAC_CARBON
static pascal OSErr handleQuitApp(const AppleEvent *event, AppleEvent *reply,
															long refcon) {

	taskmng_exit();

	(void)event, (void)reply, (void)refcon;
	return(noErr);
}
#endif

static void InitToolBox(void) {

#if !TARGET_CARBON
	MaxApplZone();
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0);
#endif
	FlushEvents(everyEvent, 0);
	InitCursor();

#ifdef TARGET_API_MAC_CARBON
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
						AEProc(handleQuitApp), 0L, false);
#endif
}

static void MenuBarInit(void) {

	Handle		hdl;
	MenuHandle	hmenu;

	hdl = GetNewMBar(IDM_MAINMENU);
	if (hdl == NULL) {
		ExitToShell();
	}
	SetMenuBar(hdl);
	hmenu = GetMenuHandle(IDM_APPLE);
	if (hmenu) {
		AppendResMenu(hmenu, 'DRVR');
	}
#if !defined(SUPPORT_SCSI)
	hmenu = GetMenuHandle(IDM_HARDDISK);
	if (hmenu) {
		DeleteMenuItem(hmenu, 7);
		DeleteMenuItem(hmenu, 6);
		DeleteMenuItem(hmenu, 5);
		DeleteMenuItem(hmenu, 4);
		DeleteMenuItem(hmenu, 3);
	}
#endif
	if (!np2oscfg.I286SAVE) {
		hmenu = GetMenuHandle(IDM_OTHER);
		if (hmenu) {
			DeleteMenuItem(hmenu, 9);
		}
	}
	InsertMenu(GetMenu(IDM_SASI1), -1);
	InsertMenu(GetMenu(IDM_SASI2), -1);
#if defined(SUPPORT_SCSI)
	InsertMenu(GetMenu(IDM_SCSI0), -1);
	InsertMenu(GetMenu(IDM_SCSI1), -1);
	InsertMenu(GetMenu(IDM_SCSI2), -1);
	InsertMenu(GetMenu(IDM_SCSI3), -1);
#endif
	InsertMenu(GetMenu(IDM_KEYBOARD), -1);
	InsertMenu(GetMenu(IDM_SOUND), -1);
	InsertMenu(GetMenu(IDM_MEMORY), -1);

#if TARGET_API_MAC_CARBON
	hmenu = GetMenuHandle(IDM_FDD2);
	SetItemCmd(hmenu, LoWord(IDM_FDD2OPEN), 'D');
	SetMenuItemModifiers(hmenu, LoWord(IDM_FDD2OPEN), kMenuShiftModifier);
	SetItemCmd(hmenu, LoWord(IDM_FDD2EJECT), 'E');
	SetMenuItemModifiers(hmenu, LoWord(IDM_FDD2EJECT), kMenuShiftModifier);
	hmenu = GetMenuHandle(IDM_SASI2);
	SetItemCmd(hmenu, LoWord(IDM_SASI2OPEN), 'O');
	SetMenuItemModifiers(hmenu, LoWord(IDM_SASI2OPEN), kMenuShiftModifier);
#else
	EnableItem(GetMenuHandle(IDM_DEVICE), LoWord(IDM_MOUSE));
	EnableItem(GetMenuHandle(IDM_KEYBOARD), LoWord(IDM_F12MOUSE));
#endif

	if (!(np2cfg.fddequip & 1)) {
		DeleteMenu(IDM_FDD1);
	}
	if (!(np2cfg.fddequip & 2)) {
		DeleteMenu(IDM_FDD2);
	}
	if (!(np2cfg.fddequip & 4)) {
		DeleteMenu(IDM_FDD3);
	}
	if (!(np2cfg.fddequip & 8)) {
		DeleteMenu(IDM_FDD4);
	}

#if defined(SUPPORT_STATSAVE)
	if (!np2oscfg.statsave) {
#endif
		DeleteMenu(IDM_STATSAVE);
#if defined(SUPPORT_STATSAVE)
	}
#endif

	DrawMenuBar();
}

static void changescreen(BYTE mode) {

	(void)mode;
}

static void HandleMenuChoice(long wParam) {

	UINT	update;
	Str255	applname;
#if defined(SUPPORT_STATSAVE)
	UINT	num;
	char	ext[16];
#endif

	update = 0;
	switch(wParam) {
		case IDM_ABOUT:
			AboutDialogProc();
			break;

		case IDM_RESET:
			pccore_cfgupdate();
			pccore_reset();
			break;

		case IDM_CONFIGURE:
			ConfigDialogProc();
			break;

		case IDM_NEWDISK:
			dialog_newdisk();
			break;

		case IDM_FONT:
			dialog_font();
			break;

		case IDM_EXIT:
			np2running = FALSE;
			break;

		case IDM_FDD1OPEN:
			dialog_changefdd(0);
			break;

		case IDM_FDD1EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case IDM_FDD2OPEN:
			dialog_changefdd(1);
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(1, NULL, 0);
			break;

		case IDM_FDD3OPEN:
			dialog_changefdd(2);
			break;

		case IDM_FDD3EJECT:
			diskdrv_setfdd(2, NULL, 0);
			break;

		case IDM_FDD4OPEN:
			dialog_changefdd(3);
			break;

		case IDM_FDD4EJECT:
			diskdrv_setfdd(3, NULL, 0);
			break;

		case IDM_SASI1OPEN:
			dialog_changehdd(0x00);
			break;

		case IDM_SASI1REMOVE:
			diskdrv_sethdd(0x00, NULL);
			break;

		case IDM_SASI2OPEN:
			dialog_changehdd(0x01);
			break;

		case IDM_SASI2REMOVE:
			diskdrv_sethdd(0x01, NULL);
			break;

		case IDM_SCSI0OPEN:
			dialog_changehdd(0x20);
			break;

		case IDM_SCSI0REMOVE:
			diskdrv_sethdd(0x20, NULL);
			break;

		case IDM_SCSI1OPEN:
			dialog_changehdd(0x21);
			break;

		case IDM_SCSI1REMOVE:
			diskdrv_sethdd(0x21, NULL);
			break;

		case IDM_SCSI2OPEN:
			dialog_changehdd(0x22);
			break;

		case IDM_SCSI2REMOVE:
			diskdrv_sethdd(0x22, NULL);
			break;

		case IDM_SCSI3OPEN:
			dialog_changehdd(0x23);
			break;

		case IDM_SCSI3REMOVE:
			diskdrv_sethdd(0x23, NULL);
			break;

		case IDM_ROLNORMAL:
			menu_setrotate(0);
			changescreen(scrnmode & (~SCRNMODE_ROTATEMASK));
			break;

		case IDM_ROLLEFT:
			menu_setrotate(1);
			changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATELEFT);
			break;

		case IDM_ROLRIGHT:
			menu_setrotate(2);
			changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATERIGHT);
			break;

		case IDM_DISPSYNC:
			menu_setdispmode(np2cfg.DISPSYNC ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			menu_setraster(np2cfg.RASTER ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOWAIT:
			menu_setwaitflg(np2oscfg.NOWAIT ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_AUTOFPS:
			menu_setframe(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_60FPS:
			menu_setframe(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_30FPS:
			menu_setframe(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_20FPS:
			menu_setframe(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_15FPS:
			menu_setframe(4);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SCREENOPT:
			dialog_scropt();
			break;

		case IDM_MOUSE:
			mousemng_toggle(MOUSEPROC_SYSTEM);
			menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MIDIOPT:
			MPU98DialogProc();
			break;

		case IDM_MIDIPANIC:
			rs232c_midipanic();
			mpu98ii_midipanic();
			pc9861k_midipanic();
			break;

		case IDM_KEY:
			menu_setkey(0);
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY1:
			menu_setkey(1);
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOY2:
			menu_setkey(2);
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_MOUSEKEY:
			menu_setkey(3);
			keystat_resetjoykey();
			update |= SYS_UPDATECFG;
			break;

		case IDM_XSHIFT:
			menu_setxshift(np2cfg.XSHIFT ^ 1);
			keystat_forcerelease(0x70);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XCTRL:
			menu_setxshift(np2cfg.XSHIFT ^ 2);
			keystat_forcerelease(0x74);
			update |= SYS_UPDATECFG;
			break;

		case IDM_XGRPH:
			menu_setxshift(np2cfg.XSHIFT ^ 4);
			keystat_forcerelease(0x73);
			update |= SYS_UPDATECFG;
			break;

		case IDM_F11KANA:
			menu_setf11key(0);
			mackbd_resetf11();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F11STOP:
			menu_setf11key(1);
			mackbd_resetf11();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F11NFER:
			menu_setf11key(3);
			mackbd_resetf11();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F11USER:
			menu_setf11key(4);
			mackbd_resetf11();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12MOUSE:
			menu_setf12key(0);
			mackbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12COPY:
			menu_setf12key(1);
			mackbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12XFER:
			menu_setf12key(3);
			mackbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_F12USER:
			menu_setf12key(4);
			mackbd_resetf12();
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_BEEPOFF:
			menu_setbeepvol(0);
			beep_setvol(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPLOW:
			menu_setbeepvol(1);
			beep_setvol(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPMID:
			menu_setbeepvol(2);
			beep_setvol(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPHIGH:
			menu_setbeepvol(3);
			beep_setvol(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOSOUND:
			menu_setsound(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_14:
			menu_setsound(0x01);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_26K:
			menu_setsound(0x02);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_86:
			menu_setsound(0x04);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_26_86:
			menu_setsound(0x06);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_86_CB:
			menu_setsound(0x14);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_118:
			menu_setsound(0x08);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SPEAKBOARD:
			menu_setsound(0x20);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SPARKBOARD:
			menu_setsound(0x40);
			update |= SYS_UPDATECFG;
			break;

		case IDM_AMD98:
			menu_setsound(0x80);
			update |= SYS_UPDATECFG;
			break;

		case IDM_JASTSND:
			menu_setjastsnd(np2oscfg.jastsnd ^ 1);
			update |= SYS_UPDATEOSCFG;
			break;

		case IDM_SEEKSND:
			menu_setmotorflg(np2cfg.MOTOR ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM640:
			menu_setextmem(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM16:
			menu_setextmem(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM36:
			menu_setextmem(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM76:
			menu_setextmem(7);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM116:
			menu_setextmem(11);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM136:
			menu_setextmem(13);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BMPSAVE:
			dialog_writebmp();
			break;

		case IDM_CALENDAR:
			CalendarDialogProc();
			break;

		case IDM_DISPCLOCK:
			menu_setdispclk(np2oscfg.DISPCLK ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_DISPFLAME:
			menu_setdispclk(np2oscfg.DISPCLK ^ 2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOYX:
			menu_setbtnmode(np2cfg.BTN_MODE ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_RAPID:
			menu_setbtnrapid(np2cfg.BTN_RAPID ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_I286SAVE:
			debugsub_status();
			break;

		default:
			if (HiWord(wParam) == IDM_APPLE) {
				GetMenuItemText(GetMenuHandle(IDM_APPLE), 
											LoWord(wParam), applname);
#if !TARGET_API_MAC_CARBON
				(void)OpenDeskAcc(applname);
#endif
			}
#if defined(SUPPORT_STATSAVE)
			else if (HiWord(wParam) == IDM_STATSAVE) {
				num = LoWord(wParam);
				if ((num >= 1) && (num < (1 + 10))) {
					SPRINTF(ext, np2flagext, num - 1);
					flagsave(ext);
				}
				else if ((num >= 12) && (num < (12 + 10))) {
					SPRINTF(ext, np2flagext, num - 12);
					flagload(ext, TRUE);
				}
			}
#endif
			break;
	}
	sysmng_update(update);
	HiliteMenu(0);
}

static void HandleUpdateEvent(EventRecord *pevent) {

	WindowPtr	hWnd;

	hWnd = (WindowPtr)pevent->message;
	BeginUpdate(hWnd);
	if (np2running) {
		scrndraw_redraw();
	}
	else {
		np2open();
	}
	EndUpdate(hWnd);
}

static void HandleMouseDown(EventRecord *pevent) {

	WindowPtr	hWnd;
	Rect		rDrag;

	switch(FindWindow(pevent->where, &hWnd)) {
		case inMenuBar:
			if (np2running) {
				soundmng_stop();
				mousemng_disable(MOUSEPROC_MACUI);
				HandleMenuChoice(MenuSelect(pevent->where));
				mousemng_enable(MOUSEPROC_MACUI);
				soundmng_play();
			}
			break;

		case inDrag:
#if TARGET_API_MAC_CARBON
		{
			BitMap	gscreenBits;
			GetQDGlobalsScreenBits(&gscreenBits);
			rDrag = gscreenBits.bounds;
			InsetRect(&rDrag, DRAG_THRESHOLD, DRAG_THRESHOLD);
			DragWindow(hWnd, pevent->where, &rDrag);
		}
#else
			rDrag = qd.screenBits.bounds;
			InsetRect(&rDrag, DRAG_THRESHOLD, DRAG_THRESHOLD);
			DragWindow(hWnd, pevent->where, &rDrag);
#endif
			break;

		case inContent:
			mousemng_buttonevent((pevent->modifiers & (1 << 12))
									?MOUSEMNG_RIGHTDOWN:MOUSEMNG_LEFTDOWN);
			break;

		case inGoAway:
			if (TrackGoAway(hWnd, pevent->where)) { }
			np2running = FALSE;
			break;
	}
}

static void eventproc(EventRecord *event) {

	int		keycode;

	keycode = (event->message & keyCodeMask) >> 8;
	switch(event->what) {
		case mouseDown:
			HandleMouseDown(event);
			break;

		case updateEvt:
			HandleUpdateEvent(event);
			break;

		case keyDown:
		case autoKey:
			if (!np2running) {
				break;
			}
#if !TARGET_API_MAC_CARBON
			if ((keycode == 0x6f) && (np2oscfg.F12KEY == 0)) {
				HandleMenuChoice(IDM_MOUSE);
				break;
			}
#endif
			if (event->modifiers & cmdKey) {
#if !TARGET_API_MAC_CARBON
				if (mackbd_keydown(keycode, TRUE)) {
					break;
				}
#endif
				soundmng_stop();
				mousemng_disable(MOUSEPROC_MACUI);
#if TARGET_API_MAC_CARBON
				HandleMenuChoice(MenuEvent(event));
#else
				HandleMenuChoice(MenuKey(event->message & charCodeMask));
#endif
				mousemng_enable(MOUSEPROC_MACUI);
				soundmng_play();
			}
			else {
				mackbd_keydown(keycode, FALSE);
			}
			break;

		case keyUp:
			mackbd_keyup(keycode);
			break;

		case mouseUp:
			mousemng_buttonevent(MOUSEMNG_LEFTUP);
			mousemng_buttonevent(MOUSEMNG_RIGHTUP);
			break;

		case activateEvt:
			mackbd_activate((event->modifiers & activeFlag)?TRUE:FALSE);
			break;
	}
}


// ----

static void framereset(void) {

	framecnt = 0;
	if (np2oscfg.DISPCLK & 3) {
		if (sysmng_workclockrenewal()) {
			sysmng_updatecaption(3);
		}
	}
}

static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		timing_setcount(0);
		framereset();
	}
}

int main(int argc, char *argv[]) {

	Rect		wRect;
	EventRecord	event;
	UINT		t;
	GrafPtr		saveport;
	Point		pt;

	dosio_init();
	file_setcd(target);

	InitToolBox();
	macossub_init();
	initload();

	MenuBarInit();

	TRACEINIT();

	keystat_initialize();

	SetRect(&wRect, np2oscfg.posx, np2oscfg.posy, 100, 100);
#if !defined(SUPPORT_PC9821)
	hWndMain = NewWindow(0, &wRect, "\pNeko Project II", FALSE,
								noGrowDocProc, (WindowPtr)-1, TRUE, 0);
#else
	hWndMain = NewWindow(0, &wRect, "\pNeko Project 21", FALSE,
								noGrowDocProc, (WindowPtr)-1, TRUE, 0);
#endif
	if (!hWndMain) {
		TRACETERM();
		macossub_term();
		dosio_term();
		return(0);
	}
	scrnmng_initialize();
	SizeWindow(hWndMain, 640, 400, TRUE);
	ShowWindow(hWndMain);

	menu_setrotate(0);
	menu_setdispmode(np2cfg.DISPSYNC);
	menu_setraster(np2cfg.RASTER);
	menu_setwaitflg(np2oscfg.NOWAIT);
	menu_setframe(np2oscfg.DRAW_SKIP);

	menu_setkey(0);
	menu_setxshift(0);
	menu_setf11key(np2oscfg.F11KEY);
	menu_setf12key(np2oscfg.F12KEY);
	menu_setbeepvol(np2cfg.BEEP_VOL);
	menu_setsound(np2cfg.SOUND_SW);
	menu_setjastsnd(np2oscfg.jastsnd);
	menu_setmotorflg(np2cfg.MOTOR);
	menu_setextmem(np2cfg.EXTMEM);

	menu_setdispclk(np2oscfg.DISPCLK);
	menu_setbtnrapid(np2cfg.BTN_RAPID);
	menu_setbtnmode(np2cfg.BTN_MODE);

	scrnmode = 0;
	if (scrnmng_create(scrnmode) != SUCCESS) {
		TRACETERM();
		macossub_term();
		dosio_term();
		DisposeWindow(hWndMain);
		return(0);
	}

	np2open();
	t = GETTICK();
	while((GETTICK() - t) < 100) {
		if (WaitNextEvent(everyEvent, &event, 0, 0)) {
			eventproc(&event);
		}
	}

	commng_initialize();
	sysmng_initialize();
	mackbd_initialize();
	pccore_init();
	S98_init();

	mousemng_initialize();
	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mousemng_enable(MOUSEPROC_SYSTEM);
	}
//	scrndraw_redraw();
	pccore_reset();

#if defined(SUPPORT_RESUME)
	if (np2oscfg.resume) {
		flagload(np2resumeext, FALSE);
	}
#endif

	SetEventMask(everyEvent);

	np2running = TRUE;
	while(np2running) {
		if (WaitNextEvent(everyEvent, &event, 0, 0)) {
			eventproc(&event);
		}
		else {
			if (np2oscfg.NOWAIT) {
				mackbd_callback();
				mousemng_callback();
				pccore_exec(framecnt == 0);
				if (np2oscfg.DRAW_SKIP) {			// nowait frame skip
					framecnt++;
					if (framecnt >= np2oscfg.DRAW_SKIP) {
						processwait(0);
					}
				}
				else {								// nowait auto skip
					framecnt = 1;
					if (timing_getcount()) {
						processwait(0);
					}
				}
			}
			else if (np2oscfg.DRAW_SKIP) {			// frame skip
				if (framecnt < np2oscfg.DRAW_SKIP) {
					mackbd_callback();
                    mousemng_callback();
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
					mackbd_callback();
                    mousemng_callback();
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

	GetPort(&saveport);
#if TARGET_API_MAC_CARBON
	SetPortWindowPort(hWndMain);
#else
	SetPort(hWndMain);
#endif
	pt.h = 0;
	pt.v = 0;
	LocalToGlobal(&pt);
	SetPort(saveport);
	if ((np2oscfg.posx != pt.h) || (np2oscfg.posy != pt.v)) {
		np2oscfg.posx = pt.h;
		np2oscfg.posy = pt.v;
		sysmng_update(SYS_UPDATEOSCFG);
	}

	np2running = FALSE;

	pccore_cfgupdate();

#if defined(SUPPORT_RESUME)
	if (np2oscfg.resume) {
		flagsave(np2resumeext);
	}
	else {
		flagdelete(np2resumeext);
	}
#endif

	pccore_term();
	S98_trash();

	mousemng_disable(MOUSEPROC_SYSTEM);

	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	macossub_term();
	dosio_term();

	DisposeWindow(hWndMain);

	(void)argc;
	(void)argv;
	return(0);
}

