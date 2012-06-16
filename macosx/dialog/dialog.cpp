#include	"compiler.h"
#include	"resource.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"dialog.h"
#include	"diskdrv.h"
#include	"font.h"
#include	"iocore.h"
#include	"np2.h"
#include	"macnewdisk.h"
#include	"scrnbmp.h"
#include	"dosio.h"
#include	"menu.h"
#include	"s98.h"
#include	"fdefine.h"
#include	"toolwin.h"

// ----

static const BYTE pathsep[2] = {0x01, ':'};

static void backpalcalcat(char *dst, int leng, const BYTE *src) {

	int		dlen;
	int		slen;

	if (leng < 2) {
		return;
	}
	slen = src[0];
	dlen = strlen(dst);
	if ((slen + dlen + 1) > leng) {
		if (slen >= leng) {
			slen = leng - 1;
			dlen = 0;
		}
		else {
			dlen = leng - slen - 1;
		}
	}
	dst[slen + dlen] = '\0';
	while(dlen--) {
		dst[slen + dlen] = dst[dlen];
	}
	CopyMemory(dst, (char *)src + 1, slen);
}

void fsspec2path(FSSpec *fs, char *dst, int leng) {

	CInfoPBRec	cipbr;
	Str255		dname;

	if (!leng) {
		return;
	}
	dst[0] = '\0';
	backpalcalcat(dst, leng, fs->name);
	cipbr.dirInfo.ioVRefNum = fs->vRefNum;
	cipbr.dirInfo.ioDrParID = fs->parID;
	cipbr.dirInfo.ioFDirIndex = -1;
	cipbr.dirInfo.ioNamePtr = dname;
	do {
		cipbr.dirInfo.ioDrDirID = cipbr.dirInfo.ioDrParID;
		if (PBGetCatInfo(&cipbr, FALSE) != noErr) {
			break;
		}
		backpalcalcat(dst, leng, pathsep);
		backpalcalcat(dst, leng, dname);
	} while(cipbr.dirInfo.ioDrDirID != fsRtDirID);
}

static NavDialogRef navWin;

static pascal void dummyproc(NavEventCallbackMessage sel, NavCBRecPtr prm,
													NavCallBackUserData ud) {
	switch( sel )
	{
        case kNavCBCancel:
        case kNavCBAccept:
            QuitAppModalLoopForWindow(NavDialogGetWindow(navWin));
            break;
        default:
            break;
	}

	(void)sel;
	(void)prm;
	(void)ud;
}

static pascal Boolean NavLaunchServicesFilterProc( AEDesc* theItem, void* info, NavCallBackUserData ioUserData, NavFilterModes filterMode)
{
	#pragma unused( info )
	OSStatus	err			= noErr;
	Boolean		showItem	= false;
    FSSpec		fsSpec;
    int			ret;
    char		name[MAX_PATH];
	AEDesc 		coerceDesc	= { 0, 0 };
		
	if ( filterMode == kNavFilteringBrowserList ) {
		if ( theItem->descriptorType != typeFSS ) {
			err = AECoerceDesc( theItem, typeFSS, &coerceDesc );
			theItem = &coerceDesc;
		}
		if ( err == noErr ) {
			err = AEGetDescData( theItem, &fsSpec, sizeof(fsSpec) );
        }
		AEDisposeDesc( &coerceDesc );
        
        if (err == noErr) {
            fsspec2path(&fsSpec, name, MAX_PATH);
            if (file_attr(name) == FILEATTR_DIRECTORY) {
                showItem = true;
            }
            else {
                ret = file_getftype(name);
                switch (*(int*)ioUserData) {
                    case OPEN_FDD:
                        if (ret == FTYPE_D88 || ret == FTYPE_BETA || ret == FTYPE_FDI) {
                            showItem = true;
                        }
                        break;
                    case OPEN_SASI:
                        if (ret == FTYPE_THD || ret == FTYPE_HDI || ret == FTYPE_NHD) {
                            showItem = true;
                        }
                        break;
                     case OPEN_SCSI:
                        if (ret == FTYPE_HDD) {
                            showItem = true;
                        }
                        break;
                   case OPEN_FONT:
                        if (ret == FTYPE_BMP || ret == FTYPE_SMIL) {
                            showItem = true;
                        }
                        break;
                    case OPEN_INI:
                        if (ret == FTYPE_INI) {
                            showItem = true;
                        }
                        break;
                    case OPEN_MIMPI:
						showItem = true;
                        break;
                }
            }
        }
	}
	return( showItem );
}

