/**
 * @file	PropProc.cpp
 * @brief	�v���p�e�B �V�[�g �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "PropProc.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

// ---- �v���p�e�B �y�[�W

/**
 * �R���X�g���N�^
 * @param[in] nIDTemplate ���̃y�[�W�Ɏg�p����e���v���[�g�� ID
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
CPropPageProc::CPropPageProc(UINT nIDTemplate, UINT nIDCaption)
{
	Construct(MAKEINTRESOURCE(nIDTemplate), nIDCaption);
}

/**
 * �R���X�g���N�^
 * @param[in] lpszTemplateName ���̃y�[�W�̃e���v���[�g�̖��O���܂ޕ�����ւ̃|�C���^�[
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
CPropPageProc::CPropPageProc(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
	Construct(lpszTemplateName, nIDCaption);
}

/**
 * �f�X�g���N�^
 */
CPropPageProc::~CPropPageProc()
{
	if (m_lpCaption)
	{
		free(m_lpCaption);
	}
}

/**
 * �R���X�g���N�g
 * @param[in] nIDTemplate ���̃y�[�W�Ɏg�p����e���v���[�g�� ID
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
void CPropPageProc::Construct(UINT nIDTemplate, UINT nIDCaption)
{
	Construct(MAKEINTRESOURCE(nIDTemplate), nIDCaption);
}

/**
 * �R���X�g���N�g
 * @param[in] lpszTemplateName ���̃y�[�W�̃e���v���[�g�̖��O���܂ޕ�����ւ̃|�C���^�[
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
void CPropPageProc::Construct(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
	ZeroMemory(&m_psp, sizeof(m_psp));
	m_psp.dwSize = sizeof(m_psp);
	m_psp.dwFlags = PSP_USECALLBACK;
	m_psp.hInstance = FindResourceHandle(lpszTemplateName, RT_DIALOG);
	m_psp.pszTemplate = lpszTemplateName;
	m_psp.pfnDlgProc = DlgProc;
	m_psp.lParam = reinterpret_cast<LPARAM>(this);
	m_psp.pfnCallback = PropPageCallback;

	m_lpCaption = NULL;
	if (nIDCaption)
	{
		std::tstring rTitle(LoadTString(nIDCaption));
		m_lpCaption = _tcsdup(rTitle.c_str());
		m_psp.pszTitle = m_lpCaption;
		m_psp.dwFlags |= PSP_USETITLE;
	}
}

/**
 * �v���p�e�B �y�[�W �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ���b�Z�[�W
 * @param[in] pPropPage ���̃v���p�e�B �V�[�g �y�[�W�̃|�C���^
 * @return 0
 */
UINT CALLBACK CPropPageProc::PropPageCallback(HWND hWnd, UINT message, LPPROPSHEETPAGE pPropPage)
{
	switch (message)
	{
		case PSPCB_CREATE:
			HookWindowCreate(reinterpret_cast<CPropPageProc*>(pPropPage->lParam));
			return TRUE;

		case PSPCB_RELEASE:
			UnhookWindowCreate();
			break;
	}
	return 0;
}

/**
 * �t���[�����[�N�́A�C�x���g���R���g���[���ɔ�������ꍇ��A�R���g���[�����ꕔ�̎�ނ̏���v������R���g���[����e�E�B���h�E�ɒʒm���邽�߂ɁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] wParam ���b�Z�[�W���R���g���[�����炻�̃��b�Z�[�W�𑗐M����R���g���[�������ʂ��܂�
 * @param[in] lParam �ʒm�R�[�h�ƒǉ������܂ޒʒm���b�Z�[�W (NMHDR) �̍\���̂ւ̃|�C���^�[
 * @param[out] pResult ���b�Z�[�W���������ꂽ�Ƃ����ʂ��i�[����R�[�h���� LRESULT �̕ϐ��ւ̃|�C���^�[
 * @retval TRUE ���b�Z�[�W����������
 * @retval FALSE ���b�Z�[�W���������Ȃ�����
 */
