/**
 * @file	d_mpu98.cpp
 * @brief	MPU-PC98 �ݒ�_�C�A���O
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "c_dipsw.h"
#include "c_midi.h"
#include "np2.h"
#include "commng.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "pccore.h"
#include "common/strres.h"
#include "generic/dipswbmp.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern	COMMNG	cm_mpu98;

#ifdef __cplusplus
}
#endif


/**
 * @brief MPU-PC98 �ݒ�_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
class CMpu98Dlg : public CDlgProc
{
public:
	CMpu98Dlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_mpu;				//!< �ݒ�l
	CComboData m_port;			//!< IO
	CComboData m_int;			//!< INT
	CStaticDipSw m_dipsw;		//!< DIPSW
	CComboMidiDevice m_midiin;	//!< MIDI IN
	CComboMidiDevice m_midiout;	//!< MIDI OUT
	CComboMidiModule m_module;	//!< MIDI Module
	CEditMimpiFile m_mimpifile;	//!< MIMPI
	void OnDipSw();
	void SetJumper(UINT8 cValue, UINT8 cBit);
	void SetPort(UINT8 cValue);
	UINT8 GetPort() const;
	void SetInt(UINT8 cValue);
	UINT8 GetInt() const;
};

/**
 * ���荞�݃��X�g
 */
