/*
 *  aboutdlg.cpp
 *  np2x
 *
 *  Created by tk800 on Tue Nov 11 2003.
 *
 */

#include	"compiler.h"
#include	"dialogutils.h"
#include	"np2opening.h"
#include	"np2ver.h"
#include	"np2info.h"
#include	"aboutdlg.h"
#include	<QuickTime/QuickTime.h>

static	WindowRef	aboutWin;
static	PicHandle	pict;
const	EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },
                                 { kEventClassWindow, kEventWindowShowing },
                                };
static const char np2infostr[] = 									\
						"CPU: %CPU% %CLOCK%\r"						\
						"MEM: %MEM1%\r"							\
						"GDC: %GDC%\r"								\
						"TEXT: %TEXT%\r"							\
						"GRPH: %GRPH%\r"							\
						"SOUND: %EXSND%\r"							\
						"\r"										\
						"BIOS: %BIOS%\r"							\
						"RHYTHM: %RHYTHM%\r"
						;

static void closeAboutDialog(void) {
    QuitAppModalLoopForWindow(aboutWin);
    HideWindow(aboutWin);
    DisposeWindow(aboutWin);
    KillPicture(pict);
}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    ControlRef	image, version, more;
    Rect		bounds, winrect;
	char		infostr[1024];

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'detl':
                GetWindowBounds(aboutWin, kWindowContentRgn, &winrect);
                more = getControlRefByID('more',0,aboutWin);
                if (winrect.bottom - winrect.top == 441) {
                    SetControl32BitValue(getControlRefByID('detl',0,aboutWin),0);
                    HideControl(more);
                    SizeWindow(aboutWin, 292, 230, true);
                }
                else {
                    np2info(infostr, np2infostr, sizeof(infostr), NULL);
                    SetControlData(more,kControlNoPart,kControlEditTextTextTag,sizeof(infostr), infostr);
                    SetControl32BitValue(getControlRefByID('detl',0,aboutWin),3);
                    SizeWindow(aboutWin, 292, 441, true);
                    ShowControl(more);
                }
                err=noErr;
                break;

            case kHICommandOK:
                closeAboutDialog();
                err=noErr;
                break;
                
            default:
                break;
        }
    }
    else if (GetEventClass(event)==kEventClassWindow) {
        switch (GetEventKind(event))
        {
            case kEventWindowShowing:
#if defined(SUPPORT_PC9821)
				SetWindowTitleWithCFString(aboutWin, CFCopyLocalizedString(CFSTR("AboutTitle"), "about 21x"));
#endif
                image = getControlRefByID('logo', 0, aboutWin);
                pict = getBMPfromResource("np2logo.tiff", &bounds);
                SetControlData(image, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);
				version = getControlRefByID('vers', 0, aboutWin);
                SetControlData(version, kControlNoPart, kControlStaticTextTextTag, sizeof(NP2VER_CORE), NP2VER_CORE);
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

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    EventHandlerRef	ref;
    
    err = CreateWindowFromNib(nibRef, CFSTR("AboutDialog"), &aboutWin);
    if (err == noErr) {
        InstallWindowEventHandler (aboutWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)aboutWin, &ref);
        ShowWindow(aboutWin);
        RunAppModalLoopForWindow(aboutWin);
    }
    return;
}

void AboutDialogProc( void ) {
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