BOOL CPropPageProc::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);

	// allow message map to override
	if (CDlgProc::OnNotify(wParam, lParam, pResult))
	{
		return TRUE;
	}

	// don't handle messages not from the page/sheet itself
	if (pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd))
	{
		return FALSE;
	}

	// handle default
	switch (pNMHDR->code)
	{
		case PSN_APPLY:
			*pResult = OnApply() ? PSNRET_NOERROR : PSNRET_INVALID_NOCHANGEPAGE;
			break;

		case PSN_RESET:
			OnReset();
			break;

		default:
			return FALSE;   // not handled
	}

	return TRUE;    // handled
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N�ɂ���� OnKillActive�t���[�����[�N�����Ăяo��������Ƀ��[�U�[��[OK]��I�����邩�A�X�V���ɌĂяo����܂�
 * @retval TRUE �ύX�����F���ꂽ
 * @retval FALSE �ύX�����F����Ȃ�����
 */
BOOL CPropPageProc::OnApply()
{
	OnOK();
	return TRUE;
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N�ɂ���ă��[�U�[��[�L�����Z��]��I������Ƃ��ɌĂяo����܂��B
 */
void CPropPageProc::OnReset()
{
	OnCancel();
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N�ɂ���� OnKillActive�t���[�����[�N�����Ăяo��������Ƀ��[�U�[��[OK]��I�����邩�A�X�V���ɌĂяo����܂�
 */
void CPropPageProc::OnOK()
{
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N��[�L�����Z��]�{�^�����I�����ꂽ�Ƃ��ɌĂяo����܂�
 */
void CPropPageProc::OnCancel()
{
}

// ---- �v���p�e�B �V�[�g

/**
 * �R���X�g���N�^
 */
CPropSheetProc::CPropSheetProc()
{
	CommonConstruct(NULL, 0);
}

/**
 * �R���X�g���N�^
 * @param[in] nIDCaption �L���v�V����
 * @param[in] hwndParent �e�E�B���h�E
 * @param[in] iSelectPage �X�^�[�g �y�[�W
 */
CPropSheetProc::CPropSheetProc(UINT nIDCaption, HWND hwndParent, UINT iSelectPage)
{
	m_strCaption = LoadTString(nIDCaption);
	CommonConstruct(hwndParent, iSelectPage);
}

/**
 * �R���X�g���N�^
 * @param[in] pszCaption �L���v�V����
 * @param[in] hwndParent �e�E�B���h�E
 * @param[in] iSelectPage �X�^�[�g �y�[�W
 */
CPropSheetProc::CPropSheetProc(LPCTSTR pszCaption, HWND hwndParent, UINT iSelectPage)
{
	m_strCaption = pszCaption;
	CommonConstruct(hwndParent, iSelectPage);
}

/**
 * �R���X�g���N�g
 * @param[in] hwndParent �e�E�B���h�E
 * @param[in] iSelectPage �X�^�[�g �y�[�W
 */
void CPropSheetProc::CommonConstruct(HWND hwndParent, UINT iSelectPage)
{
	ZeroMemory(&m_psh, sizeof(m_psh));
	m_psh.dwSize = sizeof(m_psh);
	m_psh.hwndParent = hwndParent;
	m_psh.hInstance = CWndProc::GetResourceHandle();
	m_psh.nStartPage = iSelectPage;
}

/**
 * ���[�_��
 * @return ���U���g �R�[�h
 */
INT_PTR CPropSheetProc::DoModal()
{
	m_psh.pszCaption = m_strCaption.c_str();
	m_psh.nPages = static_cast<UINT>(m_pages.size());
	m_psh.phpage = new HPROPSHEETPAGE[m_psh.nPages];
	for (UINT i = 0; i < m_pages.size(); i++)
	{
		m_psh.phpage[i] = ::CreatePropertySheetPage(&m_pages[i]->m_psp);
	}

	const INT_PTR r = ::PropertySheet(&m_psh);

	delete[] m_psh.phpage;
	m_psh.phpage = NULL;

	return r;
}

/**
 * �y�[�W�̒ǉ�
 * @param[in] pPage �y�[�W
 */
void CPropSheetProc::AddPage(CPropPageProc* pPage)
{
	m_pages.push_back(pPage);
}
