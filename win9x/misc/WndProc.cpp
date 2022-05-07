/**
 * @file	WndProc.cpp
 * @brief	�v���V�[�W�� �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "WndProc.h"
#include <assert.h>
#define ASSERT assert	/*!< assert */

//! ���N���X��
// static const TCHAR s_szClassName[] = TEXT("WndProcBase");

//! �C���X�^���X
HINSTANCE CWndProc::sm_hInstance;
//! ���\�[�X
HINSTANCE CWndProc::sm_hResource;

DWORD CWndProc::sm_dwThreadId;						//!< �����̃X���b�h ID
HHOOK CWndProc::sm_hHookOldCbtFilter;				//!< �t�b�N �t�B���^�[
CWndProc* CWndProc::sm_pWndInit;					//!< ���������̃C���X�^���X
std::map<HWND, CWndProc*>* CWndProc::sm_pWndMap;	//!< �E�B���h�E �}�b�v

/**
 * ������
 * @param[in] hInstance �C���X�^���X
 */
void CWndProc::Initialize(HINSTANCE hInstance)
{
	sm_hInstance = hInstance;
	sm_hResource = hInstance;

	sm_dwThreadId = ::GetCurrentThreadId();
	sm_hHookOldCbtFilter = ::SetWindowsHookEx(WH_CBT, CbtFilterHook, NULL, sm_dwThreadId);

	sm_pWndMap = new std::map<HWND, CWndProc*>;

#if 0
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	wc.lpszClassName = s_szClassName;
	::RegisterClass(&wc);
#endif	// 0
}

/**
 * ���
 */
void CWndProc::Deinitialize()
{
	if (sm_hHookOldCbtFilter != NULL)
	{
		::UnhookWindowsHookEx(sm_hHookOldCbtFilter);
		sm_hHookOldCbtFilter = NULL;
	}
}

/**
 * ���\�[�X�̌���
 * @param[in] lpszName ���\�[�X ID
 * @param[in] lpszType ���\�[�X�̌^�ւ̃|�C���^
 * @return �C���X�^���X
 */
HINSTANCE CWndProc::FindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType)
{
	HINSTANCE hInst = GetResourceHandle();
	if ((hInst != GetInstanceHandle()) && (::FindResource(hInst, lpszName, lpszType) != NULL))
	{
		return hInst;
	}
	return GetInstanceHandle();
}

/**
 * �R���X�g���N�^
 */
CWndProc::CWndProc()
	: CWndBase(NULL)
	, m_pfnSuper(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CWndProc::~CWndProc()
{
	DestroyWindow();
}

/**
 * �E�B���h�E�̃n���h�����w�肳��Ă���ꍇ�ACWndProc �I�u�W�F�N�g�ւ̃|�C���^�[��Ԃ��܂�
 * @param[in] hWnd �E�B���h�E �n���h��
 * @return �|�C���^
 */
CWndProc* CWndProc::FromHandlePermanent(HWND hWnd)
{
	std::map<HWND, CWndProc*>* pMap = sm_pWndMap;
	if (pMap)
	{
		std::map<HWND, CWndProc*>::iterator it = pMap->find(hWnd);
		if (it != pMap->end())
		{
			return it->second;
		}
	}
	return NULL;
}

/**
 * Windows �̃E�B���h�E���A�^�b�`���܂�
 * @param[in] hWndNew �E�B���h�E �n���h��
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CWndProc::Attach(HWND hWndNew)
{
	std::map<HWND, CWndProc*>* pMap = sm_pWndMap;
	if ((hWndNew != NULL) && (pMap))
	{
		(*pMap)[hWndNew] = this;
		m_hWnd = hWndNew;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * Windows �̃n���h����؂藣���A���̃n���h����Ԃ��܂�
 * @return �E�B���h�E �n���h��
 */
HWND CWndProc::Detach()
{
	HWND hWnd = m_hWnd;
	if (hWnd != NULL)
	{
		std::map<HWND, CWndProc*>* pMap = sm_pWndMap;
		if (pMap)
		{
			std::map<HWND, CWndProc*>::iterator it = pMap->find(hWnd);
			if (it != pMap->end())
			{
				pMap->erase(it);
			}
		}
		m_hWnd = NULL;
	}
	return hWnd;
}

/**
 * ���̃����o�[�֐��̓E�B���h�E���T�u�N���X������O�ɁA�ق��̃T�u�N���X���ɕK�v�ȑ���������邽�߂Ƀt���[�����[�N����Ă΂�܂�
 */
void CWndProc::PreSubclassWindow()
{
	// no default processing
}

/**
 * �E�B���h�E�𓮓I�T�u�N���X�����ACWnd �I�u�W�F�N�g�Ɍ��ѕt���邽�߂ɂ��̃����o�[�֐����Ăяo���܂�
 * @param[in] hWnd �E�B���h�E �n���h��
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CWndProc::SubclassWindow(HWND hWnd)
{
	if (!Attach(hWnd))
	{
		return FALSE;
	}

	PreSubclassWindow();

	WNDPROC newWndProc = &CWndProc::WndProc;
	WNDPROC oldWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(newWndProc)));
	if ((m_pfnSuper == NULL) && (oldWndProc != newWndProc))
	{
		m_pfnSuper = oldWndProc;
	}
	return TRUE;
}

/**
 * �R���g���[���𓮓I�T�u�N���X�����ACWnd �I�u�W�F�N�g�Ɍ��ѕt���邽�߂ɂ��̃����o�[�֐����Ăяo���܂�
 * @param[in] nID �R���g���[�� ID
 * @param[in] pParent �R���g���[���̐e
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CWndProc::SubclassDlgItem(UINT nID, CWndProc* pParent)
{
	HWND hWndControl = ::GetDlgItem(pParent->m_hWnd, nID);
	if (hWndControl != NULL)
	{
		return SubclassWindow(hWndControl);
	}
	return FALSE;
}

/**
 * WndProc �Ɍ��̒l��ݒ肵�� CWnd �I�u�W�F�N�g���� HWND �Ŏ��ʂ����E�B���h�E��؂藣�����߂ɁA���̃����o�[�֐����Ăяo���܂�
 * @return ��T�u�N���X�����ꂽ�E�B���h�E�ւ̃n���h��
 */
HWND CWndProc::UnsubclassWindow()
{
	if (m_pfnSuper != NULL)
	{
		::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_pfnSuper));
		m_pfnSuper = NULL;
	}
	return Detach();
}

