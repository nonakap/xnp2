/**
 * @file	statsave.cpp
 * @brief	Implementation of State save
 */

#include "compiler.h"
#include "statsave.h"
#include "strres.h"
#include "dosio.h"
#include "commng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "timemng.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "gdc_sub.h"
#include "cbuscore.h"
#include "ideio.h"
#include "sasiio.h"
#include "scsiio.h"
#include "pc9861k.h"
#include "mpu98ii.h"
#include "board14.h"
#include "amd98.h"
#include "bios/bios.h"
#include "vram.h"
#include "palettes.h"
#include "maketext.h"
#include "sound/sndcsec.h"
#include "sound.h"
#include "fmboard.h"
#include "beep.h"
#include "fdd/fddfile.h"
#include "fdd/fdd_mtr.h"
#include "fdd/sxsi.h"
#include "font/font.h"
#include "keydisp.h"
#include "hostdrv.h"
#include "calendar.h"
#include "keystat.h"

#if defined(MACOS)
#define	CRCONST		str_cr
#elif defined(WIN32) || defined(X11)
#define	CRCONST		str_lf
#else
#define	CRCONST		str_crlf
#endif

typedef struct {
	char	name[16];
	char	vername[28];
	UINT32	ver;
} NP2FHDR;

typedef struct {
	char	index[10];
	UINT16	ver;
	UINT32	size;
} NP2FENT;

/**
 * @brief handle
 */
struct TagStatFlagHandle
{
	NP2FENT		hdr;
	UINT		pos;
	OEMCHAR		*err;
	int			errlen;
};
typedef struct TagStatFlagHandle _STFLAGH;		/* define */

enum
{
	STATFLAG_BIN			= 0,
	STATFLAG_TERM,
	STATFLAG_COM,
	STATFLAG_DMA,
	STATFLAG_EGC,
	STATFLAG_EPSON,
	STATFLAG_EVT,
	STATFLAG_EXT,
	STATFLAG_FDD,
	STATFLAG_FM,
	STATFLAG_GIJ,
#if defined(SUPPORT_HOSTDRV)
	STATFLAG_HDRV,
#endif
	STATFLAG_MEM,
	STATFLAG_SXSI
};

typedef struct {
	UINT32	id;
	void	*proc;
} PROCTBL;

typedef struct {
	UINT32	id;
	NEVENTID num;
} ENUMTBL;

#define	PROCID(a, b, c, d)	(((d) << 24) + ((c) << 16) + ((b) << 8) + (a))
#define	PROC2NUM(a, b)		proc2num(&(a), (b), sizeof(b)/sizeof(PROCTBL))
#define	NUM2PROC(a, b)		num2proc(&(a), (b), sizeof(b)/sizeof(PROCTBL))

#include "statsave.tbl"


extern	COMMNG	cm_mpu98;
extern	COMMNG	cm_rs232c;

typedef struct {
	OEMCHAR	*buf;
	int		remain;
} ERR_BUF;


// ----

enum {
	SFFILEH_WRITE	= 0x0001,
	SFFILEH_BLOCK	= 0x0002,
	SFFILEH_ERROR	= 0x0004
};

typedef struct {
	_STFLAGH	sfh;
	UINT		stat;
	FILEH		fh;
	UINT		secpos;
	NP2FHDR		f;
} _SFFILEH, *SFFILEH;

static SFFILEH statflag_open(const OEMCHAR *filename, OEMCHAR *err, int errlen) {

	FILEH	fh;
	SFFILEH	ret;

	fh = file_open_rb(filename);
	if (fh == FILEH_INVALID) {
		goto sfo_err1;
	}
	ret = (SFFILEH)_MALLOC(sizeof(_SFFILEH), filename);
	if (ret == NULL) {
		goto sfo_err2;
	}
	if ((file_read(fh, &ret->f, sizeof(NP2FHDR)) == sizeof(NP2FHDR)) &&
		(!memcmp(&ret->f, &np2flagdef, sizeof(np2flagdef)))) {
		ZeroMemory(ret, sizeof(_SFFILEH));
		ret->fh = fh;
		ret->secpos = sizeof(NP2FHDR);
		if ((err) && (errlen > 0)) {
			err[0] = '\0';
			ret->sfh.err = err;
			ret->sfh.errlen = errlen;
		}
		return(ret);
	}
	_MFREE(ret);

sfo_err2:
	file_close(fh);

sfo_err1:
	return(NULL);
}

static int statflag_closesection(SFFILEH sffh) {

	UINT	leng;
	UINT8	zero[16];

	if (sffh == NULL) {
		goto sfcs_err1;
	}
	if (sffh->stat == (SFFILEH_BLOCK | SFFILEH_WRITE)) {
		leng = (0 - sffh->sfh.hdr.size) & 15;
		if (leng) {
			ZeroMemory(zero, sizeof(zero));
			if (file_write(sffh->fh, zero, leng) != leng) {
				goto sfcs_err2;
			}
		}
		if ((file_seek(sffh->fh, (long)sffh->secpos, FSEEK_SET)
												!= (long)sffh->secpos) ||
			(file_write(sffh->fh, &sffh->sfh.hdr, sizeof(sffh->sfh.hdr))
												!= sizeof(sffh->sfh.hdr))) {
			goto sfcs_err2;
		}
	}
	if (sffh->stat & SFFILEH_BLOCK) {
		sffh->stat &= ~SFFILEH_BLOCK;
		sffh->secpos += sizeof(sffh->sfh.hdr) +
									((sffh->sfh.hdr.size + 15) & (~15));
		if (file_seek(sffh->fh, (long)sffh->secpos, FSEEK_SET)
												!= (long)sffh->secpos) {
			goto sfcs_err2;
		}
	}
	return(STATFLAG_SUCCESS);

sfcs_err2:
	sffh->stat = SFFILEH_ERROR;

sfcs_err1:
	return(STATFLAG_FAILURE);
}

