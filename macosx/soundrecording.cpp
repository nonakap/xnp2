/*
 *  soundrecording.cpp
 *  from X1EMx
 *
 *  Created by tk800 on Fri Jan 31 2003.
 *
 */

#include "compiler.h"
#include "dosio.h"
#include "dialog.h"
#include "pccore.h"
#include "menu.h"
#include "np2.h"
#include "resource.h"
#include "soundrecording.h"

#define maxbuffer 4096*64
static	bool	rec = false;
static	BYTE	sndbuffer[maxbuffer];
static	long	sndposition = 0;
static	FSSpec	soundlog;
static	short	logref = -1;
static  FSSpec	fs;

static void rawtoAIFF(void) {
    FILEH	dst;
    char	filename[1024];
    SInt32	size, aiffsize, partsize;
    UInt16	data[5];
    BYTE	*buffer;
    char	form[] = "FORM";
    char	aiff[] = "AIFF";
    char	comm[] = "COMM";
    char	ssnd[] = "SSND";
    
    GetEOF(logref, &size);
    if (size <= 0) return;
    SetFPos(logref, fsFromStart, 0);
    buffer = (BYTE*)malloc(size);
    if (buffer == NULL) {
        return;
    }
    if (FSRead(logref, &size, buffer) != noErr) {
        SysBeep(0);
        return;
    }
    
    FSpCreate(&soundlog, 'hook', 'AIFF', smSystemScript);
    fsspec2path(&soundlog, filename, 1024);
    if ((dst = file_create(filename)) == FILEH_INVALID) {
        SysBeep(0);
        return;
    }
    
    file_write(dst, form, 4);
    aiffsize = size + (0x10 - (size & 0xf)) + 0x6c;;
    file_write(dst, &aiffsize, 4);
    file_write(dst, aiff, 4);
    file_write(dst, comm, 4);
    data[0] = 0x0000;
    data[1] = 0x0012;
    file_write(dst, data, 4);
    data[0] = 0x0002;
    file_write(dst, data, 2);
    partsize = (aiffsize - 0x6c) /4;
    file_write(dst, &partsize, 4);
    data[0] = 16;
    file_write(dst, data, 2);
    if (np2cfg.samplingrate==44100) {
        data[0] = 0x400e;
    }
    else if (np2cfg.samplingrate==22050) {
        data[0] = 0x400d;
    }
    else {
        data[0] = 0x400c;
    }
    data[1] = 0xac44;
    data[2] = 0x0000;
    data[3] = 0x0000;
    data[4] = 0x0000;
    file_write(dst, data, 10);
    file_write(dst, ssnd, 4);
    file_write(dst, &aiffsize, 4);
    data[0] = 0x0000;
    data[1] = 0x0000;
    file_write(dst, data, 8);
    file_write(dst, buffer, size);
    file_close(dst);
    free(buffer);
}

void recOPM(BYTE* work, int len) {
    if (rec && logref!=-1) {
        if (sndposition + len < maxbuffer) {
            memcpy(sndbuffer + sndposition, work, len);
            sndposition += len;
        }
        else {
            int		remain;
            long	max;
            OSErr	err;
            remain = sndposition + len - maxbuffer;
            memcpy(sndbuffer + sndposition, work, len - remain);
            max = maxbuffer;
            err = FSWrite(logref, &max, (char *)sndbuffer);
            memset(sndbuffer, 0, maxbuffer);
            memcpy(sndbuffer, work + (len - remain), remain);
            sndposition = remain;
            if (err != noErr) {
                SysBeep(0);
                menu_setrecording(true);
            }
        }
        
    }
}

int soundRec(bool end) {
    int ret = -1;
    
    if (rec) {
        FSWrite(logref, &sndposition, (char *)sndbuffer);
        rawtoAIFF();
        FSClose(logref);
        OSErr err;
        err = FSpDelete(&fs);
        rec = false;
        ret = 0;
    }
    else if (!end) {
        if (dialog_filewriteselect('AIFF', "Neko Project IIx Sound.aiff", &soundlog, hWndMain)) {
            SInt16		ref;
            SInt32		dir;
            Str255		path;
            OSErr		err;
            FInfo		info;
            err = FindFolder(kOnAppropriateDisk, kTemporaryFolderType, kCreateFolder, &ref, &dir);
            if (err != noErr) return ret;
            mkstr255(path, "np2x Sound Log.aiff");
            if (HGetFInfo(0, dir, path, &info)==noErr) {
                HDelete(0, dir, path);
            }
            err = HCreate(0, dir, path, 'SMil', 'slog');
            if (err != noErr) return ret;
            err=FSMakeFSSpec(0, dir, path, &fs);
            if (err != noErr) return ret;
            if (FSpOpenDF(&fs, fsRdPerm | fsWrPerm, &logref) == noErr) {
                SetFPos(logref, fsFromStart, 0);
                memset(sndbuffer, 0, maxbuffer);
                rec = true;
                ret = 1;
            }
            else {            
                SysBeep(0);
            }
        }
    }
    return ret;
}

