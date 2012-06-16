#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"ini.h"
#include	"subwind.h"
#include	"keydisp.h"
#include	"cmndraw.h"
#include	"dosio.h"
#include	"softkbd.h"

#if !defined(SUPPORT_PC9821)
static const char inifile[] = "np2.cfg";			// same file name..
#else
static const char inifile[] = "np21.cfg";
#endif
#define	FLOATINGWINDOWTITLEOFFSET 16

#if defined(SUPPORT_KEYDISP)
#define KEYDISP_PALS	3

enum {
	KDISPCFG_FM		= 0x00,
	KDISPCFG_MIDI	= 0x80
};

enum {
	IDM_KDCLOSE		= 3000
};

typedef struct {
	WindowRef		hwnd;
	Point			wlex;
	PixMapHandle	dd2hdl;
	bool			drawed;
} KDWIN;

typedef struct {
	int		posx;
	int		posy;
	BYTE	mode;
	BYTE	type;
} KDISPCFG;

static	KDWIN		kdwin;
static	KDISPCFG	kdispcfg;

static const UINT32 kdwinpal[KEYDISP_PALS] =
									{0x00000000, 0xffffffff, 0xf9ff0000};

static const INITBL kdispini[] = {
	{"WindposX", INITYPE_SINT32,	&kdispcfg.posx,			0},
	{"WindposY", INITYPE_SINT32,	&kdispcfg.posy,			0},
	{"keydmode", INITYPE_UINT8,		&kdispcfg.mode,			0},
	{"windtype", INITYPE_BOOL,		&kdispcfg.type,			0}};

static BYTE kdgetpal8(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdwinpal[num] >> 24);
	}
	return(0);
}

static UINT32 kdgetpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdwinpal[num] & 0xffffff);
	}
	return(0);
}

static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32) {
	RGB32   pal;
	
	pal.d = pal32.d & 0xF8F8F8;
	return((UINT16)((pal.p.g << 2) +
						(pal.p.r << 7) + (pal.p.b >> 3)));
}

static void drawwithcopybits(WindowPtr hWnd) {
	GWorldPtr		gw;
	PixMapHandle	pm;
	Rect			rect;
	GrafPtr			dst;
	GrafPtr			port;
	CMNVRAM			vram;
	RgnHandle		theVisibleRgn;
	bool			portchanged;

	port = GetWindowPort(hWnd);
	GetWindowBounds(hWnd, kWindowContentRgn, &rect);
	OffsetRect(&rect, -rect.left, -rect.top);
	
	if (NewGWorld(&gw, CGDisplayBitsPerPixel(kCGDirectMainDisplay), &rect, NULL, NULL, useTempMem) == noErr) {
		pm = GetGWorldPixMap(gw);
		LockPixels(pm);
		vram.ptr = (BYTE *)GetPixBaseAddr(pm);
		vram.width = rect.right - rect.left;
		vram.height = rect.bottom - rect.top;
		vram.bpp = CGDisplayBitsPerPixel(kCGDirectMainDisplay);
		vram.xalign = vram.bpp >> 3;
		vram.yalign = GetPixRowBytes(pm);
		if (vram.ptr) {
			if (keydisp_paint(&vram, TRUE)) {
				theVisibleRgn = NewRgn();
				if (!EmptyRgn(GetPortVisibleRegion(port, theVisibleRgn))) {
					LockPortBits(port);
					portchanged = QDSwapPort(port, &dst);
					CopyBits((BitMap*)(*pm),
							GetPortBitMapForCopyBits(port),
							&rect, &rect, srcCopy, theVisibleRgn);
					if (portchanged) QDSwapPort(dst, NULL);
					UnlockPortBits(port);
				}
				DisposeRgn(theVisibleRgn);
			}
		}
		UnlockPixels(pm);
		DisposeGWorld(gw);
	}
}