static int statflag_readsection(SFFILEH sffh) {

	int		ret;

	ret = statflag_closesection(sffh);
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	if ((sffh->stat == 0) &&
		(file_read(sffh->fh, &sffh->sfh.hdr, sizeof(sffh->sfh.hdr))
												== sizeof(sffh->sfh.hdr))) {
		sffh->stat = SFFILEH_BLOCK;
		sffh->sfh.pos = 0;
		return(STATFLAG_SUCCESS);
	}
	sffh->stat = SFFILEH_ERROR;
	return(STATFLAG_FAILURE);
}

int statflag_read(STFLAGH sfh, void *buf, UINT size) {

	if ((sfh == NULL) || (buf == NULL) ||
		((sfh->pos + size) > sfh->hdr.size)) {
		goto sfr_err;
	}
	if (size) {
		if (file_read(((SFFILEH)sfh)->fh, buf, size) != size) {
			goto sfr_err;
		}
		sfh->pos += size;
	}
	return(STATFLAG_SUCCESS);

sfr_err:
	return(STATFLAG_FAILURE);
}

static SFFILEH statflag_create(const OEMCHAR *filename) {

	SFFILEH	ret;
	FILEH	fh;

	ret = (SFFILEH)_MALLOC(sizeof(_SFFILEH), filename);
	if (ret == NULL) {
		goto sfc_err1;
	}
	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto sfc_err2;
	}
	if (file_write(fh, &np2flagdef, sizeof(NP2FHDR)) == sizeof(NP2FHDR)) {
		ZeroMemory(ret, sizeof(_SFFILEH));
		ret->stat = SFFILEH_WRITE;
		ret->fh = fh;
		ret->secpos = sizeof(NP2FHDR);
		return(ret);
	}
	file_close(fh);
	file_delete(filename);

sfc_err2:
	_MFREE(ret);

sfc_err1:
	return(NULL);
}

static int statflag_createsection(SFFILEH sffh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_closesection(sffh);
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	if (sffh->stat != SFFILEH_WRITE) {
		sffh->stat = SFFILEH_ERROR;
		return(STATFLAG_FAILURE);
	}
	CopyMemory(sffh->sfh.hdr.index, tbl->index, sizeof(sffh->sfh.hdr.index));
	sffh->sfh.hdr.ver = tbl->ver;
	sffh->sfh.hdr.size = 0;
	return(STATFLAG_SUCCESS);
}

int statflag_write(STFLAGH sfh, const void *buf, UINT size) {

	SFFILEH	sffh;

	if (sfh == NULL) {
		goto sfw_err1;
	}
	sffh = (SFFILEH)sfh;
	if (!(sffh->stat & SFFILEH_WRITE)) {
		goto sfw_err2;
	}
	if (!(sffh->stat & SFFILEH_BLOCK)) {
		sffh->stat |= SFFILEH_BLOCK;
		sfh->pos = 0;
		if (file_write(sffh->fh, &sfh->hdr, sizeof(sfh->hdr))
														!= sizeof(sfh->hdr)) {
			goto sfw_err2;
		}
	}
	if (size) {
		if ((buf == NULL) || (file_write(sffh->fh, buf, size) != size)) {
			goto sfw_err2;
		}
		sfh->pos += size;
		if (sfh->hdr.size < sfh->pos) {
			sfh->hdr.size = sfh->pos;
		}
	}
	return(STATFLAG_SUCCESS);

sfw_err2:
	sffh->stat = SFFILEH_ERROR;

sfw_err1:
	return(STATFLAG_FAILURE);
}

static void statflag_close(SFFILEH sffh) {

	if (sffh) {
		statflag_closesection(sffh);
		file_close(sffh->fh);
		_MFREE(sffh);
	}
}

void statflag_seterr(STFLAGH sfh, const OEMCHAR *str) {

	if ((sfh) && (sfh->errlen)) {
		milstr_ncat(sfh->err, str, sfh->errlen);
		milstr_ncat(sfh->err, CRCONST, sfh->errlen);
	}
}


// ---- function

// �֐��|�C���^�� int�ɕύX�B
static BRESULT proc2num(void *func, const PROCTBL *tbl, int size) {

	int		i;

	for (i=0; i<size; i++) {
		if (*(INTPTR *)func == (INTPTR)tbl->proc) {
			*(INTPTR *)func = (INTPTR)tbl->id;
			return(SUCCESS);
		}
		tbl++;
	}
	return(FAILURE);
}

static BRESULT num2proc(void *func, const PROCTBL *tbl, int size) {

	int		i;

	for (i=0; i<size; i++) {
		if (*(INTPTR *)func == (INTPTR)tbl->id) {
			*(INTPTR *)func = (INTPTR)tbl->proc;
			return(SUCCESS);
		}
		tbl++;
	}
	return(FAILURE);
}


