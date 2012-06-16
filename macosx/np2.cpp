#include	"compiler.h"
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"dosio.h"
#include	"commng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"mackbd.h"
#include	"ini.h"
#include	"menu.h"
#include	"dialog.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"timing.h"
#include	"debugsub.h"
#include	"bios.h"
#include	"scrndraw.h"
#include	"sound.h"
#include	"beep.h"
#include	"s98.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"mousemng.h"
#include	"configure.h"
#include	"screenopt.h"
#include	"soundopt.h"
#include	"macnewdisk.h"
#include	"fdefine.h"
#include	"hid.h"
#include	"midiopt.h"
#include	"macalert.h"
#include	"np2opening.h"
#include	"toolwin.h"
#include	"aboutdlg.h"
#include	"keystat.h"
#include	"subwind.h"
#include	"macclnd.h"

#define	NP2OPENING
// #define	OPENING_WAIT	1500


#if !defined(SUPPORT_PC9821)
		NP2OSCFG	np2oscfg = {"Neko Project IIx",						//titles
#else
		NP2OSCFG	np2oscfg = {"Neko Project 21x",						//titles
#endif
								-1,										//winx
								-1,										//winy
								0,										//NOWAIT
								0,										//DRAW_SKIP
								0,										//DISPCLK
								0,										//F11KEY
								0,										//F12KEY
								0,										//MOUSE_SW
								0,										//JOYPAD1
								{5, 0, 0x3e, 19200, "", "", "GM", ""},  //mpu
								0,										//confirm
								0,										//resume
								1,										//toolwin
#if defined(SUPPORT_KEYDISP)
								0,										//keydisp
#endif
#if defined(SUPPORT_SOFTKBD)
								0,										//softkey
#endif

								0,										//jastsnd
								0,										//I286SAVE
								};

		WindowPtr	hWndMain;
		BOOL		np2running;
static	UINT		framecnt = 0;
static	UINT		waitcnt = 0;
static	UINT		framemax = 1;
        BYTE		scrnmode = 0;


#define DRAG_THRESHOLD		5

#ifndef NP2GCC
#define	DEFAULTPATH		":"
#else
#define	DEFAULTPATH		"::::"
#endif
static	char	target[MAX_PATH] = DEFAULTPATH;


static const char np2resume[] = "sav";


// ---- おまじない

#define	AEProc(fn)	NewAEEventHandlerUPP((AEEventHandlerProcPtr)(fn))

static void setUpCarbonEvent(void);
static bool setupMainWindow(void);
static void toggleFullscreen(void);

static pascal OSErr handleQuitApp(const AppleEvent *event, AppleEvent *reply,
															long refcon) {

	taskmng_exit();

	(void)event, (void)reply, (void)refcon;
	return(noErr);
}

pascal OSErr OpenAppleEventHandler(const AppleEvent *event, AppleEvent *reply,long print)
{
	long		i,ct,len;
	FSSpec		fsc,*pp;
	DescType	rtype;
	AEKeyword	key;
	AEDescList	dlist;
    OSErr		err = noErr;
        
	if(!AEGetParamDesc(event,keyDirectObject,typeAEList,&dlist))	{
		AECountItems( &dlist,&ct );
		for( i=1;i<=ct;i++ )	{
            pp=&fsc;
			if (!AEGetNthPtr( &dlist,i,typeFSS,&key,&rtype,(Ptr)pp,(long)sizeof(FSSpec),&len))	{
                err = setDropFile(fsc, i-1);
			}
		}
		AEDisposeDesc( &dlist );
	}
	return(err);
}


static void InitToolBox(void) {

	FlushEvents(everyEvent, 0);
	InitCursor();

	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
						AEProc(handleQuitApp), 0L, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
						AEProc(OpenAppleEventHandler), 0L, false);
}

