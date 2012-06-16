#include	"compiler.h"
#include	"dosio.h"
#ifndef NP2GCC
#ifdef TARGET_API_MAC_CARBON
#include	<CFString.h>
#endif
#endif


static	char	curpath[MAX_PATH] = ":";
static	char	*curfilep = curpath + 1;

#define ISKANJI(c)	(((((c) ^ 0x20) - 0xa1) & 0xff) < 0x3c)

void dosio_init(void) { }
void dosio_term(void) { }

											// ファイル操作
FILEH file_open(const char *path) {

	FILEH	ret;
	FSSpec	fss;
	Str255	fname;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	if ((FSpOpenDF(&fss, fsRdWrPerm, &ret) == noErr) ||
		(FSpOpenDF(&fss, fsRdPerm, &ret) == noErr)) {
		SetFPos(ret, fsFromStart, 0);
		return(ret);
	}
	return(-1);
}

FILEH file_open_rb(const char *path) {

	FILEH	ret;
	FSSpec	fss;
	Str255	fname;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	if ((FSpOpenDF(&fss, fsRdWrShPerm, &ret) == noErr) ||
		(FSpOpenDF(&fss, fsRdPerm, &ret) == noErr)) {
		SetFPos(ret, fsFromStart, 0);
		return(ret);
	}
	return(-1);
}

FILEH file_create(const char *path) {

	FILEH	ret;
	FSSpec	fss;
	Str255	fname;
	OSType	creator = '????';
	OSType	fileType = '????';

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpDelete(&fss);

	if (FSpCreate(&fss, creator, fileType, smSystemScript) == noErr) {
		if ((FSpOpenDF(&fss, fsRdPerm | fsWrPerm, &ret) == noErr) ||
			(FSpOpenDF(&fss, fsRdPerm, &ret) == noErr)) {
			SetFPos(ret, fsFromStart, 0);
			return(ret);
		}
	}
	return(-1);
}

long file_seek(FILEH handle, long pointer, int method) {

	SInt32	pos;
	SInt32	setp;

	setp = pointer;
	switch(method) {
		case FSEEK_SET:
			break;
		case FSEEK_CUR:
			if (GetFPos(handle, &pos) != noErr) {
				return(-1);
			}
			setp += pos;
			break;
		case FSEEK_END:
			if (GetEOF(handle, &pos) != noErr) {
				return(-1);
			}
			setp += pos;
			break;
		default:
			return(-1);
	}
	SetFPos(handle, fsFromStart, setp);
	if (GetFPos(handle, &pos) != noErr) {
		return(-1);
	}
	return((long)pos);
}

UINT file_read(FILEH handle, void *data, UINT length) {

	long	size;
	OSErr	err;

	size = length;
	err = FSRead(handle, &size, (char *)data);
	if ((err == noErr) || (err == eofErr)) {
		return(size);
	}
	return(0);
}

UINT file_write(FILEH handle, const void *data, UINT length) {

	if (length) {
		long size = length;
		if (FSWrite(handle, &size, (char *)data) == noErr) {
			return(size);
		}
	}
	else {
		SInt32 pos;
		if (GetFPos(handle, &pos) == noErr) {
			SetEOF(handle, pos);
		}
	}
	return(0);
}

short file_close(FILEH handle) {

	FSClose(handle);
	return(0);
}

UINT file_getsize(FILEH handle) {

	SInt32 pos;

	if (GetEOF(handle, &pos) == noErr) {
		return((UINT)pos);
	}
	else {
		return(0);
	}
}

#ifdef TARGET_API_MAC_CARBON
static void cnvdatetime(UTCDateTime *dt, DOSDATE *dosdate, DOSTIME *dostime) {

	LocalDateTime	ldt;
	DateTimeRec		dtr;

	ZeroMemory(&dtr, sizeof(dtr));
	ConvertUTCToLocalDateTime(dt, &ldt);
	SecondsToDate(ldt.lowSeconds, &dtr);
	if (dosdate) {
		dosdate->year = dtr.year;
		dosdate->month = (UINT8)dtr.month;
		dosdate->day = (UINT8)dtr.day;
	}
	if (dostime) {
		dostime->hour = (UINT8)dtr.hour;
		dostime->minute = (UINT8)dtr.minute;
		dostime->second = (UINT8)dtr.second;
	}
}
#else
static void cnvdatetime(unsigned long s, DOSDATE *dosdate, DOSTIME *dostime) {

	DateTimeRec		dtr;

	ZeroMemory(&dtr, sizeof(dtr));
	SecondsToDate(s, &dtr);
	if (dosdate) {
		dosdate->year = dtr.year;
		dosdate->month = (UINT8)dtr.month;
		dosdate->day = (UINT8)dtr.day;
	}
	if (dostime) {
		dostime->hour = (UINT8)dtr.hour;
		dostime->minute = (UINT8)dtr.minute;
		dostime->second = (UINT8)dtr.second;
	}
}
#endif

