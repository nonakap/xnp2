#include	"compiler.h"
#include	<windowsx.h>
#include	<io.h>
#include	"strres.h"
#include	"profile.h"
#include	"np2.h"
#include	"np2arg.h"
#if defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif
#include	"dosio.h"
#include	"ini.h"
#include	"winkbd.h"
#include	"pccore.h"




// ---- user

static void inirdargs16(const OEMCHAR *src, const PFTBL *ini) {

	SINT16	*dst;
	int		dsize;
	int		i;
	OEMCHAR	c;

	dst = (SINT16 *)ini->value;
	dsize = ini->arg;

	for (i=0; i<dsize; i++) {
		while(*src == ' ') {
			src++;
		}
		if (*src == '\0') {
			break;
		}
		dst[i] = (SINT16)milstr_solveINT(src);
		while(*src != '\0') {
			c = *src++;
			if (c == ',') {
				break;
			}
		}
	}
}

static void inirdbyte3(const OEMCHAR *src, const PFTBL *ini) {

	UINT	i;

	for (i=0; i<3; i++) {
		if (src[i] == '\0') {
			break;
		}
		if ((((src[i] - '0') & 0xff) < 9) ||
			(((src[i] - 'A') & 0xdf) < 26)) {
			((UINT8 *)ini->value)[i] = (UINT8)src[i];
		}
	}
}

static void inirdkb(const OEMCHAR *src, const PFTBL *ini) {

	if ((!milstr_extendcmp(src, OEMTEXT("PC98"))) ||
		(!milstr_cmp(src, OEMTEXT("98")))) {
		*(UINT8 *)ini->value = KEY_PC98;
	}
	else if ((!milstr_extendcmp(src, OEMTEXT("DOS"))) ||
			(!milstr_cmp(src, OEMTEXT("PCAT"))) ||
			(!milstr_cmp(src, OEMTEXT("AT")))) {
		*(UINT8 *)ini->value = KEY_KEY106;
	}
	else if ((!milstr_extendcmp(src, OEMTEXT("KEY101"))) ||
			(!milstr_cmp(src, OEMTEXT("101")))) {
		*(UINT8 *)ini->value = KEY_KEY101;
	}
}


// ---- Use WinAPI

#if !defined(_UNICODE)
static void bitmapset(UINT8 *ptr, UINT pos, BOOL set) {

	UINT8	bit;

	ptr += (pos >> 3);
	bit = 1 << (pos & 7);
	if (set) {
		*ptr |= bit;
	}
	else {
		*ptr &= ~bit;
	}
}

static BOOL bitmapget(const UINT8 *ptr, UINT pos) {

	return((ptr[pos >> 3] >> (pos & 7)) & 1);
}

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

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