static void kddrawkeys(WindowPtr hWnd, BOOL redraw) {

	Rect	rect;
	Rect	draw;
	CMNVRAM vram;
	GrafPtr	port = NULL, dst;
	long	rowbyte;
	BYTE	drawit = FALSE;

	if (kdwin.drawed == false) {
		static int count = 5;
		drawwithcopybits(hWnd);
		count--;
		if (!count) {
			kdwin.drawed = true;
		}
		return;
	}
	
	port = GetWindowPort(hWnd);
	GetWindowBounds(hWnd, kWindowContentRgn, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(KEYDISP_WIDTH, rect.right - rect.left);
	draw.bottom = min(KEYDISP_HEIGHT, rect.bottom - rect.top);
	if ((draw.right <= 0) || (draw.bottom <= 1)) {
		return;
	}
	
    bool portchanged = QDSwapPort(port, &dst);
    LockPortBits(port);
	LockPixels(kdwin.dd2hdl);
	
    rowbyte = GetPixRowBytes(kdwin.dd2hdl);
	vram.ptr = (BYTE *)GetPixBaseAddr(kdwin.dd2hdl) + rowbyte*FLOATINGWINDOWTITLEOFFSET;
	vram.width = rect.right - rect.left;
	vram.height = rect.bottom - rect.top;
	vram.bpp = CGDisplayBitsPerPixel(kCGDirectMainDisplay);
	vram.xalign = vram.bpp >> 3;
	vram.yalign = rowbyte;
	if (vram.ptr) {
		drawit = keydisp_paint(&vram, redraw);
		if (drawit) {
			QDAddRectToDirtyRegion(port, &draw);
		}
	}
	
	UnlockPixels(kdwin.dd2hdl);
	UnlockPortBits(port);
	if (portchanged) QDSwapPort(dst, NULL);
}

static void kdsetkdwinsize(void) {

	int			width;
	int			height;
	Rect		bounds;

	keydisp_getsize(&width, &height);
	GetWindowBounds(kdwin.hwnd, kWindowContentRgn, &bounds);
	SetRect(&bounds, bounds.left, bounds.top, bounds.left+width, bounds.top+height);
	SetWindowBounds(kdwin.hwnd, kWindowContentRgn, &bounds);
}

static void kdsetkeydispmode(BYTE mode) {

	CFStringRef title;
	
	keydisp_setmode(mode);
	if (mode == KEYDISP_MODEFM) {
		title = CFStringCreateWithCString(NULL, "Key Display: FM", kCFStringEncodingMacRoman);
	}
	else {
		title = CFStringCreateWithCString(NULL, "Key Display: MIDI", kCFStringEncodingMacRoman);
	}
	if (title) {
		SetWindowTitleWithCFString(kdwin.hwnd, title);
		CFRelease(title);
	}
}

static pascal OSStatus kdWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;

    if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowClickContentRgn) {
		if (kdispcfg.mode == KDISPCFG_FM) {
			kdispcfg.mode = KDISPCFG_MIDI;
			kdsetkeydispmode(KEYDISP_MODEMIDI);
		}
		else {
			kdispcfg.mode = KDISPCFG_FM;
			kdsetkeydispmode(KEYDISP_MODEFM);
		}
		kdispwin_draw(0);
		err = noErr;
    }
    else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowClose) {
		menu_setkeydisp(np2oscfg.keydisp ^ 1);
		kdispwin_destroy();
		err = noErr;
	}
    else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowShowing) {
		kdwin.drawed = false;
	}
	(void)myHandler;
	(void)userData;
    return err;
}

// ----

BOOL kdispwin_initialize(void) {

	keydisp_initialize();
	return(SUCCESS);
}

void kdispwin_create(void) {

	WindowRef   hwnd;
	BYTE		mode;
	CMNPALFN	palfn;
	GrafPtr		dstport = NULL;

	if (kdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&kdwin, sizeof(kdwin));
    Rect wRect;
	if (kdispcfg.posx < 0) kdispcfg.posx = 8;
	if (kdispcfg.posy < 0) kdispcfg.posy = 30;
	SetRect(&wRect, kdispcfg.posx, kdispcfg.posy, kdispcfg.posx+100, kdispcfg.posy+100);
	CreateNewWindow(kFloatingWindowClass, kWindowStandardFloatingAttributes, &wRect, &hwnd);
	InstallStandardEventHandler(GetWindowEventTarget(hwnd));
	EventTypeSpec	list[]={ 
		{ kEventClassWindow,	kEventWindowClickContentRgn }, 
		{ kEventClassWindow,	kEventWindowClose }, 
		{ kEventClassWindow,	kEventWindowShowing }, 
	};

	EventHandlerRef	ref;
	InstallWindowEventHandler (hwnd, NewEventHandlerUPP(kdWinproc), GetEventTypeCount(list), list, (void *)hwnd, &ref);
	kdwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto kdcre_err1;
	}
	switch(kdispcfg.mode) {
		case KDISPCFG_FM:
		default:
			mode = KEYDISP_MODEFM;
			break;

		case KDISPCFG_MIDI:
			mode = KEYDISP_MODEMIDI;
			break;
	}
    dstport = GetWindowPort(hwnd);
    if (!dstport) {
		goto kdcre_err2;
	}
	kdwin.dd2hdl = GetPortPixMap(dstport);
	kdwin.drawed = false;
	kdsetkeydispmode(mode);
	kdsetkdwinsize();
	ShowWindow(hwnd);
	palfn.get8 = kdgetpal8;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = kdcnvpal16;
	palfn.userdata = (long)kdwin.dd2hdl;
	keydisp_setpal(&palfn);
	return;