short file_getdatetime(FILEH handle, DOSDATE *dosdate, DOSTIME *dostime) {

#ifdef TARGET_API_MAC_CARBON

	FSRef			ref;
	FSCatalogInfo	fsci;

	if ((FSGetForkCBInfo(handle, 0, NULL, NULL, NULL, &ref, NULL) != noErr) ||
		(FSGetCatalogInfo(&ref, kFSCatInfoContentMod, &fsci, NULL, NULL, NULL)
																!= noErr)) {
		return(-1);
	}
	cnvdatetime(&fsci.contentModDate, dosdate, dostime);
	return(0);
#else
	(void)handle;
	(void)dosdate;
	(void)dostime;
	return(-1);
#endif
}

short file_delete(const char *path) {

	FSSpec	fss;
	Str255	fname;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpDelete(&fss);
	return(0);
}

short file_attr(const char *path) {

#ifdef TARGET_API_MAC_CARBON
	Str255			fname;
	FSSpec			fss;
	FSRef			fsr;
	FSCatalogInfo	fsci;
	short			ret;

	mkstr255(fname, path);
	if ((FSMakeFSSpec(0, 0, fname, &fss) != noErr) ||
		(FSpMakeFSRef(&fss, &fsr) != noErr) ||
		(FSGetCatalogInfo(&fsr, kFSCatInfoNodeFlags, &fsci,
										NULL, NULL, NULL) != noErr)) {
		return(-1);
	}
	if (fsci.nodeFlags & kFSNodeIsDirectoryMask) {
		ret = FILEATTR_DIRECTORY;
	}
	else {
		ret = FILEATTR_ARCHIVE;
	}
	if (fsci.nodeFlags & kFSNodeLockedMask) {
		ret |= FILEATTR_READONLY;
	}
	return(ret);
#else
	Str255		fname;
	FSSpec		fss;
	CInfoPBRec	pb;
	short		ret;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	pb.dirInfo.ioNamePtr = fss.name;
	pb.dirInfo.ioVRefNum = fss.vRefNum;
	pb.dirInfo.ioDrDirID = fss.parID;
	if (fss.name[0] == 0) {
		pb.dirInfo.ioFDirIndex = -1;
	}
	else {
		pb.dirInfo.ioFDirIndex = 0;
	}
	if (PBGetCatInfo(&pb, false) != noErr) {
		return(-1);
	}
	if (pb.hFileInfo.ioFlAttrib & ioDirMask) {
		ret = FILEATTR_DIRECTORY;
	}
	else {
		ret = FILEATTR_ARCHIVE;
	}
	if (pb.hFileInfo.ioFlAttrib & 0x01) {
		ret |= FILEATTR_READONLY;
	}
	return(ret);
#endif
}

short file_dircreate(const char *path) {

	FSSpec	fss;
	Str255	fname;
	long	ret;

	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	if (FSpDirCreate(&fss, smSystemScript, &ret) == noErr) {
		return(0);
	}
	return(-1);
}

											// カレントファイル操作
void file_setcd(const char *exepath) {

	file_cpyname(curpath, exepath, sizeof(curpath));
	curfilep = file_getname(curpath);
	*curfilep = '\0';
}

char *file_getcd(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(curpath);
}

FILEH file_open_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_open(curpath));
}

FILEH file_open_rb_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_open_rb(curpath));
}

FILEH file_create_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_create(curpath));
}

short file_delete_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_delete(curpath));
}

short file_attr_c(const char *path) {

	*curfilep = '\0';
	file_catname(curpath, path, sizeof(curpath));
	return(file_attr(curpath));
}


#ifdef TARGET_API_MAC_CARBON

typedef struct {
	BOOL			eoff;
	FSIterator		fsi;
	FSCatalogInfo	fsci;
	HFSUniStr255	name;
} _FLHDL, *FLHDL;