BOOL dialog_fileselect(char *name, int size, WindowRef parent, int opentype) {

	BOOL				ret;
	OSErr				err;
    NavDialogCreationOptions optNav;
	NavReplyRecord		reply;
	NavEventUPP			proc;
	long				count;
	long				i;
	FSSpec				fss;
    NavObjectFilterUPP	navFilterProc;

	ret = FALSE;
    NavGetDefaultDialogCreationOptions(&optNav);
    optNav.clientName = CFSTR("Neko Project IIx");
	if (parent) {
		optNav.modality=kWindowModalityWindowModal;
		optNav.parentWindow=parent;
	}
    switch (opentype) {
        case OPEN_MIMPI:
            optNav.message = CFCopyLocalizedString(CFSTR("Choose MIMPI file."),"MIMPI Message");
            break;
        case OPEN_FONT:
            optNav.message = CFCopyLocalizedString(CFSTR("Choose font file."),"FontSelect Message");
            break;
        case OPEN_INI:
            optNav.message = CFCopyLocalizedString(CFSTR("Choose a skin file for Tool Window."),"SkinSelect Message");
            break;
        default:
            break;
    }
	proc = NewNavEventUPP(dummyproc);
    navFilterProc = NewNavObjectFilterUPP( NavLaunchServicesFilterProc );
    ret=NavCreateGetFileDialog(&optNav,NULL,proc,NULL,navFilterProc,&opentype,&navWin);
    NavDialogRun(navWin);
    RunAppModalLoopForWindow(NavDialogGetWindow(navWin));
    NavDialogGetReply(navWin, &reply);
    if (optNav.message) CFRelease(optNav.message);
    NavDialogDispose(navWin);
	DisposeNavEventUPP(proc);
    
    DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
	if ((!reply.validRecord) && (ret != noErr)) {
		goto fsel_exit;
	}
	err = AECountItems(&reply.selection, &count);
	if (err == noErr) {
		for (i=1; i<= count; i++) {
			err = AEGetNthPtr(&reply.selection, i, typeFSS, NULL, NULL,
													&fss, sizeof(fss), NULL);
			if (err == noErr) {
				fsspec2path(&fss, name, size);
				ret = TRUE;
				break;
			}
		}
		err = NavDisposeReply(&reply);
	}

fsel_exit:
	return(ret);
}

BOOL dialog_filewriteselect(OSType type, char *title, FSSpec *fsc, WindowRef parentWindow)
{	
	OSType				sign='SMil';
	NavEventUPP			eventUPP;
	NavReplyRecord		reply;
	DescType			rtype;
	OSErr				ret;
	AEKeyword			key;
	Size				len;
    FSRef				parent;
    UniCharCount		ulen;
    UniChar*			buffer = NULL;
    NavDialogCreationOptions	copt;

	InitCursor();
    NavGetDefaultDialogCreationOptions(&copt);
    copt.clientName = CFSTR("Neko Project IIx");
    copt.parentWindow = parentWindow;
    copt.saveFileName = CFStringCreateWithCString(NULL, title, CFStringGetSystemEncoding());
    copt.optionFlags += kNavPreserveSaveFileExtension;
    copt.modality = kWindowModalityWindowModal;
    switch (type) {
        case 'AIFF':
            copt.message = CFCopyLocalizedString(CFSTR("Record playing sound as AIFF file."),"SoundRecord Message");
            break;
        case 'BMP ':
            copt.message = CFCopyLocalizedString(CFSTR("Save the screen as BMP file."),"ScreenShot Message");
            break;
        case '.S98':
            copt.message = CFCopyLocalizedString(CFSTR("Log playing sound as S98 file."),"S98Log Message");
            break;
        default:
            break;
    }
	eventUPP=NewNavEventUPP( dummyproc );
    NavCreatePutFileDialog(&copt, type, sign, eventUPP, NULL, &navWin);
    
    NavDialogRun(navWin);
    RunAppModalLoopForWindow(NavDialogGetWindow(navWin));
    
    NavDialogGetReply(navWin, &reply);
    NavDialogDispose(navWin);
	DisposeNavEventUPP(eventUPP);

	if( reply.validRecord)
	{
		ret=AEGetNthPtr( &(reply.selection),1,typeFSRef,&key,&rtype,(Ptr)&parent,(long)sizeof(FSRef),&len );
        ulen = (UniCharCount)CFStringGetLength(reply.saveFileName);
        buffer = (UniChar*)NewPtr(ulen);
        CFStringGetCharacters(reply.saveFileName, CFRangeMake(0, ulen), buffer);
        ret = FSCreateFileUnicode(&parent, ulen, buffer, kFSCatInfoNone, NULL, NULL, fsc);
        DisposePtr((Ptr)buffer);
		NavDisposeReply( &reply );
        if (ret == noErr) {
            return true;
        }
	}
	return( false );
}

