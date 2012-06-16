/*
 *  soundopt.cpp
 *  np2
 *
 *  Created by tk800 on Sun Oct 26 2003.
 *
 */


#include	"compiler.h"
#include	"np2.h"
#include	"pccore.h"
#include	"sysmng.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"dialogutils.h"
#include	"hid.h"
#include	"soundopt.h"
#include	"dipswbmp.h"

#define	getControlValue(a,b)		GetControl32BitValue(getControlRefByID(a,b,soundWin))
#define	setControlValue(a,b,c)		SetControl32BitValue(getControlRefByID(a,b,soundWin),c)
#define getMenuValue				(getControlValue(cmd.commandID,0)-1)

#define	kMaxNumTabs 6
enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};

static OSStatus setupJoyConfig (OSType type);
static void initJoyPad(void);

static IBNibRef		nibRef;
static WindowRef	soundWin;
static WindowRef	joyWin;
static UInt16		lastPaneSelected = 1;	// static, to keep track of it long term (in a more complex application
                                        // you might store this in a data structure in the window refCon)                                            

static	BYTE			snd26 = 0;
static	BYTE			snd86 = 0;
static	BYTE			spb = 0;
static	BYTE			spbvrc = 0;

BYTE*	cfg[5] = {&np2cfg.vol_fm, &np2cfg.vol_ssg, &np2cfg.vol_adpcm, &np2cfg.vol_pcm, &np2cfg.vol_rhythm};

//-----init
static void initMixer(BYTE *data) {
    short j;
    for (j=0;j<5;j++) {
        setControlValue('vMix', j, *(data+j));
    }
}
static void init14(BYTE *data) {
    short i;
    for (i=0;i<6;i++) {
        setControlValue('vMix', i+10, *(data+i));
    }
}

static void set26s(BYTE chip, OSType io, OSType inturrupt, OSType rom) {
    BYTE para;
    static short paranum[4] = {0, 3, 1, 2};
    setControlValue(io, 0, ((chip >> 4) & 1)+1);
    setControlValue(inturrupt, 0, paranum[(chip >> 6) & 3]+1);
    para = chip & 7;
    if (para > 4) {
        para = 4;
    }
    setControlValue(rom, 0, para+1);
}

static void set86s(void) {
    setControlValue('86io', 0, ((~snd86) & 1)+1);
    static short paranum[4] = {0, 1, 3, 2};
    setControlValue('86in', 0, paranum[(snd86 >> 2) & 3]+1);
    setControlValue('86id', 0, (((~snd86) >> 5) & 7)+1);
    setControlValue('86it', 0, (snd86 & 0x10)?1:0);
    setControlValue('86rm', 0, (snd86 & 0x02)?1:0);
}

static void setSPB(void) {
    set26s(spb, 'spio', 'spin', 'sprm');
    setControlValue('spvl', 0, (spbvrc & 1)?1:0);
    setControlValue('spvr', 0, (spbvrc & 2)?1:0);
}

static void initSoundWindow(void) {
	PicHandle   pict;
	ControlRef  disp;
    BYTE		data[5];
    short		i;
    for (i=0;i<5;i++) {
        data[i] = *(cfg[i]);
    }
    initMixer(data);
    init14(np2cfg.vol14);
    
    snd26 = np2cfg.snd26opt;
    uncheckAllPopupMenuItems('26io', 2, soundWin);
    uncheckAllPopupMenuItems('26in', 4, soundWin);
    uncheckAllPopupMenuItems('26rm', 5, soundWin);
    set26s(snd26, '26io', '26in', '26rm');
	disp = getControlRefByID('BMP ', 0, soundWin);
	setbmp(dipswbmp_getsnd26(snd26), &pict);
	SetControlData(disp, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);

    snd86 = np2cfg.snd86opt;
    uncheckAllPopupMenuItems('86io', 2, soundWin);
    uncheckAllPopupMenuItems('86in', 4, soundWin);
    uncheckAllPopupMenuItems('86id', 8, soundWin);
    set86s();
	disp = getControlRefByID('BMP ', 1, soundWin);
	setbmp(dipswbmp_getsnd86(snd86), &pict);
	SetControlData(disp, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);

    spb = np2cfg.spbopt;
    spbvrc = np2cfg.spb_vrc;								// ver0.30
    uncheckAllPopupMenuItems('spio', 2, soundWin);
    uncheckAllPopupMenuItems('spin', 4, soundWin);
    uncheckAllPopupMenuItems('sprm', 5, soundWin);
    setSPB();
    setControlValue('splv', 0, np2cfg.spb_vrl);
    setControlValue('sprv', 0, np2cfg.spb_x);
	disp = getControlRefByID('BMP ', 2, soundWin);
	setbmp(dipswbmp_getsndspb(spb, spbvrc), &pict);
	SetControlData(disp, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);

    initJoyPad();
}

