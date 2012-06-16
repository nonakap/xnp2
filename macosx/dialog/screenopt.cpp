/*
 *  screenopt.cpp
 *  np2
 *
 *  Created by tk800 on Fri Oct 24 2003.
 *
 */

#include	"compiler.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"dialogutils.h"
#include	"screenopt.h"

#define	getControlValue(a,b)		GetControl32BitValue(getControlRefByID(a,b,screenWin))
#define	setControlValue(a,b,c)		SetControl32BitValue(getControlRefByID(a,b,screenWin),c)

#define	kMaxNumTabs 3
enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};

static WindowRef	screenWin;
static UInt16 lastPaneSelected = 1;	// static, to keep track of it long term (in a more complex application
                                        // you might store this in a data structure in the window refCon)                                            


static void setReverseStatus(void) {
    if (getControlValue('LCDy', 0)) {
        ActivateControl(getControlRefByID('rvrs', 0, screenWin));
    }
    else {
        DeactivateControl(getControlRefByID('rvrs', 0, screenWin));
    }
}

static void initScreenWindow(void) {
    setControlValue('LCDy', 0, np2cfg.LCD_MODE & 1);
    setReverseStatus();
    setControlValue('rvrs', 0, np2cfg.LCD_MODE & 2?1:0);
    setControlValue('uskr', 0, np2cfg.skipline);
    setControlValue('sl/f', 0, np2cfg.skiplight);
      
    setControlValue('GDCl', 0, np2cfg.uPD72020 + 1);
    setControlValue('GrCh', 0, (np2cfg.grcg & 3) + 1);
    setControlValue('16cl', 0, np2cfg.color16);
    
    setControlValue('sl/f', 1, np2cfg.wait[0]);
    setControlValue('sl/f', 2, np2cfg.wait[2]);
    setControlValue('sl/f', 3, np2cfg.wait[4]);
    setControlValue('sl/f', 4, np2cfg.realpal);
	
#if defined(SUPPORT_PC9821)
	DisableControl(getControlRefByID('Gral', 0, screenWin));
	DisableControl(getControlRefByID('16cl', 0, screenWin));
#endif

}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    SINT32	val;
	UInt8	value[6];
	UInt8	b;
	UInt8	update;
	int		renewal;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'LCDy':
                setReverseStatus();
                break;

            case kHICommandOK:
                renewal = 0;
                val=getControlValue('uskr', 0);
				if (np2cfg.skipline != val) {
					np2cfg.skipline = val;
					renewal = 1;
				}
                val=getControlValue('sl/f', 0);
                if (val != np2cfg.skiplight);
				if (renewal) {
					pal_makeskiptable();
				}
                val=getControlValue('LCDy', 0) | getControlValue('rvrs', 0) << 1;
				if (np2cfg.LCD_MODE != val) {
					np2cfg.LCD_MODE = val;
					pal_makelcdpal();
					renewal = 1;
				}
				if (renewal) {
					sysmng_update(SYS_UPDATECFG);
					scrndraw_redraw();
				}

                update = 0;
                val=getControlValue('GDCl', 0)-1;
				if (np2cfg.uPD72020 != val) {
					np2cfg.uPD72020 = val;
					update |= SYS_UPDATECFG;
					gdc_restorekacmode();
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
                val=getControlValue('GrCh', 0)-1;
				if (np2cfg.grcg != val) {
					np2cfg.grcg = val;
					update |= SYS_UPDATECFG;
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
                val=getControlValue('16cl', 0);
				if (np2cfg.color16 != val) {
					np2cfg.color16 = val;
					update |= SYS_UPDATECFG;
				}
    
                value[0]=getControlValue('sl/f', 1);
				if (value[0]) {
					value[1] = 1;
				}
                value[2]=getControlValue('sl/f', 2);
				if (value[0]) {
					value[3] = 1;
				}
                value[4]=getControlValue('sl/f', 3);
				if (value[0]) {
					value[5] = 1;
				}
				for (b=0; b<6; b++) {
					if (np2cfg.wait[b] != value[b]) {
						np2cfg.wait[b] = value[b];
						update |= SYS_UPDATECFG;
					}
				}
                val=getControlValue('sl/f', 4);
                if (val != np2cfg.realpal) {
                    np2cfg.realpal = val;
					update |= SYS_UPDATECFG;
                }
                
                sysmng_update(update);
                endLoop(screenWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop(screenWin);
                err=noErr;
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static pascal OSStatus sliderEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    short		base = 0;
    ControlID	conID;
    ControlRef	theControl = (ControlRef)inUserData;
    if (GetControlID(theControl, &conID) == noErr) {
        if (conID.signature == 'sl/f' && conID.id == 4) {
            base = 32;
        }
    }
    return( changeSlider(theControl, screenWin, base) );
}

static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    WindowRef theWindow = (WindowRef)inUserData;  // get the windowRef, passed around as userData    
    short ret;
    ret = changeTab(theWindow, lastPaneSelected);
    if (ret) {
        lastPaneSelected = ret;
    }
    return( eventNotHandledErr );
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    short		i;
    
    err = CreateWindowFromNib(nibRef, CFSTR("ScreenDialog"), &screenWin);
    if (err == noErr) {
        initScreenWindow();
        SetInitialTabState(screenWin, lastPaneSelected, kMaxNumTabs);
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,screenWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, screenWin, NULL );
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },};

        EventTypeSpec	sliderControlEvents[] ={
            { kEventClassControl, kEventControlDraw },
            { kEventClassControl, kEventControlValueFieldChanged }
        };
        for (i=0;i<5;i++) {
			ControlRef  target;
            target = getControlRefByID('sl/f', i, screenWin);
            InstallControlEventHandler(target, sliderEventHandlerProc, GetEventTypeCount(sliderControlEvents), sliderControlEvents, (void *)target, NULL );
        }

        EventHandlerRef	ref;
        InstallWindowEventHandler (screenWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)screenWin, &ref);
        
		ShowSheetWindow(screenWin, hWndMain);
        
        err=RunAppModalLoopForWindow(screenWin);
    }
    return;
}

void initScreenOpt( void ) {
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    else {
         return;
    }
}
                             