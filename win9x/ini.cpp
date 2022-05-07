/**
 *	@file	ini.cpp
 *	@brief	�ݒ�t�@�C�� �A�N�Z�X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "strres.h"
#include "profile.h"
#include "np2.h"
#include "np2arg.h"
#include "dosio.h"
#include "ini.h"
#include "winkbd.h"
#include "pccore.h"

// ---- user type

/**
 * 16�r�b�g�z���ǂݍ���
 * @param[in] lpString ������
 * @param[out] ini �ݒ�e�[�u��
 */
static void inirdargs16(LPCTSTR lpString, const PFTBL* ini)
{
	SINT16* lpDst = static_cast<SINT16*>(ini->value);
	const int nCount = ini->arg;

	for (int i = 0; i < nCount; i++)
	{
		while (*lpString == ' ')
		{
			lpString++;
		}

		if (*lpString == '\0')
		{
			break;
		}

		lpDst[i] = static_cast<SINT16>(milstr_solveINT(lpString));
		while (*lpString != '\0')
		{
			const TCHAR c = *lpString++;
			if (c == ',')
			{
				break;
			}
		}
	}
}

/**
 * 3�o�C�g��ǂݍ���
 * @param[in] lpString ������
 * @param[out] ini �ݒ�e�[�u��
 */
static void inirdbyte3(LPCTSTR lpString, const PFTBL* ini)
{
	for (int i = 0; i < 3; i++)
	{
		const TCHAR c = lpString[i];
		if (c == '\0')
		{
			break;
		}
		if ((((c - '0') & 0xff) < 9) || (((c - 'A') & 0xdf) < 26))
		{
			(static_cast<UINT8*>(ini->value))[i] = static_cast<UINT8>(c);
		}
	}
}

/**
 * �L�[�{�[�h�ݒ��ǂݍ���
 * @param[in] lpString ������
 * @param[out] ini �ݒ�e�[�u��
 */
static void inirdkb(LPCTSTR lpString, const PFTBL* ini)
{
	if ((!milstr_extendcmp(lpString, TEXT("PC98"))) || (!milstr_cmp(lpString, TEXT("98"))))
	{
		*(static_cast<UINT8*>(ini->value)) = KEY_PC98;
	}
	else if ((!milstr_extendcmp(lpString, TEXT("DOS"))) || (!milstr_cmp(lpString, TEXT("PCAT"))) || (!milstr_cmp(lpString, TEXT("AT"))))
	{
		*(static_cast<UINT8*>(ini->value)) = KEY_KEY106;
	}
	else if ((!milstr_extendcmp(lpString, TEXT("KEY101"))) || (!milstr_cmp(lpString, TEXT("101"))))
	{
		*(static_cast<UINT8*>(ini->value)) = KEY_KEY101;
	}
}


// ---- Use WinAPI

#if !defined(_UNICODE)
/**
 * �r�b�g��ݒ�
 * @param[in,out] lpBuffer �o�b�t�@
 * @param[in] nPos �ʒu
 * @param[in] set �Z�b�g or �N���A
 */
static void bitmapset(void* lpBuffer, UINT nPos, BOOL set)
{
	const int nIndex = (nPos >> 3);
	const UINT8 cBit = 1 << (nPos & 7);
	if (set)
	{
		(static_cast<UINT8*>(lpBuffer))[nIndex] |= cBit;
	}
	else
	{
		(static_cast<UINT8*>(lpBuffer))[nIndex] &= ~cBit;
	}
}

/**
 * �r�b�g�𓾂�
 * @param[in] lpBuffer �o�b�t�@
 * @param[in] nPos �ʒu
 * @return �r�b�g
 */
static BOOL bitmapget(const void* lpBuffer, UINT nPos)
{
	const int nIndex = (nPos >> 3);
	const UINT8 cBit = 1 << (nPos & 7);
	return (((static_cast<const UINT8*>(lpBuffer))[nIndex]) & cBit) ? TRUE : FALSE;
}

/**
 * �o�C�i�����A���V���A���C�Y
 * @param[out] lpBin �o�C�i��
 * @param[in] cbBin �o�C�i���̃T�C�Y
 * @param[in] lpString ������o�b�t�@
 */