// ---- file

typedef struct {
	OEMCHAR	path[MAX_PATH];
	UINT	ftype;
	int		readonly;
	DOSDATE	date;
	DOSTIME	time;
} STATPATH;

static const OEMCHAR str_updated[] = OEMTEXT("%s: updated");
static const OEMCHAR str_notfound[] = OEMTEXT("%s: not found");

static int statflag_writepath(STFLAGH sfh, const OEMCHAR *path,
												UINT ftype, int readonly) {

	STATPATH	sp;
	FILEH		fh;

	ZeroMemory(&sp, sizeof(sp));
	if ((path) && (path[0])) {
		file_cpyname(sp.path, path, NELEMENTS(sp.path));
		sp.ftype = ftype;
		sp.readonly = readonly;
		fh = file_open_rb(path);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &sp.date, &sp.time);
			file_close(fh);
		}
	}
	return(statflag_write(sfh, &sp, sizeof(sp)));
}

static int statflag_checkpath(STFLAGH sfh, const OEMCHAR *dvname) {

	int			ret;
	STATPATH	sp;
	FILEH		fh;
	OEMCHAR		buf[256];
	DOSDATE		dosdate;
	DOSTIME		dostime;

	ret = statflag_read(sfh, &sp, sizeof(sp));
	if (sp.path[0]) {
		fh = file_open_rb(sp.path);
		if (fh != FILEH_INVALID) {
			file_getdatetime(fh, &dosdate, &dostime);
			file_close(fh);
			if ((memcmp(&sp.date, &dosdate, sizeof(dosdate))) ||
				(memcmp(&sp.time, &dostime, sizeof(dostime)))) {
				ret |= STATFLAG_DISKCHG;
				OEMSPRINTF(buf, str_updated, dvname);
				statflag_seterr(sfh, buf);
			}
		}
		else {
			ret |= STATFLAG_DISKCHG;
			OEMSPRINTF(buf, str_notfound, dvname);
			statflag_seterr(sfh, buf);
		}
	}
	return(ret);
}


// ---- common

static int flagsave_common(STFLAGH sfh, const SFENTRY *tbl) {

	return(statflag_write(sfh, tbl->arg1, tbl->arg2));
}

static int flagload_common(STFLAGH sfh, const SFENTRY *tbl) {

	return(statflag_read(sfh, tbl->arg1, tbl->arg2));
}


// ---- memory

static int flagsave_mem(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_write(sfh, mem, 0x110000);
	ret |= statflag_write(sfh, mem + VRAM1_B, 0x18000);
	ret |= statflag_write(sfh, mem + VRAM1_E, 0x8000);
	(void)tbl;
	return(ret);
}

static int flagload_mem(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, mem, 0x110000);
	ret |= statflag_read(sfh, mem + VRAM1_B, 0x18000);
	ret |= statflag_read(sfh, mem + VRAM1_E, 0x8000);
	(void)tbl;
	return(ret);
}


// ---- dma

static int flagsave_dma(STFLAGH sfh, const SFENTRY *tbl) {

	int			i;
	_DMAC		dmabak;

	dmabak = dmac;
	for (i=0; i<4; i++) {
		if ((PROC2NUM(dmabak.dmach[i].proc.outproc, dmaproc)) ||
			(PROC2NUM(dmabak.dmach[i].proc.inproc, dmaproc)) ||
			(PROC2NUM(dmabak.dmach[i].proc.extproc, dmaproc))) {
			return(STATFLAG_FAILURE);
		}
	}
	(void)tbl;
	return(statflag_write(sfh, &dmabak, sizeof(dmabak)));
}

static int flagload_dma(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;

	ret = statflag_read(sfh, &dmac, sizeof(dmac));

	for (i=0; i<4; i++) {
		if (NUM2PROC(dmac.dmach[i].proc.outproc, dmaproc)) {
			dmac.dmach[i].proc.outproc = dma_dummyout;
			ret |= STATFLAG_WARNING;
		}
		if (NUM2PROC(dmac.dmach[i].proc.inproc, dmaproc)) {
			dmac.dmach[i].proc.inproc = dma_dummyin;
			ret |= STATFLAG_WARNING;
		}
		if (NUM2PROC(dmac.dmach[i].proc.extproc, dmaproc)) {
			dmac.dmach[i].proc.extproc = dma_dummyproc;
			ret |= STATFLAG_WARNING;
		}
	}
	(void)tbl;
	return(ret);
}


// ---- egc

static int flagsave_egc(STFLAGH sfh, const SFENTRY *tbl) {

	_EGC	egcbak;

	egcbak = egc;
	egcbak.inptr -= (INTPTR)egc.buf;
	egcbak.outptr -= (INTPTR)egc.buf;
	(void)tbl;
	return(statflag_write(sfh, &egcbak, sizeof(egcbak)));
}

static int flagload_egc(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, &egc, sizeof(egc));
	egc.inptr += (INTPTR)egc.buf;
	egc.outptr += (INTPTR)egc.buf;
	(void)tbl;
	return(ret);
}


// ---- epson