//-----change
static BYTE getsnd26int(OSType board) {
	switch (getControlValue(board, 0))
    {
		case 1:
			return(0x00);

		case 2:
			return(0x80);

		case 4:
			return(0x40);
	}
	return(0xc0);
}

static BYTE getsnd86int(void) {
	switch (getControlValue('86in', 0))
    {
		case 1:
			return(0x00);

		case 2:
			return(0x04);

		case 4:
			return(0x08);
	}
	return(0x0c);
}

static BYTE getspbVRch(void) {
	BYTE	ret = 0;

	if (getControlValue('spvl', 0)) {
		ret++;
	}
	if (getControlValue('spvr', 0)) {
		ret += 2;
	}
	return(ret);
}

static void changeSoundOpt(void) {
    SINT32	val;
	UInt8	update;
	int		renewal;
    short	i,j;
    
    renewal = 0;
    for (j=0;j<5;j++) {
        val = getControlValue('vMix', j);
        if (val != *cfg[j]) {
            *cfg[j] = val;
            renewal = 1;
        }
    }
    if (renewal) {
        sysmng_update(SYS_UPDATECFG);
    }
    opngen_setvol(np2cfg.vol_fm);
    psggen_setvol(np2cfg.vol_ssg);
    rhythm_setvol(np2cfg.vol_rhythm);
    rhythm_update(&rhythm);
    adpcm_setvol(np2cfg.vol_adpcm);
    adpcm_update(&adpcm);
    pcm86gen_setvol(np2cfg.vol_pcm);
    pcm86gen_update();

    renewal = 0;
    for (i=0;i<6;i++) {
        val = getControlValue('vMix',i+10);
        if (val != np2cfg.vol14[i]) {
            np2cfg.vol14[i] = val;
            renewal = 1;
        }
    }
    if (renewal) {
        sysmng_update(SYS_UPDATECFG);
        tms3631_setvol(np2cfg.vol14);
    }

    if (np2cfg.snd26opt != snd26) {
        np2cfg.snd26opt = snd26;
        sysmng_update(SYS_UPDATECFG);
    }
    if (np2cfg.snd86opt != snd86) {
        np2cfg.snd86opt = snd86;
        sysmng_update(SYS_UPDATECFG);
    }

    update = 0;
    if (np2cfg.spbopt != spb) {
        np2cfg.spbopt = spb;
        update |= SYS_UPDATECFG;
    }
    if (np2cfg.spb_vrc != spbvrc) {
        np2cfg.spb_vrc = spbvrc;
        update |= SYS_UPDATECFG;
    }
    val = getControlValue('splv', 0);
    if (np2cfg.spb_vrl != val) {
        np2cfg.spb_vrl = val;
        update |= SYS_UPDATECFG;
    }
    opngen_setVR(np2cfg.spb_vrc, np2cfg.spb_vrl);
    val = getControlValue('sprv', 0);
    if (np2cfg.spb_x != val) {
        np2cfg.spb_x = val;
        update |= SYS_UPDATECFG;
    }
    sysmng_update(update);
    changeJoyPadSetup();                
}


