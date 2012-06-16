/*
 *  configure.cpp
 *  from Neko Project IIx 0.3
 *
 *  Created by tk800 on Mon Sep 23 2002.
 *
 */

#include	"compiler.h"
#include	"strres.h"
#include	"np2.h"
#include	"pccore.h"
#include	"ini.h"
#include	"sysmng.h"
#include	"configure.h"
#include	"dialogutils.h"

        int			sound_renewals = 0;
static	WindowRef	configWin;

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

#define getSelectedValue(a,b)	GetControlValue(getControlRefByID(a,b,configWin))

static int getMultiple(void) {
    int multi;
    switch (getSelectedValue('Mltp', 5))
    {
        case 1:
            multi=1;
            break;
        case 2:
            multi=2;
            break;
        case 3:
            multi=4;
            break;
        case 4:
            multi=5;
            break;
        case 5:
            multi=6;
            break;
        case 6:
            multi=8;
            break;
        case 7:
            multi=10;
            break;
        case 8:
            multi=12;
            break;
        case 9:
            multi=16;
            break;
        case 10:
            multi=20;
            break;
        default:
            multi=4;
            break;
    }
    return multi;
}

static void setCPUClock(void) {
    unsigned int	base, clock;
    ControlRef	conRef;
    char	outstr[1024+64+2];
    int	length;

    if (getSelectedValue('CPU ', 3)==1) {
        base=24576;
    }
    else {
        base=19968;
    }
    clock = base * getMultiple();
    conRef=getControlRefByID('CKtx', 6, configWin);
    
    sprintf(outstr, " %2u.%04u", clock / 10000, clock % 10000);
    length=strlen(outstr);
    SetControlData(conRef,kControlNoPart,kControlStaticTextTextTag,length,outstr);
}

static void initConfigWindow(void) {
        SInt16	i;
		SInt32  val;
        Str255	title;
    
        if (np2cfg.baseclock >= AVE(PCBASECLOCK25, PCBASECLOCK20)) {
            i=1;
        }
        else {
            i=2;
        }
        SetControlValue(getControlRefByID('CPU ', 3, configWin), i);

        switch (np2cfg.multiple)
        {
            case 1:
                i=1;
                break;
            case 2:
                i=2;
                break;
            case 4:
                i=3;
                break;
            case 5:
                i=4;
                break;
            case 6:
                i=5;
                break;
            case 8:
                i=6;
                break;
            case 10:
                i=7;
                break;
            case 12:
                i=8;
                break;
            case 16:
                i=9;
                break;
            case 20:
                i=10;
                break;
            default:
                i=4;
                break;
        }
        SetControlValue(getControlRefByID('Mltp', 5, configWin), i);
        setCPUClock();
		
		if (!milstr_cmp(np2cfg.model, str_VM)) {
			val = 1;
		}
		else if (!milstr_cmp(np2cfg.model, str_EPSON)) {
			val = 3;
		}
		else {
			val = 2;
		}
        SetControlValue(getControlRefByID('Arch', 0, configWin), val);

        NumToString(np2cfg.delayms, title);
        if (np2cfg.samplingrate < AVE(11025, 22050)) {
            i=1;
        }
        else if (np2cfg.samplingrate < AVE(22050, 44100)) {
            i=2;
        }
        else {
            i=3;
        }
        SetControlValue(getControlRefByID('Rate', 8, configWin), i);
        NumToString(np2cfg.delayms, title);
        SetControlData(getControlRefByID('Bufr', 7, configWin), kControlNoPart, kControlStaticTextTextTag, *title, title+1);

        SetControlValue(getControlRefByID('cnfm', 0, configWin), np2oscfg.comfirm);
        SetControlValue(getControlRefByID('rsum', 0, configWin), np2oscfg.resume);
}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    UINT32	dval;
    UINT16	wval;
	UINT	update;
const char	*str;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'base':
                setCPUClock();
                break;

            case 'mult':
                setCPUClock();
                break;

            case kHICommandOK:
                update = 0;
                if (getSelectedValue('CPU ', 3)!=1) {
                    dval=PCBASECLOCK20;
                }
                else {
                    dval=PCBASECLOCK25;
                }
                if (dval != np2cfg.baseclock) {
                    np2cfg.baseclock = dval;
                    update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
                }
                dval=getMultiple();
                if (dval != np2cfg.multiple) {
                    np2cfg.multiple = dval;
                    update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
                }

				dval=getSelectedValue('Arch', 0);
				if (dval == 1) {
					str = str_VM;
				}
				else if (dval == 3) {
					str = str_EPSON;
				}
				else {
					str = str_VX;
				}
				if (milstr_cmp(np2cfg.model, str)) {
					milstr_ncpy(np2cfg.model, str, sizeof(np2cfg.model));
					update |= SYS_UPDATECFG;
				}

                dval=getSelectedValue('Rate', 8);
                if (dval==1) {
                    wval = 11025;
                }
                else if (dval==2) {
                    wval = 22050;
                }
                else {
                    wval = 44100;
                }
                if (wval != np2cfg.samplingrate) {
                    np2cfg.samplingrate = wval;
                    update |= SYS_UPDATECFG | SYS_UPDATERATE;
                    soundrenewal = 1;
                }

                dval=getFieldValue(getControlRefByID('Bufr', 7, configWin));
                if (dval < 100) {
                    dval = 100;
                }
                else if (dval > 1000) {
                    dval = 1000;
                }
                if (dval!= np2cfg.delayms) {
                    soundrenewal = 1;
                    np2cfg.delayms = dval;
                    update |= SYS_UPDATECFG | SYS_UPDATESBUF;
                }
                dval=getSelectedValue('cnfm', 0);
                if (dval != np2oscfg.comfirm) {
                    np2oscfg.comfirm = dval;
                    update |= SYS_UPDATEOSCFG;
                }
                dval=getSelectedValue('rsum', 0);
                if (dval != np2oscfg.resume) {
                    np2oscfg.resume = dval;
                    update |= SYS_UPDATEOSCFG;
                }

                sysmng_update(update);
                endLoop(configWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop(configWin);
                err=noErr;
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}


static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    
    err = CreateWindowFromNib(nibRef, CFSTR("ConfigDialog"), &configWin);
    if (err == noErr) {
        initConfigWindow();
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },
        { kEventClassWindow, kEventWindowActivated } };
        EventHandlerRef	ref;
        
        InstallWindowEventHandler (configWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)configWin, &ref);
        ShowSheetWindow(configWin, hWndMain);
        
        err=RunAppModalLoopForWindow(configWin);
    }
    return;
}

void initConfig( void ) {
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



                             