static void char2str(char *dst, int size, const UniChar *uni, int unicnt) {

	CFStringRef	cfsr;

	cfsr = CFStringCreateWithCharacters(NULL, uni, unicnt);
	CFStringGetCString(cfsr, dst, size, CFStringGetSystemEncoding());
	CFRelease(cfsr);
}

void *file_list1st(const char *dir, FLINFO *fli) {

	FLISTH		ret;
	Str255		fname;
	FSSpec		fss;
	FSRef		fsr;
	FSIterator	fsi;

	mkstr255(fname, dir);
	if ((FSMakeFSSpec(0, 0, fname, &fss) != noErr) ||
		(FSpMakeFSRef(&fss, &fsr) != noErr) ||
		(FSOpenIterator(&fsr, kFSIterateFlat, &fsi) != noErr)) {
		goto ff1_err1;
	}
	ret = _MALLOC(sizeof(_FLHDL), dir);
	if (ret == NULL) {
		goto ff1_err2;
	}
	((FLHDL)ret)->eoff = FALSE;
	((FLHDL)ret)->fsi = fsi;
	if (file_listnext(ret, fli) == SUCCESS) {
		return(ret);
	}

ff1_err2:
	FSCloseIterator(fsi);

ff1_err1:
	return(NULL);
}

BOOL file_listnext(FLISTH hdl, FLINFO *fli) {

	FLHDL		flhdl;
	ItemCount	count;
	OSStatus	r;
	UTCDateTime	*dt;

	flhdl = (FLHDL)hdl;
	if ((flhdl == NULL) || (flhdl->eoff)) {
		goto ffn_err;
	}
	r = FSGetCatalogInfoBulk(flhdl->fsi, 1, &count, NULL,
						kFSCatInfoNodeFlags | kFSCatInfoDataSizes |
						kFSCatInfoAllDates,
						&flhdl->fsci, NULL, NULL, &flhdl->name);
	if (r != noErr) {
		flhdl->eoff = TRUE;
		if (r != errFSNoMoreItems) {
			goto ffn_err;
		}
	}
	if (count != 1) {
		flhdl->eoff = TRUE;
		goto ffn_err;
	}
	if (fli) {
		fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
		if (flhdl->fsci.nodeFlags & kFSNodeIsDirectoryMask) {
			fli->attr = FILEATTR_DIRECTORY;
			fli->size = 0;
			dt = &flhdl->fsci.createDate;
		}
		else {
			fli->attr = FILEATTR_ARCHIVE;
			fli->size = (UINT32)flhdl->fsci.dataLogicalSize;
			dt = &flhdl->fsci.contentModDate;
		}
		if (flhdl->fsci.nodeFlags & kFSNodeLockedMask) {
			fli->attr |= FILEATTR_READONLY;
		}
		cnvdatetime(dt, &fli->date, &fli->time);
		char2str(fli->path, sizeof(fli->path),
								flhdl->name.unicode, flhdl->name.length);
	}
	return(SUCCESS);

ffn_err:
	return(FAILURE);
}

void file_listclose(FLISTH hdl) {

	if (hdl) {
		FSCloseIterator(((FLHDL)hdl)->fsi);
		_MFREE(hdl);
	}
}

BOOL getLongFileName(char *dst, const char *path) {

	FSSpec			fss;
	Str255			fname;
	FSRef			fref;
	HFSUniStr255	name;

	if (*path == '\0') {
		return(false);
	}
	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fss);
	FSpMakeFSRef(&fss, &fref);
	if (FSGetCatalogInfo(&fref, kFSCatInfoNone, NULL, &name, NULL, NULL)
																!= noErr) {
		return(false);
	}
	char2str(dst, 512, name.unicode, name.length);
	if (!dst) {
		return(false);
	}
	return(true);
}

#else

typedef struct {
	BOOL		eoff;
	short		index;
	CInfoPBRec	pb;
	long		tagid;
} _FLHDL, *FLHDL;

