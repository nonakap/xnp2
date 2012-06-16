#include	"compiler.h"
#include	"strres.h"
#include	"np2.h"
#include	"dosio.h"
#include	"ini.h"
#include	"winkbd.h"
#include	"pccore.h"


void ini_read(const TCHAR *path, const TCHAR *title,
											const PFTBL *tbl, UINT count) {

	profile_iniread(path, title, tbl, count, NULL);
}

void ini_write(const TCHAR *path, const TCHAR *title,
											const PFTBL *tbl, UINT count) {

	profile_iniwrite(path, title, tbl, count, NULL);
}


// ----

static const TCHAR ini_title[] = _T("NekoProjectII");
#if defined(_UNICODE)
static const TCHAR inifile[] = _T("np2u.cfg");
#else
static const TCHAR inifile[] = _T("np2.cfg");
#endif

enum {
	PFRO_STR		= PFFLAG_RO + PFTYPE_STR,
	PFRO_BOOL		= PFFLAG_RO + PFTYPE_BOOL,
	PFRO_UINT8		= PFFLAG_RO + PFTYPE_UINT8,
	PFRO_HEX32		= PFFLAG_RO + PFTYPE_HEX32
};

static const PFTBL iniitem[] = {
	PFVAL("WindposX", PFTYPE_SINT32,	&np2oscfg.winx),
	PFVAL("WindposY", PFTYPE_SINT32,	&np2oscfg.winy),

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

	PFEXT("VRAMwait", PFTYPE_BIN,		np2cfg.wait,			6),
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

	PFVAL("F12_COPY", PFTYPE_UINT8,		&np2oscfg.F12COPY),
	PFVAL("Joystick", PFTYPE_BOOL,		&np2oscfg.JOYPAD1),
	PFEXT("Joy1_btn", PFTYPE_BIN,		np2oscfg.JOY1BTN,		4),

	PFVAL("e_resume", PFTYPE_BOOL,		&np2oscfg.resume),
	PFVAL("jast_snd", PFTYPE_BOOL,		&np2oscfg.jastsnd),
	PFVAL("I286SAVE", PFRO_BOOL,		&np2oscfg.I286SAVE)};


void initload(void) {

	TCHAR	path[MAX_PATH];

	milstr_ncpy(path, file_getcd(inifile), NELEMENTS(path));
	ini_read(path, ini_title, iniitem, NELEMENTS(iniitem));
}

void initsave(void) {

	TCHAR	path[MAX_PATH];

	milstr_ncpy(path, file_getcd(inifile), NELEMENTS(path));
	ini_write(path, ini_title, iniitem, NELEMENTS(iniitem));
}

