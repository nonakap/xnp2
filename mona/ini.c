#include	"compiler.h"
#include	"strres.h"
#include	"profile.h"
#include	"np2.h"
#include	"dosio.h"
#include	"ini.h"
#include	"pccore.h"


typedef struct {
const char		*title;
const INITBL	*tbl;
const INITBL	*tblterm;
	UINT		count;
} _INIARG, *INIARG;

static void inirdarg8(UINT8 *dst, int dsize, const char *src) {

	int		i;
	UINT8	val;
	BOOL	set;
	char	c;

	for (i=0; i<dsize; i++) {
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
		dst[i] = val;
	}
}

static BOOL inireadcb(void *arg, const char *para,
										const char *key, const char *data) {

const INITBL	*p;

	if (arg == NULL) {
		return(FAILURE);
	}
	if (milstr_cmp(para, ((INIARG)arg)->title)) {
		return(SUCCESS);
	}
	p = ((INIARG)arg)->tbl;
	while(p < ((INIARG)arg)->tblterm) {
		if (!milstr_cmp(key, p->item)) {
			switch(p->itemtype) {
				case INITYPE_STR:
					milstr_ncpy((char *)p->value, data, p->size);
					break;

				case INITYPE_BOOL:
					*((UINT8 *)p->value) = (!milstr_cmp(data, str_true))?1:0;
					break;

				case INITYPE_BYTEARG:
					inirdarg8((UINT8 *)p->value, p->size, data);
					break;

				case INITYPE_SINT8:
				case INITYPE_UINT8:
					*((UINT8 *)p->value) = (UINT8)milstr_solveINT(data);
					break;

				case INITYPE_SINT16:
				case INITYPE_UINT16:
					*((UINT16 *)p->value) = (UINT16)milstr_solveINT(data);
					break;

				case INITYPE_SINT32:
				case INITYPE_UINT32:
					*((UINT32 *)p->value) = (UINT32)milstr_solveINT(data);
					break;

				case INITYPE_HEX8:
					*((UINT8 *)p->value) = (UINT8)milstr_solveHEX(data);
					break;

				case INITYPE_HEX16:
					*((UINT16 *)p->value) = (UINT16)milstr_solveHEX(data);
					break;

				case INITYPE_HEX32:
					*((UINT32 *)p->value) = (UINT32)milstr_solveHEX(data);
					break;
			}
		}
		p++;
	}
	return(SUCCESS);
}

void ini_read(const char *path, const char *title,
											const INITBL *tbl, UINT count) {

	_INIARG	iniarg;

	if (path == NULL) {
		return;
	}
	iniarg.title = title;
	iniarg.tbl = tbl;
	iniarg.tblterm = tbl + count;
	profile_enum(path, &iniarg, inireadcb);
}


// ----