static const CComboData::Entry s_int[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		0},
	{MAKEINTRESOURCE(IDS_INT1),		1},
	{MAKEINTRESOURCE(IDS_INT2),		2},
	{MAKEINTRESOURCE(IDS_INT5),		3},
};

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 */
CMpu98Dlg::CMpu98Dlg(HWND hwndParent)
	: CDlgProc(IDD_MPUPC98, hwndParent)
	, m_mpu(0)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CMpu98Dlg::OnInitDialog()
{
	m_mpu = np2cfg.mpuopt;

	m_port.SubclassDlgItem(IDC_MPUIO, this);
	for (UINT i = 0; i < 16; i++)
	{
		TCHAR szBuf[8];
		wsprintf(szBuf, str_4X, 0xC0D0 + (i << 10));
		m_port.Add(szBuf, i << 4);
	}
	SetPort(m_mpu);

	m_int.SubclassDlgItem(IDC_MPUINT, this);
	m_int.Add(s_int, _countof(s_int));
	SetInt(m_mpu);

	m_dipsw.SubclassDlgItem(IDC_MPUDIP, this);

	m_midiout.SubclassDlgItem(IDC_MPU98MMAP, this);
	m_midiout.EnumerateMidiOut();
	m_midiout.SetCurString(np2oscfg.mpu.mout);

	m_midiin.SubclassDlgItem(IDC_MPU98MDIN, this);
	m_midiin.EnumerateMidiIn();
	m_midiin.SetCurString(np2oscfg.mpu.min);

	m_module.SubclassDlgItem(IDC_MPU98MMDL, this);
	m_module.SetWindowText(np2oscfg.mpu.mdl);
	CheckDlgButton(IDC_MPU98DEFE, (np2oscfg.mpu.def_en) ? BST_CHECKED : BST_UNCHECKED);

	m_mimpifile.SubclassDlgItem(IDC_MPU98DEFF, this);
	m_mimpifile.SetWindowText(np2oscfg.mpu.def);

	m_port.SetFocus();

	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CMpu98Dlg::OnOK()
{
	UINT update = 0;

	if (np2cfg.mpuopt != m_mpu)
	{
		np2cfg.mpuopt = m_mpu;
		update |= SYS_UPDATECFG | SYS_UPDATEMIDI;
	}

	TCHAR mmap[MAXPNAMELEN];
	GetDlgItemText(IDC_MPU98MMAP, mmap, _countof(mmap));
	if (milstr_cmp(np2oscfg.mpu.mout, mmap)) {
		milstr_ncpy(np2oscfg.mpu.mout, mmap, NELEMENTS(np2oscfg.mpu.mout));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}

	TCHAR mdin[MAXPNAMELEN];
	GetDlgItemText(IDC_MPU98MDIN, mdin, _countof(mdin));
	if (milstr_cmp(np2oscfg.mpu.min, mdin))
	{
		milstr_ncpy(np2oscfg.mpu.min, mdin, NELEMENTS(np2oscfg.mpu.min));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}

	TCHAR mmdl[64];
	GetDlgItemText(IDC_MPU98MMDL, mmdl, _countof(mmdl));
	if (milstr_cmp(np2oscfg.mpu.mdl, mmdl))
	{
		milstr_ncpy(np2oscfg.mpu.mdl, mmdl, NELEMENTS(np2oscfg.mpu.mdl));
		update |= SYS_UPDATEOSCFG | SYS_UPDATEMIDI;
	}

	np2oscfg.mpu.def_en = (IsDlgButtonChecked(IDC_MPU98DEFE) != BST_UNCHECKED) ? 1 : 0;

	if (cm_mpu98)
	{
		cm_mpu98->msg(cm_mpu98, COMMSG_MIMPIDEFEN, np2oscfg.mpu.def_en);
	}

	TCHAR mdef[MAX_PATH];
	GetDlgItemText(IDC_MPU98DEFF, mdef, _countof(mdef));
	if (milstr_cmp(np2oscfg.mpu.def, mdef))
	{
		milstr_ncpy(np2oscfg.mpu.def, mdef, NELEMENTS(np2oscfg.mpu.def));
		if (cm_mpu98)
		{
			cm_mpu98->msg(cm_mpu98, COMMSG_MIMPIDEFFILE, reinterpret_cast<INTPTR>(mdef));
		}
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);

	CDlgProc::OnOK();
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL CMpu98Dlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_MPUIO:
			SetJumper(GetPort(), 0xf0);
			return TRUE;

		case IDC_MPUINT:
			SetJumper(GetInt(), 0x03);
			return TRUE;

		case IDC_MPUDEF:
			m_mpu = 0x82;
			SetPort(m_mpu);
			SetInt(m_mpu);
			m_dipsw.Invalidate();
			return TRUE;

		case IDC_MPUDIP:
			OnDipSw();
			return TRUE;

		case IDC_MPU98DEFB:
			m_mimpifile.Browse();
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
LRESULT CMpu98Dlg::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_DRAWITEM:
			if (LOWORD(wParam) == IDC_MPUDIP)
			{
				UINT8* pBitmap = dipswbmp_getmpu(m_mpu);
				m_dipsw.Draw((reinterpret_cast<LPDRAWITEMSTRUCT>(lParam))->hDC, pBitmap);
				_MFREE(pBitmap);
			}
			return FALSE;
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �f�B�b�v �X�C�b�`���N���b�N����
 */
void CMpu98Dlg::OnDipSw()
{
	RECT rect1;
	m_dipsw.GetWindowRect(&rect1);
	RECT rect2;
	m_dipsw.GetClientRect(&rect2);
	POINT p;
	GetCursorPos(&p);

	p.x += rect2.left - rect1.left;
	p.y += rect2.top - rect1.top;
	p.x /= 9;
	p.y /= 9;
	if ((p.y < 1) || (p.y >= 3))
	{
		return;
	}

	bool bRedraw = false;
	if ((p.x >= 2) && (p.x < 6))
	{
		UINT8 bit = 0x80 >> (p.x - 2);
		m_mpu ^= bit;
		SetPort(m_mpu);
		bRedraw = true;
	}
	else if ((p.x >= 9) && (p.x < 13))
	{
		UINT8 bit = (UINT8)(12 - p.x);
		if ((m_mpu ^ bit) & 3)
		{
			m_mpu &= ~0x3;
			m_mpu |= bit;
			SetInt(m_mpu);
			bRedraw = true;
		}
	}
	if (bRedraw)
	{
		m_dipsw.Invalidate();
	}
}

/**
 * �W�����p�[�̒l��ݒ�
 * @param[in] cValue �l
 * @param[in] cBit �}�X�N
 */
void CMpu98Dlg::SetJumper(UINT8 cValue, UINT8 cBit)
{
	if ((m_mpu ^ cValue) & cBit)
	{
		m_mpu &= ~cBit;
		m_mpu |= cValue;
		m_dipsw.Invalidate();
	}
}

/**
 * I/O ��ݒ�
 * @param[in] cValue �ݒ�
 */
void CMpu98Dlg::SetPort(UINT8 cValue)
{
	m_port.SetCurItemData(cValue & 0xf0);
}

/**
 * I/O ���擾
 * @return I/O
 */
UINT8 CMpu98Dlg::GetPort() const
{
	return m_port.GetCurItemData(0x00);
}

/**
 * INT ��ݒ�
 * @param[in] cValue �ݒ�
 */
void CMpu98Dlg::SetInt(UINT8 cValue)
{
	m_int.SetCurItemData(cValue & 0x03);
}

/**
 * INT ���擾
 * @return INT
 */
UINT8 CMpu98Dlg::GetInt() const
{
	return m_int.GetCurItemData(0x00);
}

/**
 * �R���t�B�O �_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_mpu98(HWND hwndParent)
{
	CMpu98Dlg dlg(hwndParent);
	dlg.DoModal();
}