static void MenuBarInit(void) {
#if 1
	Handle		hMenu;

	hMenu = GetNewMBar(IDM_MAINMENU);
	if (!hMenu) {
		ExitToShell();
	}
	SetMenuBar(hMenu);
#if !defined(SUPPORT_SCSI)
	hmenu = GetMenuHandle(IDM_HDD);
	if (hmenu) {
		DeleteMenuItem(hmenu, 7);
		DeleteMenuItem(hmenu, 6);
		DeleteMenuItem(hmenu, 5);
		DeleteMenuItem(hmenu, 4);
		DeleteMenuItem(hmenu, 3);
	}
#endif
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
    ChangeMenuAttributes(GetMenuRef(IDM_EDIT), kMenuAttrAutoDisable, 0);
    DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
    SetMenuItemModifiers(GetMenuRef(IDM_FDD2), IDM_FDD2OPEN, kMenuShiftModifier);
    SetMenuItemModifiers(GetMenuRef(IDM_FDD2), IDM_FDD2EJECT, kMenuShiftModifier);
    SetMenuItemModifiers(GetMenuRef(IDM_SASI2), IDM_SASI2OPEN, kMenuShiftModifier);
#ifndef SUPPORT_KEYDISP
	DisableMenuItem(GetMenuRef(IDM_OTHER), IDM_KEYDISP);
#endif
#ifndef SUPPORT_SOFTKBD
	SetMenuItemTextWithCFString(GetMenuRef(IDM_OTHER), IDM_SOFTKBD, CFSTR("please wait for 0.80"));
	DisableMenuItem(GetMenuRef(IDM_OTHER), IDM_SOFTKBD);
#endif
    if (np2oscfg.I286SAVE) {
        AppendMenuItemTextWithCFString(GetMenuRef(IDM_OTHER), CFCopyLocalizedString(CFSTR("i286 save"),"i286"), kMenuItemAttrIconDisabled, 0, 0);
    }
	
#if defined(SUPPORT_PC9821)
	AppendMenuItemTextWithCFString(GetMenuRef(IDM_MEMORY), CFSTR("11.6MB"), kMenuItemAttrIconDisabled, 0, NULL);
	AppendMenuItemTextWithCFString(GetMenuRef(IDM_MEMORY), CFSTR("13.6MB"), kMenuItemAttrIconDisabled, 0, NULL);
	SetMenuItemTextWithCFString(GetMenuRef(IDM_HELP), IDM_NP2HELP, CFCopyLocalizedString(CFSTR("Help"), "Help"));
	SetMenuItemTextWithCFString(GetMenuRef(IDM_APPLE), IDM_ABOUT, CFCopyLocalizedString(CFSTR("About"), "About"));
	SetMenuItemTextWithCFString(GetMenuRef(IDM_OTHER), IDM_I286SAVE, CFCopyLocalizedString(CFSTR("i386 save"), "save"));
#endif

	if (!(np2cfg.fddequip & 1)) {
		DisableAllMenuItems(GetMenuRef(IDM_FDD1));
	}
	if (!(np2cfg.fddequip & 2)) {
		DisableAllMenuItems(GetMenuRef(IDM_FDD2));
	}
#if 0
//Later...
	if (!(np2cfg.fddequip & 4)) {
		DisableAllMenuItems(GetMenuRef(IDM_FDD3));
	}
	if (!(np2cfg.fddequip & 8)) {
		DisableAllMenuItems(GetMenuRef(IDM_FDD4));
	}
#endif

	DrawMenuBar();
#else
    OSStatus	err;
    IBNibRef	nibRef;    
    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err!=noErr) return;
    err = SetMenuBarFromNib(nibRef, CFSTR("MainMenu"));
    DisposeNibReference(nibRef);
#endif
}

static void changescreen(BYTE mode) {

	BYTE	change;
	BYTE	renewal;

	change = scrnmode ^ mode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	if (mode & SCRNMODE_FULLSCREEN) {
		renewal |= (change & SCRNMODE_HIGHCOLOR);
	}
	else {
		renewal |= (change & SCRNMODE_ROTATEMASK);
	}
	if (renewal) {
		soundmng_stop();
		scrnmng_destroy();
		if (scrnmng_create(mode) == SUCCESS) {
			scrnmode = mode;
		}
		scrndraw_redraw();
		soundmng_play();
	}
	else {
		scrnmode = mode;
	}
}