/**
 * �w�肳�ꂽ�E�B���h�E���쐬���A����� CWndProc �I�u�W�F�N�g�ɃA�^�b�`���܂�
 * @param[in] dwExStyle �g���E�B���h�E �X�^�C��
 * @param[in] lpszClassName �o�^����Ă���V�X�e�� �E�B���h�E �N���X�̖��O
 * @param[in] lpszWindowName �E�B���h�E�̕\����
 * @param[in] dwStyle �E�B���h�E �X�^�C��
 * @param[in] x ��ʂ܂��͐e�E�B���h�E�̍��[����E�B���h�E�̏����ʒu�܂ł̐��������̋���
 * @param[in] y ��ʂ܂��͐e�E�B���h�E�̏�[����E�B���h�E�̏����ʒu�܂ł̐��������̋���
 * @param[in] nWidth �E�B���h�E�̕� (�s�N�Z���P��)
 * @param[in] nHeight �E�B���h�E�̍��� (�s�N�Z���P��)
 * @param[in] hwndParent �e�E�B���h�E�ւ̃n���h��
 * @param[in] nIDorHMenu �E�B���h�E ID
 * @param[in] lpParam ���[�U�[ �f�[�^
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CWndProc::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu, LPVOID lpParam)
{
	// �����X���b�h�̂݋���
	if (sm_dwThreadId != ::GetCurrentThreadId())
	{
		PostNcDestroy();
		return FALSE;
	}

	CREATESTRUCT cs;
	cs.dwExStyle = dwExStyle;
	cs.lpszClass = lpszClassName;
	cs.lpszName = lpszWindowName;
	cs.style = dwStyle;
	cs.x = x;
	cs.y = y;
	cs.cx = nWidth;
	cs.cy = nHeight;
	cs.hwndParent = hwndParent;
	cs.hMenu = nIDorHMenu;
	cs.hInstance = sm_hInstance;
	cs.lpCreateParams = lpParam;

	if (!PreCreateWindow(cs))
	{
		PostNcDestroy();
		return FALSE;
	}

	HookWindowCreate(this);
	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);
	if (!UnhookWindowCreate())
	{
		PostNcDestroy();
	}

	return (hWnd != NULL) ? TRUE : FALSE;
}

/**
 * CWnd �I�u�W�F�N�g�Ɍ��ѕt����ꂽ Windows �̃E�B���h�E���쐬�����O�ɁA�t���[�����[�N����Ăяo����܂�
 * @param[in,out] cs CREATESTRUCT �̍\��
 * @retval TRUE �p��
 */
BOOL CWndProc::PreCreateWindow(CREATESTRUCT& cs)
{
	return TRUE;
}

/**
 * �E�B���h�E�쐬���t�b�N
 * @param[in] pWnd �E�B���h�E
 */
void CWndProc::HookWindowCreate(CWndProc* pWnd)
{
	// �����X���b�h�̂݋���
	ASSERT(sm_dwThreadId == ::GetCurrentThreadId());

	if (sm_pWndInit == pWnd)
	{
		return;
	}

	ASSERT(sm_hHookOldCbtFilter != NULL);
	ASSERT(pWnd != NULL);
	ASSERT(pWnd->m_hWnd == NULL);
	ASSERT(sm_pWndInit == NULL);
	sm_pWndInit = pWnd;
}

/**
 * �E�B���h�E�쐬���A���t�b�N
 * @retval true �t�b�N����
 * @retval false �t�b�N���Ȃ�����
 */
bool CWndProc::UnhookWindowCreate()
{
	if (sm_pWndInit != NULL)
	{
		sm_pWndInit = NULL;
		return false;
	}
	return true;
}

