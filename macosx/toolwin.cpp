#include	"compiler.h"
#include	"np2.h"
#include	"diskdrv.h"
#include	"sysmng.h"
#include	"dosio.h"
#include	"ini.h"
#include	"resource.h"
#include	"toolwin.h"
#include	"np2opening.h"
#include	"dialogutils.h"
#include	"dialog.h"
#include	"soundmng.h"
#include	"fdefine.h"
#include	"mackbd.h"
#include	"mousemng.h"
#include	"pccore.h"
#include	"iocore.h"

extern void HandleMenuChoice(long wParam);

enum {
	IDC_TOOLHDDACC			= 0,
	IDC_TOOLFDD1ACC,
	IDC_TOOLFDD1LIST,
	IDC_TOOLFDD1BROWSE,
	IDC_TOOLFDD1EJECT,
	IDC_TOOLFDD2ACC,
	IDC_TOOLFDD2LIST,
	IDC_TOOLFDD2BROWSE,
	IDC_TOOLFDD2EJECT,
	IDC_TOOLRESET,
	IDC_TOOLPOWER,
	IDC_MAXITEMS,

	IDC_BASE				= 3000,

	IDC_SKINSEL				= 3100,
	IDC_SKINDEF				= 3101,
	IDC_SKINMRU				= 3102,
	IDC_TOOLCLOSE			= IDC_SKINMRU + SKINMRU_MAX
};

enum {
	TCTL_STATIC				= 0,
	TCTL_BUTTON				= 1,
	TCTL_DDLIST				= 2
};

typedef struct {
	char	main[MAX_PATH];
	char	font[64];
	SINT32	fontsize;
	UINT32	color1;
	UINT32	color2;
} TOOLSKIN;

typedef struct {
	UINT	tctl;
const char	*text;
	short	posx;
	short	posy;
	short	width;
	short	height;
	short	extend;
	short	padding;
} SUBITEM;

typedef struct {
	WindowRef		hwnd;
	PicHandle		hbmp;
	BYTE			fddaccess[2];
	BYTE			hddaccess;
	BYTE			_padding;
	int				winflg;
	int				wingx;
	int				wingy;
	int				wintx;
	int				winty;
	UINT			parentcn;
	int				parentx;
	int				parenty;
	PixPatHandle	access[2];
	ControlRef		sub[IDC_MAXITEMS];
} TOOLWIN;

		NP2TOOL		np2tool;
static	TOOLSKIN	toolskin;
static	SUBITEM		subitem[IDC_MAXITEMS];
static	TOOLWIN		toolwin;

#include	"toolwin.res"


typedef struct {
	UInt16	idc;
	BYTE	*counter;
} DISKACC;

static const BYTE fddlist[FDDLIST_DRV] = {
					IDC_TOOLFDD1LIST, IDC_TOOLFDD2LIST};

static const DISKACC diskacc[3] = {
					{IDC_TOOLFDD1ACC,	&toolwin.fddaccess[0]},
					{IDC_TOOLFDD2ACC,	&toolwin.fddaccess[1]},
					{IDC_TOOLHDDACC,	&toolwin.hddaccess}};


static const OSType subcommand[11] ={	'----',
                                        '----','pop1','opn1','ejt1',
                                        '----','pop2','opn2','ejt2',
                                        'rset',
                                        'exit',
                                    };

#define	BASENUMBER	4
static const ControlID popup[2] = { {'pop1', 1}, {'pop2', 2} };
static const ControlID imageid = {'back', 0};

static DragReceiveHandlerUPP	dr;
static DragTrackingHandlerUPP	tr;
static bool	isPUMA, isJaguar;

static void openpopup(Point location);
static void skinchange(bool remake);

// ----

static void checkOSVersion(void) {
    static	long	res = 0;
    if (!res) {
        Gestalt(gestaltSystemVersion, &res);
        if (res<0x1020) {
            isPUMA = true;
			isJaguar = false;
        }
		else if (res<0x1030) {
            isPUMA = false;
			isJaguar = true;
		}
        else {
            isPUMA = false;
			isJaguar = false;
        }
    }
}