static int flagsave_epson(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	if (!(pccore.model & PCMODEL_EPSON)) {
		return(STATFLAG_SUCCESS);
	}
	ret = statflag_write(sfh, &epsonio, sizeof(epsonio));
	ret |= statflag_write(sfh, mem + 0x1c0000, 0x8000);
	ret |= statflag_write(sfh, mem + 0x1e8000, 0x18000);
	(void)tbl;
	return(ret);
}

static int flagload_epson(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = statflag_read(sfh, &epsonio, sizeof(epsonio));
	ret |= statflag_read(sfh, mem + 0x1c0000, 0x8000);
	ret |= statflag_read(sfh, mem + 0x1e8000, 0x18000);
	(void)tbl;
	return(ret);
}


// ---- event

typedef struct {
	UINT		readyevents;
} NEVTSAVE;

typedef struct {
	UINT32		id;
	SINT32		clock;
	NEVENTCB	proc;
} NEVTITEM;

static int nevent_write(STFLAGH sfh, NEVENTID num) {

	NEVTITEM	nit;
	UINT		i;

	ZeroMemory(&nit, sizeof(nit));
	for (i=0; i<NELEMENTS(evtnum); i++) {
		if (evtnum[i].num == num) {
			nit.id = evtnum[i].id;
			break;
		}
	}
	nit.clock = g_nevent.item[num].clock;
	nit.proc = g_nevent.item[num].proc;
	if (PROC2NUM(nit.proc, evtproc)) {
		nit.proc = NULL;
	}
	return(statflag_write(sfh, &nit, sizeof(nit)));
}

static int flagsave_evt(STFLAGH sfh, const SFENTRY *tbl) {

	NEVTSAVE	nevt;
	int			ret;
	UINT		i;

	nevt.readyevents = g_nevent.readyevents;

	ret = statflag_write(sfh, &nevt, sizeof(nevt));
	for (i=0; i<nevt.readyevents; i++) {
		ret |= nevent_write(sfh, g_nevent.level[i]);
	}
	(void)tbl;
	return(ret);
}

static int nevent_read(STFLAGH sfh, NEVENTID *tbl, UINT *pos) {

	int			ret;
	NEVTITEM	nit;
	UINT		i;
	NEVENTID	num;

	ret = statflag_read(sfh, &nit, sizeof(nit));

	for (i=0; i<NELEMENTS(evtnum); i++) {
		if (nit.id == evtnum[i].id) {
			break;
		}
	}
	if (i < NELEMENTS(evtnum)) {
		num = evtnum[i].num;
		g_nevent.item[num].clock = nit.clock;
		g_nevent.item[num].proc = nit.proc;
		if (NUM2PROC(g_nevent.item[num].proc, evtproc)) {
			ret |= STATFLAG_WARNING;
		}
		else {
			tbl[*pos] = num;
			(*pos)++;
		}
	}
	else {
		ret |= STATFLAG_WARNING;
	}
	return(ret);
}

static int flagload_evt(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	NEVTSAVE	nevt;
	UINT		i;

	ret = statflag_read(sfh, &nevt, sizeof(nevt));

	g_nevent.readyevents = 0;
	for (i=0; i<nevt.readyevents; i++) {
		ret |= nevent_read(sfh, g_nevent.level, &g_nevent.readyevents);
	}
	(void)tbl;
	return(ret);
}


// ---- extmem

static int flagsave_ext(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = STATFLAG_SUCCESS;
	if (CPU_EXTMEM) {
		ret = statflag_write(sfh, CPU_EXTMEM, CPU_EXTMEMSIZE);
	}
	(void)tbl;
	return(ret);
}

static int flagload_ext(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;

	ret = STATFLAG_SUCCESS;
	if (CPU_EXTMEM) {
		ret = statflag_read(sfh, CPU_EXTMEM, CPU_EXTMEMSIZE);
	}
	(void)tbl;
	return(ret);
}


// ---- gaiji

static int flagsave_gij(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	int		j;
const UINT8	*fnt;

	ret = STATFLAG_SUCCESS;
	for (i=0; i<2; i++) {
		fnt = fontrom + ((0x56 + (i << 7)) << 4);
		for (j=0; j<0x80; j++) {
			ret |= statflag_write(sfh, fnt, 32);
			fnt += 0x1000;
		}
	}
	(void)tbl;
	return(ret);
}

static int flagload_gij(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	int		j;
	UINT8	*fnt;

	ret = 0;
	for (i=0; i<2; i++) {
		fnt = fontrom + ((0x56 + (i << 7)) << 4);
		for (j=0; j<0x80; j++) {
			ret |= statflag_read(sfh, fnt, 32);
			fnt += 0x1000;
		}
	}
	(void)tbl;
	return(ret);
}


// ---- FM

#if !defined(DISABLE_SOUND)

/**
 * chip flags
 */
enum
{
	FLAG_MG			= 0x0001,
	FLAG_OPNA1		= 0x0002,
	FLAG_OPNA2		= 0x0004,
#if defined(SUPPORT_PX)
	FLAG_OPNA3		= 0x0008,
	FLAG_OPNA4		= 0x0010,
	FLAG_OPNA5		= 0x0020,
#endif	/* defined(SUPPORT_PX) */
	FLAG_AMD98		= 0x0040,
	FLAG_PCM86		= 0x0080,
	FLAG_CS4231		= 0x0100,
	FLAG_OPL3		= 0x0200
};