static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    BYTE 		defaultmix[5] = {64, 64, 64, 64, 64};
    BYTE		b;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'vDEF':
                initMixer(defaultmix);
                break;
                
            case '26io':
                setjmper(&snd26, (getMenuValue == 1)?0x10:0x00, 0x10);
                break;

            case '26in':
                setjmper(&snd26, getsnd26int(cmd.commandID), 0xc0);
                break;

            case '26rm':
                setjmper(&snd26, getMenuValue, 0x07);
                break;

            case '26DF':
                snd26 = 0xd1;
                set26s(snd26, '26io', '26in', '26rm');
                break;
                
            case '86io':
                setjmper(&snd86, (getMenuValue == 0)?0x01:0x00, 0x01);
                break;

            case '86it':
                setjmper(&snd86, (getControlValue(cmd.commandID, 0))?0x10:0x00, 0x10);
                break;

            case '86in':
                setjmper(&snd86, getsnd86int(), 0x0c);
                break;

            case '86id':
                setjmper(&snd86, (~getMenuValue & 7) << 5, 0xe0);
                break;

            case '86rm':
                setjmper(&snd86, (getControlValue(cmd.commandID, 0))?0x02:0x00, 0x02);
                break;

            case '86DF':
                snd86 = 0x7f;
                set86s();
                break;

            case 'spio':
                setjmper(&spb, (getMenuValue == 1)?0x10:0x00, 0x10);
                break;
                
            case 'spin':
                setjmper(&spb, getsnd26int(cmd.commandID), 0xc0);
                break;

            case 'sprm':
                setjmper(&spb, getMenuValue, 0x07);
                break;

            case 'spvl':
            case 'spvr':
                b = getspbVRch();
                if ((spbvrc ^ b) & 3) {
                    spbvrc = b;
                }
                break;

            case 'spDF':
                spb = 0xd1;
                spbvrc = 0;
                setSPB();
                break;

            case kHICommandOK:
                changeSoundOpt();
                endLoop(soundWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                revertTemporal();
                endLoop(soundWin);
                err=noErr;
                break;
                
            case JOYPAD_UP:
            case JOYPAD_DOWN:
            case JOYPAD_LEFT:
            case JOYPAD_RIGHT:
            case JOYPAD_ABUTTON:
            case JOYPAD_BBUTTON:
                err=setupJoyConfig(cmd.commandID);
                break;
                
            default:
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static pascal OSStatus sliderEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    return( changeSlider((ControlRef)inUserData, soundWin, 0) );
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

static pascal OSStatus s26proc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
	HIPoint		pos;
	Point		p;
	BYTE		bit, b;
	Rect		ctrlbounds, winbounds;
	PicHandle   pict;
	BOOL		redraw = FALSE;

    if (GetEventClass(event)==kEventClassControl && GetEventKind(event)==kEventControlClick ) {
		err = noErr;
        GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &pos);
		GetControlBounds((ControlRef)userData, &ctrlbounds);
		GetWindowBounds(soundWin, kWindowContentRgn, &winbounds);
		p.h = (short)pos.x;
		p.v = (short)pos.y;
		p.h -= (ctrlbounds.left + winbounds.left);
		p.h /= 9;
		p.v -= (ctrlbounds.top + winbounds.top);
		p.v /= 9;
		if ((p.v < 1) || (p.v >= 3)) {
			return(err);
		}
		if ((p.h >= 2) && (p.h < 7)) {
			b = (BYTE)(p.h - 2);
			if ((snd26 ^ b) & 7) {
				snd26 &= ~0x07;
				snd26 |= b;
				redraw = TRUE;
			}
		}
		else if ((p.h >= 9) && (p.h < 12)) {
			b = snd26;
			bit = 0x40 << (2 - p.v);
			switch(p.h) {
				case 9:
					b |= bit;
					break;

				case 10:
					b ^= bit;
					break;

				case 11:
					b &= ~bit;
					break;
			}
			if (snd26 != b) {
				snd26 = b;
				redraw = TRUE;
			}
		}
		else if ((p.h >= 15) && (p.h < 17)) {
			b = (BYTE)((p.h - 15) << 4);
			if ((snd26 ^ b) & 0x10) {
				snd26 &= ~0x10;
				snd26 |= b;
				redraw = TRUE;
			}
		}
		if (redraw) {
			set26s(snd26, '26io', '26in', '26rm');
			setbmp(dipswbmp_getsnd26(snd26), &pict);
			SetControlData((ControlRef)userData, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);
			Draw1Control((ControlRef)userData);
		}
	}

	(void)myHandler;
	(void)userData;
    return err;
}