static PicHandle skinload(const char *path, Rect* bounds) {

	char		fname[MAX_PATH];
	UINT		i;
	PicHandle	ret;

	ZeroMemory(&toolskin, sizeof(toolskin));
	toolskin.fontsize = 12;
	milstr_ncpy(toolskin.font, str_deffont, sizeof(toolskin.font));
	toolskin.color1 = 0x600000;
	toolskin.color2 = 0xff0000;
	if (path) {
		ini_read(path, skintitle, skinini1, sizeof(skinini1)/sizeof(INITBL));
	}    
	if (toolskin.main[0]) {
		ZeroMemory(subitem, sizeof(defsubitem));
		for (i=0; i<IDC_MAXITEMS; i++) {
			subitem[i].tctl = defsubitem[i].tctl;
			subitem[i].text = defsubitem[i].text;
		}
	}
	else {
		CopyMemory(subitem, defsubitem, sizeof(defsubitem));
	}    
	if (path) {
		ini_read(path, skintitle, skinini2, sizeof(skinini2)/sizeof(INITBL));
	}
	if (toolskin.main[0]) {
		milstr_ncpy(fname, path, sizeof(fname));
		file_cutname(fname);
		file_catname(fname, toolskin.main, sizeof(fname));
        ret = getBMPfromPath(fname, bounds);
		if (ret != NULL) {
			return(ret);
		}
	}
	return(getBMPfromResource("np2tool.bmp", bounds));
}

// ----

static void setlist(ControlRef hwnd, const TOOLFDD *fdd, UINT sel) {
#if 0
	Rect	rc;
	int		width;
	char	dir[MAX_PATH];
	char		basedir[MAX_PATH];
#endif
    MenuHandle	menu;
	UINT		i;
    char		cfname[512];
const char		*q;
const char		*p;
    CFStringRef	str;
    Str255		pname;
    bool		success = false;
    
    GetBevelButtonMenuHandle(hwnd, &menu);
	DeleteMenuItems(menu, 1, fdd->cnt);
#if 0
	GetControlBounds(hwnd, &rc);
	width = rc.right - rc.left - 6;			// border size?
	basedir[0] = '\0';
	if (sel < fdd->cnt) {
		milstr_ncpy(basedir, fdd->name[fdd->pos[sel]], sizeof(basedir));
		file_cutname(basedir);
	}
#endif
	for (i=0; i<fdd->cnt; i++) {
        SInt32		attr = kMenuItemAttrIconDisabled;
		p = fdd->name[fdd->pos[i]];
        success = getLongFileName(cfname, p);
        str = CFStringCreateWithCString(NULL, cfname, CFStringGetSystemEncoding());
        if (str) {
			if (success) {
				if (file_attr(p) != FILEATTR_ARCHIVE) {
					attr |= kMenuItemAttrDisabled;
				}
				AppendMenuItemTextWithCFString(menu, str, attr, 0, 0);
			}
            CFRelease(str);
        }
        else {
#if 0
            milstr_ncpy(dir, p, sizeof(dir));
            file_cutname(dir);
            if (!file_cmpname(basedir, dir)) {
                q = file_getname((char *)p);
            }
            else {
            	calctextsize(dir, sizeof(dir), p, width);
            	q = dir;
            }
#else
            q = file_getname((char *)p);
#endif
            mkstr255(pname, q);
            AppendMenu(menu, pname);
            if (file_attr(p)==FILEATTR_ARCHIVE) {
                EnableMenuItem(menu, i+1);
            }
            else {
                DisableMenuItem(menu, i+1);
            }
        }
		p += sizeof(fdd->name[0]);
	}
	if (sel < fdd->cnt) {
        CFStringRef title;
        SetBevelButtonMenuValue(hwnd, sel+1);
        CopyMenuItemTextAsCFString(menu, sel+1, &title);
		if (title) {
			SetControlTitleWithCFString(hwnd, title);
			CFRelease(title);
		}
    }
    else {
        SetControlTitleWithCFString(hwnd, CFSTR(" "));
		for (i=0; i<fdd->cnt; i++) {
			CheckMenuItem(menu, i+1, 0);
		}
	}
}

static void sellist(UINT drv) {

	ControlRef	hwnd;
	TOOLFDD	*fdd;
	UINT	sel;
    SInt16	selmenu;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	hwnd = toolwin.sub[fddlist[drv]];
	fdd = np2tool.fdd + drv;
    GetBevelButtonMenuValue(hwnd, &selmenu);
    sel = (UINT)--selmenu;
	if (sel < fdd->cnt) {
		diskdrv_setfdd(drv, fdd->name[fdd->pos[sel]], 0);
		fdd->insert = 1;
		setlist(hwnd, fdd, sel);
	}
}

static void remakefddlist(ControlRef hwnd, TOOLFDD *fdd) {

	char	*p;
	UINT	cnt;
	char	*q;
	char	*fname[FDDLIST_MAX];
	UINT	i;
	UINT	j;
	UINT	sel;

	p = fdd->name[0];
	for (cnt=0; cnt<FDDLIST_MAX; cnt++) {
		if (p[0] == '\0') {
			break;
		}
		q = file_getname(p);
		fname[cnt] = q;
		for (i=0; i<cnt; i++) {
			if (file_cmpname(q, fname[fdd->pos[i]]) < 0) {
				break;
			}
		}
		for (j=cnt; j>i; j--) {
			fdd->pos[j] = fdd->pos[j-1];
		}
		fdd->pos[i] = cnt;
		p += sizeof(fdd->name[0]);
	}
	fdd->cnt = cnt;
	sel = (UINT)-1;
	if (fdd->insert) {
		for (i=0; i<cnt; i++) {
			if (fdd->pos[i] == 0) {
				sel = i;
				break;
			}
		}
	}
	setlist(hwnd, fdd, sel);
}