void HandleMenuChoice(long wParam) {

	UINT	update;

    soundmng_stop();
	update = 0;
	switch(wParam) {
		case IDM_ABOUT:
			AboutDialogProc();
			break;

		case IDM_RESET:
            if (ResetWarningDialogProc()) {
                pccore_cfgupdate();
                pccore_reset();
            }
			break;
            
		case IDM_CONFIGURE:
			initConfig();
			break;

		case IDM_NEWDISK:
            newdisk();
			break;

        case IDM_FONT:
            dialog_font();
            break;
            
		case IDM_EXIT:
			taskmng_exit();
			break;

		case IDM_FDD1OPEN:
			dialog_changefdd(0);
			break;

		case IDM_FDD1EJECT:
			diskdrv_setfdd(0, NULL, 0);
            toolwin_setfdd(0, NULL);
			break;

		case IDM_FDD2OPEN:
			dialog_changefdd(1);
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(1, NULL, 0);
            toolwin_setfdd(1, NULL);
			break;

		case IDM_SASI1OPEN:
			dialog_changehdd(0);
			break;

		case IDM_SASI1REMOVE:
			diskdrv_sethdd(0, NULL);
			break;

		case IDM_SASI2OPEN:
			dialog_changehdd(1);
			break;

		case IDM_SASI2REMOVE:
			diskdrv_sethdd(1, NULL);
			break;

#if defined(SUPPORT_SCSI)
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
#endif

		case IDM_FULLSCREEN:
            toggleFullscreen();
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
			initScreenOpt();
			break;

        case IDM_MOUSE:
			mousemng_toggle(MOUSEPROC_SYSTEM);
            menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
            sysmng_update(SYS_UPDATECFG);
			break;
            
        case IDM_MIDIOPT:
            initMidiOpt();
            break;
            
		case IDM_MIDIPANIC:
			rs232c_midipanic();
			mpu98ii_midipanic();
			pc9861k_midipanic();
			break;

        case IDM_SNDOPT:
            initSoundOpt();
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

		case IDM_F11EQU:
			menu_setf11key(2);
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

		case IDM_F12COMMA:
			menu_setf12key(2);
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

		case IDM_JASTSOUND:
			menu_setjastsound(np2oscfg.jastsnd ^ 1);
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

        case IDM_S98LOGGING:
            dialog_s98();
            break;

        case IDM_CALENDAR:
			initClnd();
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

        case IDM_MSRAPID:
            menu_setmsrapid(np2cfg.MOUSERAPID ^ 1);
            update |= SYS_UPDATECFG;
            break;
			
        case IDM_RECORDING:
            menu_setrecording(false);
            break;

#if defined(SUPPORT_KEYDISP)
		case IDM_KEYDISP:
			menu_setkeydisp(np2oscfg.keydisp ^ 1);
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
			menu_setsoftwarekeyboard(np2oscfg.softkey ^ 1);
			if (np2oscfg.softkey) {
				skbdwin_create();
			}
			else {
				skbdwin_destroy();
			}
			break;
#endif

		case IDM_I286SAVE:
			debugsub_status();
			break;
            
        case IDM_NP2HELP:
            {
                ICInstance inst;
                long start, fin;
                const char	urlStr[] = "http://retropc.net/tk800/np2x/help/help.html";

                ICStart(&inst, 'SMil');
                start = 0;
                fin = strlen(urlStr);
                ICLaunchURL(inst, "\p", urlStr, strlen(urlStr), &start, &fin);
                ICStop(inst);
            }
            break;

		default:
			break;
	}
	sysmng_update(update);
	HiliteMenu(0);
}

static void HandleMouseDown(EventRecord *pevent) {

	WindowPtr	hWnd;

	if (FindWindow(pevent->where, &hWnd) == inMenuBar) {
        soundmng_stop();
        mousemng_disable(MOUSEPROC_MACUI);
        HandleMenuChoice(MenuSelect(pevent->where));
        mousemng_enable(MOUSEPROC_MACUI);
    }
    else {
        mousemng_buttonevent(MOUSEMNG_LEFTDOWN);
    }
}


// ----

static void framereset(UINT waitcnt) {

	framecnt = 0;
	kdispwin_draw((BYTE)waitcnt);
	toolwin_draw((BYTE)waitcnt);
	skbdwin_process();
	if (np2oscfg.DISPCLK & 3) {
		if (sysmng_workclockrenewal()) {
			sysmng_updatecaption(3);
		}
	}
}

static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		timing_setcount(0);
		framereset(waitcnt);
	}
	soundmng_sync();
}