static void binset(void* lpBin, UINT cbBin, LPCTSTR lpString)
{
	for (UINT i = 0; i < cbBin; i++)
	{
		while (*lpString == ' ')
		{
			lpString++;
		}

		LPTSTR lpStringEnd;
		const long lVal = _tcstol(lpString, &lpStringEnd, 16);
		if (lpString == lpStringEnd)
		{
			break;
		}

		(static_cast<UINT8*>(lpBin))[i] = static_cast<UINT8>(lVal);
		lpString = lpStringEnd;
	}
}

/**
 * �o�C�i�����V���A���C�Y
 * @param[out] lpString ������o�b�t�@
 * @param[in] cchString ������o�b�t�@��
 * @param[in] lpBin �o�C�i��
 * @param[in] cbBin �o�C�i���̃T�C�Y
 */
static void binget(LPTSTR lpString, int cchString, const void* lpBin, UINT cbBin)
{

	if (cbBin)
	{
		TCHAR tmp[8];
		wsprintf(tmp, TEXT("%.2x"), (static_cast<const UINT8*>(lpBin))[0]);
		milstr_ncpy(lpString, tmp, cchString);
	}
	for (UINT i = 1; i < cbBin; i++)
	{
		TCHAR tmp[8];
		wsprintf(tmp, TEXT(" %.2x"), (static_cast<const UINT8*>(lpBin))[i]);
		milstr_ncat(lpString, tmp, cchString);
	}
}

/**
 * �ݒ�ǂݏo��
 * @param[in] lpPath �p�X
 * @param[in] lpTitle �^�C�g��
 * @param[in] lpTable �ݒ�e�[�u��
 * @param[in] nCount �ݒ�e�[�u�� �A�C�e����
 */
void ini_read(LPCTSTR lpPath, LPCTSTR lpTitle, const PFTBL* lpTable, UINT nCount)
{
	const PFTBL* p = lpTable;
	const PFTBL* pTerminate = p + nCount;
	while (p < pTerminate)
	{
		TCHAR szWork[512];
		UINT32 val;
		switch (p->itemtype & PFTYPE_MASK)
		{
			case PFTYPE_STR:
				GetPrivateProfileString(lpTitle, p->item, static_cast<LPCTSTR>(p->value), static_cast<LPTSTR>(p->value), p->arg, lpPath);
				break;

			case PFTYPE_BOOL:
				GetPrivateProfileString(lpTitle, p->item,
									(*(static_cast<const UINT8*>(p->value))) ? str_true : str_false,
									szWork, NELEMENTS(szWork), lpPath);
				*(static_cast<UINT8*>(p->value)) = (!milstr_cmp(szWork, str_true)) ? 1 : 0;
				break;

			case PFTYPE_BITMAP:
				GetPrivateProfileString(lpTitle, p->item,
									(bitmapget(p->value, p->arg)) ? str_true : str_false,
									szWork, _countof(szWork), lpPath);
				bitmapset(p->value, p->arg, (milstr_cmp(szWork, str_true) == 0));
				break;

			case PFTYPE_BIN:
				GetPrivateProfileString(lpTitle, p->item, str_null, szWork, _countof(szWork), lpPath);
				binset(p->value, p->arg, szWork);
				break;

			case PFTYPE_SINT8:
			case PFTYPE_UINT8:
				val = GetPrivateProfileInt(lpTitle, p->item, *(static_cast<const UINT8*>(p->value)), lpPath);
				*(static_cast<UINT8*>(p->value)) = static_cast<UINT8>(val);
				break;

			case PFTYPE_SINT16:
			case PFTYPE_UINT16:
				val = GetPrivateProfileInt(lpTitle, p->item, *(static_cast<const UINT16*>(p->value)), lpPath);
				*(static_cast<UINT16*>(p->value)) = static_cast<UINT16>(val);
				break;

			case PFTYPE_SINT32:
			case PFTYPE_UINT32:
				val = GetPrivateProfileInt(lpTitle, p->item, *(static_cast<const UINT32*>(p->value)), lpPath);
				*(static_cast<UINT32*>(p->value)) = static_cast<UINT32>(val);
				break;

			case PFTYPE_HEX8:
				wsprintf(szWork, str_x, *(static_cast<const UINT8*>(p->value)));
				GetPrivateProfileString(lpTitle, p->item, szWork, szWork, _countof(szWork), lpPath);
				*(static_cast<UINT8*>(p->value)) = static_cast<UINT8>(milstr_solveHEX(szWork));
				break;

			case PFTYPE_HEX16:
				wsprintf(szWork, str_x, *(static_cast<const UINT16*>(p->value)));
				GetPrivateProfileString(lpTitle, p->item, szWork, szWork, _countof(szWork), lpPath);
				*(static_cast<UINT16*>(p->value)) = static_cast<UINT16>(milstr_solveHEX(szWork));
				break;

			case PFTYPE_HEX32:
				wsprintf(szWork, str_x, *(static_cast<const UINT32*>(p->value)));
				GetPrivateProfileString(lpTitle, p->item, szWork, szWork, _countof(szWork), lpPath);
				*(static_cast<UINT32*>(p->value)) = static_cast<UINT32>(milstr_solveHEX(szWork));
				break;

			case PFTYPE_ARGS16:
				GetPrivateProfileString(lpTitle, p->item, str_null, szWork, _countof(szWork), lpPath);
				inirdargs16(szWork, p);
				break;

			case PFTYPE_BYTE3:
				GetPrivateProfileString(lpTitle, p->item, str_null, szWork, _countof(szWork), lpPath);
				inirdbyte3(szWork, p);
				break;

			case PFTYPE_KB:
				GetPrivateProfileString(lpTitle, p->item, str_null, szWork, _countof(szWork), lpPath);
				inirdkb(szWork, p);
				break;
		}
		p++;
	}
}