static void accdraw(ControlRef theControl, BYTE access)
{
    GrafPtr	port, dst;
    Rect	bounds;
    bool	portchanged;
    
    GetControlBounds(theControl, &bounds);    
    dst = GetWindowPort(toolwin.hwnd);
    portchanged = QDSwapPort(dst, &port);
    FillCRect(&bounds, toolwin.access[access?1:0]);
    QDAddRectToDirtyRegion(dst, &bounds);
    if (portchanged) QDSwapPort(port, NULL);
}
// ----

static pascal OSStatus cfControlproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;

    if (GetEventClass(event)==kEventClassMenu) {
        switch (GetEventKind(event)) {
            case kEventMenuBeginTracking:
                soundmng_stop();
                break;
            case kEventMenuEndTracking:
                soundmng_play();
                err=noErr;
                break;
            default:
                break;
        }
    }
    else if (GetEventClass(event)==kEventClassControl && GetEventKind(event)==kEventControlContextualMenuClick) {
        HIPoint	location;
		Point   point;
    
        soundmng_stop();
        GetEventParameter (event, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &location);
        point.h = (short)location.x;
		point.v = (short)location.y;
		if (!isJaguar && !isPUMA) {
			GrafPtr	port, dst;
			bool	portchanged;
			dst = GetWindowPort(toolwin.hwnd);
			portchanged = QDSwapPort(dst, &port);
			LocalToGlobal(&point);
			if (portchanged) QDSwapPort(port, NULL);
		}
		openpopup(point);
        soundmng_play();
    }

    (void)userData;
	(void)myHandler;
    return err;
}

static PixPatHandle	portpat;

static void setDragColor(GrafPtr window) {
    GrafPtr		port;
    RGBColor	color;
    bool		portchanged;
    
    portchanged = QDSwapPort(window, &port);
    GetThemeBrushAsColor(kThemeBrushDragHilite, 32, true, &color);
    portpat = NewPixPat();
    MakeRGBPat(portpat, &color);
    PenPixPat(portpat);
    PenSize(3, 3);
    if (portchanged) QDSwapPort(port, NULL);
}

static void toolwincreate(WindowRef hWnd) {

const SUBITEM	*p;
	UINT		i;
	ControlRef	sub;
const char		*cls;
	UInt32		style;
    
    setDragColor(GetWindowPort(hWnd));
    
    ControlButtonContentInfo	info;
    info.contentType = kControlContentPictHandle;
    info.u.picture = NULL;

    const RGBColor	col0 = {0x6000, 0x0000, 0x0000};
    const RGBColor	col1 = {0xffff, 0x0000, 0x0000};
    toolwin.access[0] = NewPixPat();
    toolwin.access[1] = NewPixPat();
    MakeRGBPat(toolwin.access[0], &col0);
    MakeRGBPat(toolwin.access[1], &col1);
    
    ControlFontStyleRec fontstyle;
    fontstyle.flags = kControlUseSizeMask;
    fontstyle.size = (SInt16)toolskin.fontsize;
    
    MenuRef	fddmenu[2];
    SInt16	menuid = 200;
    ControlButtonContentInfo	nopict;
    nopict.contentType = kControlContentTextOnly;
    EventTypeSpec	list[]={ 
            { kEventClassMenu, kEventMenuBeginTracking },
            { kEventClassMenu, kEventMenuEndTracking },
    };
    EventHandlerRef	ref;

	p = subitem;
	for (i=0; i<IDC_MAXITEMS; i++) {
		sub = NULL;
		cls = NULL;
		if ((p->width > 0) && (p->height > 0)) {
            Rect	bounds;
            SetRect(&bounds, p->posx, p->posy, p->posx+p->width, p->posy+p->height);
            switch(p->tctl) {
                case TCTL_STATIC:
                    cls = str_static;
                    style = 0;
                    CreatePictureControl(hWnd, &bounds, &info, true, &sub);
                    accdraw(sub, 0);
                    break;

                case TCTL_BUTTON:
                    cls = str_button;
                    style = 1;
                    CreateBevelButtonControl(hWnd, &bounds,	CFSTRj(p->text), 
                                                        kControlBevelButtonSmallBevel,
                                                        0 ,NULL, 0, 0, 0, &sub);
                    SetControlCommandID(sub, subcommand[i]);
                    SetControlFontStyle(sub, &fontstyle);
                    break;

                case TCTL_DDLIST:
                    cls = str_combobox;
                    style = 2 | 4;
                    fddmenu[menuid-200] = NewMenu(menuid, "\p ");
                    InsertMenu(fddmenu[menuid-200], 0);
                    CreateBevelButtonControl(hWnd, &bounds,	NULL, 
                                                        kControlBevelButtonSmallBevel,
                                                        kControlBehaviorPushbutton,
                                                        &nopict, menuid, 
                                                        kControlBehaviorSingleValueMenu, 
                                                        kControlBevelButtonMenuOnBottom, &sub);
                    SizeControl(sub, p->width-5, 16);
                    MoveControl(sub, p->posx+2, p->posy+2);
                    SetControlID(sub, &popup[menuid-200]);
                    InstallMenuEventHandler (fddmenu[menuid-200], NewEventHandlerUPP(cfControlproc), GetEventTypeCount(list), list, (void *)fddmenu[menuid-200], &ref);
                    DeleteMenu(menuid);
                    menuid++;
                    menuid = 200 + (menuid & 1);
                    SetControlCommandID(sub, subcommand[i]);
                    break;
            }
		}
		toolwin.sub[i] = sub;
		p++;
	}
	for (i=0; i<FDDLIST_DRV; i++) {
		sub = toolwin.sub[fddlist[i]];
		if (sub) {
			remakefddlist(sub, np2tool.fdd + i);
		}
	}
}