/**
 * �t�b�N �t�B���^
 * @param[in] nCode �R�[�h
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @return ���U���g �R�[�h
 */
LRESULT CALLBACK CWndProc::CbtFilterHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_CREATEWND)
	{
		HWND hWnd = reinterpret_cast<HWND>(wParam);
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCBT_CREATEWND>(lParam)->lpcs;

		CWndProc* pWndInit = sm_pWndInit;
		if (pWndInit != NULL)
		{
			pWndInit->Attach(hWnd);
			pWndInit->PreSubclassWindow();

			WNDPROC newWndProc = &CWndProc::WndProc;
			WNDPROC oldWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(newWndProc)));
			if (oldWndProc != newWndProc)
			{
				pWndInit->m_pfnSuper = oldWndProc;
			}
			sm_pWndInit = NULL;
		}
	}
	return ::CallNextHookEx(sm_hHookOldCbtFilter, nCode, wParam, lParam);
}

/**
 * CWndProc �I�u�W�F�N�g�Ɋ֘A�t����ꂽ Windows �̃E�B���h�E��j�����܂�
 * @return �E�B���h�E���j�����ꂽ�ꍇ�� 0 �ȊO��Ԃ��܂��B����ȊO�̏ꍇ�� 0 ��Ԃ��܂�
 */
BOOL CWndProc::DestroyWindow()
{
	if (m_hWnd == NULL)
	{
		return FALSE;
	}

	CWndProc* pWnd = FromHandlePermanent(m_hWnd);

	const BOOL bResult = ::DestroyWindow(m_hWnd);

	if (pWnd == NULL)
	{
		Detach();
	}

	return bResult;
}

/**
 * �E�B���h�E �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CALLBACK CWndProc::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWndProc* pWnd = FromHandlePermanent(hWnd);
	if (pWnd == NULL)
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return pWnd->WindowProc(message, wParam, lParam);
	}
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CWndProc::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_COMMAND)
	{
		if (OnCommand(wParam, lParam))
		{
			return 0;
		}
	}
	else if (nMsg == WM_NOTIFY)
	{
		NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);
		if (pNMHDR->hwndFrom != NULL)
		{
			LRESULT lResult = 0;
			if (OnNotify(wParam, lParam, &lResult))
			{
				return lResult;
			}
		}
	}
	else if (nMsg == WM_NCDESTROY)
	{
		OnNcDestroy(wParam, lParam);
		return 0;
	}
	return DefWindowProc(nMsg, wParam, lParam);
}

/**
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
 */
BOOL CWndProc::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

/**
 * �t���[�����[�N�́A�C�x���g���R���g���[���ɔ�������ꍇ��A�R���g���[�����ꕔ�̎�ނ̏���v������R���g���[����e�E�B���h�E�ɒʒm���邽�߂ɁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] wParam ���b�Z�[�W���R���g���[�����炻�̃��b�Z�[�W�𑗐M����R���g���[�������ʂ��܂�
 * @param[in] lParam �ʒm�R�[�h�ƒǉ������܂ޒʒm���b�Z�[�W (NMHDR) �̍\���̂ւ̃|�C���^�[
 * @param[out] pResult ���b�Z�[�W���������ꂽ�Ƃ����ʂ��i�[����R�[�h���� LRESULT �̕ϐ��ւ̃|�C���^�[
 * @retval TRUE ���b�Z�[�W����������
 * @retval FALSE ���b�Z�[�W���������Ȃ�����
 */
BOOL CWndProc::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	return FALSE;
}

/**
 * Windows �̃E�B���h�E���j�������Ƃ��ɔ�N���C�A���g�̈悪�j�������ƁA�Ō�ɌĂяo���ꂽ�����o�[�֐��́A�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 */
void CWndProc::OnNcDestroy(WPARAM wParam, LPARAM lParam)
{
	LONG_PTR pfnWndProc = ::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC);
	DefWindowProc(WM_NCDESTROY, wParam, lParam);
	if (::GetWindowLong(m_hWnd, GWLP_WNDPROC) == pfnWndProc)
	{
		if (m_pfnSuper != NULL)
		{
			::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_pfnSuper));
			m_pfnSuper = NULL;
		}
	}
	Detach();

	// call special post-cleanup routine
	PostNcDestroy();
}

/**
 * �E�B���h�E���j�����ꂽ��Ɋ���� OnNcDestroy �̃����o�[�֐��ɂ���ČĂяo����܂�
 */
void CWndProc::PostNcDestroy()
{
}

/**
 * ����̃E�B���h�E �v���V�[�W�����Ăяo���܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�ˑ��̒ǉ������w�肵�܂�
 * @param[in] lParam ���b�Z�[�W�ˑ��̒ǉ������w�肵�܂�
 * @return ����ꂽ���b�Z�[�W�Ɉˑ����܂�
 */
LRESULT CWndProc::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pfnSuper != NULL)
	{
		return ::CallWindowProc(m_pfnSuper, m_hWnd, nMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(m_hWnd, nMsg, wParam, lParam);
	}
}