// ----

void dialog_changefdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (drv < 4) {
		if (dialog_fileselect(fname, sizeof(fname), hWndMain, OPEN_FDD)) {
            if (file_getftype(fname)==FTYPE_D88 || file_getftype(fname)==FTYPE_BETA || file_getftype(fname)==FTYPE_FDI) {
                diskdrv_setfdd(drv, fname, 0);
                toolwin_setfdd(drv, fname);
            }
		}
	}
}

void dialog_changehdd(BYTE drv) {

	char	fname[MAX_PATH];

	if (!(drv & 0x20)) {		// SASI/IDE
		if (drv < 2) {
			if (dialog_fileselect(fname, sizeof(fname), hWndMain, OPEN_SASI)) {
				if (file_getftype(fname)==FTYPE_HDI || file_getftype(fname)==FTYPE_THD || file_getftype(fname)==FTYPE_NHD) {
					sysmng_update(SYS_UPDATEOSCFG);
					diskdrv_sethdd(drv, fname);
				}
			}
		}
	}
	else {						// SCSI
		if ((drv & 0x0f) < 4) {
			if (dialog_fileselect(fname, sizeof(fname), hWndMain, OPEN_SCSI)) {
				if (file_getftype(fname)==FTYPE_HDD) {
					sysmng_update(SYS_UPDATEOSCFG);
					diskdrv_sethdd(drv, fname);
				}
			}
		}
	}
}


void dialog_font(void) {

    char	name[1024];

	if (dialog_fileselect(name, sizeof(name), hWndMain, OPEN_FONT)) {
        if ((name != NULL) && (font_load(name, FALSE))) {
            gdcs.textdisp |= GDCSCRN_ALLDRAW2;
            milstr_ncpy(np2cfg.fontfile, name, sizeof(np2cfg.fontfile));
            sysmng_update(SYS_UPDATECFG);
        }
    }
}

void dialog_writebmp(void) {

	SCRNBMP	bmp;
	char	path[MAX_PATH];
	FILEH	fh;
    FSSpec	fss;

	bmp = scrnbmp();
	if (bmp) {
		if (dialog_filewriteselect('BMP ', "Neko Project IIx ScreenShot.bmp", &fss, hWndMain)) {
            fsspec2path(&fss, path, MAX_PATH);
			fh = file_create(path);
			if (fh != FILEH_INVALID) {
				file_write(fh, bmp->ptr, bmp->size);
				file_close(fh);
			}
		}
		_MFREE(bmp);
	}
}

void dialog_s98(void) {

	static BOOL	check = FALSE;
	char	fname[MAX_PATH];
    FSSpec	fsc;

	S98_close();

    if (check) {
        check = FALSE;
    }
    else if (dialog_filewriteselect('.S98', "S98 log.s98", &fsc, hWndMain)) {
        fsspec2path(&fsc, fname, MAX_PATH);
        if (S98_open(fname) == SUCCESS) {
            check = TRUE;
        }
    }    
	menu_sets98logging(check);
}
