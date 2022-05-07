/**
 * @file	vsteditwnd.cpp
 * @brief	VST edit �E�B���h�E �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "vsteditwnd.h"
#include "vsteffect.h"

//! �E�B���h�E �N���X��
static const TCHAR s_szClassName[] = TEXT("VstEffectWnd");

//! �C���X�^���X
HINSTANCE CVstEditWnd::sm_hInstance;

//! �E�B���h�E �}�b�v
std::map<HWND, CVstEditWnd*> CVstEditWnd::sm_wndMap;

/**
 * ������
 * @param[in] hInstance �A�v���P�[�V�����̌��݂̃C���X�^���X�̃n���h��
 * @retval true ����
 * @retval false ���s
 */
bool CVstEditWnd::Initialize(HINSTANCE hInstance)
{
	sm_hInstance = hInstance;

	WNDCLASS ws;
	ZeroMemory(&ws, sizeof(ws));
	ws.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	ws.lpfnWndProc = WndProc;
	ws.hInstance = hInstance;
//	ws.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDMUS));
	ws.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	ws.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	ws.lpszClassName = s_szClassName;
	if (!::RegisterClass(&ws))
	{
		return false;
	}
	return true;
}

/**
 * �A�C�h�����O
 */
void CVstEditWnd::OnIdle()
{
	for (std::map<HWND, CVstEditWnd*>::iterator it = sm_wndMap.begin(); it != sm_wndMap.end(); ++it)
	{
		it->second->m_pEffect->idle();
	}
}

/**
 * �R���X�g���N�^
 */
CVstEditWnd::CVstEditWnd()
	: m_hWnd(NULL)
	, m_pEffect(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CVstEditWnd::~CVstEditWnd()
{
}

/**
 * �쐬
 * @param[in] pEffect �G�t�F�N�g
 * @param[in] lpszWindowName �^�C�g��
 * @param[in] dwStyle �X�^�C��
 * @param[in] x �ʒu
 * @param[in] y �ʒu
 * @retval true ����
 * @retval false ���s
 */
bool CVstEditWnd::Create(CVstEffect* pEffect, LPCTSTR lpszWindowName, DWORD dwStyle, LONG x, LONG y)
{
	if (pEffect == NULL)
	{
		return false;
	}
	ERect* lpRect = NULL;
	if ((!pEffect->editGetRect(&lpRect)) || (lpRect == NULL))
	{
		return false;
	}

	LONG nWidth = CW_USEDEFAULT;
	LONG nHeight = CW_USEDEFAULT;

	HWND hWnd = ::CreateWindow(s_szClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, NULL, NULL, sm_hInstance, this);
	if (hWnd == NULL)
	{
		return false;
	}
	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::UpdateWindow(hWnd);

	m_pEffect = pEffect;
	pEffect->Attach(this);
	if (!pEffect->editOpen(hWnd))
	{
		Destroy();
		return false;
	}
	return true;
}

/**
 * �j��
 */
void CVstEditWnd::Destroy()
{
	if (m_pEffect)
	{
		m_pEffect->editClose();
		m_pEffect->Attach();
		m_pEffect = NULL;
	}
	if (m_hWnd)
	{
		::DestroyWindow(m_hWnd);
	}
}

/**
 * �E�B���h�E �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CALLBACK CVstEditWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CVstEditWnd* pWnd = NULL;
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pCreate = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pWnd = static_cast<CVstEditWnd*>(pCreate->lpCreateParams);
		pWnd->m_hWnd = hWnd;
		sm_wndMap[hWnd] = pWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
	}
	else
	{
		pWnd = reinterpret_cast<CVstEditWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pWnd == NULL)
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	LRESULT lResult = pWnd->WindowProc(message, wParam, lParam);

	if (message == WM_NCDESTROY)
	{
		sm_wndMap.erase(pWnd->m_hWnd);
		pWnd->m_hWnd = NULL;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
	}

	return lResult;
}

/**
 * CMainWnd �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CVstEditWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
			return OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
			break;

		default:
			break;
	}
	return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}

/**
 * �t���[�����[�N�́AWindows �̃E�B���h�E�� [�쐬] �܂��� CreateEx �̃����o�[�֐����Ăяo�����Ƃɂ���č쐬���ꂽ�A�v���P�[�V�������K�v�Ƃ���ƁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] lpCreateStruct CREATESTRUCT
 * @retval 0 �p��
 * @retval -1 �j��
 */
int CVstEditWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HMENU hMenu = ::GetSystemMenu(m_hWnd, FALSE);
	if (hMenu)
	{
		::RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
	}
	return 0;
}

/**
 * ���T�C�Y
 * @param[in] nWidth ��
 * @param[in] nHeight ����
 * @retval true ����
 * @retval false ���s
 */
bool CVstEditWnd::OnResize(int nWidth, int nHeight)
{
	if (m_hWnd == NULL)
	{
		return false;
	}
	for (UINT i = 0; i < 2; i++)
	{
		RECT rectWindow;
		::GetWindowRect(m_hWnd, &rectWindow);

		RECT rectClient;
		::GetClientRect(m_hWnd, &rectClient);

		const LONG x = rectWindow.left;
		const LONG y = rectWindow.top;
		const LONG w = nWidth + (rectWindow.right - rectWindow.left) - (rectClient.right - rectClient.left);
		const LONG h = nHeight + (rectWindow.bottom - rectWindow.top) - (rectClient.bottom - rectClient.top);
		::MoveWindow(m_hWnd, x, y, w, h, TRUE);
	}
	return true;
}

/**
 * �ĕ`��v��
 * @retval true ����
 * @retval false ���s
 */
bool CVstEditWnd::OnUpdateDisplay()
{
	return (m_hWnd != NULL);
}