kdcre_err2:
	DisposeWindow(hwnd);

kdcre_err1:
	menu_setkeydisp(0);
	sysmng_update(SYS_UPDATEOSCFG);
}

void kdispwin_destroy(void) {

	if (kdwin.hwnd != NULL) {
		Rect rc;
		GetWindowBounds(kdwin.hwnd, kWindowContentRgn, &rc);
		kdispcfg.posx = rc.left;
		kdispcfg.posy = rc.top;
		sysmng_update(SYS_UPDATEOSCFG);
		DisposeWindow(kdwin.hwnd);
		kdwin.hwnd = NULL;
		kdwin.drawed = false;
	}
}

void kdispwin_draw(BYTE cnt) {

	BYTE	flag;

	if (kdwin.hwnd) {
		if (!cnt) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			kdsetkdwinsize();
		}
		kddrawkeys(kdwin.hwnd, FALSE);
	}
}


// ---- ini

static const char kdispapp[] = "NP2 keydisp";

void kdispwin_readini(void) {

	char	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = 8;
	kdispcfg.posy = 30;
	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_read(path, kdispapp, kdispini, sizeof(kdispini)/sizeof(INITBL));
}

void kdispwin_writeini(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_write(path, kdispapp, kdispini, sizeof(kdispini)/sizeof(INITBL), FALSE);
}
#endif



// ---- soft keyboard

#if defined(SUPPORT_SOFTKBD)
typedef struct {
	WindowRef		hwnd;
	PixMapHandle	dd2hdl;
	int				width;
	int				height;
} SKBDWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	type;
} SKBDCFG;

static	SKBDWIN		skbdwin;
static	SKBDCFG		skbdcfg;

static const char skbdapp[] = "Soft Keyboard";
static const INITBL skbdini[] = {
	{"WindposX", INITYPE_SINT32,	&skbdcfg.posx,			0},
	{"WindposY", INITYPE_SINT32,	&skbdcfg.posy,			0},
	{"windtype", INITYPE_BOOL,		&skbdcfg.type,			0}};

static void skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
#if defined(SUPPORT_16BPP)
		case 16:
			for (i=0; i<pals; i++) {
				RGB32   pal;
				pal.d = src[i].d & 0xF8F8F8;
				dst[i].pal16 = (pal.p.g << 2) +	(pal.p.r << 7) + (pal.p.b >> 3);
			}
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
#endif
#if defined(SUPPORT_32BPP)
		case 32:
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
			for (i=0; i<pals; i++) {
				dst[i].pal32.d = src[i].d;
			}
			break;
#endif
	}
}