/**
 * Gets flags
 * @param[in] nSoundID The sound ID
 * @return The flags
 */
static UINT GetSoundFlags(SOUNDID nSoundID)
{
	switch (nSoundID)
	{
		case SOUNDID_PC_9801_14:
			return FLAG_MG;

		case SOUNDID_PC_9801_26K:
			return FLAG_OPNA1;

		case SOUNDID_PC_9801_86:
			return FLAG_OPNA1 | FLAG_PCM86;

		case SOUNDID_PC_9801_86_26K:
			return FLAG_OPNA1 | FLAG_OPNA2 | FLAG_PCM86;

		case SOUNDID_PC_9801_118:
			return FLAG_OPNA1 | FLAG_CS4231;

		case SOUNDID_PC_9801_86_ADPCM:
			return FLAG_OPNA1 | FLAG_PCM86;

		case SOUNDID_SPEAKBOARD:
			return FLAG_OPNA1;

		case SOUNDID_SPARKBOARD:
			return FLAG_OPNA1 | FLAG_OPNA2;

		case SOUNDID_AMD98:
			return FLAG_AMD98;

		case SOUNDID_SOUNDORCHESTRA:
		case SOUNDID_SOUNDORCHESTRAV:
			return FLAG_OPNA1 | FLAG_OPL3;

#if defined(SUPPORT_PX)
		case SOUNDID_PX1:
			return FLAG_OPNA1 | FLAG_OPNA2 | FLAG_OPNA3 | FLAG_OPNA4;

		case SOUNDID_PX2:
			return FLAG_OPNA1 | FLAG_OPNA2 | FLAG_OPNA3 | FLAG_OPNA4 | FLAG_OPNA5 | FLAG_PCM86;
#endif	/* defined(SUPPORT_PX) */

		default:
			return 0;
	}
}

static int flagsave_fm(STFLAGH sfh, const SFENTRY *tbl)
{
	int ret;
	UINT nSaveFlags;
	UINT i;

	ret = statflag_write(sfh, &g_nSoundID, sizeof(g_nSoundID));

	nSaveFlags = GetSoundFlags(g_nSoundID);
	if (nSaveFlags & FLAG_MG)
	{
		ret |= statflag_write(sfh, &g_musicgen, sizeof(g_musicgen));
	}
	for (i = 0; i < NELEMENTS(g_opna); i++)
	{
		if (nSaveFlags & (FLAG_OPNA1 << i))
		{
			ret |= opna_sfsave(&g_opna[i], sfh, tbl);
		}
	}
	if (nSaveFlags & FLAG_PCM86)
	{
		ret |= statflag_write(sfh, &g_pcm86, sizeof(g_pcm86));
	}
	if (nSaveFlags & FLAG_CS4231)
	{
		ret |= statflag_write(sfh, &cs4231, sizeof(cs4231));
	}
	if (nSaveFlags & FLAG_AMD98)
	{
		ret |= amd98_sfsave(sfh, tbl);
	}
	if (nSaveFlags & FLAG_OPL3)
	{
		ret |= opl3_sfsave(&g_opl3, sfh, tbl);
	}
	return ret;
}

static int flagload_fm(STFLAGH sfh, const SFENTRY *tbl)
{
	int ret;
	SOUNDID nSoundID;
	UINT nSaveFlags;
	UINT i;

	ret = statflag_read(sfh, &nSoundID, sizeof(nSoundID));
	fmboard_reset(&np2cfg, nSoundID);

	nSaveFlags = GetSoundFlags(g_nSoundID);
	if (nSaveFlags & FLAG_MG)
	{
		ret |= statflag_read(sfh, &g_musicgen, sizeof(g_musicgen));
		board14_allkeymake();
	}
	for (i = 0; i < NELEMENTS(g_opna); i++)
	{
		if (nSaveFlags & (FLAG_OPNA1 << i))
		{
			ret |= opna_sfload(&g_opna[i], sfh, tbl);
		}
	}
	if (nSaveFlags & FLAG_PCM86)
	{
		ret |= statflag_read(sfh, &g_pcm86, sizeof(g_pcm86));
	}
	if (nSaveFlags & FLAG_CS4231)
	{
		ret |= statflag_read(sfh, &cs4231, sizeof(cs4231));
	}
	if (nSaveFlags & FLAG_AMD98)
	{
		ret |= amd98_sfload(sfh, tbl);
	}
	if (nSaveFlags & FLAG_OPL3)
	{
		ret |= opl3_sfload(&g_opl3, sfh, tbl);
	}

	// �����B ����ړ����邱�ƁI
	pcm86gen_update();
	if (nSaveFlags & FLAG_PCM86)
	{
		fmboard_extenable((REG8)(g_pcm86.cSoundFlags & 1));
	}
	if (nSaveFlags & FLAG_CS4231)
	{
		fmboard_extenable((REG8)(cs4231.extfunc & 1));
	}
	return(ret);
}
#endif


// ---- fdd

static const OEMCHAR str_fddx[] = OEMTEXT("FDD%u");

static int flagsave_fdd(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	UINT8		i;
const OEMCHAR	*path;
	UINT		ftype;
	int			ro;

	ret = STATFLAG_SUCCESS;
	for (i=0; i<4; i++) {
		path = fdd_getfileex(i, &ftype, &ro);
		ret |= statflag_writepath(sfh, path, ftype, ro);
	}
	(void)tbl;
	return(ret);
}

