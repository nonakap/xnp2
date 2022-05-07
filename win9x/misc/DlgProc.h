/**
 * @file	DlgProc.h
 * @brief	�_�C�A���O �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <commctrl.h>
#include "tstring.h"
#include "WndProc.h"

/**
 * @brief �_�C�A���O �N���X
 */
class CDlgProc : public CWndProc
{
public:
	CDlgProc();
	CDlgProc(UINT nIDTemplate, HWND hwndParent = NULL);
	virtual ~CDlgProc();
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();

	/**
	 * ���[�_�� �_�C�A���O �{�b�N�X���I������
	 * @param[in] nResult DoModal�̌Ăяo�����ɕԂ��l
	 */
	void CDlgProc::EndDialog(int nResult)
	{
		::EndDialog(m_hWnd, nResult);
	}

protected:
	LPCTSTR m_lpszTemplateName;		//!< �e���v���[�g��
	HWND m_hwndParent;				//!< �e�E�B���h�E

	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

#if defined(_WIN64)
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#else	// defined(_WIN64)
	static BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif	// defined(_WIN64)
};


/**
 * @brief �R���{ �{�b�N�X
 */
class CComboBoxProc : public CWndProc
{
public:
	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�ɕ������ǉ����܂�
	 * @param[in] lpszString �ǉ����ꂽ null �ŏI��镶����ւ̃|�C���^�[
	 * @return �����񂪑}�����ꂽ�ʒu�������C���f�b�N�X
	 */
	int AddString(LPCTSTR lpszString)
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(lpszString)));
	}

	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�����̍��ڐ����擾����ɂ́A���̃����o�[�֐����Ăяo���܂�
	 * @return ���ڂ̐�
	 */
	int GetCount() const
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0));
	}

	/**
	 * �R���{ �{�b�N�X�̂ǂ̍��ڂ��I�����ꂽ���𔻒肷�邽�߂ɂ��̃����o�[�֐����Ăяo���܂�
	 * @return �R���{ �{�b�N�X�̃��X�g �{�b�N�X�Ō��ݑI������Ă��鍀�ڂ̃C���f�b�N�X
	 */
	int GetCurSel() const
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_GETCURSEL, 0, 0));
	}

	/**
	 * �w�肵���R���{ �{�b�N�X�̍��ڂɊ֘A�t����ꂽ�A�v���P�[�V�����ɗp�ӂ��ꂽ 32 �r�b�g�l���擾���܂�
	 * @param[in] nIndex �R���{ �{�b�N�X�̃��X�g �{�b�N�X�̍��ڂ̃C���f�b�N�X
	 * @return 32 �r�b�g�l
	 */
	DWORD_PTR GetItemData(int nIndex) const
	{
		return static_cast<DWORD_PTR>(::SendMessage(m_hWnd, CB_GETITEMDATA, static_cast<WPARAM>(nIndex), 0));
	}

	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�Ŏw�肳��Ă���v���t�B�b�N�X���܂ލŏ��̕�������������܂�
	 * @param[in] nStartAfter ��������ŏ��̍��ڂ̑O�̍��ڂ̃C���f�b�N�X
	 * @param[in] lpszString �������镶����
	 * @return �C���f�b�N�X
	 */
	int FindString(int nStartAfter, LPCTSTR lpszString) const
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_FINDSTRING, nStartAfter, reinterpret_cast<LPARAM>(lpszString)));
	}

	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�Ŏw�肳��Ă���ŏ��̕�������������܂�
	 * @param[in] nStartAfter ��������ŏ��̍��ڂ̑O�̍��ڂ̃C���f�b�N�X
	 * @param[in] lpszString �������镶����
	 * @return �C���f�b�N�X
	 */
	int FindStringExact(int nStartAfter, LPCTSTR lpszString) const
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, nStartAfter, reinterpret_cast<LPARAM>(lpszString)));
	}

	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�ɕ������ǉ����܂�
	 * @param[in] nIndex ��������󂯎�郊�X�g �{�b�N�X�̈ʒu
	 * @param[in] lpszString �ǉ����ꂽ null �ŏI��镶����ւ̃|�C���^�[
	 * @return �����񂪑}�����ꂽ�ʒu�������C���f�b�N�X
	 */
	int InsertString(int nIndex, LPCTSTR lpszString)
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_INSERTSTRING, static_cast<WPARAM>(nIndex), reinterpret_cast<LPARAM>(lpszString)));
	}

	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�ƃG�f�B�b�g �R���g���[�����炷�ׂĂ̍��ڂ��폜���܂��B
	 */
	void ResetContent()
	{
		::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0);
	}

	/**
	 * 32 �r�b�g�l���R���{ �{�b�N�X�̎w�荀�ڂɊ֘A�t������
	 * @param[in] nIndex ���ڂɎn�܂�C���f�b�N�X��ݒ肷�邽�߂̃��\�b�h���܂܂�܂�
	 * @param[in] dwItemData �V�����l�����ڂɊ֘A�t����Ɋ܂܂�Ă��܂�
	 * @return �G���[�̎��� CB_ERR
	 */
	int SetItemData(int nIndex, DWORD_PTR dwItemData)
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_SETITEMDATA, static_cast<WPARAM>(nIndex), static_cast<LPARAM>(dwItemData)));
	}

	/**
	 * �R���{ �{�b�N�X�̃��X�g �{�b�N�X�̕������I�����܂�
	 * @param[in] nSelect ������̃C���f�b�N�X��I������悤�Ɏw�肵�܂�
	 * @return ���b�Z�[�W�����������ꍇ�͑I�����ꂽ���ڂ̃C���f�b�N�X
	 */
	int SetCurSel(int nSelect)
	{
		return static_cast<int>(::SendMessage(m_hWnd, CB_SETCURSEL, static_cast<WPARAM>(nSelect), 0));
	}
};

