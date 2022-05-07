/**
 * @file	d_disk.cpp
 * @brief	disk dialog
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "dosio.h"
#include "np2.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "subwnd/toolwnd.h"
#include "pccore.h"
#include "common/strres.h"
#include "fdd/diskdrv.h"
#include "fdd/fddfile.h"
#include "fdd/newdisk.h"

/**
 * FDD �I���_�C�A���O
 * @param[in] hWnd �e�E�B���h�E
 * @param[in] drv �h���C�u
 */
void dialog_changefdd(HWND hWnd, REG8 drv)
{
	if (drv < 4)
	{
		LPCTSTR lpPath = fdd_diskname(drv);
		if ((lpPath == NULL) || (lpPath[0] == '\0'))
		{
			lpPath = fddfolder;
		}

		std::tstring rExt(LoadTString(IDS_FDDEXT));
		std::tstring rFilter(LoadTString(IDS_FDDFILTER));
		std::tstring rTitle(LoadTString(IDS_FDDTITLE));

		CFileDlg dlg(TRUE, rExt.c_str(), lpPath, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
		dlg.m_ofn.lpstrTitle = rTitle.c_str();
		dlg.m_ofn.nFilterIndex = 3;
		if (dlg.DoModal())
		{
			LPCTSTR lpImage = dlg.GetPathName();
			BOOL bReadOnly = dlg.GetReadOnlyPref();

			file_cpyname(fddfolder, lpImage, _countof(fddfolder));
			sysmng_update(SYS_UPDATEOSCFG);
			diskdrv_setfdd(drv, lpImage, bReadOnly);
			toolwin_setfdd(drv, lpImage);
		}
	}
}

/**
 * HDD �I���_�C�A���O
 * @param[in] hWnd �e�E�B���h�E
 * @param[in] drv �h���C�u
 */
void dialog_changehdd(HWND hWnd, REG8 drv)
{
	const UINT num = drv & 0x0f;

	UINT nTitle = 0;
	UINT nExt = 0;
	UINT nFilter = 0;
	UINT nIndex = 0;

	if (!(drv & 0x20))			// SASI/IDE
	{
		if (num < 2)
		{
#if defined(SUPPORT_SASI)
			nTitle = IDS_SASITITLE;
#else
			nTitle = IDS_HDDTITLE;
#endif
			nExt = IDS_HDDEXT;
			nFilter = IDS_HDDFILTER;
			nIndex = 4;
		}
#if defined(SUPPORT_IDEIO)
		else if (num == 2)
		{
			nTitle = IDS_ISOTITLE;
			nExt = IDS_ISOEXT;
			nFilter = IDS_ISOFILTER;
			nIndex = 3;
		}
#endif	// defined(SUPPORT_IDEIO)
	}
#if defined(SUPPORT_SCSI)
	else						// SCSI
	{
		if (num < 4)
		{
			nTitle = IDS_SCSITITLE;
			nExt = IDS_SCSIEXT;
			nFilter = IDS_SCSIFILTER;
			nIndex = 1;
		}
	}
#endif	// defined(SUPPORT_SCSI)
	if (nExt == 0)
	{
		return;
	}

	LPCTSTR lpPath = diskdrv_getsxsi(drv);
	if ((lpPath == NULL) || (lpPath[0] == '\0'))
	{
		lpPath = hddfolder;
	}

	std::tstring rExt(LoadTString(nExt));
	std::tstring rFilter(LoadTString(nFilter));
	std::tstring rTitle(LoadTString(nTitle));

	CFileDlg dlg(TRUE, rExt.c_str(), lpPath, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	dlg.m_ofn.lpstrTitle = rTitle.c_str();
	dlg.m_ofn.nFilterIndex = nIndex;
	if (dlg.DoModal())
	{
		LPCTSTR lpImage = dlg.GetPathName();
		file_cpyname(hddfolder, lpImage, _countof(hddfolder));
		sysmng_update(SYS_UPDATEOSCFG);
		diskdrv_setsxsi(drv, lpImage);
	}
}


// ---- newdisk

/** �f�t�H���g�� */
static const TCHAR str_newdisk[] = TEXT("newdisk");

/** HDD �T�C�Y */
static const UINT32 s_hddsizetbl[5] = {20, 41, 65, 80, 128};

/** SASI HDD */
static const UINT16 s_sasires[6] = 
{
	IDC_NEWSASI5MB, IDC_NEWSASI10MB,
	IDC_NEWSASI15MB, IDC_NEWSASI20MB,
	IDC_NEWSASI30MB, IDC_NEWSASI40MB
};

/**
 * @brief �V����HDD
 */
class CNewHddDlg : public CDlgProc
{
public:
	/**
	 * �R���X�g���N�^
	 * @param[in] hwndParent �e�E�B���h�E
	 * @param[in] nHddMinSize �ŏ��T�C�Y
	 * @param[in] nHddMaxSize �ő�T�C�Y
	 */
	CNewHddDlg(HWND hwndParent, UINT nHddMinSize, UINT nHddMaxSize)
		: CDlgProc(IDD_NEWHDDDISK, hwndParent)
		, m_nHddSize(0)
		, m_nHddMinSize(nHddMinSize)
		, m_nHddMaxSize(nHddMaxSize)
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~CNewHddDlg()
	{
	}

	/**
	 * �T�C�Y��Ԃ�
	 * @return �T�C�Y
	 */
	UINT GetSize() const
	{
		return m_nHddSize;
	}

protected:
	/**
	 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
	 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
	 * @retval FALSE ���ɐݒ��
	 */
	virtual BOOL OnInitDialog()
	{
		m_hddsize.SubclassDlgItem(IDC_HDDSIZE, this);
		m_hddsize.Add(s_hddsizetbl, _countof(s_hddsizetbl));

		TCHAR work[32];
		::wsprintf(work, TEXT("(%d-%dMB)"), m_nHddMinSize, m_nHddMaxSize);
		SetDlgItemText(IDC_HDDLIMIT, work);

		m_hddsize.SetFocus();
		return FALSE;
	}

	/**
	 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
	 */
	virtual void OnOK()
	{
		UINT nSize = GetDlgItemInt(IDC_HDDSIZE, NULL, FALSE);
		nSize = max(nSize, m_nHddMinSize);
		nSize = min(nSize, m_nHddMaxSize);
		m_nHddSize = nSize;
		CDlgProc::OnOK();
	}

private:
	CComboData m_hddsize;			/*!< HDD �T�C�Y �R���g���[�� */
	UINT m_nHddSize;				/*!< HDD �T�C�Y */
	UINT m_nHddMinSize;				/*!< �ŏ��T�C�Y */
	UINT m_nHddMaxSize;				/*!< �ő�T�C�Y */
};



/**
 * @brief �V����HDD
 */
class CNewSasiDlg : public CDlgProc
{
public:
	/**
	 * �R���X�g���N�^
	 * @param[in] hwndParent �e�E�B���h�E
	 */
	CNewSasiDlg(HWND hwndParent)
		: CDlgProc(IDD_NEWSASI, hwndParent)
		, m_nType(0)
	{
	}

	/**
	 * HDD �^�C�v�𓾂�
	 * @return HDD �^�C�v
	 */
	UINT GetType() const
	{
		return m_nType;
	}

protected:
	/**
	 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
	 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
	 * @retval FALSE ���ɐݒ��
	 */
	virtual BOOL OnInitDialog()
	{
		GetDlgItem(IDC_NEWSASI5MB).SetFocus();
		return FALSE;
	}

	/**
	 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
	 */
	virtual void OnOK()
	{
		for (UINT i = 0; i < 6; i++)
		{
			if (IsDlgButtonChecked(s_sasires[i]) != BST_UNCHECKED)
			{
				m_nType = (i > 3) ? (i + 1) : i;
				CDlgProc::OnOK();
				break;
			}
		}
	}

private:
	UINT m_nType;			/*!< HDD �^�C�v */
};

/**
 * @brief �V����FDD
 */
class CNewFddDlg : public CDlgProc
{
public:
	/**
	 * �R���X�g���N�^
	 * @param[in] hwndParent �e�E�B���h�E
	 */
	CNewFddDlg(HWND hwndParent)
		: CDlgProc((np2cfg.usefd144) ? IDD_NEWDISK2 : IDD_NEWDISK, hwndParent)
		, m_nFdType(DISKTYPE_2HD << 4)
	{
	}

	/**
	 * �^�C�v�𓾂�
	 * @return �^�C�v
	 */
	UINT8 GetType() const
	{
		return m_nFdType;
	}

	/**
	 * ���x���𓾂�
	 * @return ���x��
	 */
	LPCTSTR GetLabel() const
	{
		return m_szDiskLabel;
	}

protected:
	/**
	 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
	 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
	 * @retval FALSE ���ɐݒ��
	 */
	virtual BOOL OnInitDialog()
	{
		UINT res;
		switch (m_nFdType)
		{
			case (DISKTYPE_2DD << 4):
				res = IDC_MAKE2DD;
				break;

			case (DISKTYPE_2HD << 4):
				res = IDC_MAKE2HD;
				break;

			default:
				res = IDC_MAKE144;
				break;
		}
		CheckDlgButton(res, BST_CHECKED);
		GetDlgItem(IDC_DISKLABEL).SetFocus();
		return FALSE;
	}

	/**
	 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
	 */
	virtual void OnOK()
	{
		GetDlgItemText(IDC_DISKLABEL, m_szDiskLabel, _countof(m_szDiskLabel));
		if (milstr_kanji1st(m_szDiskLabel, _countof(m_szDiskLabel) - 1))
		{
			m_szDiskLabel[_countof(m_szDiskLabel) - 1] = '\0';
		}
		if (IsDlgButtonChecked(IDC_MAKE2DD) != BST_UNCHECKED)
		{
			m_nFdType = (DISKTYPE_2DD << 4);
		}
		else if (IsDlgButtonChecked(IDC_MAKE2HD) != BST_UNCHECKED)
		{
			m_nFdType = (DISKTYPE_2HD << 4);
		}
		else
		{
			m_nFdType = (DISKTYPE_2HD << 4) + 1;
		}
		CDlgProc::OnOK();
	}

private:
	UINT m_nFdType;					/*!< �^�C�v */
	TCHAR m_szDiskLabel[16 + 1];	/*!< ���x�� */
};

/**
 * �V�K�f�B�X�N�쐬 �_�C�A���O
 * @param[in] hWnd �e�E�B���h�E
 */
void dialog_newdisk(HWND hWnd)
{
	TCHAR szPath[MAX_PATH];
	file_cpyname(szPath, fddfolder, _countof(szPath));
	file_cutname(szPath);
	file_catname(szPath, str_newdisk, _countof(szPath));

	std::tstring rTitle(LoadTString(IDS_NEWDISKTITLE));
	std::tstring rDefExt(LoadTString(IDS_NEWDISKEXT));
#if defined(SUPPORT_SCSI)
	std::tstring rFilter(LoadTString(IDS_NEWDISKFILTER));
#else	// defined(SUPPORT_SCSI)
	std::tstring rFilter(LoadTString(IDS_NEWDISKFILTER2));
#endif	// defined(SUPPORT_SCSI)

	CFileDlg fileDlg(FALSE, rDefExt.c_str(), szPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	fileDlg.m_ofn.lpstrTitle = rTitle.c_str();
	if (fileDlg.DoModal() != IDOK)
	{
		return;
	}

	LPCTSTR lpPath = fileDlg.GetPathName();
	LPCTSTR ext = file_getext(lpPath);
	if (!file_cmpname(ext, str_thd))
	{
		CNewHddDlg dlg(hWnd, 5, 256);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_thd(lpPath, dlg.GetSize());
		}
	}
	else if (!file_cmpname(ext, str_nhd))
	{
		CNewHddDlg dlg(hWnd, 5, 512);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_nhd(lpPath, dlg.GetSize());
		}
	}
	else if (!file_cmpname(ext, str_hdi))
	{
		CNewSasiDlg dlg(hWnd);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_hdi(lpPath, dlg.GetType());
		}
	}
#if defined(SUPPORT_SCSI)
	else if (!file_cmpname(ext, str_hdd))
	{
		CNewHddDlg dlg(hWnd, 2, 512);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_vhd(lpPath, dlg.GetSize());
		}
	}
#endif
	else if ((!file_cmpname(ext, str_d88)) ||
			(!file_cmpname(ext, str_d98)) ||
			(!file_cmpname(ext, str_88d)) ||
			(!file_cmpname(ext, str_98d)))
	{
		CNewFddDlg dlg(hWnd);
		if (dlg.DoModal()  == IDOK)
		{
			newdisk_fdd(lpPath, dlg.GetType(), dlg.GetLabel());
		}
	}
}