/**
 * �ݒ菑������
 * @param[in] lpPath �p�X
 * @param[in] lpTitle �^�C�g��
 * @param[in] lpTable �ݒ�e�[�u��
 * @param[in] nCount �ݒ�e�[�u�� �A�C�e����
 */
void ini_write(LPCTSTR lpPath, LPCTSTR lpTitle, const PFTBL* lpTable, UINT nCount)
{
	const PFTBL* p = lpTable;
	const PFTBL* pTerminate = p + nCount;
	while (p < pTerminate)
	{
		if (!(p->itemtype & PFFLAG_RO))
		{
			TCHAR szWork[512];
			szWork[0] = '\0';

			LPCTSTR lpSet = szWork;
			switch(p->itemtype & PFTYPE_MASK) {
				case PFTYPE_STR:
					lpSet = static_cast<LPCTSTR>(p->value);
					break;

				case PFTYPE_BOOL:
					lpSet = (*(static_cast<const UINT8*>(p->value))) ? str_true : str_false;
					break;

				case PFTYPE_BITMAP:
					lpSet = (bitmapget(p->value, p->arg)) ? str_true : str_false;
					break;

				case PFTYPE_BIN:
					binget(szWork, _countof(szWork), p->value, p->arg);
					break;

				case PFTYPE_SINT8:
					wsprintf(szWork, str_d, *(static_cast<const SINT8*>(p->value)));
					break;

				case PFTYPE_SINT16:
					wsprintf(szWork, str_d, *(static_cast<const SINT16*>(p->value)));
					break;

				case PFTYPE_SINT32:
					wsprintf(szWork, str_d, *(static_cast<const SINT32*>(p->value)));
					break;

				case PFTYPE_UINT8:
					wsprintf(szWork, str_u, *(static_cast<const UINT8*>(p->value)));
					break;

				case PFTYPE_UINT16:
					wsprintf(szWork, str_u, *(static_cast<const UINT16*>(p->value)));
					break;

				case PFTYPE_UINT32:
					wsprintf(szWork, str_u, *(static_cast<const UINT32*>(p->value)));
					break;

				case PFTYPE_HEX8:
					wsprintf(szWork, str_x, *(static_cast<const UINT8*>(p->value)));
					break;

				case PFTYPE_HEX16:
					wsprintf(szWork, str_x, *(static_cast<const UINT16*>(p->value)));
					break;

				case PFTYPE_HEX32:
					wsprintf(szWork, str_x, *(static_cast<const UINT32*>(p->value)));
					break;

				default:
					lpSet = NULL;
					break;
			}
			if (lpSet)
			{
				::WritePrivateProfileString(lpTitle, p->item, lpSet, lpPath);
			}
		}
		p++;
	}
}

#else	// !defined(_UNICODE)

// ---- Use profile.c

/**
 * �R�[���o�b�N
 * @param[in] item �A�C�e��
 * @param[in] lpString ������
 */
static void UserReadItem(const PFTBL* item, LPCTSTR lpString)
{
	switch (item->itemtype & PFTYPE_MASK)
	{
		case PFTYPE_ARGS16:
			inirdargs16(lpString, item);
			break;

		case PFTYPE_BYTE3:
			inirdbyte3(lpString, item);
			break;

		case PFTYPE_KB:
			inirdkb(lpString, item);
			break;
	}
}