static void toolwindestroy(void) {
	UINT	i;
	ControlRef	sub;

	if (toolwin.hbmp) {
		for (i=0; i<IDC_MAXITEMS; i++) {
			sub = toolwin.sub[i];
			if (sub) {
				DisposeControl(sub);
			}
		}
		DisposePixPat(toolwin.access[0]);
		DisposePixPat(toolwin.access[1]);
        KillPicture(toolwin.hbmp);
        toolwin.hbmp = NULL;
        DisposePixPat(portpat);
	}
}


//-----

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    ControlRef	sub;
    int			i;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'rset':
                HandleMenuChoice(IDM_RESET);
                err=noErr;
                break;
                
            case 'pop1':
                sellist(0);
                err=noErr;
                break;
                                
            case 'opn1':
                HandleMenuChoice(IDM_FDD1OPEN);
                err=noErr;
                break;
                
            case 'ejt1':
                HandleMenuChoice(IDM_FDD1EJECT);
                err=noErr;
                break;
                
            case 'pop2':
                sellist(1);
                err=noErr;
                break;
                                
            case 'opn2':
                HandleMenuChoice(IDM_FDD2OPEN);
                err=noErr;
                break;
                
            case 'ejt2':
                HandleMenuChoice(IDM_FDD2EJECT);
                err=noErr;
                break;
                
            case 'exit':
                HandleMenuChoice(IDM_EXIT);
                err=noErr;
                break;
                
            default:
                break;
        }
    }
    else if (GetEventClass(event)==kEventClassWindow) {
        switch (GetEventKind(event)) {
            case kEventWindowClose:
#ifdef AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
			case kEventWindowDrawerClosed:
#endif
                toolwin_close();
                err=noErr;
                break;
                
            case kEventWindowDrawContent:
            case kEventWindowShown:
                for (i=0; i<IDC_MAXITEMS; i++) {
                    sub = toolwin.sub[i];
                    if (sub) {
                        Draw1Control(sub);
                    }
                }
                err=noErr;
                break;
                
            default:
                break;
        }
    }
    else if (GetEventClass(event)==kEventClassKeyboard) {
        UInt32	whatHappened = GetEventKind(event);
        UInt32  key;
        GetEventParameter (event, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &key);
        UInt32  modif;
        GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);
        switch (whatHappened)
        {
            case kEventRawKeyUp:
                mackbd_keyup(key);
                err = noErr;
                break;
            case kEventRawKeyRepeat:
                mackbd_keydown(key);
                err = noErr;
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
                err = noErr;
                break;
            default: 
                break;             
        }
    }

	(void)myHandler;
    return err;
}

static int getTypeFromFSSpec(char* fname, FSSpec* spec) {
    int			ftype;

    fsspec2path(spec, fname, MAX_PATH);
    ftype = file_getftype(fname);

    return (ftype);
}

OSErr setDropFile(FSSpec spec, int drv) {
    char	fname[MAX_PATH];
    
    switch (getTypeFromFSSpec(fname, &spec)) {
        case FTYPE_D88:
        case FTYPE_BETA:
            if (drv == -1) {
                return(-1);
            }
            diskdrv_setfdd(drv, fname, 0);
            toolwin_setfdd(drv, fname);
            break;
            
        case FTYPE_INI:
            strcpy(np2tool.skin, fname);
            skinchange(true);
            break;
        
        case FTYPE_THD:
        case FTYPE_HDI:
            diskdrv_sethdd(0, fname);
            break;
            
        default:
            return(-1);
    }
    return(noErr);
}