static void getstatfilename(char *path, const char *ext, int size) {

	file_cpyname(path, file_getcd("np2"), size);
	file_catname(path, str_dot, size);
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

static void flagload(const char *ext) {

	char	path[MAX_PATH];
	char	buf[1024];
	int		ret, r;

    ret = IDOK;
	getstatfilename(path, ext, sizeof(path));
	r = statsave_check(path, buf, sizeof(buf));
	if (r & (~STATFLAG_DISKCHG)) {
		ResumeErrorDialogProc();
		ret = IDCANCEL;
	}
	else if (r & STATFLAG_DISKCHG) {
		ret = ResumeWarningDialogProc(buf);
	}
	if (ret == IDOK) {
		statsave_load(path);
		toolwin_setfdd(0, fdd_diskname(0));
		toolwin_setfdd(1, fdd_diskname(1));
	}
	return;
}

int main(int argc, char *argv[]) {

    EventRef		theEvent;
    EventTargetRef	theTarget;
    Rect			bounds;
#ifdef OPENING_WAIT
	UINT32		tick;
#endif
  
	dosio_init();
	file_setcd(target);

	InitToolBox();
	macossub_init();
	initload();

	MenuBarInit();

	TRACEINIT();
    
	keystat_initialize();
	kdispwin_initialize();
	skbdwin_readini();

	toolwin_readini();
	kdispwin_readini();
    if (!(setupMainWindow())) {
        return(0);
    }
	skbdwin_initialize();

#ifdef    NP2OPENING
    openingNP2();
#endif
#ifdef OPENING_WAIT
	tick = GETTICK();
#endif

	menu_setrotate(0);
	menu_setdispmode(np2cfg.DISPSYNC);
	menu_setwaitflg(np2oscfg.NOWAIT);
	menu_setframe(np2oscfg.DRAW_SKIP);
	menu_setkey(0);
	menu_setxshift(0);
	menu_setf11key(np2oscfg.F11KEY);
	menu_setf12key(np2oscfg.F12KEY);
	menu_setbeepvol(np2cfg.BEEP_VOL);
	menu_setsound(np2cfg.SOUND_SW);
	menu_setjastsound(np2oscfg.jastsnd);
	menu_setmotorflg(np2cfg.MOTOR);
	menu_setextmem(np2cfg.EXTMEM);
	menu_setdispclk(np2oscfg.DISPCLK);
	menu_setbtnrapid(np2cfg.BTN_RAPID);
	menu_setbtnmode(np2cfg.BTN_MODE);
#if defined(SUPPORT_KEYDISP)
	menu_setkeydisp(np2oscfg.keydisp);
#endif
#if defined(SUPPORT_SOFTKBD)
	menu_setsoftwarekeyboard(np2oscfg.softkey);
#endif

	scrnmng_initialize();
	if (scrnmng_create(scrnmode) != SUCCESS) {
		TRACETERM();
		macossub_term();
		dosio_term();
		DisposeWindow(hWndMain);
		return(0);
	}

	commng_initialize();
	sysmng_initialize();
	mackbd_initialize();
	pccore_init();
	S98_init();

    hid_init();
#ifndef SUPPORT_SWSEEKSND
	if (soundmng_initialize() == SUCCESS) {
		soundmng_pcmvolume(SOUND_PCMSEEK, np2cfg.MOTORVOL);
		soundmng_pcmvolume(SOUND_PCMSEEK1, np2cfg.MOTORVOL);
	}
#endif

#if defined(NP2GCC)
	mousemng_initialize();
	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mousemng_enable(MOUSEPROC_SYSTEM);
	}
#endif

#ifdef OPENING_WAIT
	while((GETTICK() - tick) < OPENING_WAIT);
#endif
	pccore_reset();

#if defined(USE_RESUME)
    if (np2oscfg.resume) {
        flagload(np2resume);
    }
#endif
#if defined(SUPPORT_KEYDISP)
	if (np2oscfg.keydisp) {
		kdispwin_create();
	}
#endif
#if defined(SUPPORT_SOFTKBD)
	if (np2oscfg.softkey) {
		skbdwin_create();
	}
#endif

    theTarget = GetEventDispatcherTarget();

	np2running = TRUE;
	while(np2running) {
        if (ReceiveNextEvent(0, NULL,kEventDurationNoWait,true, &theEvent)== noErr)
        {
            SendEventToEventTarget (theEvent, theTarget);
            ReleaseEvent(theEvent);
        }
		else {
            soundmng_play();
			if (np2oscfg.NOWAIT) {
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
	}
    
	np2running = FALSE;
    menu_setrecording(true);

    if (scrnmode & SCRNMODE_FULLSCREEN) {
        toggleFullscreen();
    }
    
	GetWindowBounds(hWndMain, kWindowGlobalPortRgn, &bounds);
	if ((np2oscfg.winx != bounds.left) || (np2oscfg.winy != bounds.top)) {
		np2oscfg.winx = bounds.left;
		np2oscfg.winy = bounds.top;
		sysmng_update(SYS_UPDATEOSCFG);
	}

	pccore_cfgupdate();

#if defined(USE_RESUME)
    if (np2oscfg.resume) {
        flagsave(np2resume);
    }
	else {
		flagdelete(np2resume);
	}
#endif

	pccore_term();
	S98_trash();

    hid_clear();
#if defined(NP2GCC)
	mousemng_disable(MOUSEPROC_SYSTEM);
#endif
#ifndef SUPPORT_SWSEEKSND
	soundmng_deinitialize();
#endif
	scrnmng_destroy();

	kdispwin_destroy();
	skbdwin_destroy();
	if (sys_updates & (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();						// np2.cfg create
	    toolwin_writeini();				// np2.cfg append
		kdispwin_writeini();
		skbdwin_writeini();
	}
	skbdwin_deinitialize();
	TRACETERM();
	macossub_term();
	dosio_term();

    toolwin_close();
	DisposeWindow(hWndMain);

	(void)argc;
	(void)argv;
	return(0);
}

//以下、ごっそりIIxからマージ
static pascal OSStatus np2appevent (EventHandlerCallRef myHandlerChain, EventRef event, void* userData)
{
    OSStatus	result = eventNotHandledErr;
    
    long		eventClass;
    UInt32		whatHappened;
    eventClass = GetEventClass(event);
    whatHappened = GetEventKind(event);

    EventRecord	eve;
    ConvertEventRefToEventRecord( event,&eve );
    if (IsDialogEvent(&eve)) return result;

    UInt32		modif;
    GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);

#if defined(NP2GCC)
    HIPoint		delta;
    EventMouseButton buttonKind;
    GetEventParameter (event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &buttonKind);
#endif
        
    switch (eventClass)
    {
        case kEventClassCommand:
            if (GetEventKind(event)==kEventCommandProcess) {
                HICommand	cmd;
                GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
                if (cmd.commandID == kHICommandAppHelp) {
                    ICInstance inst;
                    long start, fin;
                    const char	urlStr[] = "http://retropc.net/tk800/np2x/help/help.html";

                    ICStart(&inst, 'SMil');
                    start = 0;
                    fin = strlen(urlStr);
                    ICLaunchURL(inst, "\p", urlStr, strlen(urlStr), &start, &fin);
                    ICStop(inst);
                }
            }
            break;

        case kEventClassAppleEvent:  
            if (whatHappened == kEventAppleEvent) {
                AEProcessAppleEvent(&eve);
            }
            break;

        case kEventClassMouse: 
#if defined(NP2GCC)
            switch (whatHappened)
            {
                case kEventMouseMoved:
                case kEventMouseDragged:
                    GetEventParameter (event, kEventParamMouseDelta, typeHIPoint, NULL, sizeof(HIPoint), NULL, &delta);
                    mousemng_callback(delta);
                    result = noErr;
                    break;
                case kEventMouseDown:
                    if (buttonKind == kEventMouseButtonSecondary || modif & cmdKey) {
                        mousemng_buttonevent(MOUSEMNG_RIGHTDOWN);
                    }
                    else {
                        HandleMouseDown(&eve);
                    }
                    result=noErr;
                    break;
                case kEventMouseUp:
                    if (buttonKind == kEventMouseButtonSecondary || modif & cmdKey) {
                        mousemng_buttonevent(MOUSEMNG_RIGHTUP);
                    }
                    else if (buttonKind == kEventMouseButtonTertiary) {
                        mousemng_toggle(MOUSEPROC_SYSTEM);
                        menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
                        sysmng_update(SYS_UPDATECFG);
                    }
                    else {
                        mousemng_buttonevent(MOUSEMNG_LEFTUP);
                    }
                    result=noErr;
                    break;    
                }
#else
                if (whatHappened == kEventMouseDown) {
                    HandleMouseDown(&eve);
                }
#endif
                break;
		case kEventClassKeyboard:
                if (GetEventKind(event)==kEventRawKeyModifiersChanged) {
					static  UInt32  backup = 0;
					UInt32  change = backup ^ modif;
					backup = modif;
					if (change & shiftKey) {
						if (modif & shiftKey) keystat_senddata(0x70);
						else keystat_senddata(0x70 | 0x80);
					}
					if (change & optionKey) {
						if (modif & optionKey) keystat_senddata(0x73);
						else keystat_senddata(0x73 | 0x80);
					}
					if (change & controlKey) {
						if (modif & controlKey) keystat_senddata(0x74);
						else keystat_senddata(0x74 | 0x80);
					}
                    if (change & alphaLock) {
                        keystat_senddata(0x71);
                    }
                    result = noErr;
				}
				break;
        default:
            break; 
    }

	(void)myHandlerChain;
	(void)userData;
    return result; 
}

static pascal OSStatus np2windowevent(EventHandlerCallRef myHandler,  EventRef event, void* userData)
{
    WindowRef	window;
    UInt32		whatHappened;
    OSStatus	result = eventNotHandledErr;    
    long		eventClass;
    
    GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL,
                         sizeof(window), NULL, &window);
    eventClass = GetEventClass(event);
    whatHappened = GetEventKind(event);
        
    switch (eventClass)
    {        
        case kEventClassKeyboard:
            UInt32 key;
            GetEventParameter (event, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &key);
            UInt32 modif;
            GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);
            switch (whatHappened)
            {
                case kEventRawKeyUp:
                    mackbd_keyup(key);
                    result = noErr;
                    break;
                case kEventRawKeyRepeat:
                    mackbd_keydown(key);
                    result = noErr;
                    break;
                case kEventRawKeyDown:
                    if (modif & cmdKey) {
                        EventRecord	eve;
                        ConvertEventRefToEventRecord( event,&eve );
                        mousemng_disable(MOUSEPROC_MACUI);
                        HandleMenuChoice(MenuEvent(&eve));
                        mousemng_enable(MOUSEPROC_MACUI);
                    }
                    else {
                        mackbd_keydown(key);
                    }
                    result = noErr;
                    break;
                default: 
                    break;             
            }
            break;
            
        case kEventClassWindow:
            switch (whatHappened)
            {
                case kEventWindowClose:
                    taskmng_exit();
                    result = noErr;
                    break;
                case kEventWindowActivated:
                    DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
                    break;
                case kEventWindowToolbarSwitchMode:
                    toolwin_open();
                    break;
                case kEventWindowDragStarted:
                    soundmng_stop();
                    break;
                case kEventWindowDragCompleted:
                    soundmng_play();
                    break;
                case kEventWindowShown:
                    scrndraw_redraw();
                    break;
            }
            break;
        default: 
            break;                
    }

	(void)myHandler;
	(void)userData;
    return result;
}

