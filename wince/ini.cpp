#include	"compiler.h"
#include	"strres.h"
#include	"textfile.h"
#include	"np2.h"
#include	"dosio.h"
#include	"ini.h"
#include	"pccore.h"


static void binset(UINT8 *bin, UINT binlen, const OEMCHAR *src) {

	UINT	i;
	UINT8	val;
	BOOL	set;
	OEMCHAR	c;

	for (i=0; i<binlen; i++) {
		val = 0;
		set = FALSE;
		while(*src == ' ') {
			src++;
		}
		while(1) {
			c = *src;
			if ((c == '\0') || (c == ' ')) {
				break;
			}
			else if ((c >= '0') && (c <= '9')) {
				val <<= 4;
				val += c - '0';
				set = TRUE;
			}
			else {
				c |= 0x20;
				if ((c >= 'a') && (c <= 'f')) {
					val <<= 4;
					val += c - 'a' + 10;
					set = TRUE;
				}
			}
			src++;
		}
		if (set == FALSE) {
			break;
		}
		bin[i] = val;
	}
}

static void binget(OEMCHAR *work, int size, const UINT8 *bin, UINT binlen) {

	UINT	i;
	OEMCHAR	tmp[8];

	if (binlen) {
		OEMSPRINTF(tmp, OEMTEXT("%.2x"), bin[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<binlen; i++) {
		OEMSPRINTF(tmp, OEMTEXT(" %.2x"), bin[i]);
		milstr_ncat(work, tmp, size);
	}
}


// ----

typedef struct {
const OEMCHAR	*title;
const PFTBL		*tbl;
const PFTBL		*tblterm;
	UINT		count;
} _PFARG, *PFARG;

static BOOL inireadcb(void *arg, const OEMCHAR *para,
									const OEMCHAR *key, const OEMCHAR *data) {

const PFTBL	*p;

	if (arg == NULL) {
		return(FAILURE);
	}
	if (milstr_cmp(para, ((PFARG)arg)->title)) {
		return(SUCCESS);
	}
	p = ((PFARG)arg)->tbl;
	while(p < ((PFARG)arg)->tblterm) {
		if (!milstr_cmp(key, p->item)) {
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					milstr_ncpy((OEMCHAR *)p->value, data, p->arg);
					break;

				case PFTYPE_BOOL:
					*((UINT8 *)p->value) = (!milstr_cmp(data, str_true))?1:0;
					break;

				case PFTYPE_BIN:
					binset((UINT8 *)p->value, p->arg, data);
					break;

				case PFTYPE_SINT8:
				case PFTYPE_UINT8:
					*((UINT8 *)p->value) = (UINT8)milstr_solveINT(data);
					break;

				case PFTYPE_SINT16:
				case PFTYPE_UINT16:
					*((UINT16 *)p->value) = (UINT16)milstr_solveINT(data);
					break;

				case PFTYPE_SINT32:
				case PFTYPE_UINT32:
					*((UINT32 *)p->value) = (UINT32)milstr_solveINT(data);
					break;

				case PFTYPE_HEX8:
					*((UINT8 *)p->value) = (UINT8)milstr_solveHEX(data);
					break;

				case PFTYPE_HEX16:
					*((UINT16 *)p->value) = (UINT16)milstr_solveHEX(data);
					break;

				case PFTYPE_HEX32:
					*((UINT32 *)p->value) = (UINT32)milstr_solveHEX(data);
					break;
			}
		}
		p++;
	}
	return(SUCCESS);
}

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

	_PFARG	iniarg;

	if (path == NULL) {
		return;
	}
	iniarg.title = title;
	iniarg.tbl = tbl;
	iniarg.tblterm = tbl + count;
	profile_enum(path, &iniarg, inireadcb);
}


// ----

static void iniwrsetstr(OEMCHAR *work, int size, const OEMCHAR *ptr) {

	int		i;
	OEMCHAR	c;

	if (ptr[0] == ' ') {
		goto iwss_extend;
	}
	i = OEMSTRLEN(ptr);
	if ((i) && (ptr[i-1] == ' ')) {
		goto iwss_extend;
	}
	while(i > 0) {
		i--;
		if (ptr[i] == '\"') {
			goto iwss_extend;
		}
	}
	milstr_ncpy(work, ptr, size);
	return;

iwss_extend:
	if (size > 3) {
		size -= 3;
		*work++ = '\"';
		while(size > 0) {
			size--;
			c = *ptr++;
			if (c == '\"') {
				if (size > 0) {
					size--;
					work[0] = c;
					work[1] = c;
					work += 2;
				}
			}
			else {
				*work++ = c;
			}
		}
		work[0] = '\"';
		work[1] = '\0';
	}
}

void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

	TEXTFILEH	tfh;