/**
 * �ݒ�ǂݎ��
 * @param[in] lpPath �p�X
 * @param[in] lpTitle �^�C�g��
 * @param[in] lpTable �ݒ�e�[�u��
 * @param[in] nCount �ݒ�e�[�u�� �A�C�e����
 */
void ini_read(LPCTSTR lpPath, LPCTSTR lpTitle, const PFTBL* lpTable, UINT nCount)
{
	profile_iniread(lpPath, lpTitle, lpTable, nCount, UserReadItem);
}

/**
 * �ݒ菑������
 * @param[in] lpPath �p�X
 * @param[in] lpTitle �^�C�g��
 * @param[in] lpTable �ݒ�e�[�u��
 * @param[in] nCount �ݒ�e�[�u�� �A�C�e����
 */
void ini_write(LPCTSTR lpPath, LPCTSTR lpTitle, const PFTBL* lpTable, UINT nCount)
{
	profile_iniwrite(lpPath, lpTitle, lpTable, nCount, NULL);
}

#endif	// !defined(_UNICODE)


// ----

#if !defined(SUPPORT_PC9821)
static const TCHAR s_szIniTitle[] = TEXT("NekoProjectII");		//!< �A�v����
#else
static const TCHAR s_szIniTitle[] = TEXT("NekoProject21");		//!< �A�v����
#endif

/**
 * �ǉ��ݒ�
 */
enum
{
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

/**
 * OS �ݒ� �e�[�u��
 */
static const PFTBL s_IniItems[] =
{
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

	PFVAL("SampleHz", PFTYPE_UINT32,	&np2cfg.samplingrate),
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

	// OS�ˑ��H
	PFVAL("keyboard", PFRO_KB,			&np2oscfg.KEYBOARD),
	PFVAL("F12_COPY", PFTYPE_UINT8,		&np2oscfg.F12COPY),
	PFVAL("Joystick", PFTYPE_BOOL,		&np2oscfg.JOYPAD1),
	PFEXT("Joy1_btn", PFTYPE_BIN,		np2oscfg.JOY1BTN,		4),

	PFVAL("clocknow", PFTYPE_UINT8,		&np2oscfg.clk_x),
	PFVAL("clockfnt", PFTYPE_UINT8,		&np2oscfg.clk_fnt),
	PFAND("clock_up", PFRO_HEX32,		&np2oscfg.clk_color1,	0xffffff),
	PFAND("clock_dn", PFRO_HEX32,		&np2oscfg.clk_color2,	0xffffff),

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

	PFVAL("snddev_t", PFTYPE_UINT8,		&np2oscfg.cSoundDeviceType),
	PFSTR("snddev_n", PFTYPE_STR,		np2oscfg.szSoundDeviceName),

#if defined(SUPPORT_VSTi)
	PFSTR("VSTiFile", PFRO_STR,			np2oscfg.szVSTiFile),
#endif	// defined(SUPPORT_VSTi)

	PFVAL("I286SAVE", PFRO_BOOL,		&np2oscfg.I286SAVE)
};

//! .ini �g���q
static const TCHAR s_szExt[] = TEXT(".ini");

/**
 * �ݒ�t�@�C���̃p�X�𓾂�
 * @param[out] lpPath �p�X
 * @param[in] cchPath �p�X �o�b�t�@�̒���
 */
void initgetfile(LPTSTR lpPath, UINT cchPath)
{
	LPCTSTR lpIni = Np2Arg::GetInstance()->iniFilename();
	if (lpIni)
	{
		file_cpyname(lpPath, lpIni, cchPath);
		LPCTSTR lpExt = file_getext(lpPath);
		if (lpExt[0] == '\0')
		{
			file_catname(lpPath, s_szExt, cchPath);
		}
	}
	else
	{
		file_cpyname(lpPath, modulefile, cchPath);
		file_cutext(lpPath);
		file_catname(lpPath, s_szExt, cchPath);
	}
}

/**
 * �ǂݍ���
 */
void initload(void)
{
	TCHAR szPath[MAX_PATH];

	initgetfile(szPath, _countof(szPath));
	ini_read(szPath, s_szIniTitle, s_IniItems, _countof(s_IniItems));
}

/**
 * �����o��
 */
void initsave(void)
{
	TCHAR szPath[MAX_PATH];

	initgetfile(szPath, _countof(szPath));
	ini_write(szPath, s_szIniTitle, s_IniItems, _countof(s_IniItems));
}
