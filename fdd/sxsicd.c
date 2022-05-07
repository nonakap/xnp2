#include	"compiler.h"
#include	"strres.h"
#include	"textfile.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"sxsi.h"


static const UINT8 cd001[7] = {0x01,'C','D','0','0','1',0x01};

typedef struct {
	FILEH	fh;
	UINT	type;
	UINT	trks;
	_CDTRK	trk[100];
	OEMCHAR	path[MAX_PATH];
} _CDINFO, *CDINFO;


// ---- �Z�N�^2048

static int issec2048(FILEH fh) {

	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
	UINT	fsize;

	fpos = 16 * 2048;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2048_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2048_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2048_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2048_err;
	}
	fsize = file_getsize(fh);
	if ((fsize % 2048) != 0) {
		goto sec2048_err;
	}
	return(fsize / 2048);

sec2048_err:
	return(-1);
}

static REG8 sec2048_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	FILEH	fh;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	pos = pos * 2048;
	fh = ((CDINFO)sxsi->hdl)->fh;
	if (file_seek(fh, pos, FSEEK_SET) != pos) {
		return(0xd0);
	}
	while(size) {
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
	}
	return(0x00);
}


// ---- �Z�N�^2352

static int issec2352(FILEH fh) {

	long	fpos;
	UINT8	buf[2048];
	UINT	secsize;
	UINT	fsize;

	fpos = (16 * 2352) + 16;
	if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
		goto sec2352_err;
	}
	if (file_read(fh, buf, sizeof(buf)) != sizeof(buf)) {
		goto sec2352_err;
	}
	if (memcmp(buf, cd001, 7) != 0) {
		goto sec2352_err;
	}
	secsize = LOADINTELWORD(buf + 128);
	if (secsize != 2048) {
		goto sec2352_err;
	}
	fsize = file_getsize(fh);
	if ((fsize % 2352) != 0) {
		goto sec2352_err;
	}
	return(fsize / 2352);

sec2352_err:
	return(-1);
}

static REG8 sec2352_read(SXSIDEV sxsi, long pos, UINT8 *buf, UINT size) {

	FILEH	fh;
	long	fpos;
	UINT	rsize;

	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(0x60);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(0x40);
	}
	fh = ((CDINFO)sxsi->hdl)->fh;
	while(size) {
		fpos = (pos * 2352) + 16;
		if (file_seek(fh, fpos, FSEEK_SET) != fpos) {
			return(0xd0);
		}
		rsize = min(size, 2048);
		CPU_REMCLOCK -= rsize;
		if (file_read(fh, buf, rsize) != rsize) {
			return(0xd0);
		}
		buf += rsize;
		size -= rsize;
		pos++;
	}
	return(0x00);
}


// ----