const PFTBL		*p;
const PFTBL		*pterm;
	BOOL		set;
	OEMCHAR		work[512];

	tfh = textfile_create(path, 0x800);
	if (tfh == NULL) {
		return;
	}
	milstr_ncpy(work, OEMTEXT("["), NELEMENTS(work));
	milstr_ncat(work, title, NELEMENTS(work));
	milstr_ncat(work, OEMTEXT("]\r\n"), NELEMENTS(work));
	textfile_write(tfh, work);

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		work[0] = '\0';
		set = SUCCESS;
		switch(p->itemtype & PFTYPE_MASK) {
			case PFTYPE_STR:
				iniwrsetstr(work, NELEMENTS(work), (OEMCHAR *)p->value);
				break;

			case PFTYPE_BOOL:
				milstr_ncpy(work, (*((UINT8 *)p->value))?str_true:str_false,
															NELEMENTS(work));
				break;

			case PFTYPE_BIN:
				binget(work, NELEMENTS(work), (UINT8 *)p->value, p->arg);
				break;

			case PFTYPE_SINT8:
				OEMSPRINTF(work, str_d, *((SINT8 *)p->value));
				break;

			case PFTYPE_SINT16:
				OEMSPRINTF(work, str_d, *((SINT16 *)p->value));
				break;

			case PFTYPE_SINT32:
				OEMSPRINTF(work, str_d, *((SINT32 *)p->value));
				break;

			case PFTYPE_UINT8:
				OEMSPRINTF(work, str_u, *((UINT8 *)p->value));
				break;

			case PFTYPE_UINT16:
				OEMSPRINTF(work, str_u, *((UINT16 *)p->value));
				break;

			case PFTYPE_UINT32:
				OEMSPRINTF(work, str_u, *((UINT32 *)p->value));
				break;

			case PFTYPE_HEX8:
				OEMSPRINTF(work, str_x, *((UINT8 *)p->value));
				break;

			case PFTYPE_HEX16:
				OEMSPRINTF(work, str_x, *((UINT16 *)p->value));
				break;

			case PFTYPE_HEX32:
				OEMSPRINTF(work, str_x, *((UINT32 *)p->value));
				break;

			default:
				set = FAILURE;
				break;
		}
		if (set == SUCCESS) {
			textfile_write(tfh, p->item);
			textfile_write(tfh, OEMTEXT("="));
			textfile_write(tfh, work);
			textfile_write(tfh, OEMTEXT("\r\n"));
		}
		p++;
	}
	textfile_close(tfh);
}


// ----

#if defined(OSLANG_UTF8)
static const OEMCHAR ini_title[] = OEMTEXT("NekoProjectIICE");
static const OEMCHAR inifile[] = OEMTEXT("np2ce.cfg");
#else
static const OEMCHAR ini_title[] = OEMTEXT("NekoProjectII");
static const OEMCHAR inifile[] = OEMTEXT("np2.cfg");
#endif

enum {
	PFRO_STR		= PFFLAG_RO + PFTYPE_STR,
	PFRO_BOOL		= PFFLAG_RO + PFTYPE_BOOL,
	PFRO_HEX32		= PFFLAG_RO + PFTYPE_HEX32
};