/**
 * @brief �X���C�_�[ �{�b�N�X
 */
class CSliderProc : public CWndProc
{
public:
	/**
	 * �X���C�_�[�̌��݈ʒu���擾���܂�
	 * @return ���݈ʒu��Ԃ��܂�
	 */
	int GetPos() const
	{
		return static_cast<int>(::SendMessage(m_hWnd, TBM_GETPOS, 0, 0));
	}

	/**
	 * �X���C�_�[ �R���g���[���ŃX���C�_�[�̍ŏ��͈͂�ݒ肵�܂�
	 * @param[in] nMin �X���C�_�[�̍ŏ��̈ʒu
	 * @param[in] bRedraw �ĕ`��̃t���O
	 */
	void SetRangeMin(int nMin, BOOL bRedraw)
	{
		::SendMessage(m_hWnd, TBM_SETRANGEMIN, bRedraw, nMin);
	}

	/**
	 * �X���C�_�[ �R���g���[���ŃX���C�_�[�̍ő�͈͂�ݒ肵�܂�
	 * @param[in] nMax �X���C�_�[�̍ő�̈ʒu
	 * @param[in] bRedraw �ĕ`��̃t���O
	 */
	void SetRangeMax(int nMax, BOOL bRedraw)
	{
		::SendMessage(m_hWnd, TBM_SETRANGEMAX, bRedraw, nMax);
	}

	/**
	 * �X���C�_�[�̌��݈ʒu��ݒ肵�܂�
	 * @param[in] nPos �V�����X���C�_�[�̈ʒu���w�肵�܂�
	 */
	void SetPos(int nPos)
	{
		::SendMessage(m_hWnd, TBM_SETPOS, TRUE, nPos);
	}
};

/**
 * @brief �t�@�C���I��
 */
class CFileDlg
{
public:
	CFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, HWND hParentWnd = NULL);
	int DoModal();

	/**
	 * �t�@�C�����̎擾
	 * @return full path and filename
	 */
	LPCTSTR GetPathName() const
	{
		return m_ofn.lpstrFile;
	}

	/**
	 * Readoly?
	 * @return TRUE if readonly checked
	 */
	BOOL GetReadOnlyPref() const
	{
		return (m_ofn.Flags & OFN_READONLY) ? TRUE : FALSE;
	}

public:
	OPENFILENAME m_ofn;				//!< open file parameter block

protected:
	BOOL m_bOpenFileDialog;			//!< TRUE for file open, FALSE for file save
	std::tstring m_strFilter;		//!< filter string
	TCHAR m_szFileTitle[64];		//!< contains file title after return
	TCHAR m_szFileName[_MAX_PATH];	//!< contains full path name after return
};
