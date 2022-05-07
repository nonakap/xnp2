/**
 * @file	d_clnd.cpp
 * @brief	�J�����_�ݒ�_�C�A���O
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "np2.h"
#include "sysmng.h"
#include "timemng.h"
#include "misc/DlgProc.h"
#include "calendar.h"
#include "pccore.h"
#include "common/strres.h"

/**
 * @brief �J�����_�ݒ�_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
class CCalendarDlg : public CDlgProc
{
public:
	CCalendarDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	void SetTime(const UINT8* cbuf);
	void EnableVirtualCalendar(BOOL bEnabled);
	static UINT8 getbcd(LPCTSTR str, int len);
};

/**
 * @brief �_�C�A���O �A�C�e��
 */
struct Item
{
	UINT16	res;		//!< ID
	UINT8	min;		//!< �ŏ��l
	UINT8	max;		//!< �ő�l
};

/**
 * �A�C�e��
 */
static const Item s_vircal[6] =
{
	{IDC_VIRYEAR,	0x00, 0x99},
	{IDC_VIRMONTH,	0x01, 0x12},
	{IDC_VIRDAY,	0x01, 0x31},
	{IDC_VIRHOUR,	0x00, 0x23},
	{IDC_VIRMINUTE,	0x00, 0x59},
	{IDC_VIRSECOND,	0x00, 0x59}
};

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 */
CCalendarDlg::CCalendarDlg(HWND hwndParent)
	: CDlgProc(IDD_CALENDAR, hwndParent)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CCalendarDlg::OnInitDialog()
{
	// ���Ԃ��Z�b�g�B
	UINT8 cbuf[6];
	calendar_getvir(cbuf);
	SetTime(cbuf);

	const UINT nID = (np2cfg.calendar) ? IDC_CLNDREAL : IDC_CLNDVIR;
	EnableVirtualCalendar((nID == IDC_CLNDVIR) ? TRUE : FALSE);
	CheckDlgButton(nID, BST_CHECKED);
	GetDlgItem(nID).SetFocus();
	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CCalendarDlg::OnOK()
{
	const UINT8 bMode = (IsDlgButtonChecked(IDC_CLNDREAL) != BST_UNCHECKED) ? 1 : 0;
	if (np2cfg.calendar != bMode)
	{
		np2cfg.calendar = bMode;
		sysmng_update(SYS_UPDATECFG);
	}

	UINT8 cbuf[6];
	for (UINT i = 0; i < 6; i++)
	{
		TCHAR work[32];
		GetDlgItemText(s_vircal[i].res, work, NELEMENTS(work));
		UINT8 b = getbcd(work, 2);
		if ((b >= s_vircal[i].min) && (b <= s_vircal[i].max))
		{
			if (i == 1)
			{
				b = ((b & 0x10) * 10) + (b << 4);
			}
			cbuf[i] = b;
		}
	}
	calendar_set(cbuf);
	
	CDlgProc::OnOK();
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL CCalendarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_CLNDVIR:
			EnableVirtualCalendar(TRUE);
			return TRUE;

		case IDC_CLNDREAL:
			EnableVirtualCalendar(FALSE);
			return TRUE;

		case IDC_SETNOW:
			{
				UINT8 cbuf[6];
				calendar_getreal(cbuf);
				SetTime(cbuf);
			}
			return TRUE;
	}
	return FALSE;
}

/**
 * ���Ԃ�ݒ肷��
 * @param[in] cbuf �J�����_���
 */
void CCalendarDlg::SetTime(const UINT8* cbuf)
{
	for (UINT i = 0; i < 6; i++)
	{
		TCHAR work[8];
		if (i != 1)
		{
			wsprintf(work, str_2x, cbuf[i]);
		}
		else
		{
			wsprintf(work, str_2d, cbuf[1] >> 4);
		}
		SetDlgItemText(s_vircal[i].res, work);
	}
}

/**
 * ���z�J�����_ �A�C�e���̈ꊇ�ݒ�
 * @param[in] bEnabled �L���t���O
 */
void CCalendarDlg::EnableVirtualCalendar(BOOL bEnabled)
{
	for (UINT i = 0; i < 6; i++)
	{
		GetDlgItem(s_vircal[i].res).EnableWindow(bEnabled);
	}
	GetDlgItem(IDC_SETNOW).EnableWindow(bEnabled);
}

/**
 * BCD �𓾂�
 * @param[in] str ������
 * @param[in] len ����
 * @return �l
 */
UINT8 CCalendarDlg::getbcd(LPCTSTR str, int len)
{
	UINT ret = 0;
	while (len--)
	{
		TCHAR c = *str++;
		if (!c)
		{
			break;
		}
		if ((c < '0') || (c > '9'))
		{
			return 0xff;
		}
		ret <<= 4;
		ret |= (UINT)(c - '0');
	}
	return static_cast<UINT8>(ret);
}

/**
 * �J�����_�ݒ�_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_calendar(HWND hwndParent)
{
	CCalendarDlg dlg(hwndParent);
	dlg.DoModal();
}