static const EventTypeSpec appEventList[] = {
                {kEventClassCommand, 	kEventCommandProcess },
				{kEventClassAppleEvent,	kEventAppleEvent},
				{kEventClassMouse,		kEventMouseDown},
#if defined(NP2GCC)
				{kEventClassMouse,		kEventMouseMoved},
				{kEventClassMouse,		kEventMouseDragged},
				{kEventClassMouse,		kEventMouseUp},
#endif
				{kEventClassKeyboard,	kEventRawKeyModifiersChanged},
			};

static const EventTypeSpec windEventList[] = {
				{kEventClassWindow,		kEventWindowClose},
				{kEventClassWindow,		kEventWindowActivated},
				{kEventClassWindow,		kEventWindowToolbarSwitchMode},
				{kEventClassWindow,		kEventWindowDragStarted},
				{kEventClassWindow,		kEventWindowDragCompleted},
				{kEventClassWindow,		kEventWindowShown},
				{kEventClassKeyboard,	kEventRawKeyDown},
				{kEventClassKeyboard,	kEventRawKeyUp},
				{kEventClassKeyboard,	kEventRawKeyRepeat},
			};


static void setUpCarbonEvent(void) {

	InstallStandardEventHandler(GetWindowEventTarget(hWndMain));
	InstallApplicationEventHandler(NewEventHandlerUPP(np2appevent),
								GetEventTypeCount(appEventList),
								appEventList, 0, NULL);
	InstallWindowEventHandler(hWndMain, NewEventHandlerUPP(np2windowevent),
								GetEventTypeCount(windEventList),
								windEventList, 0, NULL);
}