static BRESULT cd_reopen(SXSIDEV sxsi) {

	CDINFO	cdinfo;
	FILEH	fh;

	cdinfo = (CDINFO)sxsi->hdl;
	fh = file_open_rb(cdinfo->path);
	if (fh != FILEH_INVALID) {
		cdinfo->fh = fh;
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

static void cd_close(SXSIDEV sxsi) {

	CDINFO	cdinfo;

	cdinfo = (CDINFO)sxsi->hdl;
	file_close(cdinfo->fh);
}

static void cd_destroy(SXSIDEV sxsi) {

	_MFREE((CDINFO)sxsi->hdl);
}


// ----

static const OEMCHAR str_cue[] = OEMTEXT("cue");
static const OEMCHAR str_file[] = OEMTEXT("FILE");
static const OEMCHAR str_track[] = OEMTEXT("TRACK");
static const OEMCHAR str_mode1[] = OEMTEXT("MODE1/2352");
static const OEMCHAR str_index[] = OEMTEXT("INDEX");
static const OEMCHAR str_audio[] = OEMTEXT("AUDIO");


static BRESULT openimg(SXSIDEV sxsi, const OEMCHAR *path,
												const _CDTRK *trk, UINT trks) {

	FILEH	fh;
	UINT	type;
	int		totals;
	CDINFO	cdinfo;
	UINT	mediatype;
	UINT	i;

	fh = file_open_rb(path);
	if (fh == FILEH_INVALID) {
		goto sxsiope_err1;
	}
	type = 2048;
	totals = issec2048(fh);
	if (totals < 0) {
		type = 2352;
		totals = issec2352(fh);
	}
	if (totals < 0) {
		goto sxsiope_err2;
	}
	cdinfo = (CDINFO)_MALLOC(sizeof(_CDINFO), path);
	if (cdinfo == NULL) {
		goto sxsiope_err2;
	}
	ZeroMemory(cdinfo, sizeof(_CDINFO));
	cdinfo->fh = fh;
	cdinfo->type = type;
	if ((trk != NULL) && (trks != 0)) {
		trks = min(trks, NELEMENTS(cdinfo->trk) - 1);
		CopyMemory(cdinfo->trk, trk, trks * sizeof(_CDTRK));
	}
	else {
		cdinfo->trk[0].type = 0x14;
		cdinfo->trk[0].track = 1;
//		cdinfo->trk[0].pos = 0;
		trks = 1;
	}
	mediatype = 0;
	for (i=0; i<trks; i++) {
		if (cdinfo->trk[i].type == 0x14) {
			mediatype |= SXSIMEDIA_DATA;
		}
		else if (cdinfo->trk[i].type == 0x10) {
			mediatype |= SXSIMEDIA_AUDIO;
		}
	}
	cdinfo->trk[trks].type = 0x10;
	cdinfo->trk[trks].track = 0xaa;
	cdinfo->trk[trks].pos = totals;
	cdinfo->trks = trks;
	file_cpyname(cdinfo->path, path, NELEMENTS(cdinfo->path));

	sxsi->reopen = cd_reopen;
	if (type == 2048) {
		sxsi->read = sec2048_read;
	}
	else {
		sxsi->read = sec2352_read;
	}
	sxsi->close = cd_close;
	sxsi->destroy = cd_destroy;
	sxsi->hdl = (INTPTR)cdinfo;
	sxsi->totals = totals;
	sxsi->cylinders = 0;
	sxsi->size = 2048;
	sxsi->sectors = 1;
	sxsi->surfaces = 1;
	sxsi->headersize = 0;
	sxsi->mediatype = mediatype;
	return(SUCCESS);

sxsiope_err2:
	file_close(fh);

sxsiope_err1:
	return(FAILURE);
}

static BRESULT getint2(const OEMCHAR *str, UINT *val) {

	if ((str[0] < '0') || (str[0] > '9') ||
		(str[1] < '0') || (str[1] > '9')) {
		return(FAILURE);
	}
	if (val) {
		*val = ((str[0] - '0') * 10) + (str[1] - '0');
	}
	return(SUCCESS);
}

static UINT32 getpos(const OEMCHAR *str) {

	UINT	m;
	UINT	s;
	UINT	f;

	if ((getint2(str + 0, &m) != SUCCESS) || (str[2] != ':') ||
		(getint2(str + 3, &s) != SUCCESS) || (str[5] != ':') ||
		(getint2(str + 6, &f) != SUCCESS)) {
		return(0);
	}
	return((((m * 60) + s) * 75) + f);
}

static BRESULT opencue(SXSIDEV sxsi, const OEMCHAR *fname) {

	_CDTRK		trk[99];
	OEMCHAR		path[MAX_PATH];
	UINT		idx;
	UINT8		curtrk;
	UINT		curtype;
	TEXTFILEH	tfh;
	OEMCHAR		buf[512];
	OEMCHAR		*argv[8];
	int			argc;

	ZeroMemory(trk, sizeof(trk));
	path[0] = '\0';
	idx = 0;
	curtrk = 1;
	curtype = 0x14;
	tfh = textfile_open(fname, 0x800);
	if (tfh == NULL) {
		return(FAILURE);
	}
	while(textfile_read(tfh, buf, NELEMENTS(buf)) == SUCCESS) {
		argc = milstr_getarg(buf, argv, NELEMENTS(argv));
		if ((argc >= 3) && (!milstr_cmp(argv[0], str_file))) {
			file_cpyname(path, fname, NELEMENTS(path));
			file_cutname(path);
			file_catname(path, argv[1], NELEMENTS(path));
		}
		else if ((argc >= 3) && (!milstr_cmp(argv[0], str_track))) {
			curtrk = (UINT8)milstr_solveINT(argv[1]);
			if (!milstr_cmp(argv[2], str_mode1)) {
				curtype = 0x14;
			}
			else if (!milstr_cmp(argv[2], str_audio)) {
				curtype = 0x10;
			}
		}
		else if ((argc >= 3) && (!milstr_cmp(argv[0], str_index))) {
			if (idx < NELEMENTS(trk)) {
				trk[idx].type = curtype;
				trk[idx].track = curtrk;
				trk[idx].pos = getpos(argv[2]);
				idx++;
			}
		}
	}
	textfile_close(tfh);
	return(openimg(sxsi, path, trk, idx));
}

BRESULT sxsicd_open(SXSIDEV sxsi, const OEMCHAR *fname) {

const OEMCHAR	*ext;

	ext = file_getext(fname);
	if (!file_cmpname(ext, str_cue)) {
		return(opencue(sxsi, fname));
	}
	return(openimg(sxsi, fname, NULL, 0));
}

CDTRK sxsicd_gettrk(SXSIDEV sxsi, UINT *tracks) {

	CDINFO	cdinfo;

	cdinfo = (CDINFO)sxsi->hdl;
	if (tracks) {
		*tracks = cdinfo->trks;
	}
	return(cdinfo->trk);
}

BRESULT sxsicd_readraw(SXSIDEV sxsi, long pos, void *buf) {

	CDINFO	cdinfo;
	FILEH	fh;
	long	fpos;

	cdinfo = (CDINFO)sxsi->hdl;
	if (cdinfo->type != 2352) {
		return(FAILURE);
	}
	if (sxsi_prepare(sxsi) != SUCCESS) {
		return(FAILURE);
	}
	if ((pos < 0) || (pos >= sxsi->totals)) {
		return(FAILURE);
	}
	fh = ((CDINFO)sxsi->hdl)->fh;
	fpos = pos * 2352;
	if ((file_seek(fh, fpos, FSEEK_SET) != fpos) ||
		(file_read(fh, buf, 2352) != 2352)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