static void iniwrsetstr(char *work, int size, const char *ptr) {

	int		i;
	char	c;

	if (ptr[0] == ' ') {
		goto iwss_extend;
		
	}
	i = strlen(ptr);
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

static void iniwrsetarg8(char *work, int size, const UINT8 *ptr, int arg) {

	int		i;
	char	tmp[8];

	if (arg > 0) {
		SPRINTF(tmp, "%.2x", ptr[0]);
		milstr_ncpy(work, tmp, size);
	}
	for (i=1; i<arg; i++) {
		SPRINTF(tmp, " %.2x", ptr[i]);
		milstr_ncat(work, tmp, size);
	}
}

void ini_write(const char *path, const char *title,
											const INITBL *tbl, UINT count) {

	FILEH		fh;
const INITBL	*p;
const INITBL	*pterm;
	BOOL		set;
	char		work[512];

	fh = file_create(path);
	if (fh == FILEH_INVALID) {
		return;
	}
	milstr_ncpy(work, "[", sizeof(work));
	milstr_ncat(work, title, sizeof(work));
	milstr_ncat(work, "]\r\n", sizeof(work));
	file_write(fh, work, strlen(work));

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		work[0] = '\0';
		set = SUCCESS;
		switch(p->itemtype) {
			case INITYPE_STR:
				iniwrsetstr(work, sizeof(work), (char *)p->value);
				break;

			case INITYPE_BOOL:
				milstr_ncpy(work, (*((UINT8 *)p->value))?str_true:str_false,
																sizeof(work));
				break;

			case INITYPE_BYTEARG:
				iniwrsetarg8(work, sizeof(work), (UINT8 *)p->value, p->size);
				break;

			case INITYPE_SINT8:
				SPRINTF(work, str_d, *((char *)p->value));
				break;

			case INITYPE_SINT16:
				SPRINTF(work, str_d, *((SINT16 *)p->value));
				break;

			case INITYPE_SINT32:
				SPRINTF(work, str_d, *((SINT32 *)p->value));
				break;

			case INITYPE_UINT8:
				SPRINTF(work, str_u, *((UINT8 *)p->value));
				break;

			case INITYPE_UINT16:
				SPRINTF(work, str_u, *((UINT16 *)p->value));
				break;

			case INITYPE_UINT32:
				SPRINTF(work, str_u, *((UINT32 *)p->value));
				break;

			case INITYPE_HEX8:
				SPRINTF(work, str_x, *((UINT8 *)p->value));
				break;

			case INITYPE_HEX16:
				SPRINTF(work, str_x, *((UINT16 *)p->value));
				break;

			case INITYPE_HEX32:
				SPRINTF(work, str_x, *((UINT32 *)p->value));
				break;

			default:
				set = FAILURE;
				break;
		}
		if (set == SUCCESS) {
			file_write(fh, p->item, strlen(p->item));
			file_write(fh, "=", 1);
			file_write(fh, work, strlen(work));
			file_write(fh, "\r\n", 2);
		}
		p++;
	}
	file_close(fh);
}


// ----

static const char ini_title[] = "NekoProjectII";
static const char inifile[] = "np2.cfg";

