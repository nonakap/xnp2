/*
 *  macclnd.cpp
 *  np2
 *
 *  Created by tk800 on Sat Jul 10 2004.
 *
 */

#include	"compiler.h"
#include	"dialogutils.h"
#include	"macclnd.h"
#include	"np2.h"
#include	"parts.h"
#include	"timemng.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"calendar.h"

#define		clndControl(a)		getControlRefByID('date',a,clndWin)
#define		getCalendarMode()	GetControlValue(getControlRefByID(0,'clnd',clndWin))

static	WindowRef	clndWin;

static	BYTE	cbuf[8];

static void setCalendar(void) {
	LongDateRec np2calendar;
	UINT16		year;
	ControlRef  cref0, cref1;

	year = 1900 + AdjustBeforeDivision(cbuf[0]);
	if (year < 1980) {
		year += 100;
	}
	np2calendar.od.oldDate.year = year;
	np2calendar.od.oldDate.dayOfWeek = ((cbuf[1]) & 0x0f)+1;
	np2calendar.od.oldDate.month = ((cbuf[1]) >> 4);
	np2calendar.od.oldDate.day = AdjustBeforeDivision(cbuf[2]);
	np2calendar.od.oldDate.hour = AdjustBeforeDivision(cbuf[3]);
	np2calendar.od.oldDate.minute = AdjustBeforeDivision(cbuf[4]);
	np2calendar.od.oldDate.second = AdjustBeforeDivision(cbuf[5]);

	cref0 = clndControl(0);
	SetControlData(cref0, kControlNoPart, kControlClockLongDateTag, sizeof(LongDateRec), &np2calendar);
	Draw1Control(cref0);
	cref1 = clndControl(1);
	SetControlData(cref1, kControlNoPart, kControlClockLongDateTag, sizeof(LongDateRec), &np2calendar);
	Draw1Control(cref1);
}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    BYTE		b;
	LongDateRec buffer0, buffer1;
	ControlRef  setnow;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'clnd':
				setnow = getControlRefByID('setn', 0, clndWin);
				if (IsControlEnabled(setnow)) {
					DisableControl(clndControl(0));
					DisableControl(clndControl(1));
					DisableControl(setnow);
				}
				else {
					EnableControl(clndControl(0));
					EnableControl(clndControl(1));
					EnableControl(setnow);
				}
                break;

			case 'setn':
				calendar_getreal(cbuf);
				setCalendar();
				break;

            case kHICommandOK:
				if (getCalendarMode() == 1) {
					b = 0;
				}
				else {
					b = 1;
				}
				if (np2cfg.calendar != b) {
					np2cfg.calendar = b;
					sysmng_update(SYS_UPDATECFG);
				}
				GetControlData(clndControl(0), kControlNoPart, kControlClockLongDateTag, sizeof(LongDateRec), &buffer0, NULL);
				cbuf[0] = AdjustAfterMultiply((BYTE)((buffer0.od.oldDate.year) % 100));
				cbuf[1] = (BYTE)(((buffer0.od.oldDate.month) << 4) + (buffer0.od.oldDate.dayOfWeek)-1);
				cbuf[2] = AdjustAfterMultiply((BYTE)buffer0.od.oldDate.day);
				GetControlData(clndControl(1), kControlNoPart, kControlClockLongDateTag, sizeof(LongDateRec), &buffer1, NULL);
				cbuf[3] = AdjustAfterMultiply((BYTE)buffer1.od.oldDate.hour);
				cbuf[4] = AdjustAfterMultiply((BYTE)buffer1.od.oldDate.minute);
				cbuf[5] = AdjustAfterMultiply((BYTE)buffer1.od.oldDate.second);
				calendar_set(cbuf);
                endLoop(clndWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop(clndWin);
                err=noErr;
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static void initClndWindow(void) {
	calendar_getvir(cbuf);
	setCalendar();
	if (!np2cfg.calendar) {
		SetControl32BitValue(getControlRefByID('clnd', 0, clndWin), 2);
		EnableControl(clndControl(0));
		EnableControl(clndControl(1));
		EnableControl(getControlRefByID('setn', 0, clndWin));
	}
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    
    err = CreateWindowFromNib(nibRef, CFSTR("CalendarDialog"), &clndWin);
    if (err == noErr) {
        initClndWindow();
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },
        { kEventClassWindow, kEventWindowActivated } };
        EventHandlerRef	ref;
        
        InstallWindowEventHandler (clndWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)clndWin, &ref);
        ShowSheetWindow(clndWin, hWndMain);
        
        err=RunAppModalLoopForWindow(clndWin);
    }
    return;
}

void initClnd( void ) {
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