static int flagcheck_fdd(STFLAGH sfh, const SFENTRY *tbl) {

	int		ret;
	int		i;
	OEMCHAR	buf[8];

	ret = STATFLAG_SUCCESS;
	for (i=0; i<4; i++) {
		OEMSPRINTF(buf, str_fddx, i+1);
		ret |= statflag_checkpath(sfh, buf);
	}
	(void)tbl;
	return(ret);
}

static int flagload_fdd(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	UINT8		i;
	STATPATH	sp;

	ret = STATFLAG_SUCCESS;
	for (i=0; i<4; i++) {
		memset(&sp, 0, sizeof(sp));
		ret |= statflag_read(sfh, &sp, sizeof(sp));
		if (sp.path[0]) {
			fdd_set(i, sp.path, sp.ftype, sp.readonly);
		}
	}
	(void)tbl;
	return(ret);
}


// ---- sxsi

typedef struct {
	UINT8	ide[4];
	UINT8	scsi[8];
} SXSIDEVS;

static const OEMCHAR str_sasix[] = OEMTEXT("SASI%u");
static const OEMCHAR str_scsix[] = OEMTEXT("SCSI%u");

static int flagsave_sxsi(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	UINT		i;
	SXSIDEVS	sds;
const OEMCHAR	*path;

	sxsi_allflash();
	ret = STATFLAG_SUCCESS;
	for (i=0; i<NELEMENTS(sds.ide); i++) {
		sds.ide[i] = sxsi_getdevtype((REG8)i);
	}
	for (i=0; i<NELEMENTS(sds.scsi); i++) {
		sds.scsi[i] = sxsi_getdevtype((REG8)(i + 0x20));
	}
	ret = statflag_write(sfh, &sds, sizeof(sds));
	for (i=0; i<NELEMENTS(sds.ide); i++) {
		if (sds.ide[i] != SXSIDEV_NC) {
			path = sxsi_getfilename((REG8)i);
			ret |= statflag_writepath(sfh, path, FTYPE_NONE, 0);
		}
	}
	for (i=0; i<NELEMENTS(sds.scsi); i++) {
		if (sds.scsi[i] != SXSIDEV_NC) {
			path = sxsi_getfilename((REG8)(i + 0x20));
			ret |= statflag_writepath(sfh, path, FTYPE_NONE, 0);
		}
	}
	(void)tbl;
	return(ret);
}

static int flagcheck_sxsi(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	SXSIDEVS	sds;
	UINT		i;
	OEMCHAR		buf[8];

	sxsi_allflash();
	ret = statflag_read(sfh, &sds, sizeof(sds));
	for (i=0; i<NELEMENTS(sds.ide); i++) {
		if (sds.ide[i] != SXSIDEV_NC) {
			OEMSPRINTF(buf, str_sasix, i+1);
			ret |= statflag_checkpath(sfh, buf);
		}
	}
	for (i=0; i<NELEMENTS(sds.scsi); i++) {
		if (sds.scsi[i] != SXSIDEV_NC) {
			OEMSPRINTF(buf, str_scsix, i);
			ret |= statflag_checkpath(sfh, buf);
		}
	}
	(void)tbl;
	return(ret);
}

static int flagload_sxsi(STFLAGH sfh, const SFENTRY *tbl) {

	int			ret;
	SXSIDEVS	sds;
	UINT		i;
	REG8		drv;
	STATPATH	sp;

	ret = statflag_read(sfh, &sds, sizeof(sds));
	if (ret != STATFLAG_SUCCESS) {
		return(ret);
	}
	for (i=0; i<NELEMENTS(sds.ide); i++) {
		drv = (REG8)i;
		sxsi_setdevtype(drv, sds.ide[i]);
		if (sds.ide[i] != SXSIDEV_NC) {
			ret |= statflag_read(sfh, &sp, sizeof(sp));
			sxsi_devopen(drv, sp.path);
		}
	}
	for (i=0; i<NELEMENTS(sds.scsi); i++) {
		drv = (REG8)(i + 0x20);
		sxsi_setdevtype(drv, sds.scsi[i]);
		if (sds.scsi[i] != SXSIDEV_NC) {
			ret |= statflag_read(sfh, &sp, sizeof(sp));
			sxsi_devopen(drv, sp.path);
		}
	}
	(void)tbl;
	return(ret);
}


// ---- com

static int flagsave_com(STFLAGH sfh, const SFENTRY *tbl) {

	UINT	device;
	COMMNG	cm;
	int		ret;
	COMFLAG	flag;

	device = (UINT)(INTPTR)tbl->arg1;
	switch(device) {
		case 0:
			cm = cm_mpu98;
			break;

		case 1:
			cm = cm_rs232c;
			break;

		default:
			cm = NULL;
			break;
	}
	ret = STATFLAG_SUCCESS;
	if (cm) {
		flag = (COMFLAG)cm->msg(cm, COMMSG_GETFLAG, 0);
		if (flag) {
			ret |= statflag_write(sfh, flag, flag->size);
			_MFREE(flag);
		}
	}
	return(ret);
}