static OSErr getFSSpecFromDragItem(DragRef theDrag, HFSFlavor* aHFSFlavor) {
	UInt16		numItems;
	DragItemRef	ItemRef;
    FlavorType	Type;
    
    CountDragItems( theDrag, &numItems );
	if ( numItems != 1 ) {
		return( -1 );
    }
	GetDragItemReferenceNumber( theDrag, 1, &ItemRef );
    GetFlavorType( theDrag, ItemRef, 1, &Type );
	if ( Type != flavorTypeHFS ) {
        return( -1 );
    }
	Size dataSize = sizeof(HFSFlavor);
	GetFlavorData( theDrag, ItemRef, flavorTypeHFS, aHFSFlavor, &dataSize, 0 );
    return(noErr);
}

static SInt16 whichControl(DragRef theDrag, WindowRef theWindow) {
    SInt16		drv = -1;
    Point		pt;
    int			i;
    ControlRef	popupref[2];
    Rect		bounds[2];

    GetDragMouse(theDrag, &pt, NULL);
    GlobalToLocal(&pt);
    for (i=0;i<2;i++) {
        GetControlByID(theWindow, &popup[i], &popupref[i]);
        GetControlBounds(popupref[i], &bounds[i]);
        if (PtInRect(pt, &bounds[i])) {
            drv = i;
            break;
        }
    }
    return(drv);
}

static OSErr DragTracker (DragTrackingMessage message, WindowRef theWindow, void * handlerRefCon, DragRef theDrag) {
    SInt16		drv = -1;
	HFSFlavor	aHFSFlavor;
    RgnHandle	rgn = NULL;
    char		fname[MAX_PATH];
    ControlRef	targetControl;
    GrafPtr		port;
    bool		portchanged = false;
    static bool	hilite = false;
    Rect		bounds;
    ControlRef	back;
    
    if (message == kDragTrackingEnterHandler || message == kDragTrackingLeaveHandler) {
        hilite = false;
        return (noErr);
    }
    if (getFSSpecFromDragItem(theDrag, &aHFSFlavor) != noErr) {
        if (hilite) {
            GetControlByID(theWindow, &imageid, &back);
            Draw1Control(back);
            hilite = false;
        }
        return (-1);
    }
    if (message != kDragTrackingLeaveWindow) {
        switch (getTypeFromFSSpec(fname, &aHFSFlavor.fileSpec)) {
            case FTYPE_D88:
            case FTYPE_BETA:
                drv = whichControl(theDrag, theWindow);
                if (drv == -1) {
                    message = kDragTrackingLeaveWindow;
                }
                else {
                    GetControlByID(theWindow, &popup[drv], &targetControl);
                    GetControlBounds(targetControl, &bounds);
                }
                break;
            
            case FTYPE_INI:
            case FTYPE_THD:
            case FTYPE_HDI:
                GetWindowBounds(theWindow, kWindowContentRgn, &bounds);
                OffsetRect(&bounds, -bounds.left, -bounds.top);
                break;
            
            default:
                return(noErr);
        }
    }
    
    portchanged = QDSwapPort(GetWindowPort(theWindow), &port);
    if (message == kDragTrackingLeaveWindow && hilite) {
        GetControlByID(theWindow, &imageid, &back);
        Draw1Control(back);
        hilite = false;
    }
    else {
        rgn = NewRgn();
        if (rgn) {
            RectRgn(rgn, &bounds);
            FrameRgn(rgn);
            DisposeRgn(rgn);
        }
        hilite = true;
    }
    if (portchanged) QDSwapPort(port, NULL);

    return(noErr);
}
   
static pascal OSErr DragReceiver( WindowRef theWindow, void *handlerRefCon, DragRef theDrag )
{
    SInt16		drv = -1;
	HFSFlavor aHFSFlavor;
    ControlRef	back;

    GetControlByID(theWindow, &imageid, &back);
    Draw1Control(back);

    if (getFSSpecFromDragItem(theDrag, &aHFSFlavor) != noErr) {
        return (-1);
    }
    drv = whichControl(theDrag, theWindow);
	return( setDropFile(aHFSFlavor.fileSpec, drv) );
}