FLISTH file_list1st(const char *dir, FLINFO *fli) {

	Str255	fname;
	FSSpec	fss;
	FLHDL	ret;

	mkstr255(fname, dir);
	FSMakeFSSpec(0, 0, fname, &fss);
	ret = (FLHDL)_MALLOC(sizeof(_FLHDL), dir);
	if (ret == NULL) {
		goto ff1_err1;
	}
	ret->pb.dirInfo.ioNamePtr = fss.name;
	ret->pb.dirInfo.ioVRefNum = fss.vRefNum;
	ret->pb.dirInfo.ioDrDirID = fss.parID;
	if (fss.name[0] == 0) {
		ret->pb.dirInfo.ioFDirIndex = -1;
	}
	else {
		ret->pb.dirInfo.ioFDirIndex = 0;
	}
	if (PBGetCatInfo(&ret->pb, false) != noErr) {
		goto ff1_err2;
	}
	if (ret->pb.hFileInfo.ioFlAttrib & 0x10) {
		ret->tagid = ret->pb.dirInfo.ioDrDirID;
	}
	else {
		ret->tagid = ret->pb.hFileInfo.ioFlParID;
	}
	ret->eoff = FALSE;
	ret->index = 1;
	if (file_listnext((FLISTH)ret, fli) == SUCCESS) {
		return((FLISTH)ret);
	}

ff1_err2:
	_MFREE(ret);

ff1_err1:
	return(NULL);
}

BOOL file_listnext(FLISTH hdl, FLINFO *fli) {

	FLHDL			flhdl;
	Str255			fname;
	unsigned long	dt;

	flhdl = (FLHDL)hdl;
	if ((flhdl == NULL) || (flhdl->eoff)) {
		goto ffn_err;
	}
	fname[0] = 0;
	flhdl->pb.dirInfo.ioNamePtr = fname;
	flhdl->pb.dirInfo.ioDrDirID = flhdl->tagid;
	flhdl->pb.dirInfo.ioFDirIndex = flhdl->index;
	flhdl->pb.dirInfo.ioACUser = 0;
	if (PBGetCatInfo(&flhdl->pb, false) != noErr) {
		flhdl->eoff = TRUE;
		goto ffn_err;
	}
	flhdl->index++;
	if (fli) {
		fli->caps = FLICAPS_SIZE | FLICAPS_ATTR | FLICAPS_DATE | FLICAPS_TIME;
		if (flhdl->pb.hFileInfo.ioFlAttrib & ioDirMask) {
			fli->attr = FILEATTR_DIRECTORY;
			fli->size = 0;
			dt = flhdl->pb.dirInfo.ioDrCrDat;
		}
		else {
			fli->attr = FILEATTR_ARCHIVE;
			fli->size = flhdl->pb.hFileInfo.ioFlLgLen;
			dt = flhdl->pb.hFileInfo.ioFlMdDat;
		}
		if (flhdl->pb.hFileInfo.ioFlAttrib & 0x01) {
			fli->attr |= FILEATTR_READONLY;
		}
		cnvdatetime(dt, &fli->date, &fli->time);
		mkcstr(fli->path, sizeof(fli->path), fname);
	}
	return(SUCCESS);

ffn_err:
	return(FAILURE);
}

void file_listclose(FLISTH hdl) {

	if (hdl) {
		_MFREE(hdl);
	}
}

BOOL getLongFileName(char *dst, const char *path) {

	(void)dst;
	(void)path;
	return(false);
}
#endif


void file_catname(char *path, const char *sjis, int maxlen) {

	char	*p;

	p = path + strlen(path);
	milstr_ncat(path, sjis, maxlen);
	while(1) {
		if (ISKANJI(*p)) {
			if (*(p+1) == '\0') {
				break;
			}
			p++;
		}
		else if ((*p == '/') || (*p == '\\')) {
			*p = ':';
		}
		else if (*p == '\0') {
			break;
		}
		p++;
	}
}

char *file_getname(const char *path) {

const char 	*ret;

	ret = path;
	while(*path != '\0') {
		if (*path++ == ':') {
			ret = path;
		}
	}
	return((char *)ret);
}

void file_cutname(char *path) {

	char 	*p;

	p = file_getname(path);
	p[0] = '\0';
}

char *file_getext(const char *path) {

const char	*p;
const char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (*p++ == '.') {
			q = p;
		}
	}
	if (!q) {
		q = p;
	}
	return((char *)q);
}

void file_cutext(char *path) {

	char	*p;
	char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (*p == '.') {
			q = p;
		}
		p++;
	}
	if (q) {
		*q = '\0';
	}
}

void file_cutseparator(char *path) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos > 0) && (path[pos] == ':')) {
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos < 0) || (path[pos] == ':') || ((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = ':';
	path[++pos] = '\0';
}