static const INITBL iniitem[] = {
	{"pc_model", INITYPE_STR,		np2cfg.model,
													NELEMENTS(np2cfg.model)},
	{"clk_base", INITYPE_SINT32,	&np2cfg.baseclock,		0},
	{"clk_mult", INITYPE_SINT32,	&np2cfg.multiple,		0},

	{"DIPswtch", INITYPE_BYTEARG,	np2cfg.dipsw,			3},
	{"MEMswtch", INITYPE_BYTEARG,	np2cfg.memsw,			8},
	{"ExMemory", INITYPE_UINT8,		&np2cfg.EXTMEM,			0},
	{"ITF_WORK", INITYPE_BOOL,		&np2cfg.ITF_WORK,		0},

	{"HDD1FILE", INITYPE_STR,		np2cfg.sasihdd[0],		MAX_PATH},
	{"HDD2FILE", INITYPE_STR,		np2cfg.sasihdd[1],		MAX_PATH},
#if defined(SUPPORT_SCSI)
	{"SCSIHDD0", INITYPE_STR,		np2cfg.scsihdd[0],		MAX_PATH},
	{"SCSIHDD1", INITYPE_STR,		np2cfg.scsihdd[1],		MAX_PATH},
	{"SCSIHDD2", INITYPE_STR,		np2cfg.scsihdd[2],		MAX_PATH},
	{"SCSIHDD3", INITYPE_STR,		np2cfg.scsihdd[3],		MAX_PATH},
#endif
	{"fontfile", INITYPE_STR,		np2cfg.fontfile,		MAX_PATH},
	{"biospath", INITYPE_STR,		np2cfg.biospath,		MAX_PATH},

	{"SampleHz", INITYPE_UINT16,	&np2cfg.samplingrate,	0},
	{"Latencys", INITYPE_UINT16,	&np2cfg.delayms,		0},
	{"SNDboard", INITYPE_HEX8,		&np2cfg.SOUND_SW,		0},
	{"BEEP_vol", INITYPE_UINT8,		&np2cfg.BEEP_VOL,		0},
	{"xspeaker", INITYPE_BOOL,		&np2cfg.snd_x,			0},

	{"SND14vol", INITYPE_BYTEARG,	np2cfg.vol14,			6},
//	{"opt14BRD", INITYPE_BYTEARG,	np2cfg.snd14opt,		3},
	{"opt26BRD", INITYPE_HEX8,		&np2cfg.snd26opt,		0},
	{"opt86BRD", INITYPE_HEX8,		&np2cfg.snd86opt,		0},
	{"optSPBRD", INITYPE_HEX8,		&np2cfg.spbopt,			0},
	{"optSPBVR", INITYPE_HEX8,		&np2cfg.spb_vrc,		0},
	{"optSPBVL", INITYPE_UINT8,		&np2cfg.spb_vrl,		0},
	{"optSPB_X", INITYPE_BOOL,		&np2cfg.spb_x,			0},
	{"optMPU98", INITYPE_HEX8,		&np2cfg.mpuopt,			0},

	{"volume_F", INITYPE_UINT8,		&np2cfg.vol_fm,			0},
	{"volume_S", INITYPE_UINT8,		&np2cfg.vol_ssg,		0},
	{"volume_A", INITYPE_UINT8,		&np2cfg.vol_adpcm,		0},
	{"volume_P", INITYPE_UINT8,		&np2cfg.vol_pcm,		0},
	{"volume_R", INITYPE_UINT8,		&np2cfg.vol_rhythm,		0},

	{"Seek_Snd", INITYPE_BOOL,		&np2cfg.MOTOR,			0},
	{"Seek_Vol", INITYPE_UINT8,		&np2cfg.MOTORVOL,		0},

	{"btnRAPID", INITYPE_BOOL,		&np2cfg.BTN_RAPID,		0},
	{"btn_MODE", INITYPE_BOOL,		&np2cfg.BTN_MODE,		0},
	{"MS_RAPID", INITYPE_BOOL,		&np2cfg.MOUSERAPID,		0},

	{"VRAMwait", INITYPE_BYTEARG,	np2cfg.wait,			6},
	{"DispSync", INITYPE_BOOL,		&np2cfg.DISPSYNC,		0},
	{"Real_Pal", INITYPE_BOOL,		&np2cfg.RASTER,			0},
	{"RPal_tim", INITYPE_UINT8,		&np2cfg.realpal,		0},
	{"uPD72020", INITYPE_BOOL,		&np2cfg.uPD72020,		0},
	{"GRCG_EGC", INITYPE_UINT8,		&np2cfg.grcg,			0},
	{"color16b", INITYPE_BOOL,		&np2cfg.color16,		0},
	{"skipline", INITYPE_BOOL,		&np2cfg.skipline,		0},
	{"skplight", INITYPE_SINT16,	&np2cfg.skiplight,		0},
	{"LCD_MODE", INITYPE_UINT8,		&np2cfg.LCD_MODE,		0},
	{"BG_COLOR", INITYPE_HEX32,		&np2cfg.BG_COLOR,		0},
	{"FG_COLOR", INITYPE_HEX32,		&np2cfg.FG_COLOR,		0},
	{"pc9861_e", INITYPE_BOOL,		&np2cfg.pc9861enable,	0},
	{"pc9861_s", INITYPE_BYTEARG,	np2cfg.pc9861sw,		3},
	{"pc9861_j", INITYPE_BYTEARG,	np2cfg.pc9861jmp,		6},
	{"calendar", INITYPE_BOOL,		&np2cfg.calendar,		0},
	{"USE144FD", INITYPE_BOOL,		&np2cfg.usefd144,		0},

	// OSàÀë∂Å`
	{"s_NOWAIT", INITYPE_BOOL,		&np2oscfg.NOWAIT,		0},
	{"SkpFrame", INITYPE_UINT8,		&np2oscfg.DRAW_SKIP,	0},
	{"F12_bind", INITYPE_UINT8,		&np2oscfg.F12KEY,		0},
	{"e_resume", INITYPE_BOOL,		&np2oscfg.resume,		0},
	{"jast_snd", INITYPE_BOOL,		&np2oscfg.jastsnd,		0},		// ver0.73
};

#define	INIITEMS	(sizeof(iniitem) / sizeof(INITBL))


void initload(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), NELEMENTS(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void initsave(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), NELEMENTS(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