static WindowRef makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    WindowRef	win = NULL;

    if (isPUMA) {
        Rect	bounds;
        SetRect(&bounds, 0, 0, 100, 100);
        err = CreateNewWindow(kFloatingWindowClass, kWindowStandardHandlerAttribute, &bounds, &win);
    }
    else {
        err = CreateWindowFromNib(nibRef, CFSTR("ToolWindow"), &win);
    }
    if (err == noErr) {
        InstallStandardEventHandler(GetWindowEventTarget(win));
        EventTypeSpec	list[]={ 
            { kEventClassCommand, 	kEventCommandProcess },
            { kEventClassWindow,	kEventWindowClose }, 
            { kEventClassWindow,	kEventWindowShown }, 
            { kEventClassWindow,	kEventWindowDrawContent }, 
            { kEventClassKeyboard,	kEventRawKeyUp},
            { kEventClassKeyboard,	kEventRawKeyDown},
            { kEventClassKeyboard,	kEventRawKeyRepeat},
#ifdef AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
			{ kEventClassWindow, 	kEventWindowDrawerClosed },
#endif
        };
        EventHandlerRef	ref;
        InstallWindowEventHandler (win, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)win, &ref);

        dr = NewDragReceiveHandlerUPP((DragReceiveHandlerProcPtr)DragReceiver);
        tr = NewDragTrackingHandlerUPP((DragTrackingHandlerProcPtr)DragTracker);
        InstallReceiveHandler( dr, win, NULL);
        InstallTrackingHandler( tr, win, NULL);
    }
    return(win);
}

static WindowRef createToolWindow( void ) {
    OSStatus	err;
    IBNibRef	nibRef;
    WindowRef	win = NULL;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        win = makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    return(win);
}

static void createskinmenu(MenuRef ret) {

	UINT	cnt;
const char	*base;
	char	*p;
	UINT	i;
    char	longname[256];

	AppendMenuItemTextWithCFString(ret, CFCopyLocalizedString(CFSTR("Select Skin..."),"Slect Skin"), kMenuItemAttrIconDisabled, 0, 0);
	AppendMenu(ret, "\p-");

	base = np2tool.skin;
	AppendMenuItemTextWithCFString(ret, CFCopyLocalizedString(CFSTR("<Base Skin>"),"Base Skin"), kMenuItemAttrIconDisabled, 0, 0);
	if (base[0] == '\0') {
        CheckMenuItem(ret, BASENUMBER-1, true);
    }
	for (cnt=0; cnt<SKINMRU_MAX; cnt++) {
		p = np2tool.skinmru[cnt];
		if (p[0] == '\0') {
			break;
		}
        UInt32	attr = kMenuItemAttrIconDisabled;
        if (file_attr(p) != FILEATTR_ARCHIVE) {
            attr |= kMenuItemAttrDisabled;
        }
        ZeroMemory(longname, sizeof(longname));
		if (!getLongFileName(longname, p)) {
            strcpy(longname, file_getname(p));
        }
        AppendMenuItemTextWithCFString(ret, CFStringCreateWithCString(NULL, longname, kCFStringEncodingUTF8), attr, 0, 0);
	}
	for (i=0; i<cnt; i++) {
        if (!file_cmpname(base, np2tool.skinmru[i])) {
            CheckMenuItem(ret, i+BASENUMBER, true);
            break;
        }
	}
	return;
}

static void skinchange(bool remake) {

const char		*p;
	UINT		i;

    toolwin_close();
	p = np2tool.skin;
	if (p[0] && remake) {
		for (i=0; i<(SKINMRU_MAX - 1); i++) {
			if (!file_cmpname(p, np2tool.skinmru[i])) {
				break;
			}
		}
		while(i > 0) {
			CopyMemory(np2tool.skinmru[i], np2tool.skinmru[i-1],
												sizeof(np2tool.skinmru[0]));
			i--;
		}
		file_cpyname(np2tool.skinmru[0], p, sizeof(np2tool.skinmru[0]));
	}
	sysmng_update(SYS_UPDATEOSCFG);
    toolwin_open();
}


// ----
static void openpopup(Point location) {

	MenuRef	hMenu;
	short	sel;
    UInt16	selectclose;
    char	fname[MAX_PATH];

    hMenu = NewMenu(222, "\pSkin");
    InsertMenu(hMenu, -1);
	createskinmenu(hMenu);
	AppendMenu(hMenu, "\p-");
    AppendMenuItemTextWithCFString(hMenu, CFCopyLocalizedString(CFSTR("Close"),"ToolWin Close"), kMenuItemAttrIconDisabled, 0, 0);
    DeleteMenu(222);
    selectclose = CountMenuItems(hMenu);
    sel = LoWord(PopUpMenuSelect(hMenu, location.v, location.h, 0));
	DisposeMenu(hMenu);
    if (sel == selectclose) {
        toolwin_close();
    }
    else {
        switch (sel) {
            case 1:
                if(dialog_fileselect(fname, sizeof(fname), hWndMain, OPEN_INI)) {
                    if (file_getftype(fname)==FTYPE_INI) {
                        strcpy(np2tool.skin, fname);
                        skinchange(true);
                    }
                }
                break;
            case 3:
                if (np2tool.skin[0]) {
                    np2tool.skin[0] = '\0';
                    skinchange(false);
                }
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                if (file_cmpname(np2tool.skin, np2tool.skinmru[sel - BASENUMBER])) {
                    file_cpyname(np2tool.skin, np2tool.skinmru[sel - BASENUMBER], sizeof(np2tool.skin));
                    skinchange(false);
                }
                break;
            default:
                break;
        }
    }

}