const PFTBL	*p;
const PFTBL	*pterm;
	OEMCHAR	work[512];
	UINT32	val;

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		switch(p->itemtype & PFTYPE_MASK) {
			case PFTYPE_STR:
				GetPrivateProfileString(title, p->item, (OEMCHAR *)p->value,
											(OEMCHAR *)p->value, p->arg, path);
				break;

			case PFTYPE_BOOL:
				GetPrivateProfileString(title, p->item,
									(*((UINT8 *)p->value))?str_true:str_false,
												work, NELEMENTS(work), path);
				*((UINT8 *)p->value) = (!milstr_cmp(work, str_true))?1:0;
				break;

			case PFTYPE_BITMAP:
				GetPrivateProfileString(title, p->item,
					(bitmapget((UINT8 *)p->value, p->arg))?str_true:str_false,
												work, NELEMENTS(work), path);
				bitmapset((UINT8 *)p->value, p->arg,
										(milstr_cmp(work, str_true) == 0));
				break;

			case PFTYPE_BIN:
				GetPrivateProfileString(title, p->item, str_null,
												work, NELEMENTS(work), path);
				binset((UINT8 *)p->value, p->arg, work);
				break;

			case PFTYPE_SINT8:
			case PFTYPE_UINT8:
				val = (UINT8)GetPrivateProfileInt(title, p->item,
												*(UINT8 *)p->value, path);
				*(UINT8 *)p->value = (UINT8)val;
				break;

			case PFTYPE_SINT16:
			case PFTYPE_UINT16:
				val = (UINT16)GetPrivateProfileInt(title, p->item,
												*(UINT16 *)p->value, path);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case PFTYPE_SINT32:
			case PFTYPE_UINT32:
				val = (UINT32)GetPrivateProfileInt(title, p->item,
												*(UINT32 *)p->value, path);
				*(UINT32 *)p->value = (UINT32)val;
				break;

			case PFTYPE_HEX8:
				OEMSPRINTF(work, str_x, *(UINT8 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, NELEMENTS(work), path);
				val = (UINT8)milstr_solveHEX(work);
				*(UINT8 *)p->value = (UINT8)val;
				break;

			case PFTYPE_HEX16:
				OEMSPRINTF(work, str_x, *(UINT16 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, NELEMENTS(work), path);
				val = (UINT16)milstr_solveHEX(work);
				*(UINT16 *)p->value = (UINT16)val;
				break;

			case PFTYPE_HEX32:
				OEMSPRINTF(work, str_x, *(UINT32 *)p->value),
				GetPrivateProfileString(title, p->item, work,
												work, NELEMENTS(work), path);
				val = (UINT32)milstr_solveHEX(work);
				*(UINT32 *)p->value = (UINT32)val;
				break;

			case PFTYPE_ARGS16:
				GetPrivateProfileString(title, p->item, str_null,
												work, NELEMENTS(work), path);
				inirdargs16(work, p);
				break;

			case PFTYPE_BYTE3:
				GetPrivateProfileString(title, p->item, str_null,
												work, NELEMENTS(work), path);
				inirdbyte3(work, p);
				break;

			case PFTYPE_KB:
				GetPrivateProfileString(title, p->item, str_null,
												work, NELEMENTS(work), path);
				inirdkb(work, p);
				break;
		}
		p++;
	}
}

void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

const PFTBL		*p;
const PFTBL		*pterm;
const OEMCHAR	*set;
	OEMCHAR		work[512];

	p = tbl;
	pterm = tbl + count;
	while(p < pterm) {
		if (!(p->itemtype & PFFLAG_RO)) {
			work[0] = '\0';
			set = work;
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					set = (OEMCHAR *)p->value;
					break;

				case PFTYPE_BOOL:
					set = (*((UINT8 *)p->value))?str_true:str_false;
					break;

				case PFTYPE_BITMAP:
					set = (bitmapget((UINT8 *)p->value, p->arg))?
														str_true:str_false;
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
					set = NULL;
					break;
			}
			if (set) {
				WritePrivateProfileString(title, p->item, set, path);
			}
		}
		p++;
	}
}

#else	// !defined(_UNICODE)

// ---- Use profile.c

static void pfread(const PFTBL *item, const OEMCHAR *string) {

	switch(item->itemtype & PFTYPE_MASK) {
		case PFTYPE_ARGS16:
			inirdargs16(string, item);
			break;

		case PFTYPE_BYTE3:
			inirdbyte3(string, item);
			break;

		case PFTYPE_KB:
			inirdkb(string, item);
			break;
	}
}

void ini_read(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

	profile_iniread(path, title, tbl, count, pfread);
}

void ini_write(const OEMCHAR *path, const OEMCHAR *title,
											const PFTBL *tbl, UINT count) {

	profile_iniwrite(path, title, tbl, count, NULL);
}

#endif	// !defined(_UNICODE)


// ----

#if !defined(SUPPORT_PC9821)
static const OEMCHAR ini_title[] = OEMTEXT("NekoProjectII");
#else
static const OEMCHAR ini_title[] = OEMTEXT("NekoProject21");
#endif

enum {
	PFRO_STR		= PFFLAG_RO + PFTYPE_STR,
	PFRO_BOOL		= PFFLAG_RO + PFTYPE_BOOL,
	PFRO_BITMAP		= PFFLAG_RO + PFTYPE_BITMAP,
	PFRO_UINT8		= PFFLAG_RO + PFTYPE_UINT8,
	PFRO_SINT32		= PFFLAG_RO + PFTYPE_SINT32,
	PFRO_HEX8		= PFFLAG_RO + PFTYPE_HEX8,
	PFRO_HEX32		= PFFLAG_RO + PFTYPE_HEX32,
	PFRO_BYTE3		= PFFLAG_RO + PFTYPE_BYTE3,
	PFRO_KB			= PFFLAG_RO + PFTYPE_KB
};