static int flagload_com(STFLAGH sfh, const SFENTRY *tbl) {

	UINT		device;
	COMMNG		cm;
	int			ret;
	_COMFLAG	fhdr;
	COMFLAG		flag;

	ret = statflag_read(sfh, &fhdr, sizeof(fhdr));
	if (ret != STATFLAG_SUCCESS) {
		goto flcom_err1;
	}
	if (fhdr.size < sizeof(fhdr)) {
		goto flcom_err1;
	}
	flag = (COMFLAG)_MALLOC(fhdr.size, "com stat flag");
	if (flag == NULL) {
		goto flcom_err1;
	}
	*flag = fhdr;
	ret |= statflag_read(sfh, flag + 1, fhdr.size - sizeof(fhdr));
	if (ret != STATFLAG_SUCCESS) {
		goto flcom_err2;
	}

	device = (UINT)(INTPTR)tbl->arg1;
	switch(device) {
		case 0:
			commng_destroy(cm_mpu98);
			cm = commng_create(COMCREATE_MPU98II);
			cm_mpu98 = cm;
			break;

		case 1:
			commng_destroy(cm_rs232c);
			cm = commng_create(COMCREATE_SERIAL);
			cm_rs232c = cm;
			break;

		default:
			cm = NULL;
			break;
	}
	if (cm) {
		cm->msg(cm, COMMSG_SETFLAG, (INTPTR)flag);
	}

flcom_err2:
	_MFREE(flag);

flcom_err1:
	return(ret);
}


// ----