static pascal OSStatus s86proc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
	HIPoint		pos;
	Point		p;
	Rect		ctrlbounds, winbounds;
	PicHandle   pict;

    if (GetEventClass(event)==kEventClassControl && GetEventKind(event)==kEventControlClick ) {
		err = noErr;
        GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &pos);
		GetControlBounds((ControlRef)userData, &ctrlbounds);
		GetWindowBounds(soundWin, kWindowContentRgn, &winbounds);
		p.h = (short)pos.x;
		p.h -= (ctrlbounds.left + winbounds.left);
		p.h /= 8;
		if ((p.h < 2) || (p.h >= 10)) {
			return(err);
		}
		p.h -= 2;
		snd86 ^= (1 << p.h);
		set86s();
		setbmp(dipswbmp_getsnd86(snd86), &pict);
		SetControlData((ControlRef)userData, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);
		Draw1Control((ControlRef)userData);
	}

	(void)myHandler;
	(void)userData;
    return err;
}

static pascal OSStatus spbproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
	HIPoint		pos;
	Point		p;
	BYTE		bit, b;
	Rect		ctrlbounds, winbounds;
	PicHandle   pict;
	BOOL		redraw = FALSE;

    if (GetEventClass(event)==kEventClassControl && GetEventKind(event)==kEventControlClick ) {
		err = noErr;
        GetEventParameter(event, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &pos);
		GetControlBounds((ControlRef)userData, &ctrlbounds);
		GetWindowBounds(soundWin, kWindowContentRgn, &winbounds);
		p.h = (short)pos.x;
		p.v = (short)pos.y;
		p.h -= (ctrlbounds.left + winbounds.left);
		p.h /= 9;
		p.v -= (ctrlbounds.top + winbounds.top);
		p.v /= 9;
		if ((p.v < 1) || (p.v >= 3)) {
			return(err);
		}
		if ((p.h >= 2) && (p.h < 5)) {
			b = spb;
			bit = 0x40 << (2 - p.v);
			switch(p.h) {
				case 2:
					b |= bit;
					break;

				case 3:
					b ^= bit;
					break;

				case 4:
					b &= ~bit;
					break;
			}
			if (spb != b) {
				spb = b;
				set26s(b, 'spio', 'spin', 'sprm');
				redraw = TRUE;
			}
		}
		else if (p.h == 7) {
			spb ^= 0x20;
			redraw = TRUE;
		}
		else if ((p.h >= 10) && (p.h < 12)) {
			b = (BYTE)((p.h - 10) << 4);
			if ((spb ^ b) & 0x10) {
				spb &= ~0x10;
				spb |= b;
				set26s(b, 'spio', 'spin', 'sprm');
				redraw = TRUE;
			}
		}
		else if ((p.h >= 14) && (p.h < 19)) {
			b = (BYTE)(p.h - 14);
			if ((spb ^ b) & 7) {
				spb &= ~0x07;
				spb |= b;
				set26s(b, 'spio', 'spin', 'sprm');
				redraw = TRUE;
			}
		}
		else if ((p.h >= 21) && (p.h < 24)) {
			spbvrc ^= (BYTE)(3 - p.v);
			setControlValue('spvl', 0, (spbvrc & 1)?1:0);
			setControlValue('spvr', 0, (spbvrc & 2)?1:0);
			redraw = TRUE;
		}
		if (redraw) {
			setbmp(dipswbmp_getsndspb(spb, spbvrc), &pict);
			SetControlData((ControlRef)userData, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);
			Draw1Control((ControlRef)userData);
		}
	}

	(void)myHandler;
	(void)userData;
    return err;
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    short		i,j,l,k;
    EventHandlerRef	ref;
    ControlRef	targetCon[11], cref[3];
    
    err = CreateWindowFromNib(nibRef, CFSTR("SoundDialog"), &soundWin);
    if (err == noErr) {
    
        initSoundWindow();
        SetInitialTabState(soundWin, lastPaneSelected, kMaxNumTabs);
        
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,soundWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, soundWin, NULL );

        EventTypeSpec	sliderControlEvents[] ={
            { kEventClassControl, kEventControlDraw },
            { kEventClassControl, kEventControlValueFieldChanged }
        };
        for (i=0;i<5;i++) {
            targetCon[i] = getControlRefByID('vMix',i,soundWin);
        }
        for (j=0;j<6;j++) {
            targetCon[j+5] = getControlRefByID('vMix',10+j,soundWin);
        }
        for (l=0;l<11;l++) {
            InstallControlEventHandler( targetCon[l],  sliderEventHandlerProc , GetEventTypeCount(sliderControlEvents), sliderControlEvents, (void *)targetCon[l], NULL );
        }

        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },};
        InstallWindowEventHandler (soundWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)soundWin, &ref);
		
        EventTypeSpec	ctrllist[]={ { kEventClassControl, kEventControlClick } };
        for (k=0;k<3;k++) {
			cref[k] = getControlRefByID('BMP ', k, soundWin);
		}
		InstallControlEventHandler(cref[0], NewEventHandlerUPP(s26proc), GetEventTypeCount(ctrllist), ctrllist, (void *)cref[0], NULL);   
		InstallControlEventHandler(cref[1], NewEventHandlerUPP(s86proc), GetEventTypeCount(ctrllist), ctrllist, (void *)cref[1], NULL);   
		InstallControlEventHandler(cref[2], NewEventHandlerUPP(spbproc), GetEventTypeCount(ctrllist), ctrllist, (void *)cref[2], NULL);   
		
        ShowSheetWindow(soundWin, hWndMain);
        
        err=RunAppModalLoopForWindow(soundWin);
    }
    return;
}

