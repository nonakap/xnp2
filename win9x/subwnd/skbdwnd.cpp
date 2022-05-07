/**
 * @file	skbdwnd.cpp
 * @brief	�\�t�g�E�F�A �L�[�{�[�h �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "resource.h"
#include "skbdwnd.h"
#include "np2.h"
#include "ini.h"
#include "sysmng.h"
#include "dialog/np2class.h"
#include "generic/softkbd.h"

#if defined(SUPPORT_SOFTKBD)

//! �B��̃C���X�^���X�ł�
CSoftKeyboardWnd CSoftKeyboardWnd::sm_instance;

/**
 * @brief �R���t�B�O
 */
struct SoftKeyboardConfig
{
	int		posx;		//!< X
	int		posy;		//!< Y
	UINT8	type;		//!< �E�B���h�E �^�C�v
};

//! �R���t�B�O
static SoftKeyboardConfig s_skbdcfg;

//! �^�C�g��
static const TCHAR s_skbdapp[] = TEXT("Soft Keyboard");

/**
 * �ݒ�
 */
static const PFTBL s_skbdini[] =
{
	PFVAL("WindposX", PFTYPE_SINT32,	&s_skbdcfg.posx),
	PFVAL("WindposY", PFTYPE_SINT32,	&s_skbdcfg.posy),
	PFVAL("windtype", PFTYPE_BOOL,		&s_skbdcfg.type)
};

/**
 * ������
 */
void CSoftKeyboardWnd::Initialize()
{
	softkbd_initialize();
}

/**
 * ���
 */
void CSoftKeyboardWnd::Deinitialize()
{
	softkbd_deinitialize();
}

/**
 * �R���X�g���N�^
 */
CSoftKeyboardWnd::CSoftKeyboardWnd()
	: m_nWidth(0)
	, m_nHeight(0)
{
}

/**
 * �f�X�g���N�^
 */
CSoftKeyboardWnd::~CSoftKeyboardWnd()
{
}

/**
 * �쐬
 */
void CSoftKeyboardWnd::Create()
{
	if (m_hWnd != NULL)
	{
		return;
	}

	if (softkbd_getsize(&m_nWidth, &m_nHeight) != SUCCESS)
	{
		return;
	}

	if (!CSubWndBase::Create(IDS_CAPTION_SOFTKEY, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, s_skbdcfg.posx, s_skbdcfg.posy, m_nWidth, m_nHeight, NULL, NULL))
	{
		return;
	}
	ShowWindow(SW_SHOWNOACTIVATE);
	UpdateWindow();

	if (!m_dd2.Create(m_hWnd, m_nWidth, m_nHeight))
	{
		DestroyWindow();
		return;
	}
	Invalidate();
	SetForegroundWindow(g_hWndMain);
}

/**
 * �A�C�h������
 */
void CSoftKeyboardWnd::OnIdle()
{
	if ((m_hWnd) && (softkbd_process()))
	{
		OnDraw(FALSE);
	}
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CSoftKeyboardWnd::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_CREATE:
			np2class_wmcreate(m_hWnd);
			winloc_setclientsize(m_hWnd, m_nWidth, m_nHeight);
			np2class_windowtype(m_hWnd, (s_skbdcfg.type & 1) + 1);
			break;

		case WM_PAINT:
			OnPaint();
			break;

		case WM_LBUTTONDOWN:
			if ((softkbd_down(LOWORD(lParam), HIWORD(lParam))) && (s_skbdcfg.type & 1))
			{
				return SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0L);
			}
			break;

		case WM_LBUTTONDBLCLK:
			if (softkbd_down(LOWORD(lParam), HIWORD(lParam)))
			{
				s_skbdcfg.type ^= 1;
				SetWndType((s_skbdcfg.type & 1) + 1);
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_LBUTTONUP:
			softkbd_up();
			break;

		case WM_MOVE:
			if (!(GetWindowLong(m_hWnd, GWL_STYLE) & (WS_MAXIMIZE | WS_MINIMIZE)))
			{
				RECT rc;
				GetWindowRect(&rc);
				s_skbdcfg.posx = rc.left;
				s_skbdcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_DESTROY:
			OnDestroy();
			break;

		default:
			return CSubWndBase::WindowProc(nMsg, wParam, lParam);
	}
	return 0L;
}

/**
 * �E�B���h�E�j���̎��ɌĂ΂��
 */
void CSoftKeyboardWnd::OnDestroy()
{
	::np2class_wmdestroy(m_hWnd);
	m_dd2.Release();
}

/**
 * �`��̎��ɌĂ΂��
 */
void CSoftKeyboardWnd::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	OnDraw(TRUE);
	EndPaint(&ps);
}

/**
 * �`��
 * @param[in] redraw �ĕ`��
 */
void CSoftKeyboardWnd::OnDraw(BOOL redraw)
{
	RECT rect;
	GetClientRect(&rect);

	RECT draw;
	draw.left = 0;
	draw.top = 0;
	draw.right = min(m_nWidth, rect.right - rect.left);
	draw.bottom = min(m_nHeight, rect.bottom - rect.top);
	CMNVRAM* vram = m_dd2.Lock();
	if (vram)
	{
		softkbd_paint(vram, skpalcnv, redraw);
		m_dd2.Unlock();
		m_dd2.Blt(NULL, &draw);
	}
}

/**
 * �p���b�g�ϊ��R�[���o�b�N
 * @param[out] dst �o�͐�
 * @param[in] src �p���b�g
 * @param[in] pals �p���b�g��
 * @param[in] bpp �F��
 */
void CSoftKeyboardWnd::skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp)
{
	switch (bpp)
	{
#if defined(SUPPORT_16BPP)
		case 16:
			for (UINT i = 0; i < pals; i++)
			{
				dst[i].pal16 = CSoftKeyboardWnd::GetInstance()->m_dd2.GetPalette16(src[i]);
			}
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
#endif
#if defined(SUPPORT_32BPP)
		case 32:
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
			for (UINT i = 0; i < pals; i++)
			{
				dst[i].pal32.d = src[i].d;
			}
			break;
#endif
	}
}

/**
 * �ݒ�ǂݍ���
 */
void skbdwin_readini()
{
	s_skbdcfg.posx = CW_USEDEFAULT;
	s_skbdcfg.posy = CW_USEDEFAULT;

	TCHAR szPath[MAX_PATH];
	initgetfile(szPath, _countof(szPath));
	ini_read(szPath, s_skbdapp, s_skbdini, _countof(s_skbdini));
}

/**
 * �ݒ菑������
 */
void skbdwin_writeini()
{
	TCHAR szPath[MAX_PATH];
	initgetfile(szPath, _countof(szPath));
	ini_write(szPath, s_skbdapp, s_skbdini, _countof(s_skbdini));
}
#endif