static const PFTBL iniitem[] = {
	PFSTR("pc_model", PFTYPE_STR,		np2cfg.model),
	PFVAL("clk_base", PFTYPE_UINT32,	&np2cfg.baseclock),
	PFVAL("clk_mult", PFTYPE_UINT32,	&np2cfg.multiple),

	PFEXT("DIPswtch", PFTYPE_BIN,		np2cfg.dipsw,			3),
	PFEXT("MEMswtch", PFTYPE_BIN,		np2cfg.memsw,			8),
	PFMAX("ExMemory", PFTYPE_UINT8,		&np2cfg.EXTMEM,			63),
	PFVAL("ITF_WORK", PFRO_BOOL,		&np2cfg.ITF_WORK),

	PFSTR("HDD1FILE", PFTYPE_STR,		np2cfg.sasihdd[0]),
	PFSTR("HDD2FILE", PFTYPE_STR,		np2cfg.sasihdd[1]),
#if defined(SUPPORT_SCSI)
	PFSTR("SCSIHDD0", PFTYPE_STR,		np2cfg.scsihdd[0]),
	PFSTR("SCSIHDD1", PFTYPE_STR,		np2cfg.scsihdd[1]),
	PFSTR("SCSIHDD2", PFTYPE_STR,		np2cfg.scsihdd[2]),
	PFSTR("SCSIHDD3", PFTYPE_STR,		np2cfg.scsihdd[3]),
#endif
	PFSTR("fontfile", PFTYPE_STR,		np2cfg.fontfile),
	PFSTR("biospath", PFRO_STR,			np2cfg.biospath),

	PFVAL("SampleHz", PFTYPE_UINT16,	&np2cfg.samplingrate),
	PFVAL("Latencys", PFTYPE_UINT16,	&np2cfg.delayms),
	PFVAL("SNDboard", PFTYPE_HEX8,		&np2cfg.SOUND_SW),
	PFAND("BEEP_vol", PFTYPE_UINT8,		&np2cfg.BEEP_VOL,		3),
	PFVAL("xspeaker", PFRO_BOOL,		&np2cfg.snd_x),

	PFEXT("SND14vol", PFTYPE_BIN,		np2cfg.vol14,			6),
//	PFEXT("opt14BRD", PFTYPE_BIN,		np2cfg.snd14opt,		3),
	PFVAL("opt26BRD", PFTYPE_HEX8,		&np2cfg.snd26opt),
	PFVAL("opt86BRD", PFTYPE_HEX8,		&np2cfg.snd86opt),
	PFVAL("optSPBRD", PFTYPE_HEX8,		&np2cfg.spbopt),
	PFVAL("optSPBVR", PFTYPE_HEX8,		&np2cfg.spb_vrc),
	PFMAX("optSPBVL", PFTYPE_UINT8,		&np2cfg.spb_vrl,		24),
	PFVAL("optSPB_X", PFTYPE_BOOL,		&np2cfg.spb_x),
	PFVAL("optMPU98", PFTYPE_HEX8,		&np2cfg.mpuopt),

	PFMAX("volume_F", PFTYPE_UINT8,		&np2cfg.vol_fm,			128),
	PFMAX("volume_S", PFTYPE_UINT8,		&np2cfg.vol_ssg,		128),
	PFMAX("volume_A", PFTYPE_UINT8,		&np2cfg.vol_adpcm,		128),
	PFMAX("volume_P", PFTYPE_UINT8,		&np2cfg.vol_pcm,		128),
	PFMAX("volume_R", PFTYPE_UINT8,		&np2cfg.vol_rhythm,		128),

	PFVAL("Seek_Snd", PFTYPE_BOOL,		&np2cfg.MOTOR),
	PFMAX("Seek_Vol", PFTYPE_UINT8,		&np2cfg.MOTORVOL,		100),

	PFVAL("btnRAPID", PFTYPE_BOOL,		&np2cfg.BTN_RAPID),
	PFVAL("btn_MODE", PFTYPE_BOOL,		&np2cfg.BTN_MODE),
	PFVAL("MS_RAPID", PFTYPE_BOOL,		&np2cfg.MOUSERAPID),

	PFEXT("VRAMwait", PFTYPE_BIN,		np2cfg.wait,			6),
	PFVAL("DispSync", PFTYPE_BOOL,		&np2cfg.DISPSYNC),
	PFVAL("Real_Pal", PFTYPE_BOOL,		&np2cfg.RASTER),
	PFMAX("RPal_tim", PFTYPE_UINT8,		&np2cfg.realpal,		64),
	PFVAL("uPD72020", PFTYPE_BOOL,		&np2cfg.uPD72020),
	PFAND("GRCG_EGC", PFTYPE_UINT8,		&np2cfg.grcg,			3),
	PFVAL("color16b", PFTYPE_BOOL,		&np2cfg.color16),
	PFVAL("skipline", PFTYPE_BOOL,		&np2cfg.skipline),
	PFVAL("skplight", PFTYPE_UINT16,	&np2cfg.skiplight),
	PFAND("LCD_MODE", PFTYPE_UINT8,		&np2cfg.LCD_MODE,		0x03),
	PFAND("BG_COLOR", PFRO_HEX32,		&np2cfg.BG_COLOR,		0xffffff),
	PFAND("FG_COLOR", PFRO_HEX32,		&np2cfg.FG_COLOR,		0xffffff),

	PFVAL("pc9861_e", PFTYPE_BOOL,		&np2cfg.pc9861enable),
	PFEXT("pc9861_s", PFTYPE_BIN,		np2cfg.pc9861sw,		3),
	PFEXT("pc9861_j", PFTYPE_BIN,		np2cfg.pc9861jmp,		6),

	PFVAL("calendar", PFTYPE_BOOL,		&np2cfg.calendar),
	PFVAL("USE144FD", PFTYPE_BOOL,		&np2cfg.usefd144),

	// OSàÀë∂Å`
	PFVAL("s_NOWAIT", PFTYPE_BOOL,		&np2oscfg.NOWAIT),
	PFVAL("SkpFrame", PFTYPE_UINT8,		&np2oscfg.DRAW_SKIP),
	PFVAL("F12_bind", PFTYPE_UINT8,		&np2oscfg.F12KEY),
	PFVAL("e_resume", PFTYPE_BOOL,		&np2oscfg.resume),

#if !defined(GX_DLL)
	PFVAL("WindposX", PFTYPE_SINT32,	&np2oscfg.winx),
	PFVAL("WindposY", PFTYPE_SINT32,	&np2oscfg.winy),
#endif
#if defined(WIN32_PLATFORM_PSPC)
	PFVAL("pbindcur", PFTYPE_UINT8,		&np2oscfg.bindcur),
	PFVAL("pbindbtn", PFTYPE_UINT8,		&np2oscfg.bindbtn),
#endif
	PFVAL("jast_snd", PFTYPE_BOOL,		&np2oscfg.jastsnd),
};


void initload(void) {

	OEMCHAR	path[MAX_PATH];

	milstr_ncpy(path, file_getcd(inifile), NELEMENTS(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void initsave(void) {

	OEMCHAR	path[MAX_PATH];

	milstr_ncpy(path, file_getcd(inifile), NELEMENTS(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

