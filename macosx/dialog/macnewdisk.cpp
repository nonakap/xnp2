#include	"compiler.h"

#include	"resource.h"
#include	"np2.h"
#include	"newdisk.h"
#include	"dialog.h"
#include	"dialogutils.h"
#include	"macnewdisk.h"

static	WindowRef	diskWin;
static	SInt32	targetDisk, media, hdsize, hddtype;
static	char	disklabel[256];
enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};
#define	kMaxNumTabs 4
static UInt16		lastPaneSelected = 1;
#define	getControlValue(a,b)		GetControl32BitValue(getControlRefByID(a,b,diskWin))

const int defaultsize[5] = {20, 41, 65, 80, 128};
const EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
const EventTypeSpec	hicommandEvents[]={ { kEventClassCommand, kEventCommandProcess },};


static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
	SINT32		data;
    char		outstr[16];

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'hds2':
                data = getControlValue('hdsz', 3)-1;
                sprintf(outstr, "%d", defaultsize[data]);
                SetControlData(getControlRefByID('hdsz',2,diskWin),kControlNoPart,kControlStaticTextTextTag,strlen(outstr),outstr);
                Draw1Control(getControlRefByID('hdsz',2,diskWin));
                break;
				
            case 'hds4':
                data = getControlValue('hdsz', 5)-1;
                sprintf(outstr, "%d", defaultsize[data]);
                SetControlData(getControlRefByID('hdsz',4,diskWin),kControlNoPart,kControlStaticTextTextTag,strlen(outstr),outstr);
                Draw1Control(getControlRefByID('hdsz',4,diskWin));
                break;

            case kHICommandOK:
                targetDisk = getControlValue(kTabMasterSig, kTabMasterID);
                getFieldText(getControlRefByID('fdlb', 0, diskWin), disklabel);
                media = getControlValue('fdty', 0);
				if (targetDisk==2 || targetDisk==4) {
					data = getFieldValue(getControlRefByID('hdsz', targetDisk, diskWin));
					if (data < 0) {
						data = 0;
					}
					else if (data > 512) {
						data = 512;
					}
					hdsize = data;
				}
				else if (targetDisk==3) {
					hdsize = getControlValue('hdsz', 20)-1;
				}
                QuitAppModalLoopForWindow(diskWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                QuitAppModalLoopForWindow(diskWin);
                err=noErr;
                break;
                
            default:
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    WindowRef	theWindow = (WindowRef)inUserData;  // get the windowRef, passed around as userData    
    short		ret;
    ControlRef	focus = NULL;

    ret = changeTab(theWindow, lastPaneSelected);
    if (ret) {
        if (ret == 1) {
            focus = getControlRefByID('fdlb', 0, theWindow);
        }
        else if (ret == 2 || ret == 4) {
            focus = getControlRefByID('hdsz', ret, theWindow);
        }
        SetKeyboardFocus(theWindow, focus, kControlFocusNextPart);
        lastPaneSelected = ret;
    }
    return( eventNotHandledErr );
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    EventHandlerRef	ref;
    
    err = CreateWindowFromNib(nibRef, CFSTR("NewDiskDialog"), &diskWin);
    if (err == noErr) {
        SetInitialTabState(diskWin, lastPaneSelected, kMaxNumTabs);
        
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,diskWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, diskWin, NULL );
        InstallWindowEventHandler (diskWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(hicommandEvents), hicommandEvents, (void *)diskWin, &ref);

        ShowWindow(diskWin);
        RunAppModalLoopForWindow(diskWin);
    }
    return;
}

static SInt32 initNewDisk( void ) {
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    return(err);
}

// Ç∆ÇËÇ†Ç¶Ç∏ÉÇÅ[É_ÉãÇ≈
void newdisk(void) {

    FSSpec	fss;
    char	fname[MAX_PATH];

    initNewDisk();
    
	if (targetDisk == 1) {
        if (dialog_filewriteselect('.D88', "Newdisk.d88", &fss, diskWin)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_fdd(fname, media << 4, disklabel);
        }
	}
    else if (targetDisk == 2) {
        if (dialog_filewriteselect('.THD', "Newdisk.thd", &fss, diskWin)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_thd(fname, hdsize);
        }
    }
    else if (targetDisk == 3) {
        if (dialog_filewriteselect('.HDI', "Newdisk.hdi", &fss, diskWin)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_hdi(fname, hddtype);
        }
    }
    else if (targetDisk == 4) {
        if (dialog_filewriteselect('.HDD', "Newdisk.hdd", &fss, diskWin)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_vhd(fname, hdsize);
        }
    }
    HideWindow(diskWin);
    DisposeWindow(diskWin);
}