static void makeskinimage(WindowRef win,PicHandle bmp, Rect* rect) {
    ControlButtonContentInfo	info;
    ControlRef					image;
    EventHandlerRef				ref;
    EventTypeSpec				list[]={ 
        { kEventClassControl, kEventControlContextualMenuClick },
    };
	
    info.contentType = kControlContentPictHandle;
    info.u.picture = bmp;
    CreatePictureControl(win, rect, &info, true, &image);
    SetControlID(image, &imageid);
    InstallControlEventHandler (image, NewEventHandlerUPP(cfControlproc), GetEventTypeCount(list), list, (void *)win, &ref);
}

static void opentoolwin_puma(WindowRef win) {
	if (np2tool.posy < 35) np2tool.posy = 35;
	if (np2tool.posx < 5 ) np2tool.posx = 5;
	MoveWindow(win, np2tool.posx, np2tool.posy, true);
	ShowWindow(win);
}

void toolwin_open(void) {

	PicHandle	hbmp;
	WindowRef	hWnd = NULL;
    Rect		bounds;

	if (toolwin.hwnd) {
        toolwin_close();
		return;
	}
    
    checkOSVersion();
	ZeroMemory(&toolwin, sizeof(toolwin));

	hbmp = skinload(np2tool.skin, &bounds);
	if (hbmp == NULL) {
		goto twope_err1;
	}
	toolwin.hbmp = hbmp;
    hWnd = createToolWindow();
	toolwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto twope_err2;
	}
    

	SetWindowBounds(hWnd, kWindowContentRgn, &bounds);
	if (isPUMA) {
        toolwincreate(hWnd);
		makeskinimage(hWnd, hbmp, &bounds);
    }
	else {
		makeskinimage(hWnd, hbmp, &bounds);
        toolwincreate(hWnd);
    }
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
	opentoolwin_puma(hWnd);
#else
    if (isPUMA) {
		opentoolwin_puma(hWnd);
    }
    else {
        Rect	mainbounds;
        int		width;
        SetDrawerParent(hWnd, hWndMain);
        GetWindowBounds(hWndMain, kWindowContentRgn, &mainbounds);
        width = (mainbounds.right-mainbounds.left)-(bounds.right-bounds.left);
        if (width/2-11<0) {
            toolwin_close();
            return;
        }
        SetDrawerOffsets(hWnd, width/2-11, width/2-11);
        SetDrawerPreferredEdge(hWnd, kWindowEdgeTop);
		const HISize  size = {bounds.right-bounds.left, bounds.bottom-bounds.top};
		SetWindowResizeLimits(hWnd, &size, &size);
        OpenDrawer(hWnd, kWindowEdgeDefault, 1);
    }
#endif

    np2oscfg.toolwin = 1;
	return;

twope_err2:
	KillPicture(hbmp);

twope_err1:
	sysmng_update(SYS_UPDATEOSCFG);
	return;
}

void toolwin_close(void) {

    if (toolwin.hwnd) {
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
        HideWindow(toolwin.hwnd);
#else
        if (isPUMA) {
            HideWindow(toolwin.hwnd);
        }
        else {
            CloseDrawer(toolwin.hwnd, 0);
        }
#endif
        RemoveReceiveHandler(dr, toolwin.hwnd);
        RemoveTrackingHandler(tr, toolwin.hwnd);
        toolwindestroy();
        DisposeWindow(toolwin.hwnd);
        toolwin.hwnd = NULL;
        np2oscfg.toolwin = 0;
    }
}


void toolwin_setfdd(BYTE drv, const char *name) {

	TOOLFDD	*fdd;
	char	*q;
	char	*p;
	UINT	i;
	ControlRef	sub;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	fdd = np2tool.fdd + drv;
	if ((name == NULL) || (name[0] == '\0')) {
		fdd->insert = 0;
	}
	else {
		fdd->insert = 1;
		q = fdd->name[0];
		for (i=0; i<(FDDLIST_MAX - 1); i++) {
			if (!file_cmpname(q, name)) {
				break;
			}
			q += sizeof(fdd->name[0]);
		}
		p = q - sizeof(fdd->name[0]);
		while(i > 0) {
			i--;
			CopyMemory(q, p, sizeof(fdd->name[0]));
			p -= sizeof(fdd->name[0]);
			q -= sizeof(fdd->name[0]);
		}
		file_cpyname(fdd->name[0], name, sizeof(fdd->name[0]));
	}
	sysmng_update(SYS_UPDATEOSCFG);
	if (toolwin.hwnd != NULL) {
		sub = toolwin.sub[fddlist[drv]];
		if (sub) {
			remakefddlist(sub, fdd);
		}
	}
}