static short	backupwidth=0, backupheight=0;

static bool setupMainWindow(void) {
#if defined(NP2GCC)
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &hWndMain);
        DisposeNibReference ( nibRef);
    }
    else {
         return(false);
    }
    
#else
    Rect wRect;
    
	SetRect(&wRect, 100, 100, 100, 100);
	hWndMain = NewWindow(0, &wRect, "\pNeko Project II", FALSE,
								noGrowDocProc, (WindowPtr)-1, TRUE, 0);
	if (!hWndMain) {
		TRACETERM();
		macossub_term();
		dosio_term();
		return(false);
	}
	SizeWindow(hWndMain, 640, 400, TRUE);
#endif
    if (np2oscfg.winx != -1 && np2oscfg.winy != -1) {
        MoveWindow(hWndMain, np2oscfg.winx, np2oscfg.winy, false);
    }
    setUpCarbonEvent();
    if (backupwidth) scrnmng_setwidth(0, backupwidth);
    if (backupheight) scrnmng_setheight(0, backupheight);
	CFStringRef title = CFStringCreateWithCString(NULL, np2oscfg.titles, kCFStringEncodingUTF8);
	if (title) {
		SetWindowTitleWithCFString(hWndMain, title);
		CFRelease(title);
	}
	ShowWindow(hWndMain);
    return(true);
}