static int flagcheck_versize(STFLAGH sfh, const SFENTRY *tbl) {

	if ((sfh->hdr.ver == tbl->ver) && (sfh->hdr.size == tbl->arg2)) {
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}

static int flagcheck_veronly(STFLAGH sfh, const SFENTRY *tbl) {

	if (sfh->hdr.ver == tbl->ver) {
		return(STATFLAG_SUCCESS);
	}
	return(STATFLAG_FAILURE);
}


// ----

int statsave_save(const OEMCHAR *filename) {

	SFFILEH		sffh;
	int			ret;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_create(filename);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	SNDCSEC_ENTER;

	ret = STATFLAG_SUCCESS;
	tbl = np2tbl;
	tblterm = tbl + NELEMENTS(np2tbl);
	while(tbl < tblterm) {
		ret |= statflag_createsection(sffh, tbl);
		switch(tbl->type) {
			case STATFLAG_BIN:
			case STATFLAG_TERM:
				ret |= flagsave_common(&sffh->sfh, tbl);
				break;

			case STATFLAG_COM:
				ret |= flagsave_com(&sffh->sfh, tbl);
				break;

			case STATFLAG_DMA:
				ret |= flagsave_dma(&sffh->sfh, tbl);
				break;

			case STATFLAG_EGC:
				ret |= flagsave_egc(&sffh->sfh, tbl);
				break;

			case STATFLAG_EPSON:
				ret |= flagsave_epson(&sffh->sfh, tbl);
				break;

			case STATFLAG_EVT:
				ret |= flagsave_evt(&sffh->sfh, tbl);
				break;

			case STATFLAG_EXT:
				ret |= flagsave_ext(&sffh->sfh, tbl);
				break;

			case STATFLAG_FDD:
				ret |= flagsave_fdd(&sffh->sfh, tbl);
				break;

#if !defined(DISABLE_SOUND)
			case STATFLAG_FM:
				ret |= flagsave_fm(&sffh->sfh, tbl);
				break;
#endif

			case STATFLAG_GIJ:
				ret |= flagsave_gij(&sffh->sfh, tbl);
				break;

#if defined(SUPPORT_HOSTDRV)
				case STATFLAG_HDRV:
				ret |= hostdrv_sfsave(&sffh->sfh, tbl);
				break;
#endif

			case STATFLAG_MEM:
				ret |= flagsave_mem(&sffh->sfh, tbl);
				break;

			case STATFLAG_SXSI:
				ret |= flagsave_sxsi(&sffh->sfh, tbl);
				break;
		}
		tbl++;
	}

	SNDCSEC_LEAVE;

	statflag_close(sffh);
	return(ret);
}

int statsave_check(const OEMCHAR *filename, OEMCHAR *buf, int size) {

	SFFILEH		sffh;
	int			ret;
	BOOL		done;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_open(filename, buf, size);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	done = FALSE;
	ret = STATFLAG_SUCCESS;
	while((!done) && (ret != STATFLAG_FAILURE)) {
		ret |= statflag_readsection(sffh);
		tbl = np2tbl;
		tblterm = tbl + NELEMENTS(np2tbl);
		while(tbl < tblterm) {
			if (!memcmp(sffh->sfh.hdr.index, tbl->index, sizeof(sffh->sfh.hdr.index))) {
				break;
			}
			tbl++;
		}
		if (tbl < tblterm) {
			switch(tbl->type) {
				case STATFLAG_BIN:
				case STATFLAG_MEM:
					ret |= flagcheck_versize(&sffh->sfh, tbl);
					break;

				case STATFLAG_TERM:
					done = TRUE;
					break;

				case STATFLAG_COM:
				case STATFLAG_DMA:
				case STATFLAG_EGC:
				case STATFLAG_EPSON:
				case STATFLAG_EVT:
				case STATFLAG_EXT:
				case STATFLAG_GIJ:
#if !defined(DISABLE_SOUND)
				case STATFLAG_FM:
#endif
#if defined(SUPPORT_HOSTDRV)
				case STATFLAG_HDRV:
#endif
					ret |= flagcheck_veronly(&sffh->sfh, tbl);
					break;

				case STATFLAG_FDD:
					ret |= flagcheck_fdd(&sffh->sfh, tbl);
					break;

				case STATFLAG_SXSI:
					ret |= flagcheck_sxsi(&sffh->sfh, tbl);
					break;

				default:
					ret |= STATFLAG_WARNING;
					break;
			}
		}
		else {
			ret |= STATFLAG_WARNING;
		}
	}
	statflag_close(sffh);
	return(ret);
}

int statsave_load(const OEMCHAR *filename) {

	SFFILEH		sffh;
	int			ret;
	BOOL		done;
const SFENTRY	*tbl;
const SFENTRY	*tblterm;

	sffh = statflag_open(filename, NULL, 0);
	if (sffh == NULL) {
		return(STATFLAG_FAILURE);
	}

	// PCCORE read!
	ret = statflag_readsection(sffh);
	if ((ret != STATFLAG_SUCCESS) ||
		(memcmp(sffh->sfh.hdr.index, np2tbl[0].index, sizeof(sffh->sfh.hdr.index)))) {
		statflag_close(sffh);
		return(STATFLAG_FAILURE);
	}

	SNDCSEC_ENTER;

	soundmng_stop();
	rs232c_midipanic();
	mpu98ii_midipanic();
	pc9861k_midipanic();
	sxsi_alltrash();

	ret |= flagload_common(&sffh->sfh, np2tbl);

	CPU_RESET();
	CPU_SETEXTSIZE((UINT32)pccore.extmem);
	nevent_allreset();

	sound_changeclock();
	beep_changeclock();
	sound_reset();
	fddmtrsnd_bind();

	iocore_reset(&np2cfg);							// �T�E���h��pic���ĂԂ̂Łc
	cbuscore_reset(&np2cfg);
	fmboard_reset(&np2cfg, pccore.sound);

	done = FALSE;
	while((!done) && (ret != STATFLAG_FAILURE)) {
		ret |= statflag_readsection(sffh);
		tbl = np2tbl + 1;
		tblterm = np2tbl + NELEMENTS(np2tbl);
		while(tbl < tblterm) {
			if (!memcmp(sffh->sfh.hdr.index, tbl->index, sizeof(sffh->sfh.hdr.index))) {
				break;
			}
			tbl++;
		}
		if (tbl < tblterm) {
			switch(tbl->type) {
				case STATFLAG_BIN:
					ret |= flagload_common(&sffh->sfh, tbl);
					break;

				case STATFLAG_TERM:
					done = TRUE;
					break;

				case STATFLAG_COM:
					ret |= flagload_com(&sffh->sfh, tbl);
					break;

				case STATFLAG_DMA:
					ret |= flagload_dma(&sffh->sfh, tbl);
					break;

				case STATFLAG_EGC:
					ret |= flagload_egc(&sffh->sfh, tbl);
					break;

				case STATFLAG_EPSON:
					ret |= flagload_epson(&sffh->sfh, tbl);
					break;

				case STATFLAG_EVT:
					ret |= flagload_evt(&sffh->sfh, tbl);
					break;

				case STATFLAG_EXT:
					ret |= flagload_ext(&sffh->sfh, tbl);
					break;

				case STATFLAG_FDD:
					ret |= flagload_fdd(&sffh->sfh, tbl);
					break;

#if !defined(DISABLE_SOUND)
				case STATFLAG_FM:
					ret |= flagload_fm(&sffh->sfh, tbl);
					break;
#endif

				case STATFLAG_GIJ:
					ret |= flagload_gij(&sffh->sfh, tbl);
					break;

#if defined(SUPPORT_HOSTDRV)
				case STATFLAG_HDRV:
					ret |= hostdrv_sfload(&sffh->sfh, tbl);
					break;
#endif

				case STATFLAG_MEM:
					ret |= flagload_mem(&sffh->sfh, tbl);
					break;

				case STATFLAG_SXSI:
					ret |= flagload_sxsi(&sffh->sfh, tbl);
					break;

				default:
					ret |= STATFLAG_WARNING;
					break;
			}
		}
		else {
			ret |= STATFLAG_WARNING;
		}
	}
	statflag_close(sffh);

	// I/O��蒼��
	MEMM_ARCH((pccore.model & PCMODEL_EPSON)?1:0);
	iocore_build();
	iocore_bind();
	cbuscore_bind();
	fmboard_bind();

	gdcs.textdisp |= GDCSCRN_EXT;
	gdcs.textdisp |= GDCSCRN_ALLDRAW2;
	gdcs.grphdisp |= GDCSCRN_EXT;
	gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
	gdcs.palchange = GDCSCRN_REDRAW;
	tramflag.renewal = 1;
	cgwindow.writable |= 0x80;
#if defined(CPUSTRUC_FONTPTR)
	FONTPTR_LOW = fontrom + cgwindow.low;
	FONTPTR_HIGH = fontrom + cgwindow.high;
#endif
	MEMM_VRAM(vramop.operate);
	fddmtr_reset();
	soundmng_play();

	SNDCSEC_LEAVE;

	return(ret);
}

