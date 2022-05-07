/**
 * @file	subwnd.cpp
 * @brief	�T�u �E�B���h�E�̊��N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "subwnd.h"
#include "np2.h"
#include "soundmng.h"
#include "winloc.h"
#include "dialog/np2class.h"
#include "misc\tstring.h"

extern WINLOCEX np2_winlocexallwin(HWND base);

//! �N���X��
static const TCHAR s_szClassName[] = TEXT("NP2-SubWnd");

/**
 * ������
 * @param[in] hInstance �C���X�^���X
 */
void CSubWndBase::Initialize(HINSTANCE hInstance)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.cbWndExtra = NP2GWLP_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	wc.lpszClassName = s_szClassName;
	RegisterClass(&wc);
}

/**
 * �R���X�g���N�^
 */
CSubWndBase::CSubWndBase()
	: m_wlex(NULL)
{
}

/**
 * �f�X�g���N�^
 */
CSubWndBase::~CSubWndBase()
{
}

/**
 * �E�B���h�E�쐬
 * @param[in] nCaptionID �L���v�V���� ID
 * @param[in] dwStyle �X�^�C��
 * @param[in] x X���W
 * @param[in] y Y���W
 * @param[in] nWidth ��
 * @param[in] nHeight ����
 * @param[in] hwndParent �e�E�B���h�E
 * @param[in] nIDorHMenu ID �������� ���j���[
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CSubWndBase::Create(UINT nCaptionID, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	std::tstring rCaption(LoadTString(nCaptionID));
	return CreateEx(0, s_szClassName, rCaption.c_str(), dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * �E�B���h�E�쐬
 * @param[in] lpCaption �L���v�V����
 * @param[in] dwStyle �X�^�C��
 * @param[in] x X���W
 * @param[in] y Y���W
 * @param[in] nWidth ��
 * @param[in] nHeight ����
 * @param[in] hwndParent �e�E�B���h�E
 * @param[in] nIDorHMenu ID �������� ���j���[
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CSubWndBase::Create(LPCTSTR lpCaption, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	return CreateEx(0, s_szClassName, lpCaption, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * �E�B���h�E �^�C�v�̐ݒ�
 * @param[in] nType �^�C�v
 */
void CSubWndBase::SetWndType(UINT8 nType)
{
	WINLOCEX wlex = ::np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, m_hWnd);
	np2class_windowtype(m_hWnd, nType);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CSubWndBase::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
			::SendMessage(g_hWndMain, nMsg, wParam, lParam);
			break;

		case WM_ENTERMENULOOP:
			CSoundMng::GetInstance()->Disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			CSoundMng::GetInstance()->Enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			CSoundMng::GetInstance()->Disable(SNDPROC_SUBWIND);
			winlocex_destroy(m_wlex);
			m_wlex = np2_winlocexallwin(m_hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(m_wlex, reinterpret_cast<RECT*>(lParam));
			break;

		case WM_EXITSIZEMOVE:
			::winlocex_destroy(m_wlex);
			m_wlex = NULL;
			CSoundMng::GetInstance()->Enable(SNDPROC_SUBWIND);
			break;

		case WM_CLOSE:
			DestroyWindow();
			break;

		default:
			return CWndProc::WindowProc(nMsg, wParam, lParam);
	}
	return 0L;
}