static void setdiskacc(UINT num, BYTE count) {

const 	DISKACC*		acc;
        ControlRef		sub;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (num < (sizeof(diskacc)/sizeof(DISKACC))) {
		acc = diskacc + num;
		sub = NULL;
		if (*(acc->counter) == 0) {
			sub = toolwin.sub[acc->idc];
		}
		*(acc->counter) = count;
		if (sub) {
            accdraw(sub, *(acc->counter));
		}
	}
}

void toolwin_fddaccess(BYTE drv) {

	if (drv < 2) {
		setdiskacc(drv, 20);
	}
}

void toolwin_hddaccess(BYTE drv) {

	setdiskacc(2, 10);
}

void toolwin_draw(BYTE frame) {

const DISKACC	*acc;
const DISKACC	*accterm;
	BYTE		counter;
	ControlRef		sub;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (!frame) {
		frame = 1;
	}
	acc = diskacc;
	accterm = acc + (sizeof(diskacc)/sizeof(DISKACC));
	while(acc < accterm) {
		counter = *acc->counter;
		if (counter) {
			if (counter <= frame) {
				*(acc->counter) = 0;
				sub = toolwin.sub[acc->idc];
				if (sub) {
                    accdraw(sub, *(acc->counter));
				}
			}
			else {
				*(acc->counter) -= frame;
			}
		}
		acc++;
	}
}


// ----

static const char ini_title[] = "NP2 tool";
#if !defined(SUPPORT_PC9821)
static const char inifile[] = "np2.cfg";			// same file name..
#else
static const char inifile[] = "np21.cfg";
#endif

static const INITBL iniitem[] = {
	{"WindposX", INITYPE_SINT32,	&np2tool.posx,			0},
	{"WindposY", INITYPE_SINT32,	&np2tool.posy,			0},
	{"SkinFile", INITYPE_STR,		np2tool.skin,			MAX_PATH},
	{"SkinMRU0", INITYPE_STR,		np2tool.skinmru[0],		MAX_PATH},
	{"SkinMRU1", INITYPE_STR,		np2tool.skinmru[1],		MAX_PATH},
	{"SkinMRU2", INITYPE_STR,		np2tool.skinmru[2],		MAX_PATH},
	{"SkinMRU3", INITYPE_STR,		np2tool.skinmru[3],		MAX_PATH},
	{"FD1NAME0", INITYPE_STR,		np2tool.fdd[0].name[0],	MAX_PATH},
	{"FD1NAME1", INITYPE_STR,		np2tool.fdd[0].name[1],	MAX_PATH},
	{"FD1NAME2", INITYPE_STR,		np2tool.fdd[0].name[2],	MAX_PATH},
	{"FD1NAME3", INITYPE_STR,		np2tool.fdd[0].name[3],	MAX_PATH},
	{"FD1NAME4", INITYPE_STR,		np2tool.fdd[0].name[4],	MAX_PATH},
	{"FD1NAME5", INITYPE_STR,		np2tool.fdd[0].name[5],	MAX_PATH},
	{"FD1NAME6", INITYPE_STR,		np2tool.fdd[0].name[6],	MAX_PATH},
	{"FD1NAME7", INITYPE_STR,		np2tool.fdd[0].name[7],	MAX_PATH},
	{"FD2NAME0", INITYPE_STR,		np2tool.fdd[1].name[0],	MAX_PATH},
	{"FD2NAME1", INITYPE_STR,		np2tool.fdd[1].name[1],	MAX_PATH},
	{"FD2NAME2", INITYPE_STR,		np2tool.fdd[1].name[2],	MAX_PATH},
	{"FD2NAME3", INITYPE_STR,		np2tool.fdd[1].name[3],	MAX_PATH},
	{"FD2NAME4", INITYPE_STR,		np2tool.fdd[1].name[4],	MAX_PATH},
	{"FD2NAME5", INITYPE_STR,		np2tool.fdd[1].name[5],	MAX_PATH},
	{"FD2NAME6", INITYPE_STR,		np2tool.fdd[1].name[6],	MAX_PATH},
	{"FD2NAME7", INITYPE_STR,		np2tool.fdd[1].name[7],	MAX_PATH}};


void toolwin_readini(void) {

	char	path[MAX_PATH];

	ZeroMemory(&np2tool, sizeof(np2tool));
	np2tool.posx = 0;
	np2tool.posy = 0;
	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_read(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}

void toolwin_writeini(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_write(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL), FALSE);
}