static void skdrawkeys(WindowRef hWnd, BOOL redraw) {

	Rect	rect;
	Rect	draw;
	CMNVRAM vram;
	GrafPtr	port = NULL, dst;
	long	rowbyte;

	port = GetWindowPort(hWnd);
	GetWindowBounds(hWnd, kWindowContentRgn, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(skbdwin.width, rect.right - rect.left);
	draw.bottom = min(skbdwin.height, rect.bottom - rect.top);
	
    bool portchanged = QDSwapPort(port, &dst);
    LockPortBits(port);
	LockPixels(skbdwin.dd2hdl);
	
    rowbyte = GetPixRowBytes(skbdwin.dd2hdl);
	vram.ptr = (BYTE *)GetPixBaseAddr(skbdwin.dd2hdl) + rowbyte*FLOATINGWINDOWTITLEOFFSET;
	vram.width = rect.right - rect.left;
	vram.height = rect.bottom - rect.top;
	vram.bpp = CGDisplayBitsPerPixel(kCGDirectMainDisplay);
	vram.xalign = vram.bpp >> 3;
	vram.yalign = rowbyte;
	if (vram.ptr) {
		if (softkbd_paint(&vram, skpalcnv, redraw)) {
			QDAddRectToDirtyRegion(port, &draw);
		}
	}
	
	UnlockPixels(skbdwin.dd2hdl);
	UnlockPortBits(port);
	if (portchanged) QDSwapPort(dst, NULL);
}

static pascal OSStatus skproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;

    if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowClickContentRgn) {
		HIPoint   position;
		GetEventParameter (event, kEventParamWindowMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &position);
		
		softkbd_down((int)position.x, (int)position.y-FLOATINGWINDOWTITLEOFFSET);
		err = noErr;
    }
    else if (GetEventClass(event)==kEventClassMouse && GetEventKind(event)==kEventMouseUp) {
		softkbd_up();
	}
    else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowClose) {
		menu_setsoftwarekeyboard(np2oscfg.softkey ^ 1);
		skbdwin_destroy();
		err = noErr;
	}
    else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowShown) {
		skdrawkeys(skbdwin.hwnd, TRUE);
		err = noErr;
	}

	(void)myHandler;
	(void)userData;
    return err;
}

BOOL skbdwin_initialize(void) {

	softkbd_initialize();
	return(SUCCESS);
}

void skbdwin_deinitialize(void) {

	softkbd_deinitialize();
}

void skbdwin_create(void) {

	WindowRef	hwnd;

	if (skbdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&skbdwin, sizeof(skbdwin));
	if (softkbd_getsize(&skbdwin.width, &skbdwin.height) != SUCCESS) {
		return;
	}
    Rect wRect;
	if (skbdcfg.posx < 0) skbdcfg.posx = 12;
	if (skbdcfg.posy < 0) skbdcfg.posy = 38;
	SetRect(&wRect, skbdcfg.posx, skbdcfg.posy, skbdcfg.posx+skbdwin.width, skbdcfg.posy+skbdwin.height+8);
	CreateNewWindow(kFloatingWindowClass, kWindowStandardFloatingAttributes, &wRect, &hwnd);
	InstallStandardEventHandler(GetWindowEventTarget(hwnd));
	EventTypeSpec	skbdlist[]={ 
		{ kEventClassWindow,	kEventWindowClickContentRgn }, 
		{ kEventClassWindow,	kEventWindowShown }, 
		{ kEventClassMouse,		kEventMouseUp }, 
		{ kEventClassWindow,	kEventWindowClose }, 
	};
	EventHandlerRef	ref;
	InstallWindowEventHandler (hwnd, NewEventHandlerUPP(skproc), GetEventTypeCount(skbdlist), skbdlist, (void *)hwnd, &ref);
	skbdwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto skcre_err1;
	}
	skbdwin.dd2hdl = GetPortPixMap(GetWindowPort(hwnd));
	if (skbdwin.dd2hdl == NULL) {
		goto skcre_err2;
	}
	skdrawkeys(skbdwin.hwnd, TRUE);
	ShowWindow(hwnd);
	return;

skcre_err2:
	DisposeWindow(hwnd);

skcre_err1:
	return;
}

void skbdwin_destroy(void) {

	if (skbdwin.hwnd != NULL) {
		Rect rc;
		GetWindowBounds(skbdwin.hwnd, kWindowContentRgn, &rc);
		skbdcfg.posx = rc.left;
		skbdcfg.posy = rc.top;
		sysmng_update(SYS_UPDATEOSCFG);
		DisposeWindow(skbdwin.hwnd);
		skbdwin.hwnd = NULL;
	}
}

void skbdwin_process(void) {

	if ((skbdwin.hwnd) && (softkbd_process())) {
		skdrawkeys(skbdwin.hwnd, FALSE);
	}
}

void skbdwin_readini(void) {

	char	path[MAX_PATH];

	skbdcfg.posx = 12;
	skbdcfg.posy = 38;
	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_read(path, skbdapp, skbdini, sizeof(skbdini)/sizeof(INITBL));
}

void skbdwin_writeini(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_write(path, skbdapp, skbdini, sizeof(skbdini)/sizeof(INITBL), FALSE);
}
#endif