static void toggleFullscreen(void) {
    static Ptr 	bkfullscreen;
    static BYTE mouse = 0;
    static bool toolwin = false;
    MenuRef	menu = GetMenuRef(IDM_SCREEN);
    Rect	bounds;
    short	w, h;

    soundmng_stop();
    if (!(scrnmode & SCRNMODE_FULLSCREEN)) {
        HandleMenuChoice(IDM_ROLNORMAL);
        GetWindowBounds(hWndMain, kWindowContentRgn, &bounds);
        backupwidth = bounds.right - bounds.left;
        backupheight = bounds.bottom - bounds.top;
		w = backupwidth;
		h = backupheight;
        toolwin = np2oscfg.toolwin;
        toolwin_close();
        np2oscfg.winx = bounds.left;
        np2oscfg.winy = bounds.top;
        DisposeWindow(hWndMain);
        BeginFullScreen(&bkfullscreen, 0, &w, &h, &hWndMain, NULL, fullScreenAllowEvents);	
        DisableMenuItem(menu, IDM_ROLNORMAL);
        DisableMenuItem(menu, IDM_ROLLEFT);
        DisableMenuItem(menu, IDM_ROLRIGHT);
        HideMenuBar();
        setUpCarbonEvent();
        if (!np2oscfg.MOUSE_SW) {
            mouse = np2oscfg.MOUSE_SW;
            mousemng_enable(MOUSEPROC_SYSTEM);
            menu_setmouse(1);
        }
        changescreen(scrnmode | SCRNMODE_FULLSCREEN);
    }
    else {
        scrnmng_destroy();
        EndFullScreen(bkfullscreen, 0);
        setupMainWindow();
        changescreen(scrnmode & (~SCRNMODE_FULLSCREEN));
        if (!mouse) {
            mousemng_disable(MOUSEPROC_SYSTEM);
            menu_setmouse(0);
        }
        EnableMenuItem(menu, IDM_ROLNORMAL);
        EnableMenuItem(menu, IDM_ROLLEFT);
        EnableMenuItem(menu, IDM_ROLRIGHT);
        ShowMenuBar();
        if (toolwin) {
            toolwin_open();
        }
    }
    CheckMenuItem(GetMenuHandle(IDM_SCREEN), LoWord(IDM_FULLSCREEN), scrnmode & SCRNMODE_FULLSCREEN);
    soundmng_play();
}
