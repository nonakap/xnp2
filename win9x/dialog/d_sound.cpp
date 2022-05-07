/**
 * @file	d_sound.cpp
 * @brief	Sound configure dialog procedure
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "c_dipsw.h"
#include "c_slidervalue.h"
#include "np2class.h"
#include "dosio.h"
#include "joymng.h"
#include "np2.h"
#include "sysmng.h"
#include "misc\PropProc.h"
#include "pccore.h"
#include "iocore.h"
#include "generic\dipswbmp.h"
#include "sound\sound.h"
#include "sound\fmboard.h"
#include "sound\tms3631.h"

// ---- mixer

/**
 * @brief Mixer �y�[�W
 */
class SndOptMixerPage : public CPropPageProc
{
public:
	SndOptMixerPage();
	virtual ~SndOptMixerPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	CSliderValue m_fm;			//!< FM ���H�����[��
	CSliderValue m_psg;			//!< PSG ���H�����[��
	CSliderValue m_adpcm;		//!< ADPCM ���H�����[��
	CSliderValue m_pcm;			//!< PCM ���H�����[��
	CSliderValue m_rhythm;		//!< RHYTHM ���H�����[��
};

/**
 * �R���X�g���N�^
 */
SndOptMixerPage::SndOptMixerPage()
	: CPropPageProc(IDD_SNDMIX)
{
}

/**
 * �f�X�g���N�^
 */
