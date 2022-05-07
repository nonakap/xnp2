/**
 * @file	DlgProc.cpp
 * @brief	�_�C�A���O �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "DlgProc.h"

/**
 * �R���X�g���N�^
 */
CDlgProc::CDlgProc()
	: m_lpszTemplateName(NULL)
	, m_hwndParent(NULL)
{
}

/**
 * �R���X�g���N�^
 * @param[in] nIDTemplate �_�C�A���O �{�b�N�X �e���v���[�g�̃��\�[�X id �ԍ����w�肵�܂�
 * @param[in] hwndParent �e�E�B���h�E
 */
CDlgProc::CDlgProc(UINT nIDTemplate, HWND hwndParent)
	: m_lpszTemplateName(MAKEINTRESOURCE(nIDTemplate))
	, m_hwndParent(hwndParent)
{
}

/**
 * �f�X�g���N�^
 */
CDlgProc::~CDlgProc()
{
}

/**
 * ���[�_��
 * @return �_�C�A���O �{�b�N�X����邽�߂Ɏg�p�����ACDialog::EndDialog �̃����o�[�֐��ɓn���ꂽ nResult �̃p�����[�^�[�l���w�肷�� int �̒l
 */
INT_PTR CDlgProc::DoModal()
{
	HookWindowCreate(this);

	HINSTANCE hInstance = FindResourceHandle(m_lpszTemplateName, RT_DIALOG);
	const INT_PTR nRet = ::DialogBox(hInstance, m_lpszTemplateName, m_hwndParent, DlgProc);

	if (!UnhookWindowCreate())
	{
		PostNcDestroy();
	}
	return nRet;
}

/**
 * �_�C�A���O �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
#if defined(_WIN64)
INT_PTR CALLBACK CDlgProc::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
#else	// defined(_WIN64)
BOOL CALLBACK CDlgProc::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
#endif	// defined(_WIN64)
{
	if (message == WM_INITDIALOG)
	{
		CDlgProc* pDlg = static_cast<CDlgProc*>(FromHandlePermanent(hWnd));
		if (pDlg != NULL)
		{
			return pDlg->OnInitDialog();
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * CDlgProc �I�u�W�F�N�g�� Windows �v���V�[�W��
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CDlgProc::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
			case IDOK:
				OnOK();
				return TRUE;

			case IDCANCEL:
				OnCancel();
				return TRUE;
		}
	}
	return CWndProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CDlgProc::OnInitDialog()
{
	return TRUE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CDlgProc::OnOK()
{
	EndDialog(IDOK);
}

/**
 * �t���[�����[�N�́A���[�U�[�� [�L�����Z��] ���N���b�N���邩�A���[�_���܂��̓��[�h���X �_�C�A���O �{�b�N�X�� Esc �L�[���������Ƃ��ɂ��̃��\�b�h���Ăяo���܂�
 */
void CDlgProc::OnCancel()
{
	EndDialog(IDCANCEL);
}



/**
 * �R���X�g���N�^
 * @param[in] bOpenFileDialog �쐬����_�C�A���O �{�b�N�X���w�肷��p�����[�^�[
 * @param[in] lpszDefExt ����̃t�@�C�����̊g���q�ł�
 * @param[in] lpszFileName �{�b�N�X�ɕ\������鏉���t�@�C����
 * @param[in] dwFlags �t���O
 * @param[in] lpszFilter �t�B���^�[
 * @param[in] hParentWnd �e�E�B���h�E
 */
CFileDlg::CFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, HWND hParentWnd)
	: m_bOpenFileDialog(bOpenFileDialog)
{
	ZeroMemory(&m_ofn, sizeof(m_ofn));
	m_szFileName[0] = '\0';
	m_szFileTitle[0] = '\0';

	m_ofn.lStructSize = sizeof(m_ofn);
	m_ofn.lpstrFile = m_szFileName;
	m_ofn.nMaxFile = _countof(m_szFileName);
	m_ofn.lpstrDefExt = lpszDefExt;
	m_ofn.lpstrFileTitle = m_szFileTitle;
	m_ofn.nMaxFileTitle = _countof(m_szFileTitle);
	m_ofn.Flags = dwFlags;
	m_ofn.hwndOwner = hParentWnd;

	// setup initial file name
	if (lpszFileName != NULL)
	{
		lstrcpyn(m_szFileName, lpszFileName, _countof(m_szFileName));
	}

	// Translate filter into commdlg format (lots of \0)
	if (lpszFilter != NULL)
	{
		m_strFilter = lpszFilter;
		for (std::tstring::iterator it = m_strFilter.begin(); it != m_strFilter.end(); ++it)
		{
#if !defined(_UNICODE)
			if (IsDBCSLeadByte(static_cast<BYTE>(*it)))
			{
				++it;
				if (it == m_strFilter.end())
				{
					break;
				}
				continue;
			}
#endif	// !defined(_UNICODE)
			if (*it == '|')
			{
				*it = '\0';
			}
		}
		m_ofn.lpstrFilter = m_strFilter.c_str();
	}
}

/**
 * ���[�_��
 * @return ���U���g �R�[�h
 */
int CFileDlg::DoModal()
{
	int nResult;
	if (m_bOpenFileDialog)
	{
		nResult = ::GetOpenFileName(&m_ofn);
	}
	else
	{
		nResult = ::GetSaveFileName(&m_ofn);
	}
	return nResult;
}