void initSoundOpt( void ) {
    OSStatus	err;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    else {
         return;
    }
}

//--------JoyPad

static pascal OSStatus joyWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'notJ':
                HideSheetWindow(joyWin);
                DisposeWindow(joyWin);
                err = noErr;
                break;
            default:
                break;
        }
    }

    return err;
}

static void setTitle(OSType type, char* elementName) {
    Str255 str;
    mkstr255(str, elementName);
    SetControlTitle(getControlRefByID(type, 0, soundWin), str);
}

static OSStatus setupJoyConfig (OSType type) {
    OSStatus	err = noErr;
    char elementName[256] = "----";

    err = CreateWindowFromNib(nibRef, CFSTR("InputWindow"), &joyWin);
    if (err == noErr) {
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess } };
        EventHandlerRef	ref;
        InstallWindowEventHandler (joyWin, NewEventHandlerUPP(joyWinproc), 1, list, (void *)joyWin, &ref);
        ShowSheetWindow(joyWin, soundWin);

        if (setJoypad(type, elementName)) {
            setTitle(type, elementName);
        }

        HideSheetWindow(joyWin);
        DisposeWindow(joyWin);
    }
    return(err);
}

static void setName (OSType type) {
    char elementName[256] = "----";
    if (getJoypadName(type, elementName)) {
        if (elementName) {
            setTitle(type, elementName);
        }
    }
}

static void initJoyPad(void) {
    setName(JOYPAD_UP);
    setName(JOYPAD_DOWN);
    setName(JOYPAD_LEFT);
    setName(JOYPAD_RIGHT);
    setName(JOYPAD_ABUTTON);
    setName(JOYPAD_BBUTTON);

    initTemporal();
}