SndOptMixerPage::~SndOptMixerPage()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL SndOptMixerPage::OnInitDialog()
{
	m_fm.SubclassDlgItem(IDC_VOLFM, this);
	m_fm.SetStaticId(IDC_VOLFMSTR);
	m_fm.SetRange(0, 128);
	m_fm.SetPos(np2cfg.vol_fm);

	m_psg.SubclassDlgItem(IDC_VOLPSG, this);
	m_psg.SetStaticId(IDC_VOLPSGSTR);
	m_psg.SetRange(0, 128);
	m_psg.SetPos(np2cfg.vol_ssg);

	m_adpcm.SubclassDlgItem(IDC_VOLADPCM, this);
	m_adpcm.SetStaticId(IDC_VOLADPCMSTR);
	m_adpcm.SetRange(0, 128);
	m_adpcm.SetPos(np2cfg.vol_adpcm);

	m_pcm.SubclassDlgItem(IDC_VOLPCM, this);
	m_pcm.SetStaticId(IDC_VOLPCMSTR);
	m_pcm.SetRange(0, 128);
	m_pcm.SetPos(np2cfg.vol_pcm);

	m_rhythm.SubclassDlgItem(IDC_VOLRHYTHM, this);
	m_rhythm.SetStaticId(IDC_VOLRHYTHMSTR);
	m_rhythm.SetRange(0, 128);
	m_rhythm.SetPos(np2cfg.vol_rhythm);

	return TRUE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void SndOptMixerPage::OnOK()
{
	bool bUpdated = false;

	const UINT8 cFM = static_cast<UINT8>(m_fm.GetPos());
	if (np2cfg.vol_fm != cFM)
	{
		np2cfg.vol_fm = cFM;
		opngen_setvol(cFM);
		bUpdated = true;
	}

	const UINT8 cPSG = static_cast<UINT8>(m_psg.GetPos());
	if (np2cfg.vol_ssg != cPSG)
	{
		np2cfg.vol_ssg = cPSG;
		psggen_setvol(cPSG);
		bUpdated = true;
	}

	const UINT8 cADPCM = static_cast<UINT8>(m_adpcm.GetPos());
	if (np2cfg.vol_adpcm != cADPCM)
	{
		np2cfg.vol_adpcm = cADPCM;
		adpcm_setvol(cADPCM);
		for (UINT i = 0; i < _countof(g_opna); i++)
		{
			adpcm_update(&g_opna[i].adpcm);
		}
		bUpdated = true;
	}

	const UINT8 cPCM = static_cast<UINT8>(m_pcm.GetPos());
	if (np2cfg.vol_pcm != cPCM)
	{
		np2cfg.vol_pcm = cPCM;
		pcm86gen_setvol(cPCM);
		pcm86gen_update();
		bUpdated = true;
	}

	const UINT8 cRhythm = static_cast<UINT8>(m_rhythm.GetPos());
	if (np2cfg.vol_rhythm != cRhythm)
	{
		np2cfg.vol_rhythm = cRhythm;
		rhythm_setvol(cRhythm);
		for (UINT i = 0; i < _countof(g_opna); i++)
		{
			rhythm_update(&g_opna[i].rhythm);
		}
		bUpdated = true;
	}

	if (bUpdated)
	{
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL SndOptMixerPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == IDC_SNDMIXDEF)
	{
		m_fm.SetPos(64);
		m_psg.SetPos(64);
		m_adpcm.SetPos(64);
		m_pcm.SetPos(64);
		m_rhythm.SetPos(64);
		return TRUE;
	}
	return FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT SndOptMixerPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_HSCROLL)
	{
		switch (::GetDlgCtrlID(reinterpret_cast<HWND>(lParam)))
		{
			case IDC_VOLFM:
				m_fm.UpdateValue();
				break;

			case IDC_VOLPSG:
				m_psg.UpdateValue();
				break;

			case IDC_VOLADPCM:
				m_adpcm.UpdateValue();
				break;

			case IDC_VOLPCM:
				m_pcm.UpdateValue();
				break;

			case IDC_VOLRHYTHM:
				m_rhythm.UpdateValue();
				break;

			default:
				break;
		}
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}



// ---- PC-9801-14

/**
 * @brief 14 �y�[�W
 */
class SndOpt14Page : public CPropPageProc
{
public:
	SndOpt14Page();
	virtual ~SndOpt14Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	/**
	 * @brief �A�C�e��
	 */
	struct Item
	{
		UINT nSlider;		//!< �X���C�_�[
		UINT nStatic;		//!< �X�^�e�B�b�N
	};

	CSliderValue m_vol[6];	//!< ���H�����[��
};

/**
 * �R���X�g���N�^
 */
SndOpt14Page::SndOpt14Page()
	: CPropPageProc(IDD_SND14)
{
}

/**
 * �f�X�g���N�^
 */
SndOpt14Page::~SndOpt14Page()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL SndOpt14Page::OnInitDialog()
{
	static const Item s_snd14item[6] =
	{
		{IDC_VOL14L,	IDC_VOL14LSTR},
		{IDC_VOL14R,	IDC_VOL14RSTR},
		{IDC_VOLF2,		IDC_VOLF2STR},
		{IDC_VOLF4,		IDC_VOLF4STR},
		{IDC_VOLF8,		IDC_VOLF8STR},
		{IDC_VOLF16,	IDC_VOLF16STR},
	};

	for (UINT i = 0; i < 6; i++)
	{
		m_vol[i].SubclassDlgItem(s_snd14item[i].nSlider, this);
		m_vol[i].SetStaticId(s_snd14item[i].nStatic);
		m_vol[i].SetRange(0, 15);
		m_vol[i].SetPos(np2cfg.vol14[i]);
	}

	return TRUE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void SndOpt14Page::OnOK()
{
	bool bUpdated = false;

	for (UINT i = 0; i < 6; i++)
	{
		const UINT8 cVol = static_cast<UINT8>(m_vol[i].GetPos());
		if (np2cfg.vol14[i] != cVol)
		{
			np2cfg.vol14[i] = cVol;
			bUpdated = true;
		}
	}

	if (bUpdated)
	{
		::tms3631_setvol(np2cfg.vol14);
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT SndOpt14Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_HSCROLL)
	{
		for (UINT i = 0; i < 6; i++)
		{
			if (m_vol[i] == reinterpret_cast<HWND>(lParam))
			{
				m_vol[i].UpdateValue();
				break;
			}
		}
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}



// ---- PC-9801-26

/**
 * @brief 26 �y�[�W
 */
class SndOpt26Page : public CPropPageProc
{
public:
	SndOpt26Page();
	virtual ~SndOpt26Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_snd26;				//!< �ݒ�l
	CComboData m_io;			//!< IO
	CComboData m_int;			//!< INT
	CComboData m_rom;			//!< ROM
	CStaticDipSw m_dipsw;		//!< DIPSW
	void Set(UINT8 cValue);
	void SetJumper(UINT cAdd, UINT cRemove);
	void OnDipSw();
};

//! 26 I/O
static const CComboData::Entry s_io26[] =
{
	{MAKEINTRESOURCE(IDS_0088),		0x00},
	{MAKEINTRESOURCE(IDS_0188),		0x10},
};

//! 26 INT
static const CComboData::Entry s_int26[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0x00},
	{MAKEINTRESOURCE(IDS_INT41),	0x80},
	{MAKEINTRESOURCE(IDS_INT5),		0xc0},
	{MAKEINTRESOURCE(IDS_INT6),		0x40},
};

//! 26 ROM
static const CComboData::Entry s_rom26[] =
{
	{MAKEINTRESOURCE(IDS_C8000),		0x00},
	{MAKEINTRESOURCE(IDS_CC000),		0x01},
	{MAKEINTRESOURCE(IDS_D0000),		0x02},
	{MAKEINTRESOURCE(IDS_D4000),		0x03},
	{MAKEINTRESOURCE(IDS_NONCONNECT),	0x04},
};

/**
 * �R���X�g���N�^
 */
SndOpt26Page::SndOpt26Page()
	: CPropPageProc(IDD_SND26)
	, m_snd26(0)
{
}

/**
 * �f�X�g���N�^
 */
SndOpt26Page::~SndOpt26Page()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL SndOpt26Page::OnInitDialog()
{
	m_io.SubclassDlgItem(IDC_SND26IO, this);
	m_io.Add(s_io26, _countof(s_io26));

	m_int.SubclassDlgItem(IDC_SND26INT, this);
	m_int.Add(s_int26, _countof(s_int26));

	m_rom.SubclassDlgItem(IDC_SND26ROM, this);
	m_rom.Add(s_rom26, _countof(s_rom26));

	Set(np2cfg.snd26opt);

	m_dipsw.SubclassDlgItem(IDC_SND26JMP, this);

	m_io.SetFocus();
	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void SndOpt26Page::OnOK()
{
	if (np2cfg.snd26opt != m_snd26)
	{
		np2cfg.snd26opt = m_snd26;
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL SndOpt26Page::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_SND26IO:
			SetJumper(m_io.GetCurItemData(m_snd26 & 0x10), 0x10);
			break;

		case IDC_SND26INT:
			SetJumper(m_int.GetCurItemData(m_snd26 & 0xc0), 0xc0);
			break;

		case IDC_SND26ROM:
			SetJumper(m_rom.GetCurItemData(m_snd26 & 0x07), 0x07);
			break;

		case IDC_SND26DEF:
			Set(0xd1);
			m_dipsw.Invalidate();
			break;

		case IDC_SND26JMP:
			OnDipSw();
			break;
	}
	return FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT SndOpt26Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_DRAWITEM:
			if (LOWORD(wParam) == IDC_SND26JMP)
			{
				UINT8* pBitmap = dipswbmp_getsnd26(m_snd26);
				m_dipsw.Draw((reinterpret_cast<LPDRAWITEMSTRUCT>(lParam))->hDC, pBitmap);
				_MFREE(pBitmap);
			}
			return FALSE;
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �R���g���[���ݒ�
 * @param[in] cValue �ݒ�l
 */
void SndOpt26Page::Set(UINT8 cValue)
{
	m_snd26 = cValue;

	m_io.SetCurItemData(cValue & 0x10);
	m_int.SetCurItemData(cValue & 0xc0);

	const UINT nRom = cValue & 0x07;
	m_rom.SetCurItemData((nRom & 0x04) ? 0x04 : nRom);
}

/**
 * �ݒ�
 * @param[in] nAdd �ǉ��r�b�g
 * @param[in] nRemove �폜�r�b�g
 */
void SndOpt26Page::SetJumper(UINT nAdd, UINT nRemove)
{
	const UINT nValue = (m_snd26 & (~nRemove)) | nAdd;
	if (m_snd26 != static_cast<UINT8>(nValue))
	{
		m_snd26 = static_cast<UINT8>(nValue);
		m_dipsw.Invalidate();
	}
}

/**
 * DIPSW ���^�b�v����
 */
void SndOpt26Page::OnDipSw()
{
	RECT rect1;
	m_dipsw.GetWindowRect(&rect1);

	RECT rect2;
	m_dipsw.GetClientRect(&rect2);

	POINT p;
	::GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3))
	{
		return;
	}

	UINT nValue = m_snd26;
	if ((p.x >= 2) && (p.x < 7))
	{
		nValue = (nValue & (~7)) | (p.x - 2);
	}
	else if ((p.x >= 9) && (p.x < 12))
	{
		UINT cBit = 0x40 << (2 - p.y);
		switch (p.x)
		{
			case 9:
				nValue |= cBit;
				break;

			case 10:
				nValue ^= cBit;
				break;

			case 11:
				nValue &= ~cBit;
				break;
		}
	}
	else if ((p.x >= 15) && (p.x < 17))
	{
		nValue = (nValue & (~0x10)) | ((p.x - 15) << 4);
	}

	if (m_snd26 != static_cast<UINT8>(nValue))
	{
		Set(static_cast<UINT8>(nValue));
		m_dipsw.Invalidate();
	}
}



// ---- PC-9801-86

/**
 * @brief 86 �y�[�W
 */
class SndOpt86Page : public CPropPageProc
{
public:
	SndOpt86Page();
	virtual ~SndOpt86Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_snd86;				//!< �ݒ�l
	CComboData m_io;			//!< IO
	CComboData m_int;			//!< INT
	CComboData m_id;			//!< ID
	CStaticDipSw m_dipsw;		//!< DIPSW
	void Set(UINT8 cValue);
	void SetJumper(UINT cAdd, UINT cRemove);
	void OnDipSw();
};

//! 86 I/O
static const CComboData::Entry s_io86[] =
{
	{MAKEINTRESOURCE(IDS_0188),		0x01},
	{MAKEINTRESOURCE(IDS_0288),		0x00},
};

//! 86 INT
static const CComboData::Entry s_int86[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0x00},
	{MAKEINTRESOURCE(IDS_INT41),	0x04},
	{MAKEINTRESOURCE(IDS_INT5),		0x0c},
	{MAKEINTRESOURCE(IDS_INT6),		0x08},
};

//! 86 ID
static const CComboData::Entry s_id86[] =
{
	{MAKEINTRESOURCE(IDS_0X),	0xe0},
	{MAKEINTRESOURCE(IDS_1X),	0xc0},
	{MAKEINTRESOURCE(IDS_2X),	0xa0},
	{MAKEINTRESOURCE(IDS_3X),	0x80},
	{MAKEINTRESOURCE(IDS_4X),	0x60},
	{MAKEINTRESOURCE(IDS_5X),	0x40},
	{MAKEINTRESOURCE(IDS_6X),	0x20},
	{MAKEINTRESOURCE(IDS_7X),	0x00},
};

/**
 * �R���X�g���N�^
 */
SndOpt86Page::SndOpt86Page()
	: CPropPageProc(IDD_SND86)
	, m_snd86(0)
{
}

/**
 * �f�X�g���N�^
 */
SndOpt86Page::~SndOpt86Page()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL SndOpt86Page::OnInitDialog()
{
	m_io.SubclassDlgItem(IDC_SND86IO, this);
	m_io.Add(s_io86, _countof(s_io86));

	m_int.SubclassDlgItem(IDC_SND86INTA, this);
	m_int.Add(s_int86, _countof(s_int86));

	m_id.SubclassDlgItem(IDC_SND86ID, this);
	m_id.Add(s_id86, _countof(s_id86));

	Set(np2cfg.snd86opt);

	m_dipsw.SubclassDlgItem(IDC_SND86DIP, this);

	m_io.SetFocus();
	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void SndOpt86Page::OnOK()
{
	if (np2cfg.snd86opt != m_snd86)
	{
		np2cfg.snd86opt = m_snd86;
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL SndOpt86Page::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_SND86IO:
			SetJumper(m_io.GetCurItemData(m_snd86 & 0x01), 0x01);
			break;

		case IDC_SND86INT:
			SetJumper((IsDlgButtonChecked(IDC_SND86INT) != BST_UNCHECKED) ? 0x10 : 0x00, 0x10);
			break;

		case IDC_SND86INTA:
			SetJumper(m_int.GetCurItemData(m_snd86 & 0x0c), 0x0c);
			break;

		case IDC_SND86ROM:
			SetJumper((IsDlgButtonChecked(IDC_SND86ROM) != BST_UNCHECKED) ? 0x02 : 0x00, 0x02);
			break;

		case IDC_SND86ID:
			SetJumper(m_id.GetCurItemData(m_snd86 & 0xe0), 0xe0);
			break;

		case IDC_SND86DEF:
			Set(0x7f);
			m_dipsw.Invalidate();
			break;

		case IDC_SND86DIP:
			OnDipSw();
			break;
	}
	return FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT SndOpt86Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_DRAWITEM:
			if (LOWORD(wParam) == IDC_SND86DIP)
			{
				UINT8* pBitmap = dipswbmp_getsnd86(m_snd86);
				m_dipsw.Draw((reinterpret_cast<LPDRAWITEMSTRUCT>(lParam))->hDC, pBitmap);
				_MFREE(pBitmap);
			}
			return FALSE;
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �R���g���[���ݒ�
 * @param[in] cValue �ݒ�l
 */
void SndOpt86Page::Set(UINT8 cValue)
{
	m_snd86 = cValue;
	m_io.SetCurItemData(cValue & 0x01);
	CheckDlgButton(IDC_SND86INT, (cValue & 0x10) ? BST_CHECKED : BST_UNCHECKED);
	m_int.SetCurItemData(cValue & 0x0c);
	m_id.SetCurItemData(cValue & 0xe0);
	CheckDlgButton(IDC_SND86ROM, (cValue & 0x02) ? BST_CHECKED : BST_UNCHECKED);
}

/**
 * �ݒ�
 * @param[in] nAdd �ǉ��r�b�g
 * @param[in] nRemove �폜�r�b�g
 */
void SndOpt86Page::SetJumper(UINT nAdd, UINT nRemove)
{
	const UINT nValue = (m_snd86 & (~nRemove)) | nAdd;
	if (m_snd86 != static_cast<UINT8>(nValue))
	{
		m_snd86 = static_cast<UINT8>(nValue);
		m_dipsw.Invalidate();
	}
}

/**
 * DIPSW ���^�b�v����
 */
void SndOpt86Page::OnDipSw()
{
	RECT rect1;
	m_dipsw.GetWindowRect(&rect1);

	RECT rect2;
	m_dipsw.GetClientRect(&rect2);

	POINT p;
	::GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 8;
	p.y /= 8;
	if ((p.x < 2) || (p.x >= 10) || (p.y < 1) || (p.y >= 3))
	{
		return;
	}
	p.x -= 2;
	m_snd86 ^= (1 << p.x);
	Set(m_snd86);
	m_dipsw.Invalidate();
}



// ---- Speak board

/**
 * @brief Speak board �y�[�W
 */
class SndOptSpbPage : public CPropPageProc
{
public:
	SndOptSpbPage();
	virtual ~SndOptSpbPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_spb;				//!< �ݒ�l
	UINT8 m_vr;					//!< VR�ݒ�l
	CComboData m_io;			//!< IO
	CComboData m_int;			//!< INT
	CComboData m_rom;			//!< ROM
	CSliderProc m_vol;			//!< VOL
	CStaticDipSw m_dipsw;		//!< DIPSW
	void Set(UINT8 cValue, UINT8 cVR);
	void SetJumper(UINT cAdd, UINT cRemove);
	void OnDipSw();
};

/**
 * �R���X�g���N�^
 */
SndOptSpbPage::SndOptSpbPage()
	: CPropPageProc(IDD_SNDSPB)
	, m_spb(0)
	, m_vr(0)
{
}

/**
 * �f�X�g���N�^
 */
SndOptSpbPage::~SndOptSpbPage()
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL SndOptSpbPage::OnInitDialog()
{
	m_io.SubclassDlgItem(IDC_SPBIO, this);
	m_io.Add(s_io26, _countof(s_io26));

	m_int.SubclassDlgItem(IDC_SPBINT, this);
	m_int.Add(s_int26, _countof(s_int26));

	m_rom.SubclassDlgItem(IDC_SPBROM, this);
	m_rom.Add(s_rom26, _countof(s_rom26));

	Set(np2cfg.spbopt, np2cfg.spb_vrc);

	m_vol.SubclassDlgItem(IDC_SPBVRLEVEL, this);
	m_vol.SetRangeMin(0, FALSE);
	m_vol.SetRangeMax(24, FALSE);
	m_vol.SetPos(np2cfg.spb_vrl);

	CheckDlgButton(IDC_SPBREVERSE, (np2cfg.spb_x) ? BST_CHECKED : BST_UNCHECKED);

	m_dipsw.SubclassDlgItem(IDC_SPBJMP, this);

	m_io.SetFocus();
	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void SndOptSpbPage::OnOK()
{
	bool bUpdated = false;

	if (np2cfg.spbopt != m_spb)
	{
		np2cfg.spbopt = m_spb;
		bUpdated = true;
	}

	if (np2cfg.spb_vrc != m_vr)
	{
		np2cfg.spb_vrc = m_vr;
		bUpdated = true;
	}
	const UINT8 cVol = static_cast<UINT8>(m_vol.GetPos());
	if (np2cfg.spb_vrl != cVol)
	{
		np2cfg.spb_vrl = cVol;
		bUpdated = true;
	}

	const UINT8 cRev = (IsDlgButtonChecked(IDC_SPBREVERSE) != BST_UNCHECKED) ? 1 : 0;
	if (np2cfg.spb_x != cRev)
	{
		np2cfg.spb_x = cRev;
		bUpdated = true;
	}

	if (bUpdated)
	{
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL SndOptSpbPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_SPBIO:
			SetJumper(m_io.GetCurItemData(m_spb & 0x10), 0x10);
			break;

		case IDC_SPBINT:
			SetJumper(m_int.GetCurItemData(m_spb & 0xc0), 0xc0);
			break;

		case IDC_SPBROM:
			SetJumper(m_rom.GetCurItemData(m_spb & 0x07), 0x07);
			break;

		case IDC_SPBDEF:
			Set(0xd1, 0);
			m_dipsw.Invalidate();
			break;

		case IDC_SPBVRL:
		case IDC_SPBVRR:
			m_vr = 0;
			if (IsDlgButtonChecked(IDC_SPBVRL) != BST_UNCHECKED)
			{
				m_vr |= 0x01;
			}
			if (IsDlgButtonChecked(IDC_SPBVRR) != BST_UNCHECKED)
			{
				m_vr |= 0x02;
			}
			m_dipsw.Invalidate();
			break;

		case IDC_SPBJMP:
			OnDipSw();
			break;
	}
	return FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT SndOptSpbPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_DRAWITEM:
			if (LOWORD(wParam) == IDC_SPBJMP)
			{
				UINT8* pBitmap = dipswbmp_getsndspb(m_spb, m_vr);
				m_dipsw.Draw((reinterpret_cast<LPDRAWITEMSTRUCT>(lParam))->hDC, pBitmap);
				_MFREE(pBitmap);
			}
			return FALSE;
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �R���g���[���ݒ�
 * @param[in] cValue �ݒ�l
 * @param[in] cVR VR �ݒ�l
 */
void SndOptSpbPage::Set(UINT8 cValue, UINT8 cVR)
{
	m_spb = cValue;
	m_vr = cVR;

	m_io.SetCurItemData(cValue & 0x10);
	m_int.SetCurItemData(cValue & 0xc0);

	const UINT nRom = cValue & 0x07;
	m_rom.SetCurItemData((nRom & 0x04) ? 0x04 : nRom);

	CheckDlgButton(IDC_SPBVRL, (cVR & 0x01) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_SPBVRR, (cVR & 0x02) ? BST_CHECKED : BST_UNCHECKED);
}

/**
 * �ݒ�
 * @param[in] nAdd �ǉ��r�b�g
 * @param[in] nRemove �폜�r�b�g
 */
void SndOptSpbPage::SetJumper(UINT nAdd, UINT nRemove)
{
	const UINT nValue = (m_spb & (~nRemove)) | nAdd;
	if (m_spb != static_cast<UINT8>(nValue))
	{
		m_spb = static_cast<UINT8>(nValue);
		m_dipsw.Invalidate();
	}
}

/**
 * DIPSW ���^�b�v����
 */
void SndOptSpbPage::OnDipSw()
{
	RECT rect1;
	m_dipsw.GetWindowRect(&rect1);

	RECT rect2;
	m_dipsw.GetClientRect(&rect2);

	POINT p;
	::GetCursorPos(&p);
	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3))
	{
		return;
	}

	UINT8 cValue = m_spb;
	UINT8 cVR = m_vr;
	if ((p.x >= 2) && (p.x < 5))
	{
		UINT8 cBit = 0x40 << (2 - p.y);
		switch (p.x)
		{
			case 2:
				cValue |= cBit;
				break;

			case 3:
				cValue ^= cBit;
				break;

			case 4:
				cValue &= ~cBit;
				break;
		}
	}
	else if (p.x == 7)
	{
		cValue ^= 0x20;
	}
	else if ((p.x >= 10) && (p.x < 12))
	{
		cValue = static_cast<UINT8>((cValue & (~0x10)) | ((p.x - 10) << 4));
	}
	else if ((p.x >= 14) && (p.x < 19))
	{
		cValue = static_cast<UINT8>((cValue & (~7)) | (p.x - 14));
	}
	else if ((p.x >= 21) && (p.x < 24))
	{
		cVR ^= (3 - p.y);
	}

	if ((m_spb != cValue) || (m_vr != cVR))
	{
		Set(cValue, cVR);
		m_dipsw.Invalidate();
	}
}



// ---- JOYPAD

/**
 * @brief PAD �y�[�W
 */
class SndOptPadPage : public CPropPageProc
{
public:
	SndOptPadPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

//! �{�^��
static const UINT s_pad[4][3] =
{
	{IDC_PAD1_1A, IDC_PAD1_2A, IDC_PAD1_RA},
	{IDC_PAD1_1B, IDC_PAD1_2B, IDC_PAD1_RB},
	{IDC_PAD1_1C, IDC_PAD1_2C, IDC_PAD1_RC},
	{IDC_PAD1_1D, IDC_PAD1_2D, IDC_PAD1_RD},
};

/**
 * �R���X�g���N�^
 */
SndOptPadPage::SndOptPadPage()
	: CPropPageProc(IDD_SNDPAD1)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL SndOptPadPage::OnInitDialog()
{
	CheckDlgButton(IDC_JOYPAD1, (np2oscfg.JOYPAD1 & 1) ? BST_CHECKED : BST_UNCHECKED);

	for (UINT i = 0; i < _countof(s_pad); i++)
	{
		for (UINT j = 0; j < 3; j++)
		{
			CheckDlgButton(s_pad[i][j], (np2oscfg.JOY1BTN[i] & (1 << j)) ? BST_CHECKED : BST_UNCHECKED);
		}
	}

	return TRUE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void SndOptPadPage::OnOK()
{
	bool bUpdated = false;

	const UINT8 cJoyPad = (np2oscfg.JOYPAD1 & (~1)) | ((IsDlgButtonChecked(IDC_JOYPAD1) != BST_UNCHECKED) ? 1 : 0);
	if (np2oscfg.JOYPAD1 != cJoyPad)
	{
		np2oscfg.JOYPAD1 = cJoyPad;
	}

	for (UINT i = 0; i < _countof(s_pad); i++)
	{
		UINT8 cBtn = 0;
		for (UINT j = 0; j < 3; j++)
		{
			if (IsDlgButtonChecked(s_pad[i][j]) != BST_UNCHECKED)
			{
				cBtn |= (1 << j);
			}
			if (np2oscfg.JOY1BTN[i] != cBtn)
			{
				np2oscfg.JOY1BTN[i] = cBtn;
				bUpdated = true;
			}
		}
	}

	if (bUpdated)
	{
		::joymng_initialize();
		::sysmng_update(SYS_UPDATEOSCFG);
	}
}



// ----

/**
 * �T�E���h�ݒ�
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_sndopt(HWND hwndParent)
{
	CPropSheetProc prop(IDS_SOUNDOPTION, hwndParent);

	SndOptMixerPage mixer;
	prop.AddPage(&mixer);

	SndOpt14Page pc980114;
	prop.AddPage(&pc980114);

	SndOpt26Page pc980126;
	prop.AddPage(&pc980126);

	SndOpt86Page pc980186;
	prop.AddPage(&pc980186);

	SndOptSpbPage spb;
	prop.AddPage(&spb);

	SndOptPadPage pad;
	prop.AddPage(&pad);

	prop.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	prop.m_psh.hIcon = LoadIcon(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2));
	prop.m_psh.pfnCallback = np2class_propetysheet;
	prop.DoModal();

	InvalidateRect(hwndParent, NULL, TRUE);
}