static const PFTBL iniitem[] = {
	PFSTR("np2title", PFRO_STR,			np2oscfg.titles),
	PFVAL("np2winid", PFRO_BYTE3,		np2oscfg.winid),
	PFVAL("WindposX", PFTYPE_SINT32,	&np2oscfg.winx),
	PFVAL("WindposY", PFTYPE_SINT32,	&np2oscfg.winy),
	PFMAX("paddingx", PFRO_SINT32,		&np2oscfg.paddingx,		32),
	PFMAX("paddingy", PFRO_SINT32,		&np2oscfg.paddingy,		32),
	PFVAL("Win_Snap", PFTYPE_BOOL,		&np2oscfg.WINSNAP),

	PFSTR("FDfolder", PFTYPE_STR,		fddfolder),
	PFSTR("HDfolder", PFTYPE_STR,		hddfolder),
	PFSTR("bmap_Dir", PFTYPE_STR,		bmpfilefolder),
	PFSTR("fontfile", PFTYPE_STR,		np2cfg.fontfile),
	PFSTR("biospath", PFRO_STR,			np2cfg.biospath),

#if defined(SUPPORT_HOSTDRV)
	PFSTR("hdrvroot", PFRO_STR,			np2cfg.hdrvroot),
	PFVAL("hdrv_acc", PFRO_UINT8,		&np2cfg.hdrvacc),
#endif

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
	PFVAL("Mouse_sw", PFTYPE_BOOL,		&np2oscfg.MOUSE_SW),
	PFVAL("MS_RAPID", PFTYPE_BOOL,		&np2cfg.MOUSERAPID),

	PFAND("backgrnd", PFTYPE_UINT8,		&np2oscfg.background,	3),
	PFEXT("VRAMwait", PFTYPE_BIN,		np2cfg.wait,			6),
	PFAND("DspClock", PFTYPE_UINT8,		&np2oscfg.DISPCLK,		3),
	PFVAL("DispSync", PFTYPE_BOOL,		&np2cfg.DISPSYNC),
	PFVAL("Real_Pal", PFTYPE_BOOL,		&np2cfg.RASTER),
	PFMAX("RPal_tim", PFTYPE_UINT8,		&np2cfg.realpal,		64),
	PFVAL("s_NOWAIT", PFTYPE_BOOL,		&np2oscfg.NOWAIT),
	PFVAL("SkpFrame", PFTYPE_UINT8,		&np2oscfg.DRAW_SKIP),
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
	PFEXT("FDDRIVE1", PFRO_BITMAP,		&np2cfg.fddequip,		0),
	PFEXT("FDDRIVE2", PFRO_BITMAP,		&np2cfg.fddequip,		1),
	PFEXT("FDDRIVE3", PFRO_BITMAP,		&np2cfg.fddequip,		2),
	PFEXT("FDDRIVE4", PFRO_BITMAP,		&np2cfg.fddequip,		3),

	// OSàÀë∂ÅH
	PFVAL("keyboard", PFRO_KB,			&np2oscfg.KEYBOARD),
	PFVAL("F12_COPY", PFTYPE_UINT8,		&np2oscfg.F12COPY),
	PFVAL("Joystick", PFTYPE_BOOL,		&np2oscfg.JOYPAD1),
	PFEXT("Joy1_btn", PFTYPE_BIN,		np2oscfg.JOY1BTN,		4),

	PFVAL("clocknow", PFTYPE_UINT8,		&np2oscfg.clk_x),
	PFVAL("clockfnt", PFTYPE_UINT8,		&np2oscfg.clk_fnt),
	PFAND("clock_up", PFRO_HEX32,		&np2oscfg.clk_color1,	0xffffff),
	PFAND("clock_dn", PFRO_HEX32,		&np2oscfg.clk_color2,	0xffffff),

	PFVAL("use_sstp", PFTYPE_BOOL,		&np2oscfg.sstp),
	PFVAL("sstpport", PFTYPE_UINT16,	&np2oscfg.sstpport),
	PFVAL("comfirm_", PFTYPE_BOOL,		&np2oscfg.comfirm),
	PFVAL("shortcut", PFTYPE_HEX8,		&np2oscfg.shortcut),

	PFSTR("mpu98map", PFTYPE_STR,		np2oscfg.mpu.mout),
	PFSTR("mpu98min", PFTYPE_STR,		np2oscfg.mpu.min),
	PFSTR("mpu98mdl", PFTYPE_STR,		np2oscfg.mpu.mdl),
	PFSTR("mpu98def", PFTYPE_STR,		np2oscfg.mpu.def),

	PFMAX("com1port", PFTYPE_UINT8,		&np2oscfg.com1.port,	5),
	PFVAL("com1para", PFTYPE_UINT8,		&np2oscfg.com1.param),
	PFVAL("com1_bps", PFTYPE_UINT32,	&np2oscfg.com1.speed),
	PFSTR("com1mmap", PFTYPE_STR,		np2oscfg.com1.mout),
	PFSTR("com1mmdl", PFTYPE_STR,		np2oscfg.com1.mdl),
	PFSTR("com1mdef", PFTYPE_STR,		np2oscfg.com1.def),

	PFMAX("com2port", PFTYPE_UINT8,		&np2oscfg.com2.port,	5),
	PFVAL("com2para", PFTYPE_UINT8,		&np2oscfg.com2.param),
	PFVAL("com2_bps", PFTYPE_UINT32,	&np2oscfg.com2.speed),
	PFSTR("com2mmap", PFTYPE_STR,		np2oscfg.com2.mout),
	PFSTR("com2mmdl", PFTYPE_STR,		np2oscfg.com2.mdl),
	PFSTR("com2mdef", PFTYPE_STR,		np2oscfg.com2.def),

	PFMAX("com3port", PFTYPE_UINT8,		&np2oscfg.com3.port,	5),
	PFVAL("com3para", PFTYPE_UINT8,		&np2oscfg.com3.param),
	PFVAL("com3_bps", PFTYPE_UINT32,	&np2oscfg.com3.speed),
	PFSTR("com3mmap", PFTYPE_STR,		np2oscfg.com3.mout),
	PFSTR("com3mmdl", PFTYPE_STR,		np2oscfg.com3.mdl),
	PFSTR("com3mdef", PFTYPE_STR,		np2oscfg.com3.def),

	PFVAL("force400", PFRO_BOOL,		&np2oscfg.force400),
	PFVAL("e_resume", PFTYPE_BOOL,		&np2oscfg.resume),
	PFVAL("STATSAVE", PFRO_BOOL,		&np2oscfg.statsave),
#if !defined(_WIN64)
	PFVAL("nousemmx", PFTYPE_BOOL,		&np2oscfg.disablemmx),
#endif
	PFVAL("windtype", PFTYPE_UINT8,		&np2oscfg.wintype),
	PFVAL("toolwind", PFTYPE_BOOL,		&np2oscfg.toolwin),
	PFVAL("keydispl", PFTYPE_BOOL,		&np2oscfg.keydisp),
	PFVAL("jast_snd", PFTYPE_BOOL,		&np2oscfg.jastsnd),
	PFVAL("useromeo", PFTYPE_BOOL,		&np2oscfg.useromeo),
	PFVAL("thickfrm", PFTYPE_BOOL,		&np2oscfg.thickframe),
	PFVAL("xrollkey", PFRO_BOOL,		&np2oscfg.xrollkey),
	PFVAL("fscrn_cx", PFRO_SINT32,		&np2oscfg.fscrn_cx),
	PFVAL("fscrn_cy", PFRO_SINT32,		&np2oscfg.fscrn_cy),
	PFVAL("fscrnbpp", PFRO_UINT8,		&np2oscfg.fscrnbpp),
	PFVAL("fscrnmod", PFTYPE_HEX8,		&np2oscfg.fscrnmod),
	PFVAL("I286SAVE", PFRO_BOOL,		&np2oscfg.I286SAVE)};


static const OEMCHAR ext_ini[] = OEMTEXT(".ini");

void initgetfile(OEMCHAR *path, UINT size) {

const OEMCHAR	*ext;

	file_cpyname(path, modulefile, size);
	if (np2arg.ini) {
		if ((np2arg.ini[0] == '/') || (np2arg.ini[0] == '-')) {
			file_cutname(path);
			file_catname(path, file_getname(np2arg.ini + 2), size);
		}
		else {
			file_cpyname(path, np2arg.ini, size);
		}
		ext = file_getext(path);
		if (ext[0] != '\0') {
			file_catname(path, ext_ini, size);
		}
	}
	else {
		file_cutext(path);
		file_catname(path, ext_ini, size);
	}
}

void initload(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void initsave(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

